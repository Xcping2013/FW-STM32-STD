/*
*********************************************************************************************************
*	                                  
*	ģ������ : EEPROM��д��ʾģ�顣    
*	�ļ����� : demo_i2c_eeprom.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _EEDEMO_H
#define _EEDEMO_H

#include "bsp.h"


typedef struct	
{
	uint32_t  ParamVer;									//4					/* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */
	uint16_t	LED_RB;
	uint16_t	LED_WB;
}
PARAM_T;

extern uint8_t LED_redValue;
extern uint8_t LED_whiteValue;

extern PARAM_T g_tParam;	

/* ���ⲿ���õĺ������� */
void EEPROM_Init(void);

void eeprom_readBrightness(void);
void eeprom_saveBrightness(void);

#endif


