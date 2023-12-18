#pragma once
#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti;

class USBHostMultiMsdDriver : public IUSBHostMultiDriver 
{
public:

  USBHostMultiMsdDriver(USBHostMulti *pHostMulti);
  virtual ~USBHostMultiMsdDriver(void);

  // IUSBHostMultiDriver
  virtual std::string     GetDriverName(void) override;
  virtual void            RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) override;
  virtual bool            IsEndpointSupported(ENDPOINT_TYPE endpointType) override;
  virtual bool            IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) override;
  virtual void            DeviceConnected(uint8_t uDeviceIndex) override;
  virtual void            DeviceDisconnected(uint8_t uDeviceIndex) override;
  virtual bool            HandleTransfersAutomatically(void) override { return false; } ;
private:
  USBHostMulti            *m_pHostMulti = nullptr;
};
