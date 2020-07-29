
#ifndef __BSP_MCU_SPI_H
#define __BSP_MCU_SPI_H

#include "bsp_mcu_gpio.h"

#define	SPI_DEV_TMC429	1

#define	USING_INC_MBTMC429
	 
#if defined(USING_INC_MBTMC429) 

	#define CS_PIN_TMC429 		 		 PC_4
	#define SELECT_TMC429()        GPIOC->BRR=BIT4
	#define DESELECT_TMC429()      GPIOC->BSRR=BIT4
												
	#define POSCMP1_PIN						 PE_12
	#define	INTOUT1_PIN						 PE_13	

	#define	MX_SPI_Init			 			 MX_SPI1_Init					
	#define	ReadWriteSPI					 ReadWriteSPI1	

#elif defined(USING_INC_MB1616DEV6)

	#define CS_PIN_TMC429 		 		 PB_12
	#define SELECT_TMC429()        GPIOB->BRR=BIT12
	#define DESELECT_TMC429()      GPIOB->BSRR=BIT12
							
	#define POSCMP1_PIN						 PD_13
	#define	INTOUT1_PIN						 PD_14

	#define	MX_SPI_Init			  		 MX_SPI2_Init					
	#define	ReadWriteSPI					 ReadWriteSPI2		
#endif

void SPI1_Init(void);

UCHAR ReadWriteSPI1(UCHAR DeviceNumber, UCHAR Data, UCHAR LastTransfer);

#endif

