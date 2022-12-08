#ifndef VGM_H
#define VGM_H

#define VGM_HEADER_LOOP_OFFSET      0x1C
#define VGM_HEADER_DATA_OFFSET      0x34

#define VGM_COMMAND_GAME_GEAR_WRITE 0x4F
#define VGM_COMMAND_SN76489_WRITE   0x50
#define VGM_COMMAND_YM2612_WRITE1   0x52
#define VGM_COMMAND_YM2612_WRITE2   0x53
#define VGM_COMMAND_WAITN           0x61
#define VGM_COMMAND_WAIT_735        0x62
#define VGM_COMMAND_WAIT_882        0x63
#define VGM_COMMAND_END_OF_SOUND    0x66
#define VGM_COMMAND_DATA_BLOCK      0x67
#define VGM_COMMAND_WAITN1          0x70
#define VGM_COMMAND_YM2612_WRITED   0x80
#define VGM_COMMAND_SEEK            0xE0

#include "fat32.h"
#include "usart.h"

struct VGM_Stream;

typedef void(*vgm_stream_callback)(struct VGM_Stream*, uint8_t*, uint8_t, void*);

typedef struct VGM_Stream {
    FAT32_FileStream fileStream;
    uint32_t loopOffset;
    uint8_t loopCount;
    uint8_t command[16];
    uint8_t commandLen;
    vgm_stream_callback callback;

    uint8_t* buffer;
    uint16_t buffer_index;
    uint32_t buffer_position;
    size_t buffer_size;
} VGM_Stream;

void vgm_stream(VGM_Stream* vgmStream, FAT32_File* file, SD_Block_Cache* block, vgm_stream_callback callback);
bool vgm_stream_next(VGM_Stream* stream, void* userData);
void vgm_stream_next_command(VGM_Stream* stream);

void vgm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* vgmStreamPtr);

size_t vgm_stream_position(VGM_Stream* stream);

void vgm_stream_debug(VGM_Stream* stream);

#endif // #VGM_H
