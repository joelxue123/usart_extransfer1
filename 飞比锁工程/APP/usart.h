#ifndef __USART_H
#define __USART_H
#include "stm8l15x.h"
#include "global.h"
extern u8 key[];
extern u8 password[6];
extern u8 npass[6];
extern u8 data_buff[50];
 void wake_up_in(u8 cmd);
 void encryption(void);
 u8 crc_8n(unsigned char *ptr,unsigned char n);

#endif