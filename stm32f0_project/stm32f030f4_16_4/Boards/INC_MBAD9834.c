#include "inc_mbad9834.h"
#include "delay.h"
#include "mcp42010.h"

/*
D=10 	Vref=3.16V
D=100	Vref=2V

Vref+=3.3-((3.16-2)/(100-10))xD��3.3-Dnx0.012

K=Vo/Vi

Dn=256-310/K  MinK=1.21   Dn=200->K=

Vi=0.79	Vo=1.57	Dn=100 -->M=(1.57/0.79)*(256-100)��310

K=2,DN=101

SINE:Vpp
52=1V 168=2V	186=3V 	197=5V  

*/
uint8_t SetDnAmp(uint16_t Vo, uint8_t Vi,uint8_t Dn0)
{
	//k>1=1
	float kGain;
	
	uint16_t Dn;
	
	kGain=Vo/Vi;
	
	Dn=(256*(kGain)-256+Dn0)/kGain;
	
	return Dn;
}
//1.hw deng 2.project requare   3.cmd out
/*

�����źŷ����� �����ض���Ƶ�� �����Ƶ�ʿ����趨������,ִ����Ϻ󷵻���Ϣ

����Ƶ�ʲ�������λ�����ã��ײ����ã�1.�����鷳,�߼����ô��� 2.�޸Ĳ�����

set_sineSignal(amp,freq,times)

where the sine min is 0 V and sine max is 5 V

The waveform should aways start at 0 V, and the amplitude would increase 
so that the P2P signal from the signal generator is always positive from 0 V

�źŷ����� ������ ִ���� ��ѡ���ο�������
---���εķ������ú���ǹ̶��ĵ�ѹ������ʹ��5V
����0.01 Hz��5 kHz��Ƶ���в���0��10 V��������Ҳ�
---0.01 Hz to 5 kHz	0~10V
��������� 45 um ��ƫ����Ϊ2.5 V��P2P����Ϊ5 V������������СֵΪ0 V���������ֵΪ5V��
---���ֵΪ5V������ѹ������ѹ�Դ�Ϊ��������


���磬��һ��Ƶ��Ϊ1 Hz��Ȼ��ʹ��ÿ��Ƶ��֮��Ķ�����������ǽ�ÿʮ�����10����ɢƵ�ʡ�
�ڸ�ʾ���У�����ʮ���ʱ�䣬����ҽ�����31����Ƶ�ʣ�1-10��10-100��100-1000��1000����
��ÿ��Ƶ���£����ǽ�ִ���û����õ�������������ˣ����ڽϵ�Ƶ�ʵ�ѭ���ٶȽ����������ÿ��Ƶ���²��������ѵ�ʱ���������ͬ...
*/

//#define PLL_SOURCE_HSI
//PF0-LED							PF1-TAKE:RS485 DE
//PA0-RESET:AD9843 		PA1-PEAK_RST	PA2-VPEAK
//PA3-SCL							PA4-SDA	PA5-SCK	PA6-CS1
//PA7-MOSI						PB1-CS0
//PA9-TX							PA10-RX
//PA13-SWDIO					PA14-SWCLK
#define SPIx                             SPI1
#define SPIx_CLK                         RCC_APB2Periph_SPI1
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler

#define SPIx_SCK_PIN                     GPIO_Pin_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_GPIO_CLK                RCC_AHBPeriph_GPIOA
#define SPIx_SCK_SOURCE                  GPIO_PinSource5
#define SPIx_SCK_AF                      GPIO_AF_0

#define SPIx_MISO_PIN                    GPIO_Pin_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_GPIO_CLK               RCC_AHBPeriph_GPIOB
#define SPIx_MISO_SOURCE                 GPIO_PinSource4
#define SPIx_MISO_AF                     GPIO_AF_0

#define SPIx_MOSI_PIN                    GPIO_Pin_7
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SPIx_MOSI_SOURCE                 GPIO_PinSource7
#define SPIx_MOSI_AF                     GPIO_AF_0

SPI_InitTypeDef  SPI_InitStructure;

static void SPI_Config(void);

static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(SPIx_CLK, ENABLE);
  
  /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
  RCC_AHBPeriphClockCmd(SPIx_SCK_GPIO_CLK |  SPIx_MOSI_GPIO_CLK, ENABLE);
  
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);
//  GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
//  GPIO_InitStructure.GPIO_Pin = SPIx_MISO_PIN;
//  GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;//SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
   
  /* Configure the SPI interrupt priority */
//  NVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(SPIx, &SPI_InitStructure);
	
    SPI_Cmd(SPIx, ENABLE);		
}

void INC_MBAD9834_hw_init(void)
{
		//LED
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
		//														led_run			
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 ;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOF, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIOF, GPIO_Pin_0 );	

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);			
		//PA6 CS1 MCP42010
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_1;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
		GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_1);	
		//PB1 CS0 AD9834
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 ;
		GPIO_Init(GPIOB, &GPIO_InitStruct);	
		GPIO_SetBits(GPIOB,  GPIO_Pin_1);		
//	SPI_Config();
	  AD9834_Init_GPIO();
}

//SPI_SendData8(SPIx, CMD_ACK);


void INC_MBAD9834_execute(uint32_t startFreq,uint32_t finshFreq,uint16_t Decade, uint16_t Cycles)
{
	
	
	
}

uint8_t Command_INC_MBAD9834(char* string)
{
	uint8_t result = REPLY_OK;

	if( !strncmp(string,"sine ",5) ) 
	{
		char *p = NULL;
		char *s = &string[5];	
		uint32_t VotageVal=0;	
		uint32_t FreqVal=0;
		VotageVal=strtol(s, &p, 10);	
		
		if(*p==' ')	
		{
			s=NULL;
			FreqVal=strtol(p+1, &s, 10);	
			
			if(*(s)=='\0' )
			{
				AD9834_SetFreq(FreqVal);
				
//				MCP42010_Set256Taps(1,100);
//				MCP42010_Set256Taps(2,100);
				
				printf("<OK>\r\n");
				return REPLY_OK;
			}			
		}
	}
	
	if( !strncmp(string,"set plot",8) ) 
	{
		char *p = NULL;
		char *s = &string[8];	
		uint8_t plotNum=0;	
		uint8_t plotVal=0;
		plotNum=strtol(s, &p, 10);	
		
		if(*p==' ')	
		{
			s=NULL;
			plotVal=strtol(p+1, &s, 10);	
			
			if(*(s)=='\0' )
			{	
				MCP42010_Set256Taps(plotNum,plotVal);				
				printf("<OK>\r\n");
				return REPLY_OK;
			}			
		}
	}
	else result = REPLY_INVALID_CMD;
	return result;
}
