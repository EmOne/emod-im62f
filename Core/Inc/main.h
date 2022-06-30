/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DI_Pin GPIO_PIN_11
#define DI_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_2
#define CS_GPIO_Port GPIOD
#define ENCODER_M_Pin GPIO_PIN_5
#define ENCODER_M_GPIO_Port GPIOB
#define CK_Pin GPIO_PIN_10
#define CK_GPIO_Port GPIOC
#define DO_Pin GPIO_PIN_12
#define DO_GPIO_Port GPIOC
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define TXD_Pin GPIO_PIN_9
#define TXD_GPIO_Port GPIOA
#define IO2_Pin GPIO_PIN_7
#define IO2_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_9
#define SDA_GPIO_Port GPIOB
#define RXD_Pin GPIO_PIN_10
#define RXD_GPIO_Port GPIOA
#define ENCODER_P_Pin GPIO_PIN_4
#define ENCODER_P_GPIO_Port GPIOB
#define RF_RX_SW_Pin GPIO_PIN_13
#define RF_RX_SW_GPIO_Port GPIOC
#define IO1_Pin GPIO_PIN_8
#define IO1_GPIO_Port GPIOA
#define WKUP_Pin GPIO_PIN_0
#define WKUP_GPIO_Port GPIOA
#define MOSI_Pin GPIO_PIN_15
#define MOSI_GPIO_Port GPIOB
#define MISO_Pin GPIO_PIN_14
#define MISO_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define NSS_Pin GPIO_PIN_12
#define NSS_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define RF_MISO_Pin GPIO_PIN_6
#define RF_MISO_GPIO_Port GPIOA
#define ADC1_Pin GPIO_PIN_1
#define ADC1_GPIO_Port GPIOA
#define RF_DIO2_Pin GPIO_PIN_11
#define RF_DIO2_GPIO_Port GPIOB
#define RF_DIO2_EXTI_IRQn EXTI15_10_IRQn
#define RF_BUSY_Pin GPIO_PIN_1
#define RF_BUSY_GPIO_Port GPIOB
#define RF_BUSY_EXTI_IRQn EXTI1_IRQn
#define RF_SCk_Pin GPIO_PIN_5
#define RF_SCk_GPIO_Port GPIOA
#define RF_NRST_Pin GPIO_PIN_2
#define RF_NRST_GPIO_Port GPIOA
#define RF_DIO1_Pin GPIO_PIN_10
#define RF_DIO1_GPIO_Port GPIOB
#define RF_DIO1_EXTI_IRQn EXTI15_10_IRQn
#define RF_NSS_Pin GPIO_PIN_0
#define RF_NSS_GPIO_Port GPIOB
#define RF_MOSI_Pin GPIO_PIN_7
#define RF_MOSI_GPIO_Port GPIOA
#define RF_TX_SW_Pin GPIO_PIN_4
#define RF_TX_SW_GPIO_Port GPIOA
#define ADC2_Pin GPIO_PIN_3
#define ADC2_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
