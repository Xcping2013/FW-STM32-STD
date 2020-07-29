/**********************************************************************************************************************/
#include "app_gpio_dido.h"	
/**********************************************************************************************************************/
uint8_t rgb_pin_num[3]={PD_6,PD_5,PD_4};

uint8_t	inputs_pin_num[INPUT_COUNT]=
{
	PB_5,	//IN1
	PB_6, //IN2
	PB_7, //IN3
	PB_8,	//IN4
	PB_9,	//IN5	---HOME0
	PE_0,	//IN6	---HOME1
	PE_1,	//IN7	---HOME2
	PE_2,	//IN8
	PE_3,	//IN9
	PE_4,	//IN10
	PE_5,	//IN11
	PE_6,	//IN12
	
//PB_1,
//PD_8,
//PD_12,
//PD_9,
//PB_15,
//PB_13,
};  
uint8_t	outputs_pin_num[OUTPUT_COUNT]=
{
	PA_15	 ,
	PC_10  ,
	PC_11	 ,
	PC_12	 ,
	PD_0	 ,
	PD_1	 ,
	PD_2   ,
	PD_3   ,
//	PB_14  ,
//	PD_10  ,
//	PD_13  ,
//	PD_11  ,
//	PE_14  ,
//	PE_15	 ,
};
/*********************************************************************************************************/
static void printf_cmdList_gpio(void);
static int readinput(int argc, char **argv);
static int output(int argc, char **argv);
static int rgb(int argc, char **argv);
/*********************************************************************************************************/
uint8_t getChInput(uint8_t channel)
{
	return pinRead(inputs_pin_num[channel-1]) ? 1:0 ;
}
void setChOutput(uint8_t channel, uint8_t setval)
{
	pinWrite(outputs_pin_num[channel-1],setval);
}

void dido_hw_init(void)
{    
	uint8_t i;
	pinSWD_Init();
	pinMode(PD_7, GPIO_Mode_Out_PP); 
	for(i=0;i<12;i++)
	{
    pinMode(inputs_pin_num[i], GPIO_Mode_IPU); 
	}
	for(i=0;i<8;i++)
	{
    pinMode(outputs_pin_num[i], GPIO_Mode_Out_PP); 
		pinWrite(outputs_pin_num[i],LOW);
	}   
	for(i=0;i<3;i++)
	{
    pinMode(rgb_pin_num[i], GPIO_Mode_Out_PP); 
		pinWrite(rgb_pin_num[i],LOW);
	} 
	printf("dido_hw_init()...ok\r\n");
}

static int readinput(int argc, char **argv)
{	
    if (argc == 1)
    {
        printf("Please input: readinput <ch1> <ch2> ......\r\n");
        return REPLY_INVALID_CMD;
    }
    else
    {
			uint8_t channel,i;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< (INPUT_COUNT+1))
				{
					if((argc-i)>1) printf("in[%d]=%d,",channel,pinRead(inputs_pin_num[channel-1]) ? 0:1 );
					else           printf("in[%d]=%d",channel,pinRead(inputs_pin_num[channel-1]) ? 0:1 );
				}
				else 
				{		
					printf("input channel must be 1~12\r\n");
					return REPLY_INVALID_VALUE;
				}
			}
			printf("\r\n");
    }
    return REPLY_OK;
}
static int output(int argc, char **argv)
{
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					pinWrite(outputs_pin_num[channel-1],HIGH);
					if((argc-i)>1)  printf("out[%d]=1,",channel); 
					else           	printf("out[%d]=1",channel);
					
				}
				else 
				{		
					printf("output channel must be 1~8\r\n");
					return REPLY_INVALID_VALUE;
				}
			}
			printf("\r\n");
			return REPLY_OK;
		}
		else if (!strcmp(argv[1], "off")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					pinWrite(outputs_pin_num[channel-1],LOW);
					if((argc-i)>1)  printf("out[%d]=0,",channel); 
					else           	printf("out[%d]=0",channel);
				}
				else 
				{		
					printf("output channel must be 1~8\r\n");
					return REPLY_INVALID_VALUE;
				}
			}
			printf("\r\n");
			return REPLY_OK;
		}
	}
	printf("Usage: \r\n");
	printf("output on <ch>  - set the output channel on\r\n");
	printf("output off <ch>     - set the output channel off\r\n");
	
	return REPLY_INVALID_CMD;
}

static int rgb(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Please input: rgb <r|g|b|0>......\r\n");
		return -1;
	}
  else
  {
		switch(argv[1][0])
		{
			case 'r':	pinWrite(rgb_pin_num[0],1);pinWrite(rgb_pin_num[1],0);pinWrite(rgb_pin_num[2],0);
				break;
			case 'g':	pinWrite(rgb_pin_num[0],0);pinWrite(rgb_pin_num[1],1);pinWrite(rgb_pin_num[2],0);
				break;
			case 'b':	pinWrite(rgb_pin_num[0],0);pinWrite(rgb_pin_num[1],0);pinWrite(rgb_pin_num[2],1);
				break;
			default:	pinWrite(rgb_pin_num[0],0);pinWrite(rgb_pin_num[1],0);pinWrite(rgb_pin_num[2],0);
				break;		
		}
		printf("rgb=%c\r\n",argv[1][0]);
	}
	return 0;
}
//
uint8_t Command_analysis_dido(char *string)
{
	uint8_t result = REPLY_OK;
	if( !strcmp(string,"gpio help") )	{printf_cmdList_gpio();}
	else if( !strncmp(string,"readinput ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (INPUT_COUNT+1)))
		{
			printf("in[%d]=%d\r\n",channel,pinRead(inputs_pin_num[channel-1]) ? 0:1 );
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("input channel must be 1~12\r\n");
		}
	}
	else if( !strncmp(string,"output on ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (OUTPUT_COUNT+1)))
		{
			pinWrite(outputs_pin_num[channel-1],HIGH);
			printf("out[%d]=1\r\n",channel); 
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("output channel must be 1~8\r\n");
		}
	}
	else if( !strncmp(string,"output off ",11) ) 
	{
		char *p = NULL;
		char *s = &string[11];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (OUTPUT_COUNT+1)))
		{
			pinWrite(outputs_pin_num[channel-1],LOW);
			printf("out[%d]=0\r\n",channel); 
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("output channel must be 1~8\r\n");
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
static void printf_cmdList_gpio(void)
{
		printf("GPIO Usage: \r\n");
		printf("readinput: read the state of the input channel\r\n");
		printf("output on: set the output on\r\n");
  	printf("output off: set the output off\r\n");	
}
//




//



