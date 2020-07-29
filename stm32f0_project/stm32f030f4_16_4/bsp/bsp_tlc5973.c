

#include "bsp.h"

#define GPIO_PORT_SDI	GPIOA									/* GPIO端口 */
#define RCC_SDI_PORT 	RCC_AHBPeriph_GPIOA		/* GPIO端口时钟 */
#define SDI_PIN				GPIO_Pin_7						/* 连接到SDI数据线的GPIO */

#define pTime 1					//in useconds
int tCycle = pTime * 6; // time for one bit. Per datasheet tCycle is between 0.33 useconds and 10 useconds
int numLeds = 1; 				//number of LEDs

static void writeCommTimer(void) ;
static void writeCommand(void);
static void waitEOS(void);
static void waitGSLAT(void);
static void writeData(uint8_t data);

void bsp_tlc5973_init() 
{
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_AHBPeriphClockCmd(RCC_SDI_PORT, ENABLE);
													
		GPIO_InitStruct.GPIO_Pin = SDI_PIN ;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIO_PORT_SDI, &GPIO_InitStruct);	
		GPIO_ResetBits(GPIO_PORT_SDI,  SDI_PIN);	
}



void writeLED(uint8_t ledNum, uint8_t redValue, uint8_t greenValue, uint8_t blueValue) 
{
  for (int i = 0; i< numLeds; i++) 
	{
    writeCommTimer();
    
    //if (i = ledNum)
		{
      writeCommand();
      writeData(redValue);
      writeData(greenValue);
      writeData(blueValue);
    }
    waitEOS();
  }
  waitGSLAT();   
}

void writeZero() 
{
  GPIO_SetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(pTime);
  GPIO_ResetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(pTime);
  delay_us(pTime);
  delay_us(pTime);
  delay_us(pTime);
  delay_us(pTime);
  
}

void writeOne() 
{
  GPIO_SetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(pTime);
  GPIO_ResetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(pTime); 
  GPIO_SetBits(GPIO_PORT_SDI,  SDI_PIN);
  delay_us(pTime);
  GPIO_ResetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(pTime);
  delay_us(pTime);
  delay_us(pTime);
}

static void writeCommTimer(void) 
{
  //first two zeroes determine the timing (tcycle) after device is powered up or after a GSLAT
  writeZero();
  writeZero();
}

static void writeCommand(void) 
{
  //write command in hex is 3AA, binary is 0011 1010 1010
	writeZero();
	writeZero();
	
  writeOne();
  writeOne();
  
  writeOne();
  writeZero();
  writeOne();
  writeZero();

  writeOne();
  writeZero();
  writeOne();
  writeZero();  
}

//end of sequence (for a single driver chip)
static void waitEOS(void) 
{
  GPIO_ResetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(tCycle*4); // min 3.5 to max 5.5 times tCycle
}

//grayscale latch (for the end of a chain of driver chips)
static void waitGSLAT(void) 
{
  GPIO_SetBits(GPIO_PORT_SDI,  SDI_PIN);	
  delay_us(tCycle*10); // minimum 8 time tCycle
}
  
static void writeData(uint8_t data) 
{
  for (uint8_t i = 0; i<8; i++)
	{
    if(data & 0x80) 
		{
			writeOne();
		}
    else
		{
			writeZero();
		}
    data <<= 1;
  }
  //pad the end of each 8 bit number with four more 
  //bits (use 1010 because that's the end of the
  //write command so it doesn't have to have an entirely
  //separate function) to make a 12 bit number
  writeZero();
  writeZero();
  writeZero();
  writeZero();
}









