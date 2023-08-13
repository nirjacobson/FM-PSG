#include "buttons.h"

void buttons_init() {
    DDRC |= 0x0F;
    PORTC |= 0x0F;
}

bool is_button_pressed(uint8_t button) {
    DIR((0xC0 | button), 0);
    bool result = !IN((0xC0 | button));
    DIR((0xC0 | button), 1);

    return result;
}

void set_button_led(uint8_t button, bool active) {
    DIR((0xC0 | button), 1);
    OUT((0xC0 | button), !active);
}