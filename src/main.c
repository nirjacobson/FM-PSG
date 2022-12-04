#define  BAUD      57600

#define BLOCK_SIZE  256

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#include "usart.h"
#include "sram.h"

uint8_t block[BLOCK_SIZE];

int main() {
    usart_init(BAUD);

    usart_send_line("Hello world!");

    spi_init(SPI_2);

    sram_init();

    while (true) {
        char address_str[9];
        uint32_t address;
        usart_send_str("Enter a byte address: 0x");
        usart_receive_str(address_str, true);
        sscanf(address_str, "%lx", &address);

        usart_send_line(NULL);

        memset(block, 0, BLOCK_SIZE);
        usart_send_str("Enter a block of text: ");
        usart_receive_str((char*)block, true);

        for (uint32_t i = 0; i < BLOCK_SIZE; i++) {
            sram_write(address + i, block[i]);
        }

        memset(block, 0, BLOCK_SIZE);

        for (uint32_t i = 0; i < BLOCK_SIZE; i++) {
            block[i] = sram_read(address + i);
        }

        usart_send_line(NULL);

        usart_send_str("This text was written: ");
        usart_send_str((char*)block);

        usart_send_line(NULL);
    }

    return 0;
}
