#include "lcomp_e75_test.h"

/**************************************************************************************/

#define E75_VBUS_PORT	GPIOB
#define E75_VBUS_Pin	GPIO_Pin_1

static void LCOMP_E75_Init(void);

/**************************************************************************************/	
static void LCOMP_E75_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
  GPIO_InitStruct.GPIO_Pin = E75_VBUS_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
  GPIO_Init(E75_VBUS_PORT, &GPIO_InitStruct);
}
/**************************************************************************************/
//return 0---correct cmd
//return 1---error   cmd
uint8_t Command_analysis_LCOMP(char* string)
{
	uint8_t result = REPLY_OK;
	if(!strcmp("cable debug",string ))
	{
		static uint8_t lcomp_inited=0;
		if(lcomp_inited==0)
		{
			LCOMP_E75_Init();
			lcomp_inited=1;
		}
		if(GPIO_ReadInputDataBit(E75_VBUS_PORT,E75_VBUS_Pin)==0)
		{
				 LCOMP_TRACE("fail---------------------------电源线路接线有问题\r\n>>");
		}
		else LCOMP_TRACE("pass---------------------------线材测试ok\r\n>>");
	}	
	else if(!strcmp("lcomp help", string))	
	{
		printf("\r\nlcomp command list:\r\n");
				
		LCOMP_TRACE("cable debug-------------lcomp e75 cable test\r\n");
	}		
	else result = REPLY_INVALID_CMD;
	return result;
}
