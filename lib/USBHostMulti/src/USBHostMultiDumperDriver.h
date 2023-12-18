#pragma once

#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti;

class USBHostMultiDumperDriver : public IUSBHostMultiDriver 
{
public:

  USBHostMultiDumperDriver(USBHostMulti *pHostMulti);
  virtual ~USBHostMultiDumperDriver(void);

  // IUSBHostMultiDriver
  std::string     GetDriverName(void) override;
  void            RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) override;
  bool            IsEndpointSupported(ENDPOINT_TYPE endpointType) override;
  bool            IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) override;
  void            DeviceConnected(uint8_t uDeviceIndex) override;
  void            DeviceDisconnected(uint8_t uDeviceIndex) override;
  void            ParseConfigEntry(uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData) override;

private:
  USBHostMulti                      *m_pHostMulti = nullptr;
};

