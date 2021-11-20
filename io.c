#include "io.h"

#include <avr/io.h>

void
init(void) {
    // Set up pin functions.
    // 1 = PB5 = reset
    // 3 = PB4 = OC1B = LED (active low)
    // 2 = PB3 = ADC3 = ADC of temp. sensor
    // 7 = PB2 = T0/INT0 = rotation feedback
    // 6 = PB1 = OC1A = fan PWM (active high)
    // 5 = PB0 = debug fan o.k.
    PORTB = 0x37;
    DDRB = 0x13;

    // Initialize TIMER1 as a ~976.6 Hz PWM timer.
    OCR1A = 0xff;
    OCR1B = 0xff;
    PLLCSR &= ~0x04; // TIMER1 clock from internal RC
    TCCR1 = 0x66; // PWM A active high, divisor 64
    GTCCR = 0x70; // PWM B active low

    // Initialize TIMER0 as a passive event counter.
    TCCR0A = 0x00; // normal mode, no outputs
    TCCR0B = 0x05; // slow counting mode

    // Initialize ADC.
    DIDR0 = 0x08; // no digital input at #3
    ADCSRB = 0x00; // no special mode

    // Disable analog comparator.
    ACSR = 0x80;

    // Enable timer interrupts.
    TIMSK = 0x06;

    // Enaple INT0 pin interrupts.
    MCUCR = 0x01;
    GIMSK = 0x40;
}
