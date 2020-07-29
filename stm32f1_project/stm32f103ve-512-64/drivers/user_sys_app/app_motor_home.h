#ifndef  __APP_MOTOR_HOME_H
#define  __APP_MOTOR_HOME_H

#include "app_motor_tmc429.h"

extern uint8_t motorsReset_InOrder;

void MotorAutoReset_preset( void );

void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed);
void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed);

#endif



