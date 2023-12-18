#include "USBHostMultiEndpoint.h"
#include "USBHostMultiInterface.h"
#include "USBHostMultiDevice.h"


USBHostMultiEndpoint::USBHostMultiEndpoint(USBHostMultiDevice *pDevice, USBHostMultiInterface *pInterface, ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction, bool bHandleTransfersAutomatically)
: m_pDevice(pDevice), m_pInterface(pInterface), m_type(type), m_direction(direction), m_bHandleTransfersAutomatically(bHandleTransfersAutomatically)
{
  m_pHost = USBHost::getHostInst();
} 

USBHostMultiEndpoint::~USBHostMultiEndpoint(void)
{
  // seems to be no way of detaching endpoint!
}
  
uint16_t USBHostMultiEndpoint::GetKey(ENDPOINT_TYPE type, ENDPOINT_DIRECTION direction)
{
  return (type << 8) + direction;
}

uint16_t USBHostMultiEndpoint::GetKey(void)
{
  return GetKey(m_type, m_direction);
}

bool USBHostMultiEndpoint::AttachDevice(USBDeviceConnected *pUSBDevice, uint8_t m_uInterfaceNum)
{
  m_pUSBDevice = pUSBDevice;
  m_pUSBEndpoint = pUSBDevice->getEndpoint(m_uInterfaceNum, m_type, m_direction);
  if(m_pUSBEndpoint && (m_direction == IN))
  {
    m_pUSBEndpoint->attach(this, &USBHostMultiEndpoint::RxHandler);
    m_uSize = m_pUSBEndpoint->getSize();
    if(m_bHandleTransfersAutomatically)
    {
      switch(m_type)
      {
        case BULK_ENDPOINT: 
          m_pHost->bulkRead(m_pUSBDevice, m_pUSBEndpoint, m_data, m_uSize, false); 
        break;

        case INTERRUPT_ENDPOINT:
          m_pHost->interruptRead(m_pUSBDevice, m_pUSBEndpoint, m_data, m_uSize, false);
        break;

        default:
          USB_ERR("USBHostMultiEndpoint: Unsupported Endpoint type %u", m_type);
      }
    }
  }
  return nullptr != m_pUSBEndpoint;
}

void USBHostMultiEndpoint::RxHandler() 
{
  if (m_pUSBEndpoint) 
  {
    uint16_t uLength = m_pUSBEndpoint->getLengthTransferred();
    if (m_pUSBEndpoint->getState() == USB_TYPE_IDLE || m_pUSBEndpoint->getState() == USB_TYPE_FREE) 
    {
      if(uLength)
        m_pDevice->RecievedUSBData(m_pInterface->GetInterfaceNum(), m_data, uLength);

      // read another message
      if(m_bHandleTransfersAutomatically)
      {
        switch(m_type)
        {
          case BULK_ENDPOINT: 
            m_pHost->bulkRead(m_pUSBDevice, m_pUSBEndpoint, m_data, m_uSize, false);
            //rtos::ThisThread::sleep_for(1);
          break;

          case INTERRUPT_ENDPOINT:
            m_pHost->interruptRead(m_pUSBDevice, m_pUSBEndpoint, m_data, m_uSize, false);
          break;

          default:
            USB_ERR("USBHostMultiEndpoint: Unsupported Endpoint type %u", m_type);
          break;
        }
      }
    }
  }
}

bool USBHostMultiEndpoint::SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;

  if(m_pHost && m_pUSBDevice)
  {
    switch(m_type)
    {
      case BULK_ENDPOINT:
        bResult = (USB_TYPE_OK == m_pHost->bulkWrite(m_pUSBDevice, m_pUSBEndpoint, pData, uLength, bBlocking));
      break;

      case INTERRUPT_ENDPOINT:
        bResult = (USB_TYPE_OK == m_pHost->interruptWrite(m_pUSBDevice, m_pUSBEndpoint, pData, uLength, bBlocking));
      break;

      default:
        USB_ERR("USBHostMultiEndpoint: Unsupported Endpoint type %u", m_type);
      break;
    }
  }
  
  return bResult;
}

bool USBHostMultiEndpoint::GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking)
{
  bool bResult = false;

  if(m_pUSBEndpoint->getState() != USB_TYPE_IDLE)
  {
    m_pUSBEndpoint->setState(USB_TYPE_ERROR);
    m_pUSBEndpoint->ep_queue.get(0);
    m_pUSBEndpoint->unqueueTransfer(m_pUSBEndpoint->getProcessedTD());
    m_pUSBEndpoint->setState(USB_TYPE_IDLE);
  }

  if(m_pHost && m_pUSBDevice)
  {
    switch(m_type)
    {
      case BULK_ENDPOINT:
        bResult = (USB_TYPE_OK == m_pHost->bulkRead(m_pUSBDevice, m_pUSBEndpoint, pData, uLength, bBlocking));
      break;

      case INTERRUPT_ENDPOINT:
        bResult = (USB_TYPE_OK == m_pHost->interruptRead(m_pUSBDevice, m_pUSBEndpoint, pData, uLength, bBlocking));
      break;

      default:
        USB_ERR("USBHostMultiEndpoint: Unsupported Endpoint type %u", m_type);
      break;
    }
  }
  
  return bResult;
}
