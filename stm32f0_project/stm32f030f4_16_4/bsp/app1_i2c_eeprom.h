/*
*********************************************************************************************************
*	                                  
*	模块名称 : EEPROM读写演示模块。    
*	文件名称 : demo_i2c_eeprom.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _EEDEMO_H
#define _EEDEMO_H

#include "bsp.h"


typedef struct	
{
	uint32_t  ParamVer;									//4					/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */
	uint16_t	LED_RB;
	uint16_t	LED_WB;
}
PARAM_T;

extern uint8_t LED_redValue;
extern uint8_t LED_whiteValue;

extern PARAM_T g_tParam;	

/* 供外部调用的函数声明 */
void EEPROM_Init(void);

void eeprom_readBrightness(void);
void eeprom_saveBrightness(void);

#endif


