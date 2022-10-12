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
#include "stm32_timer.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern UTIL_TIMER_Object_t WakeupTimer;
extern bool bIsWakeup;
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
EXTI_HandleTypeDef g_hExtiHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void PWR_WakeupCallback ( void );
/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void PWR_EnterOffMode(void)
{
  /* USER CODE BEGIN EnterOffMode_1 */

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

  Sx_Board_IoDeInit();

  HAL_ADC_MspDeInit(&hadc);

  /*clear wake up flag*/
  SET_BIT(PWR->CR, PWR_CR_CWUF);

  UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN EnterStopMode_2 */
  HAL_UART_DeInit(&huart1);

//	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
//	GPIO_InitStruct.Pin = RXD_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  	EXTI_ConfigTypeDef hExtiConfig;
  	hExtiConfig.Line = EXTI_LINE_10; // PA10
  	hExtiConfig.Mode = EXTI_MODE_INTERRUPT;
  	hExtiConfig.Trigger = EXTI_TRIGGER_RISING;
  	hExtiConfig.GPIOSel = EXTI_GPIOA;   // PA10
  	HAL_EXTI_SetConfigLine(&g_hExtiHandle, &hExtiConfig);
  	HAL_EXTI_RegisterCallback(&g_hExtiHandle, HAL_EXTI_COMMON_CB_ID, PWR_WakeupCallback);

  	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 4);
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  	HAL_SuspendTick();
  /* USER CODE END EnterStopMode_2 */
  /* Enter Stop Mode */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  /* USER CODE BEGIN EnterStopMode_3 */
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END EnterStopMode_3 */
}

void PWR_ExitStopMode(void)
{
  /* USER CODE BEGIN ExitStopMode_1 */

  /* USER CODE END ExitStopMode_1 */
  /* Disable IRQ while the MCU is not running on HSI */

  UTILS_ENTER_CRITICAL_SECTION();

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
  Sx_Board_IoInit();

  UTILS_EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN ExitStopMode_2 */

  /* USER CODE END ExitStopMode_2 */
}

void PWR_EnterSleepMode(void)
{
  /* USER CODE BEGIN EnterSleepMode_1 */

  /* USER CODE END EnterSleepMode_1 */
  /* USER CODE BEGIN EnterSleepMode_2 */

  /* USER CODE END EnterSleepMode_2 */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  /* USER CODE BEGIN EnterSleepMode_3 */

  /* USER CODE END EnterSleepMode_3 */
}

void PWR_ExitSleepMode(void)
{
  /* USER CODE BEGIN ExitSleepMode_1 */

  /* USER CODE END ExitSleepMode_1 */

  /* USER CODE BEGIN ExitSleepMode_2 */

  /* USER CODE END ExitSleepMode_2 */
}

/* USER CODE BEGIN EF */
static void PWR_WakeupCallback ( void )
{
	HAL_ResumeTick();
	SystemClock_Config();
	MX_USART1_UART_Init();

	/*Set verbose LEVEL*/
	UTIL_ADV_TRACE_StartRxProcess(&emod_UART_RxCpltCallback);

	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
#if defined (USE_EMOD_IMS62F)
	//  MX_TIM5_Init();
	MX_TIM9_Init();
#else
	    MX_TIM8_Init();
	#endif


	g_hExtiHandle.PendingCallback = NULL;
//	HAL_EXTI_RegisterCallback(&g_hExtiHandle., HAL_EXTI_COMMON_CB_ID, NULL);
	HAL_EXTI_ClearPending(&g_hExtiHandle, EXTI_TRIGGER_RISING);
//	HAL_EXTI_ClearConfigLine(&g_hExtiHandle);
//	bIsWakeup = true;
//	UTIL_TIMER_Start(&WakeupTimer);
}
/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
