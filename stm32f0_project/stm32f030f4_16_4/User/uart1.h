#ifndef __UART1_H
#define __UART1_H 		

#include "bsp_defines.h" 

#define USART1_REC_LEN					100

extern uint8_t  								USART1_RX_BUF[USART1_REC_LEN]; 
extern uint16_t 								USART1_RX_STA;

#define REPLY_OK 								0               //!< command successfully executed
#define REPLY_CHKERR 						1              	//!< checksum error
#define REPLY_INVALID_CMD		 		2        			  //!< command not supported
#define REPLY_WRONG_TYPE 				3          		  //!< wrong type code
#define REPLY_INVALID_VALUE 		4       				//!< wrong value
#define REPLY_EEPROM_LOCKED 		5       				//!< EEPROM is locked
#define REPLY_CMD_NOT_AVAILABLE 6   						//!< command not available due to current state
#define REPLY_CMD_LOAD_ERROR 		7      					//!< error when storing command to EEPROM
#define REPLY_WRITE_PROTECTED 	8     					//!< EEPROM is write protected
#define REPLY_MAX_EXCEEDED 			9        				//!< maximum number of commands in EEPROM exceeded


void uart1_init(int BaudRate);

#endif





























