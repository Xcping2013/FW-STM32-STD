/**	
  \file 	  main.c
  \author 	Xcping2016	2016-5-10			2018-1-5	20190803
  \version 	1.00
  \brief   	Main file
*/

/*	压力报警的安全监控

1.	IN8中断 回原点状态下触发报警,向上旋转找原点 否则电机停止 停止后屏蔽向下运动							H->L 触发一次,不管方向运动  复位L-H 不影响

																									  moveto命令先进行目标位置判断	
 
2.	打印串口3压力值 判断是否为向下运动并触发报警    非复位状态下目标位置判断

3.  任务采集报警输入电平,正转触发，立刻停止					当前速度判断,reset速度为<0 不影响



*/


#include "bsp_mcu_delay.h"
#include "bsp_mcu_uart.h"
#include "bsp_mcu_exti.h"
#include "app_gpio_dido.h"
#include "app_eeprom_24xx.h"

#include "app_serial.h"
#include "app_button.h"
#include "app_motor_home.h"
#include "app_sys_control.h"
#include "app_task_tick.h"

#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                               /* Reset value, write to SCB_AIRCR can reset cpu */

uint8_t Command_analysis_os_check(char *string);

uint8_t  Command_Analysis_Process(char  *Commands);
void 		 CommandCheckAndExeViaUart1(void);

char version_string[]="ver3.6.1";

void user_show_version(void)
{
    printf("\r\n \\ | / \r\n");
    printf("- INC -     AUTOMATION\r\n");
    printf(" / | \\     INC-MBTMC429-01\r\n");
//    printf(" 2015 - 2019 Copyright by rt-thread team\n");
//		rt_kprintf("The board type is INC-MBTMC429-01\n");
	  printf("\r\nfirmware %s build at %s %s\r\n\r\n", version_string, __TIME__, __DATE__);

}

int main(void)
{ 
	uint8_t i=0;
	
  U32_T u32BlinkDelay;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
	EnableInterrupts();
	
  SysTick_Init();

	InitUART_1(230400);		
	
	user_show_version();
	
//	InitUART_2(115200);		
	
//  dido_hw_init();
//	
//	at24cxx_hw_init();

//	tmc429_hw_init();
//	
//	EXTI_Init_IN8();
	
	delay_ms(200);															
	
	printf(">>");
	
  u32BlinkDelay=SysTick_GetTimer();
	
  for(;;)
  {		
		for(i=0;i<N_O_MOTORS;i++)
		{			
			MotorHomingWithHomeSensor(i,homeInfo.HomeSpeed[i]);
			MotorHomingWithLimitSwitch(i,homeInfo.HomeSpeed[i]);
		}	
		
		CommandCheckAndExeViaUart1();
		
		PrintfUart3DataToUart1();
		
		KeyTask_every_ms(10);
		
		PressAlarmTask_every_ms(2);
		
  	if(abs((int)(SysTick_GetTimer()-u32BlinkDelay))>350)
  	{
			pinToggle(PD_7);
  		u32BlinkDelay=SysTick_GetTimer();
  	}
  }
}

void CommandCheckAndExeViaUart1(void)
{
	u8 len;
	if(USART1_RX_STA&0x8000)									//接收到\r\n						
	{	
		len=USART1_RX_STA&0x3fff;							//得到此次接收到的数据长度
		USART1_RX_BUF[len]='\0';								//在末尾加入结束符. 
		//printf((char*)USART1_RX_BUF);
		//printf("\r\n");	
		if(USART1_RX_BUF[0]=='\0')																{;}//{printf(">>");}	
		else if(!Command_Analysis_Process((char*)USART1_RX_BUF))	{;}//{printf(">>");}
		else printf("commands no found , type help to see commands list\r\n>>");			
		
		USART1_RX_STA=0;
	}	
}

uint8_t  Command_Analysis_Process( char* string)
{
	uint8_t result = REPLY_OK;
	
	if(!Command_analysis_dido(string))						{;}
	else if(!Command_analysis_button(string))			{;}
	else if(!Command_analysis_eeprom(string))			{;}
	else if(!Command_analysis_uart3(string))			{;}
	
	else if(!Command_analysis_motor(string))			{;}
	else if(!Command_analysis_motor_get(string))	{;}
	else if(!Command_analysis_motor_set(string))	{;}
	
		else if(!Command_analysis_os_check(string))	{;}
			
	else if(!strcmp("help",string))	
	{
		printf("gpio help:\r\n");
		printf("button help:\r\n");
		printf("eeprom help:\r\n");
		printf("uart3 help:\r\n");
		
		printf("motor help:\r\n");
		printf("motor get help:\r\n");
		printf("motor set help:\r\n");
		
		printf("version:\r\n");
		printf("reboot:\r\n");
		
	}
	else if(!strcmp("version",string))
	{
		printf("firmware %s build at %s %s\r\n", version_string ,__TIME__, __DATE__);
	}
	else if(!strcmp("reboot",string))
	{
		SCB_AIRCR = SCB_RESET_VALUE;		
	}
	else if(!strcmp("sys reset",string))
	{
		ResetCPU(1);
	}
	else if(!strcmp("core reset",string))
	{
		ResetCPU(0);
	}
	else result = REPLY_CHKERR;
	return result;
}

//20200603 add os_check_commands
uint8_t Command_analysis_os_check(char *string)
{
	uint8_t result = REPLY_OK;

	if( !strcmp(string,"[tunnel-R-0]") )	
	{	
			printf("[tunnel-R-0]<tunnel-R-0>\r\n[00105098] TUNNEL: OPEN\r\n");		
	}
	else if( !strcmp(string,"syscfg print MLB#") )		
	{
		printf("syscfg print MLB#\r\n> syscfg:ok \"GFH8293001GJJNV1H\"\r\n]\r\n");		
	}
	else if( !strcmp(string,"ft upload 0 16") )		
	{
		printf("ft upload 0 16\r\n> ft:ok begin");		
	}
	else if( !strcmp(string,"00000000010000000000000000000000") )		
	{
		printf("00000000010000000000000000000000\r\n> ft:ok done 0x771E073A\r\n]\r\n");		
	}

	else if( !strcmp(string,"00000000000000000000000000000000") )		
	{
		printf("00000000000000000000000000000000\r\n> ft:ok done 0xECBB4B55\r\n]\r\n");		
	}
	else if( !strcmp(string,"syscfg add ClfE 16") )		
	{
		printf("syscfg add ClfE 16\r\n> syscfg:ok\r\n]\r\n");		
	}
	
	else if( !strcmp(string,"ft tunnel close") )		
	{
		printf("ft tunnel close\r\n> ft:ok\r\n[00115170] TUNNEL: CLOSED\r\n");		
	}
	
	else if( !strcmp(string,"ft reset") )		
	{
		printf("ft reset\r\n>[00108943] TUNNEL: CLOSED\r\n");		
	}
	else if( !strcmp(string,"[tunnel-R-0]") )		
	{
		printf("[tunnel-R-0]<tunnel-R-0>\r\n[00110169] TUNNEL: OPEN\r\n]\r\n");		
	}	
	
	
	else if( !strcmp(string,"ft upload 0 16") )		
	{
		printf("ft upload 0 16\r\n> ft:ok begin\r\n");		
	}	
	
	else if( !strcmp(string,"beryl iedtest raw 15 3300") )		
	{
		printf("beryl iedtest raw 15 3300\r\n");		
		printf("concha_raw_on   |concha_scale_on |concha_raw_off  |concha_scale_off|\r\n");		
		printf("0x0070          |0x0070          |0x003e          |0x003e          |samplecount = 0x0001\r\n");			
		printf("0x0070          |0x0070          |0x003e          |0x003e          |samplecount = 0x0002\r\n");		
		printf("0x0072          |0x0072          |0x0044          |0x0044          |samplecount = 0x0003\r\n");		
		printf("0x0072          |0x0072          |0x003f          |0x003f          |samplecount = 0x0004\r\n");		
		printf("0x0074          |0x0074          |0x0041          |0x0041          |samplecount = 0x0005\r\n");		
		printf("0x0074          |0x0074          |0x0041          |0x0041          |samplecount = 0x0006\r\n");		

		printf("0x0073          |0x0073          |0x0041          |0x0041          |samplecount = 0x0007\r\n");		
		printf("0x0071          |0x0071          |0x0041          |0x0041          |samplecount = 0x0008\r\n");		
		printf("0x0072          |0x0072          |0x0040          |0x0040          |samplecount = 0x0009\r\n");		
		printf("0x0072          |0x0072          |0x0040          |0x0040          |samplecount = 0x000a\r\n");		

		
		printf("0x0072          |0x0072          |0x0043          |0x0043          |samplecount = 0x000b\r\n");		
		printf("0x0071          |0x0071          |0x003e          |0x003e          |samplecount = 0x000c\r\n");		
		printf("0x0075          |0x0075          |0x0043          |0x0043          |samplecount = 0x000d\r\n");		
		printf("0x0072          |0x0072          |0x0041          |0x0041          |samplecount = 0x000e\r\n");		
		printf("0x0074          |0x0074          |0x003f          |0x003f          |samplecount = 0x000f\r\n");		
		printf("\r\n> beryl:ok\r\n]\r\n");		

	}	
	
	else result = REPLY_INVALID_CMD;
	return result;
}
//



