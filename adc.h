#pragma once

#include <stdbool.h>
#include <stdint.h>

void adc_start(void);
bool adc_fetch(void);
uint16_t adc_temp_ext(void);
uint16_t adc_temp_int(void);
