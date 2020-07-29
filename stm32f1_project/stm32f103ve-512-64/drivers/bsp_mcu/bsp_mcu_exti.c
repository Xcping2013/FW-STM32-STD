/**
  \Project		IN3-SMC-3
	\file 			IO.c
  \author 		Xcping2016
  \version 		1.00

  \brief I/O functions

  This file provides functions for intializing and
  using the I/O ports and some other miscellaneous
  stuff.
*/

#include "bsp_mcu_exti.h"
#include "app_gpio_dido.h"
#include "bsp_mcu_systick.h"
#include "app_button.h"
#include "app_serial.h"
#include "app_motor_home.h"
#include "app_sys_control.h"

//PE2 input8
void EXTI_Init_IN8(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);

  EXTI_InitStructure.EXTI_Line=EXTI_Line2;	
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
	NVIC_Init(&NVIC_InitStructure); 
	
	printf("init input8 as exti...ok\r\n");
}
volatile  uint8_t u8PressAlarm=FALSE;
//增加调试打印命令
uint8_t printStr5times=0;
void EXTI2_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE0上的中断标志位  
	
	if(INPUT8==0 )//&& buttonRESETpressed==0 && homeInfo.GoHome[2]==FALSE)	 	
	{		
		u8PressAlarm = TRUE;		
		UART3_DisableRX();

		if(homeInfo.GoHome[2]==TRUE && homeInfo.HomeSpeed[2]<0)
		{
			TMC429_MotorRotate(2,homeInfo.HomeSpeed[2]);			//向上		
		}
		else
		{
			HardStop(2);	
			printf("stop motor z due to pressure overhigh!!!\r\n");	
		}
	} 
	if(INPUT8==1 )						//Alarm signal remove
	{			
		u8PressAlarm = FALSE;
		//printf("pressure alarm signal remove\r\n");	
	} 
	
}









