#include "descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header = {.Size = sizeof(USB_Descriptor_Device_t),.Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(01.00),
    .Class = 0,
    .SubClass = 0,
    .Protocol = 0,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID = VENDOR_ID,
    .ProductID = PRODUCT_ID,
    .ReleaseNumber = VERSION_BCD(01.00),

    .ManufacturerStrIndex = 0x01,
    .ProductStrIndex = 0x02,
    .SerialNumStrIndex = 0x03,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config = {
               .Header = {
                   .Size = sizeof(USB_Descriptor_Configuration_Header_t),
                   .Type = DTYPE_Configuration
               },
               .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
               .TotalInterfaces = 1,
               .ConfigurationNumber = 1,
               .ConfigurationStrIndex = NO_DESCRIPTOR,
               .ConfigAttributes = 0x80,
               .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
    },

    .Interface = {
                  .Header = {
                      .Size = sizeof(USB_Descriptor_Interface_t),
                      .Type = DTYPE_Interface
                  },
                  .InterfaceNumber = 0,
                  .AlternateSetting = 0,
                  .TotalEndpoints = 0,
                  .Class = 0,
                  .SubClass = 0,
                  .Protocol = 0,
                  .InterfaceStrIndex = NO_DESCRIPTOR
    },
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
    .Header = {.Size = USB_STRING_LEN(1),.Type = DTYPE_String},
    .UnicodeString = {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
    .Header = {.Size = USB_STRING_LEN(6),.Type = DTYPE_String},
    .UnicodeString = VENDOR_STRING
};

const USB_Descriptor_String_t PROGMEM ProductString = {
    .Header = {.Size = USB_STRING_LEN(7),.Type = DTYPE_String},
    .UnicodeString = PRODUCT_STRING
};

const USB_Descriptor_String_t PROGMEM SerialString = {
    .Header = {.Size = USB_STRING_LEN(6),.Type = DTYPE_String},
    .UnicodeString = SERIAL_STRING
};

uint16_t
CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                           const uint8_t wIndex,
                           const void **const DescriptorAddress)
{
    const uint8_t DescriptorType = (wValue >> 8);
    const uint8_t DescriptorNumber = (wValue & 0xFF);
    const void *Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;

    switch (DescriptorType) {
    case DTYPE_Device:
        Address = &DeviceDescriptor;
        Size = sizeof(USB_Descriptor_Device_t);
        break;
    case DTYPE_Configuration:
        Address = &ConfigurationDescriptor;
        Size = sizeof(USB_Descriptor_Configuration_t);
        break;
    case DTYPE_String:
        switch (DescriptorNumber) {
        case 0x00:
            Address = &LanguageString;
            Size = pgm_read_byte(&LanguageString.Header.Size);
            break;
        case 0x01:
            Address = &ManufacturerString;
            Size = pgm_read_byte(&ManufacturerString.Header.Size);
            break;
        case 0x02:
            Address = &ProductString;
            Size = pgm_read_byte(&ProductString.Header.Size);
            break;
        case 0x03:
            Address = &SerialString;
            Size = pgm_read_byte(&SerialString.Header.Size);
            break;
        }
        break;
    }

    *DescriptorAddress = Address;

    return Size;
}
