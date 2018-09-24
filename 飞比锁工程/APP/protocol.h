#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "stm8l15x.h"


#define ZIGBEE_BAUD 19200


#define UART_CMD_ADDR 1
#define DATA_ADDR 4


/********* zigbee 模块协议 宏定义 **************/
#define ZIGBEE_CMD_ADDR 0x01

#define CMD_CRC_ERRO 1
#define CMD_LEN_ERRO 1
#define ZIGBEE_CMD_ADD_USER 0x13  
#define ZIGBEE_CMD_ACK 0x00
#define ZIGBEE_CMD_OPENNET_FAIL 0x95
#define ZIGBEE_CMD_OPENNET_SUCEESS 0x93

#define ZIGBEE_CMD_OPENLOCK 0x18
#define ZIGBEE_CMD_OPENLOCK_BYFORCE 0x22
#define ZIGBEE_CMD_LOCAL_OPEN_LOCK 0x80
#define ZIGBEE_CMD_OPENNET 0x40
#define ZIGBEE_CMD_CLOSENET 0x41
#define ZIGBEE_CMD_LOW_POWER 0x30
#define ZIGBEE_CMD_CLOCK_SYNC 0x10

#define ZIGBEE_CMD_INQURE_INFO 0x12
#define ZIGBEE_CMD_INQURE_LOCK_STATE 0x17
#define ZIGBEE_CMD_ADDUSER_FROM_LOCK 0x33

#define ACK_ZIGBEE_CMD_INQURE_LOCK_STATE  0x3B
#define ACK_ZIGBEE_CMD_LOW_POWER 0X30
#define ACK_ZIGBEE_CMD_UPLOAD 0X3D
#define ACK_ZIGBEE_CMD_LOCAL_OPEN_LOCK 0x80
#define ACK_ZIGBEE_CMD_OPENNET_SUCESS 0x40
#define ACK_ZIGBEE_CMD_OPENLOCK 0x54
/********* zigbee 模块协议 宏定义 **************/

/******* 锁协议宏定义 ************************/

#define LOCK_CMD_ADDR 0x03

#define LOCK_LOCK_BE_OPENED 0X15
#define LOCK_OPENNET 0X1C
#define LOCK_CLOSENET 0X1D
#define LOCK_OPENLOCK 0x06
#define LOCK_OPENLOCK_BYFORCE 0x1A
#define LOCK_LOGIN_SAFE_MODE 0x1B
#define LOCK_LOGOUT_SAFE_MODE 0x9B
#define LOCK_LOGOUT_SETTING_MODE 0x26

#define LOCK_ADD_USER 0x21
#define LOCK_MODIFY_USER_2 0x50
#define LOCK_STATE_UPDATA 0X17
#define LOCK_ADD_USER_FROM_WIRELESS 0X01
#define LOCK_TIME_CALIBRATION 0X08
#define LOCK_CMD_INQURE_LOCK_STATE 0x09

#define ACK_LOCK_CMD_INQURE_LOCK_STATE  0X89
#define ACK_LOCK_OPENLOCK 0X86
#define ACK_OPENLOCK_BYFORCE 0X9a
#define ACK_LOCK_OPENNET 0X9c

/******* 锁协议宏定义 ************************/

/******函数对外接口 **********************/








#endif 