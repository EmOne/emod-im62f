/*
 * uart.h
 *
 *  Created on: Dec 19, 2021
 *      Author: HP
 */

#ifndef INC_UART_H_
#define INC_UART_H_

//------------------------------------------------------------------------------
//
// Include Files
//
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "CRC16.h"


#define kMaxUARTPayloadSize			300U

typedef union{

    uint8_t b[kMaxUARTPayloadSize];
    uint16_t w[kMaxUARTPayloadSize/2];
    uint32_t lw[kMaxUARTPayloadSize/4];

}tCi;

typedef struct {

	tCi CmdFrmUser;
	uint16_t CmdFrmUserCount;
	uint16_t CmdFrmUserLen;
	uint8_t ReceiveFrmUserRequest;

}Ci;

extern Ci EmWimodData;

typedef struct {

	void (*begin)(uint32_t baudrate);
	void (*end)(void);
	int (*read)(void);
	void (*write)(uint8_t data);
	void (*print)(uint8_t* data);
	int (*available)(void);

}Stream;

extern Stream  Serial;
extern uint8_t Rx2_buffer[kMaxUARTPayloadSize];
extern uint8_t Rx2_byte;
/*-----------------------------------------main function-------------------------------------*/
void UART_Init (uint32_t baudrate);
void UART_DeInit (void);
int UART_Read (void);
void UART_Write (uint8_t data);
void UART_Print (uint8_t* data);
uint8_t CmdFrmUserIsReceived(Ci * pData);
UINT8* ProcessRxMessage(UINT8* rxBuffer, UINT16 rxLength);
/*-----------------------------------------local function-------------------------------------*/
void SetValue (uint8_t * pdata, uint8_t data, uint16_t size);
#endif /* INC_UART_H_ */
