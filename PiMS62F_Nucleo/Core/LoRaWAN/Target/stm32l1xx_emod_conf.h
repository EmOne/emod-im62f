/**
  ******************************************************************************
  * @file           : stm32l1xx_emod_conf.h
  * @brief          : Configuration file
  ******************************************************************************
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L1XX_EMOD_CONF_H
#define STM32L1XX_EMOD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L1XX_EMOD
  * @{
  */

/** @defgroup STM32L1XX_EMOD_CONFIG Config
  * @{
  */

/** @defgroup STM32L1XX_EMOD_CONFIG_Exported_Constants
  * @{
  */
/* COM Feature define */
#define USE_BSP_COM_FEATURE                 1U

/* COM define */
#define USE_COM_LOG                         1U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY         15U

/* I2C1 Frequency in Hz  */
#define BUS_I2C1_FREQUENCY                  100000U /* Frequency of I2C1 = 100 KHz*/

/* SPI1 Baud rate in bps  */
#define BUS_SPI1_BAUDRATE                   16000000U /* baud rate of SPIn = 16 Mbps */

/* UART1 Baud rate in bps  */
#define BUS_UART1_BAUDRATE                  115200U /* baud rate of UARTn = 9600 baud */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif  /* STM32L1XX_EMOD_CONF_H */

/************************ (C) COPYRIGHT EmOne *****END OF FILE****/
