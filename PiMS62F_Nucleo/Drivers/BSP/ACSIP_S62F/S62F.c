/*!
 * \file      sx1262dvk1cas-board.c
 *
 * \brief     Target board SX1261DVK1BAS shield driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
/**
  ******************************************************************************
  * @file    sx1262dvk1cas.c
  * @author  MCD Application Team
  * @brief   driver sx1262dvk1cas board
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "S62F.h"
#include "sx126x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BOARD_TCXO_WAKEUP_TIME  0 // no TCXO

#if !defined (USE_EMOD_IMS62F)
	#error "Please define USE_EMOD_IMS62F"
#endif

#define RADIO_GET_STATUS                        0xC0
/* Private macro -------------------------------------------------------------*/
/* static */ EXTI_HandleTypeDef hRADIO_DIO_exti[RADIO_DIOn];

static void S62F_RADIO_SPI_IoInit(SPI_HandleTypeDef *spiHandle);
static void S62F_RADIO_SPI_IoDeInit(void);
/* Private function prototypes -----------------------------------------------*/
static void S62F_RADIO_AntSwOn( void );
static void S62F_RADIO_AntSwOff( void );
/*!
 * \brief Writes new Tx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void S62F_RADIO_DbgPinTxWrite( uint8_t state );

/*!
 * \brief Writes new Rx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void S62F_RADIO_DbgPinRxWrite( uint8_t state );
/* Exported functions --------------------------------------------------------*/

void S62F_RADIO_IoInit( void )
{
  GPIO_InitTypeDef initStruct={0};
  SPI_HandleTypeDef dummy_hspi;

//   Set DioIrqHandler port in IT_RISING mode
  RADIO_DIO_1_GPIO_CLK_ENABLE();
  initStruct.Mode = GPIO_MODE_IT_RISING;
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_HIGH;
  initStruct.Pin = RADIO_DIO_1_PIN;
  HAL_GPIO_Init(RADIO_DIO_1_PORT, &initStruct);

  /* Radio IO Init */
	RADIO_DEV_SEL_CLK_ENABLE();
	RADIO_BUSY_CLK_ENABLE();
  initStruct.Mode = GPIO_MODE_INPUT;
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Pin = DEVICE_SEL_PIN;
  HAL_GPIO_Init( DEVICE_SEL_PORT, &initStruct);
  initStruct.Pin = RADIO_BUSY_PIN;
  HAL_GPIO_Init( RADIO_BUSY_PORT, &initStruct);
  
  /* Antenna IO Init */
//    RADIO_ANT_SWITCH_POWER_CLK_ENABLE();
//    initStruct.Pull = GPIO_NOPULL;
//    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    initStruct.Pin = RADIO_ANT_SWITCH_POWER_PIN;
//    HAL_GPIO_Init(RADIO_ANT_SWITCH_POWER_PORT, &initStruct);
//    HAL_GPIO_WritePin(RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, GPIO_PIN_RESET);

  /* SPI IO Init */
  /* Normally done by the HAL_MSP callback but not for this applic */
  S62F_RADIO_SPI_IoInit(&dummy_hspi);

  /* NSS initialization */
  RADIO_NSS_CLK_ENABLE();
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Mode = GPIO_MODE_OUTPUT_PP;
  initStruct.Pin = RADIO_NSS_PIN;
  HAL_GPIO_Init( RADIO_NSS_PORT, &initStruct );
  HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_SET);

  initStruct.Pin = RADIO_LEDTX_PIN;
  HAL_GPIO_Init( RADIO_LEDTX_PORT, &initStruct );
  HAL_GPIO_WritePin( RADIO_LEDTX_PORT, RADIO_LEDTX_PIN,  GPIO_PIN_RESET );
  
  initStruct.Pin = RADIO_LEDRX_PIN;
  HAL_GPIO_Init( RADIO_LEDRX_PORT, &initStruct );
  HAL_GPIO_WritePin( RADIO_LEDRX_PORT, RADIO_LEDRX_PIN,  GPIO_PIN_RESET );

  initStruct.Pin = RADIO_RX_SWITCH_POWER_PIN;
  HAL_GPIO_Init( RADIO_RX_SWITCH_POWER_PORT, &initStruct);
  HAL_GPIO_WritePin( RADIO_RX_SWITCH_POWER_PORT, RADIO_RX_SWITCH_POWER_PIN, GPIO_PIN_RESET);

  initStruct.Pin = RADIO_TX_SWITCH_POWER_PIN;
  HAL_GPIO_Init( RADIO_TX_SWITCH_POWER_PORT, &initStruct);
  HAL_GPIO_WritePin( RADIO_TX_SWITCH_POWER_PORT, RADIO_TX_SWITCH_POWER_PIN, GPIO_PIN_RESET);
}

void S62F_RADIO_IoDeInit( void )
{
	/* SPI IO DeInit */
	S62F_RADIO_SPI_IoDeInit();

	GPIO_InitTypeDef initStruct={0};
	initStruct.Mode = GPIO_MODE_INPUT;
	initStruct.Pull = GPIO_NOPULL;
	initStruct.Pin = RADIO_DIO_1_PIN;
	HAL_GPIO_Init( RADIO_DIO_1_PORT, &initStruct);

	initStruct.Pin = RADIO_BUSY_PIN;
	HAL_GPIO_Init( RADIO_BUSY_PORT, &initStruct);
}

void S62F_RADIO_IoIrqInit( DioIrqHandler **irqHandlers )
{
//  HW_GPIO_SetIrq( RADIO_DIO_1_PORT, RADIO_DIO_1_PIN, IRQ_HIGH_PRIORITY, dioIrq );
  HAL_EXTI_GetHandle(&hRADIO_DIO_exti[0], RADIO_DIO_1_EXTI_LINE);
  HAL_EXTI_RegisterCallback(&hRADIO_DIO_exti[0], HAL_EXTI_COMMON_CB_ID, irqHandlers[0]);
  HAL_NVIC_SetPriority(RADIO_DIO_1_IRQn, RADIO_DIO_1_IT_PRIO, 0x00);
  HAL_NVIC_EnableIRQ(RADIO_DIO_1_IRQn);
}

/*!
 * @brief it points the demanded line in the EXTI_HandleTypeDef TABLE before calling HAL_EXTI_IRQHandler()
 * @param [IN]: DIO extiLine demanded
 * @retval none
 * @note "stm32lyxx_it.c" code generated by MX do not call this function but directly the HAL_EXTI_IRQHandler()
 */
void S62F_RADIO_IRQHandler(RADIO_DIO_TypeDef DIO)
{
  HAL_EXTI_IRQHandler(&hRADIO_DIO_exti[DIO]);
}

uint32_t S62F_RADIO_GetWakeUpTime(void)
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void S62F_RADIO_Reset( void )
{
  GPIO_InitTypeDef initStruct = { 0 };

  RADIO_RESET_CLK_ENABLE();

  initStruct.Mode =GPIO_MODE_OUTPUT_PP;
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_HIGH;

  // Wait 10 ms
  HAL_Delay( 10 );
  
  // Set RESET pin to 0
  initStruct.Pin = RADIO_RESET_PIN;
  HAL_GPIO_Init(RADIO_RESET_PORT, &initStruct);
  HAL_GPIO_WritePin(RADIO_RESET_PORT, RADIO_RESET_PIN, GPIO_PIN_RESET);
  
  // Wait 20 ms
  HAL_Delay( 20 );
  // Configure RESET as input
  initStruct.Mode = GPIO_MODE_ANALOG;
  initStruct.Pin = RADIO_RESET_PIN;
  HAL_GPIO_Init(RADIO_RESET_PORT, &initStruct);
  HAL_GPIO_WritePin(RADIO_RESET_PORT, RADIO_RESET_PIN, GPIO_PIN_RESET);  // internal pull-up
  
  // Wait 10 ms
  HAL_Delay( 10 );
}

void S62F_RADIO_WaitOnBusy( void )
{
	  while (HAL_GPIO_ReadPin(RADIO_BUSY_PORT, RADIO_BUSY_PIN) == 1);
}

void S62F_RADIO_WakeUp(void)
{
    //NSS = 0;
	HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_RESET);

//    HW_SPI_InOut(RADIO_GET_STATUS);
//    HW_SPI_InOut(0);

    S62F_RADIO_SendRecv(RADIO_GET_STATUS);
    S62F_RADIO_SendRecv(0);

    //NSS = 1;
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_SET);

    // Wait for chip to be ready.
    S62F_RADIO_WaitOnBusy();
}

void S62F_RADIO_ChipSelect(int32_t state)
{
  if (state == 0)
  {
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, GPIO_PIN_SET);
  }
}

uint8_t S62F_RADIO_GetPaSelect( uint32_t channel )
{
	if (HAL_GPIO_ReadPin(DEVICE_SEL_PORT, DEVICE_SEL_PIN) ==  GPIO_PIN_SET)
    {
        return SX1261;
    }
    else
    {
        return SX1262;
    }
}

void S62F_RADIO_SetAntSw(RfSw_TypeDef state)
{
  switch (state)
  {
    case RFSW_OFF:
    	S62F_RADIO_AntSwOff();
//      HAL_GPIO_WritePin(RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, GPIO_PIN_RESET);
      break;
    case RFSW_ON:
    	S62F_RADIO_AntSwOn();
//      HAL_GPIO_WritePin(RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, GPIO_PIN_SET);
      break;
    default:
    	S62F_RADIO_AntSwOn();
//      HAL_GPIO_WritePin(RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, GPIO_PIN_SET);
      break;
  }
}

bool S62F_RADIO_CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported

    return true;
}

bool S62F_RADIO_BoardIsTcxo(void)
{ /*no TCXO on board*/
  return false;
}

void S62F_RADIO_BoardSetLedTx( bool state )
{
	if (state == false)	{
		HAL_GPIO_WritePin(RADIO_LEDTX_PORT, RADIO_LEDTX_PIN, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(RADIO_LEDTX_PORT, RADIO_LEDTX_PIN, GPIO_PIN_SET);
	}
}

void S62F_RADIO_BoardSetLedRx( bool state )
{
	if (state == false) {
		HAL_GPIO_WritePin(RADIO_LEDRX_PORT, RADIO_LEDRX_PIN, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(RADIO_LEDRX_PORT, RADIO_LEDRX_PIN, GPIO_PIN_SET);
	}
}

/* Bus mapping to SPI */
void S62F_RADIO_Bus_Init(void)
{
  RADIO_SPI_Init();
}

void S62F_RADIO_Bus_deInit(void)
{
  RADIO_SPI_DeInit();
}

/**
  * @brief  Send Receive data via SPI
  * @param  Data to send
  * @retval Received data
  */
uint16_t S62F_RADIO_SendRecv(uint16_t txData)
{
  uint16_t rxData ;

  /*RADIO_SPI_SendRecv(( uint8_t * ) &txData, ( uint8_t* ) &rxData, 1);*/
  RADIO_SPI_SendRecv((uint8_t *) &txData, (uint8_t *) &rxData, 1);

  return rxData;
}

/* Private functions --------------------------------------------------------*/

/**
  * @brief  Initializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
*/
static void S62F_RADIO_SPI_IoInit(SPI_HandleTypeDef *spiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
  /* Enable Peripheral clock */
  RADIO_SPI_SCK_GPIO_CLK_ENABLE();
  RADIO_SPI_MOSI_GPIO_CLK_ENABLE();
  RADIO_SPI_MISO_GPIO_CLK_ENABLE();

  /**SPI1 GPIO Configuration
  PA5     ------> SPI1_SCK
  PA6     ------> SPI1_MISO
  PA7     ------> SPI1_MOSI
    */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Alternate = RADIO_SPI_MOSI_GPIO_AF;
  GPIO_InitStruct.Pin = RADIO_SPI_MOSI_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Alternate = RADIO_SPI_MISO_GPIO_AF;
  GPIO_InitStruct.Pin = RADIO_SPI_MISO_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Alternate = RADIO_SPI_SCK_GPIO_AF;
  GPIO_InitStruct.Pin = RADIO_SPI_SCK_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  DeInitializes SPI MSP.
  * @param  hspi  SPI handler
  * @retval None
  */
static void S62F_RADIO_SPI_IoDeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Peripheral clock disable */
  /* no need to call SPI1_CLK_DISABLE() because going in LowPower it gets disabled automatically */

  /* DeInitialize Peripheral GPIOs */
  /* Instead of using HAL_GPIO_DeInit() which set ANALOG mode
     it's preferred to set in OUTPUT_PP mode, with the pins set to 0 */

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = RADIO_SPI_MOSI_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RADIO_SPI_MISO_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RADIO_SPI_SCK_GPIO_PIN;
  HAL_GPIO_Init(RADIO_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);


  HAL_GPIO_WritePin(RADIO_SPI_MOSI_GPIO_PORT, RADIO_SPI_MOSI_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RADIO_SPI_MISO_GPIO_PORT, RADIO_SPI_MISO_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RADIO_SPI_SCK_GPIO_PORT, RADIO_SPI_SCK_GPIO_PIN, GPIO_PIN_RESET);
}
///////////////////////////////////////////////////////////////////////////////////////

void SX126xIoTcxoInit( void )
{
    SX126xSetDio3AsTcxoCtrl( 0x06, 1000 );
    // No TCXO component available on this board design.
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
    // GpioInit( &AntTx, RADIO_ANT_SWITCH_TX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
    // GpioInit( &AntRx, RADIO_ANT_SWITCH_RX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
}

//void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
//{
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut(command);
//
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        HW_SPI_InOut( buffer[i] );
//    }
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    if( command != RADIO_SET_SLEEP )
//    {
//        SX126xWaitOnBusy( );
//    }
//}
//
//uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
//{
//    uint8_t status = 0;
//
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut(command);
//    status = HW_SPI_InOut(0x00 );
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        buffer[i] = HW_SPI_InOut( 0 );
//    }
//
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    SX126xWaitOnBusy( );
//
//    return status;
//}
//
//void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
//{
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut(RADIO_WRITE_REGISTER );
//    HW_SPI_InOut(( address & 0xFF00 ) >> 8 );
//    HW_SPI_InOut(address & 0x00FF );
//
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        HW_SPI_InOut(buffer[i] );
//    }
//
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    SX126xWaitOnBusy( );
//}
//
//void SX126xWriteRegister( uint16_t address, uint8_t value )
//{
//    SX126xWriteRegisters( address, &value, 1 );
//}
//
//void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
//{
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut(RADIO_READ_REGISTER);
//    HW_SPI_InOut(( address & 0xFF00) >> 8 );
//    HW_SPI_InOut(address & 0x00FF);
//    HW_SPI_InOut(0);
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        buffer[i] = HW_SPI_InOut( 0 );
//    }
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    SX126xWaitOnBusy( );
//}
//
//uint8_t SX126xReadRegister( uint16_t address )
//{
//    uint8_t data;
//    SX126xReadRegisters( address, &data, 1 );
//    return data;
//}
//
//void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
//{
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut( RADIO_WRITE_BUFFER );
//    HW_SPI_InOut( offset );
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        HW_SPI_InOut( buffer[i] );
//    }
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    SX126xWaitOnBusy( );
//}
//
//
//void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
//{
//    SX126xCheckDeviceReady( );
//
//    //NSS = 0;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
//
//    HW_SPI_InOut( RADIO_READ_BUFFER );
//    HW_SPI_InOut( offset );
//    HW_SPI_InOut( 0 );
//    for( uint16_t i = 0; i < size; i++ )
//    {
//        buffer[i] = HW_SPI_InOut( 0 );
//    }
//
//    //NSS = 1;
//    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
//
//    SX126xWaitOnBusy( );
//}

static void S62F_RADIO_AntSwOn( void )
{
	S62F_RADIO_DbgPinTxWrite(0);
	S62F_RADIO_DbgPinRxWrite(1);
//  HW_GPIO_Write( RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, 1);
}

static void S62F_RADIO_AntSwOff( void )
{
	S62F_RADIO_DbgPinTxWrite(0);
	S62F_RADIO_DbgPinRxWrite(0);
//  HW_GPIO_Write( RADIO_ANT_SWITCH_POWER_PORT, RADIO_ANT_SWITCH_POWER_PIN, 0);
}


static void S62F_RADIO_DbgPinTxWrite( uint8_t state )
{
//    GpioWrite( &DbgPinTx, state );
    HAL_GPIO_WritePin( RADIO_TX_SWITCH_POWER_PORT, RADIO_TX_SWITCH_POWER_PIN, state);
}

static void S62F_RADIO_DbgPinRxWrite( uint8_t state )
{
//    GpioWrite( &DbgPinRx, state );
    HAL_GPIO_WritePin( RADIO_RX_SWITCH_POWER_PORT, RADIO_RX_SWITCH_POWER_PIN, state);

}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
