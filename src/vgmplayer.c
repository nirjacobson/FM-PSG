#include "vgmplayer.h"

volatile uint16_t timer = 0;

ISR (TIMER1_COMPA_vect)
{
    if (timer > 0) timer--;
}

void stream_vgm(VGM_Stream* stream, uint8_t* command, uint8_t len, void* player) {
    VGMPlayer* vgmPlayer = (VGMPlayer*)player;

    if (command[0] == VGM_COMMAND_YM2612_WRITE1) {
        while (timer > 0);
        ym2612_write(1, command[1], command[2]);
    } else if (command[0] == VGM_COMMAND_YM2612_WRITE2) {
        while (timer > 0);
        ym2612_write(2, command[1], command[2]);
    } else if (command[0] == VGM_COMMAND_WAITN) {
        timer += *(uint16_t*)&command[1];
    } else if (command[0] == VGM_COMMAND_WAIT_735) {
        timer += 735;
    } else if (command[0] == VGM_COMMAND_WAIT_882) {
        timer += 882;
    } else if ((command[0] & 0xF0) == VGM_COMMAND_WAITN1) {
        timer += (command[0] & 0x0F) + 1;
    } else if ((command[0] & 0xF0) == VGM_COMMAND_YM2612_WRITED) {
        for (uint8_t i = 0; i < len; i++) {
            uint8_t data = pcm_stream_next(&vgmPlayer->pcm_stream);
            while (timer > 0);
            ym2612_write(1, YM2612_DAC, data);
            timer += (command[i] & 0x0F);
        }
    } else if (command[0] == VGM_COMMAND_SN76489_WRITE) {
        while (timer > 0);
        sn76489_write(command[1]);
    } else if (command[0] == VGM_COMMAND_DATA_BLOCK) {
        uint32_t size = *(uint32_t*)&command[3];
        pcm_stream_set_data(&vgmPlayer->pcm_stream, vgm_stream_position(stream), size);
    } else if (command[0] == VGM_COMMAND_SEEK) {
        uint32_t offset = *(uint32_t*)&command[1];
        pcm_stream_seek(&vgmPlayer->pcm_stream, offset);
    } else if (command[0] == VGM_COMMAND_END_OF_SOUND) {
        // end song
    } else if (command[0] == VGM_COMMAND_GAME_GEAR_WRITE) {

    } else {
        vgm_stream_debug(stream);
        _delay_ms(500);
    }
}

void vgm_player_init(VGMPlayer* player, SD_Block_Cache* block) {
    player->block = block;

    // Timer 1
    OCR1A = F_CPU / 44100;                // 1 sample length
    TIMSK1 = (1 << OCIE1A);               // Timer interrupt

    TCNT1 = 0;                           // reset timer counter
    TCCR1B = (1 << WGM12) | (1 << CS10); // CTC, no prescaling

    sei();
}

void vgm_player_play(VGMPlayer* player, FAT32_File* file, vgm_input_callback input_callback) {
    vgm_stream(&player->vgm_stream, file, player->block, stream_vgm);
    pcm_stream(&player->pcm_stream, file, player->block);

    while (vgm_stream_next(&player->vgm_stream, player)) {
        char byte;
        if (usart_try_receive(false, &byte)) {
            if (input_callback(byte)) {
                break;
            }
        }
        if (is_button_pressed(1)) {
            while (is_button_pressed(1)) ;
            if (input_callback('p')) {
                break;
            }
        }
        if (is_button_pressed(3)) {
            while (is_button_pressed(3)) ;
            if (input_callback('n')) {
                break;
            }
        }
    }
}

void vgm_player_reset(VGMPlayer* player) {
    ym2612_init();
    sn76489_init();
}