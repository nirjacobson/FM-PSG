#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include "global.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "sd.h"
#include "fat32.h"
#include "vgm.h"
#include "pcm.h"
#include "ym2612.h"
#include "sn76489.h"
#include "usart.h"
#include "buttons.h"

struct VGMPlayer;

typedef bool(*vgm_input_callback)(char byte);

typedef struct VGMPlayer {
    VGM_Stream vgm_stream;
    PCM_Stream pcm_stream;
    SD_Block_Cache* block;
} VGMPlayer;

void vgm_player_init(VGMPlayer* player, SD_Block_Cache* block);
void vgm_player_play(VGMPlayer* player, FAT32_File* file, vgm_input_callback input_callback);
void vgm_player_reset(VGMPlayer* player);

#endif // VGMPLAYER_H
