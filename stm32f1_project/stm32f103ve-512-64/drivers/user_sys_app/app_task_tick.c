
#include "app_task_tick.h"

#include "bsp_mcu_exti.h"
#include "app_gpio_dido.h"
#include "bsp_mcu_systick.h"
#include "app_button.h"
#include "app_serial.h"
#include "app_motor_home.h"

//
void KeyTask_every_ms(uint16_t nms)
{
	static U32_T KeyTaskDelay =0;

  if(abs((int)(SysTick_GetTimer()-KeyTaskDelay))>nms)
  {	
		if(g_tParam.Project_ID!=AOI_2AXIS || g_tParam.Project_ID!=COMMON || g_tParam.Project_ID!=LIDOPEN)
		{
			buttonSTART_process(1,1);
			buttonRESET_process(2,2);	
		}
  	KeyTaskDelay=SysTick_GetTimer();		
  }	
}
//报警信号后续会继续保持,所以不能自锁,要一直判断
void PressAlarmTask_every_ms(uint16_t nms)
{
	static U32_T PressAlarmTaskDelay =0;
	
	static uint8_t IN8_ONCnt=0;

  if(abs((int)(SysTick_GetTimer()-PressAlarmTaskDelay))>nms)
  {	
		if(g_tParam.Project_ID!=AOI_2AXIS || g_tParam.Project_ID!=COMMON || g_tParam.Project_ID!=LIDOPEN)
		{
			if(INPUT8==0 && Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) > 0 )
			{
				IN8_ONCnt++; 	
				if(IN8_ONCnt>=2) 
				{
					HardStop(2);
					IN8_ONCnt=0;
					printf("stop motor z due to pressure overhigh!!!\r\n");	
				}
			}
			else if(INPUT8==1)
			{
				IN8_ONCnt=0;		
			}
		}
		PressAlarmTaskDelay=SysTick_GetTimer();		
	}
}




