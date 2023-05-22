#include "global.h"

#include <stdbool.h>
#include <util/delay.h>

#define LED     0xB2

int main() {
    DIR(LED, 1);

    while (true) {
        OUT(LED, 1);
        _delay_ms(500);
        OUT(LED, 0);
        _delay_ms(500);
    }
}