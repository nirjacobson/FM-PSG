#define BAUD      57600
#define ECHO      true

#include "global.h"

#include <string.h>
#include <stdio.h>
#include <util/delay.h>

#include "usart.h"

#define     LED     0xB2

int main() {
    char name[20];
    char message[50];

    DIR(LED, 1);

    usart_init(BAUD);

    usart_send_str("Hello! What is your name? ");

    OUT(LED, 1);
    usart_receive_str(name, ECHO);
    usart_send_line(NULL);

    sprintf(message, "Hello, %s!", name);

    usart_send_line(message);
}
