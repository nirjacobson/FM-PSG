#ifndef SN76489_H
#define SN76489_H

#define SN76489_CLK     3579545

#define SN76489_CE      0xD7
#define SN76489_READY   0xD6

#define C4              262
#define E4              330
#define G4              392
#define n(note)         (SN76489_CLK / (32 * note))
#define d(note)         (((n(note) & 0xF) << 8) | ((n(note) >> 4) & 0x3F))

#include <avr/cpufunc.h>

#include "global.h"
#include "spi.h"

#include <util/delay.h>

void sn76489_init();
void sn76489_write(uint8_t data);
void sn76489_test();


#endif // SN76489_H
