#include "adc.h"
#include "kelvin.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "calibration.h"

static volatile bool data_ready = false;
static volatile uint16_t channels[2] = { 0 };

static uint16_t temp_int = 0;
static uint16_t temp_ext = 0;

static inline uint16_t
convert_temp_ext(uint16_t adc) {
    return (int16_t)((int32_t)((int16_t)EXT_CAL_COUNTS - adc) * EXT_CAL_FAC / 4096) + EXT_CAL_TEMP;
}

static inline uint16_t
convert_temp_int(uint16_t adc) {
    return ((int16_t)adc - INT_CAL_COUNTS) * INT_CAL_FAC / 64 + INT_CAL_TEMP;
}

static const uint8_t admuxx[2] = {
    0x03, // external sensor
    0x8f, // internal sensor
};

static inline void
adc_start_conversion(const uint8_t channel) {
    ADMUX = 0x8f & admuxx[channel];
    ADCSRA = 0xdf; // (re)start ADC with f/128 speed
}

void
adc_start(void) {
    adc_start_conversion(0);
}

bool
adc_fetch(void) {
    if (! data_ready)
        return false;
    uint16_t chn[2];
    for (uint8_t c = 0; c < 2; ++c)
        chn[c] = channels[c];
    data_ready = false;

    temp_ext = convert_temp_ext(chn[0]);
    temp_int = convert_temp_int(chn[1]);

    return true;
}

uint16_t adc_temp_ext(void) {
    return temp_ext;
}

uint16_t adc_temp_int(void) {
    return temp_int;
}

ISR(ADC_vect) {
    static uint8_t counter = 0;
    static uint8_t ch = 0;
    static uint16_t acc[2] = { 0 };

    acc[ch] += ADC;
    ch ^= 1;
    adc_start_conversion(ch);
    sei();
    if (! ch) {
        counter = (counter + 1) & 0xf;
        if (! counter && ! data_ready) {
            for (uint8_t c = 0; c < 2; ++c) {
                channels[c] = acc[c];
                acc[c] = 0;
            }
            data_ready = true;
        }
    }
}
