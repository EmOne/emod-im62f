/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_app.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
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
#include "platform.h"
#include "Region.h" /* Needed for LORAWAN_DEFAULT_DATA_RATE */
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "sys_conf.h"
#include "CayenneLpp.h"
#include "sys_sensors.h"

/* USER CODE BEGIN Includes */
#include "WiMODLRHCI.h"
#include "WiMODLoRaWAN.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern WiMODLoRaWAN_t WiMODLoRaWAN;
extern TWiMODLORAWAN_RadioStackConfig radioStack;
static uint8_t rejoinCounter = 0;
/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief LoRa State Machine states
  */
typedef enum TxEventType_e
{
  /**
    * @brief Appdata Transmission issue based on timer every TxDutyCycleTime
    */
  TX_ON_TIMER,
  /**
    * @brief Appdata Transmission external event plugged on OnSendEvent( )
    */
  TX_ON_EVENT
  /* USER CODE BEGIN TxEventType_t */

  /* USER CODE END TxEventType_t */
} TxEventType_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  LoRa End Node request reset
 */
static void
Reset (void);
/**
  * @brief  LoRa End Node send request
  */
static void Rejoin(void);

/**
  * @brief  TX timer callback function
  * @param  context ptr of timer context
  */
//static void OnTxTimerEvent(void *context);
static void JoinRequest( void );

/**
  * @brief  join event callback function
  * @param  joinParams status of join
  */
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);

/**
  * @brief  tx event callback function
  * @param  params status of last Tx
  */
static void OnTxData(LmHandlerTxParams_t *params);

/**
  * @brief callback when LoRa application has received a frame
  * @param appData data received in the last Rx
  * @param params status of last Rx
  */
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/*!
 * Will be called each time a Radio IRQ is handled by the MAC layer
 *
 */
static void OnMacProcessNotify(void);

/* USER CODE BEGIN PFP */
void OnWakeupEvent(void *context);

/**
  * @brief  LED Tx timer callback function
  * @param  context ptr of LED context
  */
static void OnTxTimerLedEvent(void *context);

/**
  * @brief  LED Rx timer callback function
  * @param  context ptr of LED context
  */
static void OnRxTimerLedEvent(void *context);

/**
  * @brief  LED Join timer callback function
  * @param  context ptr of LED context
  */
static void OnJoinTimerLedEvent(void *context);

static void OnNetworkParamsChange( CommissioningParams_t *params );

static void OnCurrentClassChange( DeviceClass_t deviceClass );

static void OnSystemTimeChange( void );

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

/**
  * @brief LoRaWAN handler Callbacks
  */
static LmHandlerCallbacks_t LmHandlerCallbacks =
{
  .GetBatteryLevel =           GetBatteryLevel,
  .GetTemperature =            GetTemperatureLevel,
  .GetUniqueId =               GetUniqueId,
  .GetDevAddr =                GetDevAddr,
  .OnMacProcess =              OnMacProcessNotify,
  .OnJoinRequest =             OnJoinRequest,
  .OnTxData =                  OnTxData,
  .OnRxData =                  OnRxData,
  .OnNetworkParametersChange = OnNetworkParamsChange,
  .OnClassChange = 				OnCurrentClassChange,
  .OnSysTimeUpdate = 			OnSystemTimeChange
};

/**
  * @brief LoRaWAN handler parameters
  */
LmHandlerParams_t lmHParams =
{
  .ActiveRegion =             ACTIVE_REGION,
  .DefaultClass =             LORAWAN_DEFAULT_CLASS,
  .AdrEnable =                LORAWAN_ADR_STATE,
  .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
  .PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};

/**
  * @brief Type of Event to generate application Tx
  */
//static TxEventType_t EventType = TX_ON_EVENT;

LmHandlerMsgTypes_t MsgType = LORAWAN_DEFAULT_CONFIRMED_MSG_STATE;

/**
  * @brief Timer to handle the application Tx
  */
//static UTIL_TIMER_Object_t TxTimer;

/* USER CODE BEGIN PV */
/**
  * @brief User application buffer
  */
//static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/**
  * @brief User application data structure
  */
//static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

/**
  * @brief Specifies the state of the application LED
  */
//static uint8_t AppLedStateOn = RESET;

/**
  * @brief Timer to handle the wake up period
  */
UTIL_TIMER_Object_t WakeupTimer;
uint32_t bIsWakeup = 100000;
/**
  * @brief Timer to handle the application Tx Led to toggle
  */
static UTIL_TIMER_Object_t TxLedTimer;

/**
  * @brief Timer to handle the application Rx Led to toggle
  */
static UTIL_TIMER_Object_t RxLedTimer;

/**
  * @brief Timer to handle the application Join Led to toggle
  */
static UTIL_TIMER_Object_t JoinLedTimer;

/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */

/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_1 */
  if (__HAL_PWR_GET_FLAG (PWR_FLAG_SB) != 0)
    {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

      HAL_PWR_DisableWakeUpPin (PWR_WAKEUP_PIN1 | PWR_WAKEUP_PIN2);

      if (HAL_RTCEx_SetWakeUpTimer_IT (&hrtc, 0x1F4,
				       RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) //Wake up 0x1F4:500ms 16MHz interval
	{
	  Error_Handler ();
	}
    }

  LED_Init(LED_BLUE);
  LED_Init(LED_RED1);
  LED_Init(LED_RED2);

  /* Get LoRa APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION:        V%X.%X.%X\r\n",
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW LoraWAN info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

//  UTIL_TIMER_Create(&WakeupTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnWakeupEvent, NULL);
  UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
  UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
  UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
//  UTIL_TIMER_SetPeriod(&WakeupTimer, 10000);
  UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
  UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
  UTIL_TIMER_SetPeriod (&JoinLedTimer, 50000);

  /* USER CODE END LoRaWAN_Init_1 */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaRejoinEvent), UTIL_SEQ_RFU, Rejoin);
  UTIL_SEQ_RegTask ((1 << CFG_SEQ_Task_Reset), UTIL_SEQ_RFU, Reset);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Join), UTIL_SEQ_RFU, JoinRequest);

  /* Init Info table used by LmHandler*/
  LoraInfo_Init();

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks);

  LmHandlerConfigure (&lmHParams, false);

  /* USER CODE BEGIN LoRaWAN_Init_2 */
  WiMODLoRaWAN.beginAndAutoSetup ();

  WiMODLoRaWAN.SapLoRaWan->setRegion (lmHParams.ActiveRegion);

//  UTIL_TIMER_Start(&JoinLedTimer);

  /* USER CODE END LoRaWAN_Init_2 */
//  if (LmHandlerJoinStatus () == LORAMAC_HANDLER_SET)
//    {
//      JoinRequest ();
//    }


//  LmHandlerJoin(ActivationType);

//  if (EventType == TX_ON_TIMER)
//  {
//    /* send every time timer elapses */
//    UTIL_TIMER_Create(&TxTimer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
//    UTIL_TIMER_SetPeriod(&TxTimer,  APP_TX_DUTYCYCLE);
//    UTIL_TIMER_Start(&TxTimer);
//  }
//  else
//  {
//    /* USER CODE BEGIN LoRaWAN_Init_3 */
//
//    /* send every time button is pushed */
////    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
//    /* USER CODE END LoRaWAN_Init_3 */
//  }

  /* USER CODE BEGIN LoRaWAN_Init_Last */

  /* USER CODE END LoRaWAN_Init_Last */
}

/* USER CODE BEGIN PB_Callbacks */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  switch (GPIO_Pin)
//  {
////    case  USER_BUTTON_PIN:
////      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
////      break;
//    default:
//      break;
//  }
//}

/* USER CODE END PB_Callbacks */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
  if ((appData != NULL) || (params != NULL))
  {
    LED_On(LED_BLUE);

    UTIL_TIMER_Start(&RxLedTimer);
      UTIL_TIMER_Stop (&JoinLedTimer);

    static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

    APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Indication ==========\r\n");
    APP_LOG(TS_OFF, VLEVEL_H, "###### D/L FRAME:%04d | SLOT:%s | PORT:%d | DR:%d | RSSI:%d | SNR:%d\r\n",
            params->DownlinkCounter, slotStrings[params->RxSlot], appData->Port, params->Datarate, params->Rssi, params->Snr);

    TWiMODLR_HCIMessage *tx = &WiMODLoRaWAN.SapLoRaWan->HciParser->TxMessage;
    uint8_t offset = 0 ;
    tx->Payload[offset++] = DeviceInfo.Status = WiMODLR_RESULT_OK;

    if (params->IsMcpsIndication != 0) {
    	if (appData->BufferSize == 0) {
    		if (appData->Port == 0 && MsgType == LORAMAC_HANDLER_CONFIRMED_MSG) {
					//Channel idx
					tx->Payload[offset++] = params->Channel;
					//DR idx
					tx->Payload[offset++] = params->Datarate;
					//RSSi
					tx->Payload[offset++] = params->Rssi;
					//SNR
					tx->Payload[offset++] = params->Snr;
					//RX Slot
					tx->Payload[offset++] = params->RxSlot;

					WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
					LORAWAN_SAP_ID,
					LORAWAN_MSG_RECV_ACK_IND,
							&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], offset);
			} else {
				tx->Payload[0] = DeviceInfo.Status = WiMODLR_RESULT_NO_RESPONSE;
				tx->Payload[1] = params->Status; //Error code

				WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
				LORAWAN_SAP_ID,
				LORAWAN_MSG_RECV_NO_DATA_IND,
						&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1 + 1);

			}
	      UTIL_LPM_SetStopMode ((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
		} else {
			//LRW Port LEN: 1
			tx->Payload[offset++] = appData->Port;
			//LRW Payload LEN: 1
			tx->Payload[offset++] = appData->BufferSize;
			//LRW Payload data LEN: n
			memcpy(&tx->Payload[offset], appData->Buffer, appData->BufferSize);
			offset += appData->BufferSize;
			//Channel idx
			tx->Payload[offset++] = params->Channel;
			//DR idx
			tx->Payload[offset++] = params->Datarate;
			//RSSi
			tx->Payload[offset++] = params->Rssi;
			//SNR
			tx->Payload[offset++] = params->Snr;
			//RX Slot
			tx->Payload[offset++] = params->RxSlot;
			switch (MsgType) {
				case LORAMAC_HANDLER_UNCONFIRMED_MSG:
					WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
						LORAWAN_SAP_ID,
						LORAWAN_MSG_RECV_UDATA_IND,
						&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
						offset);
					break;
				case LORAMAC_HANDLER_CONFIRMED_MSG:
					WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
						LORAWAN_SAP_ID,
						LORAWAN_MSG_RECV_CDATA_IND,
						&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
						offset);
					break;
				default:
					break;
			}
			UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
	//			switch (appData->Port) {
	//			case LORAWAN_SWITCH_CLASS_PORT:
	//				/*this port switches the class*/
	//				if (appData->BufferSize == 1) {
	//					switch (appData->Buffer[0]) {
	//					case 0: {
	//						LmHandlerRequestClass(CLASS_A);
	//						break;
	//					}
	//					case 1: {
	//						LmHandlerRequestClass(CLASS_B);
	//						break;
	//					}
	//					case 2: {
	//						LmHandlerRequestClass(CLASS_C);
	//						break;
	//					}
	//					default:
	//						break;
	//					}
	//				}
	//				break;
	//			case LORAWAN_USER_APP_PORT:
	//				if (appData->BufferSize == 1) {
	//					AppLedStateOn = appData->Buffer[0] & 0x01;
	//					if (AppLedStateOn == RESET) {
	//						APP_LOG(TS_OFF, VLEVEL_H, "LED OFF\r\n");
	//
	//						LED_Off(LED_RED1);
	//					} else {
	//						APP_LOG(TS_OFF, VLEVEL_H, "LED ON\r\n");
	//
	//						LED_On(LED_RED1);
	//					}
	//				}
	//				break;
	//
	//			default:
	//
	//				break;
	//			}
			}
		} else {
			//TODO: WiMOD response
			if (params->LinkCheck == false) {
				//Devaddress
				LmHandlerGetDevAddr((uint32_t *) &tx->Payload[1]);
				//Channel idx
				tx->Payload[5] = params->Channel;
				//DR idx
	      tx->Payload[6] = params->Datarate;
				//RSSi
				tx->Payload[7] = params->Rssi;
				//SNR
				tx->Payload[8] = params->Snr;
				//RX Slot
				tx->Payload[9] = params->RxSlot;

				WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
						LORAWAN_SAP_ID,
						LORAWAN_MSG_JOIN_NETWORK_IND,
						&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
						1 + 9);
				LmHandlerSetTxDatarate(lmHParams.TxDatarate);
				rejoinCounter = 0;
	      MibRequestConfirm_t mibSet;
	      mibSet.Type = MIB_DEVICE_CLASS;
	      mibSet.Param.Class = CLASS_A;
	      LoRaMacMibSetRequestConfirm (&mibSet);
	      LoRaMacStart ();
				UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);

			} else {

			}

		}

	}
  /* USER CODE END OnRxData_1 */
 }

static void
Reset (void)
{
  NVIC_SystemReset ();
}

static void Rejoin(void)
{
//  /* USER CODE BEGIN SendTxData_1 */
//  uint16_t pressure = 0;
//  int16_t temperature = 0;
//  sensor_t sensor_data;
	UTIL_TIMER_Time_t nextTxIn = 0;
	LmHandlerAppData_t AppData;
	AppData.Port = 0;
	AppData.BufferSize = 0;
	AppData.Buffer = NULL;
//
//#ifdef CAYENNE_LPP
//  uint8_t channel = 0;
//#else
//  uint16_t humidity = 0;
//  uint32_t i = 0;
//  int32_t latitude = 0;
//  int32_t longitude = 0;
//  uint16_t altitudeGps = 0;
//#endif /* CAYENNE_LPP */
//
//  EnvSensors_Read(&sensor_data);
//#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
//  temperature = (int16_t) sensor_data.temperature;
//#else
//  temperature = (SYS_GetTemperatureLevel() >> 8);
//#endif  /* SENSOR_ENABLED */
//  pressure    = (uint16_t)(sensor_data.pressure * 100 / 10);      /* in hPa / 10 */
//
//  AppData.Port = LORAWAN_USER_APP_PORT;
//
//#ifdef CAYENNE_LPP
//  CayenneLppReset();
//  CayenneLppAddBarometricPressure(channel++, pressure);
//  CayenneLppAddTemperature(channel++, temperature);
//  CayenneLppAddRelativeHumidity(channel++, (uint16_t)(sensor_data.humidity));
//
//  if ((LmHandlerParams.ActiveRegion != LORAMAC_REGION_US915) && (LmHandlerParams.ActiveRegion != LORAMAC_REGION_AU915)
//      && (LmHandlerParams.ActiveRegion != LORAMAC_REGION_AS923))
//  {
//    CayenneLppAddDigitalInput(channel++, GetBatteryLevel());
//    CayenneLppAddDigitalOutput(channel++, AppLedStateOn);
//  }
//
//  CayenneLppCopy(AppData.Buffer);
//  AppData.BufferSize = CayenneLppGetSize();
//#else  /* not CAYENNE_LPP */
//  humidity    = (uint16_t)(sensor_data.humidity * 10);            /* in %*10     */
//
//  AppData.Buffer[i++] = AppLedStateOn;
//  AppData.Buffer[i++] = (uint8_t)((pressure >> 8) & 0xFF);
//  AppData.Buffer[i++] = (uint8_t)(pressure & 0xFF);
//  AppData.Buffer[i++] = (uint8_t)(temperature & 0xFF);
//  AppData.Buffer[i++] = (uint8_t)((humidity >> 8) & 0xFF);
//  AppData.Buffer[i++] = (uint8_t)(humidity & 0xFF);
//
//  if ((LmHandlerParams.ActiveRegion == LORAMAC_REGION_US915) || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AU915)
//      || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AS923))
//  {
//    AppData.Buffer[i++] = 0;
//    AppData.Buffer[i++] = 0;
//    AppData.Buffer[i++] = 0;
//    AppData.Buffer[i++] = 0;
//  }
//  else
//  {
//    latitude = sensor_data.latitude;
//    longitude = sensor_data.longitude;
//
//    AppData.Buffer[i++] = GetBatteryLevel();        /* 1 (very low) to 254 (fully charged) */
//    AppData.Buffer[i++] = (uint8_t)((latitude >> 16) & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)((latitude >> 8) & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)(latitude & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)((longitude >> 16) & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)((longitude >> 8) & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)(longitude & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)((altitudeGps >> 8) & 0xFF);
//    AppData.Buffer[i++] = (uint8_t)(altitudeGps & 0xFF);
//  }
//
//  AppData.BufferSize = i;
//#endif /* CAYENNE_LPP */

	rejoinCounter++;

  if (LORAMAC_HANDLER_SUCCESS == LmHandlerSend(&AppData, MsgType, &nextTxIn, false))
  {
    APP_LOG(TS_ON, VLEVEL_L, "SEND REQUEST\r\n");
  }
  else if (nextTxIn > 0)
  {
    APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
  }

  /* USER CODE END SendTxData_1 */
}

//static void OnTxTimerEvent(void *context)
//{
//  /* USER CODE BEGIN OnTxTimerEvent_1 */
//
//  /* USER CODE END OnTxTimerEvent_1 */
//  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
//
//  /*Wait for next tx slot*/
//  UTIL_TIMER_Start(&TxTimer);
//  /* USER CODE BEGIN OnTxTimerEvent_2 */
//
//  /* USER CODE END OnTxTimerEvent_2 */
//}

/* USER CODE BEGIN PrFD_LedEvents */

void OnWakeupEvent(void *context)
{
	//TODO: Go to low power mode
	bIsWakeup = false;
}

static void OnTxTimerLedEvent(void *context)
{
  LED_Off(LED_RED2);
}

static void OnRxTimerLedEvent(void *context)
{
  LED_Off(LED_BLUE) ;
}

static void OnJoinTimerLedEvent(void *context)
{
  LED_Toggle(LED_RED1) ;
  UTIL_TIMER_Stop (&JoinLedTimer);
  UTIL_SEQ_SetTask ((1 << CFG_SEQ_Task_LoRaRejoinEvent), CFG_SEQ_Prio_0);
}

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
  if ((params != NULL))
  {
	  TWiMODLR_HCIMessage *tx = &WiMODLoRaWAN.SapLoRaWan->HciParser->TxMessage;
	  tx->Payload[0] = DeviceInfo.Status;
		//Channel idx
		tx->Payload[1] = params->Channel;
		//DR idx
		tx->Payload[2] = params->Datarate;

    /* Process Tx event only if its a mcps response to prevent some internal events (mlme) */
    if (params->IsMcpsConfirm != 0)
    {
      LED_On(LED_RED2) ;
      UTIL_TIMER_Start(&TxLedTimer);

      APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Confirm =============\r\n");
      APP_LOG(TS_OFF, VLEVEL_H, "###### U/L FRAME:%04d | PORT:%d | DR:%d | PWR:%d", params->UplinkCounter,
              params->AppData.Port, params->Datarate, params->TxPower);

      APP_LOG(TS_OFF, VLEVEL_H, " | MSG TYPE:");
      if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG)
      {
        APP_LOG(TS_OFF, VLEVEL_H, "CONFIRMED [%s]\r\n", (params->AckReceived != 0) ? "ACK" : "NACK");

        //Number TX packet
   		tx->Payload[3] = params->UplinkCounter;
   		//TRX PowerLevel
		tx->Payload[4] = params->TxPower;
		//RF Airtime Len: 4
//		memcpy(&tx->Payload[5], Airtime, 4);

        WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
				LORAWAN_SAP_ID,
				LORAWAN_MSG_SEND_CDATA_TX_IND,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
				1 + 8);
	      UTIL_LPM_SetStopMode ((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_H, "UNCONFIRMED\r\n");
		WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
								LORAWAN_SAP_ID,
								LORAWAN_MSG_SEND_UDATA_TX_IND,
								&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
								1 + 2);
	      UTIL_LPM_SetStopMode ((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
      }

    } else {

    	//Number TX packet
	  tx->Payload[3] =
	      params->UplinkCounter == 0 ?
		  rejoinCounter : params->UplinkCounter;

		WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_JOIN_NETWORK_TX_IND,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 3);

		if (ActivationType == ACTIVATION_TYPE_ABP) {
			UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
		}
    }
  } else { // params is NULL

  }

  /* USER CODE END OnTxData_1 */
}

static void JoinRequest( void )
{
  rejoinCounter = 0;
//  UTIL_TIMER_Start (&JoinLedTimer);

  LmHandlerJoin (ActivationType);

}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
  if (joinParams != NULL)
  {
	  TWiMODLR_HCIMessage *tx = &WiMODLoRaWAN.SapLoRaWan->HciParser->TxMessage;
    if (joinParams->Status == LORAMAC_HANDLER_SUCCESS)
    {
      UTIL_TIMER_Stop(&JoinLedTimer);

      LED_Off(LED_RED1) ;

      APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOINED = ");
      if (joinParams->Mode == ACTIVATION_TYPE_ABP)
      {
        APP_LOG(TS_OFF, VLEVEL_M, "ABP ======================\r\n");
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_M, "OTAA =====================\r\n");
      }

	  MibRequestConfirm_t mibReq;
	  mibReq.Type = MIB_NVM_CTXS;
	  LoRaMacMibGetRequestConfirm (&mibReq);
	  LoRaMacNvmData_t *nvm = mibReq.Param.Contexts;

	  nvm->MacGroup1.ChannelsDatarate = nvm->userSettings.DataRateIndex;

//	  UTIL_LPM_SetStopMode ((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
	}
    else
    {
      APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### = JOIN FAILED\r\n");
		tx->Payload[0] = DeviceInfo.Status = WiMODLR_RESULT_NO_RESPONSE;
		tx->Payload[1] = joinParams->Status; //Error code

		WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
		LORAWAN_SAP_ID,
		LORAWAN_MSG_RECV_NO_DATA_IND,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1 + 1);

		if ( rejoinCounter < radioStack.Retransmissions)
		{
	      UTIL_TIMER_Start (&JoinLedTimer);
//			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaRejoinEvent), CFG_SEQ_Prio_0);
		}
		else
		{
				MibRequestConfirm_t mibSet;
				mibSet.Type = MIB_DEVICE_CLASS;
				mibSet.Param.Class = CLASS_A;
				LoRaMacMibSetRequestConfirm(&mibSet);
			UTIL_LPM_SetStopMode ((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
		}
    }

  }
  /* USER CODE END OnJoinRequest_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */

  /* USER CODE END OnMacProcessNotify_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);

  /* USER CODE BEGIN OnMacProcessNotify_2 */

  /* USER CODE END OnMacProcessNotify_2 */
}

static void OnNetworkParamsChange( CommissioningParams_t *params )
{

}

static void OnCurrentClassChange( DeviceClass_t deviceClass )
{

}

static void OnSystemTimeChange( void )
{

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
