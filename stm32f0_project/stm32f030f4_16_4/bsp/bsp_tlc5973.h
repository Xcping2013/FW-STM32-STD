/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_TLC5973_H
#define _BSP_TLC5973_H

#include "bsp_defines.h"

void bsp_tlc5973_init(void);
void writeLED(uint8_t ledNum, uint8_t redValue, uint8_t greenValue, uint8_t blueValue);

#endif

