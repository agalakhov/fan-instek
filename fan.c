#include "fan.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

static volatile uint8_t cycles = 0;

ISR(INT0_vect) {
    TCNT0 = 0;
    cycles = 0;
    PORTB |= 1;
}

ISR(TIMER0_OVF_vect) {
    ++cycles;
    if (cycles >= 8) {
        PORTB &= ~1;
        cycles = 0;
    }
}
