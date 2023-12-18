#include "USBHostMultiDumperDriver.h"
#include "USBHostMultiInterface.h"
#include "USBHostMulti.h"


USBHostMultiDumperDriver::USBHostMultiDumperDriver(USBHostMulti *pHostMulti)
: m_pHostMulti(pHostMulti)
{
  m_pHostMulti->AddDriver(this);
}

USBHostMultiDumperDriver::~USBHostMultiDumperDriver(void)
{

}

std::string USBHostMultiDumperDriver::GetDriverName(void)
{
  return "USB Dumper Driver example";
}

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

bool USBHostMultiDumperDriver::IsEndpointSupported(ENDPOINT_TYPE endpointType)
{
  // we support BULK and INTERRUPT
  return (INTERRUPT_ENDPOINT == endpointType) || (BULK_ENDPOINT == endpointType);
}

bool USBHostMultiDumperDriver::IsInterfaceSupported(uint8_t uClass, uint8_t uSubclass, uint8_t uProtocol)
{
  // we support all interfaces
  return true;
}

void USBHostMultiDumperDriver::DeviceConnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  printf("USB Device connected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());

}

void USBHostMultiDumperDriver::DeviceDisconnected(uint8_t uDeviceIndex)
{
  USBHostMultiDevice *pDevice = m_pHostMulti->GetDevice(uDeviceIndex);
  printf("USB Device disconnected : %s %s\r\n", pDevice->GetManufacturer().c_str(), pDevice->GetProduct().c_str());
}
 
void USBHostMultiDumperDriver::ParseConfigEntry(uint8_t uType, uint8_t *pData, uint32_t uLength, uint8_t *pRawData)
{
  switch (uType) 
  {
    case 2:  // Configuration
    {
      ConfigurationDescriptor* pconf = (ConfigurationDescriptor*)pRawData;
      printf("Config:\n");
      printf(" wTotalLength: %u\n", pconf->wTotalLength);
      printf(" bNumInterfaces: %u\n", pconf->bNumInterfaces);
      printf(" bConfigurationValue: %u\n", pconf->bConfigurationValue);
      printf(" iConfiguration: %u\n", pconf->iConfiguration);
      printf(" bmAttributes: %u\n", pconf->bmAttributes);
      printf(" bMaxPower: %u\n", pconf->bMaxPower);
    }
    break;

    case 4:  // Interface
    {
      InterfaceDescriptor* pintf = (InterfaceDescriptor*)pRawData;
      printf("****************************************\n");
      printf("** Interface level **\n");
      printf("  bInterfaceNumber: %u\n", pintf->bInterfaceNumber);
      printf("  bAlternateSetting: %u\n", pintf->bAlternateSetting);
      printf("  Number of endpoints: %u\n", pintf->bNumEndpoints);
      printf("  bInterfaceClass: %u\n", pintf->bInterfaceClass);
      printf("  bInterfaceSubClass: %u\n", pintf->bInterfaceSubClass);
      switch (pintf->bInterfaceClass) 
      {
        case 2: printf("    Communications and CDC\n"); break;
        case 3:
          if (pintf->bInterfaceSubClass == 1) printf("    HID (BOOT)\n");
          else printf("    HID\n");
          break;
        case 0xa: printf("    CDC-Data\n"); break;
      }
      printf("  bInterfaceProtocol: %u\n", pintf->bInterfaceProtocol);
      if (pintf->bInterfaceClass == 3) 
      {
        switch (pintf->bInterfaceProtocol) 
        {
          case 0: printf("    None\n"); break;
          case 1: printf("    Keyboard\n"); break;
          case 2: printf("    Mouse\n"); break;
        }
      }
      printf("  iInterface: %u\n", pintf->iInterface);
    }
    break;

    case 5:  // Endpoint
    {
      EndpointDescriptor* pendp = (EndpointDescriptor*)pRawData;
      printf("  Endpoint: ");
      printf("%x", pendp->bEndpointAddress);
      if (pendp->bEndpointAddress & 0x80) 
        printf(" In\n");
      else 
        printf(" Out\n");

      printf("    Attrributes: %u", pendp->bmAttributes);
      switch (pendp->bmAttributes & 0x3) 
      {
        case 0: printf(" Control\n"); break;
        case 1: printf(" Isochronous\n"); break;
        case 2: printf(" Bulk\n"); break;
        case 3: printf(" Interrupt\n"); break;
      }
      printf("    Size: %u\n", pendp->wMaxPacketSize);
      printf("    Interval: %u\n", pendp->bInterval);
    }
    break;

    case 0x24:  // CS_INTERFACE
    {
      printf("  CS_INTERFACE(CDC/ACM): ");
      switch (pData[0]) {
        case 0x00: printf("Header Functional Descriptor.\n"); break;
        case 0x01: printf("Call Management Functional Descriptor.\n"); break;
        case 0x02: printf("Abstract Control Management Functional Descriptor.\n"); break;
        case 0x03: printf("Direct Line Management Functional Descriptor.\n"); break;
        case 0x04: printf("Telephone Ringer Functional Descriptor.\n"); break;
        case 0x05: printf("Telephone Call and Line State Reporting Capabilities Functional Descriptor.\n"); break;
        case 0x06: printf("Union Functional Descriptor\n"); break;
        case 0x07: printf("Country Selection Functional Descriptor\n"); break;
        case 0x08: printf("Telephone Operational Modes Functional Descriptor\n"); break;
        case 0x09: printf("USB Terminal Functional Descriptor\n"); break;
        case 0x0A: printf("Network Channel Terminal Descriptor\n"); break;
        case 0x0B: printf("Protocol Unit Functional Descriptor\n"); break;
        case 0x0C: printf("Extension Unit Functional Descriptor\n"); break;
        case 0x0D: printf("Multi-Channel Management Functional Descriptor\n"); break;
        case 0x0E: printf("CAPI Control Management Functional Descriptor\n"); break;
        case 0x0F: printf("Ethernet Networking Functional Descriptor\n"); break;
        case 0x10: printf("ATM Networking Functional Descriptor\n"); break;
        case 0x11: printf("Wireless Handset Control Model Functional Descriptor\n"); break;
        case 0x12: printf("Mobile Direct Line Model Functional Descriptor\n"); break;
        case 0x13: printf("MDLM Detail Functional Descriptor\n"); break;
        case 0x14: printf("Device Management Model Functional Descriptor\n"); break;
        case 0x15: printf("OBEX Functional Descriptor\n"); break;
        case 0x16: printf("Command Set Functional Descriptor\n"); break;
        case 0x17: printf("Command Set Detail Functional Descriptor\n"); break;
        case 0x18: printf("Telephone Control Model Functional Descriptor\n"); break;
        case 0x19: printf("OBEX Service Identifier Functional Descriptor\n"); break;
        case 0x1A: printf("NCM Functional Descriptor\n"); break;
      }
    }
    break;

    default:
      printf("Unknown: type = %x\n", uType);
    break;
  }
}

