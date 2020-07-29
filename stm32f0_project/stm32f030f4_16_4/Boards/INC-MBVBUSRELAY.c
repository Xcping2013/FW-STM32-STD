#include "INC-MBVBUSRELAY.h"
#include "delay.h"

//VBUS+_PIN	PA6
//VBUS-_PIN	PA5
//LED_PIN	PA4
void INC_MBVBUSRELAY_hw_init(void)
{

		GPIO_InitTypeDef GPIO_InitStruct;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);			
		//PA6 CS1 MCP42010
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
		GPIO_Init(GPIOA, &GPIO_InitStruct);	
		//GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4);	

		GPIO_ResetBits(GPIOA,  GPIO_Pin_6);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_5);		
		GPIO_ResetBits(GPIOA,  GPIO_Pin_4);		

}
uint8_t Command_INC_MBVBUSRELAY(char* string)
{
	uint8_t result = REPLY_OK;

	if( !strcmp(string,"VBUS+ ON")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_6);		
				
		printf("<VBUS+ ON OK>\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"VBUS+ OFF")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_6);		
				
		printf("<VBUS+ OFF OK>\r\n");
		return REPLY_OK;
	}	
	if( !strcmp(string,"VBUS- ON")) 
	{
		GPIO_ResetBits(GPIOA,  GPIO_Pin_5);		
				
		printf("<VBUS- ON OK>\r\n");
		return REPLY_OK;
	}			
	if( !strcmp(string,"VBUS- OFF")) 
	{
		GPIO_SetBits(GPIOA,  GPIO_Pin_5);		
				
		printf("<VBUS- OFF OK>\r\n");
		return REPLY_OK;
	}	
	else result = REPLY_INVALID_CMD;
	return result;
}
void INC_MBVBUSRELAY_CMDList(void)
{
	printf("VBUS+ ON\r\n");	
	printf("VBUS+ OFF\r\n");
	printf("VBUS- ON\r\n");	
	printf("VBUS- OFF\r\n");
	
}
void EXTI4_15_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
    /* Toggle LED1 */
		KeyS2=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
		//printf("sensor<%d>>>\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)? 0:1);
		if(KeyS1!=KeyS2)
		printf("sensor<%d>\r\n>>",KeyS2 ? 0:1);
    /* Clear the EXTI line 8 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line5);
  }  
}
//
