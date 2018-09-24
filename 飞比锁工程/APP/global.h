#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "stm8l15x.h"

#define ON 1
#define OFF 0

#define Wakeup_OUT_PORT GPIOB  
#define Wakeup_OUT_PIN  GPIO_Pin_0

#define Wakeup_IN_PORT GPIOC
#define Wakeup_IN_PIN  GPIO_Pin_4

#define TX_REMAP_PORT GPIOA 
#define TX_REMAP_PIN GPIO_Pin_2


extern u8 G_receive_data[];
extern u8 G_send_data[];

typedef struct
{
    u8 receive_len;
    u8* receive_data;
    u8 send_len;
    u8* send_data; 
    u8 receive_flag;
    u8 receive_fail;
    u8 send_fail;
    u8 receive_enable;
    u8 command;
} BFCT_protocol;




/* ����ȫ�ֱ����ĵ�ַ��eeprom*/
#define battery_percent_addr  0x1000   


extern BFCT_protocol BFCT_protocol_Lock;


#define SUCCESS  0x00
#define FAIL     0xff

#define UPLOAD   0x00  
#define DOWNLOAD 0X01


/******��Э������׼ ***********/
#define crc_8n_ERRO_BIT 0X01
#define CMD_LOST_BIT    0X02
#define LOCK_BUSY_BIT   0X04
/******��Э������׼ ***********/


/******ZigbeeЭ������׼ ***********/
#define sum_check_BIT       0X01
#define NACK_BIT           0x02
/******ZigbeeЭ������׼ ***********/

extern u8 lock_interrupt;
extern u8 zigbee_interrupt ;

extern u8 Zigbee_process_done;
extern u8 Lock_process_done;

extern u8 Zigbee_processing_flag ;
extern u8 Lock_processing_flag ;

extern u8 t_1ms;

#endif



