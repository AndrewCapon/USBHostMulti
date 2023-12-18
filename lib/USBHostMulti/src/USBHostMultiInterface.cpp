#include "USBHostMultiInterface.h"
#include "USBHostMultiDevice.h"

USBHostMultiInterface::USBHostMultiInterface(USBHostMultiDevice *pParentDevice, uint8_t uInterfaceNum, InterfaceType interfaceType)
: m_uInterfaceNum(uInterfaceNum),
  m_interfaceType(interfaceType),
  m_pParentDevice(pParentDevice)
{
  for(uint_fast8_t uType = 0; uType < 4; uType ++)
    for(uint_fast8_t uDirection = 0; uDirection < 2; uDirection++)
      m_endpoints[uType][uDirection] = nullptr;
}

USBHostMultiInterface::~USBHostMultiInterface(void)
{
  for(uint_fast8_t uType = 0; uType < 4; uType ++)
    for(uint_fast8_t uDirection = 0; uDirection < 2; uDirection++)
      delete m_endpoints[uType][uDirection];
}

USBHostMultiEndpoint *USBHostMultiInterface::GetEndpoint(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction)
{
  return m_endpoints[type][direction-1];
}

void USBHostMultiInterface::AddEndpoint(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction, bool bHandleTransfersAutomatically)
{
  if(!GetEndpoint(type, direction))
    m_endpoints[type][direction-1] = new USBHostMultiEndpoint(m_pParentDevice, this, type, direction, bHandleTransfersAutomatically);
}

int16_t USBHostMultiInterface::AttachDevice(USBDeviceConnected *pUSBDevice)
{
  // connect up endpoints
  m_bEndpointsConnected = false;

  for(uint_fast8_t uType = 0; uType < 4; uType ++)
  {
    // OUT endpoint
    if(m_endpoints[uType][0] && m_endpoints[uType][0]->AttachDevice(pUSBDevice, m_uInterfaceNum))
    {
      m_bEndpointsConnected = true;
      m_pOutEndpoint = m_endpoints[uType][0];
    }

    // IN endpoint
    if(m_endpoints[uType][1] && m_endpoints[uType][1]->AttachDevice(pUSBDevice, m_uInterfaceNum))
    {
      m_bEndpointsConnected = true;
      m_pInEndpoint = m_endpoints[uType][1];
    }
  }
              
  if(m_bEndpointsConnected)
    return m_uInterfaceNum;
  else
    return -1;
}

bool USBHostMultiInterface::SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;
 
  if(m_pOutEndpoint)
    bResult = m_pOutEndpoint->SendUSBData(pData, uLength, bBlocking);

  return bResult;
}

bool USBHostMultiInterface::GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;
 
  if(m_pInEndpoint)
    bResult = m_pInEndpoint->GetUSBData(pData, uLength, bBlocking);

  return bResult;
}
