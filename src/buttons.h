#ifndef BUTTONS_H
#define BUTTONS_H

#include "global.h"

#include <stdbool.h>
#include <stdint.h>

void buttons_init();
bool is_button_pressed(uint8_t button);
void set_button_led(uint8_t button, bool active);

#endif // BUTTONS_H