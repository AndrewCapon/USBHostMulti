#include "USBHostMultiMidiDriver.h"
#include "USBHostMultiInterface.h"
#include "USBHostMulti.h"

USBHostMultiMidiDriver::USBHostMultiMidiDriver(USBHostMulti *pHostMulti)
: m_pHostMulti(pHostMulti)
{
  m_pHostMulti->AddDriver(this);
}

USBHostMultiMidiDriver::~USBHostMultiMidiDriver(void)
{

}

void USBHostMultiMidiDriver::ParseConfigEntry(uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData)
{
  if(uType == 0x24) // class-specific interface descriptor
  {
    // Midi jack
    uint8_t uSubType = *pData++;
    switch (uSubType)
    {
      case 0x01: // Header
        USB_DBG("Midi Header V%u.%u", pData[1], pData[0]);
      break;

      case 0x02: // Midi in jack
      {
        uint8_t uJackType = pData[0];
        uint8_t uJackId   = pData[1];

        USB_DBG("%s Midi in jack [%u]", uJackType == 1 ? "Embedded" : "External", uJackId);

        if(uJackType == 1)
          m_uPortCount++;
      }
      break;

      case 0x03: // Midi out jack
      {
        uint8_t uJackType = pData[0];
        uint8_t uJackId   = pData[1];

        USB_DBG("%s Midi out jack [%u]", uJackType== 1 ? "Embedded" : "External", uJackId);
        uint8_t uNumPins = pData[2];
        for(uint_fast8_t uPin = 0; uPin < uNumPins; uPin++)
        {
          uint8_t *uP = &pData[3];
          uint8_t uSource = *uP++;
          uint8_t uSourcePin = *uP++;
          USB_DBG("  Pin[%u] source = (%u, %u)", uPin, uSource, uSourcePin);
        }
      }
      break;
    }
  }  
}


void USBHostMultiMidiDriver::DeviceConnected(uint8_t uDeviceIndex)
{
  for(DeviceConnectedEventHandler handler : m_deviceConnectedEventHandlers)
    handler(m_pHostMulti->GetDevice(uDeviceIndex));
}

void USBHostMultiMidiDriver::DeviceDisconnected(uint8_t uDeviceIndex)
{
  for(DeviceDisconnectedEventHandler handler : m_deviceDisconnectedEventHandlers)
    handler(m_pHostMulti->GetDevice(uDeviceIndex));
}

std::string USBHostMultiMidiDriver::GetDriverName(void)
{
  return "MIDI Driver";
}

void USBHostMultiMidiDriver::RecievedUSBData(uint8_t uDeviceIndex,  uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  // MIDI event handling

  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);

  // make sure divisable by 4
  uLength -= (uLength % 4);

  for (int i = 0; i < uLength; i += 4) 
  {
    if(pData[i])
    {
      uint8_t uJackId = pData[i] >> 4;
      for(RawMidiEventHandler handler : m_rawMidiEventHandlers)
      {
        handler(pDevice, uJackId, pData[i] & 0x0f, pData[i+1], pData[i+2], pData[i+3]);
      }
    }
  }
}

bool USBHostMultiMidiDriver::SendRawMidi(USBHostMultiDevice *pDevice, uint8_t uJackId, uint8_t uB1, uint8_t uB2, uint8_t uB3, uint8_t uB4)
{
  bool bResult = false;

  if(pDevice)
  {
    uint8_t buffer[4];
    buffer[0] = uB1 + (uJackId << 4);
    buffer[1] = uB2;
    buffer[2] = uB3;
    buffer[3] = uB4;
    
    bResult = pDevice->SendUSBData(1, buffer, 4);
  }

  return bResult;
}


bool USBHostMultiMidiDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  return BULK_ENDPOINT == endpointType;
}

bool USBHostMultiMidiDriver::IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
  return (((uClass == AUDIO_CLASS) && (uSubclass == 0x03)) || ((uClass == 0xff) && (uSubclass == 0x03)));
}
