#ifndef __INC_MBAD9834_H
#define __INC_MBAD9834_H 	

#include "bsp_defines.h"
#include "ad9834.h"

#define CS0_0()		GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define CS0_1()		GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define FSYNC_0()		GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define FSYNC_1()		GPIO_SetBits(GPIOB,GPIO_Pin_1)

#define SCLK_0()			GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define SCLK_1()			GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define SDATA_0()			GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define SDATA_1()			GPIO_SetBits(GPIOA,GPIO_Pin_7)


#define  LED_TOGGLE_MBAD9834()	  GPIOF->ODR ^= BIT0

void 	INC_MBAD9834_hw_init(void);
uint8_t Command_INC_MBAD9834(char *string);

#endif
