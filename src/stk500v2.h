#ifndef STK500V2_H
#define STK500V2_H

#include "global.h"

#define SIGRD 5
#include <avr/boot.h>

#include "usart.h"
#include "flash.h"

#define STK500V2_START 0x1B
#define STK500V2_TOKEN 0x0E

#define STK500V2_CMD_SIGN_ON                0x01
#define STK500V2_CMD_GET_PARAMETER          0x03
#define STK500V2_CMD_LOAD_ADDRESS           0x06
#define STK500V2_CMD_ENTER_PROGMODE_ISP     0x10
#define STK500V2_CMD_LEAVE_PROGMODE_ISP     0x11
#define STK500V2_CMD_CHIP_ERASE_ISP         0x12
#define STK500V2_CMD_PROGRAM_FLASH_ISP      0x13
#define STK500V2_CMD_READ_FLASH_ISP         0x14
#define STK500V2_CMD_READ_FUSE_ISP          0x18
#define STK500V2_CMD_READ_SIGNATURE_ISP     0x1B

#define STK500V2_STATUS_CMD_OK        0x00

#define STK500V2_PARAM_HW_VER         0x90
#define STK500V2_PARAM_SW_MAJOR       0x91
#define STK500V2_PARAM_SW_MINOR       0x92
#define STK500V2_PARAM_VTARGET        0x94

typedef struct {
  uint32_t load_address;
} STK500V2;

typedef struct {
  uint8_t start;
  uint8_t sequenceNumber;
  uint16_t size;
  uint8_t token;
  uint8_t body[256];
  uint8_t checksum;
} STK500V2_Message;

void stk500v2_init(STK500V2* stk500v2);

void stk500v2_message_init(STK500V2_Message* message);
void stk500v2_receive_message(STK500V2_Message* message);
void stk500v2_send_message(STK500V2_Message* message);

void stk500v2_answer_message(STK500V2* stk500v2, STK500V2_Message* message, STK500V2_Message* answer);
uint8_t stk500v2_message_checksum(STK500V2_Message* message);

#endif // STK500V2_H