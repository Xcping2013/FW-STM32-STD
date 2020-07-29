#include "user_test.h"
#include "delay.h"

void Uart1Task_test(uint16_t nms)
{
	static U32_T KeyTaskDelay =0;

  if(abs((int)(SysTick_GetTimer()-KeyTaskDelay))>nms)
  {	
		printf("=+1.23@\r\n");
  	KeyTaskDelay=SysTick_GetTimer();		
		
		for(;;)
		{
			printf("motor moveto 2 -3000\r");	delay_ms(20);	
			printf("printdata\r");						delay_ms(1000);		
			printf("output on 8\r");					delay_ms(20);
			printf("output off 8\r");					delay_ms(20);
			printf("motor moveto 2 -6000\r");	delay_ms(20);	
			printf("printdata\r");						delay_ms(1000);		
			printf("output on 8\r");					delay_ms(20);
			printf("output off 8\r");					delay_ms(20);
		}
  }	
}

