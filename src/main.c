#include "global.h"

#include <stdbool.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "spi.h"

#define LEDS_CE 0xD2

int main() {
    DIR(LEDS_CE, 1);
    OUT(LEDS_CE, 1);

    spi_init(SPI_2);

    while(true) {
        spi_transmit(0xAA);
        _delay_ms(500);
        spi_transmit(0x55);
        _delay_ms(500);
    }
}