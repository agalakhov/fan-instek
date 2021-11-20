#include "led.h"

#include "io.h"

#include <avr/interrupt.h>

static volatile uint8_t mode = LED_OFF;

void
led_set_mode(const enum LedMode new_mode) {
    mode = new_mode;
}

ISR(TIMER1_OVF_vect) {
    static uint16_t cycle = 0;
    const enum LedMode mo = mode;
    const uint16_t cy = cycle++;
    sei();
    switch (mo) {
        case LED_OFF:
            LED = 0;
            break;
        case LED_CONSTANT_ON:
            LED = 255;
            break;
        case LED_BREATHING:
            LED = (cy & 0x200) ? (255 - ((cy >> 1) & 0xff)) : ((cy >> 1) & 0xff);
            break;
        case LED_SLOW_FLASHING:
            LED = (cy & 0x200) ? 0 : 255;
            break;
        case LED_FAST_FLASHING:
            LED = (cy & 0x80) ? 0 : 255;
            break;
    }
}
