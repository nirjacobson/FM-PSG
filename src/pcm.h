#ifndef PCM_H
#define PCM_H

#include "fat32.h"
#include "sram.h"

typedef struct {
    FAT32_FileStream fileStream;

    uint32_t write_ptr;
    uint32_t read_ptr;
    uint32_t copy_bytes_remaining;
} PCM_Stream;

void pcm_stream(PCM_Stream* pcmStream, FAT32_File* file, SD_Block_Cache* block);
void pcm_stream_set_data(PCM_Stream* pcmStream, uint32_t offset, uint32_t size);

uint8_t pcm_stream_next(PCM_Stream* pcmStream);
void pcm_stream_seek(PCM_Stream* pcmStream, uint32_t offset);

void pcm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* pcmStreamPtr);

#endif // PCM_H
