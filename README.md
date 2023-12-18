# USBHostMidi

## WIP example code to easily implement USB drivers that are automatically attached when midi devices are attached.

Supports multiple connected USB devices.
Supports USB devices with multiple interfaces.
Currently Control, Bulk and Interupt endpoints are supported.

## For PlatformIO and Arduino users
You need to enable debug output by setting DEBUG to at least 1 in `dbg.h` in `Arduino_USBHostMbed5`.

The following branch of `Arduino_USBHostMbed5` should be used : https://github.com/AndrewCapon/Arduino_USBHostMbed5/tree/USBHostMulti

## For arduino users:
This is a PlatformIO project, if you want to play with it in arduino you need the source code for the main lib here: https://github.com/AndrewCapon/USBHostMulti/tree/main/lib/USBHostMulti/src
You also need `MidiTest.h` from https://github.com/AndrewCapon/USBHostMulti/tree/main/src if you are running the example code with midi devices.
Main.cpp will also need changeing to your main ino file.

## USBHostMultiDumperDriver
By default the sample code uses the `USBHostMultiDumperDriver`, this is very simple, it logs out the info about the configuration when a device is connected.
Also every interface of the device is attached and you should see any data being sent from the device on those interfaces being logged.

## Multiple drivers
In main.cpp changing USE_DUMPER to 0 will enable a test showing multiple drivers being registered.

The drivers are not full drivers, just examples for showing the automatic registering of devices.

The test drivers are for Mass Media, HID Keybaords and MIDI devices.

When connecting devices you should see in the log the correct drivers being attached to the device, if you tap keys on the keyboard you should see data logged, if you send MIDI then you should see that logged.

## Midi output test
Set `USE_MIDI_TEST` to 1 to enable this test
CC0 messages will be sent to all connected midi devices.


## Implementing drivers
A driver needs to subclass from `IUSBHostMultiDriver`

The following are mandatory:
```
virtual std::string     GetDriverName(void) = 0;
virtual bool            IsEndpointSupported(ENDPOINT_TYPE endpointType) = 0;
virtual bool            IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol) = 0;
virtual void            RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength) = 0;
```

The following are optional:
```
virtual void            ParseConfigEntry(uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData) {};
virtual void            DeviceConnected(uint8_t uDeviceIndex) {};
virtual void            DeviceDisconnected(uint8_t uDeviceIndex) {};
virtual bool            HandleTransfersAutomatically(void) { return true; };
```

### std::string GetDriverName(void)
Return a std::string with your Driver name.

For example:
```
std::string USBHostMultiHIDKeyboardDriver::GetDriverName(void)
{
  return "HID Keyboard Driver example";
}
```

### bool IsEndpointSupported(ENDPOINT_TYPE endpointType)
Return true if this sort of endpoint is supported by the driver.

For example, for a HID keyboard we support Interrupt endpoints:
```
bool USBHostMultiHIDKeyboardDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  return INTERRUPT_ENDPOINT == endpointType;
}
```

### bool IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
Return true if this sort of interface is supported:

For example, for a HID keyboard:
```
bool USBHostMultiHIDKeyboardDriver::IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
  return((uClass == HID_CLASS) && (uSubclass == 0x01) && (uProtocol == 0x01));
}
```

### void RecievedUSBData(uint8_t uDeviceIndex, uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
This is called whenever data is received.
`uDeviceIndex` can be used to get the device.
`uInterfaceNum` is the interface number that the data was received from (for multi iinterface devices)
`pData` pointer to `uint8_t` data.
`uLength` the length in bytes.

For example:
```
void USBHostMultiDumperDriver::RecievedUSBData(uint8_t uDeviceIndex,  uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength)
{
  // Dump data to log
  if(uLength > 0)
  {
    printf("Data received from device [%u]:'%s' interface [%u]: ", uDeviceIndex, m_pHostMulti->GetDevice(uDeviceIndex)->GetProduct().c_str(), uInterfaceNum);
    for(uint_fast8_t u = 0; u < uLength; u++)
      printf("0x%.2x%s", pData[u], u==static_cast<uint_fast8_t>(uLength-1) ? "\r\n" : ", ");
  }
}
```

### void ParseConfigEntry(uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData)
