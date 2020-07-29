#ifndef __APP_SERIAL_H
#define __APP_SERIAL_H

#include "bsp_mcu_uart.h"

void UART3_EnableRX(void);
void UART3_DisableRX(void);

void    PrintfUart3DataToUart1(void);
uint8_t Command_analysis_uart3(char *string);

#endif
