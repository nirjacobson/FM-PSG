#include "usart.h"

#define UBRR(baud)    (F_CPU / 16 / baud - 1)

void usart_init(uint16_t baud) {
    uint16_t ubrr = UBRR(baud);
    UBRR0 = ubrr;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void usart_send(char c) {
    while (!(UCSR0A & (1 << UDRE0))) ;

    UDR0 = c;
}

char usart_receive(bool echo) {
    while (!(UCSR0A & (1 << RXC0))) ;

    const char c = UDR0;

    if (echo)
        usart_send(c);

    return c;
}

bool usart_try_receive(bool echo, char* c) {
    if (UCSR0A & (1 << RXC0)) {
        *c = UDR0;

        if (echo)
            usart_send(*c);

        return true;
    }

    return false;
}
void usart_send_dec(long value) {
    if (value >= 10) {
        usart_send_dec(value / 10);
    }
    usart_send('0'+(value % 10));
}

void usart_send_str(char* str) {
    for (size_t i = 0; i < strlen(str); i++)
        usart_send(str[i]);
}

void usart_send_line(char* str) {
    usart_send_str(str);
    usart_send_str("\r\n");
}

void usart_receive_str(char* str, bool echo) {
    size_t i;
    for (i = 0; (str[i] = usart_receive(echo)) != '\r'; i++) ;
    str[i] = '\0';
}
