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
	u32 reload=SysTick->LOAD;					//1ms��ʱ��Ҫ��LOADֵ 	T 
	ticks=nus*9; 											//  		 
	tcnt=0;

	told=SysTick->VAL;        			  //�ս���ʱ�ļ�����ֵ		T0
	while(1)
	{
		tnow=SysTick->VAL;							//����ʱ�̵�ʵʱ��ֵ	  T1
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����T0-T1=��T1
			else tcnt+=reload-tnow+told;	//��T0=1	T1=9000	��T1=T0+T-T1   
			told=tnow;										//��T=��T1+��Tn
			if(tcnt>=ticks)break;				  //��Tʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};						    
}

void delay_ms(u16 nms)
{	
	drv_mdelay(nms);
	//delay_us((u32)(nms*1000));				
}

