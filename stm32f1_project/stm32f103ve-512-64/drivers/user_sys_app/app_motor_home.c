/***********************************************************************/
#include "app_motor_home.h"
#include "bsp_mcu_delay.h"
#include "app_button.h"
#include "app_gpio_dido.h"
#include "bsp_mcu_uart.h"
/***********************************************************************/
uint8_t motorsReset_InOrder=FALSE;

/*********************************************************************************************************************/
static void setPositionAsOrigin(uint8_t axisNum);
/*********************************************************************************************************************/
static void setPositionAsOrigin(u8 axisNum) 
{
	/* 
	Before overwriting X_ACTUAL choose velocity_mode or hold_mode.If X_ACTUAL is overwritten in ramp_mode or soft_mode the motor directly drives to X_TARGET
	*/	
	StopMotorByRamp(axisNum);

	delay_ms(50);
	Write429Short(IDX_VMAX|(axisNum<<5), 0);	
	delay_ms(200);
	Write429Int(IDX_XTARGET|(axisNum<<5), 0);
	Write429Int(IDX_XACTUAL|(axisNum<<5), 0);
	
	homeInfo.Homed[axisNum]=TRUE;
	homeInfo.GoHome[axisNum]=FALSE;	
	homeInfo.GoLimit[axisNum]=FALSE;	
	
	printf("motor[%d] go home ok\r\n",axisNum);
	
	if(motorsReset_InOrder==TRUE)	
	{
		switch(g_tParam.Project_ID)
		{
			case BUTTON_ONLINE:
			case BUTTON_OFFLINE:
					if(axisNum==AXIS_X) //Y轴先里后出复位OK
					{
						//stop_timer_5ms();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						printf("motor auto reset ok\r\n");
					}
					if(axisNum==AXIS_Y) //Y轴先里后出复位OK
					{
						homeInfo.GoHome[AXIS_Z]=TRUE;	
						TMC429_MotorRotate(AXIS_Z,-homeInfo.HomeSpeed[AXIS_Z]);	
					}
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
						homeInfo.GoHome[AXIS_X]=TRUE;	
						TMC429_MotorRotate(AXIS_X,homeInfo.HomeSpeed[AXIS_X]);	
					}			 
				break;
			case BUTTON_ROAD:
					if(axisNum==AXIS_X) 
					{
						//stop_timer_5ms();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						printf("motor auto reset ok\r\n");
					}
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
						 homeInfo.GoHome[AXIS_X] =TRUE;
						 homeInfo.GoLimit[AXIS_X]=FALSE;
						 homeInfo.Homed[AXIS_X]  =FALSE;
						 homeInfo.HomeSpeed[AXIS_X]=g_tParam.speed_home[AXIS_X];

						 TMC429_MotorRotate(AXIS_X, g_tParam.speed_home[AXIS_X]);				//逆时针旋转		L		Z轴先复位		Z轴先上后下	
						 printf("motor[%d] is start go home by searching home sensor\r\n",AXIS_X);					
					}	
				break;
					
			case OQC_FLEX:
					if(axisNum==AXIS_Z) //Z轴先上后下复位OK
					{
//						stop_timer_5ms();
						motorsReset_InOrder=OK_2;
						buttonRESETpressed=FALSE;
						setChOutput(2,0);	
						printf("motor auto reset ok\r\n");
					}							
				break;
			default:
				break;
					
		}
	}
}

void MotorHomingWithLimitSwitch(uint8_t axisNum, int HomeSpeed)
{	
	//正速度为正限位为原点
	if(homeInfo.GoLimit[axisNum])
	{
		uint8_t SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);																								
		if(((SwitchStatus& (0x02<<axisNum*2)) ? 1:0) && (HomeSpeed<0)) 								
		{																													 
			{
				setPositionAsOrigin(axisNum);							
			}
		}
		if(((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)		&& (HomeSpeed>0)) 
		{																													  //REFR1  顺时针触发右限位时反转				
			{
				setPositionAsOrigin(axisNum);				
			}
		}
	}
}
/*
滑台传感器：内置   原点和限位传感器触发后 V=H，未触发时V=L
road和FLEx  原点触发式V=L，限位未触发时V=L

BUG:  原点一直触发的时候，再触发限位不会进行反向运动
*/

void MotorHomingWithHomeSensor(uint8_t axisNum, int HomeSpeed)
{	
	if(homeInfo.GoHome[axisNum])
	{
		//回原点过程中速度=0,则触发原点，电机会往方向运行
		//如果回原点过程响应电机停止命令，则动作为：电机向上回原点，电机停止命令，电机向下找负限位，再发停止命令，电机又反向运动
		//所以电机回原点命令后不响应其他动作命令，或者响应其他命令时关闭回原点开关
		if(Read429Short(IDX_VACTUAL|(axisNum<<5))==0)
		{
			u8 SwitchStatus=Read429SingleByte(IDX_REF_SWITCHES, 3);
			if((SwitchStatus& (0x02<<axisNum*2)) ? 1:0)											 	  //触发左限位
			{		

					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,HomeSpeed);				  //向右转
					else 						TMC429_MotorRotate(axisNum,-HomeSpeed);
			}
			if((SwitchStatus& (0x01<<axisNum*2)) ? 1:0)												//触发右限位
			{																													 		
					if(HomeSpeed>0)	TMC429_MotorRotate(axisNum,-HomeSpeed);					//左转
					else 						TMC429_MotorRotate(axisNum,HomeSpeed);
			}
		}
	  //电机还在运行中触发原点
		//road 的Z轴使用外置原点	X轴使用内置原点
    else if((g_tParam.Project_ID==BUTTON_ROAD && ((axisNum==0 && pinRead(homeSensorPin[axisNum])==IN_OFF) || \
			
																					 (axisNum!=0 && pinRead(homeSensorPin[axisNum])==IN_ON ))) ||	\
			 (g_tParam.Project_ID!=BUTTON_ROAD && pinRead(homeSensorPin[axisNum])==IN_ON ))		//外置原点
		
		{
			if((HomeSpeed>0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))<0 ) || (HomeSpeed<0 && (Read429Short(IDX_VACTUAL|(axisNum<<5)))>0 ))		 
			{				
				setPositionAsOrigin(axisNum);		
			}
		}
	}
}
void MotorAutoReset_preset( void )
{	
	//u8PressAlarm=0;
	buttonRESETpressed=TRUE;	setChOutput(2,1);	 setChOutput(1,0);	
	
	UART3_DisableRX();
	
	motorsReset_InOrder=TRUE;

	TMC429_MotorStop(0);
	TMC429_MotorStop(1);
	TMC429_MotorStop(2);
	Write429Int(IDX_VMAX|MOTOR0, 2047);		
	Write429Int(IDX_VMAX|MOTOR1, 2047);			
	Write429Int(IDX_VMAX|MOTOR2, 2047);		
	
	for(uint8_t i=0;i<3;i++)
	{
		homeInfo.Homed[i]=FALSE;
		homeInfo.GoHome[i]=FALSE;
		homeInfo.GoLimit[i]=FALSE;
		SetAmaxAutoByspeed(i,g_tParam.speed_home[i]);	
	}				
	switch(g_tParam.Project_ID)
	{
		case BUTTON_OFFLINE:				
		case BUTTON_ONLINE:
				 homeInfo.GoHome[AXIS_Y]=TRUE;
				 TMC429_MotorRotate(AXIS_Y,-homeInfo.HomeSpeed[AXIS_Y]);			//逆时针旋转		L		Y轴先复位		Y轴先后后前	
  
				break;
				 
		case OQC_FLEX:
    case BUTTON_ROAD:
			
				 homeInfo.GoHome[AXIS_Z]=TRUE;
				 homeInfo.GoLimit[AXIS_Z]=FALSE;
				 homeInfo.Homed[AXIS_Z]=FALSE;
				 homeInfo.HomeSpeed[AXIS_Z]=-g_tParam.speed_home[AXIS_Z];
		
		     TMC429_MotorRotate(AXIS_Z, -g_tParam.speed_home[AXIS_Z]);			//逆时针旋转		L		Z轴先复位		Z轴先上后下
	
				 printf("motor[%d] is start go home by searching home sensor\r\n",AXIS_Z);
		
		
		break;	
		
		default:
			break;
	}
}
/******************************************finsh***********************************************************************/

/*********************************************************************************************************************/



