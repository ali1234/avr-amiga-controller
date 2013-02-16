#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include <LUFA/Drivers/USB/USB.h>

#include <avr/pgmspace.h>

#include "../common.h"

#define VENDOR_STRING L"Vendor"
#define PRODUCT_STRING L"Product"
#define SERIAL_STRING L"Serial"

typedef struct {
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t Interface;
} USB_Descriptor_Configuration_t;

/* Function Prototypes: */
uint16_t
CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                           const uint8_t wIndex,
                           const void **const DescriptorAddress)
ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif 
