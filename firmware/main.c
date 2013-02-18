// ======================================================================
// avr-amiga-controller - emulates amiga kb&mouse in hardware
//
// Copyright (C) 2013 Alistair Buxton <a.j.buxton@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ======================================================================

#include <util/delay.h>

#include "main.h"
#include "../keytable.h"

volatile uint8_t got_sync;

inline static void kclock(void) {
    _delay_us(20);
    PORTD &= 0xfe;
    _delay_us(20);
    PORTD |= 0x01;
    _delay_us(20);
}

inline static void kdat(uint8_t b) {
    if(b)
        PORTD &= 0xfd;
    else
        PORTD |= 0x02;
    kclock();
}


ISR(INT1_vect)
{
    got_sync = 1;
    EIMSK &= ~_BV(1); // disable INT1
} 

uint8_t sync(void) {
    EIMSK &= ~_BV(1); // disable INT1
    PORTD &= ~_BV(1);
    DDRD &= ~_BV(1);
    _delay_us(20);
    EIFR = 2; // clear INT1 interrupt
    got_sync = 0;
    EIMSK |= 2; // enable INT1
    _delay_us(170);
    if(got_sync) return true;

    while(1)
    {
        EIMSK &= ~_BV(1); // disable INT1
        PORTD &= ~_BV(1);
        DDRD |= _BV(1);
        kclock();
        DDRD &= ~_BV(1);
        PORTD |= _BV(1);
        _delay_us(20);
        EIFR = 2;
        got_sync = 0;
        EIMSK |= _BV(1); // enable INT1
        _delay_us(120);
        if(got_sync) return false;
    }
}


volatile uint8_t xc;
volatile uint8_t yc;

volatile int16_t dx;
volatile int16_t dy;

void mouse(void)
{
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
    DDRB = (DDRB & 0x0f) | (tmp << 4);
    _delay_us(100);
}

void keyboard(uint8_t k)
{
    if(k == 0xff) return;
    PORTD |= _BV(1);
    DDRD |= _BV(1);
    kdat((k>>6)&1);
    kdat((k>>5)&1);
    kdat((k>>4)&1);
    kdat((k>>3)&1);
    kdat((k>>2)&1);
    kdat((k>>1)&1);
    kdat((k>>0)&1);
    kdat((k>>7)&1);
    if(sync()) PORTD^=0x40;
    else PORTD^=0x20;
}



void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    DDRC  &= 0x0f;
    PORTC &= 0x0f;
    DDRB  &= 0x0f;
    PORTB &= 0x0f;
// PD0 = clock
// PD1 = data
    DDRD  |= 0x01;
    PORTD |= 0x01;

    EIMSK &= ~_BV(1); // disable INT1
    EICRA &= 0xf3; // INT1 interrupt on low level
    EIFR = 2; // clear INT1 interrupt

    sei();
    sync();
    keyboard(0xfd);
    keyboard(0xfe);
    keyboard(0x35);
    cli();

    LEDs_Init();
    USB_Init();

    /* Hardware Initialization */
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

    static uint8_t hmm = 0;

    switch (USB_ControlRequest.bmRequestType) {
    case 0x40:
        
        switch (USB_ControlRequest.bRequest) {
        case REQ_MOUSE_REL:
            dx += (int16_t)USB_ControlRequest.wValue;
            dy += (int16_t)USB_ControlRequest.wIndex;
            while(dx != 0 || dy != 0) mouse();
            Endpoint_ClearSETUP();
            Endpoint_ClearIN();
            break;
        case REQ_MOUSE_BTN:
            DDRC = (DDRC & 0x0f) | (USB_ControlRequest.wValue << 4);
            Endpoint_ClearSETUP();
            Endpoint_ClearIN();
            break;
        case REQ_KEYBOARD:
            if(USB_ControlRequest.wValue == 115 && USB_ControlRequest.wIndex) {
                PORTD &= 0xfe;
                _delay_ms(500);
                PORTD |= 0x01;
            }
            if(USB_ControlRequest.wValue < 0xa0) {
                keyboard(keycodes[USB_ControlRequest.wValue] | (USB_ControlRequest.wIndex ? 0x00 : 0x80));
            }
            Endpoint_ClearSETUP();
            Endpoint_ClearIN();
            break;
        }
        break;
    }
}
