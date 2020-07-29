
#include "app_sys_control.h"

#include "bsp_mcu_exti.h"
#include "app_gpio_dido.h"
#include "bsp_mcu_systick.h"
#include "app_button.h"
#include "app_serial.h"
#include "app_motor_home.h"
//
static __INLINE void NVIC_CoreReset(void)
{
    __DSB();

    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
                  (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                   SCB_AIRCR_VECTRESET_Msk);
    __DSB();
    while(1);
}
//
void ResetCPU(UCHAR ResetPeripherals)
{
  __set_FAULTMASK(1);
  if(ResetPeripherals)
  {  
	  SCB->AIRCR = SCB_AIRCR_VECTKEY_Msk | (u32)0x04;
  }
  else
    //NVIC_GenerateCoreReset();
	  SCB->AIRCR = SCB_AIRCR_VECTKEY_Msk | (u32)0x01;
}
void EnableInterrupts(void)
{
  __ASM volatile("cpsie i");		 
}


/***************************************************************//**
   \fn DisableInterrupts(void)
   \brief Disable all interrupts

  This function globally disables all interrupts.
********************************************************************/
void DisableInterrupts(void)
{
  __ASM volatile("cpsid i");
}

uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}
//
void xstrcat(char *str1,char *str2)
{
	int i,len1;
	for(i=0;str1[i]!='\0';i++);
	len1=i;
	for(i=0;str2[i]!='\0';i++)
	str1[i+len1]=str2[i];
}
void MakeLowercase( char *str)		
{
	while(*str!='\0')
	{
			if( ((*str)>='A')&&((*str)<='Z') )
			{
					*str=*str+32;
			}
			str++;
	}
}
//
void PressAlarm_Handler(void)
{
	if(INPUT8==0 && buttonRESETpressed==0 && homeInfo.GoHome[2]==FALSE)	 	
	{			
		UART3_DisableRX();

		HardStop(2);//HardStop(1);HardStop(0);

		homeInfo.Homed[2]=FALSE;
		homeInfo.GoHome[2]=FALSE;	
		homeInfo.GoLimit[2]=FALSE;	

		printf("stop motor z due to pressure overhigh!!!\r\n");	
	} 	
}

void Task1_every_ms(uint16_t nms)
{
	static U32_T task1Delay =0;
	static uint8_t printStr5times;
  if(abs((int)(SysTick_GetTimer()-task1Delay))>nms)
  {
  		task1Delay=SysTick_GetTimer();		
//		// if(INPUT8==1)	u8PressAlarm=0;
//			//if(printStr5times!=0) 
//			if(u8PressAlarm==1)
//			{
//				//printStr5times--;
//				printf("stop motor z due to pressure overhigh!!!\r\n>>");
//			}
  }	
	
}
//
void Task2_every_ms(uint16_t nms)
{
	static U32_T task2Delay =0;

  if(abs((int)(SysTick_GetTimer()-task2Delay))>nms)
  {
		
		if(g_tParam.Project_ID!=AOI_2AXIS || g_tParam.Project_ID!=COMMON || g_tParam.Project_ID!=LIDOPEN)
		{
			buttonSTART_process(1,1);
			buttonRESET_process(2,2);	
		}
  	task2Delay=SysTick_GetTimer();		

  }	
}





