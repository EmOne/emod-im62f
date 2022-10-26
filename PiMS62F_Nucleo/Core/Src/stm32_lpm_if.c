/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_lpm_if.c
  * @author  MCD Application Team
  * @brief   Low layer function to enter/exit low power modes (stop, sleep)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sys_debug.h"
#include "stm32_lpm_if.h"
#include "usart_if.h"
#include "radio_board_if.h"
#include "adc.h"

/* USER CODE BEGIN Includes */
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "rtc.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Power driver callbacks handler
  */
const struct UTIL_LPM_Driver_s UTIL_PowerDriver =
{
  PWR_EnterSleepMode,
  PWR_ExitSleepMode,

  PWR_EnterStopMode,
  PWR_ExitStopMode,

  PWR_EnterOffMode,
  PWR_ExitOffMode,
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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

/* Exported functions --------------------------------------------------------*/

void PWR_EnterOffMode(void)
{
  /* USER CODE BEGIN EnterOffMode_1 */
  UTILS_ENTER_CRITICAL_SECTION();

  Sx_Board_IoDeInit ();

  HAL_ADC_MspDeInit (&hadc);

  UTIL_ADV_TRACE_DeInit ();

  /*clear wake up flag*/
  SET_BIT(PWR->CR, PWR_CR_CWUF);

  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

  HAL_PWR_EnableWakeUpPin (PWR_WAKEUP_PIN1);

  UTILS_EXIT_CRITICAL_SECTION();

  HAL_PWR_EnterSTANDBYMode ();
  /* USER CODE END EnterOffMode_1 */
}

void PWR_ExitOffMode(void)
{
  /* USER CODE BEGIN ExitOffMode_1 */

  /* USER CODE END ExitOffMode_1 */
}

void PWR_EnterStopMode(void)
{
  /* USER CODE BEGIN EnterStopMode_1 */

  /* USER CODE END EnterStopMode_1 */
  UTILS_ENTER_CRITICAL_SECTION();

//  Gpio_PreInit();
  HAL_RTCEx_DeactivateWakeUpTimer (&hrtc);

  Sx_Board_IoDeInit();

  HAL_ADC_MspDeInit(&hadc);

  UTIL_ADV_TRACE_DeInit();

  /*clear wake up flag*/
  SET_BIT(PWR->CR, PWR_CR_CWUF);

  UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN EnterStopMode_2 */

  /* USER CODE END EnterStopMode_2 */
  /* Enter Stop Mode */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* USER CODE BEGIN EnterStopMode_3 */

  /* USER CODE END EnterStopMode_3 */
}

void PWR_ExitStopMode(void)
{
  /* USER CODE BEGIN ExitStopMode_1 */

  /* USER CODE END ExitStopMode_1 */
  /* Disable IRQ while the MCU is not running on HSI */

  UTILS_ENTER_CRITICAL_SECTION();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSI */
  __HAL_RCC_HSI_ENABLE();

  /* Wait till HSI is ready */
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET) {}

  /* Enable PLL */
  __HAL_RCC_PLL_ENABLE();
  /* Wait till PLL is ready */
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) {}

  /* Select PLL as system clock source */
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK) {}

  /* initializes the peripherals */
  UTIL_ADV_TRACE_Resume();

  HAL_ADC_MspInit(&hadc);

  Sx_Board_IoInit();

  Sx_Board_IoIrqInit(NULL);

  if (HAL_RTCEx_SetWakeUpTimer_IT (&hrtc, 0x1F4, RTC_WAKEUPCLOCK_RTCCLK_DIV16)
      != HAL_OK) //Wake up 0x1F4:500ms 16MHz interval
    {
      Error_Handler ();
    }

  UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN ExitStopMode_2 */

  /* USER CODE END ExitStopMode_2 */
}

void PWR_EnterSleepMode(void)
{
  /* USER CODE BEGIN EnterSleepMode_1 */
//	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	UTILS_ENTER_CRITICAL_SECTION();
  /* USER CODE END EnterSleepMode_1 */
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
//    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
//	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
//	RCC_OscInitStruct.MSIClockRange = RCC_ICSCR_MSIRANGE_4;
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
//		Error_Handler();
//	}
//
//	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
//	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
//		Error_Handler();
//	}
//	UTIL_ADV_TRACE_DeInit();
//	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
//
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
//		Error_Handler();
//		}

  /* USER CODE BEGIN EnterSleepMode_2 */
	UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE END EnterSleepMode_2 */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  /* USER CODE BEGIN EnterSleepMode_3 */

  /* USER CODE END EnterSleepMode_3 */
}

void PWR_ExitSleepMode(void)
{
  /* USER CODE BEGIN ExitSleepMode_1 */
	UTILS_ENTER_CRITICAL_SECTION();
  /* USER CODE END ExitSleepMode_1 */
//	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//	UTIL_ADV_TRACE_Resume();
//	SystemClock_Config();
//
//	HAL_ADC_MspInit(&hadc);
//
//	Sx_Board_IoInit();
//
//	Sx_Board_IoIrqInit(NULL);

  /* USER CODE BEGIN ExitSleepMode_2 */
	UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE END ExitSleepMode_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
