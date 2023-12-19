#include "USBHostMultiDevice.h"
#include "USBHostMulti.h"

void USBHostMultiDevice::InterfaceDisconnected(USBHostMultiInterface *pInterface)
{
}

USBHostMultiInterface *USBHostMultiDevice::GetInterface(uint8_t uInterfaceNum) const
{
  USBHostMultiInterface *pInterface = nullptr;
  if(uInterfaceNum < MAX_INTF)
    pInterface = m_interfaces[uInterfaceNum];

  return pInterface;
} 

USBHostMultiInterface *USBHostMultiDevice::GetInterface(USBHostMultiInterface::InterfaceType interfaceType) const
{
  USBHostMultiInterface *pInterface = nullptr;
  
  for(uint_fast8_t u = 0; u < MAX_INTF; u++)
    if(m_interfaces[u]->GetInterfaceType() == interfaceType)
      pInterface = m_interfaces[u];
  
  return pInterface;
} 


void USBHostMultiDevice::AddInterface(uint8_t uInterfaceNum, USBHostMultiInterface::InterfaceType interfaceType)
{
  if(!GetInterface(uInterfaceNum))
  {
    m_interfaces[uInterfaceNum] = new USBHostMultiInterface(this, uInterfaceNum, interfaceType);
  }
}

bool USBHostMultiDevice::ContainsInterfaceType(USBHostMultiInterface::InterfaceType type) const
{
  return nullptr != GetInterface(type);
}

std::string USBHostMultiDevice::GetStringDescription(uint8_t uIndex) 
{
  std::string sResult;
  if(uIndex != 0)
  {
    constexpr uint8_t uBufferSize = 64*2;
    uint8_t buffer[uBufferSize + 1];
    buffer[1] = 0x00;
    USBHost *pUSBHost = USBHost::getHostInst();

    USB_TYPE res = pUSBHost->controlRead(m_pUSBDevice, USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE, GET_DESCRIPTOR, (0x03 << 8) | (uIndex),  0x0409, buffer, uBufferSize);

    if ((res == USB_TYPE_OK) && (buffer[1] == 0x03)) 
    {
      uint8_t uLen = buffer[0];
      uint8_t *pDst = &buffer[2];
      uint8_t *pSrc = &buffer[2];

      if(uLen > 64)
        uLen = 64;
        
      while(uLen--)
      {
        *pDst = *pSrc;
        pDst++;
        pSrc+=2;
      }
      *pDst = 0;

      sResult = std::string(reinterpret_cast<char *>(&buffer[2]));
    }
    else
      USB_WARN("Failed to get Descriptor for string index %u.", uIndex);
  }

  return sResult;
}

bool USBHostMultiDevice::AttachUSBDevice(USBDeviceConnected *pUSBDevice, uint8_t uDeviceId)
{
  m_uDeviceId = uDeviceId;
  m_pUSBDevice = pUSBDevice;
  USBHost *pUSBHost = USBHost::getHostInst();

  uint8_t uConnectedInterfaces = 0;
  for(uint_fast8_t u = 0; u < MAX_INTF; u++)
  {
    USBHostMultiInterface *pInterface = m_interfaces[u];
    if(pInterface)
    {
      USBHost::Lock  Lock(pUSBHost);

      int16_t nConnectedInterface = pInterface->AttachDevice(m_pUSBDevice);
      if(nConnectedInterface != -1)
      {
        if(m_uDefaultInterface == 0xffff)
          m_uDefaultInterface = nConnectedInterface;

        m_pUSBDevice->setName(m_sProduct.c_str(), static_cast<uint8_t>(nConnectedInterface));
        if(uConnectedInterfaces == 0) // just one disconnect
          pUSBHost->registerDriver(m_pUSBDevice, static_cast<uint8_t>(nConnectedInterface), this, &USBHostMultiDevice::Disconnect);
        uConnectedInterfaces++;
      }
    }
  }

  if(uConnectedInterfaces != 0)
  {
    DeviceDescriptor device_descriptor;
    USB_TYPE res = pUSBHost->controlRead(  m_pUSBDevice, USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE, GET_DESCRIPTOR, (DEVICE_DESCRIPTOR << 8) | (0), 0, reinterpret_cast<uint8_t *>(&device_descriptor), DEVICE_DESCRIPTOR_LENGTH);

    if (res == USB_TYPE_OK) 
    {
      m_sManufacturer = GetStringDescription(device_descriptor.iManufacturer);
      m_sProduct      = GetStringDescription(device_descriptor.iProduct);
      m_sSerial       = GetStringDescription(device_descriptor.iSerialNumber);
    }
    else
      USB_WARN("Failed to get Descriptor");

    USB_DBG("AttachUSBDevice(%s, %s, %s)", m_sManufacturer.c_str(), m_sProduct.c_str(), m_sSerial.c_str())
  }

  return (uConnectedInterfaces!=0);
}

void USBHostMultiDevice::Disconnect(void)
{
  m_pUSBDevice = nullptr;
  m_uVid = 0;
  m_uPid = 0;
  m_uDefaultInterface = 0xffff;

  for(uint_fast8_t u = 0; u < MAX_INTF; u++)
  {
    delete(m_interfaces[u]);
    m_interfaces[u] = nullptr;
  } 

  m_pParent->DeviceDisconnected(m_uDeviceId);
}

void USBHostMultiDevice::SetVidPid(uint16_t uVid, uint16_t uPid)
{
  m_uVid = uVid;
  m_uPid = uPid;
}

void USBHostMultiDevice::RecievedUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  m_pParent->RecievedUSBData(m_uDeviceId, uInterfaceNum, pData, uLength);
}

bool USBHostMultiDevice::SendUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;

  if(m_interfaces[uInterfaceNum])
    bResult = m_interfaces[uInterfaceNum]->SendUSBData(pData, uLength, bBlocking);

  return bResult;
}

bool USBHostMultiDevice::GetUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;

  if(m_interfaces[uInterfaceNum])
    bResult = m_interfaces[uInterfaceNum]->GetUSBData(pData, uLength, bBlocking);

  return bResult;
}

bool USBHostMultiDevice::SendControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex)
{
  return USB_TYPE_OK == USBHost::getHostInst()->controlWrite(m_pUSBDevice, uRequestType, uRequest, uValue, uIndex, pData, uLength);
}

bool USBHostMultiDevice::GetControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex)
{
  return USB_TYPE_OK == USBHost::getHostInst()->controlRead(m_pUSBDevice, uRequestType, uRequest, uValue, uIndex, pData, uLength);
}
