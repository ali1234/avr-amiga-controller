// ======================================================================
// libusb host stuff
//
// Copyright (C) 2007 Barry Carter
// Copyright (C) 2008,2013 Alistair Buxton
//
// This is free software, licensed under the terms of the GNU General
// Public License V2 as published by the Free Software Foundation.
// ======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <usb.h>

#include "../common.h"

usb_dev_handle *usbhandle;
static usb_dev_handle *find_device(unsigned short, unsigned short);

int init()
{
    usb_init();
    usbhandle = find_device(VENDOR_ID, PRODUCT_ID);
    if (usbhandle == NULL) {
        fprintf(stderr,
                "Could not find USB device with vid=0x%x pid=0x%x\n",
                VENDOR_ID, PRODUCT_ID);
        return -1;
    }
    return 0;
}

static usb_dev_handle *find_device(unsigned short vid, unsigned short pid)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    usb_dev_handle *handle = 0;

    usb_find_busses();
    usb_find_devices();
    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == vid
                && dev->descriptor.idProduct == pid) {

                handle = usb_open(dev); /* we need to open the device in order to query strings */
                if (!handle) {
                    fprintf(stderr,
                            "Warning: cannot open USB device: %s\n",
                            usb_strerror());
                    continue;
                }
                fprintf(stderr, "Connected\n");
            }
        }
        if (handle)
            break;
    }
    if (!handle)
        fprintf(stderr, "Could not find USB device\n");
    return handle;
}

unsigned char buf[0xffff];

int main(int argc, char **argv)
{
    int retlen;

    init();

    unsigned short wValue = 0x0000;
    unsigned short wIndex = 0x0000;
    unsigned char bRequest = 0x00;

    retlen =
        usb_control_msg(usbhandle, 0x40, bRequest, wValue, wIndex, buf, 0,
                        5000);
    printf("%d\n", retlen);
}
