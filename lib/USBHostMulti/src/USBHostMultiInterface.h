#pragma once
#include <unordered_map>

#include "USBHost/USBHostConf.h"
#include "USBHost/USBHost.h"

#include "USBHostMultiEndpoint.h"

class USBHostMultiDevice;

class USBHostMultiInterface
{
public:
  
  typedef uint32_t InterfaceType;

  static InterfaceType MakeInterfaceType(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
  {
    return (uClass << 16) | (uSubclass << 8) | uProtocol;
  }

  USBHostMultiInterface(USBHostMultiDevice *pParentDevice, uint8_t uInterfaceNum, InterfaceType interfaceType);
  virtual ~USBHostMultiInterface(void);

  USBHostMultiEndpoint *GetEndpoint(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction);
  void AddEndpoint(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction, bool bHandleTransfersAutomatically);

  int16_t AttachDevice(USBDeviceConnected *pUSBDevice);

  bool SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking);
  bool GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking);
  bool SendControlData(uint8_t *pData, uint16_t uLength, bool bBlocking);
  bool GetControlData(uint8_t *pData, uint16_t uLength, bool bBlocking);

  uint8_t GetInterfaceNum(void)
  {
    return m_uInterfaceNum;
  }

  uint8_t GetInterfaceType(void)
  {
    return m_interfaceType;
  }

private:

  uint8_t               m_uInterfaceNum;
  InterfaceType         m_interfaceType;
  USBHostMultiEndpoint  *m_endpoints[4][2];
  USBHostMultiEndpoint  *m_pInEndpoint = nullptr;
  USBHostMultiEndpoint  *m_pOutEndpoint = nullptr;
  bool                  m_bEndpointsConnected;
  USBHostMultiDevice    *m_pParentDevice = nullptr;
};
