#ifndef __LCOMP_E75_TEST_H
#define __LCOMP_E75_TEST_H 	

#include "bsp_defines.h"

#define LCOMP_DEBUG

#ifdef  LCOMP_DEBUG

#define LCOMP_TRACE         printf
#else
#define LCOMP_TRACE(...)    
#endif

uint8_t Command_analysis_LCOMP(char* string);

#endif
