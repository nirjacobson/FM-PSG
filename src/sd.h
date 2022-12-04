#ifndef SD_H
#define SD_H

#define SD_CS               0xC4

#define SD_CMD0             0
#define SD_CMD8             8
#define SD_CMD58           58
#define SD_CMD55           55
#define SD_CMD41           41
#define SD_CMD17           17
#define SD_CMD24           24
#define SD_CMD16           16
#define SD_ARG_NONE        0x00000000
#define SD_ARG_CMD8        0x000001AA
#define SD_ARG_CMD41       0x40000000
#define SD_CRC_NONE        0x00
#define SD_CRC_CMD0        (0x94 | 0x01)
#define SD_CRC_CMD8        (0x86 | 0x01)
#define SD_CRC_CMD58       (0x74 | 0x01)

#define SD_IDLE            (1 << 0)
#define SD_ILLEGAL_COMMAND (1 << 2)
#define SD_VOLTAGE_OK      ((uint32_t)1 <<  8)
#define SD_DATA_ACCEPTED(x) ((x & 0xF) == 0b0101)

#define SD_BLOCK_SIZE       512

#define SD_DATA_BLOCK_TOKEN 0xFE
#define SD_READY            0x00

#include <avr/io.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include "global.h"
#include "spi.h"

typedef struct {
    uint32_t lba[4];
    uint32_t sectors[4];
} MBR;

typedef struct {
    uint32_t lba;
    uint8_t data[SD_BLOCK_SIZE];
} SD_Block_Cache;

bool sd_init();
uint8_t sd_command(uint8_t command, uint32_t argument, uint8_t crc);
uint8_t sd_command_ocr(uint8_t command, uint32_t argument, uint8_t crc, uint32_t* ocr);
uint8_t sd_transmit(uint8_t command, uint32_t argument, uint8_t crc);
bool sd_read_block(uint32_t address, uint8_t* data);
bool sd_read_long(uint32_t lba, uint8_t la, uint32_t* value);
bool sd_write_block(uint32_t address, const uint8_t* data);
bool sd_read_mbr(MBR* mbr);
bool sd_block_cache_load( SD_Block_Cache* cache, uint32_t lba);


#endif // SD_H
