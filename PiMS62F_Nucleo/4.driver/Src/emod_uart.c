/*
 * emod_uart.c
 *
 *  Created on: Dec 19, 2021
 *      Author: HP
 */

#include <emod_uart.h>

/*------------------------------------extern variable--------------------------------*/
static UART_HandleTypeDef* uartHandler;
extern uint8_t charRx;
/*-------------------------------------main variable---------------------------------*/
uint8_t Rx2_buffer[kMaxUARTPayloadSize];
Ci EmWimodData;

Stream  Serial =
{
	emod_UART_Init,
	emod_UART_DeInit,
	emod_UART_Read,
	emod_UART_Write,
	emod_UART_Print,
};

/*-------------------------------------main function---------------------------------*/
void emod_UART_Init (void * handler, uint32_t baudrate)
{
	uartHandler = (UART_HandleTypeDef *) handler;
}


void emod_UART_DeInit (void)
{

}

int emod_UART_Read (void)
{
	uint16_t lenght = 0x00;

	if(CmdFrmUserIsReceived(&EmWimodData)) {
	  lenght = EmWimodData.CmdFrmUserLen;
	}
	return lenght;
}

void emod_UART_Write (uint8_t data)
{
	HAL_UART_Transmit(uartHandler, &data, 1, 0xFF);
}

void emod_UART_Print (uint8_t* data)
{
	uint16_t size;

	size = strlen((char *) data);
	HAL_UART_Transmit(uartHandler, data, size, 0xFF);
}

/* ------------------------------------------------------------------- From User ------------------------------------------------------------*/
uint8_t CmdFrmUserIsReceived(Ci * pData)
{
//	uint16_t len = 0x0000;
//	uint8_t * pBuffer;
//	uint8_t * pRawdata;

    if(((pData->CmdFrmUserCount != pData->CmdFrmUserLen) && (pData->CmdFrmUserCount < kMaxUARTPayloadSize)) || (pData->CmdFrmUserCount == 0x0))
    {
        pData->CmdFrmUserLen = pData->CmdFrmUserCount;
        pData->ReceiveFrmUserRequest = 0;
    }
    else if((pData->CmdFrmUserCount == pData->CmdFrmUserLen) && (pData->CmdFrmUserCount < kMaxUARTPayloadSize))
    {
        // found something
        pData->ReceiveFrmUserRequest = 1;
        pData->CmdFrmUserCount = 0;
    }
    else{
        pData->CmdFrmUserLen = 0x0;
        pData->CmdFrmUserCount = 0x0;
        pData->ReceiveFrmUserRequest = 0x0;
    }
    return pData->ReceiveFrmUserRequest;
}


/* ----------------------UART RX Complete and Error Callback----------------------*/

void emod_UART_RxCpltCallback(uint8_t *rxChar, uint16_t size, uint8_t error)
{

//    if(huart == uartHandler) {
//    	__disable_irq();
	UTILS_ENTER_CRITICAL_SECTION( );
        Rx2_buffer[EmWimodData.CmdFrmUserCount++] = *rxChar;
        UTILS_EXIT_CRITICAL_SECTION( );
//        __enable_irq();
//        HAL_UART_Receive_IT(uartHandler, rxChar, 1);
//    }
}

void emod_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

    if(huart == uartHandler) {

        __USART1_FORCE_RESET();
        __USART1_RELEASE_RESET();
//        UART2_ReInit();
    	HAL_UART_Init(huart);

        EmWimodData.ReceiveFrmUserRequest = 0x0;
        EmWimodData.CmdFrmUserCount = 0x0;
        SetValue(&Rx2_buffer[0], 0, kMaxUARTPayloadSize);

        charRx = 0x00;

        HAL_UART_Receive_IT(huart, &charRx, 1);
    }

}

/*-----------------------------------------local function-------------------------------------*/

void SetValue (uint8_t * pdata, uint8_t data, uint16_t size)
{
	uint16_t count = 0x00;
	uint8_t value;
	uint8_t * pbuffer;

	pbuffer = pdata;
	value = data;

	while(count < size) {

		*pbuffer = value;
		pbuffer++;
		count++;
	}
}

