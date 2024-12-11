/*
 * A simple API that enhances the usage of MSP430 ports.
 */
#ifndef MSPAPI_H_
#define MSPAPI_H_
#include <msp430.h>

#define RED_LED P1, BIT0
#define GREEN_LED P4, BIT7

#define BUTTON_1 P2, BIT1, PORT2_VECTOR
#define BUTTON_2 P1, BIT1, PORT1_VECTOR

#define PDEVICE(port, mask) (port | (mask << 4))
#define PPORT(dev) (dev & 0x0F)
#define PMASK(dev) ((dev >>> 4) & 0xF)

#define P1 (&PAIN_L)
#define P2 (&PAIN_H)
#define P3 (&PBIN_L)
#define P4 (&PBIN_H)
#define P5 (&PCIN_L)
#define P6 (&PCIN_H)
#define P7 (&PDIN_L)
#define P8 (&PDIN_H)

#define GET(port, type) port[type]

#define PIN  0
#define POUT 2
#define PDIR 4
#define PREN 6
#define PDS  8
#define PSEL 10
#define PIES 24
#define PIE  26
#define PIFG 28

#define set_on(pin, bit) pin |= bit
#define set_off(pin, bit) pin &= ~bit
#define set_bits(pin, mask, val) pin = ((pin & ~mask) | val)

#define _setup_led(port, mask) do {\
    set_on (GET(port, PDIR), mask);\
    set_off(GET(port, PSEL), mask);\
    set_off(GET(port, POUT), mask);\
} while(0)


// ---------- LEDS ---------

#define setup_led(args) _setup_led(args)

#define _led_on(port, mask) set_on(GET(port, POUT), mask)
#define led_on(args) _led_on(args)

#define _led_off(port, mask) set_off(GET(port, POUT), mask)
#define led_off(args) _led_off(args)

#define _led_invert(port, mask) GET(port, POUT) ^= mask
#define led_invert(args) _led_invert(args)


// ---------- BUTTONS ---------

#define PULL_DOWN 0
#define PULL_UP 1

#define RISING_EDGE 0
#define FALLING_EDGE 1

#define INTERRUPT_ENABLED 0
#define INTERRUPT_DISABLED 1

#define _setup_button(port, mask, vector, pull, edge, interr) {\
    set_off(GET(port, PDIR), mask);\
    set_on (GET(port, PREN), mask);\
    set_off(GET(port, PSEL), mask);\
    set_bits(GET(port, POUT), mask, pull == PULL_UP ? mask : 0);\
    set_bits(GET(port, PIES), mask, edge ==  FALLING_EDGE ? mask : 0);\
    set_bits(GET(port, PIE), mask, interr ==  INTERRUPT_ENABLED ? mask : 0);\
    set_off(GET(port, PIFG), mask);\
}

#define setup_button(dev, pull, edge, interr) _setup_button(dev, pull, edge, interr)

#define _reset_interrupt(port, mask, vector) set_off(GET(port, PIFG), mask)
#define reset_interrupt(dev) _reset_interrupt(dev)

#define wait_button_click(dev) _wait_button_click(dev)
#define _wait_button_click(port, mask, vector) do {\
    int c = 0;\
    while (c < 50) {\
        if ((GET(port, PIN) & mask) == 0) {\
            c++;\
        } else {\
            c = 0;\
        }\
    }\
    c = 0;\
    while (c < 50) {\
        if ((GET(port, PIN) & mask) != 0) {\
            c++;\
        } else {\
            c = 0;\
        }\
    }\
} while (0);

#define is_pressed(dev) _is_pressed(dev)
#define _is_pressed(port, mask, vector) ((GET(port, PIN) & mask) == 0)

#define _BTN_VECTOR(port, mask, vector) vector
#define BTN_VECTOR(dev) _BTN_VECTOR(dev)


#endif /* MSPAPI_H_ */
