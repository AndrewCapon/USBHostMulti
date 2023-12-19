#pragma once

#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti;


class USBHostMultiJoystickDriver : public IUSBHostMultiDriver 
{
public:
  typedef enum { jtUnknown, jtPs3, jtPs4, jtXboxOne, jtXbox360, jtXbox360W, jtPs3Motion, jtSpaceNav, jtSwitch, jtNes, jtLogiExtreme3DPro} JoystickType;
  typedef enum { jsDisconnected, jsEnumerating, jsConnected, jsErrorInititalising} JoystickState;

  USBHostMultiJoystickDriver(USBHostMulti *pHostMulti);
  virtual ~USBHostMultiJoystickDriver(void);

  // IUSBHostMultiDriver
  std::string     GetDriverName(void) override;
  void            RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) override;
  bool            IsEndpointSupported(ENDPOINT_TYPE endpointType) override;
  bool            IsInterfaceSupported(uint8_t uDeviceIndex, uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) override;
  void            DeviceConnected(uint8_t uDeviceIndex) override;
  void            DeviceDisconnected(uint8_t uDeviceIndex) override;
  void            ParseConfigEntry(uint8_t uDeviceIndex, uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData) override;

  //////////////////////////////////////////////

private:
  USBHostMulti                      *m_pHostMulti = nullptr;

  struct JoystickDevice
  {
    JoystickState m_state = jsDisconnected;
    JoystickType  m_type  = jtUnknown;
  };

  JoystickDevice m_joystickDevices[MAX_DEVICE_CONNECTED];

  //////////////////////////////////////////////
  struct ProductVendorMapping
  {
    uint16_t      uVid;
    uint16_t      uPid;
    JoystickType  joystickType;
    bool          bHidDevice;
	};

  static ProductVendorMapping s_pidVidMapping[];

  USBHostMultiJoystickDriver::JoystickType GetJoystickTypeForDevice(uint8_t uDeviceIndex, bool bExcludeHidDevices = false);
};

