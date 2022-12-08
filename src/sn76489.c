#include "sn76489.h"

void sn76489_init() {
    DIR(SN76489_CE, 1);
    OUT(SN76489_CE, 1);

    DIR(SN76489_READY, 0);

    while (!IN(SN76489_READY)) ;

    // Tone 1 frequency
    sn76489_write(((0x8 | 0) << 4) | 0x0);
    sn76489_write(0x00);

    // Tone 1 attenuation
    sn76489_write(((0x8 | 1) << 4) | 0xF);

    // Tone 2 frequency
    sn76489_write(((0x8 | 2) << 4) | 0x0);
    sn76489_write(0x00);

    // Tone 2 attenuation
    sn76489_write(((0x8 | 3) << 4) | 0xF);

    // Tone 3 frequency
    sn76489_write(((0x8 | 4) << 4) | 0x0);
    sn76489_write(0x00);

    // Tone 3 attenuation
    sn76489_write(((0x8 | 5) << 4) | 0xF);

    // Noise control
    sn76489_write(((0x8 | 6) << 4) | 0x0);

    // Noise attenuation
    sn76489_write(((0x8 | 7) << 4) | 0xF);
}

void sn76489_write(uint8_t data) {
    spi_transmit(data);

    OUT(SN76489_CE, 0);

    _NOP();
    _NOP();
    _NOP();
    _NOP();

    while (!IN(SN76489_READY)) ;

    OUT(SN76489_CE, 1);
}

void sn76489_test() {
    DIR(SN76489_CE, 1);
    OUT(SN76489_CE, 1);

    DIR(SN76489_READY, 0);

    while (!IN(SN76489_READY)) ;

    // Tone 1 frequency
    uint16_t d1 = d(C4);
    sn76489_write(((0x8 | 0) << 4) | (d1 >> 8));
    sn76489_write(d1 & 0xFF);

    // Tone 1 attenuation
    sn76489_write(((0x8 | 1) << 4) | 0x4);

    _delay_ms(1000);

    // Tone 2 frequency
    uint16_t d2 = d(E4);
    sn76489_write(((0x8 | 2) << 4) | (d2 >> 8));
    sn76489_write(d2 & 0xFF);

    // Tone 2 attenuation
    sn76489_write(((0x8 | 3) << 4) | 0x2);

    _delay_ms(1000);

    // Tone 3 frequency
    uint16_t d3 = d(G4);
    sn76489_write(((0x8 | 4) << 4) | (d3 >> 8));
    sn76489_write(d3 & 0xFF);

    // Tone 3 attenuation
    sn76489_write(((0x8 | 5) << 4) | 0x1);

    _delay_ms(1000);
}
