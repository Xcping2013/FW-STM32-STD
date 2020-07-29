#ifndef __BSP_MCU_GPIO_H
#define __BSP_MCU_GPIO_H

#include "bsp_defines.h"

#define __STM32_PORT(port)  GPIO##port

#define GET_PIN(PORTx,PIN) (long)((16 * ( ((long)__STM32_PORT(PORTx) - (long)GPIOA)/(0x0400UL) )) + PIN)

#define __STM32_PIN(index, gpio, gpio_index)                                \
    {                                                                       \
        index, GPIO##gpio, GPIO_Pin_##gpio_index                            \
    }
/* STM32 GPIO driver */
struct pin_index
{
    int index;
    GPIO_TypeDef *gpio;
    uint32_t pin;
};

void 			pinMode(uint8_t pin, uint8_t mode);
void 			pinSet(uint8_t  pin);
void 			pinReset(uint8_t pin);
void 			pinWrite(uint8_t pin,uint8_t val);
uint8_t   pinRead(uint8_t pin);
void 			pinToggle(uint8_t pin);

void 			pinSWD_Init(void);
void      shift_out(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);
uint8_t   shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order);

#endif


