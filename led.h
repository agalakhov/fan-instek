#pragma once

enum LedMode {
    LED_OFF = 0,
    LED_BREATHING,
    LED_SLOW_FLASHING,
    LED_FAST_FLASHING,
    LED_CONSTANT_ON,
};

void led_set_mode(const enum LedMode new_mode);
