#include "stm8l15x.h"
#include "global.h"
#include "eeprom.h"
#include "usart.h"
#include "protocol.h"

u8 key[]={0x46,0x45,0x49,0x42,0x47};
u8 password[6]={1,2,3,4,5,6};
u8 npass[6];
u8 data_buff[50]={0};

u8 G_receive_data[100]={0xAA ,0x0A, 0x40, 00, 00, 00, 0x01, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xE1, 0x55};
u8 G_send_data[100]={0xAA ,0x0A, 0x40, 00, 00, 00, 0x01, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xE1, 0x55};

BFCT_protocol BFCT_protocol_Zigbee;
BFCT_protocol BFCT_protocol_Lock;

u8 continue_usart_data=0;  /* 续传标志位 */
u8 data3_tc;
u8 timeout_flag;
u8 t_1ms;

u8 zigbee_send_cmd;

u8 lock_interrupt =0;
u8 zigbee_interrupt =0;

u8 Zigbee_process_done =1;
u8 Lock_process_done =1;

u8 Zigbee_processing_flag =0;
u8 Lock_processing_flag =0;

u8 Lock_process_timeout=0;
u8 Zigbee_process_timeout=0;
u8 lock_state =0;
u8 zigbee_state =0;
u8 zigbee_moni_state=0;
u8 lock_moni_state=0;

/**********
**函数描述 ： 南京模块 回复 ACK 流程
**********/
u8 zigbee_usart_send(void)
{
    static u16 delay=0;
    if(t_1ms){
      delay++;
      if(delay ==2)
          wake_up_in(SET);
      if(delay ==20)   
          wake_up_in(RESET);
      if(delay ==40)
      {
        enableInterrupts();    //似乎没啥用，忘记了为啥放到这里了
        UART1_Init(ZIGBEE_BAUD); //PC5,PC6为串口,与zigbee模块通讯
        wake_up_in(SET);
        send_hex(BFCT_protocol_Zigbee.send_data,BFCT_protocol_Zigbee.send_len); // 发送数据

      } 
      if(delay ==42)
      {
        delay =0;
        BFCT_protocol_Zigbee.receive_enable =1; //使能zigbee模块 接收功能
        BFCT_protocol_Lock.receive_enable =0;   //失能lock串口接收 
        BFCT_protocol_Zigbee.receive_len =0;
        BFCT_protocol_Zigbee.receive_flag =0;
        return 1;
      }
      if(delay > 100)
      {
        delay =0;
      }
    }  
   return 0;
}  

/**********************
*锁发送函数
*
************************/
u8 lock_usart_send(void)
{
   static u16 delay=0;

   
    if(t_1ms){
       delay++;
       if(1==delay)
       {
          USART_Cmd(USART1 , DISABLE);
          SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC , DISABLE); //把USART1串口模块默认的PC5(TX) , PC6(RX)引脚
          SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA , DISABLE); //把USART1串口模块默认的PC5(TX) , PC6(RX)引脚，重映射到PA2(TX) ,PA3(RX)
          CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , DISABLE); //使能USART1时钟
          GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //TXD
          GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);      //RXD
          
          GPIO_WriteBit(GPIOA , GPIO_Pin_2 ,RESET);  //设置PA2,唤醒锁通讯
       }
       
       if(20 ==delay)
       {
          GPIO_WriteBit(GPIOA , GPIO_Pin_2 ,SET);  //设置PA2,唤醒锁通讯
       } 
       if(delay < 1000 && delay > 20)
       {
          if(  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) )
          {
              UART1_RemapInit(9600);//波特率 设置成 9600
              send_hex(BFCT_protocol_Lock.send_data,BFCT_protocol_Lock.send_len); // 发送数据
              BFCT_protocol_Lock.receive_enable =1;
              delay =0;
              for(delay=0;delay<1000;delay++);
              delay=0;  
              return 1;
          }
       }
       if(delay > 1000)
       {
          delay=0;
          return 2;
       }
    }
       return 0;
}

/**********
**函数描述 ： 讲zigbee收到的数据协议 转行成 锁协议
**********/
u8 zigbee_convert2lock(void)
{
    u8 i;
    u16 data_len;
    u8 lock_cmd=0；
    static u16 delay=0;
    static u8 erro_num=0;
    static u8 check_timeout=0;
    /************
    **南京模块,协议应答格式

    ***************/
    /****
    将zigbee解析后的数据，填充到lock的数据区中
    ****/
  
  switch(zigbee_state)
  {
    case 0:
        BFCT_protocol_Lock.receive_enable =0;  //关闭锁串口接受函数
        BFCT_protocol_Zigbee.receive_enable =0; //关闭zigbee串口接受函数
        
        data_len = (BFCT_protocol_Zigbee.receive_data[2] << 8) + BFCT_protocol_Zigbee.receive_data[3];
        BFCT_protocol_Zigbee.command=BFCT_protocol_Zigbee.receive_data[ZIGBEE_CMD_ADDR]; //获取锁的命令
        if(BFCT_protocol_Zigbee.receive_data[data_len +3] != sum_check(BFCT_protocol_Zigbee.receive_data,data_len +3) )
        { /**********数据校验错误 ************/
          data_len =0 ;
          zigbee_erro =  zigbee_erro | sum_check_BIT;
          return 1;
        }
        zigbee_state ++;
    break;
    case 1:
        ret = exe_zigbee_cmd(BFCT_protocol_Zigbee.command);
        if(ret)
        {
          if(zigbee_erro ==0 && lock_erro==0 )
          {
            zigbee_state =10;
          }
          else
          {
            while(1);
          }
        }
    break;

    case 10；
       Zigbee_process_done =1;
       zigbee_state =0;
       delay =0;
       return 1;
     
      break;
    default: 
    break;
    
  }
  
  return 0;
}


/***
**从锁中读回来的协议，转换成zigbee模式
**/
u8 lock_convert2zigbee(void)
{
  u16 data_len;
  static u16 delay=0;
  static u32 cmd_id=1;
  u8 data_buff[20]={0};
  static u8 erro_num=0;

  switch(lock_state )
  {
    case 0:
        BFCT_protocol_Lock.receive_enable =0;  //关闭锁串口接受函数
        BFCT_protocol_Zigbee.receive_enable =0; //关闭zigbee串口接受函数
        data_len = BFCT_protocol_Lock.receive_data[1] + (BFCT_protocol_Lock.receive_data[2] << 8); //获取数据长度
        lock_cmd=BFCT_protocol_Lock.receive_data[LOCK_CMD_ADDR]; //获取锁的命令
        if(BFCT_protocol_Lock.receive_data[data_len +3] != crc_8n(BFCT_protocol_Lock.receive_data,data_len +3) )
        {    /** 数据校验错误**/
          data_len =0 ;
          lock_erro =  lock_erro | crc_8n_ERRO_BIT;
          return 1;
        }
        data_len =0;
        lock_state ++;
    break;
    

   case 2:
      ret = exe_lock_cmd(lock_cmd);
      if(ret)
      {
      if(zigbee_erro ==0 && lock_erro==0 )
      {
        lock_state =10;
      }
      else
      {
        while(1);
      }
      }
     break;
      
    case 10:
      lock_state =0;
      delay =0;
      sys_timer = 999;
      Lock_process_done=1;
      return 1;
    break;
    
    default:
    break;
  }
  return 0;
}

/**********
**函数描述 ： 接收zigbee数据处理
**********/
void USART_process(void)
{
    u16 DataLen;
    u8 res;
    static u8 delay=0;
/********************
    *串口超时函数处理
    **********************/
    if( t_1ms==1 ) //10ms定时器,定时器每10ms,进入此函数
    {
      data3_tc++;
      if( data3_tc > 5 ) //超时增加到 300ms,停止计数
      {
        data3_tc=5;  //此为全局变量，需要谨慎使用
      }
      if( data3_tc == 4 ) //超时280ms的时候,表示接受终止
      {
        if( (BFCT_protocol_Zigbee.receive_enable == 1) && (BFCT_protocol_Zigbee.receive_flag != 1)
           BFCT_protocol_Zigbee.receive_flag = 1;
        if( (BFCT_protocol_Lock.receive_enable == 1) && (BFCT_protocol_Lock.receive_flag != 1)
           BFCT_protocol_Zigbee.receive_flag = 1;          
        
      }
    }
/****************
 *判断串口接收完成接收完成
*****************/
    if(BFCT_protocol_Zigbee.receive_flag == 1)
    { 
      data3_tc =5;
      if(Lock_processing_flag ==0)/****只允许一个协议进入主流程******/
      {
        Zigbee_processing_flag =1;  //进入zigbee流程  
        BFCT_protocol_Zigbee.receive_flag =0;
      }
    }
    else if(BFCT_protocol_Lock.receive_flag==1)
    {
      data3_tc =5;
      if(Zigbee_processing_flag ==0)/****只允许一个协议进入主流程******/
      {
        Lock_processing_flag  = 1; //进行锁处理流程
        BFCT_protocol_Lock.receive_flag =0; 
      }
    }  

/****************
 *Zigbee处理函数流程
*****************/
    if(Zigbee_processing_flag ==1)
    {
     res = zigbee_convert2lock();
     if(res==1)
     {
       if(zigbee_erro)
       {
         sys_timer =999;
       }
        Zigbee_processing_flag =0;
        Zigbee_process_done =1;
     }
    }
/****************
 *锁处理函数流程
*****************/
    if(Lock_processing_flag ==1)
    {
      res =lock_convert2zigbee();
      if(res==1)
      {
       if(lock_erro)
       {
         sys_timer =999;
       }
        Lock_processing_flag=1;
        Lock_process_done=1;
      }
    }
/**************
    *锁串口发送函数
***********/
    if(BFCT_protocol_Lock.send_len)
    {
      res = lock_usart_send();
     if(res==1)
     {
      BFCT_protocol_Lock.send_len =0;
     }  
     else if(res ==2)
     {
      BFCT_protocol_Lock.send_fail=1;
      BFCT_protocol_Lock.send_len =0;
     }
    }
/**************
    *zigbee串口发送函数
***********/
    if(BFCT_protocol_Zigbee.send_len)
    {
      res =zigbee_usart_send();
     if(res ==1)
     {
      BFCT_protocol_Zigbee.send_len =0;
     }  
    }
}
             
/*********主函数流程 *****************/             

void main()
{
   static  u16 timer=0;
   static u16 sys_timer=0;
  disableInterrupts();   //关闭系统总中断
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); //内部时钟为1分频 = 16Mhz 
  IOInit();
  TIM2_Init(16000);
  TIM2_ARRPreloadConfig(ENABLE);  //使能定时器2自动重载功能    
  TIM2_Cmd(ENABLE); 


  
  UART1_Init(ZIGBEE_BAUD);
  enableInterrupts();   //使能系统总中断

  BFCT_protocol_Zigbee.receive_enable = 0;
  BFCT_protocol_Zigbee.receive_flag = 0;
  BFCT_protocol_Zigbee.receive_data = G_receive_data ;
  BFCT_protocol_Zigbee.send_data = G_send_data ;
  
  BFCT_protocol_Lock.receive_enable=0;
  BFCT_protocol_Lock.receive_len =0;
  BFCT_protocol_Lock.receive_flag = 0;
  BFCT_protocol_Lock.receive_data = G_receive_data ;
  BFCT_protocol_Lock.send_data = G_send_data ;
  
  while(1)
  {

   led();  //点灯
   
   USART_process();
   
   if((Zigbee_process_done && Lock_process_done) | (sys_timer >= 10000) |(Zigbee_process_timeout) |(Lock_process_timeout) )
   {
      lock_state=0;
      zigbee_state =0;
      lock_state =0;
      sys_timer=0;
      
      zigbee_erro =0;
      lock_erro =0;
      
      Zigbee_process_done =0;  //清除串口完成标志位
      Lock_process_done =0;
      
      Zigbee_processing_flag =0;  //清除串口处理使能函数
      Lock_processing_flag =0;
      
      BFCT_protocol_Lock.receive_enable=0;  //清除串口接收标志位
      BFCT_protocol_Lock.receive_len =0;
      
      Zigbee_process_timeout =0;  //超时标志位清零
      Lock_process_timeout=0;
      
      USART1->CR2=0;//disable transmision and receiver
      USART_Cmd(USART1 , DISABLE);
      SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC , DISABLE); //把
      SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA , DISABLE); //把
      CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , DISABLE); //使能USART1时钟
      
      init_pin_interrupt();//进入低功耗模式
      disableInterrupts();
      
      if(lock_interrupt)
      {
        if(t_1ms){ 
          delay++;
          if(delay ==1)
          config_lock_rx_pin(GPIO_Mode_In_FL_No_IT); //配置唤醒引脚为 无中断的浮空输入 ，此引脚为本模块唤醒引脚
          if(delay >0) && (delay < 5)
          {
            if( get_lock_rx_pin()){}
            else 
            {
              delay =0;
              sys_timer = 10000;
            }
               
          }
          if(delay ==5)
          {
            lock_interrupt =0;
            UART1_RemapInit(9600);//波特率 设置成 9600
            config_lock_tx_pin(GPIO_Mode_Out_PP_High_Fast); //TXD
            BFCT_protocol_Lock.receive_enable=1;
            BFCT_protocol_Zigbee.receive_enable=0;
            BFCT_protocol_Lock.receive_len =0;
            delay =0；
          }
        }
        

      }
       
      else if(zigbee_interrupt)
      {   
        if(t_1ms){ 
          delay++;
          if(delay ==1)
          wake_up_out(GPIO_Mode_In_FL_No_IT); //配置唤醒引脚为 无中断的浮空输入 
          if(delay >0) && (delay < 5)
          {
            if( get_wakeup_out_pin_state()){}
            else 
            {
              delay=0;
              sys_timer = 10000;
            }
               
          }
          if(delay ==5)
          {
            zigbee_interrupt = 0;
            UART1_Init(ZIGBEE_BAUD);//波特率 设置成 9600
            BFCT_protocol_Lock.receive_enable=0;
            BFCT_protocol_Zigbee.receive_enable =1;
            BFCT_protocol_Zigbee.receive_len =0;
            delay =0；
          }
        }
      }
      enableInterrupts();
    }
    if(t_1ms)
      sys_timer ++;
    
    os_time();   //系统时钟
  }
  
}