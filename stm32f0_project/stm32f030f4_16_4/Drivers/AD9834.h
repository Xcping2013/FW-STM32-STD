#ifndef __AD9834_H_
#define __AD9834_H_

#include "bsp_defines.h"

//#define TRI_WAVE 	0  		//������ǲ�
//#define SIN_WAVE 	1		//������Ҳ�
//#define SQU_WAVE 	2		//�������


//void AD9834_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase );

//void AD9834_Init_GPIO(void);
//void AD9834_AmpSet(unsigned char amp);


void AD9834_Init_GPIO(void);
int AD9834_SetFreq(uint32_t freqdata);

#endif



