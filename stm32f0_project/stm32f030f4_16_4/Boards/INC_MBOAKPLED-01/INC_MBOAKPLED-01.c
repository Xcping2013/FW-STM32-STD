
#include "main.h"

#include "INC_MBOAKPLED-01.h"

/**************************************************************************************/
//PA0	---LED1_CN1
//PA1	---LED1_CN2
//PA2	---LED2_CN1
//PA3	---LED2_CN2
//PB1	---LED
//PA9	---MCU_TX
//PA10---MCU_RX
/**************************************************************************************/
#define  LED_TOGGLE_MBOAKPLED()	  GPIOB->ODR ^= BIT1		

RCC_ClocksTypeDef RCC_Clocks;

char PromptString[]=">>";
char version_string[]="INC-MBMBOAKPLED-Ver1.0";

static void INC_MBOAKPLED_hw_init(void);
static uint8_t Command_INC_MBOAKPLED(char* string);
static void INC_MBOAKPLED_CMDList(void);
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
	
	INC_MBOAKPLED_hw_init();
		
	delay_ms(20);	
	
	BlinkDelay=SysTick_GetTimer();
	
  while (1)
  {
  	if(abs((int)(SysTick_GetTimer()-BlinkDelay))>350)
  	{
  		BlinkDelay=SysTick_GetTimer();
			
			LED_TOGGLE_MBOAKPLED();
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
		if(USART1_RX_BUF[0]=='\0')																{printf("%s",PromptString);}	
		else if(!Command_Analysis_Process((char*)USART1_RX_BUF))	{printf("%s",PromptString);}	
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
	
	if(!Command_INC_MBOAKPLED(string))		;
		
	else if(!strcmp("help",string))	
	{
//		printf("> Version\r\n");
//		printf(">	reboot:\r\n");	
		INC_MBOAKPLED_CMDList();	
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
void INC_MBOAKPLED_hw_init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);			

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
	
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
		GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 );	
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);			
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 ;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOB, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIOB, GPIO_Pin_1 );	
}
static uint8_t Command_INC_MBOAKPLED(char* string)
{
	uint8_t result = REPLY_OK;

	if( !strcmp(string,"version")) 
	{				
		printf("Fixture Name:OAKP\r\n");
		printf("Vendor:INC\r\n");
		printf("FW Version:V1.0\r\n");
		printf("Update Date:20200729\r\n");		
		return REPLY_OK;
	}		
	if( !strcmp(string,"led1 off")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_0);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_1);	
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"led1 red on")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_0);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_1);	
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"led1 green on")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_1);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_0);	
		printf("Pass\r\n");
		return REPLY_OK;
	}		
	if( !strcmp(string,"led2 off")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_2);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_3);	
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"led2 red on")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_2);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_3);	
		printf("Pass\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"led2 green on")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_3);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_2);	
		printf("Pass\r\n");
		return REPLY_OK;
	}		
	else result = REPLY_INVALID_CMD;
	return result;
}
static void INC_MBOAKPLED_CMDList(void)
{
	printf("MBOAKPLED V1.0\r\n");	
	printf("Author: Canping\r\n");
	printf("----------------------\r\n");
//	printf("> Version\r\n");		
//	printf("> WriteDO [index] [state]\r\n");		
//	printf("> ReadDI\r\n");		
//	printf("> ReadDO\r\n");		
//	printf("> Test\r\n");		
//	printf("> WriteEEprom [addr] [value]\r\n");	
//	printf("> ReadEEprom [addr]\r\n");
	printf("> led1|2 off\r\n");
	printf("> led1|2 red on\r\n");
	printf("> led1|2 green on\r\n");
	printf("----------------------\r\n");	
}
//
#endif

/*
 新增LED模块：
1.串口波特率115200 命令以\r或\r\n结束，返回命令以\r\n>>结束
2.命令：

led1 off 		   --- 第一个双色灯不亮
led2 off  	   --- 第二个双色灯不亮

led1 red on    --- 第一个双色灯亮红灯
led1 green on  --- 第一个双色灯亮绿灯

led2 red on    --- 第2个双色灯亮红灯
led2 green on  --- 第2个双色灯亮绿灯


*/





