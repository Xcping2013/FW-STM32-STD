/**
  \file 		UART.c
  \author 	Xcping2016
  \version 	1.00

  \brief 		UART functions

  This file provides all functions needed for using
  the UART interface.
*/
#include "bsp_mcu_uart.h"

u8 	USART1_RX_BUF[USART1_REC_LEN];  
u16 USART1_RX_STA=0;    

u8 	USART3_RX_BUF[USART3_REC_LEN];  
u16 USART3_RX_STA=0;  

#define UART_1 1
#define UART_2 2
#define UART_3 3

#define UART_INTERFACE_1	UART_1
#define UART_INTERFACE_2	UART_3

#if defined(UART_INTERFACE_1)

#define UART_INTR_PRI        4

#if UART_INTERFACE_1==UART_1
  #define USARTx USART1
#elif UART_INTERFACE_1==UART_2
  #define USARTx USART2
#elif UART_INTERFACE_1==UART_3
  #define USARTx USART3
#else
  #error "UART device not supported"
#endif


	#ifdef __GNUC__
	/* With GCC, small printf (option LD Linker->Libraries->Small printf
		 set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
	#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
	#endif /* __GNUC__ */
	
	PUTCHAR_PROTOTYPE
	{
		/* Place your implementation of fputc here */
		/* e.g. write a character to the USART1 and Loop until the end of transmission */
		while((USART1->SR&0X40)==0){;} //循环发送,直到发送完毕   
			USART1->DR = (u8) ch;    
		return ch;
	}
/*******************************************************************
   Funktion: InitUART()
   Parameter: Baudrate: Baudrate der Schnittstelle (z.B. 9600)

   Zweck: Intialisierung der UART
********************************************************************/
void InitUART_1(int BaudRate)
{
  USART_InitTypeDef  UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  #if UART_INTERFACE_1 == UART_2
  //UART2 aktivieren
  USART_DeInit(USART2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  //GPIOA aktivieren (UART2-Pins)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);

  //UART2-Pins zuweisen (PA2 und PA3)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_1 == UART_1
  //UART1 aktivieren
  USART_DeInit(USART1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_1 == UART_3
  //UART3 aktivieren
  USART_DeInit(USART3);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);


  //UART3-Pins zuweisen (PB10 und PB11)
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, DISABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  #else
  #error "UART pin mapping not defined"
  #endif

  //UART konfigurieren
  USART_StructInit(&UART_InitStructure);
  UART_InitStructure.USART_BaudRate=BaudRate;

  USART_Init(USARTx,&UART_InitStructure);

  //Interrupt f? UART aktivieren
  #if UART_INTERFACE_1 == UART_2
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  #elif UART_INTERFACE_1 == UART_1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  #elif UART_INTERFACE_1 == UART_3
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  #else
  #error "Device not supported!"
  #endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_INTR_PRI;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  USART_ClearFlag(USARTx, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USARTx,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USARTx,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USARTx,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_ERR ,DISABLE);

  USART_Cmd(USARTx, ENABLE);
}
//串口发送一个字节数据
void UART_SendChar(u8 ch)
{  
	if(ch=='\n')
	{
		USART_SendData(USARTx, '\r');
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		USART_SendData(USARTx, ch);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
	}
	else 
	{
		USART_SendData(USARTx, ch);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
//串口发送字符串
void UART_SendStr(char *ptr)
{
  	while(*ptr!='\0') UART_SendChar(*ptr++);
}
//串口格式打印输出
void UART_Printf(const char *fmt,...)
{   
	va_list ap;	   						//This is support the format output 
	char string[200];
	va_start(ap,fmt);
	vsprintf(string,fmt,ap); 	//Store the string in the String buffer
	UART_SendStr(string);
	va_end(ap);	 
}
/*******************************************************************
  UART-Interrupthandler
  Wird durch den NVIC aufgerufen, wenn ein UART-Interrupt auftritt.
  Dies paasiert, wenn ein Zeichen angekommen ist oder
  ein Zeichen gesendet werden kann.
  Der Aufruf dieser Funktion mu? in stm32f10x_it.c eingetragen werden.
********************************************************************/
#if UART_INTERFACE_1 == UART_1
void USART1_IRQHandler(void)
#elif UART_INTERFACE_1 == UART_2
void USART2_IRQHandler(void)
#elif UART_INTERFACE_1 == UART_3
void USART3_IRQHandler(void)
#else
#error "UART interrupt handler not defined"
#endif
{
  if(USART_GetFlagStatus(USARTx,USART_FLAG_ORE) == SET) 
  {
      USART_ClearFlag(USARTx,USART_FLAG_ORE);
      USART_ReceiveData(USARTx);
  }
  if(USART_GetFlagStatus(USARTx,USART_FLAG_PE) == SET) 
  {
      USART_ClearFlag(USARTx,USART_FLAG_PE);
      USART_ReceiveData(USARTx);
  }
  if(USART_GetFlagStatus(USARTx,USART_FLAG_FE) == SET) 
  {
      USART_ClearFlag(USARTx,USART_FLAG_FE);
      USART_ReceiveData(USARTx);
  }
  if(USARTx->SR & USART_FLAG_RXNE)
  {
		u8 Res = USART_ReceiveData(USARTx);									
		//USARTx->DR = (Res & (uint16_t)0x01FF);
		if((USART1_RX_STA&0x8000)==0)												
		{
				if(Res==0x0d || Res==0x0a )	USART1_RX_STA|=0x8000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
//			if(USART1_RX_STA&0x4000)															//接收到了0x0d
//			{
//				if(Res!=0x0a)	USART1_RX_STA=0;											//接收错误,重新开始
//				else 					USART1_RX_STA|=0x8000;												//接收完成了 
//			}
//			else 																								//还没收到0X0D
//			{	
//				if(Res==0x0d)	USART1_RX_STA|=0x4000;
//				else
//				{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//				}		 
//			}
		} 
		USART_ClearFlag(USARTx, USART_FLAG_RXNE);
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
	}
}

#endif


#if defined(UART_INTERFACE_2)

#if UART_INTERFACE_2==UART_1
   #define USARTy USART1
#elif UART_INTERFACE_2==UART_2
   #define USARTy USART2
#elif UART_INTERFACE_2==UART_3
   #define USARTy USART3
#else
  #error "UART device not supported"
#endif

/*******************************************************************
   Funktion: InitUART_2()
   Parameter: Baudrate: Baudrate der Schnittstelle (z.B. 9600)

   Zweck: Intialisierung der UART
********************************************************************/
void InitUART_2(int BaudRate)
{
  USART_InitTypeDef  UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  #if UART_INTERFACE_2 == UART_2
  //UART2 aktivieren
  USART_DeInit(USART2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  //GPIOA aktivieren (UART2-Pins)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);

  //UART2-Pins zuweisen (PA2 und PA3)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_2 == UART_1
  //UART1 aktivieren
  USART_DeInit(USART1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  //UART1-Pins zuweisen (PA9 und PA10)
  GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_2 == UART_3

  USART_DeInit(USART3);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  //UART3-Pins zuweisen (PB10 and PB11) GPIO_PartialRemap_DISABLE
	//UART3-Pins zuweisen (PD8 and PD9)
  GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  #else
  #error "UART pin mapping not defined"
  #endif

  //UART konfigurieren
  USART_StructInit(&UART_InitStructure);
  UART_InitStructure.USART_BaudRate=BaudRate;

  USART_Init(USARTy,&UART_InitStructure);

  //Interrupt f? UART aktivieren
  #if UART_INTERFACE_2 == UART_2
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  #elif UART_INTERFACE_2 == UART_1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  #elif UART_INTERFACE_2 == UART_3
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  #else
  #error "Device not supported!"
  #endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_INTR_PRI+1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  USART_ClearFlag(USARTy, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USARTy,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,DISABLE);	//ENABLE
  USART_ITConfig(USARTy,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USARTy,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_ERR ,DISABLE);

  USART_Cmd(USARTy, ENABLE);
}


/*******************************************************************
  UART-Interrupthandler
  Wird durch den NVIC aufgerufen, wenn ein UART-Interrupt auftritt.
  Dies paasiert, wenn ein Zeichen angekommen ist oder
  ein Zeichen gesendet werden kann.
  Der Aufruf dieser Funktion mu? in stm32f10x_it.c eingetragen werden.
********************************************************************/
#if UART_INTERFACE_2 == UART_1
void USART1_IRQHandler(void)
#elif UART_INTERFACE_2 == UART_2
void USART2_IRQHandler(void)
#elif UART_INTERFACE_2 == UART_3
void USART3_IRQHandler(void)
#else
#error "UART interrupt handler not defined"
#endif
{
  if(USART_GetFlagStatus(USARTy,USART_FLAG_ORE) == SET) 
  {
      USART_ClearFlag(USARTy,USART_FLAG_ORE);
      USART_ReceiveData(USARTy);
  }
  if(USART_GetFlagStatus(USARTy,USART_FLAG_PE) == SET) 
  {
      USART_ClearFlag(USARTy,USART_FLAG_PE);
      USART_ReceiveData(USARTy);
  }
  if(USART_GetFlagStatus(USARTy,USART_FLAG_FE) == SET) 
  {
      USART_ClearFlag(USARTx,USART_FLAG_FE);
      USART_ReceiveData(USARTy);
  }
  if(USARTy->SR & USART_FLAG_RXNE)
  {
		u8 Res = USART_ReceiveData(USARTy);	
		if((USART3_RX_STA&0x8000)==0)			
		{
			if(Res==0x0d || Res==0x0a) 
			{
				if(USART3_RX_STA!=0)
				USART3_RX_STA|=0x8000;		
			}
			else
			{
				USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
				USART3_RX_STA++;
				if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;
			}	
		}
		USART_ClearFlag(USARTy, USART_FLAG_RXNE);
		USART_ClearITPendingBit(USARTy, USART_IT_RXNE);
	}
}
/*******************************************************************
   Funktion: DisableRS485()
   Parameter: ---

   R?kgabewert: ---

   Zweck: Abschalten der RS485-Schnittstelle zum TMC562
********************************************************************/
void DisableRS485(void)
{
  USART_ITConfig(USARTy,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,DISABLE);
}


/*******************************************************************
   Funktion: DisableRS485()
   Parameter: ---

   R?kgabewert: ---

   Zweck: Wiedereinschalten der RS485-Schnittstelle zum TMC562
********************************************************************/
void EnableRS485(void)
{
  USART_ITConfig(USARTy,USART_IT_TXE ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,ENABLE);
}

#endif
