#include "usart.h"
#include "global.h"
#include "eeprom.h"

/***********
** �������ƣ�wake_up_in()
**��������������ģ�麯��
**��ڲ��� ��cmd 
SET,Ϊ�ߵ�ƽ
RESETΪ�͵�ƽ
**����ֵ 
***********/
void wake_up_in(u8 cmd)
{
  if(cmd ==SET)
   GPIO_WriteBit(GPIOC , GPIO_Pin_4 ,SET);  //����PB0Ϊ�ߵ�ƽ������ģ��
  else
    GPIO_WriteBit(GPIOC , GPIO_Pin_4 ,RESET);  //����PB0Ϊ�ߵ�ƽ������ģ��
}


/**********************
** ��������: ���ڳ�ʼ��
**������ baudrate ������
**********************/
void UART1_Init(unsigned int baudrate)
{ 
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast); //TXD
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);      //RXD
  
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
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);   //�ȴ����ݷ������
}