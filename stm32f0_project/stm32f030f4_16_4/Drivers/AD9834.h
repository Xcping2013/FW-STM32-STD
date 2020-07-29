#ifndef __AD9834_H_
#define __AD9834_H_

#include "bsp_defines.h"

//#define TRI_WAVE 	0  		//输出三角波
//#define SIN_WAVE 	1		//输出正弦波
//#define SQU_WAVE 	2		//输出方波


//void AD9834_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase );

//void AD9834_Init_GPIO(void);
//void AD9834_AmpSet(unsigned char amp);


void AD9834_Init_GPIO(void);
int AD9834_SetFreq(uint32_t freqdata);

#endif



