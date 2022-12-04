#include "sram.h"

void sram_init() {
    PORT_DIR(SRAM_ADDR_PORT) |= 0b111 << 5;

    DIR(SRAM_WE, 1);
    OUT(SRAM_WE, 1);

    DIR(SRAM_CE, 1);
    OUT(SRAM_CE, 1);
}

uint8_t sram_read(uint32_t address) {
    PORT_DIR(SRAM_DATA_PORT) = 0x00;

    spi_transmit((address >> 8) & 0xFF);
    spi_transmit(address & 0xFF);
    PORT_OUT(SRAM_ADDR_PORT) = (PORT_OUT(SRAM_ADDR_PORT) & ~(0b111 << 5)) | (((address >> 16) & 0b111) << 5);

    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    uint8_t value = PORT_IN(SRAM_DATA_PORT);

    OUT(SRAM_CE, 1);

    return value;
}

void sram_write(uint32_t address, uint8_t value) {
    PORT_DIR(SRAM_DATA_PORT) = 0xFF;
    PORT_OUT(SRAM_DATA_PORT) = value;

    spi_transmit((address >> 8) & 0xFF);
    spi_transmit(address & 0xFF);
    PORT_OUT(SRAM_ADDR_PORT) = (PORT_OUT(SRAM_ADDR_PORT) & ~(0b111 << 5)) | (((address >> 16) & 0b111) << 5);

    OUT(SRAM_WE, 0);
    OUT(SRAM_CE, 0);

    _NOP();
    _NOP();

    OUT(SRAM_CE, 1);
    OUT(SRAM_WE, 1);
}
