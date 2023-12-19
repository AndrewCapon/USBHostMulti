#pragma once

#include "USBHost/USBHostConf.h"

#include "USBHost/USBHost.h"

class USBHostMultiDevice;
class USBHostMultiInterface;

class USBHostMultiEndpoint
{
public:
  USBHostMultiEndpoint(USBHostMultiDevice *pDevice, USBHostMultiInterface *pInterface, ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction, bool bHandleTransfersAutomatically);
  virtual ~USBHostMultiEndpoint(void);
  
  static uint16_t GetKey(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction);

  uint16_t GetKey(void);

  bool AttachDevice(USBDeviceConnected *pUSBDevice, uint8_t m_uInterfaceNum);

  void RxHandler();

  bool SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking);
  bool GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking);

private:
  USBHostMultiDevice    *m_pDevice = nullptr;
  USBHostMultiInterface *m_pInterface = nullptr;

  ENDPOINT_TYPE       m_type;
  ENDPOINT_DIRECTION  m_direction;
  USBEndpoint         *m_pUSBEndpoint = nullptr;
  uint32_t            m_uSize;
  uint8_t             m_data[64];
  uint                m_uKey;
  USBDeviceConnected  *m_pUSBDevice = nullptr;
  USBHost             *m_pHost = nullptr;
  bool                m_bHandleTransfersAutomatically;

  inline bool CheckUSBResult(USB_TYPE result)
  {
    return (result == USB_TYPE_OK) || (result = USB_TYPE_PROCESSING);
  }

};
