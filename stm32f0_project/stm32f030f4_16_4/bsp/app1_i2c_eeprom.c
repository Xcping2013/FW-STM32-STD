/*
*********************************************************************************************************
*
*	模块名称 : EEPROM读写演示模块。
*	文件名称 : eeDemo.c
*	版    本 : V1.0
*	说    明 : EERPOM （24xx）读写例程。安富莱STM32-F4开发板标配的EEPROM型号为 AT24C128  (16K字节,128Kbit)
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-12-12 armfly    首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "app1_i2c_eeprom.h"

#define PARAM_VER										01

PARAM_T g_tParam;		


uint8_t ParVer=01;
uint8_t LED_redValue=50;
uint8_t LED_whiteValue=50;

/*
*********************************************************************************************************
*	函 数 名: DemoEEPROM
*	功能说明: 串行EEPROM读写例程
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void EEPROM_Init(void)
{
	if (ee_CheckOk() == 0)
	{

	}
	else
	{
		printf("eeprom found\r\n");
		eeprom_readBrightness();
	}
}

/*
*********************************************************************************************************
*	函 数 名: eeprom_read
*	功能说明: 读串行EEPROM全部数据，并打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void eeprom_readBrightness(void)
{
	ee_ReadBytes(&ParVer,0,1);
	ee_ReadBytes(&LED_redValue,1,1);
	ee_ReadBytes(&LED_whiteValue,2,1);

	if (ParVer != PARAM_VER)
	{
		LED_redValue  = 50;
		LED_whiteValue  = 50;
		
		ee_WriteBytes(&ParVer,0,1);
		ee_WriteBytes(&LED_redValue,1,1);
		ee_WriteBytes(&LED_whiteValue,2,1);
	}
}

/*
*********************************************************************************************************
*	函 数 名: eeprom_read
*	功能说明: 写串行EEPROM全部数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void eeprom_saveBrightness(void)
{
	ee_WriteBytes(&LED_redValue,1,1);
	ee_WriteBytes(&LED_whiteValue,2,1);
}

