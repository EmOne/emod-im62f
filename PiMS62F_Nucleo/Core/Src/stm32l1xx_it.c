/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "spi.h"
#include "usart.h"
#include "ComSLIP.h"
#include "WiMODLRHCI.h"
#include "WiMODLoRaWAN.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
#ifdef STM32L1
//extern PCD_HandleTypeDef hpcd_USB_FS;
//extern ADC_HandleTypeDef hadc;
//extern I2C_HandleTypeDef hi2c1;
//extern WWDG_HandleTypeDef hwwdg;
#else
//extern PCD_HandleTypeDef hpcd_USB_FS;
//extern ADC_HandleTypeDef hadc;
//extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
//extern WWDG_HandleTypeDef hwwdg;
#endif

/* USER CODE BEGIN EV */
extern TWiMODLORAWAN_ActivateDeviceData activationData;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l1xx.s).                    */
/******************************************************************************/

///**
//  * @brief This function handles Window watchdog interrupt.
//  */
//void WWDG_IRQHandler(void)
//{
//  /* USER CODE BEGIN WWDG_IRQn 0 */
//
//  /* USER CODE END WWDG_IRQn 0 */
//  HAL_WWDG_IRQHandler(&hwwdg);
//  /* USER CODE BEGIN WWDG_IRQn 1 */
//
//  /* USER CODE END WWDG_IRQn 1 */
//}
//
///**
//  * @brief This function handles ADC global interrupt.
//  */
//void ADC1_IRQHandler(void)
//{
//  /* USER CODE BEGIN ADC1_IRQn 0 */
//
//  /* USER CODE END ADC1_IRQn 0 */
//  HAL_ADC_IRQHandler(&hadc);
//  /* USER CODE BEGIN ADC1_IRQn 1 */
//
//  /* USER CODE END ADC1_IRQn 1 */
//}
//
///**
//  * @brief This function handles USB high priority interrupt.
//  */
//void USB_HP_IRQHandler(void)
//{
//  /* USER CODE BEGIN USB_HP_IRQn 0 */
//
//  /* USER CODE END USB_HP_IRQn 0 */
//  HAL_PCD_IRQHandler(&hpcd_USB_FS);
//  /* USER CODE BEGIN USB_HP_IRQn 1 */
//
//  /* USER CODE END USB_HP_IRQn 1 */
//}
//
///**
//  * @brief This function handles USB low priority interrupt.
//  */
//void USB_LP_IRQHandler(void)
//{
//  /* USER CODE BEGIN USB_LP_IRQn 0 */
//
//  /* USER CODE END USB_LP_IRQn 0 */
//  HAL_PCD_IRQHandler(&hpcd_USB_FS);
//  /* USER CODE BEGIN USB_LP_IRQn 1 */
//
//  /* USER CODE END USB_LP_IRQn 1 */
//}

///**
//  * @brief This function handles I2C1 event interrupt.
//  */
//void I2C1_EV_IRQHandler(void)
//{
//  /* USER CODE BEGIN I2C1_EV_IRQn 0 */
//
//  /* USER CODE END I2C1_EV_IRQn 0 */
//  HAL_I2C_EV_IRQHandler(&hi2c1);
//  /* USER CODE BEGIN I2C1_EV_IRQn 1 */
//
//  /* USER CODE END I2C1_EV_IRQn 1 */
//}
//
///**
//  * @brief This function handles I2C1 error interrupt.
//  */
//void I2C1_ER_IRQHandler(void)
//{
//  /* USER CODE BEGIN I2C1_ER_IRQn 0 */
//
//  /* USER CODE END I2C1_ER_IRQn 0 */
//  HAL_I2C_ER_IRQHandler(&hi2c1);
//  /* USER CODE BEGIN I2C1_ER_IRQn 1 */
//
//  /* USER CODE END I2C1_ER_IRQn 1 */
//}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi1);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
  /* USER CODE BEGIN SPI2_IRQn 0 */

  /* USER CODE END SPI2_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi2);
  /* USER CODE BEGIN SPI2_IRQn 1 */

  /* USER CODE END SPI2_IRQn 1 */
}

/**
  * @brief This function handles SPI3 global interrupt.
  */
void SPI3_IRQHandler(void)
{
  /* USER CODE BEGIN SPI3_IRQn 0 */

  /* USER CODE END SPI3_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi3);
  /* USER CODE BEGIN SPI3_IRQn 1 */

  /* USER CODE END SPI3_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}


/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
//  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  /* USER CODE END TIM2_IRQn 1 */
}

///**
//  * @brief This function handles TIM3 global interrupt.
//  */
//void TIM3_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM3_IRQn 0 */
//
//  /* USER CODE END TIM3_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim3);
//  /* USER CODE BEGIN TIM3_IRQn 1 */
//
//  /* USER CODE END TIM3_IRQn 1 */
//}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
//	TWiMODLRHCI.WaitForResponse(0x01, 0x01);
	TWiMODLRHCI.Process();
	if(EmWimodData.ReceiveFrmUserRequest) {
		WiMODLoRaWAN.Process(&TWiMODLRHCI.Rx.Message);
		EmWimodData.ReceiveFrmUserRequest = 0x0;
	}
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
//  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

  /* USER CODE END TIM4_IRQn 1 */
}

///**
//  * @brief This function handles TIM6 global interrupt.
//  */
//void TIM6_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM6_IRQn 0 */
//
//  /* USER CODE END TIM6_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim6);
//  /* USER CODE BEGIN TIM6_IRQn 1 */
//
//  /* USER CODE END TIM6_IRQn 1 */
//}

/**
  * @brief This function handles TIM8 global interrupt.
  */
void TIM8_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_IRQn 0 */

  /* USER CODE END TIM8_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_IRQn 1 */

  /* USER CODE END TIM8_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

