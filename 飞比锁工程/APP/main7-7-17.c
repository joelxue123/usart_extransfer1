#include "stm8l15x.h"
#include "global.h"


u8 key[]={0x46,0x45,0x49,0x42,0x47};
u8 password[6]={1,2,3,4,5,6};
u8 npass[6];

u8 G_receive_data[100]={0xAA ,0x0A, 0x40, 00, 00, 00, 0x01, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xE1, 0x55};
u8 G_send_data[100]={0xAA ,0x0A, 0x40, 00, 00, 00, 0x01, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 0xE1, 0x55};

BFCT_protocol BFCT_protocol_Zigbee;
BFCT_protocol BFCT_protocol_Lock;

u8 data3_tc;
u8 timeout_flag;
u8 t_1ms;

u8 zigbee_send_cmd;

#define ON 1
#define OFF 1
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

/*******************************************************************************
**�������ƣ�void TIM2_Init()     Name: void TIM2_Init()
**������������ʼ����ʱ��2
**��ڲ�������
**�������
*******************************************************************************/
void TIM2_Init()
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //ʹ�ܶ�ʱ��2ʱ��
  TIM2_TimeBaseInit(TIM2_Prescaler_1 , TIM2_CounterMode_Up , 16000);    //���ö�ʱ��2Ϊ1��Ƶ�����ϼ���������ֵΪ16000��Ϊ1����ļ���ֵ
}

/*******************************************************************************
**�������ƣ�void TIM2_DelayMs(unsigned int ms)
**������������ʱ��2�ν��о�ȷ��ʱ����СΪ1���룬���65535
**��ڲ�����unsigned int ms     1=< ms <= 65535 
**�������
*******************************************************************************/
void TIM2_DelayMs(unsigned int ms)
{ 
  TIM2_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���    
  TIM2_Cmd(ENABLE);               //������ʱ��2��ʼ����
  while(ms--)
  {
    while( TIM2_GetFlagStatus(TIM2_FLAG_Update) == RESET); //�ȴ������Ƿ�ﵽ1����
    TIM2_ClearFlag(TIM2_FLAG_Update);  //�������1���룬�����Ӧ�ı�־
  }
  TIM2_Cmd(DISABLE);                   //��ʱȫ���������رն�ʱ��2
}
/*******************************************************************************
**�������ƣ�void All_LEDInit()     Name: void All_LEDInit()
**������������ʼ������LED��IO��
**��ڲ�������
**�������
*******************************************************************************/
void All_LEDInit()
{
  GPIO_Init(GPIOB , GPIO_Pin_2 , GPIO_Mode_Out_PP_Low_Fast);  //��ʼ��LED1������PB_2Ϊ�������ģʽ
  GPIO_Init(GPIOB , GPIO_Pin_1 , GPIO_Mode_Out_PP_Low_Fast);  //��ʼ��LED2������PB_1Ϊ�������ģʽ
  GPIO_Init(GPIOB , GPIO_Pin_6 , GPIO_Mode_Out_PP_Low_Fast);  //��ʼ��LED3������PB_6Ϊ�������ģʽ
}
/*******************************************************************************
**�������ƣ�LED2
**������������ʼ������LED��IO��
**��ڲ�������
**�������
*******************************************************************************/
void LED2(u8 cmd)
{
  if(cmd == ON)
    GPIO_WriteBit(GPIOB , GPIO_Pin_1 ,RESET);  //����PB1����͵�ƽ ��ʹLED2����  
  else
    GPIO_WriteBit(GPIOB , GPIO_Pin_1 ,SET);  //����PB1����͵�ƽ ��ʹLED2����  
}
/*******************************************************************************
**�������ƣ�LED3
**������������ʼ������LED��IO��
**��ڲ�������
**�������
*******************************************************************************/
void LED3(u8 cmd)
{
    if(cmd == ON)
    GPIO_WriteBit(GPIOB , GPIO_Pin_6 ,RESET);  //����PB1����͵�ƽ ��ʹLED3����  
  else
    GPIO_WriteBit(GPIOB , GPIO_Pin_6 ,SET);  //����PB1����͵�ƽ ��ʹLED3���� 
}

/**********************
** ��������: ���ڳ�ʼ��
**������ baudrate ������
**********************/
void UART1_Init(unsigned int baudrate)
{ 
    //GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); //TXD
  SYSCFG_REMAPDeInit();
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC , ENABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)���ţ���ӳ�䵽PA2(TX) ,PA3(RX)
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE); //ʹ��USART1ʱ��
  USART_Init(USART1,                //����USART1
            baudrate,               //����������
            USART_WordLength_8b,    //���ݳ�����Ϊ8λ
            USART_StopBits_1,       //1λֹͣλ
            USART_Parity_No,        //��У��
            (USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART1 , ENABLE);
}

/**********************
** ��������: ��������ӳ��
**������ baudrate ������
**********************/
void UART1_RemapInit(unsigned int baudrate)
{ 
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); //TXD
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);      //RXD
  
  SYSCFG_REMAPDeInit();
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA , ENABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)���ţ���ӳ�䵽PA2(TX) ,PA3(RX)
  
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE); //ʹ��USART1ʱ��

  USART_Init(USART1,                //����USART1
            baudrate,               //����������
            USART_WordLength_8b,    //���ݳ�����Ϊ8λ
            USART_StopBits_1,       //1λֹͣλ
            USART_Parity_No,        //��У��
            (USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
   
  USART_Cmd(USART1 , ENABLE);
  
}
/**********************
** ��������: ���ڷ�������
**������ char *p ���͵�����, u8 len ���͵��ֽ���
**********************/
void send_hex(char *p,u8 len)
{
  u8 i;
  for(i=0;i<len;i++){
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);       
    USART_SendData8(USART1 , (u8)p[i]);             //���ͼĴ���д������     
  }
}

/**********
**�������� �� zigbee�յ�������Э��,��ϳ�����Э����ʽ,ֻ��������Ӧ��
**********/
void  lock_protocal_component_ACK(u8 cmd,u8 state)
{
    BFCT_protocol_Lock.send_data[0]=0x02;      //֡ͷ
    
    BFCT_protocol_Lock.send_data[1]=0x06;      //����
    BFCT_protocol_Lock.send_data[2]=0x00;     //����
    BFCT_protocol_Lock.send_data[3]=cmd;     //������
    BFCT_protocol_Lock.send_data[4]=state;      //״̬
    BFCT_protocol_Lock.send_data[5]=crc_8n(BFCT_protocol_Lock.send_data,5);  //����
    BFCT_protocol_Lock.send_data[6]=0xff;     //����
    
}
/**********
**�������� ��zigbee�յ�������Э��,��ϳ�����Э����ʽ,ֻ��������
**********/
void  lock_protocal_component(u16 len,u8 cmd,u8* data_buff)
{
    u16 i;
    BFCT_protocol_Lock.send_data[0]=0x02;      //֡ͷ
    
    BFCT_protocol_Lock.send_data[1]=len &0x0f;      //����
    BFCT_protocol_Lock.send_data[2]=(len >> 4)&0x0f;     //����
    BFCT_protocol_Lock.send_data[3]=cmd;     //������
    for(i=0;i<len-1;i++)
    {
       BFCT_protocol_Lock.send_data[4+i] = data_buff[i];
    }
    BFCT_protocol_Lock.send_data[len+4]=crc_8n(BFCT_protocol_Lock.send_data,len+3);  //����
    BFCT_protocol_Lock.send_data[len+5]=0xff;     //֡β
    
    send_hex(BFCT_protocol_Lock.send_data,BFCT_protocol_Lock.send_len+6); // ��������
}


/***
** �������zigbeЭ��
**/
void  zigbee_protocal_component(u8 len,u8 cmd,u32 cmd_id,u8* data_buff)
{
  u8 i;
  BFCT_protocol_Zigbee.send_data[0]=0xAA;      //֡ͷ
  BFCT_protocol_Zigbee.send_data[1]=len;      //��Ч����
  BFCT_protocol_Zigbee.send_data[2]=cmd;      //������
  BFCT_protocol_Zigbee.send_data[3]=cmd_id & 0x0f;      //����������ID
  BFCT_protocol_Zigbee.send_data[4]=(cmd_id >> 4) & 0x0f;      //����������ID
  BFCT_protocol_Zigbee.send_data[5]=(cmd_id >> 8) & 0x0f;     //����������ID
  BFCT_protocol_Zigbee.send_data[6]=(cmd_id >> 12) & 0x0f;    //����������ID
  BFCT_protocol_Zigbee.send_data[7]=0x00;      //Ӧ��
  
  for(i=0;i<len;i++){
     BFCT_protocol_Zigbee.send_data[8+i]=data_buff[i];      //Ӧ��  
  }
  BFCT_protocol_Zigbee.send_data[8+len]=xor_check(BFCT_protocol_Zigbee.send_data,len+8);     //����
  BFCT_protocol_Zigbee.send_data[9+len]=0x55;      //����û������ֶ�
  BFCT_protocol_Zigbee.send_len =len+10; 
}


/**********
**�������� �� **ģ��Э�����
**********/
void lock_protocal_analysis(void)
{

  
}
/**********
**�������� �� ��zigbee�յ�������Э�� ת�г� ��Э��
**********/
void zigbee_convert2lock(void)
{


}


/**********
**�������� �� ʱ�������,1ms����ж�
**********/
void os_time(void) 
{
    if( ( TIM2_GetFlagStatus(TIM2_FLAG_Update) != RESET) ) //�ȴ������Ƿ�ﵽ1����
    {
      t_1ms=1;
      TIM2_ClearFlag(TIM2_FLAG_Update);
    }else t_1ms=0;
  //////////////////////////////////////////////// ʵʱʱ��
 
 
  
}

/**********************
** ��������: ���
**********************/
void led(void)
{
   static u8 led_on=0;
   static  u16 timer=0;
   if(t_1ms)
   {
    timer++;
    
    if(timer == 2000){
      led_on=1-led_on;
      timer=0;
    }
    
    if(led_on)
      GPIO_WriteBit(GPIOB , GPIO_Pin_2 ,RESET);  //����PB2����͵�ƽ ��ʹLED1����  
    else
      GPIO_WriteBit(GPIOB , GPIO_Pin_2 ,SET);  //����PB2����͵�ƽ ��ʹLED1���� 
   }
}

void init_pin_interrupt(void)
{
  disableInterrupts();
 // GPIO_Init(GPIOC,GPIO_Pin_6,GPIO_Mode_In_FL_IT); //PC6,rx, ��������,���ж�
  GPIO_Init(GPIOC,GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast); //PC5,tx,�͵�ƽ���
   GPIO_WriteBit(GPIOC , GPIO_Pin_5 ,SET);  //����PB2����͵�ƽ ��ʹLED1���� 
  
  GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_In_FL_IT); //PA3,rx, ��������,���ж�
  GPIO_Init(GPIOA,GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PA2,tx,�͵�ƽ���
  
    EXTI->CR1=0xff;
    EXTI->CR2=0xff;
    EXTI->CR3=0XFF;
    EXTI->SR1=0XFF;
    EXTI->SR2=0XFF;  
    enableInterrupts();	

    halt();
    
}

void main()
{
   static  u16 timer=0;
  disableInterrupts();   //�ر�ϵͳ���ж�
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); //�ڲ�ʱ��Ϊ1��Ƶ = 16Mhz 
  All_LEDInit();
  TIM2_Init();
  TIM2_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���    
  TIM2_Cmd(ENABLE); 
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); //TXD
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);      //RXD
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast); //TXD
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);      //RXD
  
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast); //TXD  
  
  UART1_Init(57600);
  enableInterrupts();   //ʹ��ϵͳ���ж�

  BFCT_protocol_Zigbee.receive_enable = 1;
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
 //  led();  //���
   if(t_1ms)
   {
        timer++;
        
        if(timer ==800)
        {
          GPIO_WriteBit(GPIOB , GPIO_Pin_0 ,SET);  //����PB2����͵�ƽ ��ʹLED1���� 
        }
        if(timer ==830)
        {
          
          timer =0;
            SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC , ENABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)����
            send_hex(G_send_data,20);
            GPIO_WriteBit(GPIOB , GPIO_Pin_0 ,RESET);  //����PB2����͵�ƽ ��ʹLED1���� 
        }
     }
   if(BFCT_protocol_Zigbee.receive_flag == 1)
   {
    
     SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA , ENABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)���ţ���ӳ�䵽PA2(TX) ,PA3(RX)
     send_hex(G_receive_data,BFCT_protocol_Zigbee.receive_len);
     BFCT_protocol_Zigbee.receive_enable =1;
     BFCT_protocol_Zigbee.receive_flag =0;
     BFCT_protocol_Zigbee.receive_len =0;
   
   }
   os_time();   //ϵͳʱ��

  }
  
}