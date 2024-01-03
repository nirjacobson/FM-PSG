#include "sram.h"

void sram_init() {
    PORT_DIR(SRAM_ADDR_PORT) |= 0b111 << 5;
    PORT_OUT(SRAM_ADDR_PORT) &= ~(0b111 << 5);

    DIR(SRAM_WE, 1);
    OUT(SRAM_WE, 1);

    DIR(SRAM_CE, 1);
    OUT(SRAM_CE, 1);
}

uint8_t sram_read(uint16_t address) {
    PORT_DIR(SRAM_DATA_PORT) = 0x00;

    spi_transmit(((uint8_t*)&address)[1]);
    spi_transmit(((uint8_t*)&address)[0]);

    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    uint8_t value = PORT_IN(SRAM_DATA_PORT);

    OUT(SRAM_CE, 1);

    return value;
}

uint8_t sram_read_ext(uint8_t addressHigh, uint16_t addressLow) {
    PORT_DIR(SRAM_DATA_PORT) = 0x00;

    spi_transmit(((uint8_t*)&addressLow)[1]);
    spi_transmit(((uint8_t*)&addressLow)[0]);
    PORT_OUT(SRAM_ADDR_PORT) |= addressHigh << 5;

    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    uint8_t value = PORT_IN(SRAM_DATA_PORT);

    OUT(SRAM_CE, 1);
    PORT_OUT(SRAM_ADDR_PORT) &= ~(0b111 << 5);

    return value;
}

void sram_write(uint16_t address, uint8_t value) {
    PORT_DIR(SRAM_DATA_PORT) = 0xFF;
    PORT_OUT(SRAM_DATA_PORT) = value;

    spi_transmit(((uint8_t*)&address)[1]);
    spi_transmit(((uint8_t*)&address)[0]);

    OUT(SRAM_WE, 0);
    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    OUT(SRAM_CE, 1);
    OUT(SRAM_WE, 1);
}

void sram_write_ext(uint32_t address, uint8_t value) {
    PORT_DIR(SRAM_DATA_PORT) = 0xFF;
    PORT_OUT(SRAM_DATA_PORT) = value;

    spi_transmit(((uint8_t*)&address)[1]);
    spi_transmit(((uint8_t*)&address)[0]);
    PORT_OUT(SRAM_ADDR_PORT) |= ((uint8_t*)&address)[2] << 5;

    OUT(SRAM_WE, 0);
    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    OUT(SRAM_CE, 1);
    OUT(SRAM_WE, 1);

    PORT_OUT(SRAM_ADDR_PORT) &= ~(0b111 << 5);
}
