#include "sd.h"

bool sd_init() {
    uint8_t result;
    uint32_t ocr;

    DIR(SD_CS, 1);
    OUT(SD_CS, 1);

    for (uint8_t i = 0; i < 10; i++)
        spi_transmit(0xFF);

    while (sd_command(SD_CMD0, SD_ARG_NONE, SD_CRC_CMD0) != SD_IDLE) ;

    result = sd_command_ocr(SD_CMD8, SD_ARG_CMD8, SD_CRC_CMD8, &ocr);

    if (result & SD_ILLEGAL_COMMAND) {
        if ((result = sd_command_ocr(SD_CMD58, SD_ARG_NONE, SD_CRC_CMD58, &ocr)) != SD_IDLE)
            return false;
    }

    if (!(ocr & SD_VOLTAGE_OK))
        return false;

    while (true) {
        if ((result = sd_command(SD_CMD55, SD_ARG_NONE, SD_CRC_NONE)) != SD_IDLE)
            return false;

        if ((result = sd_command(SD_CMD41, SD_ARG_CMD41, SD_CRC_NONE)) == SD_READY)
            break;
    }

    return sd_command(SD_CMD16, SD_BLOCK_SIZE, SD_CRC_NONE) == SD_READY;
}

uint8_t sd_command(uint8_t command, uint32_t argument, uint8_t crc) {
    OUT(SD_CS, 0);
    uint8_t result = sd_transmit(command, argument, crc);
    OUT(SD_CS, 1);

    return result;
}

uint8_t sd_command_ocr(uint8_t command, uint32_t argument, uint8_t crc, uint32_t* ocr) {
    uint32_t ocrResult = 0;

    OUT(SD_CS, 0);
    uint8_t result = sd_transmit(command, argument, crc);
    for (uint8_t i = 0; i < sizeof(ocrResult); i++) {
        ocrResult = (ocrResult << 8) | spi_transmit(0xFF);
    }
    OUT(SD_CS, 1);

    *ocr = ocrResult;
    return result;
}

uint8_t sd_transmit(uint8_t command, uint32_t argument, uint8_t crc) {
    uint8_t result;

    spi_transmit(command | 0x40);
    spi_transmit(argument >> 24);
    spi_transmit(argument >> 16);
    spi_transmit(argument >> 8);
    spi_transmit(argument);
    spi_transmit(crc);

    while ((result = spi_transmit(0xFF)) & (1 << 7)) ;

    return result;
}

bool sd_read_block(uint32_t address, uint8_t* data) {
    OUT(SD_CS, 0);

    if (sd_transmit(SD_CMD17, address, SD_CRC_NONE) != SD_READY) {
        OUT(SD_CS, 1);
        return false;
    }

    while (spi_transmit(0xFF) != SD_DATA_BLOCK_TOKEN) ;

    for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++) {
        data[i] = spi_transmit(0xFF);
    }

    // CRC
    spi_transmit(0xFF);
    spi_transmit(0xFF);

    OUT(SD_CS, 1);

    return true;
}

bool sd_read_long(uint32_t lba, uint8_t la, uint32_t* value) {
    OUT(SD_CS, 0);

    if (sd_transmit(SD_CMD17, lba, SD_CRC_NONE) != SD_READY) {
        OUT(SD_CS, 1);
        return false;
    }

    while (spi_transmit(0xFF) != SD_DATA_BLOCK_TOKEN) ;

    uint8_t value_i = 0;
    for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++) {
        if (i >= la * 4 && value_i < 4) {
            *((uint8_t*)(value) + value_i++) = spi_transmit(0xFF);
        } else {
            spi_transmit(0xFF);
        }
    }

    // CRC
    spi_transmit(0xFF);
    spi_transmit(0xFF);

    OUT(SD_CS, 1);

    return true;
}

bool sd_write_block(uint32_t address, const uint8_t* data) {
    OUT(SD_CS, 0);

    if (sd_transmit(SD_CMD24, address, SD_CRC_NONE) != SD_READY) {
        OUT(SD_CS, 1);
        return false;
    }

    spi_transmit(SD_DATA_BLOCK_TOKEN);
    for (uint16_t i = 0; i < SD_BLOCK_SIZE; i++) {
        spi_transmit(data[i]);
    }

    while (!SD_DATA_ACCEPTED(spi_transmit(0xFF))) ;

    while (spi_transmit(0xFF) != 0xFF) ;

    OUT(SD_CS, 1);

    return true;
}

bool sd_read_mbr(MBR* mbr) {
    uint8_t block[SD_BLOCK_SIZE];
    if (!sd_read_block(0, block))
        return false;

    uint16_t offset = 446 + 8;

    for (uint8_t i = 0; i < 4; i++) {
        mbr->lba[i] = *(uint32_t*)(block + offset + (i * 16));
        mbr->sectors[i] = *(uint32_t*)(block + offset + 4 + (i * 16));
    }

    return true;
}

bool sd_block_cache_load(SD_Block_Cache* cache, uint32_t lba) {
    if (cache->lba != lba) {
        bool retval = sd_read_block(lba, cache->data);
        cache->lba = lba;

        return retval;
    }

    return true;
}
