
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_TASK_TICK_H
#define __APP_TASK_TICK_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "bsp_defines.h"	

void KeyTask_every_ms(uint16_t nms);
void PressAlarmTask_every_ms(uint16_t nms);
	 
#ifdef __cplusplus
}
#endif
#endif /*__APP_SYS_CONTROL_H */


