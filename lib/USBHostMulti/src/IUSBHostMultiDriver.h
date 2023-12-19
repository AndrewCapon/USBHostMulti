#pragma once
#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

class IUSBHostMultiDriver 
{
public:
  // mandatory
  virtual std::string     GetDriverName(void) = 0;
  virtual bool            IsEndpointSupported(ENDPOINT_TYPE endpointType) = 0;
  virtual bool            IsInterfaceSupported(uint8_t uDeviceIndex, uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) = 0;
  virtual void            RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) = 0;

  // optional
  virtual void            ParseConfigEntry(uint8_t uDeviceIndex, uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData) {};
  virtual void            DeviceConnected(uint8_t uDeviceIndex) {};
  virtual void            DeviceDisconnected(uint8_t uDeviceIndex) {};
  virtual bool            HandleTransfersAutomatically(void) { return true; };
};
