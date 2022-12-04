#include "spi.h"

spi_mode current_mode;

void spi_init(spi_mode mode) {
    DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
    spi_set_mode(mode);
}

void spi_set_mode(spi_mode mode) {
    SPCR = (1 << SPE) | (1 << MSTR);

    if (mode == SPI_128) {
        SPCR |= (1 << SPR1) | (1 << SPR0);
    } else if (mode == SPI_64) {
        SPCR |= (1 << SPR1);
    } else if (mode == SPI_32) {
        SPCR |= (1 << SPR1);
        SPSR |= (1 << SPI2X);
    } else if (mode == SPI_16) {
        SPCR |= (1 << SPR0);
    } else if (mode == SPI_8) {
        SPCR |= (1 << SPR0);
        SPSR |= (1 << SPI2X);
    } else if (mode == SPI_4) {
        // SPR0 and SPR1 are 0 and SPI2X is not set
    } else if (mode == SPI_2) {
        SPSR |= (1 << SPI2X);
    }

    current_mode = mode;
}

spi_mode spi_get_mode() {
    return current_mode;
}

uint8_t spi_transmit(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF))) ;

    return SPDR;
}
