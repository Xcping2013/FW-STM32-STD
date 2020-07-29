#ifndef __INC_MBOAKPLED_H
#define __INC_MBOAKPLED_H 	

#include "bsp_defines.h"

#define  LED_TOGGLE_MBOAKPLED()	  GPIOA->ODR ^= BIT4

void INC_MBOAKPLED_hw_init(void);
uint8_t Command_INC_MBOAKPLED(char *string);
void INC_MBOAKPLED_CMDList(void);

#endif
