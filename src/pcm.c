#include "pcm.h"

void pcm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* pcmStreamPtr) {
    PCM_Stream* pcmStream = (PCM_Stream*)pcmStreamPtr;
    uint8_t* cdata = (uint8_t*)data;
    size_t length = pcmStream->copy_bytes_remaining < len ? pcmStream->copy_bytes_remaining : len;

    for (size_t i = 0; i < length; i++) {
        sram_write(pcmStream->write_ptr++, cdata[i]);
    }

    pcmStream->copy_bytes_remaining -= length;

    fat32_stream_advance(fileStream, length);
}

void pcm_stream(PCM_Stream* pcmStream, FAT32_File* file, SD_Block_Cache* block) {
    fat32_stream(&pcmStream->fileStream, file, block, pcm_stream_file);

    pcmStream->write_ptr = 0;
    for (int i = 0; i < PCM_CHANNELS; i++) {
        pcmStream->read_ptrs[i] = -1;
        pcmStream->attens[i] = 0;
    }
    pcmStream->stream_count = 0;
    pcmStream->last_sample = 0x80;
    pcmStream->copy_bytes_remaining = 0;
}

void pcm_stream_set_data(PCM_Stream* pcmStream, uint32_t offset, uint32_t size) {
    pcmStream->copy_bytes_remaining = size;
    fat32_stream_set_position(&pcmStream->fileStream, offset);

    while (pcmStream->copy_bytes_remaining > 0 && fat32_stream_next(&pcmStream->fileStream, pcmStream)) ;
}

uint8_t pcm_stream_next(PCM_Stream* pcmStream) {
    int16_t result = 0x00;

    if (pcmStream->stream_count > 1) {
        pcmStream->stream_count--;

        for (int i = 0; i < PCM_CHANNELS; i++) {
            if (pcmStream->read_ptrs[i] != -1) {
                pcmStream->read_ptrs[i]++;
            }
        }

        return pcmStream->last_sample;
    }

    pcmStream->stream_count = 0;
    for (int i = 0; i < PCM_CHANNELS; i++) {
        if (pcmStream->read_ptrs[i] != -1) {
            pcmStream->stream_count++;

            int16_t sample = sram_read(pcmStream->read_ptrs[i]++) - 0x80;

            result += sample >> pcmStream->attens[i];
        }
    }

    result += 0x80;

    if (result < 0)    result = 0;
    if (result > 0xFF) result = 0xFF;

    pcmStream->last_sample = result;

    return result;
}

void pcm_stream_seek(PCM_Stream* pcmStream, uint8_t channel, uint32_t offset) {
    pcmStream->read_ptrs[channel] = offset;
}

void pcm_set_attenuation(PCM_Stream* pcmStream, uint8_t channel, uint8_t att) {
    pcmStream->attens[channel] = att;
}
