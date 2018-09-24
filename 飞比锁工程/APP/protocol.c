#include "protocol.h"
#include "global.h"
#include "userdata.h"

/**************************************************************************
**�������ƣ�void crc_8n()     Name: void crc_8n()
**�������� �����
**��ڲ�������
**�������
**************************************************************************/

u8 crc_8n(unsigned char *ptr,unsigned char n)
{
	unsigned char j=8;
	unsigned char cbit,out;
	unsigned char crc=0x79,crc_a=0,crc_b=0;	
	//*ȡ��λ��λ*/	
	do{
		crc_a=*ptr;
		ptr++;
		j=8;
		cbit=1;
		do{
			crc_b=crc_a;
			crc_b=crc_b^crc;
			out=crc_b&cbit;
		//	crc_b=crc_b>>1;
			if(out!=0)
                        {
                                crc=crc^0X18;
                                crc=crc>>1;
                                crc|=0x80;
                        }else
                                crc=crc>>1;
			crc_a=crc_a>>1;
		//	cbit=cbit<<1;
		}while(--j);
	}while(--n);
	return crc;
}

/***********
** �������ƣ�sum_check()
**������������У��
**��ڲ��� �� char * ��u8 len
**����ֵ  ����� sum
***********/

u8 sum_check(char *p,u16 len)
{
  u16 i;
  u8 sum=0;
  for(i=0;i<len;i++)
  {
    sum = sum +p[i];
  }
  return sum;
}
/***********
** �������ƣ�xor_check()
**������������У��
**��ڲ��� �� char * ��u8 len
**����ֵ  ������� xor
***********/

u8 xor_check(char *p,u16 len)
{
  u16 i;
  u8 xor=0;
  for(i=0;i<len;i++)
  {
    xor = xor^p[i];
  }
  return xor;
}

/***********
** �������ƣ�encryption()
**�����������������
**��ڲ��� ��
**����ֵ 
***********/

void encryption(void)
{
  u8 i;
  for(i=0;i<6;i++)
  {
    npass[i] =(u8) key[i] ^  password[i];
  }
}

/**********
**�������� �� zigbee�յ�������Э��,��ϳ�����Э����ʽ,ֻ��������Ӧ��
**********/
void  lock_protocal_component_ACK(u8 cmd,u8 state)
{
    BFCT_protocol_Lock.send_data[0]=0x02;      //֡ͷ
    
    BFCT_protocol_Lock.send_data[1]=0x02;      //����
    BFCT_protocol_Lock.send_data[2]=0x00;     //����
    BFCT_protocol_Lock.send_data[3]=cmd;     //������
    BFCT_protocol_Lock.send_data[4]=state;   //״̬
    BFCT_protocol_Lock.send_data[5]=crc_8n(BFCT_protocol_Lock.send_data,5);  //����
    BFCT_protocol_Lock.send_data[6]=0xff;     //����   
}

/**********
*�������ƣ�lock_protocal_component
**�������� ����֡��lockЭ��
**���������� buf:������õ�֡��len:���ݳ���+������ cmd:Э������ ��data_buf :��������
**********/
u8  lock_protocal_component(u8*buf, u16 len,u8 cmd,u8* data_buff)
{
    u16 i;
    u8 send_len=0;
    
    buf[0]=0x02;      //֡ͷ
    buf[1]=len &0xFf;      //����
    buf[2]=(len >> 8)&0xFf;     //����
    buf[3]=cmd;     //������
    for(i=0;i<len-1;i++)
    {
       buf[4+i] = data_buff[i];
    }
    buf[len+3]=crc_8n(buf,len+3);  //����
    buf[len+4]=0xff;     //֡β
    send_len =5+len;
    return send_len;
}

/**********
*�������ƣ�zigbee_protocal_component
**�������� ����֡��zigbeeЭ��
**���������� buf:������õ�֡��len:���ݳ���+������ cmd:Э������ ��data_buf :��������
**********/
u8  zigbee_protocal_component(u8 *buf,u16 len,u8 cmd,u8* data_buff)
{
  u8 i;
  buf[0]=0xA1;      //֡ͷ
  buf[1]=cmd;      //��Ч����
  buf[2]=(len >>8) &0xff;      //������
  buf[3]=len & 0xff;      //����������ID
  
  for(i=0;i<len-1;i++){
     buf[4+i]=data_buff[i];      //Ӧ��  
  }
  buf[3+len]=sum_check(buf,len+3);     //����
  return (len+4); 
}

/**********
**�������� ��  �Ͼ�ģ�� �ظ� ACK����
**********/
void reply_ACK(BFCT_protocol *BFCT_protocol_Zigbee)
{
  u8 *data_buff;
  BFCT_protocol_Zigbee->send_data = zigbee_protocal_component(BFCT_protocol_Zigbee->send_data,0x01,ZIGBEE_CMD_ACK,data_buff);  //
  send_hex(BFCT_protocol_Zigbee->send_data,5); // ��������
}



/**********
**�������� �� �Ͼ�ģ�� �ظ� ACK ����
**********/
u8 reply2zigbee(BFCT_protocol *BFCT_protocol_Zigbee)
{
    static u16 delay=0;
    //  lock_protocal_component_ACK(ACK_ZIGBEE_CMD_LOCAL_OPEN_LOCK,SUCCESS);    //ǿ�ƿ����ظ�����
    if(t_1ms){
      delay++;
      if(delay > 100)
        delay =0;
      if(delay ==2)
              wake_up_in(SET);
      if(delay ==20)     
              wake_up_in(RESET);
      if(delay ==40)
      {
        reply_ACK(BFCT_protocol_Zigbee);
        BFCT_protocol_Zigbee->receive_len=0;
      }
          
      if(delay ==50)
      {
        delay =0;
        return 1;
      }
      return 0;
    }  
}  
/******
*������ transfer2_zigbee_cmd
*���������� ����zigbeeģ�������,�������û�����
*������ lock_cmd �������� ��
*����ֵ�� zigbee����
**********/

u8 transfer2_zigbee_cmd(u8 lock_cmd,BFCT_protocol *BFCT_protocol_Lock)
{
  u8 zigbee_cmd; 
    
  if(lock_cmd==LOCK_OPENNET)
  { 
    zigbee_cmd = ZIGBEE_CMD_OPENNET;
  }
  else if(lock_cmd==LOCK_CLOSENET)  //�����ظ�
  {
    zigbee_cmd = ZIGBEE_CMD_CLOSENET;
  }
  else if(lock_cmd==ACK_LOCK_OPENLOCK)  //�����ظ�
  {
     zigbee_cmd = 0;  //��ʱû��
  }
   else if(lock_cmd==LOCK_OPENLOCK_BYFORCE)
  {
    zigbee_cmd =ZIGBEE_CMD_OPENLOCK_BYFORCE;
  } 
   else if(lock_cmd==LOCK_STATE_UPDATA){  
    zigbee_cmd =ACK_ZIGBEE_CMD_LOW_POWER;
   }
   else if(lock_cmd==LOCK_LOGIN_SAFE_MODE)
   {
    zigbee_cmd =0; //��ʱû��
   }
   else if(lock_cmd==0x89)
   {
    zigbee_cmd =0; //��ʱû��
   } 
   else if(lock_cmd==LOCK_ADD_USER_FROM_WIRELESS)
   {  
    zigbee_cmd =0; //��ʱû��
   }
   else if(lock_cmd==LOCK_LOGOUT_SETTING_MODE)
   {  
    zigbee_cmd =0; //��ʱû��
   }
   else if(lock_cmd==LOCK_ADD_USER)
   {  
    write_userdata2eeprom(lock_user_No_addr,&(BFCT_protocol_Lock->receive_data[5]),40);//�����û����ݵ�eeprom��
    zigbee_cmd =ZIGBEE_CMD_ADDUSER_FROM_LOCK;//����û�
   }
   else if(lock_cmd==LOCK_LOCK_BE_OPENED)
   {  
    zigbee_cmd = ACK_ZIGBEE_CMD_UPLOAD; 
   }
   else if(lock_cmd==ACK_LOCK_CMD_INQURE_LOCK_STATE)
   {  
     
     
    zigbee_cmd = ACK_ZIGBEE_CMD_INQURE_LOCK_STATE; 
   }
   else
   {
    zigbee_cmd =0; //��ʱû��
   }
  return zigbee_cmd; 
}

/******
*������ zigbee_transfer2_lock_cmd
*���������� ������������,������zigbeeģ������
*������ zigbee_cmd �������� ��  BFCT_protocol_Zigbee :zigbee�ṹ�壬�ӽṹ���л���û�����
*����ֵ�� zigbee����
**********/

u8 zigbee_transfer2_lock_cmd(u8 zigbee_cmd,BFCT_protocol *BFCT_protocol_Zigbee)
{
  u8 lock_cmd; 
    
  if(zigbee_cmd==ZIGBEE_CMD_ACK)
  { 
    lock_cmd = 0; 
  }
  if(zigbee_cmd==ZIGBEE_CMD_OPENNET_SUCEESS)
  { 
    lock_cmd = 0; 
  }
  else if(zigbee_cmd==ZIGBEE_CMD_ADD_USER)  //�����ظ�
  {
    
    lock_cmd = LOCK_ADD_USER;
  }
  else if(zigbee_cmd==ACK_LOCK_OPENLOCK)  //�����ظ�
  {
     lock_cmd = 0;  //��ʱû��
  }
   else if(zigbee_cmd==LOCK_OPENLOCK_BYFORCE)
  {
    lock_cmd =ZIGBEE_CMD_OPENLOCK_BYFORCE;
  } 
   else if(zigbee_cmd==LOCK_STATE_UPDATA){  
    zigbee_cmd =ACK_ZIGBEE_CMD_LOW_POWER;
   }
   else if(zigbee_cmd==LOCK_LOGIN_SAFE_MODE)
   {
    lock_cmd =0; //��ʱû��
   }
   else if(zigbee_cmd==0x89)
   {
    lock_cmd =0; //��ʱû��
   } 
   else if(zigbee_cmd==LOCK_ADD_USER_FROM_WIRELESS)
   {  
    lock_cmd =0; //��ʱû��
   }
   else if(zigbee_cmd==LOCK_LOGOUT_SETTING_MODE)
   {  
    lock_cmd =0; //��ʱû��
    
   }
   else if(zigbee_cmd==LOCK_ADD_USER)
   {  
    write_userdata2eeprom(lock_user_No_addr,&(BFCT_protocol_Lock->receive_data[5]),40);//�����û����ݵ�eeprom��
    zigblock_cmdee_cmd =ZIGBEE_CMD_ADDUSER_FROM_LOCK;//����û�
   }
   else if(zigbee_cmd==ACK_ZIGBEE_CMD_UPLOAD)
   {  
    lock_cmd = LOCK_LOCK_BE_OPENED ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_CLOCK_SYNC)
   {  
     /********����ͬ��ʱ���ֵ************/
    write_userdata2eeprom(zigbee_clock_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_clock_len);
    lock_cmd = LOCK_TIME_CALIBRATION ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_INQURE_INFO)
   {  
     /********�����û�����***********/  //�û���Ϣ���ӣ��õ�����һ������ 
    write_userdata2eeprom(zigbee_user_attribute_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_user_attribute_len);
    lock_cmd = LOCK_LOCK_BE_OPENED ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_INQURE_LOCK_STATE)
   {  
     /********�����û�����***********/
    write_userdata2eeprom(zigbee_user_attribute_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_user_attribute_len);
    lock_cmd = LOCK_CMD_INQURE_LOCK_STATE ;
   }
   else
   {
    lock_cmd =0; //��ʱû��
   }
  return lock_cmd; 
}



ZIGBEE_CMD_INQURE_INFO

/***********
**��ѯ�û���Ϣ����
*************/
void inqure_userinfo()
{
  
  switch(zigbee_moni_state)
  {
    case 0:
        user_attribute = BFCT_protocol_Zigbee.receive_data[4];
        write_userdata2eeprom(zigbee_user_attribute_addr,temp,zigbee_user_attribute_len);  //дzigbee�û����� 
        
        BFCT_protocol_Lock.receive_enable =0;  //�ر������ڽ��ܺ���
        BFCT_protocol_Zigbee.receive_enable =1; //��zigbee���ڽ��ܺ���
        lock_moni_state++;
        delay=0;
        data_buff[0]=0;
        data_buff[1]=0;
        data_buff[1]=0;
      break;
    case 1:
        BFCT_protocol_Lock.send_len = lock_protocal_component(buf,0X01,ZIGBEE_CMD_OPENNET,data_buff); //
        BFCT_protocol_Lock.receive_enable =1;  //�ر������ڽ��ܺ���
        BFCT_protocol_Zigbee.receive_enable =0; //��zigbee���ڽ��ܺ���
      break;
    case 1:
        if(BFCT_protocol_Lock.receive_flag==1)
        {
          BFCT_protocol_Lock.receive_flag==0;
          data_len = BFCT_protocol_Lock.receive_data[1] + (BFCT_protocol_Lock.receive_data[2] << 8); //��ȡ���ݳ���
          if(BFCT_protocol_Lock.receive_data[data_len +3] != crc_8n(BFCT_protocol_Lock.receive_data,data_len +3) )
          {  /**********����У����� ************/
                data_len =0 ;
                lock_erro =  lock_erro | crc_8n_ERRO_BIT;
                zigbee_moni_state=0;
                delay=0;
                return 1;
          }
          
          lockdata_2_zigbeedata(lock_user_attribute,&(BFCT_protocol_Lock.receive_data[16]),&temp)
          
          if(temp == user_attribute)
          {
              
          }

          zigbee_moni_state ++;
         }
       break;
      case 2:
        
        
      break;
      
      case 3:
      break;
       
       
    default:
      break;
  } 
}

/******
*������ make_zigbeeframe
*���������� ����zigbeeģ�������
*������ lock_cmd �������� ��
*����ֵ�� 1 �ɹ� �� 0�� ʧ��
**********/

u8 make_zigbeeframe(u8 *buf,u8 zigbee_cmd)
{
    u8 data_buff[40];
    u8 temp[10];
    u8 send_len=0;
    
    if(t_1ms)
    {
     delay++;
     if(delay > 300){
       lock_moni_state=0;
       delay=0;
       return 1;
     }
    }
         
    if(zigbee_cmd==ZIGBEE_CMD_ACK)
    { 
      return 1;
    }
    else if(zigbee_cmd==ZIGBEE_CMD_OPENNET)
    { 
      switch(zigbee_moni_state)
      {
      case 0:
          zigbee_protocal_component(&BFCT_protocol_Zigbee,0X01,ZIGBEE_CMD_OPENNET,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0
          BFCT_protocol_Lock.receive_enable =0;  //�ر������ڽ��ܺ���
          BFCT_protocol_Zigbee.receive_enable =1; //��zigbee���ڽ��ܺ���
          lock_moni_state++;
          delay=0;
        break;
      case 1:
          if(BFCT_protocol_Zigbee.receive_flag==1)
          {
            data_len = (BFCT_protocol_Zigbee.receive_data[2] << 8) + BFCT_protocol_Zigbee.receive_data[3];
            if(BFCT_protocol_Zigbee.receive_data[data_len +3] != sum_check(BFCT_protocol_Zigbee.receive_data,data_len +3) )
            { /**********����У����� ************/
              data_len =0 ;
              zigbee_erro =  zigbee_erro | sum_check_BIT;
              return 1;
            }
            if( BFCT_protocol_Zigbee.receive_data[ZIGBEE_CMD_ADDR] != ZIGBEE_CMD_ACK)   //��ȡ�������� != ZIGBEE_CMD_ACK)
            {
              data_len =0 ;
              zigbee_erro =  zigbee_erro | NACK_BIT;
              return 1;
            }
             lock_moni_state=0;
             return 1;
          }
         break;
      default:
        break;
      } 
    }
    else if(zigbee_cmd==ZIGBEE_CMD_CLOSENET)  //�����ظ�
    {
     send_len = zigbee_protocal_component(buf,0X01,ZIGBEE_CMD_CLOSENET,data_buff);
    }
    else if(zigbee_cmd==ZIGBEE_CMD_OPENLOCK_BYFORCE)
    {
    send_len = zigbee_protocal_component(buf,10,ZIGBEE_CMD_OPENLOCK_BYFORCE,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(zigbee_cmd==ACK_ZIGBEE_CMD_LOW_POWER){  /*  �ϴ����� */
     send_len = zigbee_protocal_component(buf,0x01,ACK_ZIGBEE_CMD_LOW_POWER,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(zigbee_cmd== 35)
    { 
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //��ȡ�û�����
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //�����û�����ת����zigbee�û�����
    data_buff[1] = 0x01;  //�û����
    data_buff[2] = 0x01;  // ������ʽ
    data_buff[3] = 0x01;  //  ������ 
    send_len = zigbee_protocal_component(buf,0X0b,35,data_buff); // 
    }
    else if(zigbee_cmd==ZIGBEE_CMD_ADDUSER_FROM_LOCK)
    {  
    data_buff[0]=0X01;
    data_buff[1]=0X01;
    send_len = zigbee_protocal_component(buf,0X03,ZIGBEE_CMD_ADDUSER_FROM_LOCK,data_buff); //
    
    }
    else if(zigbee_cmd==ACK_ZIGBEE_CMD_UPLOAD)
    {  
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //��ȡ�û�����
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //�����û�����ת����zigbee�û�����
      send_len = zigbee_protocal_component(buf,0x05,ACK_ZIGBEE_CMD_UPLOAD,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }
    else if(zigbee_cmd==ZIGBEE_CMD_INQURE_INFO)
    {  

    }
    else
    {
      send_len = 0;
    }
    return send_len;
}

/******
*������ make_lockframe
*���������� ��֡  lock  
*������ lock_cmd �������� ��
*����ֵ��֡����
**********/

u8 make_lockframe(u8 *buf,u8 lock_cmd)
{
    u8 data_buff[40];
    u8 temp[10];
    u8 send_len=0;
    
    if(lock_cmd==LOCK_OPENLOCK)
    { 
      send_len = lock_protocal_component(buf,0X01,ZIGBEE_CMD_OPENNET,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }
    else if(lock_cmd==ZIGBEE_CMD_CLOSENET)  //�����ظ�
    {
     send_len = zigbee_protocal_component(buf,0X01,ZIGBEE_CMD_CLOSENET,data_buff);
    }
    else if(lock_cmd==ZIGBEE_CMD_OPENLOCK_BYFORCE)
    {
    send_len = zigbee_protocal_component(buf,10,ZIGBEE_CMD_OPENLOCK_BYFORCE,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(lock_cmd==ACK_ZIGBEE_CMD_LOW_POWER){  /*  �ϴ����� */
     send_len = zigbee_protocal_component(buf,0x01,ACK_ZIGBEE_CMD_LOW_POWER,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(lock_cmd== 35)
    { 
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //��ȡ�û�����
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //�����û�����ת����zigbee�û�����
    data_buff[1] = 0x01;  //�û����
    data_buff[2] = 0x01;  // ������ʽ
    data_buff[3] = 0x01;  //  ������ 
    send_len = zigbee_protocal_component(buf,0X0b,35,data_buff); // 
    }
    else if(lock_cmd==ZIGBEE_CMD_ADDUSER_FROM_LOCK)
    {  
    data_buff[0]=0X01;
    data_buff[1]=0X01;
    send_len = zigbee_protocal_component(buf,0X03,ZIGBEE_CMD_ADDUSER_FROM_LOCK,data_buff); //
    
    }
    else if(lock_cmd==ACK_ZIGBEE_CMD_UPLOAD)
    {  
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //��ȡ�û�����
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //�����û�����ת����zigbee�û�����
      send_len = zigbee_protocal_component(buf,0x05,ACK_ZIGBEE_CMD_UPLOAD,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }
    else if(lock_cmd==ACK_ZIGBEE_CMD_UPLOAD)
    {  
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //��ȡ�û�����
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //�����û�����ת����zigbee�û�����
      send_len = zigbee_protocal_component(buf,0x05,ACK_ZIGBEE_CMD_UPLOAD,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }  
    else
    {
      send_len = 0;
    }
    return send_len;
}

/******
*������ if_next_zigbee_cmd
*���������� ����zigbeeģ�������
*������ lock_cmd �������� ��
*����ֵ�� 1 �ɹ� �� 0�� ʧ��
**********/

u8 if_next_zigbee_cmd(zigbee_cmd)
{
    u8 next_zigbee_cmd;
      
    next_zigbee_cmd =0;

    if(zigbee_cmd==ZIGBEE_CMD_ADDUSER_FROM_LOCK)
    {  
      next_zigbee_cmd =35;
    }
    else if(zigbee_cmd==ACK_ZIGBEE_CMD_UPLOAD)
    {  
     if(get_battery_lever() < LOW_BATTERY_LEVEL)
       next_zigbee_cmd = ACK_ZIGBEE_CMD_LOW_POWER;
    }
    else
    {
      next_zigbee_cmd=0;
    }
    return next_zigbee_cmd;
}
