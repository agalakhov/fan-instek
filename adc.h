#pragma once

#include <stdbool.h>
#include <stdint.h>

void adc_start(void);
bool adc_fetch(void);
int16_t adc_temp_ext(void);
int16_t adc_temp_int(void);
