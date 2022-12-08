#include "ym2612.h"

void ym2612_init() {
    DIR(YM2612_CS, 1);
    OUT(YM2612_CS, 1);

    DIR(YM2612_IC, 1);
    OUT(YM2612_IC, 1);

    OUT(YM2612_IC, 0);
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    OUT(YM2612_IC, 1);
    _NOP();
    _NOP();
    _NOP();
    _NOP();
}

void ym2612_write(unsigned int part, unsigned char address, unsigned char data) {
    unsigned char controlByte;

    // Select register
    controlByte = (1 << YM2612_RD);

    if (part == 2)
        controlByte |= (1 << YM2612_A1);

    spi_transmit(controlByte);
    spi_transmit(address);

    OUT(YM2612_CS, 0);
    _NOP();
    _NOP();
    OUT(YM2612_CS, 1);

    // Write register
    controlByte = (1 << YM2612_RD) | (1 << YM2612_A0);

    if (part == 2)
        controlByte |= (1 << YM2612_A1);

    spi_transmit(controlByte);
    spi_transmit(data);

    OUT(YM2612_CS, 0);
    _NOP();
    _NOP();
    OUT(YM2612_CS, 1);
}

void ym2612_test() {

    ym2612_write(1, 0x22, 0);
    ym2612_write(1, 0x27, 0);

    ym2612_write(1, 0x28, 0);
    ym2612_write(1, 0x28, 1);
    ym2612_write(1, 0x28, 2);
    ym2612_write(1, 0x28, 4);
    ym2612_write(1, 0x28, 5);
    ym2612_write(1, 0x28, 6);

    ym2612_write(1, 0x2B, 0);

    ym2612_write(1, 0x30, 0x71);
    ym2612_write(1, 0x34, 0x0D);
    ym2612_write(1, 0x38, 0x33);
    ym2612_write(1, 0x3C, 0x01);

    ym2612_write(1, 0x40, 0x23);
    ym2612_write(1, 0x44, 0x2D);
    ym2612_write(1, 0x48, 0x26);
    ym2612_write(1, 0x4C, 0x00);

    ym2612_write(1, 0x50, 0x5F);
    ym2612_write(1, 0x54, 0x99);
    ym2612_write(1, 0x58, 0x5F);
    ym2612_write(1, 0x5C, 0x94);

    ym2612_write(1, 0x60, 5);
    ym2612_write(1, 0x64, 5);
    ym2612_write(1, 0x68, 5);
    ym2612_write(1, 0x6C, 7);

    ym2612_write(1, 0x70, 2);
    ym2612_write(1, 0x74, 2);
    ym2612_write(1, 0x78, 2);
    ym2612_write(1, 0x7C, 2);

    ym2612_write(1, 0x80, 0x11);
    ym2612_write(1, 0x84, 0x11);
    ym2612_write(1, 0x88, 0x11);
    ym2612_write(1, 0x8C, 0xA6);

    ym2612_write(1, 0x90, 0);
    ym2612_write(1, 0x94, 0);
    ym2612_write(1, 0x98, 0);
    ym2612_write(1, 0x9C, 0);

    ym2612_write(1, 0xB0, 0x32);
    ym2612_write(1, 0xB4, 0xC0);
    ym2612_write(1, 0x28, 0x00);
    ym2612_write(1, 0xA4, 0x22);
    ym2612_write(1, 0xA0, 0x69);
    ym2612_write(1, 0x28, 0xF0);
}
