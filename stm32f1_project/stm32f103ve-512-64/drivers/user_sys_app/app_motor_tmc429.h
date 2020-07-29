#ifndef __APP_MOTION_TMC429_H__
#define __APP_MOTION_TMC429_H__

#include "bsp_motor_tmc429.h"

#include "app_eeprom_24xx.h"

#define	 USING_TMC429_CLK_12MHZ
//#define	 USING_TMC429_CLK_16MHZ
                                        
#define AXIS_X	0
#define AXIS_Y	1
#define AXIS_Z	2

#define MVP_ABS   0            	
#define MVP_REL   1                 

typedef struct 
{
	uint8_t Homed[3];
	int HomeSpeed[3];
	uint8_t GoHome[3];
	uint8_t GoLimit[3];
}homeInfo_t;

extern homeInfo_t homeInfo;

extern uint8_t homeSensorPin[3];

typedef struct
{
  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor (TMC429)
  UCHAR RampDiv;               //!< ramp divisor (TMC429)

} TMotorConfig;

extern  TMotorConfig MotorConfig[N_O_MOTORS];

void  tmc429_hw_init(void);

void 	  SetAmaxAutoByspeed(u8 axisNum,int speed);

uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed);
uint8_t TMC429_MotorStop(uint8_t motor_number);
void 		StopMotorByRamp(UINT Motor);

void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed);

uint8_t Command_analysis_motor(char *string);
uint8_t Command_analysis_motor_get(char *string);
uint8_t Command_analysis_motor_set(char *string);

#endif

