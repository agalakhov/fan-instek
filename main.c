#include "io.h"
#include "led.h"
#include "adc.h"
#include "fan.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stdint.h>

FUSES = {
    .low = 0xE2, // 8 MHz internal oscillator
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT,
};

static const uint8_t FAN_MIN = 16;

static inline int16_t
t(const int8_t deg) {
    return (int16_t) deg * 16;
}

static const int32_t TARGET_TEMP = 40;
static const int32_t MAX_INTEGRAL = 524288;

int
main() {
    wdt_enable(WDTO_120MS);
    init();

    LED = 0;
    FAN = 255;

    adc_start();
    sei();

    int32_t integral = 0;

    led_set_mode(LED_OFF);
    while (true) {
        wdt_reset();
        sleep_mode();
        if (adc_fetch()) {
            // approx. every 7 ms
            const int16_t temp_ext = adc_temp_ext();
            const int16_t temp_int = adc_temp_int();

            if (temp_ext > t(94)) {
                // Too hot or sensor is broken.
                led_set_mode(LED_FAST_FLASHING);
                FAN = 255;
            } else if (temp_ext < t(10)) {
                if (temp_int < temp_ext + t(7)) {
                    // It is really cold. Stop the fan.
                    led_set_mode(LED_SLOW_FLASHING);
                    FAN = 0;
                } else {
                    // Sensor is broken.
                    led_set_mode(LED_FAST_FLASHING);
                    FAN = 255;
                }
            } else {
                int16_t delta = temp_ext - t(TARGET_TEMP);
                integral += delta;
                if (integral < 0)
                    integral = 0;
                if (integral > MAX_INTEGRAL)
                    integral = MAX_INTEGRAL;

                // PI-regulator
                int16_t p = delta / 2; // MIN..MAX = 32 degrees
                int16_t i = integral / 2048; // ~2 sec. time constant
                int16_t v = p + i;

                if (v > 255)
                    v = 255;
                if (v < FAN_MIN)
                    v = 0;

                if (v > 0) {
                    if (PORTB & 1) {
                        // fan ok
                        led_set_mode(LED_BREATHING);
                    } else {
                        led_set_mode(LED_CONSTANT_ON);
                    }
                } else {
                    led_set_mode(LED_SLOW_FLASHING);
                }
                FAN = (uint8_t)v;
            }
        }
    }

    return 0;
}

