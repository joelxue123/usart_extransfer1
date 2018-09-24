#include "eeprom.h"

/******
*������ EEPROM_multbyte_write
*������������EEPROM���ֽ�
*������ start_address ����ʼ��ַ �� 0x1000��ʼ�� 256���ֽ�
*data,д��Ļ�����   �� len :д��ĳ���
**********/
void EEPROM_multbyte_write(u32 start_address , u8 *data,u8 len)
{
  u8 count;
  FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg); //�趨���ʱ��Ϊ��׼���ʱ��
  
 //MASS ��Կ�����EEPROM�ı���
  FLASH_Unlock(FLASH_MemType_Data);
  

  //��һ��д��128���ֽ�����
  for(count = 0; count < len ; count++)
  {
    *((unsigned char *)(start_address + count)) = data[count];
  }
 
  while(FLASH_GetFlagStatus(FLASH_FLAG_WR_PG_DIS) == 1);      //�ȴ���̽��� 
  
  while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == 1);            //�ȴ���̽���
}

/******
*������ EEPROM_multbyte_read
*������������EEPROM���ֽ�
*������ start_address ����ʼ��ַ �� 0x1000��ʼ�� 256���ֽ�
*data,�����Ļ�����   �� len :��ȡ�ĳ���
**********/
void EEPROM_multbyte_read(u32 start_address , u8 *data,u8 len)
{
  u8 count;

  //��N���ֽ�
  for(count = 0; count < len ; count++)
  {
    data[count] = *((unsigned char *)(start_address + count));
  }
        
}










#if 0   /* ���Դ���*/

  EEPROM_multbyte_write(0x1000,"12234",5);
    EEPROM_multbyte_read(0x1000,data_buf,5);
#endif  