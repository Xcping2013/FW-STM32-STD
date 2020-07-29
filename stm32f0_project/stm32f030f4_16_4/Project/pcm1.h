#ifndef __PCM1_H
#define __PCM1_H 	

#include "bsp_defines.h"

#define PCM1_DEBUG

#ifdef  PCM1_DEBUG

#define PCM1_TRACE         printf
#else
#define PCM1_TRACE(...)    
#endif

uint8_t Command_analysis_PCM1(char *string);

#endif
