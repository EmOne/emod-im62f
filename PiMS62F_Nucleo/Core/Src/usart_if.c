/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart_if.c
  * @author  MCD Application Team
  * @brief   Configuration of UART driver interface for hyperterminal communication
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
#include "usart_if.h"

/* USER CODE BEGIN Includes */
#include "emod_uart.h"
#include "stm32_lpm.h"
#include "utilities_def.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/

extern EXTI_HandleTypeDef hRADIO_DIO_exti[];

/**
  * @brief DMA handle
  */
#if defined ( USE_EMOD_IMS62F )
extern DMA_HandleTypeDef hdma_usart1_tx;
#else
extern DMA_HandleTypeDef hdma_usart2_tx;
#endif
/**
  * @brief UART handle
  */
#if defined ( USE_EMOD_IMS62F )
extern UART_HandleTypeDef huart1;
#else
extern UART_HandleTypeDef huart2;
#endif

/**
  * @brief buffer to receive 1 character
  */
extern uint8_t charRx;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Trace driver callbacks handler
  */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  vcom_Init,
  vcom_DeInit,
  vcom_ReceiveInit,
  vcom_Trace_DMA,
  vcom_Resume
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
/**
  * @brief  TX complete callback
  * @return none
  */
static void (*TxCpltCallback)(void *);
/**
  * @brief  RX complete callback
  * @param  rxChar ptr of chars buffer sent by user
  * @param  size buffer size
  * @param  error errorcode
  * @return none
  */
static void (*RxCpltCallback)(uint8_t *rxChar, uint16_t size, uint8_t error);

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void vcom_ResumeCallback ( void );
/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *))
{
  /* USER CODE BEGIN vcom_Init_1 */

  /* USER CODE END vcom_Init_1 */
  TxCpltCallback = cb;
  MX_DMA_Init();
#if defined ( USE_EMOD_IMS62F )
  MX_USART1_UART_Init();
#else
  MX_USART2_UART_Init();
#endif
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN vcom_Init_2 */

  /* USER CODE END vcom_Init_2 */
}

UTIL_ADV_TRACE_Status_t vcom_DeInit(void)
{
  /* USER CODE BEGIN vcom_DeInit_1 */

  /* USER CODE END vcom_DeInit_1 */
#if defined ( USE_EMOD_IMS62F )
  /* ##-1- Reset peripherals ################################################## */
  __HAL_RCC_USART1_FORCE_RESET();
  __HAL_RCC_USART1_RELEASE_RESET();

  /* ##-2- MspDeInit ################################################## */
  HAL_UART_DeInit(&huart1);

  /* ##-3- Disable the NVIC for DMA ########################################### */
  /* USER CODE BEGIN 1 */
  HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);
#else
  /* ##-1- Reset peripherals ################################################## */
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    /* ##-2- MspDeInit ################################################## */
    HAL_UART_MspDeInit(&huart2);

    /* ##-3- Disable the NVIC for DMA ########################################### */
    /* USER CODE BEGIN 1 */
    HAL_NVIC_DisableIRQ(DMA1_Channel7_IRQn);
#endif
  /* USER CODE END 1 */
  /* USER CODE BEGIN vcom_DeInit_2 */
  vcom_Sleep();
  /* USER CODE END vcom_DeInit_2 */
  return UTIL_ADV_TRACE_OK;
}

void vcom_Trace(uint8_t *p_data, uint16_t size)
{
  /* USER CODE BEGIN vcom_Trace_1 */

  /* USER CODE END vcom_Trace_1 */
#if defined ( USE_EMOD_IMS62F )
  HAL_UART_Transmit(&huart1, p_data, size, 1000);
#else
  HAL_UART_Transmit(&huart2, p_data, size, 1000);
#endif
  /* USER CODE BEGIN vcom_Trace_2 */

  /* USER CODE END vcom_Trace_2 */
}

UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size)
{
  /* USER CODE BEGIN vcom_Trace_DMA_1 */

  /* USER CODE END vcom_Trace_DMA_1 */
#if defined ( USE_EMOD_IMS62F )
  HAL_UART_Transmit_DMA(&huart1, p_data, size);
#else
  HAL_UART_Transmit_DMA(&huart2, p_data, size);
#endif
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN vcom_Trace_DMA_2 */

  /* USER CODE END vcom_Trace_DMA_2 */
}

UTIL_ADV_TRACE_Status_t vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error))
{
  /* USER CODE BEGIN vcom_ReceiveInit_1 */

  /* USER CODE END vcom_ReceiveInit_1 */
  /*record call back*/
  RxCpltCallback = RxCb;

  /*Start LPUART receive on IT*/
#if defined ( USE_EMOD_IMS62F )
  HAL_UART_Receive_IT(&huart1, &charRx, 1);
#else
  HAL_UART_Receive_IT(&huart2, &charRx, 1);
#endif
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN vcom_ReceiveInit_2 */

  /* USER CODE END vcom_ReceiveInit_2 */
}

UTIL_ADV_TRACE_Status_t vcom_Resume ( void )
{
  /* USER CODE BEGIN vcom_Resume_1 */
    HAL_EXTI_RegisterCallback(&hRADIO_DIO_exti[1], HAL_EXTI_COMMON_CB_ID, NULL);
    HAL_EXTI_ClearPending(&hRADIO_DIO_exti[1], EXTI_TRIGGER_RISING_FALLING);
    HAL_EXTI_ClearConfigLine(&hRADIO_DIO_exti[1]);
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

	__HAL_RCC_USART1_CLK_ENABLE();
  /* USER CODE END vcom_Resume_1 */
#if defined ( USE_EMOD_IMS62F )
  /*to re-enable lost UART settings*/
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  /*to re-enable lost DMA settings*/
  if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
  {
    Error_Handler();
  }
#else
  /*to re-enable lost UART settings*/
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
      Error_Handler();
    }

    /*to re-enable lost DMA settings*/
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }
#endif
  /* USER CODE BEGIN vcom_Resume_2 */
    //	HAL_ResumeTick();

    UTIL_ADV_TRACE_StartRxProcess(&emod_UART_RxCpltCallback);
//    UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_RX_Id), UTIL_LPM_DISABLE);
//    UTIL_LPM_SetOffMode((1 << CFG_LPM_UART_RX_Id), UTIL_LPM_ENABLE);
  /* USER CODE END vcom_Resume_2 */
    return UTIL_ADV_TRACE_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/* USER CODE BEGIN HAL_UART_TxCpltCallback_1 */

	/* USER CODE END HAL_UART_TxCpltCallback_1 */
	/* buffer transmission complete*/
	if(TxCpltCallback != NULL)
		TxCpltCallback(huart);
	/* USER CODE BEGIN HAL_UART_TxCpltCallback_2 */

	/* USER CODE END HAL_UART_TxCpltCallback_2 */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* USER CODE BEGIN HAL_UART_RxCpltCallback_1 */

	/* USER CODE END HAL_UART_RxCpltCallback_1 */
	if ((NULL != RxCpltCallback) && (HAL_UART_ERROR_NONE == huart->ErrorCode)) {
		RxCpltCallback(&charRx, 1, 0);
	}
	HAL_UART_Receive_IT(huart, &charRx, 1);
  /* USER CODE BEGIN HAL_UART_RxCpltCallback_2 */
    UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
  /* USER CODE END HAL_UART_RxCpltCallback_2 */
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) {
		/* USER CODE BEGIN HAL_UART_ErrorCpltCallback_1 */
		emod_UART_ErrorCallback(huart);
		/* USER CODE END HAL_UART_ErrorCpltCallback_1 */
	} else if (huart->Instance == USART2) {

	}

}
/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

/* USER CODE BEGIN PrFD */
void vcom_ResumeCallback ( void )
{
	vcom_Resume ();
}

void vcom_Sleep ( void )
{
	GPIO_InitTypeDef gpioInitType;

	USARTx_RX_GPIO_CLK_ENABLE();
	gpioInitType.Mode = GPIO_MODE_IT_FALLING;
	gpioInitType.Pin = USARTx_RX_PIN;
	gpioInitType.Pull = GPIO_NOPULL;
	gpioInitType.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &gpioInitType);

  	EXTI_ConfigTypeDef hExtiConfig;
  	hExtiConfig.Line = EXTI_LINE_10; // PA10
  	hExtiConfig.Mode = EXTI_MODE_INTERRUPT;
  	hExtiConfig.Trigger = EXTI_TRIGGER_FALLING;
  	hExtiConfig.GPIOSel = EXTI_GPIOA;   // PA10
  	HAL_EXTI_SetConfigLine(&hRADIO_DIO_exti[1], &hExtiConfig);
  	HAL_EXTI_RegisterCallback(&hRADIO_DIO_exti[1], HAL_EXTI_COMMON_CB_ID, vcom_ResumeCallback);

  	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

//  	HAL_SuspendTick();
//  	UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_RX_Id), UTIL_LPM_ENABLE);
}

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
