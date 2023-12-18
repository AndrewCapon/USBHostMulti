#include "USBHostMultiHIDKeyboardDriver.h"
#include "USBHostMultiInterface.h"
#include "USBHostMulti.h"

USBHostMultiHIDKeyboardDriver::USBHostMultiHIDKeyboardDriver(USBHostMulti *pHostMulti)
: m_pHostMulti(pHostMulti)
{
  m_pHostMulti->AddDriver(this);
}

USBHostMultiHIDKeyboardDriver::~USBHostMultiHIDKeyboardDriver(void)
{

}

std::string USBHostMultiHIDKeyboardDriver::GetDriverName(void)
{
  return "HID Keyboard Driver example";
}

void USBHostMultiHIDKeyboardDriver::RecievedUSBData(uint8_t uDeviceIndex,  uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  if(uLength > 0)
  {
    printf("Keyboard data received : ");
    for(uint_fast8_t u = 0; u < uLength; u++)
      printf("0x%.2x%s", pData[u], u==static_cast<uint_fast8_t>(uLength-1) ? "\r\n" : ", ");
  }
}

bool USBHostMultiHIDKeyboardDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  return INTERRUPT_ENDPOINT == endpointType;
}

bool USBHostMultiHIDKeyboardDriver::IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
  return((uClass == HID_CLASS) && (uSubclass == 0x01) && (uProtocol == 0x01));
}


