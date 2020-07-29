
#include "bsp.h"
#include "ad9834.h"
#include "inc_mbad9834.h"

void AD9834_Init_GPIO(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
													
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7 ;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIOA,  GPIO_Pin_5|GPIO_Pin_7);	
	
//		GPIO_SetBits(GPIOA,  GPIO_Pin_6);	

//		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 ;
//		GPIO_Init(GPIOB, &GPIO_InitStruct);	
//		GPIO_SetBits(GPIOB,  GPIO_Pin_1);	
}

void serial_out(uint16_t data) 
{
    FSYNC_0();
    data & 0x8000? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x4000? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x2000? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x1000? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0800? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0400? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0200? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0100? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0080? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0040? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0020? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0010? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0008? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0004? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0002? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    data & 0x0001? SDATA_1() : SDATA_0(); SCLK_0(); SCLK_1();
    FSYNC_1();
}
int AD9834_SetFreq(uint32_t freqdata) 
{
    float SetFreq;
    uint32_t temp;
    uint16_t Uptemp, Lowtemp;
    int begin, end;
 
    SCLK_1();
    SDATA_0();
    FSYNC_1();
		//2^28/48MHZ=	5.592405-> 1HZ��Ӧ��������
		SetFreq = 5.592405 * (uint32_t)freqdata;
		temp = (uint32_t)SetFreq;
		Lowtemp = (uint16_t)(temp & 0x3FFF);
		Uptemp = (uint16_t)((temp/16384) & 0x3FFF);
		serial_out(0x2028);
		serial_out(Lowtemp + 0x4000);
		serial_out(Uptemp + 0x4000);
}
/*
���Ҳ����/���ǲ����
����ʽSPI�ӿ�	40MHZ
���Ƶ�ʸߴ�37.5 MHz=75MHZ/2	(75 MHz-0.28 Hz�ķֱ��� ʱ������Ϊ1 MHzʱ0.004 Hz�ķֱ���)
1.LOAD REG 2.Bit FSEL Bit PSE					LFSELECT/PSELECT

AD9834����һ��16λ���ƼĴ���

DB15 DB14 			DB13 . . . DB0
0 		0 						����λ

fMCLK/228 �� FREQREG

*/


#if 0

void WriteToAD9834Word(unsigned short Buf)
{

	unsigned	short	ValueToWrite = 0;
    unsigned	char	i = 0;
	unsigned	char	j = 0;
	
	SET_SCL();
	CLR_FSYNC();			//FSYNC become low, indicate the beginning of the write
	
	ValueToWrite =	Buf;

	for (j=0; j<16; j++)
	{
		SET_SCL();
		if(0x8000 == (ValueToWrite & 0x8000))
		{
			SET_SDO();	    //Wirte 1
		}
		else
		{
			CLR_SDO();	    //Write 0
		}
		CLR_SCL();								 
		
		ValueToWrite <<= 1;	//Rotate data
	}

	SET_FSYNC();		  	//FSYNC become high, indicate the end of the write

}
int main (void)
{ 

    ADuC7026_Initiate();

	SET_SCL();
	SET_FSYNC();

//Write Sequence refer to Figure31,32 and Table 11 in the AD9834's datasheet
//RESET: RESET=1,PIN/SW=0	
	WriteToAD9834Word(0X0100);
//Write a full 28-Bit Word to Frequency Register: B28=1, RESET=1
	WriteToAD9834Word(0X2100);
//Frequency0
	WriteToAD9834Word(0X599A);		//1MHz Output, 10MHz MCLK
	WriteToAD9834Word(0X4666);					
//Frequency1	
	WriteToAD9834Word(0XB5C3);		//100kHz Output, 10MHz MCLK
	WriteToAD9834Word(0X80A3);
//Phase0 and Phase1
	WriteToAD9834Word(0XC000);
	WriteToAD9834Word(0XE000);	
//Select the Frequency Source and CLEAR RESET: RESET BIT=0, FSEL=0,PSEL=0
	//WriteToAD9834Word(0X0000);
//Select the Frequency Source and CLEAR RESET: RESET BIT=0, FSEL=1,PSEL=1	
	WriteToAD9834Word(0X0C00);

	while(1);	
}
	#define FSYNC_PIN	PA_3
	#define SCK_PIN		PA_5
	#define DAT_PIN		PA_7
	#define CS_PIN		PA_4

//****************************************************************

	#define FSYNC_0()		pinReset(FSYNC_PIN)
	#define FSYNC_1()		pinSet(FSYNC_PIN)

	#define SCK_0()			pinReset(SCK_PIN)
	#define SCK_1()			pinSet(SCK_PIN)
	
	#define DAT_0()			pinReset(DAT_PIN)
	#define DAT_1()			pinSet(DAT_PIN)

	#define CS_0()			pinReset(CS_PIN)
	#define CS_1()			pinSet(CS_PIN)

//��ʼ��AD9834 GPIO

void AD9834_Init_GPIO()
{
	pinMode(FSYNC_PIN,PIN_MODE_OUTPUT);
	pinMode(SCK_PIN,PIN_MODE_OUTPUT);
	pinMode(DAT_PIN,PIN_MODE_OUTPUT);
	pinMode(CS_PIN,PIN_MODE_OUTPUT);
}



/*
*********************************************************************************************************
*	�� �� ��: AD9834_Delay
*	����˵��: ʱ����ʱ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AD9834_Delay(void)
{
	uint16_t i;
	for (i = 0; i < 10; i++);
}



/*
*********************************************************************************************************
*	�� �� ��: AD9834_Write
*	����˵��: ��SPI���߷���16��bit����
*	��    ��: TxData : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD9834_Write(unsigned int TxData)
{
	unsigned char i;

	SCK_1();
	AD9834_Delay();
	FSYNC_1();
	AD9834_Delay();
	FSYNC_0();
	AD9834_Delay();
	for(i = 0; i < 16; i++)
	{
		if (TxData & 0x8000)
			DAT_1();
		else
			DAT_0();
		
		AD9834_Delay();
		SCK_0();
		AD9834_Delay();		
		SCK_1();
		
		TxData <<= 1;
	}
	FSYNC_1();
	
} 

/*
*********************************************************************************************************
*	�� �� ��: AD9834_AmpSet
*	����˵��: �ı�����źŷ���ֵ
*	��    ��: 1.amp ������ֵ  0- 255
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 


void AD9834_AmpSet(unsigned char amp)
{
	unsigned char i;
	unsigned int temp;
   	
	CS_0();
	temp =0x1100|amp;
	for(i=0;i<16;i++)
	{
	    SCK_0();	
	   if(temp&0x8000)
	   	DAT_1();
	   else
		DAT_0();
		temp<<=1;
		  AD9834_Delay();
	    SCK_1();
	    AD9834_Delay();
	}
	
   	CS_1();
}


/*
*********************************************************************************************************
*	�� �� ��: AD9834_WaveSeting
*	����˵��: ��SPI���߷���16��bit����
*	��    ��: 1.Freq: Ƶ��ֵ, 0.1 hz - 12Mhz
			  2.Freq_SFR: 0 �� 1
			  3.WaveMode: TRI_WAVE(���ǲ�),SIN_WAVE(���Ҳ�),SQU_WAVE(����)
			  4.Phase : ���εĳ���λ
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void AD9834_WaveSeting(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase )
{

		int frequence_LSB,frequence_MSB,Phs_data;
		double   frequence_mid,frequence_DATA;
		long int frequence_hex;

		/*********************************����Ƶ�ʵ�16����ֵ***********************************/
		frequence_mid=268435456/25;//�ʺ�25M����
		//���ʱ��Ƶ�ʲ�Ϊ25MHZ���޸ĸô���Ƶ��ֵ����λMHz ��AD9834���֧��25MHz
		frequence_DATA=Freq;
		frequence_DATA=frequence_DATA/1000000;
		frequence_DATA=frequence_DATA*frequence_mid;
		frequence_hex=frequence_DATA;  //���frequence_hex��ֵ��32λ��һ���ܴ�����֣���Ҫ��ֳ�����14λ���д���
		frequence_LSB=frequence_hex; //frequence_hex��16λ�͸�frequence_LSB
		frequence_LSB=frequence_LSB&0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ
		frequence_MSB=frequence_hex>>14; //frequence_hex��16λ�͸�frequence_HSB
		frequence_MSB=frequence_MSB&0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ

		Phs_data=Phase|0xC000;	//��λֵ
		AD9834_Write(0x0100); //��λAD9834,��RESETλΪ1
		AD9834_Write(0x2100); //ѡ������һ��д�룬B28λ��RESETλΪ1

		if(Freq_SFR==0)				  //���������õ�����Ƶ�ʼĴ���0
		{
		 	frequence_LSB=frequence_LSB|0x4000;
		 	frequence_MSB=frequence_MSB|0x4000;
			 //ʹ��Ƶ�ʼĴ���0�������
			AD9834_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���0�ĵ�14λ��������
			AD9834_Write(frequence_MSB); //H14 Ƶ�ʼĴ����ĸ�14λ��������
			AD9834_Write(Phs_data);	//������λ
			//AD9834_Write(0x2000); /**����FSELECTλΪ0��оƬ���빤��״̬,Ƶ�ʼĴ���0�������**/
	    }
		if(Freq_SFR==1)				//���������õ�����Ƶ�ʼĴ���1
		{
			 frequence_LSB=frequence_LSB|0x8000;
			 frequence_MSB=frequence_MSB|0x8000;
			//ʹ��Ƶ�ʼĴ���1�������
			AD9834_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���1�ĵ�14λ����
			AD9834_Write(frequence_MSB); //H14 Ƶ�ʼĴ���1Ϊ
			AD9834_Write(Phs_data);	//������λ
			//AD9834_Write(0x2800); /**����FSELECTλΪ0������FSELECTλΪ1����ʹ��Ƶ�ʼĴ���1��ֵ��оƬ���빤��״̬,Ƶ�ʼĴ���1�������**/
		}

		if(WaveMode==TRI_WAVE) //������ǲ�����
		 	AD9834_Write(0x2002); 
		if(WaveMode==SQU_WAVE)	//�����������
			AD9834_Write(0x2028); 
		if(WaveMode==SIN_WAVE)	//������Ҳ���
			AD9834_Write(0x2000); 

}
int sineWave(int argc, char **argv)
//void sine(double Freq, uint8_t am)
{
	if (argc == 2)
	{
		AD9834_WaveSeting(atoi(argv[1]),0,SIN_WAVE,0);
		return RT_EOK;
	}
	if (argc == 3)
	{
		AD9834_WaveSeting(atoi(argv[1]),0,SIN_WAVE,0);
		AD9834_AmpSet(atoi(argv[2]));
		return RT_EOK;	
	}
	return RT_ERROR;
}


MSH_CMD_EXPORT_ALIAS(AD9834_WaveSeting, wave,...);

#endif
