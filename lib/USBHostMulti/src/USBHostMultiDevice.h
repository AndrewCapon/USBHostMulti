#pragma once

#include <string>
#include <unordered_map>
#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiInterface.h"

class USBHostMulti;

class USBHostMultiDevice
{
public:
  USBHostMultiDevice(USBHostMulti *pParent)
  : m_pParent(pParent)
  {
    for(uint_fast8_t u = 0; u < MAX_INTF; u++)
      m_interfaces[u] = nullptr;
  }

  virtual ~USBHostMultiDevice(void)
  {
    Disconnect();
  }

  USBHostMultiInterface *GetInterface(uint8_t uInterfaceNum) const;
  USBHostMultiInterface *GetInterface(USBHostMultiInterface::InterfaceType interfaceType) const;

  void AddInterface(uint8_t uInterfaceNum, USBHostMultiInterface::InterfaceType interfaceType);

  bool ContainsInterfaceType(USBHostMultiInterface::InterfaceType type) const;

  bool AttachUSBDevice(USBDeviceConnected *pUSBDevice, uint8_t uDeviceId);

  void SetVidPid(uint16_t uVid, uint16_t uPid);

  void InterfaceDisconnected(USBHostMultiInterface *pInterface);

  void Disconnect(void);

  void RecievedUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength);

  bool SendUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking = false);
  bool GetUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking = false);
  
  bool SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking = false)
  {
    return SendUSBData(m_uDefaultInterface, pData, uLength, bBlocking);
  }

  bool GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking = false)
  {
    return GetUSBData(m_uDefaultInterface, pData, uLength, bBlocking);
  }
  
  bool SendControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex);
  bool GetControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex);
  
  uint8_t GetDeviceId(void) const
  {
    return m_uDeviceId;
  }

  std::string GetManufacturer(void) const
  {
    return m_sManufacturer;
  } 

  std::string GetProduct(void) const
  {
    return m_sProduct;
  } 

  std::string GetSerial(void) const
  {
    return m_sSerial;
  } 

  uint16_t GetVid(void) const
  {
    return m_uVid;
  }

  uint16_t GetPid(void) const
  {
    return m_uPid;
  }

  uint16_t GetDefaultInterface(void)
  {
    return m_uDefaultInterface;
  }
  
private:
  std::string         GetStringDescription(uint8_t uIndex);

  USBHostMultiInterface   *m_interfaces[MAX_INTF];

  USBDeviceConnected      *m_pUSBDevice = nullptr;

  uint16_t m_uVid;
  uint16_t m_uPid;

  uint16_t m_uDefaultInterface = 0xffff;
  std::string m_sManufacturer;
  std::string m_sProduct;
  std::string m_sSerial;

  uint8_t m_uDeviceId;

  USBHostMulti *m_pParent = nullptr;

};
