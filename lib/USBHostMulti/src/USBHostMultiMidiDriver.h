#pragma once
#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti;

class USBHostMultiMidiDriver : public IUSBHostMultiDriver 
{
public:
  typedef std::function<void(USBHostMultiDevice *pDevice, uint8_t uJackId, uint8_t uB1, uint8_t uB2, uint8_t uB3, uint8_t uB4)> RawMidiEventHandler;
  typedef std::function<void(USBHostMultiDevice *pDevice)> DeviceConnectedEventHandler;
  typedef std::function<void(USBHostMultiDevice *pDevice)> DeviceDisconnectedEventHandler;
  
  typedef std::list<RawMidiEventHandler>            RawMidiEventHandlers;
  typedef std::list<DeviceConnectedEventHandler>    DeviceConnectedEventHandlers;
  typedef std::list<DeviceDisconnectedEventHandler> DeviceDisconnectedEventHandlers;

  USBHostMultiMidiDriver(USBHostMulti *pHostMulti);
  virtual ~USBHostMultiMidiDriver(void);

  // IUSBHostMultiDriver
  void        ParseConfigEntry(uint8_t uDeviceIndex, uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData) override;
  void        DeviceConnected(uint8_t uDeviceIndex) override;
  void        DeviceDisconnected(uint8_t uDeviceIndex) override;
  std::string GetDriverName(void) override;
  bool        IsEndpointSupported(ENDPOINT_TYPE endpointType) override;
  bool        IsInterfaceSupported(uint8_t uDeviceIndex, uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) override;
  void        RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) override;


  bool SendRawMidi(USBHostMultiDevice *pDevice, uint8_t uJackId, uint8_t uB1, uint8_t uB2, uint8_t uB3, uint8_t uB4);
  
  // listeners

  void AddRawMidiEventHandler(RawMidiEventHandler handler)
  {
    m_rawMidiEventHandlers.push_back(handler);
  }

  void AddDeviceConnectedEventHandler(DeviceConnectedEventHandler handler)
  {
    m_deviceConnectedEventHandlers.push_back(handler);
  }
   
  void AddDeviceDisconnectedEventHandler(DeviceDisconnectedEventHandler handler)
  {
    m_deviceDisconnectedEventHandlers.push_back(handler);
  }
   
  

private:
  USBHostMulti                      *m_pHostMulti = nullptr;
  uint8_t                           m_uPortCount = 0;
  
  RawMidiEventHandlers              m_rawMidiEventHandlers;
  DeviceConnectedEventHandlers      m_deviceConnectedEventHandlers;
  DeviceDisconnectedEventHandlers   m_deviceDisconnectedEventHandlers;
};
