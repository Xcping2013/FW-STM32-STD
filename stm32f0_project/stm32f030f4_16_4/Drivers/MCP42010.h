#ifndef __MCP42010_H_
#define __MCP42010_H_

#include "bsp_defines.h"

//MCP42010: Dual Digital Potentiometer (10kOHM)

void MCP42010_Init_GPIO(void);

//POT=1 P0	POT=2 P1 POT=3 ALL
int MCP42010_Set256Taps(uint8_t pot,uint8_t taps);

#endif



