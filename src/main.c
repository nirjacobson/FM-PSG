#include "global.h"

#include <stdbool.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "spi.h"
#include "ym2612.h"
#include "sn76489.h"

int main() {
    spi_init(SPI_2);

    sn76489_init();
    ym2612_init();

    ym2612_test();
    _delay_ms(1000);
    sn76489_test();
}