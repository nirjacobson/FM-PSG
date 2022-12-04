#define  BAUD      57600

#include <stdio.h>
#include <avr/io.h>

#include "spi.h"
#include "sd.h"
#include "usart.h"

int main() {
    uint8_t block[SD_BLOCK_SIZE];

    usart_init(BAUD);

    usart_send_line("Hello world!");

    spi_init(SPI_128);
    if (!sd_init()) {
        usart_send_line("Failed to initialize SD card.");
        return 1;
    }
    spi_init(SPI_2);

    while (true) {
        char address_str[9];
        unsigned long address;
        usart_send_str("Enter a block address: 0x");
        usart_receive_str((char*)address_str, true);
        sscanf(address_str, "%lx", &address);

        usart_send_line(NULL);

        memset(block, 0, SD_BLOCK_SIZE);
        usart_send_str("Enter a block of text: ");
        usart_receive_str((char*)block, true);

        if (!sd_write_block(address, block)) {
            usart_send_line("Failed to write to SD card.");
            return 1;
        }
        memset(block, 0, SD_BLOCK_SIZE);
        if (!sd_read_block(address, block)) {
            usart_send_line("Failed to read from SD card.");
            return 1;
        }

        usart_send_line(NULL);

        usart_send_str("This text was written: ");
        usart_send_str((char*)block);

        usart_send_line(NULL);
        usart_send_line(NULL);
    }

    return 0;
}
