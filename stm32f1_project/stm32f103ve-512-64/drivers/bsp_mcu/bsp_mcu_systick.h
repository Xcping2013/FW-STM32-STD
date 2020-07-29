
/**
  \file SysTick.h

  \version 1.00

  \brief System tick timer

  This file contains the definitions of the tick timer functions.
*/

#ifndef __BSP_MCU_SYSTICK_H
#define __BSP_MCU_SYSTICK_H

#include "bsp_defines.h"

void SysTick_Init(void);
UINT SysTick_GetTimer(void);

#endif

