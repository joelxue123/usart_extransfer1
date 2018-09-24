#include "protocol.h"
#include "global.h"
#include "userdata.h"

/**************************************************************************
**函数名称：void crc_8n()     Name: void crc_8n()
**功能描述 检验和
**入口参数：无
**输出：无
**************************************************************************/

u8 crc_8n(unsigned char *ptr,unsigned char n)
{
	unsigned char j=8;
	unsigned char cbit,out;
	unsigned char crc=0x79,crc_a=0,crc_b=0;	
	//*取移位的位*/	
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
** 函数名称：sum_check()
**功能描述：和校验
**入口参数 ： char * ，u8 len
**返回值  检验和 sum
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
** 函数名称：xor_check()
**功能描述：和校验
**入口参数 ： char * ，u8 len
**返回值  异或检验和 xor
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
** 函数名称：encryption()
**功能描述：密码加密
**入口参数 ：
**返回值 
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
**函数描述 ： zigbee收到的数据协议,组合成锁的协议形式,只用作命令应答
**********/
void  lock_protocal_component_ACK(u8 cmd,u8 state)
{
    BFCT_protocol_Lock.send_data[0]=0x02;      //帧头
    
    BFCT_protocol_Lock.send_data[1]=0x02;      //长度
    BFCT_protocol_Lock.send_data[2]=0x00;     //长度
    BFCT_protocol_Lock.send_data[3]=cmd;     //命令字
    BFCT_protocol_Lock.send_data[4]=state;   //状态
    BFCT_protocol_Lock.send_data[5]=crc_8n(BFCT_protocol_Lock.send_data,5);  //检验
    BFCT_protocol_Lock.send_data[6]=0xff;     //长度   
}

/**********
*函数名称：lock_protocal_component
**函数描述 ：组帧，lock协议
**参数描述： buf:储存组好的帧，len:数据长度+命令子 cmd:协议命令 。data_buf :数据内容
**********/
u8  lock_protocal_component(u8*buf, u16 len,u8 cmd,u8* data_buff)
{
    u16 i;
    u8 send_len=0;
    
    buf[0]=0x02;      //帧头
    buf[1]=len &0xFf;      //长度
    buf[2]=(len >> 8)&0xFf;     //长度
    buf[3]=cmd;     //命令字
    for(i=0;i<len-1;i++)
    {
       buf[4+i] = data_buff[i];
    }
    buf[len+3]=crc_8n(buf,len+3);  //检验
    buf[len+4]=0xff;     //帧尾
    send_len =5+len;
    return send_len;
}

/**********
*函数名称：zigbee_protocal_component
**函数描述 ：组帧，zigbee协议
**参数描述： buf:储存组好的帧，len:数据长度+命令子 cmd:协议命令 。data_buf :数据内容
**********/
u8  zigbee_protocal_component(u8 *buf,u16 len,u8 cmd,u8* data_buff)
{
  u8 i;
  buf[0]=0xA1;      //帧头
  buf[1]=cmd;      //有效长度
  buf[2]=(len >>8) &0xff;      //命令字
  buf[3]=len & 0xff;      //命令字自增ID
  
  for(i=0;i<len-1;i++){
     buf[4+i]=data_buff[i];      //应答  
  }
  buf[3+len]=sum_check(buf,len+3);     //长度
  return (len+4); 
}

/**********
**函数描述 ：  南京模块 回复 ACK命令
**********/
void reply_ACK(BFCT_protocol *BFCT_protocol_Zigbee)
{
  u8 *data_buff;
  BFCT_protocol_Zigbee->send_data = zigbee_protocal_component(BFCT_protocol_Zigbee->send_data,0x01,ZIGBEE_CMD_ACK,data_buff);  //
  send_hex(BFCT_protocol_Zigbee->send_data,5); // 发送数据
}



/**********
**函数描述 ： 南京模块 回复 ACK 流程
**********/
u8 reply2zigbee(BFCT_protocol *BFCT_protocol_Zigbee)
{
    static u16 delay=0;
    //  lock_protocal_component_ACK(ACK_ZIGBEE_CMD_LOCAL_OPEN_LOCK,SUCCESS);    //强制开锁回复命令
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
*函数名 transfer2_zigbee_cmd
*函数描述： 返回zigbee模块的命令,并保存用户数据
*参数： lock_cmd ：锁命令 。
*返回值： zigbee命令
**********/

u8 transfer2_zigbee_cmd(u8 lock_cmd,BFCT_protocol *BFCT_protocol_Lock)
{
  u8 zigbee_cmd; 
    
  if(lock_cmd==LOCK_OPENNET)
  { 
    zigbee_cmd = ZIGBEE_CMD_OPENNET;
  }
  else if(lock_cmd==LOCK_CLOSENET)  //开锁回复
  {
    zigbee_cmd = ZIGBEE_CMD_CLOSENET;
  }
  else if(lock_cmd==ACK_LOCK_OPENLOCK)  //开锁回复
  {
     zigbee_cmd = 0;  //暂时没做
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
    zigbee_cmd =0; //暂时没做
   }
   else if(lock_cmd==0x89)
   {
    zigbee_cmd =0; //暂时没做
   } 
   else if(lock_cmd==LOCK_ADD_USER_FROM_WIRELESS)
   {  
    zigbee_cmd =0; //暂时没做
   }
   else if(lock_cmd==LOCK_LOGOUT_SETTING_MODE)
   {  
    zigbee_cmd =0; //暂时没做
   }
   else if(lock_cmd==LOCK_ADD_USER)
   {  
    write_userdata2eeprom(lock_user_No_addr,&(BFCT_protocol_Lock->receive_data[5]),40);//保存用户数据到eeprom中
    zigbee_cmd =ZIGBEE_CMD_ADDUSER_FROM_LOCK;//添加用户
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
    zigbee_cmd =0; //暂时没做
   }
  return zigbee_cmd; 
}

/******
*函数名 zigbee_transfer2_lock_cmd
*函数描述： 返回锁的命令,并保存zigbee模块数据
*参数： zigbee_cmd ：锁命令 。  BFCT_protocol_Zigbee :zigbee结构体，从结构体中获得用户数据
*返回值： zigbee命令
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
  else if(zigbee_cmd==ZIGBEE_CMD_ADD_USER)  //开锁回复
  {
    
    lock_cmd = LOCK_ADD_USER;
  }
  else if(zigbee_cmd==ACK_LOCK_OPENLOCK)  //开锁回复
  {
     lock_cmd = 0;  //暂时没做
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
    lock_cmd =0; //暂时没做
   }
   else if(zigbee_cmd==0x89)
   {
    lock_cmd =0; //暂时没做
   } 
   else if(zigbee_cmd==LOCK_ADD_USER_FROM_WIRELESS)
   {  
    lock_cmd =0; //暂时没做
   }
   else if(zigbee_cmd==LOCK_LOGOUT_SETTING_MODE)
   {  
    lock_cmd =0; //暂时没做
    
   }
   else if(zigbee_cmd==LOCK_ADD_USER)
   {  
    write_userdata2eeprom(lock_user_No_addr,&(BFCT_protocol_Lock->receive_data[5]),40);//保存用户数据到eeprom中
    zigblock_cmdee_cmd =ZIGBEE_CMD_ADDUSER_FROM_LOCK;//添加用户
   }
   else if(zigbee_cmd==ACK_ZIGBEE_CMD_UPLOAD)
   {  
    lock_cmd = LOCK_LOCK_BE_OPENED ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_CLOCK_SYNC)
   {  
     /********保存同步时间的值************/
    write_userdata2eeprom(zigbee_clock_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_clock_len);
    lock_cmd = LOCK_TIME_CALIBRATION ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_INQURE_INFO)
   {  
     /********保存用户属性***********/  //用户信息复杂，得单独开一个进程 
    write_userdata2eeprom(zigbee_user_attribute_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_user_attribute_len);
    lock_cmd = LOCK_LOCK_BE_OPENED ;
   }
   else if(zigbee_cmd==ZIGBEE_CMD_INQURE_LOCK_STATE)
   {  
     /********保存用户属性***********/
    write_userdata2eeprom(zigbee_user_attribute_addr,&(BFCT_protocol_Zigbee.receive_data[4]),zigbee_user_attribute_len);
    lock_cmd = LOCK_CMD_INQURE_LOCK_STATE ;
   }
   else
   {
    lock_cmd =0; //暂时没做
   }
  return lock_cmd; 
}



ZIGBEE_CMD_INQURE_INFO

/***********
**查询用户信息流程
*************/
void inqure_userinfo()
{
  
  switch(zigbee_moni_state)
  {
    case 0:
        user_attribute = BFCT_protocol_Zigbee.receive_data[4];
        write_userdata2eeprom(zigbee_user_attribute_addr,temp,zigbee_user_attribute_len);  //写zigbee用户属性 
        
        BFCT_protocol_Lock.receive_enable =0;  //关闭锁串口接受函数
        BFCT_protocol_Zigbee.receive_enable =1; //打开zigbee串口接受函数
        lock_moni_state++;
        delay=0;
        data_buff[0]=0;
        data_buff[1]=0;
        data_buff[1]=0;
      break;
    case 1:
        BFCT_protocol_Lock.send_len = lock_protocal_component(buf,0X01,ZIGBEE_CMD_OPENNET,data_buff); //
        BFCT_protocol_Lock.receive_enable =1;  //关闭锁串口接受函数
        BFCT_protocol_Zigbee.receive_enable =0; //打开zigbee串口接受函数
      break;
    case 1:
        if(BFCT_protocol_Lock.receive_flag==1)
        {
          BFCT_protocol_Lock.receive_flag==0;
          data_len = BFCT_protocol_Lock.receive_data[1] + (BFCT_protocol_Lock.receive_data[2] << 8); //获取数据长度
          if(BFCT_protocol_Lock.receive_data[data_len +3] != crc_8n(BFCT_protocol_Lock.receive_data,data_len +3) )
          {  /**********数据校验错误 ************/
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
*函数名 make_zigbeeframe
*函数描述： 返回zigbee模块的命令
*参数： lock_cmd ：锁命令 。
*返回值： 1 成功 。 0： 失败
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
          BFCT_protocol_Lock.receive_enable =0;  //关闭锁串口接受函数
          BFCT_protocol_Zigbee.receive_enable =1; //打开zigbee串口接受函数
          lock_moni_state++;
          delay=0;
        break;
      case 1:
          if(BFCT_protocol_Zigbee.receive_flag==1)
          {
            data_len = (BFCT_protocol_Zigbee.receive_data[2] << 8) + BFCT_protocol_Zigbee.receive_data[3];
            if(BFCT_protocol_Zigbee.receive_data[data_len +3] != sum_check(BFCT_protocol_Zigbee.receive_data,data_len +3) )
            { /**********数据校验错误 ************/
              data_len =0 ;
              zigbee_erro =  zigbee_erro | sum_check_BIT;
              return 1;
            }
            if( BFCT_protocol_Zigbee.receive_data[ZIGBEE_CMD_ADDR] != ZIGBEE_CMD_ACK)   //获取锁的命令 != ZIGBEE_CMD_ACK)
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
    else if(zigbee_cmd==ZIGBEE_CMD_CLOSENET)  //开锁回复
    {
     send_len = zigbee_protocal_component(buf,0X01,ZIGBEE_CMD_CLOSENET,data_buff);
    }
    else if(zigbee_cmd==ZIGBEE_CMD_OPENLOCK_BYFORCE)
    {
    send_len = zigbee_protocal_component(buf,10,ZIGBEE_CMD_OPENLOCK_BYFORCE,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(zigbee_cmd==ACK_ZIGBEE_CMD_LOW_POWER){  /*  上传数据 */
     send_len = zigbee_protocal_component(buf,0x01,ACK_ZIGBEE_CMD_LOW_POWER,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(zigbee_cmd== 35)
    { 
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //读取用户属性
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //将锁用户属性转化成zigbee用户属性
    data_buff[1] = 0x01;  //用户编号
    data_buff[2] = 0x01;  // 开锁方式
    data_buff[3] = 0x01;  //  密码编号 
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
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //读取用户属性
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //将锁用户属性转化成zigbee用户属性
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
*函数名 make_lockframe
*函数描述： 组帧  lock  
*参数： lock_cmd ：锁命令 。
*返回值：帧长度
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
    else if(lock_cmd==ZIGBEE_CMD_CLOSENET)  //开锁回复
    {
     send_len = zigbee_protocal_component(buf,0X01,ZIGBEE_CMD_CLOSENET,data_buff);
    }
    else if(lock_cmd==ZIGBEE_CMD_OPENLOCK_BYFORCE)
    {
    send_len = zigbee_protocal_component(buf,10,ZIGBEE_CMD_OPENLOCK_BYFORCE,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(lock_cmd==ACK_ZIGBEE_CMD_LOW_POWER){  /*  上传数据 */
     send_len = zigbee_protocal_component(buf,0x01,ACK_ZIGBEE_CMD_LOW_POWER,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0  
    }
    else if(lock_cmd== 35)
    { 
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //读取用户属性
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //将锁用户属性转化成zigbee用户属性
    data_buff[1] = 0x01;  //用户编号
    data_buff[2] = 0x01;  // 开锁方式
    data_buff[3] = 0x01;  //  密码编号 
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
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //读取用户属性
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //将锁用户属性转化成zigbee用户属性
      send_len = zigbee_protocal_component(buf,0x05,ACK_ZIGBEE_CMD_UPLOAD,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }
    else if(lock_cmd==ACK_ZIGBEE_CMD_UPLOAD)
    {  
      read_userdata4eeprom(lock_user_attribute_addr,temp,1);  //读取用户属性
      lockdata_2_zigbeedata(lock_user_attribute,temp,&data_buff[0]); //将锁用户属性转化成zigbee用户属性
      send_len = zigbee_protocal_component(buf,0x05,ACK_ZIGBEE_CMD_UPLOAD,data_buff); //data_buff = 0 0 0 0 0 0 0 0 0 0 0 0 
    }  
    else
    {
      send_len = 0;
    }
    return send_len;
}

/******
*函数名 if_next_zigbee_cmd
*函数描述： 返回zigbee模块的命令
*参数： lock_cmd ：锁命令 。
*返回值： 1 成功 。 0： 失败
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
