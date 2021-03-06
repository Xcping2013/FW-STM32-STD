/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_TLC5973_H
#define _BSP_TLC5973_H

#include "bsp_defines.h"

void bsp_tlc5973_init(void);
void writeLED(uint8_t ledNum, uint8_t redValue, uint8_t greenValue, uint8_t blueValue);

#endif

