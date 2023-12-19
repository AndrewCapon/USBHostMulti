# USBHostMidi

## WIP example code to easily implement USB drivers that are automatically attached when midi devices are attached.

Supports multiple connected USB devices.  
Supports USB devices with multiple interfaces.  
Currently Control, Bulk and Interupt endpoints are supported. (ISO to be added)  

This all started as I wanted to use multiple MIDI devices connected to a HUB and I wanted everything to be automatic.

The idea is that you write drivers that confrom to a simple interface.  
You then register these drivers with the system.  
Then everything is automatic, when a device is attached the system runs through the registered drivers checking to see if a driver matches, if it does it attaches that driver to the device.  
For sending and receiveing the idea was that I did not need to worry about endpoint types, this would be automatic.  
Also I wanted receiving of data to be automatic, so Bulk and Interupt endpoints are automatically set receiving again after data is retrieved.  

TODO:   
Implement ISO endpoints.  
Interrupt receives should be done on SOF with correct interval.  
Implement proper MIDI driver.  
Get other people to implement other devices from existing code.  

## For PlatformIO and Arduino users
You need to enable debug output by setting DEBUG to at least 1 in `dbg.h` in `Arduino_USBHostMbed5`.

The following branch of `Arduino_USBHostMbed5` should be used : https://github.com/AndrewCapon/Arduino_USBHostMbed5/tree/USBHostMulti

define USE_SERIAL to be the serial device you are using

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
This is called for every entry in the device configuration.  
`uType` the type of the config entry.  
`pData` the data for the config entry (type has been removed)  
`uLength` the lengh of the data in `pData`  
`pRawData` the raw data.  

For example in the midi driver we can use this to find the ports and jacks:  
```
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
```

### DeviceConnected(uint8_t uDeviceIndex)
A Device has been connected (only devices supported by this driver)

### DeviceDisconnected(uint8_t uDeviceIndex)
A device has been disconnected (only devices supported by this driver)

### bool HandleTransfersAutomatically(void)
Return false if you do not want transfers handled automatically.
For example a mass media driver.

## USBHostMulti
This is the top level class, this is the `Arduino_USBHostMbed5` driver that is used by the underlying mbed code.  

There are only three methods you need to be interested in:  

### void AddDriver(IUSBHostMultiDriver *pDriver)
Adds a `HostMultiDriver`  
Call this for any multi driver you want to use.

### USBHostMultiDevice *GetDevice(uint8_t uDeviceIndex)
Get a `USBHostMultiDevice` for a device index

### USBHostMultiInterface *GetInterface(uint8_t uDeviceIndex, uint8_t uInterfaceNum)
Get a `USBHostMultiInterface` for a device index and an interface num.

## USBHostMultiDevice
For every USB device attached with a driver we will have one of these.  

There are four methods for data transfer, if automatic transfers are enabled the only one you will probably be interested in is `SendUSBData`  
If you are writing say a mass media driver then you need to turn off automatic transfers by implementing `HandleTransfersAutomatically()` and returning false. Then manually use `GetUSBData()`

### bool SendUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking = false)
Sends data in `pData` of length `uLength` to default interface. Blocking if requred.

### bool GetUSBData(uint8_t *pData, uint16_t uLength, bool bBlocking = false)
Gets data in `pData` of length `uLength` to default interface. Blocking if requred.

### bool SendUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking = false)
Sends data in `pData` of length `uLength` to interface `uInterfaceNum`. Blocking if requred.

### bool GetUSBData(uint8_t uInterfaceNum, uint8_t *pData, uint16_t uLength, bool bBlocking = false)
Gets data in `pData` of length `uLength` to interface `uInterfaceNum`. Blocking if requred.

### bool SendControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex);
Sends control Data.

### bool GetControlData(uint8_t *pData, uint16_t uLength, uint8_t uRequestType, uint8_t uRequest, uint32_t uValue, uint32_t uIndex);
Gets control data.

There are also a couple of methods for getting at interfaces :
### USBHostMultiInterface *GetInterface(uint8_t uInterfaceNum) const;
Get by interface number

### USBHostMultiInterface *GetInterface(USBHostMultiInterface::InterfaceType interfaceType) const;
Get by interface type.

## USBHostMultiInterface
We are down in the gubbins now and you will probably not need to use this stuff.  
Each `USBHostMultiDevice` can have multiple `USBHostMultiInterface`s  
There are Send and Get methods in for the interface level.  

## USBHostMultiInterface
More gubbins.
Each `USBHostMultiInterface` can have multiple `USBHostMultiEndpoint`s  
There are Send and Get methods in for the endpoint level.





