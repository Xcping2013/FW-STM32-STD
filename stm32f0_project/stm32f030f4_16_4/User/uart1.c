#include "uart1.h"

/**************************************************************************************/
typedef struct __FILE FILE;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
#if 0
if(ch=='\n')	
{
  USART_SendData(USART1,(uint8_t) '\r');
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{}	
}	
#endif
  USART_SendData(USART1,(uint8_t)  ch);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{}
 
  return (ch);
}


//
u8 	USART1_RX_BUF[USART1_REC_LEN];  
u16 USART1_RX_STA=0;  

void uart1_init(int BaudRate)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
					
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
					
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);        

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
	USART_Init(USART1, &USART_InitStructure); //������ṹ��
	
  USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USART1,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USART1,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USART1,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USART1,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USART1,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USART1,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USART1,USART_IT_ERR ,DISABLE);
	
	USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1

}			


// /r/n
void USART1_IRQHandler1(void)
{
	u8 Res;
  if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_ORE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_PE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_PE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_FE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_FE);
      USART_ReceiveData(USART1);
  }
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)
  {
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Res = USART_ReceiveData(USART1);															//��ȡ���յ�������	
		//USARTx->DR = (Res & (uint16_t)0x01FF);											//���ж��з��ʹ������ݻ��������
		if((USART1_RX_STA&0x8000)==0)																	//����δ���
		{
			if(USART1_RX_STA&0x4000)																			//���յ���0x0d
			{
				if(Res!=0x0a)	USART1_RX_STA=0;															//���մ���,���¿�ʼ
				else 					USART1_RX_STA|=0x8000;												//��������� 
			}
			else 																												//��û�յ�0X0D  /R
			{	
				if(Res==0x0d)	USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;				//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		} 
	}
}
// /r ����/n
void USART1_IRQHandler(void)
{
	u8 Res;
  if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_ORE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_PE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_PE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_FE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_FE);
      USART_ReceiveData(USART1);
  }
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)
  {
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Res = USART_ReceiveData(USART1);															//��ȡ���յ�������	
		if((USART1_RX_STA&0x8000)==0)																	//�����Ƿ������
		{
				if(Res==0x0d)	USART1_RX_STA|=0x8000;
				else if(Res==0x0a)	 {;}
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;				//�������ݴ���,���¿�ʼ����	  
				}		 
		} 
	}
}
// /n ����/r
void USART1_IRQHandler2(void)
{
	u8 Res;
  if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_ORE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_PE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_PE);
      USART_ReceiveData(USART1);
  }
  if(USART_GetFlagStatus(USART1,USART_FLAG_FE) == SET) 
  {
      USART_ClearFlag(USART1,USART_FLAG_FE);
      USART_ReceiveData(USART1);
  }
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)
  {
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Res = USART_ReceiveData(USART1);															//��ȡ���յ�������	
		if((USART1_RX_STA&0x8000)==0)																	//�����Ƿ������
		{
				if(Res==0x0a)	USART1_RX_STA|=0x8000;
				else if(Res==0x0d)	 {;}
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;				//�������ݴ���,���¿�ʼ����	  
				}		 
		} 
	}
}
//





