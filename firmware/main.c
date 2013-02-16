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

    DDRC  &= 0x0f;
    PORTC &= 0x0f;
    DDRD  &= 0xf0;
    PORTD &= 0xf0;
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


volatile uint8_t xc;
volatile uint8_t yc;

volatile int16_t dx;
volatile int16_t dy;

void timer(void) {
    uint8_t tmp;
    if(dx == 0 && dy == 0) {
        // disable timer interrupt
        return;
    }
    if(dx < 0) { dx++; xc--; }
    else if(dx > 0) { dx--; xc++; }
    if(dy < 0) { dy++; yc--; }
    else if(dy > 0) { dy--; yc++; }
    tmp = ((xc>>1)&3) | (((yc>>1)&3)<<2);
    tmp ^= (tmp>>1)&5;
    DDRD = (DDRD & 0xf0) | (tmp & 0x0f);
    _delay_loop_2(468);
}





void EVENT_USB_Device_ControlRequest(void)
{
//    unsigned char buf[0x4] = {0, 1, 2, 3};

    switch (USB_ControlRequest.bmRequestType) {
    case 0x40:
        PORTD ^= 0x60;
        switch (USB_ControlRequest.bRequest) {
        case REQ_MOUSE_REL:
            dx += (int16_t)USB_ControlRequest.wValue;
            dy += (int16_t)USB_ControlRequest.wIndex;
            while(dx != 0 || dy != 0) timer();
            Endpoint_ClearSETUP();
            Endpoint_ClearIN();
            break;
        case REQ_MOUSE_BTN:
            DDRC = (DDRC & 0x0f) | (USB_ControlRequest.wValue << 4);
            Endpoint_ClearSETUP();
            Endpoint_ClearIN();
            break;
        }
        break;
    }
}
