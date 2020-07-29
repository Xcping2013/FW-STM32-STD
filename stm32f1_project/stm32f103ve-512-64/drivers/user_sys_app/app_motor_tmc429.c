/*  TMCL Instruction
1-ROR rotate right
2-ROL rotate left
3-MST Motor  stop
4-MVP move to position
5-SAP set axis parameter
6-GAP get axis parameter
7-STAP store axis parameter
8-RSAP restore axis parameter
9-SGP set global parameter
10-SGP get global parameter
11-STGP store global parameter
12-RSGP restore global parameter
13-REF reference search
14-SIO set output
15-GIO get input
*/
#include "app_motor_tmc429.h"
#include "bsp_mcu_delay.h"
#include "bsp_mcu_timer.h"


#include "app_serial.h"
#include "app_button.h"
#include "app_motor_home.h"
#include "app_sys_control.h"
#include "app_gpio_dido.h"
/*********************************************************************************************************************/
enum axisParameter {

next_position, 			
actual_position, 
	
next_speed, 				//int
actual_speed,				//int 
max_v_positioning, 	//int
max_acc, 						//int

position_reached,
ref_SwStatus,				//=leftLimit_SwS
rightLimit_SwS,
leftLimit_SwS,
allLimit_SwS,
rightLimit_disable,	
leftLimit_disable,

max_current,
standby_current,
	
min_speed,					//int
actual_acc,	
ramp_mode,

ref_sw_tolerance,
softStop_flag,

ramp_divisor,
pulse_divisor,	

limitSignal,
home_SenS,

is_homed,
is_stop,
is_reach,
is_reset,

Type_none,

};
//
homeInfo_t homeInfo;

uint8_t homeSensorPin[3]={PB_9,PE_0,PE_1};
//usteps=8; //200*8=1600	3R/S 	如果是Y轴2mm 1圈的话 V=70=30mm/min	 0.25R/S	3/0.25=12	839/12=70
TMotorConfig MotorConfig[N_O_MOTORS]=
{
				//12MHZ 																			
				//3R/S																			
  839,  //!< VMax					839,  											
  1000, //!< AMax					1000, 												
  5,    //!< Pulsediv			5,    													
  8,    //!< Rampdiv			8,    													
};
//
typedef struct
{
  UCHAR Type;        //!< type parameter
  UCHAR Motor;       //!< motor/bank parameter
  union
  {
    long Int32;      //!< value parameter as 32 bit integer
    UCHAR Byte[4];   //!< value parameter as 4 bytes
  } Value;           //!< value parameter
	
} TMC429_Command;

TMC429_Command CMDGetFromUart;

typedef struct
{
  uint8_t Status;               
  union
  {
    long Int32;      
    uint8_t Byte[4];   
  } Value;   
	
} TMC429_Parameter;

TMC429_Parameter TMC429_ParameterGet;
TMC429_Parameter TMC429_ParameterSet;

static UCHAR SpeedChangedFlag[N_O_MOTORS];

static uint8_t MotorStopByLimit[N_O_MOTORS]={DISABLE,DISABLE,DISABLE};								//碰到限位处理一次
	
/*********************************************************************************************************************/
static void 		TMC429_ParameterPresetToRam(void);
static uint8_t 	TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position);

static void 		TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type);
static void 		TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value);
static void 		MotorKeepStop_removeLimitSigal(uint8_t motor_number);

static void 		printf_cmdList_motor(void);
static void 		printf_cmdList_motor_set(void);
static void 		printf_cmdList_motor_get(void);

/*********************************************************************************************************************/
void Init429(void)
{
  UINT addr;
  UCHAR Motor;

  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)
      Write429Zero(addr|(Motor<<5));
  }

	Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);

	Write429Datagram(IDX_SMGP, 0x00, 0x04, 0x02);

  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00, (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x04);
  Write429Datagram(IDX_REFCONF_RM|MOTOR0, 0x00, HARD_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR1, 0x00, HARD_REF, 0x00);
  Write429Datagram(IDX_REFCONF_RM|MOTOR2, 0x00, HARD_REF, 0x00);
  Write429Short(IDX_VMIN|MOTOR0, 1);
  Write429Short(IDX_VMIN|MOTOR1, 1);
  Write429Short(IDX_VMIN|MOTOR2, 1);

  Write429Int(IDX_VMAX|MOTOR0, MotorConfig[0].VMax);
  SetAMax(0, MotorConfig[0].AMax);
  Write429Int(IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(1, MotorConfig[1].AMax);
  Write429Int(IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(2, MotorConfig[2].AMax);
}
//
void  tmc429_hw_init(void)
{
	pinMode(POSCMP1_PIN, GPIO_Mode_IPU);
  pinMode(INTOUT1_PIN, GPIO_Mode_IPU);	

	pinMode(CS_PIN_TMC429,GPIO_Mode_Out_PP);
	pinSet(CS_PIN_TMC429);

	SPI1_Init();
	
#if defined(USING_TMC429_CLK_12MHZ) 
		
	TMC429_CLK_Init();
	
#endif
	
	TMC429_ParameterPresetToRam();
	
	Init429();

//板卡上电后已当前位置为原点，正常应用需要回原点动作
	for(uint8_t i=0;i<3;i++)
	{
		HardStop(i);
		delay_ms(50);
		Write429Short(IDX_VMAX|(i<<5), 0);	
		delay_ms(50);
		Write429Int(IDX_XTARGET|(i<<5), 0);
		Write429Int(IDX_XACTUAL|(i<<5), 0);
	}
	printf("tmc429_hw_init()...ok\r\n");
}
static void TMC429_ParameterPresetToRam(void)
{
  uint8_t i;
	for(i=0; i<N_O_MOTORS; i++) 					
	{																  
		MotorConfig[i].VMax 		= g_tParam.tmc429_VMax[i];   				
		MotorConfig[i].AMax 		=	g_tParam.tmc429_AMax[i];
		MotorConfig[i].PulseDiv	=	g_tParam.tmc429_PulseDiv[i];	
		MotorConfig[i].RampDiv	=	g_tParam.tmc429_RampDiv[i];	
		
		SpeedChangedFlag[i]			=	TRUE;
		homeInfo.Homed[i]				=	FALSE;
		homeInfo.GoHome[i]			=	FALSE;
		homeInfo.GoLimit[i]			=	FALSE;
		homeInfo.HomeSpeed[i]		=	g_tParam.speed_home[i];
	}
}

//
static uint8_t TMC429_MoveToPosition(uint8_t motor_number, uint8_t motion_mode, int32_t position)
{
	uint8_t result = REPLY_OK;
	
  if(motor_number<N_O_MOTORS)
  {
    if(motion_mode==MVP_ABS || motion_mode==MVP_REL)
    {
			//MotorStopByLimit[motor_number]=1;
      if(SpeedChangedFlag[motor_number])
      {
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
        SpeedChangedFlag[motor_number]=FALSE;
      }
      if(motion_mode==MVP_ABS)
			{
				printf("motor[%d] is start to make absolute motion\r\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position);
			}
      else
			{
				printf("motor[%d] is start to make relative motion\r\n",motor_number);
        Write429Int(IDX_XTARGET|(motor_number<<5), position + Read429Int(IDX_XACTUAL|(motor_number<<5)));
			}
      Set429RampMode(motor_number, RM_RAMP);
    }
    else 
		{
			result = REPLY_WRONG_TYPE;
		}
  }
  else result = REPLY_INVALID_VALUE;
	
	return result;
}
/***************************************************************//**
  \fn GetAxisParameter(void)
  \brief Command GAP

  GAP (Get Axis Parameter) command (see TMCL manual).
********************************************************************/
static void TMC429_GetAxisParameter(uint8_t motor_number, uint8_t parameter_type)
{
		
  TMC429_ParameterGet.Value.Int32=0;
	
  if(motor_number < N_O_MOTORS)	
  {
    switch(parameter_type)
    {
      case next_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XTARGET|(motor_number<<5));				
				printf("motor[%d] next position=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
				break;

      case actual_position:
        TMC429_ParameterGet.Value.Int32=Read429Int(IDX_XACTUAL|(motor_number<<5));
				printf("motor[%d] actual position=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case next_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VTARGET|(motor_number<<5));
			  printf("motor[%d] next speed=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case actual_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VACTUAL|(motor_number<<5));
			  printf("motor[%d] actual speed=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_v_positioning:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].VMax;
			  printf("motor[%d] max positioning speed=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_acc:
        TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].AMax;
			  printf("motor[%d] max acceleration=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case max_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IRun;
				printf("motor[%d] max current=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case standby_current:
        //TMC429_ParameterGet.Value.Int32=MotorConfig[motor_number].IStandby;
				printf("motor[%d] standby current=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case position_reached:
//        if(Read429Status() & 0x01) TMC429_ParameterGet.Value.Byte[0]=1;
//				printf("motor[%d] position reached=%d\r\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				printf("motor[%d] position reached=%d\r\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);
        break;

      case leftLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x02<<motor_number*2)) ? 1:0;
			  printf("motor[%d] left limit switch status=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case rightLimit_SwS:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<motor_number*2)) ? 1:0;
			  printf("motor[%d] right limit switch status=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;
			
      case allLimit_SwS:
				{
						uint8_t limit_char[6];
						uint8_t i;
						uint8_t limit = Read429SingleByte(IDX_REF_SWITCHES, 3);
						for(i=0;i<6;i++)
						{
							limit_char[i]=(limit&(0x20>>i)) ? '1':'0';
						}
						printf("motor limit switch status L2R2L1R1L0R0=%.6s\r\n",limit_char);
				}
        break;

      case rightLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x02) ? 1:0;
			  printf("motor[%d] right limit switch disable=%d\r\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
				break;

      case leftLimit_disable:
        TMC429_ParameterGet.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 2) & 0x01) ? 1:0;
				printf("motor[%d] left limit switch disable=%d\r\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case min_speed:
        TMC429_ParameterGet.Value.Int32=Read429Short(IDX_VMIN|(motor_number<<5));
				printf("motor[%d] minimum speed=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case ramp_mode:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_REFCONF_RM|(motor_number<<5), 3);
			  printf("motor[%d] ramp mode=%d\r\n",motor_number,TMC429_ParameterGet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        TMC429_ParameterGet.Value.Int32=Read429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE);
				printf("motor[%d] ref switch tolerance=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case softStop_flag:
        TMC429_ParameterGet.Value.Int32=(Read429SingleByte(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, 0) & 0x04) ? 1:0;
				printf("motor[%d] soft stop status=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
			  break;

      case ramp_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) & 0x0f;
		  	printf("motor[%d] ramp divisor=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

      case pulse_divisor:
        TMC429_ParameterGet.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(motor_number<<5), 2) >> 4;
				printf("motor[%d] pulse divisor=%ld\r\n",motor_number,TMC429_ParameterGet.Value.Int32);
        break;

			case is_homed:
				printf("motor[%d] homed=%d\r\n",motor_number,homeInfo.Homed[motor_number]);
				break;
			case is_stop:
				printf("motor[%d] stop=%d\r\n",motor_number,Read429Short(IDX_VACTUAL|(motor_number<<5))? 0:1) ;
				break;
			case is_reach:

				printf("motor[%d] reach=%d\r\n",motor_number,(Read429Status() & (0x01<<motor_number*2)) ? 1:0);

				break;
			case home_SenS:
				printf("motor[%d] homeSensor=%d\r\n",motor_number, pinRead(homeSensorPin[motor_number]) ? 0:1) ;
				break;						
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
}



static void TMC429_SetAxisParameter(uint8_t motor_number, uint8_t parameter_type, int32_t parameter_value)
{
  UCHAR Read[4], Write[4];
  if(motor_number < N_O_MOTORS)
  {
		TMC429_ParameterSet.Value.Int32=parameter_value;
		
    switch(parameter_type)
    {
      case next_position:
        Write429Int(IDX_XTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] next position=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_position:
        Write429Int(IDX_XACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] actual position=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case next_speed:
        Write429Short(IDX_VTARGET|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] next speed=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case actual_speed:
        Write429Short(IDX_VACTUAL|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] actual speed=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_v_positioning:
        MotorConfig[motor_number].VMax=TMC429_ParameterSet.Value.Int32;
        Write429Short(IDX_VMAX|(motor_number<<5), MotorConfig[motor_number].VMax);
				SetAmaxAutoByspeed(motor_number,MotorConfig[motor_number].VMax);
				printf("set motor[%d] speed=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case max_acc:
        MotorConfig[motor_number].AMax=TMC429_ParameterSet.Value.Int32;
        SetAMax(motor_number, MotorConfig[motor_number].AMax);
				printf("set motor[%d] max acceleration speed=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);		
        break;

      case rightLimit_disable:	//12
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x02;
        else
          Write[2]=Read[2]&  ~0x02;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					printf("motor[%d] right limit switch is disable\r\n",motor_number);	
				else 
					printf("motor[%d] right limit switch is enable\r\n",motor_number);	
        break;

      case leftLimit_disable: //13
        Write[0]=IDX_REFCONF_RM|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        if(CMDGetFromUart.Value.Byte[0]!=0)
          Write[2]=Read[2]|0x01;
        else
          Write[2]=Read[2]&  ~0x01;
        Write[3]=Read[3];
        Write[0]=IDX_REFCONF_RM|(motor_number<<5);
        ReadWrite429(Read, Write);
				if(CMDGetFromUart.Value.Byte[0]!=0)
					printf("motor[%d] left limit switch is disable\r\n",motor_number);	
				else 
					printf("motor[%d] left limit switch is enable\r\n",motor_number);	
        break;

      case min_speed:
        Write429Short(IDX_VMIN|(motor_number<<5), TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] minimum speed=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_mode:
        Set429RampMode(motor_number, TMC429_ParameterSet.Value.Byte[0]);
				printf("set motor[%d] ramp mode=%d ok\r\n",motor_number,TMC429_ParameterSet.Value.Byte[0]);
        break;

      case ref_sw_tolerance:
        Write429Short(MOTOR_NUMBER(motor_number)<<5|IDX_DX_REFTOLERANCE, TMC429_ParameterSet.Value.Int32);
				printf("set motor[%d] ref switch tolerance=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
			  break;

      case softStop_flag:
        Read429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
        if(TMC429_ParameterSet.Value.Int32!=0)
          Read[1]|=0x04;
        else
          Read[1]&= ~0x04;
        Write429Bytes(IDX_REFCONF_RM|MOTOR_NUMBER(motor_number)<<5, Read);
				printf("set motor[%d] soft stop status=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case ramp_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0xf0) | (CMDGetFromUart.Value.Byte[0] & 0x0f);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].RampDiv=CMDGetFromUart.Value.Byte[0] & 0x0f;
				printf("set motor[%d] ramp divisor=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;

      case pulse_divisor:
        Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(motor_number<<5);
        ReadWrite429(Read, Write);
        Write[1]=Read[1];
        Write[2]=(Read[2] & 0x0f) | (CMDGetFromUart.Value.Byte[0] << 4);
        Write[3]=Read[3];
        Write[0]=IDX_PULSEDIV_RAMPDIV|(motor_number<<5);
        ReadWrite429(Read, Write);
        MotorConfig[motor_number].PulseDiv=CMDGetFromUart.Value.Byte[0]& 0x0f;
				printf("set motor[%d] pulse divisor=%ld ok\r\n",motor_number,TMC429_ParameterSet.Value.Int32);
        break;
			case limitSignal:
				if(TMC429_ParameterSet.Value.Int32!=0) 
				{
				  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);
				}
				else Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);
				printf("set limit signal effective trigger level=%ld ok\r\n",TMC429_ParameterSet.Value.Int32);
				break;
				
      default:
        TMC429_ParameterGet.Status=REPLY_WRONG_TYPE;
        break;
    }
  } else TMC429_ParameterGet.Status=REPLY_INVALID_VALUE;
}

/*
 对轴号和参数范围先进行判断	内部调用函数不再进行判断
		0		 		1       		2 							3					4						5
argc=2
  motor   reset			//封装一系列动作，需要提取项目号来触发，不建议应用
argc=3
	motor 	stop  		<axisNum> 
argc=4
	motor 	rotate 		<axisNum> 	<axis_speed>  
  motor 	move	 		<axisNum> 	<position> 
	motor 	moveto	 	<axisNum> 	<position> 
	motor 	get 			<axisNum> 	<type>  

  motor 	gohome 		<axisNum> 	<speed> 
	motor 	golimit		<axisNum> 	<speed> 
argc=5
  motor 	set 			<type> 			<axisNum> 				<value>  

  	
*/
static void	printf_cmdList_motor(void)
{
		printf("Usage: \r\n");
		printf("motor stop <axisNum>                  - stop motor\r\n");		
		printf("motor rotate <axisNum> <speed>        - rotate motor\r\n");	
		printf("motor moveto <axisNum> <position>     - absolute motion of the motor\r\n");
		printf("motor move <axisNum> <position>       - relative motion of the motor\r\n");

		printf("motor get <type> <axisNum>            - get axis parameter\r\n");	
		printf("motor set <type> <axisNum> <value>    - set axis parameter\r\n");	
		
		printf("motor gohome <axisNum> <speed>        - home sensor as origin position\r\n");	
		printf("motor golimit <axisNum> <speed>       - limit sensor as origin position\r\n");	
		
		printf("motor reset                           - motor reset\r\n");
    printf("motor stop reset                      - motor stop reset\r\n");	
	
		printf("motor reset?                          - is axis reset or not\r\n");		
}
static void printf_cmdList_motor_set(void)
{
		printf("Usage: \r\n");
	
//		printf("motor set speed <axis> <value>             -set the rotate speed \r\n");				
//		printf("motor set next_position <axis> <value>      -set the target position to move\r\n");
		printf("motor set actual_position <axis> <value>    -set the current position\r\n");				
//		printf("motor set actual_speed <axis> <value>       -set the current speed \r\n");
		printf("motor set VMAX <axis> <value>               -set max positioning speed\r\n");
		printf("motor set AMAX <axis> <value>               -set max acceleration\r\n");
		printf("motor set ramp_div <axis> <value>           -set ramp divisor value\r\n");
		printf("motor set pulse_div <axis> <value>          -set pulse divisor value\r\n");	
//		printf("motor set rightLimit <axis> <value>         -set right limit switch\r\n");
//		printf("motor set leftLimit <axis> <value>          -set left limit switch\r\n");
//		printf("motor set limitSignal <axis> <value>        -set limit signal effective trigger level\r\n");		
}
static void printf_cmdList_motor_get(void)
{
	printf("Usage: \r\n");
	//用户接口
	printf("motor get speed <axis>             -get the current speed \r\n");
	printf("motor get position <axis>          -get the current position\r\n");
	 
	printf("motor get limit all                -get all limit switch status\r\n");	
	printf("motor get rightLimit <axis>        -get right switch status\r\n");
	printf("motor get leftLimit  <axis>        -get left switch status\r\n");	
	
	printf("motor get is_homed <axis>          -is axis homed or not\r\n");	
	printf("motor get is_stop <axis>           -is axis stop or not\r\n");	
	printf("motor get is_reach <axis>          -is axis reach the position\r\n");	
	printf("motor get homeSensor <axis>        -get home sensor status\r\n");	
	//调试接口
//	printf("motor get next_speed <axis>        -get the target speed \r\n");
	printf("motor get next_position <axis>     -get the target position to move\r\n");
	
	printf("motor get VMAX <axis>              -get max positioning speed\r\n");
	printf("motor get AMAX <axis>              -get max acceleration\r\n");

//	printf("motor get position_reached <axis>  -is reach positon or not\r\n");
	printf("motor get ramp_div <axis>          -get ramp divisor value\r\n");
	printf("motor get pulse_div <axis>         -get pulse divisor value\r\n");			
	
}
/*********************************************************************************************************************/
/***************************************************************//**
  \fn RotateRight(void)
  \brief Command ROR (see TMCL manual)
********************************************************************/
uint8_t TMC429_MotorRotate(uint8_t motor_number, int32_t motor_speed)
{
  if(motor_number < N_O_MOTORS)
  {
		//MotorStopByLimit[motor_number]=1;	
    SpeedChangedFlag[motor_number]=TRUE;
		
    Set429RampMode(motor_number, RM_VELOCITY);
		
    Write429Short(IDX_VMAX|(motor_number<<5), 2047);
    Write429Short(IDX_VTARGET|(motor_number<<5), motor_speed);
		printf("motor%d is rotate at %d\r\n",motor_number,motor_speed);
		return 0;	
  }
	return 1;
}
/***************************************************************//**
  \fn MotorStop(void)
  \brief Command MST

  MST (Motor StoP) command (see TMCL manual).
********************************************************************/
uint8_t TMC429_MotorStop(uint8_t motor_number)
{
  if(motor_number < N_O_MOTORS)
  {	
		/*  立刻停止  */
		HardStop(motor_number);
		/* 回原点过程中响应停止命令的变量置位 */
		homeInfo.GoHome[motor_number]=0;
		homeInfo.GoLimit[motor_number]=0;
		/* 电机停止会自动返回当前位置 */
		
		return 0;
  }
	return 1;
}
void StopMotorByRamp(UINT Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);
}
void SetAmaxAutoByspeed(u8 axisNum,int speed)
{
	//16MHZ|1600steps PulseDiv=6 //speed=105->0.25/s  15.02/m 2047=4.88/S  292.83/m
	//16MHZ|1600steps PulseDiv=7 //speed=210->0.25/s  15.02/m 2047=2.44/S  146.41/m
	if(MotorConfig[axisNum].PulseDiv==6 &&	MotorConfig[axisNum].RampDiv==8)
	{
		//按项目进行速度设置 0.5mm/S为最小速度  其他速度都大于此设置值
		if (speed==105)										MotorConfig[axisNum].AMax=275;			//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=10;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed+10;	
		else if (105<speed && speed<210)	MotorConfig[axisNum].AMax=speed*2;	
		else if (209<speed && speed<400)	MotorConfig[axisNum].AMax=1000;	
		else 															MotorConfig[axisNum].AMax=2047;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed==70)										MotorConfig[axisNum].AMax=200;			//0.05s-->210---0.5/S
		else if (speed<50)								MotorConfig[axisNum].AMax=speed;	
		else if (49<speed && speed<105)		MotorConfig[axisNum].AMax=speed*2;	
		else if (104<speed && speed<210)	MotorConfig[axisNum].AMax=speed*3;		
		else 															MotorConfig[axisNum].AMax=2000;	
		SetAMax(axisNum, MotorConfig[axisNum].AMax);
	}
	else if(MotorConfig[axisNum].PulseDiv==7&&	MotorConfig[axisNum].RampDiv==10)
	{
		if (speed<25)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}			
		else if (speed<100)
		{		SetAMax(axisNum, speed+10);	MotorConfig[axisNum].AMax=speed+10;}
		else if (speed<150)
		{		SetAMax(axisNum, speed+30);MotorConfig[axisNum].AMax=speed+30;}
		else if (speed<280)
		{		SetAMax(axisNum, speed+60);MotorConfig[axisNum].AMax=speed+60;}
		else if (speed<420)
		{		SetAMax(axisNum, speed+100);MotorConfig[axisNum].AMax=speed+100;}
		else if (speed<560)
		{		SetAMax(axisNum, speed+150);MotorConfig[axisNum].AMax=speed+150;}
		else if (speed<700)
		{		SetAMax(axisNum, speed+200);MotorConfig[axisNum].AMax=speed+200;}
		else if (speed<840)
		{		SetAMax(axisNum, speed+250);MotorConfig[axisNum].AMax=speed+250;}
		else if (speed<980)
		{		SetAMax(axisNum, speed+300);MotorConfig[axisNum].AMax=speed+300;}
		else if (speed<=1180)
		{		SetAMax(axisNum, speed+350);MotorConfig[axisNum].AMax=speed+350;}
		else if (speed<2047)
		{		SetAMax(axisNum, speed+600);MotorConfig[axisNum].AMax=speed+600;}	
	}
	else if(MotorConfig[axisNum].PulseDiv==5 &&	MotorConfig[axisNum].RampDiv==8)
	{
		if (speed<25)	//1~24
		{		SetAMax(axisNum, speed);	MotorConfig[axisNum].AMax=speed;}			
		else if (speed<50)
		{		SetAMax(axisNum, speed+5);	MotorConfig[axisNum].AMax=speed+5;}
		else if (speed<100)
		{		SetAMax(axisNum, 100);	MotorConfig[axisNum].AMax=100;}
		else if (speed<150)
		{		SetAMax(axisNum, 500);MotorConfig[axisNum].AMax=500;}
		else if (speed<250)
		{		SetAMax(axisNum, 1000);MotorConfig[axisNum].AMax=1000;}
		else if (speed<2047)
		{		SetAMax(axisNum, 2047);MotorConfig[axisNum].AMax=2047;}	
	}
}
//
//
uint8_t Command_analysis_motor(char *string)
{
//  CMDGetFromUart.Type=Type_none;
//	TMC429_ParameterGet.Status=REPLY_OK;
	
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"motor help") )		 					printf_cmdList_motor();
	else if( !strcmp(string,"motor get help") )		 	printf_cmdList_motor_get();
	else if( !strcmp(string,"motor set help") )		 	printf_cmdList_motor_set();

	else if( !strcmp(string,"motor reset?")) 				printf("motor reset=%d\n",motorsReset_InOrder==OK_2 ? 1:0) ;
	else if( !strcmp(string,"motor reset")) 				MotorAutoReset_preset();
	else if( !strcmp(string,"motor stop reset")) 		
	{
		buttonRESETpressed=FALSE;
		homeInfo.GoHome[0]=0;
		homeInfo.GoHome[1]=0;
		homeInfo.GoHome[2]=0;
		TMC429_MotorStop(0);TMC429_MotorStop(1);TMC429_MotorStop(2);	
		printf("motor stop reset and all motor is stop\r\n") ;		
	}
	else if( !strncmp(string,"motor stop ",11)) 		
	{
		if(buttonRESETpressed!=FALSE || homeInfo.GoHome[2]==1) 
		{
		  printf("motor is reseting\n");
		}
		else
		{
			char *p = NULL;
			char *s = &string[11];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);	
			UART3_DisableRX();
			if(!TMC429_MotorStop(CMDGetFromUart.Motor))	//电机停止后自动会关闭串口打印压力
			{
				printf("motor[%d] is stop and P[%d]=%d\n",CMDGetFromUart.Motor,CMDGetFromUart.Motor,Read429Int(IDX_XACTUAL|(CMDGetFromUart.Motor<<5)));			
			}
		}
	}
	else if( !strncmp(string,"motor rotate ",13)) 
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0 )	
			{
				if(u8PressAlarm == TRUE && CMDGetFromUart.Value.Int32>0 && CMDGetFromUart.Motor==2 )  
				{
					printf("pressure overhigh, motor z is forbidden to move downward\r\n");				
				}
				else if(buttonRESETpressed==TRUE )
				{	
					printf("motor is resetting\r\n");
				}
				else  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
			}
		}
	}
	/*motor move 2 -1000*/
	else if( !strncmp(string,"motor move ",11)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[11];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
				if(buttonRESETpressed!=FALSE || homeInfo.GoHome[2]==1) {printf("motor is reseting\n");}
				else if(!TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_REL, CMDGetFromUart.Value.Int32)) {;}
			}
		}
	}
	else if( !strncmp(string,"motor moveto ",13)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{		
				if(buttonRESETpressed==TRUE )
				{	
					printf("motor is resetting\r\n");
				}	
				else if((u8PressAlarm == TRUE || INPUT8==0)  && CMDGetFromUart.Motor==2 )  
				{
					if(CMDGetFromUart.Value.Int32 > Read429Int(IDX_XACTUAL|(CMDGetFromUart.Motor<<5)))
						printf("pressure overhigh, motor z is forbidden to move downward\r\n");		
					else if(!TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_ABS, CMDGetFromUart.Value.Int32)) {;}					
				}	
				else if(!TMC429_MoveToPosition(CMDGetFromUart.Motor,  MVP_ABS, CMDGetFromUart.Value.Int32)) {;}
			}
		}
	}
	else if( !strncmp(string,"motor gohome ",13)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[13];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
					homeInfo.GoHome[CMDGetFromUart.Motor] = TRUE;
				  homeInfo.GoLimit[CMDGetFromUart.Motor]= FALSE;
					homeInfo.Homed[CMDGetFromUart.Motor]	= FALSE;

					homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
				
					SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));
		
				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					
					printf("motor[%d] is start go home by searching home sensor\n",CMDGetFromUart.Motor);
			}
		}
	}
	else if( !strncmp(string,"motor golimit ",14)) 		
	{
		char *p = NULL;	char *p1 = NULL;
		char *s = &string[14];	
		CMDGetFromUart.Motor =strtol(s, &p, 10);	
		
		if(*p==' ')
		{
			if(*p+1=='-')	CMDGetFromUart.Value.Int32 =-strtol(p+2, &p1, 10);		
			else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);		
			if(*p1=='\0' && CMDGetFromUart.Value.Int32!=0)
			{
				  homeInfo.GoHome[CMDGetFromUart.Motor]	=FALSE;
					homeInfo.GoLimit[CMDGetFromUart.Motor]=TRUE;
					homeInfo.Homed[CMDGetFromUart.Motor]	=FALSE;
					homeInfo.HomeSpeed[CMDGetFromUart.Motor]=CMDGetFromUart.Value.Int32;	
					SetAmaxAutoByspeed(CMDGetFromUart.Motor,abs(CMDGetFromUart.Value.Int32));

				  TMC429_MotorRotate(CMDGetFromUart.Motor,CMDGetFromUart.Value.Int32);
					printf("motor[%d] is start go home by searching limit sensor\n",CMDGetFromUart.Motor);

			}
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
//
uint8_t Command_analysis_motor_get(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"motor get help") )		 	printf_cmdList_motor_get();
	
	else if( !strncmp(string,"motor get ",10)) 		
	{
		char *string1 = &string[10];	
				
		if( !strncmp(string1,"speed ",6)) 		
		{
			char *p = NULL;
			char *s = &string1[6];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,actual_speed);
		}
		else if( !strncmp(string1,"position ",9)) 		
		{
			char *p = NULL;
			char *s = &string1[9];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,actual_position);
		}
		else if( !strncmp(string1,"is_homed ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_homed);
		}	
		else if( !strncmp(string1,"is_stop ",8)) 		
		{
				char *p = NULL;
				char *s = &string1[8];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_stop);
		}		
		else if( !strncmp(string1,"is_reach ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,is_reach);
		}		
		else if( !strncmp(string1,"next_speed ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,next_speed);
		}	
		else if( !strncmp(string1,"next_position ",14)) 		
		{
				char *p = NULL;
				char *s = &string1[14];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,next_position);
		}	
		else if( !strncmp(string1,"VMAX ",5)) 		
		{
				char *p = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,max_v_positioning);
		}	
		else if( !strncmp(string1,"AMAX ",5)) 		
		{
				char *p = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,max_acc);
		}	
		else if( !strncmp(string1,"position_reached ",17)) 		
		{
				char *p = NULL;
				char *s = &string1[17];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,position_reached);
		}	
		else if( !strncmp(string1,"rightLimit ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,rightLimit_SwS);
		}
		else if( !strncmp(string1,"leftLimit ",10)) 		
		{
				char *p = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,leftLimit_SwS);
		}
		else if( !strcmp(string1,"limit all")) 		
		{
			TMC429_GetAxisParameter(CMDGetFromUart.Motor,allLimit_SwS);
		}
		else if( !strncmp(string1,"homeSensor ",11)) 		
		{
				char *p = NULL;
				char *s = &string1[11];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,home_SenS);
		}
		else if( !strncmp(string1,"ramp_div ",9)) 		
		{
				char *p = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,ramp_divisor);
		}
		else if( !strncmp(string1,"pulse_div ",10)) 		
		{
				char *p = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p=='\0')	TMC429_GetAxisParameter(CMDGetFromUart.Motor,pulse_divisor);
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
//不需要配置负数
uint8_t Command_analysis_motor_set(char *string)
{
	uint8_t result = REPLY_OK;

	if( !strncmp(string,"motor set ",10)) 		
	{
		char *string1 = &string[10];	
		
		if( !strcmp(string1,"help") )		 	printf_cmdList_motor_set();		
		
		else if( !strncmp(string1,"speed ",6)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[6];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,max_v_positioning,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"next_speed ",11)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[11];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,next_speed,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"next_position ",14)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[14];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,next_position,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"actual_position ",16)) 		
		{
			char *p = NULL;char *p1 = NULL;
			char *s = &string1[16];	
			CMDGetFromUart.Motor =strtol(s, &p, 10);
			if(*p==' ')	
			{
				if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
				else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
			  if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
					TMC429_SetAxisParameter(CMDGetFromUart.Motor,actual_position,CMDGetFromUart.Value.Int32);
			}
		}
		else if( !strncmp(string1,"VMAX ",5)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{	
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,max_v_positioning,CMDGetFromUart.Value.Int32);
				}
		}	
		else if( !strncmp(string1,"AMAX ",5)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[5];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{	
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,max_acc,CMDGetFromUart.Value.Int32);
				
				}
		}	
		else if( !strncmp(string1,"ramp_div ",9)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[9];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,ramp_divisor,CMDGetFromUart.Value.Int32);
				
				}
		}
		else if( !strncmp(string1,"pulse_div ",10)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[10];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,pulse_divisor,CMDGetFromUart.Value.Int32);
				
				}
		}
		else if( !strncmp(string1,"limitSignal ",12)) 		
		{
				char *p = NULL;char *p1 = NULL;
				char *s = &string1[12];	
				CMDGetFromUart.Motor =strtol(s, &p, 10);
				if(*p==' ')	
				{
					CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
					if(*p1=='\0')	TMC429_SetAxisParameter(CMDGetFromUart.Motor,limitSignal,CMDGetFromUart.Value.Int32);	
				}
		}
//		else if( !strncmp(string1,"actual_speed ",13)) 		
//		{
//				char *p = NULL;char *p1 = NULL;
//				char *s = &string1[13];	
//				CMDGetFromUart.Motor =strtol(s, &p, 10);
//				if(*p==' ')	
//				{
//					if(*p+1=='-')	CMDGetFromUart.Value.Int32 =strtol(p+2, &p1, 10);		
//					else 				  CMDGetFromUart.Value.Int32 =strtol(p+1, &p1, 10);			
//					if(*p1=='\0'&& CMDGetFromUart.Value.Int32!=0)	
//						TMC429_SetAxisParameter(CMDGetFromUart.Motor,actual_speed,CMDGetFromUart.Value.Int32);
//				}
//		}	
	}

	else result = REPLY_INVALID_CMD;
	return result;
}
//


//


