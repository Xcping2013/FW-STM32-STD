#ifndef __APP_GPIO_DIDO_H
#define __APP_GPIO_DIDO_H

#include "bsp_mcu_gpio.h"

//OSE 8MHZ  USE UART2	SPI1
//#define LED_SYS_PIN			PD_7
//#define	MCU_TX					PA_9
//#define	MCU_RX					PA_10
//#define	SWDIO						PA_13
//#define	SWCLK						PA_14
//#define	IIC_SDA					PB_3
//#define	IIC_SCL					PB_4
//#define	SPI1_CS					PB_4
//#define	SPI1_SCK				PA_5
//#define	SPI1_MISO				PA_6
//#define	SPI1_MOSI				PA_7
//#define	TMC429_CLK1			PB_0
//#define	TMC429_CLK2			PB_1		//IN13
//#define	POSCMP1					PE_12
//#define	INTOUT1					PE_13
//#define	INTOUT2					PE_14		//OUT13
//#define	POSCMP2					PE_15		//OUT14
//#define	SPI2_CS					PB_12		//LED2	H-ON
//#define	SPI2_SCK				PB_13		//IN18
//#define	SPI2_MISO				PB_14		//OUT9
//#define	SPI2_MOSI				PB_15		//IN17
//#define	TMC262_CS1			PD_8		//IN14
//#define	TMC262_CS2			PD_9		//IN16
//#define	TMC262_CS3			PD_10		//OUT10
//#define	TMC262_EN1			PD_11		//OUT12
//#define	TMC262_EN2			PD_12		//IN15
//#define	TMC262_EN3			PD_13		//OUT11
#define INPUT_COUNT    	12	
#define OUTPUT_COUNT    8	

#define INPUT1	PBin(5)
#define INPUT2	PBin(6)
#define INPUT3	PBin(7)
#define INPUT4	PBin(8)
#define INPUT5	PBin(9)

#define INPUT6	PEin(0)
#define INPUT7	PEin(1)
#define INPUT8	PEin(2)
#define INPUT9	PEin(3)
#define INPUT10	PEin(4)
#define INPUT11	PEin(5)
#define INPUT12	PEin(6)

#define OUTPUT1		PAout(15)
#define OUTPUT2		PCout(10)
#define OUTPUT3		PCout(11)
#define OUTPUT4		PCout(12)
#define OUTPUT5		PDout(0)
#define OUTPUT6		PDout(1)
#define OUTPUT7		PDout(2)
#define OUTPUT8		PDout(3)

#define RGB_R 	PDout(6)
#define RGB_G 	PDout(5)
#define RGB_B 	PDout(4)						


extern uint8_t inputs_pin_num[INPUT_COUNT];
extern uint8_t outputs_pin_num[OUTPUT_COUNT];
extern uint8_t rgb_pin_num[3];
extern uint8_t homeSensorPin[3];

uint8_t getChInput(uint8_t channel);
void setChOutput(uint8_t channel, uint8_t setval);

void dido_hw_init(void);
uint8_t Command_analysis_dido(char *string);

#endif
