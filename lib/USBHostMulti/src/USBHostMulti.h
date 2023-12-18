#pragma once
#include <list>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiDevice.h"
#include "IUSBHostMultiDriver.h"

class USBHostMulti : public IUSBEnumerator 
{
public:
  typedef std::list<IUSBHostMultiDriver *>            USBHostMidiDrivers;
  typedef std::list<IUSBHostMultiDriver *>::iterator  USBHostMidiDriversIter;

  USBHostMulti(void);
  virtual ~USBHostMulti(void);

  void AddDriver(IUSBHostMultiDriver *pDriver)
  {
    m_drivers.push_back(pDriver);
  }

  USBHostMultiDevice *GetDevice(uint8_t uDeviceIndex)
  {
    USBHostMultiDevice *pDevice = nullptr;
    if(uDeviceIndex < MAX_DEVICE_CONNECTED)
      pDevice = &(m_devices[uDeviceIndex]);

    return pDevice;
  }

  USBHostMultiInterface *GetInterface(uint8_t uDeviceIndex, uint8_t uInterfaceNum)
  {
    USBHostMultiInterface *pInterface = nullptr;
    if(uDeviceIndex < MAX_DEVICE_CONNECTED)
      pInterface = m_devices[uDeviceIndex].GetInterface(uInterfaceNum);

    return pInterface;
  }

  void AddInterfaceNum(uint8_t uDeviceIndex, uint8_t uInterfaceNum, USBHostMultiInterface::InterfaceType interfaceType)
  {
    if(uDeviceIndex < MAX_DEVICE_CONNECTED)
      m_devices[uDeviceIndex].AddInterface(uInterfaceNum, interfaceType);
  }
 
  bool DeviceIndexIsContainsInterfaceType(uint8_t uDeviceIndex, USBHostMultiInterface::InterfaceType type)
  {
    bool bResult = false;

    if(uDeviceIndex < MAX_DEVICE_CONNECTED)
      bResult = m_devices[uDeviceIndex].ContainsInterfaceType(type);

    return bResult;
  }

  bool AttachUSBDevice(uint8_t uDeviceIndex, USBDeviceConnected *pDevice)
  {
    bool bResult = false;
    if(uDeviceIndex < MAX_DEVICE_CONNECTED)
    {
      bResult =  m_devices[uDeviceIndex].AttachUSBDevice(pDevice, uDeviceIndex);
      if(bResult)
      {
        m_connectedDrivers[uDeviceIndex] = m_pDriverBeingEnumerated;
        m_connectedDrivers[uDeviceIndex]->DeviceConnected(uDeviceIndex);
      }
    }
    return bResult;
  }

  void DeviceDisconnected(uint8_t uDeviceIndex)
  {
    m_connectedDrivers[uDeviceIndex]->DeviceDisconnected(uDeviceIndex);
    m_connectedDrivers[uDeviceIndex] = nullptr;
  }

  void RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength);
  void DeviceConnected(uint8_t uDeviceIndex);

  // IUSBEnumerator
  void setVidPid(uint16_t vid, uint16_t pid) override;
  bool parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) override;
  bool useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) override;
  void parseConfigEntry(uint8_t type, uint8_t *data, uint32_t len, uint8_t *rawdata) override;
  void setEnumeratingDeviceIndex(int index) override;


private:
  USBHost *m_pHost = nullptr;
  USBHostMultiDevice  m_devices[MAX_DEVICE_CONNECTED] = {USBHostMultiDevice(this), USBHostMultiDevice(this), USBHostMultiDevice(this), USBHostMultiDevice(this), USBHostMultiDevice(this)}; 
  IUSBHostMultiDriver *m_connectedDrivers[MAX_DEVICE_CONNECTED] = {nullptr, nullptr, nullptr, nullptr, nullptr}; 

  USBHostMidiDrivers m_drivers;
  IUSBHostMultiDriver *m_pDriverBeingEnumerated = nullptr;

  uint8_t m_uCurrentDeviceIndex = 0;
};

