#include "USBHostMultiJoystickDriver.h"
#include "USBHostMultiInterface.h"
#include "USBHostMulti.h"

USBHostMultiJoystickDriver::USBHostMultiJoystickDriver(USBHostMulti *pHostMulti)
: m_pHostMulti(pHostMulti)
{
  m_pHostMulti->AddDriver(this);
}

USBHostMultiJoystickDriver::~USBHostMultiJoystickDriver(void)
{

}

std::string USBHostMultiJoystickDriver::GetDriverName(void)
{
  return "USB Joystick Driver example";
}

void USBHostMultiJoystickDriver::RecievedUSBData(uint8_t uDeviceIndex,  uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  // Dump data to log
  if(uLength > 0)
  {
    printf("Data received from device [%u]:'%s' interface [%u]: ", uDeviceIndex, m_pHostMulti->GetDevice(uDeviceIndex)->GetProduct().c_str(), uInterfaceNum);
    for(uint_fast8_t u = 0; u < uLength; u++)
      printf("0x%.2x%s", pData[u], u==static_cast<uint_fast8_t>(uLength-1) ? "\r\n" : ", ");
  }
}

bool USBHostMultiJoystickDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  // we support INTERRUPT
  return (INTERRUPT_ENDPOINT == endpointType);
}

bool USBHostMultiJoystickDriver::IsInterfaceSupported(uint8_t uDeviceIndex, uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
  bool bSupported = false;

  JoystickType joystickType = GetJoystickTypeForDevice(uDeviceIndex);

  switch(joystickType)
  {
    case jtXboxOne:
      bSupported = (uClass == 0xff) && (uSubclass == 0x47) && (uProtocol == 0xd0);
    break;

    case jtXbox360W:
      bSupported = (uClass == 0xff) && (uSubclass == 0x5d) && (uProtocol == 0x01);
    break;

    default:
      bSupported = (uClass == HID_CLASS) && (uSubclass == 0x00) && (uProtocol == 0x00);
    break;
  }
  
  if(bSupported)
  {
    m_joystickDevices[uDeviceIndex].m_state = jsEnumerating;
    m_joystickDevices[uDeviceIndex].m_type  = joystickType;
  }

  return bSupported;
}

void USBHostMultiJoystickDriver::DeviceConnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  printf("*** USB Joystick connected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());
  m_joystickDevices[uDeviceIndex].m_state = jsConnected;

  bool bInitialised = false;

  switch(m_joystickDevices[uDeviceIndex].m_type)
  {
    case jtXbox360:
    case jtXbox360W:
    {
      uint8_t command[] = {0x08, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      bInitialised = pDevice->SendUSBData(command, sizeof(command), true);
    }
    break;

    case jtXboxOne:
    {
      uint8_t command[] = {0x05, 0x20, 0x00, 0x01, 0x00};
      bInitialised = pDevice->SendUSBData(command, sizeof(command), true);
    }
    break;

    case jtSwitch:
    {
      uint8_t command[] = {0x80, 0x02};
      bInitialised = pDevice->SendUSBData(command, sizeof(command), true);
    }
    break;

    default:
    break;
  }

  if(bInitialised)
  {
    printf("*** Joystick inititalised\r\n");
    m_joystickDevices[uDeviceIndex].m_state = jsConnected;
  }
  else
  {
    printf("*** ERROR: Joystick failed to inititalise\r\n");
    m_joystickDevices[uDeviceIndex].m_state = jsErrorInititalising;
  }

  printf("\r\n");
}

void USBHostMultiJoystickDriver::DeviceDisconnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  m_joystickDevices[uDeviceIndex].m_state = jsDisconnected;
  printf("USB Joystick disconnected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());
}
 
void USBHostMultiJoystickDriver::ParseConfigEntry(uint8_t uDeviceIndex, uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData)
{
}

///////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////

USBHostMultiJoystickDriver::ProductVendorMapping USBHostMultiJoystickDriver::s_pidVidMapping[] = 
{
  { 0x045e, 0x02ea, jtXboxOne,          false },  // Xbox One S Controller - only tested on giga
  { 0x045e, 0x0b12, jtXboxOne,          false },  // Xbox One S Controller - only tested on giga
  { 0x045e, 0x0719, jtXbox360,          false },
  { 0x045e, 0x02e3, jtXboxOne,          false },  // Xbox 360 Elite
  { 0x045e, 0x028E, jtXbox360W,         false },  // xbox360 wireless receiver
  { 0x057E, 0x2009, jtSwitch,           false },  // Switch Pro controller.  // Let the swtich grab it, but...
  { 0x054C, 0x0268, jtPs3,              true  },
  { 0x054C, 0x042F, jtPs3,              true  },   // PS3 Navigation controller
  { 0x054C, 0x03D5, jtPs3Motion,        true  },   // PS3 Motion controller
  { 0x054C, 0x05C4, jtPs4,              true  },
  { 0x054C, 0x09CC, jtPs4,              true  },
  { 0x0A5C, 0x21E8, jtPs4,              true  },
  { 0x046D, 0xC626, jtSpaceNav,         true  },  // 3d Connextion Space Navigator, 0x10008
  { 0x046D, 0xC628, jtSpaceNav,         true  },  // 3d Connextion Space Navigator, 0x10008
  { 0x046d, 0xc215, jtLogiExtreme3DPro, true  },
  { 0x0079, 0x0011, jtNes,              true  }
};

USBHostMultiJoystickDriver::JoystickType USBHostMultiJoystickDriver::GetJoystickTypeForDevice(uint8_t uDeviceIndex, bool bExcludeHidDevices)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  USBHostMultiJoystickDriver::JoystickType joystickFound = jtUnknown;

  if(pDevice)
  {
    uint8_t uSupportedJoystickCount = sizeof(s_pidVidMapping) / sizeof(ProductVendorMapping);

    for (uint8_t i = 0; (joystickFound == jtUnknown) && (i < uSupportedJoystickCount); i++) 
    {
      ProductVendorMapping &map = s_pidVidMapping[i];
      if ((pDevice->GetVid() == map.uVid) && (pDevice->GetPid() == map.uPid) && !(bExcludeHidDevices && map.bHidDevice)) 
        joystickFound = map.joystickType;
    }
  }

  return joystickFound; 
}
