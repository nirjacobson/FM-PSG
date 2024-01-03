#ifndef YM2612_H
#define YM2612_H

#include "global.h"
#include "spi.h"

#include <avr/cpufunc.h>

#define YM2612_CS           0xD4
#define YM2612_IC           0xD5

#define YM2612_WR           7
#define YM2612_RD           6
#define YM2612_A0           5
#define YM2612_A1           4

#define YM2612_KEY_ON_OFF   0x28
#define YM2612_DAC          0x2A

void ym2612_init();
void ym2612_write(unsigned int part, unsigned char address, unsigned char data);
void ym2612_test();

#endif // YM2612_H
