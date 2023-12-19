#pragma once
#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti;

class USBHostMultiHIDKeyboardDriver : public IUSBHostMultiDriver 
{
public:

  USBHostMultiHIDKeyboardDriver(USBHostMulti *pHostMulti);
  virtual ~USBHostMultiHIDKeyboardDriver(void);

  // IUSBHostMultiDriver
  virtual std::string                           GetDriverName(void) override;
  virtual void                                  RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) override;
  virtual bool                                  IsEndpointSupported(ENDPOINT_TYPE endpointType) override;
  virtual bool                                  IsInterfaceSupported(uint8_t uDeviceIndex, uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) override;

private:
  USBHostMulti                      *m_pHostMulti = nullptr;
};

