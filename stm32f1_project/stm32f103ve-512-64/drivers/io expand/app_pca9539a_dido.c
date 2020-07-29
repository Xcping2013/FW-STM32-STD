
#include "app_pca9539a_dido.h"

#define INPUT_CNT		16
#define OUTPUT_CNT	16

pca9539a_t pca9539a_1;
pca9539a_t pca9539a_2;

void dido_pca9539a_init(void);
/******************--->	INPUT & OUTPUT	**************************
 */
pca9539a_t pca9539a_1=
{
	{PC_14,PC_15},
	0xE8,
	0,
	0,
};
pca9539a_t pca9539a_2=
{
	{PC_14,PC_15},
	0xEC,
	0,
	0,
};

void dido_pca9539a_init(void)
{
	PCA9539A_Init(pca9539a_1);
	PCA9539A_Init(pca9539a_2);
}
//
uint8_t getChInput(uint8_t channel)
{
	SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
	SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
	return (channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0));	
}
void setChOutput(uint8_t channel, uint8_t setval)
{
	if(channel<9)	
	{
		pca9539a_1.out_data= setval? (pca9539a_1.out_data | (1 << (channel-1))) :(pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1))));
		SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
	}
	else
	{
		pca9539a_2.out_data= setval? (pca9539a_2.out_data | (1 << (16-channel))) :(pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel))));
		SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
	}	
}
//
int readinput(int argc, char **argv)
{
    if (argc == 1)
    {
        rt_kprintf("Please input: readinput <ch1> <ch2> ......\n");
        return 1;
    }
    else
    {
			uint8_t channel,i;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 17)
				{
					SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
					SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	

					if((argc-i)>1) rt_kprintf("in[%d]=%d,",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
					else           rt_kprintf("in[%d]=%d", channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
				}
				else 
				{		
					rt_kprintf("input channel must be 1~16\n");
					return 1;
				}
			}
			rt_kprintf("\n");
    }
    return 0;
}
//
int output(int argc, char **argv)
{
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 17)
				{
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data | (1 << (channel-1)) ;
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data | (1 << (16-channel));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					if((argc-i)>1)  rt_kprintf("out[%d]=1,",channel); 
					else           	rt_kprintf("out[%d]=1",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~16\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
		else if (!strcmp(argv[1], "off")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 17)
				{
					if(channel<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1)));
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel)));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					if((argc-i)>1)  rt_kprintf("out[%d]=0,",channel); 
					else           	rt_kprintf("out[%d]=0",channel);	
				}
				else 
				{		
					rt_kprintf("output channel must be 1~16\n");
					return RT_ERROR;
				}
			}
			rt_kprintf("\n");
			return RT_EOK;
		}
	}
	if (argc == 4)
	{
		if (!strcmp(argv[1], "toggle"))
		{
			rt_uint8_t flash_ch=atoi(argv[2]);
			rt_uint8_t Delaycnt=atoi(argv[3]);
			if(0<flash_ch && flash_ch< 17)
			{
				if(Delaycnt)
				{
					out_flash_on[flash_ch-1]=1;
					out_flash_delay[flash_ch-1]=Delaycnt;							 
					rt_kprintf("out[%d] toggle every %dx100ms\n",flash_ch,Delaycnt);
				}
				else
				{
					out_flash_on[flash_ch-1]=0;
					if(flash_ch<9)	
					{
						pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (flash_ch-1)));
						SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
					}
					else
					{
						pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-flash_ch)));
						SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
					}	
					rt_kprintf("out[%d] stop toggle\n",flash_ch);		
				}
				return RT_EOK;
			}
			else 
			{		
				rt_kprintf("output channel must be 1~16\n");
				return RT_ERROR;
			}
		}
	}
	rt_kprintf("Usage: \n");
	rt_kprintf("output on <ch>  - set the output channel on\n");
	rt_kprintf("output off <ch>     - set the output channel off\n");
	//rt_kprintf("output toggle <delay_time>  - start or stop output toggle per time x100ms\n");
	return -RT_ERROR;
}

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
		
		if( (*p=='\0')	&& (0<channel && channel< (INPUT_CNT+1)))
		{
			SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
			SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	

			rt_kprintf("in[%d]=%d\r\n",channel,channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0) );
		}
		else 
		{
			printf("input channel must be 1~%d\r\n",INPUT_CNT);
		}
	}
	else if( !strncmp(string,"output on ",10) ) 
	{
		char *p = NULL;
		char *s = &string[10];	
		uint32_t channel=0;	
		channel=strtol(s, &p, 10);
		
		if( (*p=='\0')	&& (0<channel && channel< (OUTPUT_CNT+1)))
		{
			if(channel<9)	
			{
				pca9539a_1.out_data= pca9539a_1.out_data | (1 << (channel-1)) ;
				SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
			}
			else
			{
				pca9539a_2.out_data= pca9539a_2.out_data | (1 << (16-channel));
				SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
			}	
			printf("out[%d]=1\r\n",channel); 
		}
		else 
		{
			//result = REPLY_INVALID_VALUE;
			printf("output channel must be 1~%d\r\n",OUTPUT_CNT);
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
			if(channel<9)	
			{
				pca9539a_1.out_data= pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1)));
				SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
			}
			else
			{
				pca9539a_2.out_data= pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel)));
				SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
			}	
			printf("out[%d]=0\r\n",channel); 
		}
		else 
		{
			printf("output channel must be 1~%d\r\n",OUTPUT_COUNT);
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



