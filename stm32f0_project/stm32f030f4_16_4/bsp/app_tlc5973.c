
#include "app_tlc5973.h"

void tlc_5973_test(void) 
{
  for(uint8_t i=0; i<255; i++) 
	{
    writeLED(1,i,0,0);
    delay_ms(1000);
  }
  for(uint8_t i=0; i<255; i++) {
    writeLED(1,0,i,0);
    delay_ms(1000);
  }
}

