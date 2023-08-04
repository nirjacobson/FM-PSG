#ifndef GLOBAL_H
#define GLOBAL_H

#define F_CPU     20000000UL

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>

#define PORT_BASE_ADDR(x)       ((x - 0xA) * 3)
#define PORT_IN(x)              _SFR_IO8(PORT_BASE_ADDR(x))
#define PORT_DIR(x)             _SFR_IO8(PORT_BASE_ADDR(x) + 1)
#define PORT_OUT(x)             _SFR_IO8(PORT_BASE_ADDR(x) + 2)

#define PIN_MASK(x)             (1 << (x & 0x0F))
#define IN(x)                   ((_SFR_IO8(PORT_BASE_ADDR((x >> 4))) & PIN_MASK(x)) > 0)
#define DIR(x, v)               (v ? (_SFR_IO8(PORT_BASE_ADDR((x >> 4)) + 1) |= PIN_MASK(x)) : (_SFR_IO8(PORT_BASE_ADDR((x >> 4)) + 1) &= ~PIN_MASK(x)))
#define OUT(x, v)               (v ? (_SFR_IO8(PORT_BASE_ADDR((x >> 4)) + 2) |= PIN_MASK(x)) : (_SFR_IO8(PORT_BASE_ADDR((x >> 4)) + 2) &= ~PIN_MASK(x)))


#endif
