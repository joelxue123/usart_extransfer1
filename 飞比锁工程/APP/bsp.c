#include "stm8l15x.h"
#include "bsp.h"
#include "global.h"

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

/***********
** �������ƣ�wake_up_in()
**��������������ģ�麯��
**��ڲ��� ��cmd  
**GPIO_Mode_In_FL_No_IT
**
**����ֵ 
***********/
void config_wake_up_out(u8 GPIO_Mode)
{
  GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode); //PB0, ��������,���ж� 
}
/*****ģ�黽�Ѻ󣬻�ȡzigbee�Ļ������ŵĸߵ͵�ƽ **********/
u8 get_wakeup_out_pin_state()   //��ȡwakeup_out pin��״̬
{
  return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
}


//////////***********���е��շ������ǶԱ�ģ����ѣ���Ӧ�Է�ģ�������෴��****************//

/***ģ�黽���Ժ󣬻�ȡrx���ŵĵ�ƽ**********/
u8 get_lock_rx_pin()  //��ȡ��ģ��rx����
{
  return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3); //PA3,rx, ��������,���ж�
}
/*** ��������rx���ţ�*********/
u8 config_lock_rx_pin(u8 GPIO_Mode)  //��ȡ��ģ��rx����
{
  GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode);  //PA3,rx, ��������,���ж�
}

/**********
*�������� �� ����ģ��rx���ŵ� ����
*GPIO_Mode_Out_PP_High_Fast
********/
void config_lock_tx_pin(u8 GPIO_Mode)
{
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode); //TXD
}
/**********
*�������� �� ����ģ�����ŵĸߵ͵�ƽ
********/
void set_lock_tx_pin(u8 state)
{
  GPIO_WriteBit(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); //TXD
}

/*******************************************************************************
**�������ƣ�void TIM2_Init()     Name: void TIM2_Init()
**������������ʼ����ʱ��2
**��ڲ�������
**�������
*******************************************************************************/
void TIM2_Init(u16 prescaler)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //ʹ�ܶ�ʱ��2ʱ��
  TIM2_TimeBaseInit(TIM2_Prescaler_1 , TIM2_CounterMode_Up , prescaler);    //���ö�ʱ��2Ϊ1��Ƶ�����ϼ���������ֵΪ16000��Ϊ1����ļ���ֵ
}

/*******************************************************************************
**�������ƣ�void IOInit()     Name: void IOInit()
**������������ʼ������LED��IO��
**��ڲ�������
**�������
*******************************************************************************/
void IOInit()
{
  GPIO_DeInit(GPIOC);  /*  ��ʼ��LED1*/
  GPIO_Init(GPIOC , GPIO_Pin_1 , GPIO_Mode_Out_PP_High_Fast);  //��ʼ��LED1������PC_1Ϊ�������ģʽ
  
  GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); //TX_REMAP
  GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);      //RX_REMAP
  GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast); //TXD
  GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);      //RXD
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); //Wakeup_IN  
  GPIO_WriteBit(GPIOB , GPIO_Pin_0 ,RESET);  //Wakeup_OUT
}

/*******************************************************************************
**�������ƣ�LED2
**������������ʼ������LED��IO��
**��ڲ�������
**�������
*******************************************************************************/
void LED2(u8 cmd)
{
  if(cmd == RESET)
    GPIO_WriteBit(GPIOC , GPIO_Pin_1 ,RESET);  //����PB1����͵�ƽ ��ʹLED2����  
  else
    GPIO_WriteBit(GPIOC , GPIO_Pin_1 ,SET);  //����PB1����͵�ƽ ��ʹLED2����  
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

void init_pin_interrupt(void)
{
    u32 delay=0; 
    
    disableInterrupts();
    USART1->CR2=0;//disable transmision and receiver
    USART_Cmd(USART1 , DISABLE);
    SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC , DISABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)����
    SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA , DISABLE); //��USART1����ģ��Ĭ�ϵ�PC5(TX) , PC6(RX)���ţ���ӳ�䵽PA2(TX) ,PA3(RX)
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , DISABLE); //ʹ��USART1ʱ��

    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_IT); //PC4, ��������,���ж� 
    GPIO_Init(GPIOC,GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast); //PC5,tx,�͵�ƽ���
    
    GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_In_FL_IT); //PA3,rx, ��������,���ж�
    GPIO_Init(GPIOA,GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); //PA2,tx,�͵�ƽ���
    
    for(delay=0;delay<5000;delay++);//��ʱһ������ȴ�IO���ȶ�
    LED2(OFF);
    LED3(OFF);
    EXTI->CR1=0xff;
    EXTI->CR2=0xff;
    EXTI->CR3=0XFF;
    EXTI->SR1=0XFF;
    EXTI->SR2=0XFF;  
    enableInterrupts();	
    
    halt();  //����˯��
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_No_IT); //PC4, ��������,���ж�   
}

/**********************
** ��������: ���
**********************/
void led(void)
{
   static u8 led_on=0;
   static  u16 led_timer=0;
   if(t_1ms)
   {
    led_timer++;
    
    if(led_timer == 1000){
      led_on=1-led_on;
      led_timer=0;
    }
    if(led_on)
       LED2(RESET); //���� LED
    else
       LED2(SET);   //�ر�LED
   }
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
}


