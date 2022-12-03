#ifndef SPI_H
#define SPI_H

#define   DDR_SPI   DDRB
#define   DD_MOSI   DDB5
#define   DD_SCK    DDB7
#define   DD_SS     DDB4

#include <avr/io.h>
#include <stdint.h>

enum spi_mode {
    SPI_128,
    SPI_64,
    SPI_32,
    SPI_16,
    SPI_8,
    SPI_4,
    SPI_2
};

typedef enum spi_mode spi_mode;

void spi_init(spi_mode mode);
void spi_set_mode(spi_mode mode);
spi_mode spi_get_mode();
uint8_t spi_transmit(uint8_t c);

#endif // SPI_H
