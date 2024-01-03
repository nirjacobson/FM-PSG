#include "pcm.h"

void pcm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* pcmStreamPtr) {
    PCM_Stream* pcmStream = (PCM_Stream*)pcmStreamPtr;
    uint8_t* cdata = (uint8_t*)data;
    size_t length = pcmStream->copy_bytes_remaining < len ? pcmStream->copy_bytes_remaining : len;

    for (size_t i = 0; i < length; i++) {
        if (pcmStream->write_ptr_ext != -1) {
            sram_write_ext(pcmStream->write_ptr_ext++, cdata[i]);
        } else {
            sram_write(pcmStream->write_ptr++, cdata[i]);
            if (pcmStream->write_ptr == 0) {
                pcmStream->write_ptr_ext = 0x10000;
            }
        }
    }

    pcmStream->copy_bytes_remaining -= length;

    fat32_stream_advance(fileStream, length);
}

void pcm_stream(PCM_Stream* pcmStream, FAT32_File* file, SD_Block_Cache* block) {
    fat32_stream(&pcmStream->fileStream, file, block, pcm_stream_file);

    pcmStream->write_ptr = 0;
    pcmStream->write_ptr_ext = -1;
    for (int i = 0; i < PCM_CHANNELS; i++) {
        pcmStream->read_ptrs[i] = -1;
        pcmStream->attens[i] = 0;
        pcmStream->sizes[i] = 0;
        pcmStream->ends[i] = 0;
    }
    pcmStream->divisor = 1;
    pcmStream->read_ptr_ext_high = -1;
    pcmStream->last_sample = 0x80;
    pcmStream->copy_bytes_remaining = 0;
}

void pcm_stream_set_data(PCM_Stream* pcmStream, uint32_t offset, uint32_t size) {
    pcmStream->copy_bytes_remaining = size;
    fat32_stream_set_position(&pcmStream->fileStream, offset);

    while (pcmStream->copy_bytes_remaining > 0 && fat32_stream_next(&pcmStream->fileStream, pcmStream)) ;
}

void pcm_stream_seek(PCM_Stream* pcmStream, uint8_t channel, uint16_t offset) {
    pcmStream->read_ptrs[channel] = offset;
    pcmStream->ends[channel] = offset + pcmStream->sizes[channel];
}

void pcm_stream_seek_ext(PCM_Stream* pcmStream, uint8_t channel, uint32_t offset) {
    pcmStream->read_ptrs[channel] = offset & 0xFFFF;
    pcmStream->read_ptr_ext_high = ((uint8_t*)&offset)[2];
}

uint8_t pcm_stream_next(PCM_Stream* pcmStream) {
    int16_t result = 0x00;

    if (pcmStream->read_ptr_ext_high != (uint8_t)-1) {
        int16_t sample = sram_read_ext(pcmStream->read_ptr_ext_high, pcmStream->read_ptrs[PCM_LONG_CHANNEL]) - 0x80;

        if (++pcmStream->read_ptrs[PCM_LONG_CHANNEL] == 0) {
            pcmStream->read_ptr_ext_high++;
        }

        result += 0x80 + (sample >> pcmStream->attens[PCM_LONG_CHANNEL]);
    } else {
        if (pcmStream->divisor > 1) {
            pcmStream->divisor--;

            return pcmStream->last_sample;
        }

        pcmStream->divisor = 0;
        for (int i = 0; i < PCM_CHANNELS; i++) {
            if (i != PCM_LONG_CHANNEL && pcmStream->read_ptrs[i] != -1) {
                int16_t sample = sram_read(pcmStream->read_ptrs[i]) - 0x80;

                result += sample >> pcmStream->attens[i];
                pcmStream->divisor += 2;
            }
        }

        for (int i = 0; i < PCM_CHANNELS; i++) {
            if (i != PCM_LONG_CHANNEL && pcmStream->read_ptrs[i] != -1) {
                if ((pcmStream->ends[i] - pcmStream->read_ptrs[i]) > pcmStream->divisor) {
                    pcmStream->read_ptrs[i] += pcmStream->divisor;
                } else {
                    pcmStream->read_ptrs[i] = -1;
                }
            }
        }

        result += 0x80;

        if (result < 0)    result = 0;
        if (result > 0xFF) result = 0xFF;

        pcmStream->last_sample = result;
    }

    return result;
}

void pcm_set_attenuation(PCM_Stream* pcmStream, uint8_t channel, uint8_t att) {
    pcmStream->attens[channel] = att;
}

void pcm_set_size(PCM_Stream* pcmStream, uint8_t channel, uint16_t size) {
    pcmStream->sizes[channel] = size;
    if (pcmStream->read_ptrs[channel] != -1) {
        pcmStream->ends[channel] = pcmStream->read_ptrs[channel] + size;
    }
}

bool pcm_is_long_channel(uint8_t channel) {
    return channel == PCM_LONG_CHANNEL;
}