#include "USBHostMulti.h"
#include "USBHostMultiInterface.h"

USBHostMulti::USBHostMulti(void)
{
  m_pHost = USBHost::getHostInst();
  m_pHost->setDeviceConnectedCallback(mbed::callback(this, &USBHostMulti::DeviceConnected));
}

USBHostMulti::~USBHostMulti(void)
{

}

void USBHostMulti::DeviceConnected(uint8_t uDeviceIndex)
{
  USBDeviceConnected *pDeviceConnected = m_pHost->getDevice(uDeviceIndex);
  if (pDeviceConnected) 
  {
    if(!pDeviceConnected->isEnumerated())
    {
      for(USBHostMidiDriversIter i = m_drivers.begin(); (i != m_drivers.end()) && (m_connectedDrivers[uDeviceIndex] == nullptr); i++)
      {
        m_pDriverBeingEnumerated = *i;
        std::string sDriverName = m_pDriverBeingEnumerated->GetDriverName();

        USB_DBG("USBHostMulti: Enumerating device index %u with driver %s", uDeviceIndex, sDriverName.c_str());
        if(USB_TYPE_OK == m_pHost->enumerate(pDeviceConnected, this))
        {
          if(AttachUSBDevice(uDeviceIndex, pDeviceConnected))
          {
            USB_DBG("USBHostMulti: Successfully enumerated device index %u with driver %s", uDeviceIndex, sDriverName.c_str());
          }
          else
            USB_DBG("USBHostMulti: Driver %s is not valid for device index %u",  sDriverName.c_str(), uDeviceIndex);
        }
        else
          USB_DBG("USBHostMulti: Failed to enumerate device index %u with driver %s", uDeviceIndex, sDriverName.c_str());
      }
    }
  }
}

void USBHostMulti::RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  if(IUSBHostMultiDriver *pDriver = m_connectedDrivers[uDeviceIndex])
    pDriver->RecievedUSBData(uDeviceIndex, uInterfaceNum, pData, uLength);
}

void USBHostMulti::setEnumeratingDeviceIndex(int index)
{
  m_uCurrentDeviceIndex = index;
}

void USBHostMulti::setVidPid(uint16_t vid, uint16_t pid)
{
  m_devices[m_uCurrentDeviceIndex].SetVidPid(vid, pid);
}

bool USBHostMulti::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol)
{
  bool bResult = m_pDriverBeingEnumerated->IsInterfaceSupported(m_uCurrentDeviceIndex, intf_class, intf_subclass, intf_protocol);
  if(bResult)
  {
    USBHostMultiInterface::InterfaceType interfaceType = USBHostMultiInterface::MakeInterfaceType(intf_class, intf_subclass, intf_protocol);
    AddInterfaceNum(m_uCurrentDeviceIndex, intf_nb, interfaceType);
    USB_DBG("Added interfaceNum %u to Device index %u\r\n", intf_nb, m_uCurrentDeviceIndex);
  }
  return bResult;
}

void USBHostMulti::parseConfigEntry(uint8_t type, uint8_t *data, uint32_t len, uint8_t *rawdata)
{
  m_pDriverBeingEnumerated->ParseConfigEntry(m_uCurrentDeviceIndex, type, data, len, rawdata);
}

bool USBHostMulti::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir)
{
  bool bResult = false;

  if(m_pDriverBeingEnumerated->IsEndpointSupported(type))
  {
    USBHostMultiInterface *pInterface = GetInterface(m_uCurrentDeviceIndex, intf_nb);
    if(pInterface)
    {
      bool bHandleTransfersAutomatically = m_pDriverBeingEnumerated->HandleTransfersAutomatically();
      pInterface->AddEndpoint(type, dir, bHandleTransfersAutomatically);
      bResult = true;
    }
  }

  return bResult;
}

