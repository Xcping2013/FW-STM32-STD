/**
  \file SysTick.c

  \version 1.00

  \brief 1ms system tick timer functions
*/

#include "bsp_mcu_systick.h"

static volatile UINT SysTick_u32Timer;
/***************************************************//**
  \fn SysTick_Handler(void)
  \brief System timer interrupt handler

  The system timer interrupt handler counts up the
  1ms counter.
*******************************************************/
void SysTick_Handler(void)
{
	SysTick_u32Timer++;
}
/***************************************************//**
  \fn SysTick_Init(void)
  \brief Initalize system timer

  Initalize the tick timer for generating an interrupt
  every 1ms.
*******************************************************/
void SysTick_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	SysTick->LOAD=9000;
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   						//开启SYSTICK中断
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   						//开启SYSTICK    

}
/***************************************************//**
  \fn SysTick_GetTimer(void)
  \brief Read the system timer
  \return System timer (1ms)

  This function returns the actual value of the 1ms
  system timer.
*******************************************************/
UINT SysTick_GetTimer(void)
{
  return SysTick_u32Timer;
}


