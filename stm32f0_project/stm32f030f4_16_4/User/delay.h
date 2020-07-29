#ifndef __DELAY_H
#define __DELAY_H 		


#include "bsp_defines.h" 

//void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
u32  SysTick_GetTimer(void);

#endif





























