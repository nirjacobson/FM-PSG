#include "flash.h"

void flash_erase_chip() {
  uint8_t sreg;
  sreg = SREG;

  cli();

  eeprom_busy_wait ();
  
  for (size_t i = 0; i < FLASH_PAGES; i++) {
    boot_page_erase(i * FLASH_PAGE_SIZE);
    boot_spm_busy_wait ();
  }
  
  boot_rww_enable ();

  SREG = sreg;
}

void flash_write_page(uint32_t page, uint8_t* data, size_t len) {
    uint16_t i;
    uint8_t sreg;
    sreg = SREG;

    cli();

    eeprom_busy_wait ();

    boot_page_erase (page);
    boot_spm_busy_wait ();

    for (i=0; i<FLASH_PAGE_SIZE && i<len; i+=2)
    {
        // Set up little-endian word.
        uint16_t w = *data++;
        w += (*data++) << 8;
    
        boot_page_fill (page + i, w);
    }

    boot_page_write (page);
    boot_spm_busy_wait();
    
    boot_rww_enable ();
    
    SREG = sreg;
}


void flash_read_page(uint32_t page, uint8_t* data, size_t len) {
  for (size_t i = 0; i < FLASH_PAGE_SIZE && i < len; i++) {
    *(uint16_t*)(data + i) = pgm_read_byte(page + i);
  }
}

void flash_program(FAT32_File* file) {
    uint8_t block[SD_BLOCK_SIZE];
    uint8_t sreg;

    uint32_t page = 0;

    uint32_t written = 0;
    uint32_t block_offset = 0;

    uint8_t block_idx = 0;

    fat32_read_block_from_file(file, block_idx, block);

    while (written < file->size) {
        sreg = SREG;
        cli();

        // eeprom_busy_wait ();

        boot_page_erase (page);
        boot_spm_busy_wait ();

        for (uint16_t i = 0; i < SPM_PAGESIZE && written < file->size; i += sizeof(uint16_t)) {
            if (block_offset == SD_BLOCK_SIZE) {
                fat32_read_block_from_file(file, ++block_idx, block);
                block_offset = 0;
            }
            uint16_t w = *(uint16_t*)(block + block_offset);
            block_offset += 2;

            boot_page_fill(page + i, w);
            written += 2;
        }

        boot_page_write(page);
        boot_spm_busy_wait();

        boot_rww_enable();

        SREG = sreg;

        page += SPM_PAGESIZE;
    }
}

void flash_args(int argc, char* argv[]) {
  uint8_t page[SPM_PAGESIZE];
  uint8_t offset = 0;

  *(uint16_t*)(page + offset) = argc;
  offset += sizeof(uint16_t);
  uint16_t ptr = ((argc + 1) * sizeof(uint16_t));
  for (int i = 0; i < argc; i++) {
    *(uint16_t*)(page + offset) = ptr;
    offset += sizeof(uint16_t);
    strcpy((char*)(page + ptr), argv[i]);
    ptr += strlen(argv[i]) + 1;
  }

  uint8_t sreg = SREG;
  cli();

//   eeprom_busy_wait ();

  boot_page_erase (FLASH_ARGS_PAGE);
  boot_spm_busy_wait ();

  for (uint16_t i = 0; i < SPM_PAGESIZE; i += sizeof(uint16_t)) {
      uint16_t w = *(uint16_t*)(page + i);

      boot_page_fill(FLASH_ARGS_PAGE + i, w);
  }

  boot_page_write(FLASH_ARGS_PAGE);
  boot_spm_busy_wait();

  boot_rww_enable();

  SREG = sreg;
}