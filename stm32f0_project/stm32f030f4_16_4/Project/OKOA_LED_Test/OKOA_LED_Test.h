#ifndef _OKOA_LED_Test_H
#define _OKOA_LED_Test_H	

#include "bsp_defines.h"

#define LED_TEST_DEBUG

#ifdef  LED_TEST_DEBUG

#define LED_TEST_TRACE         printf
#else
#define LED_TEST_TRACE(...)    
#endif


#define  LED_TOGGLE()	  GPIOA->ODR ^= BIT4

void MBLEDRW_HW_Init(void);

uint8_t Command_analysis_LED_TEST(char* string);

#endif
