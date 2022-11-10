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
#include "radio_board_if.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wwdg.h"
#include "tim.h"
#include "usart.h"
#include "ComSLIP.h"
#include "WiMODLRHCI.h"
#include "WiMODLoRaWAN.h"
#include "stm32_lpm.h"
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
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
//extern PCD_HandleTypeDef hpcd_USB_FS;
extern TIM_HandleTypeDef htim6;
extern RTC_HandleTypeDef hrtc;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN EV */
extern EXTI_HandleTypeDef g_hExtiHandle;
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
	  __NOP();
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
	 HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/////******************************************************************************/
/////* STM32L1xx Peripheral Interrupt Handlers                                    */
/////* Add here the Interrupt Handlers for the used peripherals.                  */
/////* For the available peripheral interrupt handler names,                      */
/////* please refer to the startup file (startup_stm32l1xx.s).                    */
/////******************************************************************************/
////
/////**
////  * @brief This function handles USB high priority interrupt.
////  */
////void USB_HP_IRQHandler(void)
////{
////  /* USER CODE BEGIN USB_HP_IRQn 0 */
////
////  /* USER CODE END USB_HP_IRQn 0 */
////  HAL_PCD_IRQHandler(&hpcd_USB_FS);
////  /* USER CODE BEGIN USB_HP_IRQn 1 */
////
////  /* USER CODE END USB_HP_IRQn 1 */
////}
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

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
//	TWiMODLRHCI.WaitForResponse(0x01, 0x01);

//	if(EmWimodData.ReceiveFrmUserRequest) {
//		WiMODLoRaWAN.Process(&TWiMODLRHCI.Rx.Message);
//		EmWimodData.ReceiveFrmUserRequest = 0x0;
//		memset(&Rx2_buffer, 0x00, kMaxUARTPayloadSize);
////		UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_RX_Id), UTIL_LPM_ENABLE);
//	}
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
//  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  /* USER CODE END TIM4_IRQn 1 */
}

#if defined (USE_EMOD_IMS62F)
///**
//  * @brief This function handles TIM5 global interrupt.
//  */
//void TIM5_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM5_IRQn 0 */
//
//  /* USER CODE END TIM5_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim5);
//  /* USER CODE BEGIN TIM5_IRQn 1 */
//
//  /* USER CODE END TIM5_IRQn 1 */
//}

/**
  * @brief This function handles TIM9 global interrupt.
  */
void TIM9_IRQHandler(void)
{
  /* USER CODE BEGIN TIM9_IRQn 0 */

  /* USER CODE END TIM9_IRQn 0 */
  HAL_TIM_IRQHandler(&htim9);
  /* USER CODE BEGIN TIM9_IRQn 1 */

  /* USER CODE END TIM9_IRQn 1 */
}
#else
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
#endif
/**
  * @brief This function handles TIM6 global interrupt.
  */
//void TIM6_IRQHandler(void)
//{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
//}

/* USER CODE BEGIN 1 */
void EXTI0_IRQHandler(void)
{
}

void EXTI1_IRQHandler(void)
{
}

void EXTI2_IRQHandler(void)
{
}

void EXTI3_IRQHandler(void)
{
#if (defined(SX1276MB1MAS) | defined(SX1276MB1LAS) | defined(SX1272MB2DAS))
  HAL_EXTI_IRQHandler(&H_EXTI_3);
#endif
}

void EXTI4_IRQHandler(void)
{
#ifndef USE_EMOD_IMS62F
  HAL_EXTI_IRQHandler(&H_EXTI_4);
#endif
}

void EXTI9_5_IRQHandler(void)
{
#if (defined(SX1276MB1MAS) | defined(SX1276MB1LAS) | defined(SX1272MB2DAS))
  HAL_EXTI_IRQHandler(&H_EXTI_5);
#endif
}

void EXTI15_10_IRQHandler(void)
{
	if (hRADIO_DIO_exti[1].PendingCallback != NULL) {
		hRADIO_DIO_exti[1].PendingCallback();
	}
#if (defined(SX1276MB1MAS) | defined(SX1276MB1LAS) | defined(SX1272MB2DAS) | defined(USE_EMOD_IMS62F))
//	if (HAL_GPIO_ReadPin(RADIO_DIO_1_PORT, RADIO_DIO_1_PIN) == GPIO_PIN_SET) {
		HAL_EXTI_IRQHandler(&H_EXTI_10);
//	}
#endif

//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles USART2 Interrupt.
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
  * @brief This function handles RTC Alarms (A and B) Interrupt.
  */
void RTC_Alarm_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_Alarm_IRQn 0 */

  /* USER CODE END RTC_Alarm_IRQn 0 */
  HAL_RTC_AlarmIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_Alarm_IRQn 1 */

  /* USER CODE END RTC_Alarm_IRQn 1 */
}


/**
  * @brief This function handles RTC Wakeup Interrupt.
  */
void RTC_WKUP_IRQHandler( void )
{
  /* USER CODE BEGIN RTC_WKUP_IRQn 0 */
	if (TWiMODLRHCI.Process) {
		TWiMODLRHCI.Process();
	}

  /* USER CODE END RTC_WKUP_IRQn 0 */
	HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_WKUP_IRQn 1 */
	if (EmWimodData.ReceiveFrmUserRequest)
	{
		WiMODLoRaWAN.Process(&TWiMODLRHCI.Rx.Message);
		EmWimodData.ReceiveFrmUserRequest = 0x0;
		memset(&Rx2_buffer, 0x00, kMaxUARTPayloadSize);
//		UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
	}
  /* USER CODE END RTC_WKUP_IRQn 1 */	
}

void WWDG_IRQhandle ( void ) {
	HAL_WWDG_IRQHandler(&hwwdg);
	HAL_WWDG_Refresh(&hwwdg);
}
/* USER CODE END 1 */
