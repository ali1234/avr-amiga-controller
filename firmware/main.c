#include "main.h"

void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    LEDs_Init();
    USB_Init();

    DDRD |= 0x0f;
}

int main(void)
{
    SetupHardware();
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);

    sei();

    for (;;) {
        USB_USBTask();
    }
}

void EVENT_USB_Device_Connect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

void EVENT_USB_Device_Disconnect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;
    LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

void EVENT_USB_Device_ControlRequest(void)
{
//    unsigned char buf[0x4] = {0, 1, 2, 3};

    switch (USB_ControlRequest.bmRequestType) {
    case 0x40:
//        switch (USB_ControlRequest.bRequest) {
//        case 0x00:
            Endpoint_ClearSETUP();
//            Endpoint_Read_Control_Stream_LE(&buf, 4);
            Endpoint_ClearIN();
            PORTD ^= 0xff;
//            break;
//        }
        break;
    case 0xc0:
//        switch (USB_ControlRequest.bRequest) {
//        case 0x00:
            Endpoint_ClearSETUP();
//            Endpoint_Write_Control_Stream_LE(&buf, 4);
            Endpoint_ClearOUT();
            PORTD ^= 0xff;
//            break;
//        }
        break;
    }
}
