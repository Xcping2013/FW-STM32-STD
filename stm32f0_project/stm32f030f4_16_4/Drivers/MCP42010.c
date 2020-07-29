
#include "bsp.h"
#include "mcp42010.h"
#include "inc_mbad9834.h"

#if 0
class MCP42010 {
  public:
	// These are the pins used to connect to the chip
    MCP42010(int CS, int CLK, int MOSI);
	// Sets the given pot (1 or 2) to a given value (0-255)
    void setPot(byte pot, byte value);
  private:
    void transferSPI(byte data);
    int _CS, _CLK, _MOSI;
};

MCP42010::MCP42010(int CS, int CLK, int MOSI) {
  _CS = CS;
  _CLK = CLK;
  _MOSI = MOSI;
  
  pinMode(_CS, OUTPUT);
  pinMode(_CLK, OUTPUT);
  pinMode(_MOSI, OUTPUT);
  digitalWrite(_CLK, LOW);
}

void MCP42010::setPot(byte pot, byte value) {
  digitalWrite(_CS, LOW);
  transferSPI((pot & B11) | B00010000);
  transferSPI(value);
  digitalWrite(_CS, HIGH);
}

void MCP42010::transferSPI(byte data) {
  for (byte i=1; i<=8; i++) {
    digitalWrite(_MOSI, (data >> (8-i)) & 1 ? HIGH : LOW);
    digitalWrite(_CLK, HIGH);
    digitalWrite(_CLK, LOW);
  }
}
#endif

void mcp42010_Init(void)
{
	
	
	
}

void MCP42010_writeDate(uint8_t data)
{
	uint8_t i;
	
  for ( i=1; i<=8; i++) 
	{
		if((data >> (8-i)) & 1)  SDATA_1();	
		else SDATA_0();
		
		SCLK_1();
		SCLK_0();
  }	
}


int MCP42010_Set256Taps(uint8_t pot,uint8_t taps)
{
//	GPIO_ResetBits(GPIOA,  GPIO_Pin_6);	

//	SPI_SendData8(SPI1, (pot & 0x03) | 0x10);
//	
//	SPI_SendData8(SPI1, taps);
//	
//	GPIO_SetBits(GPIOA,  GPIO_Pin_6);	
	
	
	CS0_0();
	MCP42010_writeDate((pot & 0x03) | 0x10);
	MCP42010_writeDate(taps);
	CS0_1();
	
}
