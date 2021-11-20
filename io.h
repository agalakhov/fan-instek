#pragma once

#define FAN OCR1A
#define LED OCR1B

#include <stdbool.h>

void init(void);

bool fan_rotates(void);
