#ifndef USART_H
#define USART_H

#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "global.h"

void usart_init(uint16_t baud);
void usart_send(char c);
char usart_receive(bool echo);
bool usart_try_receive(bool echo, char* c);
void usart_send_dec(long value);
void usart_send_str(char* str);
void usart_send_line(char* str);
void usart_receive_str(char* str, bool echo);

#endif // USART_H
