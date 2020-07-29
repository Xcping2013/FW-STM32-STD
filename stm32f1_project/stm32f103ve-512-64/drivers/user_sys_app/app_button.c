/*********************************************************************************************************************/
#include "app_gpio_dido.h"	
#include "app_motor_home.h"
#include "app_button.h"
/*********************************************************************************************************************/
uint8_t buttonSTARTpressed=FALSE;
uint8_t buttonRESETpressed=FALSE;

static uint8_t buttonSTARTCNT=0;
static uint8_t buttonRESETCNT=0;

/*********************************************************************************************************************/
void buttonSTART_process(uint8_t inCh, uint8_t outCh)
{
	if(buttonRESETpressed==FALSE)																									  //电机在复位中，START无效
	{
		if( getChInput(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			buttonSTARTCNT++;	
			if( buttonSTARTCNT>=2)											 																//开始按键
			{
				 setChOutput(1,1);	setChOutput(2,0);							 									 			
				 buttonSTARTpressed=TRUE;  		
				 printf("buttonSTART=1\r\n");
			}
		}
		else if( getChInput(inCh)==IN_OFF)
		{
			buttonSTARTpressed=FALSE; //由命令下发解锁 改为自动解锁
			buttonSTARTCNT=0;	
		}
	}
}
/*
回原点速度从EEPROM读出
Bx35_button: Y轴先里后出 ---> Z轴先上后下 ---> X轴先左后右

B445_button: Z轴先上后下回原点后再往上运行一段位置放置传感器撞到Holder---> Y轴先里后出---> X轴先左后右

ROAD			 ：Z轴先上后下				
*/
void buttonRESET_process(uint8_t inCh, uint8_t outCh)
{
	if( getChInput(inCh)==IN_ON && buttonRESETpressed==FALSE )							
	{
		buttonRESETCNT++;		
		if( buttonRESETCNT>=2)			
		{		
			MotorAutoReset_preset();	
		}
	}
	else if( getChInput(inCh)==IN_OFF)
	{
		//buttonRESETpressed=FALSE; 复位完成自动解锁
		buttonRESETCNT=0;	
	}
}

int buttonSTART(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))	printf("buttonSTART=%d\r\n",buttonSTARTpressed);
		if (!strcmp(argv[1], "enable")) 
		{
			buttonSTARTpressed=FALSE;
			setChOutput(1,0);	
			printf("enable button START for next test\r\n");		
		}
	}
	else
	{
		printf("Usage: \r\n");
		printf("buttonSTART status          -button START is pressed or not\r\n");
		printf("buttonSTART enable          -enable button START for next test\r\n");
	}
	return 0;
}
static void printf_cmdList_button(void)
{
	printf("button Usage: \r\n");
	printf("buttonSTART status          -button START is pressed or not\r\n");
	printf("buttonSTART enable          -enable button START for next test\r\n");	
}

uint8_t Command_analysis_button(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"buttonSTART status") )		 		 printf("buttonSTART=%d\r\n",buttonSTARTpressed);
	else if( !strcmp(string,"buttonSTART enable") )
	{
			buttonSTARTpressed=FALSE;
			setChOutput(1,0);	
			printf("enable button START for next test\r\n");	
	}
	
	else if( !strcmp(string,"button help") )			printf_cmdList_button();
	else result = REPLY_INVALID_CMD;
	return result;
}
//


