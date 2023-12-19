#include <Arduino.h>

#include <Arduino_USBHostMbed5.h>

#include "USBHostMulti.h"
#include "USBHostMultiMidiDriver.h"
#include "USBHostMultiHIDKeyboardDriver.h"
#include "USBHostMultiMsdDriver.h"
#include "USBHostMultiDumperDriver.h"
#include "USBHostMultiJoystickDriver.h"
#include "MidiTest.h"

// Set to whatever SERIAL device you want for logging.
#define USE_SERIAL Serial2

// Set to 1 for dumper example from rawdata in enumerator
#define USE_DUMPER (0)

// SET to 1 to test midi output, sends varying CC0 to all connected midi devices
#define USE_MIDI_TEST (0)


//Redirect the console (printf) to the USB serial port.
mbed::FileHandle *mbed::mbed_override_console(int fd) 
{
  return USE_SERIAL;
}

USBHostMulti                  *pMultiHost = nullptr;
#if USE_DUMPER
  USBHostMultiDumperDriver      *pMultiDumperDriver = nullptr;
#else
  USBHostMultiMidiDriver        *pMultiMidiDriver = nullptr;
  USBHostMultiHIDKeyboardDriver *pMultiHIDKeyboardDriver = nullptr;
  USBHostMultiMsdDriver         *pMultiMsdDriver = nullptr;
  USBHostMultiJoystickDriver    *pMultiJoystickDriver = nullptr;
  #if USE_MIDI_TEST
    MidiTest                      *pMidiTest = nullptr;
  #endif
#endif

void setup() 
{
  USE_SERIAL.begin(115200);
  printf("Starting USBMultiHost test)\r\n");

  // enable the USB
  pinMode(PA_15, OUTPUT);
  digitalWrite(PA_15, HIGH);

  // Create multi host
  pMultiHost = new USBHostMulti();

#if USE_DUMPER
  pMultiDumperDriver = new USBHostMultiDumperDriver(pMultiHost);
#else
  // Create Midi driver and attach to host (bulk)
  pMultiMidiDriver = new USBHostMultiMidiDriver(pMultiHost);

  // Create test keyboard driver (interrupt)
  pMultiHIDKeyboardDriver = new USBHostMultiHIDKeyboardDriver(pMultiHost);

  // Create Msd driver and attach to host (bulk)
  pMultiMsdDriver = new USBHostMultiMsdDriver(pMultiHost);

  // Create Joystick driver and attach to host (bulk)
  pMultiJoystickDriver = new USBHostMultiJoystickDriver(pMultiHost);

#if USE_MIDI_TEST
  // Create simple test class for testing midi
  pMidiTest = new MidiTest(pMultiMidiDriver);
#endif

#endif
}

void loop() 
{
#if USE_MIDI_TEST  
  // Send midi CC to device for simple test
  pMidiTest->SimpleTest();
#endif

  delay(100);
}
