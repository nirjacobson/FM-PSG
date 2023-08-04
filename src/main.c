#include "global.h"

#include <util/delay.h>

#include "buttons.h"

int main() {
    set_button_led(0, 1);
    _delay_ms(500);
    set_button_led(0, 0);
    set_button_led(1, 1);
    _delay_ms(500);
    set_button_led(1, 0);
    set_button_led(2, 1);
    _delay_ms(500);
    set_button_led(2, 0);
    set_button_led(3, 1);
    _delay_ms(500);
    set_button_led(3, 0);

    int button = 0;

    while (true) {
        if (is_button_pressed(0)) {
            set_button_led(button, 0);
            set_button_led((button = 0), 1);
        }
        if (is_button_pressed(1)) {
            set_button_led(button, 0);
            set_button_led((button = 1), 1);
        }
        if (is_button_pressed(2)) {
            set_button_led(button, 0);
            set_button_led((button = 2), 1);
        }
        if (is_button_pressed(3)) {
            set_button_led(button, 0);
            set_button_led((button = 3), 1);
        }
    }
}