#include "vgm.h"

void vgm_stream_file(FAT32_FileStream* fileStream, void* data, size_t len, void* vgmStreamPtr) {
    VGM_Stream* vgmStream = (VGM_Stream*)vgmStreamPtr;
    vgmStream->buffer = (uint8_t*)data;
    vgmStream->buffer_index = 0;
    vgmStream->buffer_size = len;
    vgmStream->buffer_position = fileStream->position;

    fat32_stream_advance(fileStream, len);
}

void vgm_stream(VGM_Stream* vgmStream, FAT32_File* file, SD_Block_Cache* block, vgm_stream_callback callback) {
    fat32_stream(&vgmStream->fileStream, file, block, vgm_stream_file);
    vgmStream->loopOffset = 0;
    vgmStream->loopCount = 1;
    vgmStream->callback = callback;
}

bool vgm_stream_next(VGM_Stream* stream, void* userData) {
    if (stream->loopOffset == 0) {
        fat32_stream_next(&stream->fileStream, (void*)stream);

        stream->loopOffset = *(uint32_t*)&stream->buffer[VGM_HEADER_LOOP_OFFSET] + VGM_HEADER_LOOP_OFFSET;

        if (stream->loopOffset == VGM_HEADER_LOOP_OFFSET) {
            stream->loopCount = 0;
        }

        uint32_t dataOffset = *(uint32_t*)&stream->buffer[VGM_HEADER_DATA_OFFSET] + VGM_HEADER_DATA_OFFSET;

        if (dataOffset == VGM_HEADER_DATA_OFFSET) {
            dataOffset = 0x40;
        }

        while ((stream->buffer_position + stream->buffer_size) <= dataOffset) {
            fat32_stream_next(&stream->fileStream, (void*)stream);
        }

        stream->buffer_index = dataOffset - stream->buffer_position;
    }

    if (stream->command[0] == VGM_COMMAND_DATA_BLOCK) {
        uint32_t* dataSize = (uint32_t*)&stream->command[3];

        fat32_stream_set_position(&stream->fileStream, vgm_stream_position(stream) + *dataSize);
        fat32_stream_next(&stream->fileStream, (void*)stream);
    }

    bool called = vgm_stream_next_command(stream, userData);

    if (!called && stream->command[0] == VGM_COMMAND_END_OF_SOUND) {
        if (stream->loopCount > 0) {
            fat32_stream_set_position(&stream->fileStream, stream->loopOffset);
            fat32_stream_next(&stream->fileStream, (void*)stream);
            called = vgm_stream_next_command(stream, userData);
            stream->loopCount--;
        }
    }

    if (!called) {
        stream->callback(stream, stream->command, stream->commandLen, userData);
    }

    return !(stream->command[0] == VGM_COMMAND_END_OF_SOUND && stream->loopCount == 0);
}

uint8_t vgm_command_length(uint8_t command) {
    switch (command & 0xF0) {
        case VGM_COMMAND_PCM_SIZE:
            if (pcm_is_long_channel(command & 0x0F)) {
                return 5;
            }
            return 3;
        case VGM_COMMAND_PCM_SEEK:
            if (pcm_is_long_channel(command & 0x0F)) {
                return 5;
            }
            return 3;
        case VGM_COMMAND_PCM_ATTENUATION:
            return 2;
        default:
            switch (command) {
                case VGM_COMMAND_SN76489_WRITE:
                case VGM_COMMAND_GAME_GEAR_WRITE:
                    return 2;
                case VGM_COMMAND_YM2612_WRITE1:
                case VGM_COMMAND_YM2612_WRITE2:
                case VGM_COMMAND_WAITN:
                    return 3;
                case VGM_COMMAND_YM2612_WRITEDN:
                    return 5;
                case VGM_COMMAND_DATA_BLOCK:
                    return 7;
                default:
                    return 1;
            }
    }
}

bool vgm_stream_next_command(VGM_Stream* stream, void* userData) {
    if (stream->buffer_index == stream->buffer_size) {
        fat32_stream_next(&stream->fileStream, (void*)stream);
    }

    uint8_t command = stream->buffer[stream->buffer_index];

    if ((command & 0xF0) == VGM_COMMAND_YM2612_WRITED) {
        uint16_t commandLength = 0;
        for (uint16_t i = 0; (stream->buffer_index + i) < stream->buffer_size
                && (stream->buffer[stream->buffer_index + i] & 0xF0) == VGM_COMMAND_YM2612_WRITED;
                i++) {
            commandLength++;
        }

        stream->callback(stream, &stream->buffer[stream->buffer_index], commandLength, userData);

        stream->buffer_index += commandLength;

        return true;
    } else {
        uint8_t commandLength = vgm_command_length(command);

        if (stream->buffer_index + commandLength > stream->buffer_size) {
            size_t firstSize = stream->buffer_size - stream->buffer_index;
            size_t restSize = commandLength - firstSize;
            memcpy(stream->command, stream->buffer + stream->buffer_index, firstSize);
            fat32_stream_next(&stream->fileStream, (void*)stream);
            memcpy(stream->command + firstSize, stream->buffer, restSize);
            stream->buffer_index = restSize;
        } else {
            memcpy(stream->command, stream->buffer + stream->buffer_index, commandLength);
            stream->buffer_index += commandLength;
        }

        stream->commandLen = commandLength;

        return false;
    }
}

size_t vgm_stream_position(VGM_Stream* stream) {
    return stream->buffer_position + stream->buffer_index;
}

void vgm_stream_debug(VGM_Stream* stream) {
    usart_send_dec(vgm_stream_position(stream));
    usart_send_str("    ");
    for (size_t i = 0; i < sizeof(stream->command); i++) {
        char str[4];
        sprintf(str, "%02X ", stream->command[i]);
        usart_send_str(str);
    }
    usart_send_line(NULL);
}
