/**
  \file SysTick.c

  \version 1.00

  \brief 1ms system tick timer functions
*/

#include "bsp_mcu_delay.h"

void drv_udelay(uint32_t nus)
{
    int i = (SystemCoreClock / 4000000 * nus);
    while (i)
    {
        i--;
    }
}
void drv_mdelay(uint16_t nms)
{
	drv_udelay(nms*1000);							
}

void delay_us(uint32_t nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//1ms定时需要的LOAD值 	T 
	ticks=nus*9; 											//  		 
	tcnt=0;

	told=SysTick->VAL;        			  //刚进入时的计数器值		T0
	while(1)
	{
		tnow=SysTick->VAL;							//其他时刻的实时数值	  T1
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//正常T0-T1=△T1
			else tcnt+=reload-tnow+told;	//如T0=1	T1=9000	△T1=T0+T-T1   
			told=tnow;										//△T=△T1+△Tn
			if(tcnt>=ticks)break;				  //△T时间超过/等于要延迟的时间,则退出.
		}  
	};						    
}

void delay_ms(u16 nms)
{	
	drv_mdelay(nms);
	//delay_us((u32)(nms*1000));				
}

