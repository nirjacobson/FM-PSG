#define  BAUD      57600

#include "global.h"

#include <stdint.h>
#include <avr/pgmspace.h>

#include "usart.h"
#include "buttons.h"
#include "spi.h"
#include "sd.h"
#include "fat32.h"
#include "flash.h"
#include "stk500v2.h"

STK500V2 stk500v2;

SD_Block_Cache block = { -1 };

MBR mbr;
FAT32_FS fs;
FAT32_File root;

int parse_command(char* command, char* argv[]) {
  int argc = 0;
  size_t len = strlen(command);

  for (size_t i = 0; i < len; i++) {
      if ((i == 0 || command[i-1] == '\0') && command[i] != ' ') {
          argv[argc++] = &command[i];
      }

      if (command[i] == ' ') {
          command[i] = '\0';
      }
  }

  return argc;
}

void common_init() {
    // Buttons
    buttons_init();

    // USART
    usart_init(BAUD);
}

bool sd_boot_init() {
    // SPI Low speed
    spi_init(SPI_128);

    // Storage
    if (!sd_init()) {
        return false;
    }

    // SPI High speed
    spi_init(SPI_2);

    // Filesystem
    if (!sd_read_mbr(&mbr)) {
        return false;
    }
    if (!fat32_init(&fs, mbr.lba[0])) {
        return false;
    }

    fat32_root(&root, &fs);

    return true;
}

bool sd_boot() {
    if (!sd_boot_init()) return 1;

    FAT32_File bindir;
    bool found = fat32_get_file_from_directory(&root, &bindir, "BIN");

    if (found) {
        while (true) {
            usart_send_str("> ");
            char line[128];
            usart_receive_str(line, true);
            usart_send_line(NULL);

            char* argv[8];
            int argc = parse_command(line, argv);

            FAT32_File bin;
            found = fat32_get_file_from_directory(&bindir, &bin, argv[0]);

            if (found && argc > 0) {
                flash_program(&bin);
                flash_args(argc, argv);
                ((void(*)(void))0)(); // reset
            }
        }
    }

    return 0;
}

void usb_program_init() {
    stk500v2_init(&stk500v2);
}

void usb_program() {
    usb_program_init();

    while (true) {
        STK500V2_Message message;
        STK500V2_Message answer;
        
        stk500v2_receive_message(&message);

        if (message.checksum == stk500v2_message_checksum(&message)) {
            stk500v2_answer_message(&stk500v2, &message, &answer);
            stk500v2_send_message(&answer);

            if (answer.body[0] == STK500V2_CMD_LEAVE_PROGMODE_ISP) {
                ((void(*)(void))0)(); // reset
            }
        }
    }

}

int main() {
    common_init();

    if (is_button_pressed(0)) {
        usb_program();
    } else if (is_button_pressed(3)) {
        sd_boot();
    }

    ((void(*)(void))0)(); // reset
}

