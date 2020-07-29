
#include "main.h"
#include "delay.h"
#include "uart1.h"

/**************************************************************************************/
#include "pcm1.h"
#include "lcomp_e75_test.h"
#include "user_test.h"
#include "okoa_led_test.h"

#include "inc_mbad9834.h"
#include "INC-MBVBUSRELAY.h"
#include "INC-MBDIOCN1.h"

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                               /* Reset value, write to SCB_AIRCR can reset cpu */


/**************************************************************************************/
RCC_ClocksTypeDef RCC_Clocks;

/* sensor\r\n<0|>>>\r\n   ---> sensor<0|1>>>\r\n
   auto toggle bug
   add disable commands
*/
char version_string[]="ver1.01";

char SN_STRING[]="2020EF0755A2";
/**************************************************************************************/
static uint8_t  Command_Analysis_Process(char  *Commands);
static void 	  CommandCheckAndExeViaUart1(void);

/**************************************************************************************/
void user_show_version(void)
{
    printf("\r\n \\ | / \r\n");
    printf("- INC -     AUTOMATION\r\n");
    printf(" / | \\     \r\n");
	  printf("\r\nfirmware %s build at %s %s\r\n\r\n", version_string, __TIME__, __DATE__);

}
void CommandCheckAndExeViaUart1_137(void)
{
	u8 len;
	if(USART1_RX_STA&0x8000)									//接收到\r\n						
	{	
		len=USART1_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART1_RX_BUF[len]='\0';								//在末尾加入结束符. 
		//printf((char*)USART1_RX_BUF);
		//printf("\r\n");	
		if(USART1_RX_BUF[0]=='\0')																{}	
		else if(!Command_Analysis_Process((char*)USART1_RX_BUF))	{}
		else 
		{
			printf("\r\ncommands no found , type help to see commands list\r\n");	
			
		}
		
		USART1_RX_STA=0;
	}	
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
  
	delay_ms(100);	
	//uart1_init(9600);
//	uart1_init(115200);
	
	uart1_init(57600);	//J137 AUDIT
  
	user_show_version();
		
	delay_ms(100);	
	
	//MBLEDRW_HW_Init();
	//INC_MBAD9834_hw_init();
	//INC_MBVBUSRELAY_hw_init();
//	INC_MBDIOCN1_hw_init();
//	printf("The interval between sending two commands is better than 50ms\r\n>>");
	BlinkDelay=SysTick_GetTimer();
	
  while (1)
  {
  	if(abs((int)(SysTick_GetTimer()-BlinkDelay))>350)
  	{
  		BlinkDelay=SysTick_GetTimer();
			
			//okoa_led_test LED_RUN
			//LED_TOGGLE();
			//LED_TOGGLE_MBAD9834();
			//LED_TOGGLE_MBVBUSRELAY();
			//LED_TOGGLE_MBDIOCN1();
  	}
		
		//CommandCheckAndExeViaUart1();
		CommandCheckAndExeViaUart1_137();
		//KeyS1=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
		AutoTask_per_ms(10);
		//Uart1Task_test(5);	
  }
}


void CommandCheckAndExeViaUart1(void)
{
	u8 len;
	if(USART1_RX_STA&0x8000)									//接收到\r\n						
	{	
		len=USART1_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART1_RX_BUF[len]='\0';								//在末尾加入结束符. 
		printf((char*)USART1_RX_BUF);
		//printf("\r\n");	
		if(USART1_RX_BUF[0]=='\0')																{printf(">>");}	
		else if(!Command_Analysis_Process((char*)USART1_RX_BUF))	{printf(">>");}
		else 
		{
			printf("\r\ncommands no found , type help to see commands list\r\n");	
			printf("The interval between sending two commands is better than 50ms\r\n>>");
		}
		
		USART1_RX_STA=0;
	}	
}




uint8_t  Command_Analysis_Process( char* string)
{
	uint8_t result = REPLY_OK;
	if(!Command_INC_MBDIOCN1(string))		;
//	if(!Command_INC_MBVBUSRELAY(string))		;
//	if(!Command_INC_MBAD9834(string))		;
//	if(!Command_analysis_PCM1(string))		;
//	if(!Command_analysis_LCOMP(string))		;
//	if(!Command_analysis_LED_TEST(string))		;
		
	else if(!strcmp("help",string))	
	{
		printf("version\r\n");
		
		printf("audit.getsn()\r\n");
//		printf("reboot:\r\n");
		
//		printf("led help:\r\n");
//		
//		printf("pcm1 help:\r\n");
//		printf("lcomp help:\r\n");
//		INC_MBDIOCN1_CMDList();
//		INC_MBVBUSRELAY_CMDList();
		
	}
	else if(!strcmp("version",string))
	{
		printf("firmware %s build at %s %s\r\n", version_string ,__TIME__, __DATE__);
	}
	else if(!strcmp("reboot",string))
	{
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else if(!strcmp("audit.getsn()",string))
	{
		printf("ok@audit.getsn(\"2017EF0755A2\")\r\n");
	}
		else if(!strcmp("audit.getsn( )",string))
	{
		printf("ok@audit.getsn(\"2017EF0755A2\")\r\n");
	}
//	else if(!strncmp("audit.setsn(",string,12))
//	{
//		printf("ok@audit.getsn(\"2017EF0755A2\")\r\n");
//	}	
	else result = REPLY_CHKERR;
	return result;
}




//

