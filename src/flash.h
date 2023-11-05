#ifndef FLASH_H
#define FLASH_H

#define FLASH_ARGS_PAGE 0xDE00

#define FLASH_PAGE_SIZE SPM_PAGESIZE
#define FLASH_SIZE      65536
#define FLASH_PAGES     (FLASH_SIZE / FLASH_PAGE_SIZE)

#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>

#include "sd.h"
#include "fat32.h"

void flash_erase_chip();
void flash_write_page(uint32_t page, uint8_t* data, size_t len);
void flash_read_page(uint32_t page, uint8_t* data, size_t len);

void flash_program(FAT32_File* file);
void flash_args(int argc, char* argv[]);
void flash_read_args(int* argc, char** argv[]);

#endif // FLASH_H
