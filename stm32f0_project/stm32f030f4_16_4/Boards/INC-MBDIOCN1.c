#include "INC-MBDIOCN1.h"
#include "delay.h"

//SENSOR_PIN	PA5
//LED_PIN			PA4

#define KeyS_time  8
uint8_t AutoToggle=0;
uint8_t AutoStream=0;

uint8_t KeyS1=0;
uint8_t KeyS2=0;

	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
void EXTI5_Config(void);

void EXTI5_Disable(void);

void INC_MBDIOCN1_hw_init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);			
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);	
	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
		printf("controller init ok\r\n");
	
	  printf("please send commands end with\\r\\n and you will receive data end with \\r\\n>>\r\nex:");
	
		INC_MBDIOCN1_CMDList();

}
uint8_t Command_INC_MBDIOCN1(char* string)
{
	uint8_t result = REPLY_OK;

	if( !strcmp(string,"sensor")) 
	{
		printf("<%d>\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)? 0:1);
		return REPLY_OK;
	}		
	else if( !strcmp(string,"auto toggle enable")) 
	{
		AutoToggle=1;
		
		//EXTI5_Config();
		
		printf("<mode:print message when the status change>\r\n");
		return REPLY_OK;
	}	
	else if( !strcmp(string,"auto toggle disable")) 
	{
		AutoToggle=0;
		//EXTI5_Disable();
		printf("<disbale auto toggle ok>\r\n");
		return REPLY_OK;
	}	
	else if( !strcmp(string,"auto stream enable")) 
	{
		AutoStream=1;
		printf("<mode:print status per 100ms>\r\n");
		return REPLY_OK;
	}		
	else if( !strcmp(string,"auto stream disable")) 
	{
		AutoStream=0;
		printf("<disbale auto stream ok>\r\n");
		return REPLY_OK;
	}		
	else result = REPLY_INVALID_CMD;
	return result;
}
void INC_MBDIOCN1_CMDList(void)
{
	printf("\r\n");
	
	printf("help                    ---Print commands list\r\n");
	 
	printf("sensor                  ---Get data in polling mode\r\n");	
	
	printf("auto toggle enable      ---Get data once when the sensor status changes\r\n");	
	
	printf("auto toggle disable     ---disable toggle mode\r\n");
	
	printf("auto stream enable      ---Stream sensor status per 100ms\r\n");
	
	printf("auto stream disable     ---disable stream mode\r\n");
	
	
}

void AutoTask_per_ms(uint16_t nms)
{
	static U32_T AutoTaskDelay =0;

	static uint8_t PreStatus=0;	static uint8_t SecStatus=0;
	
	static uint8_t StatusCNT=0;	static uint8_t StatusCNT1=0;
	
	static uint8_t StreamCNT=0;


	if(AutoToggle==1)
	{
			//if(StatusCNT==0) {PreStatus=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);	StatusCNT=1;}
			
			if(StatusCNT++<KeyS_time )
			{ 
					if(PreStatus!=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)) StatusCNT=0;
			}	
			else
			{
				if(StatusCNT1==0) {SecStatus=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);	StatusCNT1=1;}
			
				if(StatusCNT1++<KeyS_time )
				{ 
						if(SecStatus==PreStatus) StatusCNT1=0;
				}	
				else
				{
					printf("sensor<%d>\r\n>>",SecStatus? 0:1);
				
					StatusCNT=0;	
					StatusCNT1=0;		
					
					PreStatus=SecStatus;				
				}
				
//				if(PreStatus!=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))
//				
//				if(PreStatus!=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))
//				{
////					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)	printf("lid close\r\n");	
////					else printf("lid open\r\n");	
////					printf("sensor<%d>\r\n>>",SecStatus? 0:1);
//					printf("sensor<%d>\r\n>>",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)? 0:1);
//					StatusCNT=0;	
//					StatusCNT1=0;				
//				}
			}			
		}	
  if(abs((int)(SysTick_GetTimer()-AutoTaskDelay))>nms)
  {	
  	AutoTaskDelay=SysTick_GetTimer();	
		
		if(AutoStream==1)
		{
			if(StreamCNT++>=10)
			{
				StreamCNT=0;
				printf("sensor<%d>\r\n>>",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)? 0:1);
			}
			
		}	
//		if(AutoToggle==1)
//		{
//			if(StatusCNT==0) PreStatus=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
//			
//			StatusCNT++;
//			
//			if(StatusCNT==3 )
//			{
//				StatusCNT=0;		
//				if(PreStatus!=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5))
//				{
////					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)	printf("lid close\r\n");	
////					else printf("lid open\r\n");	
//					printf("sensor<%d>>>\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)? 0:1);
//				}
//			}			
//		}	
  }	
}
//
void EXTI5_Config(void)
{

  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void EXTI5_Disable(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
}



//
