#ifndef PCM_H
#define PCM_H

#define PCM_CHANNELS        4
#define PCM_LONG_CHANNEL    0

#include "fat32.h"
#include "sram.h"

typedef struct {
    FAT32_FileStream fileStream;

    uint16_t write_ptr;
    uint32_t write_ptr_ext;
    uint16_t read_ptrs[PCM_CHANNELS];
    uint8_t read_ptr_ext_high;
    uint8_t attens[PCM_CHANNELS];
    uint16_t sizes[PCM_CHANNELS];
    uint16_t ends[PCM_CHANNELS];
    uint8_t  divisor;
    uint8_t last_sample;
    uint32_t copy_bytes_remaining;
} PCM_Stream;

void pcm_stream(PCM_Stream* pcmStream, FAT32_File* file, SD_Block_Cache* block);
void pcm_stream_set_data(PCM_Stream* pcmStream, uint32_t offset, uint32_t size);

void pcm_stream_seek(PCM_Stream* pcmStream, uint8_t channel, uint16_t offset);
void pcm_stream_seek_ext(PCM_Stream* pcmStream, uint8_t channel, uint32_t offset);
uint8_t pcm_stream_next(PCM_Stream* pcmStream);

void pcm_set_attenuation(PCM_Stream* pcmStream, uint8_t channel, uint8_t att);
void pcm_set_size(PCM_Stream* pcmStream, uint8_t channel, uint16_t size);
void pcm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* pcmStreamPtr);

bool pcm_is_long_channel(uint8_t channel);

#endif // PCM_H
