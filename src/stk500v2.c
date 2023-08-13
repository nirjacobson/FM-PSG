#include "stk500v2.h"

void stk500v2_init(STK500V2* stk500v2) {
  stk500v2->load_address = 0;
}

void stk500v2_message_init(STK500V2_Message* message) {
  message->start = STK500V2_START;
  message->sequenceNumber = 0;
  message->size = 0;
  message->token = STK500V2_TOKEN;
  memset(message->body, '0', sizeof(message->body));
  message->checksum = 0;
}

void stk500v2_receive_message(STK500V2_Message* message) {
  stk500v2_message_init(message);

  message->start = usart_receive(false);

  if (message->start == STK500V2_START) {
      message->sequenceNumber = usart_receive(false);

      message->size = usart_receive(false);
      message->size = (message->size << 8) | (usart_receive(false) & 0xFF);

      message->token = usart_receive(false);
      if (message->token == STK500V2_TOKEN) {
          for (uint16_t i = 0; i < message->size; i++) {
              message->body[i] = usart_receive(false);
          }

          message->checksum = usart_receive(false);
      }
  }
}

void stk500v2_send_message(STK500V2_Message* message) {
    usart_send(message->start);
    usart_send(message->sequenceNumber);
    usart_send(message->size >> 8);
    usart_send(message->size & 0xFF);
    usart_send(message->token);

    for (uint16_t i = 0; i < message->size; i++) {
        usart_send(message->body[i]);
    }

    usart_send(message->checksum);
}

void stk500v2_answer_message(STK500V2* stk500v2, STK500V2_Message* message, STK500V2_Message* answer) {
  stk500v2_message_init(answer);
  answer->sequenceNumber = message->sequenceNumber;

  if (message->body[0] == STK500V2_CMD_SIGN_ON) {
        answer->body[0] = STK500V2_CMD_SIGN_ON;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->body[2] = 0x08;
        strncpy((char*)&answer->body[3], "AVRISP_2", 9);
        answer->size = 11;
    } else if (message->body[0] == STK500V2_CMD_GET_PARAMETER) {
        answer->body[0] = STK500V2_CMD_GET_PARAMETER;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        switch (message->body[1]) {
            case STK500V2_PARAM_HW_VER:
                answer->body[2] = 0x01;
                break;
            case STK500V2_PARAM_SW_MAJOR:
                answer->body[2] = 0x01;
                break;
            case STK500V2_PARAM_SW_MINOR:
                answer->body[2] = 0x00;
                break;
            case STK500V2_PARAM_VTARGET:
                answer->body[2] = 50;
                break;
            break;
        }
        answer->size = 3;
    } else if (message->body[0] == STK500V2_CMD_ENTER_PROGMODE_ISP) {
        answer->body[0] = STK500V2_CMD_ENTER_PROGMODE_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->size = 2;
    } else if (message->body[0] == STK500V2_CMD_READ_SIGNATURE_ISP) {
        answer->body[0] = STK500V2_CMD_READ_SIGNATURE_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->body[2] = boot_signature_byte_get(message->body[4] * 2);
        answer->body[3] = STK500V2_STATUS_CMD_OK;
        answer->size = 4;
    } else if (message->body[0] == STK500V2_CMD_READ_FUSE_ISP) {
        answer->body[0] = STK500V2_CMD_READ_FUSE_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        if (message->body[2] == 0x50 && message->body[3] == 0x00) { // lfuse
            answer->body[2] = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
        } else if (message->body[2] == 0x58 && message->body[3] == 0x08) { // hfuse
            answer->body[2] = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
        } else if (message->body[2] == 0x50 && message->body[3] == 0x08) { // efuse
            answer->body[2] = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
        }
        answer->body[3] = 0x00;
        answer->size = 4;
    } else if (message->body[0] == STK500V2_CMD_LEAVE_PROGMODE_ISP) {
        answer->body[0] = STK500V2_CMD_LEAVE_PROGMODE_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->size = 2;
    } else if (message->body[0] == STK500V2_CMD_CHIP_ERASE_ISP) {
        // uint8_t eraseDelay = message->body[1];
        // uint8_t pollMethod = message->body[2];
        flash_erase_chip();
        answer->body[0] = STK500V2_CMD_CHIP_ERASE_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->size = 2;
    } else if (message->body[0] == STK500V2_CMD_LOAD_ADDRESS) {
        stk500v2->load_address =
            ((uint32_t)message->body[1] << 24) |
            ((uint32_t)message->body[2] << 16) |
            ((uint32_t)message->body[3] << 8) |
            ((uint32_t)message->body[4]);
        answer->body[0] = STK500V2_CMD_LOAD_ADDRESS;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->size = 2;
    } else if (message->body[0] == STK500V2_CMD_PROGRAM_FLASH_ISP) {
        uint16_t numBytes = ((uint16_t)message->body[1] << 8) | (uint16_t)message->body[2];
        // uint8_t mode = message->body[3];
        // uint8_t delay = message->body[4];
        uint8_t* page = &message->body[10];
        flash_write_page(stk500v2->load_address * 2, page, numBytes);
        answer->body[0] = STK500V2_CMD_PROGRAM_FLASH_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        answer->size = 2;
    } else if (message->body[0] == STK500V2_CMD_READ_FLASH_ISP) {
        uint16_t numBytes = ((uint16_t)message->body[1] << 8) | (uint16_t)message->body[2];
        // uint8_t cmd1 = message->body[3];
        answer->body[0] = STK500V2_CMD_READ_FLASH_ISP;
        answer->body[1] = STK500V2_STATUS_CMD_OK;
        flash_read_page(stk500v2->load_address * 2, &answer->body[2], numBytes);
        answer->body[numBytes+2] = STK500V2_STATUS_CMD_OK;
        answer->size = numBytes + 3;
    }

    answer->checksum = stk500v2_message_checksum(answer);
}

uint8_t stk500v2_message_checksum(STK500V2_Message* message) {
  uint8_t checksum = message->start;
  checksum ^= message->sequenceNumber;
  checksum ^= (message->size >> 8);
  checksum ^= (message->size & 0xFF);
  checksum ^= message->token;
  for (uint16_t i = 0; i < message->size; i++) {
      checksum ^= message->body[i];
  }

  return checksum;
}