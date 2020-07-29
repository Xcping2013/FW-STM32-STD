/**						
  \Project		IN3-SMC-3
	\file 			IO.h
  \author 		Xcping2016
  \version 		1.00

  \brief 			I/O functions
  This file contains definitions for using the I/O functions.
*/
#ifndef __BSP_MCU_EXTI_H
#define __BSP_MCU_EXTI_H

#include "bsp_mcu_gpio.h"

void EXTI_Init_IN8(void);

void Task1_every_ms(uint16_t nms);
	
#endif
