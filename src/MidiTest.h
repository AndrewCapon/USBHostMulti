#pragma once
#include <set>

#include "USBHostMultiDevice.h"
#include "USBHostMultiMidiDriver.h"

// Simple test class for multiple device testing
// Logs raw data with the device id and the jack
// SimpleTest() just sends out CC 0 to all connected midi devices
class MidiTest
{
public:
  MidiTest(USBHostMultiMidiDriver *pHostMultiMidiDriver)
  : m_pHostMultiMidiDriver(pHostMultiMidiDriver)
  {
    m_pHostMultiMidiDriver->AddDeviceConnectedEventHandler(DeviceConnectedHandler);
    m_pHostMultiMidiDriver->AddDeviceDisconnectedEventHandler(DeviceDisconnectedHandler);
    m_pHostMultiMidiDriver->AddRawMidiEventHandler(RawMidiHandler);
  }

  USBHostMultiMidiDriver::RawMidiEventHandler RawMidiHandler = [this](USBHostMultiDevice *pDevice, uint8_t uJackId, uint8_t uB1, uint8_t uB2, uint8_t uB3, uint8_t uB4)
  {
    printf("MIDI IN [%u][%u] = %x, %x, %x, %x\r\n", pDevice->GetDeviceId(), uJackId, uB1, uB2, uB3, uB4);
  };

  USBHostMultiMidiDriver::DeviceConnectedEventHandler DeviceConnectedHandler = [this](USBHostMultiDevice *pDevice)
  {
    printf("Device Connected [%u] '%s', '%s', '%s'\r\n", pDevice->GetDeviceId(), pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str(), pDevice->GetSerial().c_str());
    m_connectedMidiDevices.insert(pDevice);
  };

  USBHostMultiMidiDriver::DeviceDisconnectedEventHandler DeviceDisconnectedHandler = [this](USBHostMultiDevice *pDevice)
  {
    printf("Device Disconnected [%u] '%s', '%s', '%s'\r\n", pDevice->GetDeviceId(), pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str(), pDevice->GetSerial().c_str());
    m_connectedMidiDevices.erase(pDevice);
  };

  void SimpleTest(void)
  {
    if(m_connectedMidiDevices.size())
    {
      if(m_uVal < 127)
        m_uVal += 1;
      else
        m_uVal = 0;

      for (USBHostMultiDevice *pDevice : m_connectedMidiDevices)
        m_pHostMultiMidiDriver->SendRawMidi(pDevice, 0, 0x0b, 0xb0, 0x00, m_uVal);
    }
  }
private:
  std::set<USBHostMultiDevice *> m_connectedMidiDevices;
  
  USBHostMultiDevice      *m_pUseDevice = nullptr;
  USBHostMultiMidiDriver  *m_pHostMultiMidiDriver = nullptr;

  uint8_t m_uVal = 0;
};
