
#ifndef __MAIN_H
#define __MAIN_H 	

#include "bsp_defines.h"

#include "delay.h"
#include "uart1.h"

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                               /* Reset value, write to SCB_AIRCR can reset cpu */


extern void AutoTask0_per_ms(uint16_t nms);
extern void AutoTask1_per_ms(uint16_t nms);

#endif


