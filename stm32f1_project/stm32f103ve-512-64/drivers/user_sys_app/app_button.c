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
	if(buttonRESETpressed==FALSE)																									  //����ڸ�λ�У�START��Ч
	{
		if( getChInput(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			buttonSTARTCNT++;	
			if( buttonSTARTCNT>=2)											 																//��ʼ����
			{
				 setChOutput(1,1);	setChOutput(2,0);							 									 			
				 buttonSTARTpressed=TRUE;  		
				 printf("buttonSTART=1\r\n");
			}
		}
		else if( getChInput(inCh)==IN_OFF)
		{
			buttonSTARTpressed=FALSE; //�������·����� ��Ϊ�Զ�����
			buttonSTARTCNT=0;	
		}
	}
}
/*
��ԭ���ٶȴ�EEPROM����
Bx35_button: Y�������� ---> Z�����Ϻ��� ---> X���������

B445_button: Z�����Ϻ��»�ԭ�������������һ��λ�÷��ô�����ײ��Holder---> Y��������---> X���������

ROAD			 ��Z�����Ϻ���				
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
		//buttonRESETpressed=FALSE; ��λ����Զ�����
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


