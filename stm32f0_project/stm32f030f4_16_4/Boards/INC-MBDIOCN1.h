#ifndef __INC_MBDIOCN1_H
#define __INC_MBDIOCN1_H 	

#include "bsp_defines.h"

#define  LED_TOGGLE_MBDIOCN1()	  GPIOA->ODR ^= BIT4

extern uint8_t AutoGet;
extern uint8_t AutoStream;

extern uint8_t KeyS1;
extern uint8_t KeyS2;
	
void 	INC_MBDIOCN1_hw_init(void);
uint8_t Command_INC_MBDIOCN1(char *string);
void INC_MBDIOCN1_CMDList(void);

void AutoTask_per_ms(uint16_t nms);

#endif
