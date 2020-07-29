
#include "main.h"

#include "INC-MBVBUSRELAY4.h"

/**************************************************************************************/
//PA0	---VBUS1_CN
//PA1	---VBUS2_CN
//PA2	---VBUS3_CN
//PA3	---VBUS4_CN
//PA5	---LED
//PA9	---MCU_TX
//PA10---MCU_RX
/**************************************************************************************/
#define  LED_TOGGLE_MBVBUSRELAY4()	  GPIOA->ODR ^= BIT5		

RCC_ClocksTypeDef RCC_Clocks;

char version_string[]="INC-MBVBUSRELAY-Ver1.0";

static void INC_MBVBUSRELAY4_hw_init(void);
static uint8_t Command_INC_MBVBUSRELAY4(char* string);
static void INC_MBVBUSRELAY4_CMDList(void);
/**************************************************************************************/
static uint8_t  Command_Analysis_Process(char  *Commands);
static void 	  CommandCheckAndExeViaUart1(void);


/**************************************************************************************/
#if 1		//通用
static void MakeLowercase( char *str)		
{
    while(*str!='\0')
    {
        if( ((*str)>='A')&&((*str)<='Z') )
        {
            *str=*str+32;
        }
        str++;
    }
}
void user_show_version(void)
{
    printf("\r\n \\ | / \r\n");
    printf("- INC -     AUTOMATION\r\n");
    printf(" / | \\     \r\n");
		printf("\r\ncontroller init ok\r\n");
	  printf("\r\nfirmware %s build at %s %s\r\n\r\n", version_string, __TIME__, __DATE__);
	
	  printf("\r\nyou can type commands end with \\r\r\n");

}
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	u32 BlinkDelay;
	
  RCC_GetClocksFreq(&RCC_Clocks);
	
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//1ms
  
	delay_ms(20);	
	
	uart1_init(115200);		
  
	user_show_version();
	
	INC_MBVBUSRELAY4_hw_init();
		
	delay_ms(20);	
	
	BlinkDelay=SysTick_GetTimer();
	
  while (1)
  {
  	if(abs((int)(SysTick_GetTimer()-BlinkDelay))>350)
  	{
  		BlinkDelay=SysTick_GetTimer();
			
			LED_TOGGLE_MBVBUSRELAY4();
  	}	
		CommandCheckAndExeViaUart1();

		AutoTask0_per_ms(10);
		//Uart1Task_test(5);	
  }
}


void CommandCheckAndExeViaUart1(void)
{
	u8 len;
	if(USART1_RX_STA&0x8000)								//接收到\r\n						
	{	
		len=USART1_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART1_RX_BUF[len]='\0';							//在末尾加入结束符. 
		printf((char*)USART1_RX_BUF);
		printf("\r\n");	
		if(USART1_RX_BUF[0]=='\0')																{printf("@_@");}	
		else if(!Command_Analysis_Process((char*)USART1_RX_BUF))	{printf("@_@");}
		else 
		{
			printf("This command is illegal,please check it again\r\n@_@");
			//printf("commands no found , type help to see commands list \r\n@_@");	
			//printf("The interval between sending two commands is better than 50ms\r\n>>");
		}	
		USART1_RX_STA=0;
	}	
}




uint8_t  Command_Analysis_Process( char* string)
{
	uint8_t result = REPLY_OK;
	
	MakeLowercase(string);
	
	if(!Command_INC_MBVBUSRELAY4(string))		;
		
	else if(!strcmp("help",string))	
	{
//		printf("> Version\r\n");
//		printf(">	reboot:\r\n");	
		INC_MBVBUSRELAY4_CMDList();	
	}
//	else if(!strcmp("version",string))
//	{
//		//printf("firmware %s build at %s %s\r\n", version_string ,__TIME__, __DATE__);
//		printf("firmware %s build at %s %s\r\n", version_string ,__TIME__, __DATE__);
//	}
	else if(!strcmp("reboot",string))
	{
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else result = REPLY_CHKERR;
	return result;
}

#endif
//
#if 1	 //板卡应用
void INC_MBVBUSRELAY4_hw_init(void)
{

		GPIO_InitTypeDef GPIO_InitStruct;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);			

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5;
	
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
		GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5);	
}
static uint8_t Command_INC_MBVBUSRELAY4(char* string)
{
	uint8_t result = REPLY_OK;

	if( !strcmp(string,"version")) 
	{				
		printf("Fixture Name:630CCB2016B Demo\r\n");
		printf("Vendor:INNOREV\r\n");
		printf("FW Version:V1.0\r\n");
		printf("Update Date:20181118\r\n");		
		return REPLY_OK;
	}		
	
	if( !strcmp(string,"writedo 0x01 0x01")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_0);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"writedo 0x01 0x00")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_0);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"writedo 0x02 0x01")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_1);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"writedo 0x02 0x00")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_1);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"writedo 0x03 0x01")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_2);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"writedo 0x03 0x00")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_2);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"writedo 0x04 0x01")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_3);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"writedo 0x04 0x00")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_3);		
				
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	else result = REPLY_INVALID_CMD;
	return result;
}
static void INC_MBVBUSRELAY4_CMDList(void)
{
	printf("630CCB2016B Demo V1.0\r\n");	
	printf("Author: Jansow\r\n");
	printf("----------------------\r\n");
	printf("> Version\r\n");		
	printf("> WriteDO [index] [state]\r\n");		
	printf("> ReadDI\r\n");		
	printf("> ReadDO\r\n");		
	printf("> Test\r\n");		
	printf("> WriteEEprom [addr] [value]\r\n");	
	printf("> ReadEEprom [addr]\r\n");
	printf("----------------------\r\n");	
}
//
#endif

//





