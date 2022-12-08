#define  BAUD      57600

#include "global.h"

#include <stdint.h>
#include <avr/pgmspace.h>

#include "usart.h"
#include "spi.h"
#include "sd.h"
#include "fat32.h"
#include "ym2612.h"
#include "sn76489.h"
#include "vgmplayer.h"
#include "sram.h"

SD_Block_Cache block = { -1 };

MBR mbr;
FAT32_FS fs;
FAT32_File root_dir;
FAT32_FileStream dir_stream;

VGMPlayer vgm_player;

bool input_callback(char byte) {
    switch (byte) {
        case 'p':   // pause
            while (usart_receive(false) != 'p') ;
            break;
        case 'n':   // next
            return true;
    }

    return false;
}

void stream_directory(FAT32_FileStream* stream, void* file, size_t len, void* level) {
    FAT32_File* ffile = (FAT32_File*)file;

    if (ffile->name[0] == '.') {
        fat32_stream_advance(stream, 1);
        return;
    }

    // Print file info
    char format[20];
    char line[40];
    sprintf(format, "[%%c] %%10lu%%%ds%%s", ((int)level + 1) * 4);
    sprintf(line, format, (DIRECTORY(ffile) ? 'D' : 'F'), ffile->size, "", ffile->name);
    usart_send_line(line);

    if (DIRECTORY(ffile)) {
        // Traverse directory
        FAT32_FileStream dir_stream;
        fat32_stream(&dir_stream, ffile, &block, stream_directory);

        while (fat32_stream_next(&dir_stream, (void*)((int)level + 1))) ;
    } else if (fat32_file_has_extension(ffile, "VGM")) {
        // Play VGM
        vgm_player_reset(&vgm_player);
        vgm_player_play(&vgm_player, ffile, input_callback);
    }

    fat32_stream_advance(stream, 1);
}

bool init() {
    // Serial
    usart_init(BAUD);
    usart_send_line("Hello world!");
    usart_send_line(NULL);

    // SPI Low speed
    spi_init(SPI_128);

    // Storage
    if (!sd_init()) {
        return false;
    }

    // SPI High speed
    spi_init(SPI_2);

    sram_init();

    // Synths
    ym2612_init();
    sn76489_init();

    // Filesystem
    if (!sd_read_mbr(&mbr)) {
        return false;
    }
    if (!fat32_init(&fs, mbr.lba[0])) {
        return false;
    }

    // VGM player
    vgm_player_init(&vgm_player, &block);

    return true;
}

int main() {
    if (!init()) return 1;

    // Print file list header
    usart_send_line(" T  Size          Filename");
    usart_send_line("--------------------------");

    // Walk the file system
    fat32_root(&root_dir, &fs);
    fat32_stream(&dir_stream, &root_dir, &block, stream_directory);
    while (fat32_stream_next(&dir_stream, 0)) ;

    return 0;
}

