#include "pcm1.h"
#include "delay.h"

#define SYS_LED_Pin GPIO_Pin_0
#define SYS_LED_GPIO_Port GPIOA
#define K10_GPIO_Pin GPIO_Pin_1
#define K10_GPIO_GPIO_Port GPIOA
#define K1_GPIO_Pin GPIO_Pin_4
#define K1_GPIO_GPIO_Port GPIOA
#define K2_GPIO_Pin GPIO_Pin_5
#define K2_GPIO_GPIO_Port GPIOA
#define K9_GPIO_Pin GPIO_Pin_6
#define K9_GPIO_GPIO_Port GPIOA
#define K4_GPIO_Pin GPIO_Pin_7
#define K4_GPIO_GPIO_Port GPIOA
#define K5_GPIO_Pin GPIO_Pin_1
#define K5_GPIO_GPIO_Port GPIOB
#define K7_GPIO_Pin GPIO_Pin_9
#define K7_GPIO_GPIO_Port GPIOA
#define K6_GPIO_Pin GPIO_Pin_10
#define K6_GPIO_GPIO_Port GPIOA
/**************************************************************************************/
GPIO_TypeDef *RELAY_port[8]={
GPIOA,
GPIOA,
//GPIOA,
GPIOA,
GPIOB,
GPIOA,
GPIOA,
//GPIOA,
GPIOA,
GPIOA
};
uint16_t	RELAY_pin[8]=
{
	K1_GPIO_Pin,
	K2_GPIO_Pin,
	//K2_GPIO_Pin,
	K4_GPIO_Pin,
	K5_GPIO_Pin,
	K6_GPIO_Pin,
	K7_GPIO_Pin,
	//K7_GPIO_Pin,
	K9_GPIO_Pin,
	K10_GPIO_Pin,
};

/**************************************************************************************/
static void pcm1_init(void);
static void relay_close(uint8_t number);
static void relay_open(uint8_t number);
static void relay_status(uint8_t number);

static void printf_cmdList_PCM1(void);

/**************************************************************************************/
static void pcm1_init(void)
{
	static uint8_t pcm1_inited=0;
	if(pcm1_inited==0)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = SYS_LED_Pin|K1_GPIO_Pin| K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		GPIO_ResetBits(GPIOA, SYS_LED_Pin|K1_GPIO_Pin| K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);
		
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		GPIO_InitStruct.GPIO_Pin = K5_GPIO_Pin;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		GPIO_ResetBits(GPIOB, K5_GPIO_Pin );
		
		pcm1_inited=1;
	}	
}

static void relay_close(uint8_t number)
{
	if(0<number && number<11)
	{
		if(number<3)  GPIO_SetBits(RELAY_port[number-1], RELAY_pin[number-1]);
		else if(number<8)  GPIO_SetBits(RELAY_port[number-2], RELAY_pin[number-2]);
		else if(number<11) GPIO_SetBits(RELAY_port[number-3], RELAY_pin[number-3]);
		PCM1_TRACE("<ok>\r\n");
	}
}

static void relay_open(uint8_t number)
{
	if(0<number && number<11)
	{
		if(number<3)  						GPIO_ResetBits(RELAY_port[number-1], RELAY_pin[number-1]);
		else if(number<8)  				GPIO_ResetBits(RELAY_port[number-2], RELAY_pin[number-2]);
		else if(number<11) 				GPIO_ResetBits(RELAY_port[number-3], RELAY_pin[number-3]);
		PCM1_TRACE("<ok>\r\n");
	}
}

static void relay_status(uint8_t number)
{
	if(0<number && number<11)
	{
		if(( RELAY_port[number-1]->ODR & RELAY_pin[number-1] ) == 0 )
		PCM1_TRACE("<open>\r\n");
		else 
		PCM1_TRACE("<close>\r\n");
	}
}


/**************************************************************************************/
static void printf_cmdList_PCM1(void)
{
		printf("\r\npcm1 command list:\r\n");
		printf("relay_set(xxxxxxxx)---------set relays status 1-close 0-open\r\n");
		printf("relay_open(channel)---------open the relay,channel=1,2,4,5,6,7,9,10\r\n");
		printf("relay_close(channel)--------close the relay,channel=1,2,4,5,6,7,9,10\r\n");
		printf("relay_status(channel)-------get the relay status,channel=1,2,4,5,6,7,9,10\r\n");
		printf("switch to enable pcm--------\r\n");
		printf("switch to record V----------\r\n");
		printf("switch to record I----------\r\n");
		printf("vbat discharge--------------\r\n");
		printf("enable buddy link-----------\r\n");
		printf("disable buddy link----------\r\n");
		printf("all open--------------------open all relays\r\n");
}
//
uint8_t Command_analysis_PCM1(char* string)
{
	uint8_t result = REPLY_OK;
	if(strncmp("relay_set(",string, 10)==0	&&  string[18] == ')' )
	{
		uint8_t i;
		
		pcm1_init();
		
		for(i=0;i<8;i++)
		{
			if(string[10+i]=='0')
			{
				GPIO_ResetBits(RELAY_port[i], RELAY_pin[i]);
				
			}
			else GPIO_SetBits(RELAY_port[i], RELAY_pin[i]);
		}	
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strncmp("relay_open(", string,11)==0)	
	{
		char *p = NULL;
		char *s = &string[11];
		
		uint8_t channel=strtol(s, &p, 10);	
		
		pcm1_init();
		
		if(*p==')' && *(p+1)=='\0')		relay_open(channel);
	}
	else if(strncmp("relay_close(",	 string,12)==0)	
	{
		char *p = NULL;
		char *s = &string[12];
		uint8_t channel=strtol(s, &p, 10);	
		
		pcm1_init();
		
		if(*p==')' && *(p+1)=='\0')		relay_close(channel);
	}	
	else if(strncmp("relay_status(",	 string,13)==0)	
	{
		char *p = NULL;
		char *s = &string[13];
		uint8_t channel=strtol(s, &p, 10);	
		
		pcm1_init();
		if(*p==')' && *(p+1)=='\0')			relay_status(channel);
	}	
	else if(strcmp("all open",						 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		PCM1_TRACE("<ok>\r\n");	
	}
	else if(strcmp("switch to enable pcm",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K1_GPIO_Pin|K4_GPIO_Pin|K9_GPIO_Pin);	
		delay_ms(1000);		
		GPIO_ResetBits(GPIOA,K4_GPIO_Pin);
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("switch to record V",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K1_GPIO_Pin|K6_GPIO_Pin|K9_GPIO_Pin);	
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("switch to record I",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin);	
		delay_ms(500);
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin);
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("vbat discharge",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K10_GPIO_Pin);
		delay_ms(5000);				
		GPIO_SetBits(GPIOA,K10_GPIO_Pin);	
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("enable buddy link",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K1_GPIO_Pin|K9_GPIO_Pin);	
		GPIO_SetBits(GPIOB,K5_GPIO_Pin);	
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("disable buddy link",	 string)==0)	
	{
		pcm1_init();
		GPIO_ResetBits(GPIOA,K1_GPIO_Pin|K2_GPIO_Pin|K4_GPIO_Pin|K6_GPIO_Pin|K7_GPIO_Pin|K9_GPIO_Pin|K10_GPIO_Pin);		
		GPIO_ResetBits(GPIOB,K5_GPIO_Pin);
		delay_ms(20);
		GPIO_SetBits(GPIOA,K1_GPIO_Pin);	
		PCM1_TRACE("<ok>\r\n");
	}
	else if(strcmp("pcm1 help",						 string)==0)	
	{
		printf_cmdList_PCM1();
	}	
	else result = REPLY_INVALID_CMD;
	return result;
}
/*------------------------------------------------------------------------------------*/







