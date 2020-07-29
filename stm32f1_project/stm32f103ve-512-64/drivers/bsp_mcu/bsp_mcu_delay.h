#ifndef __BSP_MCU_DELAY_H
#define __BSP_MCU_DELAY_H 

#include "bsp_defines.h"
#include "bsp_mcu_systick.h"

void drv_udelay(uint32_t nus);
void drv_mdelay(uint16_t nms);

void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);

#endif
