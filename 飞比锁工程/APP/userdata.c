#include "userdata.h"
#include "eeprom.h"


/**********
*�������ƣ�write_userdata2eeprom
*�����û����ݵ�EEPROM
*������addr�� eeprom��д��ĵ�ַ 
* prt : Ҫд������ݻ�����
*len :Ҫд��ĳ���
*���أ� ��
***********/

void write_userdata2eeprom(u16 addr, u8 *ptr, u8 len)
{
  EEPROM_multbyte_write(addr , ptr,len);
}


/**********
*��������:read_userdata4eeprom
*��EEPROM��ȡ�û���Ϣ��ptr
*������addr�� eeprom�ж���ĵ�ַ 
* prt : Ҫ���������ݻ�����
*len :Ҫ�����ĳ���
*���أ���
***********/

void read_userdata4eeprom(u16 addr, u8 *ptr, u8 len)
{
  EEPROM_multbyte_read(addr,ptr, len);
}


/****1���ֽ�BCD��תHEX *********/

u8 u8_BCD_2_hex(u8 data)
{
  return ( ((data >> 4) & 0x0f) *10 + (data & 0x0f) );
}

/***2���ֽ�BCD��תHEX *********/

unsigned char u16_BCD_2_hex(short data)
{
  return ( u8_BCD_2_hex(data & 0x00ff) +u8_BCD_2_hex( (data>>8) & 0x00ff) *100);
}

/****1���ֽ�HEX��תBCD *********/

u16 u8_HEX_2_BCD(u8 data)
{
  return ( ((data / 100) << 8) + ( ((data %100) /10) <<4) + (data %10) );
}


/**********
*�������ƣ�lockdata_2_zigbeedata
*����;�����û�������Ϣ��ת����zigbee�û���Ϣ
**********/
void lockdata_2_zigbeedata( u8 lockdata_type,u8 *lockdata,u8 *zigbeedata)
{
  u16 temp ;
  u8 i;
  switch(lockdata_type)
  {
  case lock_user_No:
      temp =*((u16*)lockdata);
      *zigbeedata=u16_BCD_2_hex(temp);
    break;
  case lock_username:
      for(i=0;i<8;i++)
      {
        *(lockdata+i) = *(zigbeedata+i);
      }
    break;  
    
  case lock_user_attribute:
    
      if(*lockdata & 0x01)
       *zigbeedata = 0X01; //��ͨ�û�
      else
       *zigbeedata = 0X00; //����Ա����
    
    break;
  case 100:
    break;
    
  default:
    break;
    
  }
}

/**********
*�������ƣ�zigbeedata_2_lockdata
*����;��zigbee�û�������Ϣ��ת�������û���Ϣ
**********/
void zigbeedata_2_lockdata(u8 zigbeedata_type,u8 *lockdata,u8 *zigbeedata)
{
  u16 temp;
  switch(zigbeedata_type)
  {
    case zigbee_user_No:
      temp = u8_HEX_2_BCD(*zigbeedata);
      *((u16*)lockdata) =temp;
    break;
    case 
      
    
    
  default:
    break;
    
  }
}







