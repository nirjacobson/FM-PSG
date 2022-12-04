#ifndef SRAM_H
#define SRAM_H

#include "global.h"

#include <avr/io.h>
#include <avr/cpufunc.h>

#include "spi.h"

#define SRAM_WE         0xB0
#define SRAM_CE         0xB1
#define SRAM_DATA_PORT  0xA
#define SRAM_ADDR_PORT  0xC

void sram_init();
uint8_t sram_read(uint32_t address);
void sram_write(uint32_t address, uint8_t value);

#endif // SRAM_H
