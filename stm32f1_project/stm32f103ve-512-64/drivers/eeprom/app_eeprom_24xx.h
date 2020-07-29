#ifndef __APP_EEPROM_24XX_H
#define __APP_EEPROM_24XX_H

#include "bsp_eeprom_24xx.h"

#define PARAM_ADDR						0			
#define PARAM_VER							0x100				

#define N_O_MOTORS 				 		3   

enum ProjectId 
{
	COMMON, 
	BUTTON_ONLINE, 
	BUTTON_OFFLINE,	
	BUTTON_VER3,
	
	BUTTON_ROAD,
	OQC_FLEX,

	LIDOPEN,
	AOI_2AXIS
};

typedef struct
{
	uint32_t ParamVer;									//4					/* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */
	uint32_t MasterBaudrate;						//4
	uint32_t SlaveBaudrate;
	uint8_t	 Project_ID;								//1
	
	UINT tmc429_VMax[N_O_MOTORS];  			//12   			//!< maximum positioning velocity
	UINT tmc429_AMax[N_O_MOTORS];  			//12  			//!< maximum acceleration
	
	UCHAR tmc429_PulseDiv[N_O_MOTORS]; 	//3    			//!< pulse divisor (TMC429)
	UCHAR tmc429_RampDiv[N_O_MOTORS];  	//3    			//!< ramp divisor (TMC429)

	UINT speed_home[N_O_MOTORS];	 			//12	
}
PARAM_T;

extern PARAM_T g_tParam;


void at24cxx_hw_init(void);

uint8_t Command_analysis_eeprom(char *string);

#endif
















