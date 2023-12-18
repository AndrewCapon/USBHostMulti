# USBHostMidi

## WIP example code to easily implement USB drivers that are automatically attached when midi devices are attached.

Supports multiple connected USB devices.
Supports USB devices with multiple interfaces.

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


