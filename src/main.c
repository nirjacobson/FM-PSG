#define  BAUD      57600

#include <stdio.h>
#include <avr/io.h>

#include "spi.h"
#include "sd.h"

#include "fat32.h"
#include "usart.h"

SD_Block_Cache block = { -1 };
MBR mbr;
FAT32_FS fs;
FAT32_File root_dir;
FAT32_FileStream root_stream;

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
    }

    fat32_stream_advance(stream, 1);
}

int main() {
    usart_init(BAUD);
    usart_send_line("Hello world!");
    usart_send_line(NULL);
    
    spi_init(SPI_128);
    if (!sd_init()) {
        usart_send_line("Failed to initialize SD card.");
        return 1;
    }
    spi_init(SPI_2);

    if (!sd_read_mbr(&mbr)) {
        usart_send_line("Failed to read MBR.");
        return 1;
    }
    if (!fat32_init(&fs, mbr.lba[0])) {
        usart_send_line("Failed to read FAT32 filesystem.");
        return 1;
    }

    fat32_root(&root_dir, &fs);
    fat32_stream(&root_stream, &root_dir, &block, stream_directory);

    char line[50];
    sprintf(line, "%-4s%-10s    %-s", " T", "Size", "Filename");
    usart_send_line(line);
    usart_send_line("------------------------------------------");
    while (fat32_stream_next(&root_stream, 0)) ;

    return 0;
}
