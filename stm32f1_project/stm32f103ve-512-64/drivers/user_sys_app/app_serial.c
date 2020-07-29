#include "app_serial.h"
#include "app_motor_tmc429.h"
#include "app_gpio_dido.h"
#include "app_button.h"
static uint8_t 	DataRemoveCnt=0;				
static uint8_t 	AxisSpeedKeepZeroCnt=0;

void UART3_EnableRX(void)
{
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
}
void UART3_DisableRX(void)
{
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
}

static int printdata(int argc, char **argv)
{
//	if (argc !=1)
//	{
//		printf("Please input: printdata\r\n");
//		return RT_ERROR;
//	}	
	AxisSpeedKeepZeroCnt=0;

	UART3_EnableRX();
	return 0;
}
static uint8_t input8Cnt=0;
static int next_posititon=0;
static int current_posititon=0;

static void serial3_data_processing(void)
{
	if(Read429Short(IDX_VACTUAL|(AXIS_Z<<5)) ==0 )				
	{
		if(AxisSpeedKeepZeroCnt++ > 1)
		{
			AxisSpeedKeepZeroCnt=0;
			UART3_DisableRX();
			printf("motor[2] is stop and stop printing data\r\n>>");
		}
	}
	else 
	{
		AxisSpeedKeepZeroCnt=0;
		if(DataRemoveCnt<2)	DataRemoveCnt++;
		else 
		{
			current_posititon=Read429Int(IDX_XACTUAL|(AXIS_Z<<5));
			if(next_posititon > current_posititon && buttonRESETpressed==FALSE)
			{
				if(INPUT8==0) input8Cnt++; else input8Cnt=0;		
				if(input8Cnt>=3) {HardStop(2);input8Cnt=0;}							
			}
			printf("P[2]=%d,Press%s\r\n",current_posititon,(char*)(&USART3_RX_BUF));			
		}
	}		
}
//
void PrintfUart3DataToUart1(void)
{
	u8 len;
	if(USART3_RX_STA&0x8000)	
	{
		len=USART3_RX_STA&0x3fff;	
		USART3_RX_BUF[len]='\0';
		serial3_data_processing();
		USART3_RX_STA=0; 
	}
}
void printf_cmdList_uart3(void)
{
	printf("UART3 Usage: \r\n");
	printf("printdata:\r\n");			
	printf("stop printdata:\r\n");		
	printf("uart3 reinit:\r\n");	
}
	
uint8_t Command_analysis_uart3(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"uart3 help") )		 				printf_cmdList_uart3();
	else if( !strcmp(string,"printdata") )		  
	{	
		AxisSpeedKeepZeroCnt=0;											
		next_posititon=Read429Int(IDX_XTARGET|(2<<5));		
		UART3_EnableRX();
	}
	else if( !strcmp(string,"stop printdata") )		
	{
		UART3_DisableRX();
		printf("stop printing the pressure value ok\r\n");		
	}
	else if( !strcmp(string,"uart3 reinit") )		
	{
		InitUART_2(115200);
		printf("stop printing the pressure value ok\r\n");		
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
//









