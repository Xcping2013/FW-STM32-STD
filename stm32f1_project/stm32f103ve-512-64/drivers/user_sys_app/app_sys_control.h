
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SYS_CONTROL_H
#define __APP_SYS_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "bsp_defines.h"	

	 
extern volatile uint8_t u8PressAlarm;
	
void 			ResetCPU(UCHAR ResetPeripherals); 
void		  EnableInterrupts(void);
void		  DisableInterrupts(void);
	 
void 			xstrcat(char *str1,char *str2);
uint16_t  Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void 			MakeLowercase( char *str);

void 			PressAlarm_Handler(void);
void 			Task1_every_ms(uint16_t nms);
void 			Task2_every_ms(uint16_t nms);
	 
#ifdef __cplusplus
}
#endif
#endif /*__APP_SYS_CONTROL_H */


