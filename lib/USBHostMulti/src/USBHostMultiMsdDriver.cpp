#include "USBHostMultiMsdDriver.h"
#include "USBHostMultiInterface.h"
#include "USBHostMulti.h"

USBHostMultiMsdDriver::USBHostMultiMsdDriver(USBHostMulti *pHostMulti)
: m_pHostMulti(pHostMulti)
{
  m_pHostMulti->AddDriver(this);
}

USBHostMultiMsdDriver::~USBHostMultiMsdDriver(void)
{

}

std::string USBHostMultiMsdDriver::GetDriverName(void)
{
  return "MSD Driver example";
}

void USBHostMultiMsdDriver::RecievedUSBData(uint8_t uDeviceIndex,  uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  if(uLength > 0)
  {
    printf("data received : ");
    for(uint_fast8_t u = 0; u < uLength; u++)
      printf("0x%.2x%s", pData[u], u==static_cast<uint_fast8_t>(uLength-1) ? "\r\n" : ", ");
  }
}

bool USBHostMultiMsdDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  return BULK_ENDPOINT == endpointType;
}

bool USBHostMultiMsdDriver::IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
 return((uClass == MSD_CLASS) && (uSubclass == 0x06) && (uProtocol == 0x50));
}

void USBHostMultiMsdDriver::DeviceConnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  printf("MSD Device connected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());

}

void USBHostMultiMsdDriver::DeviceDisconnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  printf("MSD Device disconnected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());
}
 