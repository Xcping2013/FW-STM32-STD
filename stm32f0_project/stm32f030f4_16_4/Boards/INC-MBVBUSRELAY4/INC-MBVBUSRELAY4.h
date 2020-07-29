#ifndef __INC_MBVBUSRELAY_H
#define __INC_MBVBUSRELAY_H 	

#include "bsp_defines.h"

#define  LED_TOGGLE_MBVBUSRELAY()	  GPIOA->ODR ^= BIT4

void 	INC_MBVBUSRELAY_hw_init(void);
uint8_t Command_INC_MBVBUSRELAY(char *string);
void INC_MBVBUSRELAY_CMDList(void);

#endif
