/**
  \file 			UART.h
  \author 		Xcping2016
  \version 		1.00

  \brief Motor monitoring

  This file contains the definitions of the UART functions.
*/
#ifndef __BSP_MCU_UART_H
#define __BSP_MCU_UART_H

#include "bsp_defines.h"

#define USART1_REC_LEN  			200 

extern u8  USART1_RX_BUF[USART1_REC_LEN]; 
extern u16 USART1_RX_STA;

#define USART3_REC_LEN  			50 

extern u8  USART3_RX_BUF[USART3_REC_LEN]; 
extern u16 USART3_RX_STA;

void InitUART_1(int BaudRate);

void InitUART_2(int BaudRate);

void UART3_EnableRX(void);
void UART3_DisableRX(void);
#endif

