#include "okoa_led_test.h"
#include "app1_i2c_eeprom.h"
#include "app_tlc5973.h"
//EASV2010RWA0  RW_LED	Driver: TLC5973DR  one wire
/**************************************************************************************/
static void MBLEDRW_IO_Init(void);
	
void MBLEDRW_HW_Init(void)
{
	MBLEDRW_IO_Init();
	EEPROM_Init();
	bsp_tlc5973_init();
	writeLED(1,g_tParam.LED_RB,0,0);
	writeLED(1,0,g_tParam.LED_WB,0);
}

static void MBLEDRW_IO_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		//														led_run			
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIOA, GPIO_Pin_4 );	
		//mode_cs
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ADC_Config(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Configure ADC Channel11 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles);

  ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles);

	ADC_GetCalibrationFactor(ADC1);
  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
}
__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;

void getADCData(void)
{
	
	uint32_t v=0,mv=0;
  uint8_t text[50];

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	/* Get ADC1 converted data */
	ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
	
	/* Compute the voltage */
	ADC1ConvertedVoltage = (ADC1ConvertedValue *3300)/0xFFF;	


	v=(ADC1ConvertedVoltage)/1000;
	mv = (ADC1ConvertedVoltage%1000)/100;
	sprintf((char*)text,"   ADC = %d,%d V   ",v,mv);	
}



/**************************************************************************************/
//return 0---correct cmd
//return 1---error   cmd
uint8_t Command_analysis_LED_TEST(char* string)
{
	uint8_t result = REPLY_OK;

	if(!strcmp("led help", string))	
	{
		printf("\r\nled command list:\r\n");
				
		printf("led var1 var2\r\n");
		printf("    var1:0-red | 1-white\r\n");
		printf("    var2:0~100\r\n");
		
	}		
	if(!strcmp("led 0 brightness", string))	
	{
		//setLEDBrightness(whichled,NbitBrightness);
	}
	if(!strcmp("led 1 brightness", string))	
	{
		//setLEDBrightness(whichled,NbitBrightness);
	}
	else if( !strncmp(string,"led 0 ",6) ) 
	{
		char *p = NULL;
		char *s = &string[6];	
		uint16_t brightness=strtol(s, &p, 10);	
		if(*p=='\0')	
		{
			//setLEDBrightness(0,brightness);		
			writeLED(1,brightness,0,0);
		}
	}
	else if( !strncmp(string,"led 1 ",6) ) 
	{
		char *p = NULL;
		char *s = &string[6];	
		uint16_t brightness=strtol(s, &p, 10);	
		if(*p=='\0')	
		{
			//setLEDBrightness(1,brightness);		
			writeLED(1,0,brightness,0);
		}
	}
	else if( !strcmp(string,"led test") ) 
	{	
		tlc_5973_test();	
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
