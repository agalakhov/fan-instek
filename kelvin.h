#pragma once

#include <stdint.h>

#define KELVIN (16 * 273 + 2)

#define kelvin_delta(degrees) (16 * (int16_t)(degrees))

#define kelvin(degrees) (KELVIN + kelvin_delta(degrees))
