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
    pcmStream->read_ptr = 0;
    pcmStream->copy_bytes_remaining = 0;
}

void pcm_stream_set_data(PCM_Stream* pcmStream, uint32_t offset, uint32_t size) {
    pcmStream->copy_bytes_remaining = size;
    fat32_stream_set_position(&pcmStream->fileStream, offset);

    while (pcmStream->copy_bytes_remaining > 0 && fat32_stream_next(&pcmStream->fileStream, pcmStream)) ;
}

uint8_t pcm_stream_next(PCM_Stream* pcmStream) {
    return sram_read(pcmStream->read_ptr++);
}

void pcm_stream_seek(PCM_Stream* pcmStream, uint32_t offset) {
    pcmStream->read_ptr = offset;
}
