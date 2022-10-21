//------------------------------------------------------------------------------
//! @file WiMOD_SAP_LORAWAN.cpp
//! @ingroup WiMOD_SAP_LORAWAN
//! <!------------------------------------------------------------------------->
//! @brief Implementation of the commands of the LoRaWAN SericeAccessPoint
//! @version 0.1
//! <!------------------------------------------------------------------------->
//!
//!
//!
//! <!--------------------------------------------------------------------------
//! Copyright (c) 2016
//! IMST GmbH
//! Carl-Friedrich Gauss Str. 2-4
//! 47475 Kamp-Lintfort
//! --------------------------------------------------------------------------->
//! @author (FB), IMST
//! <!--------------------------------------------------------------------------
//! Target OS:    none
//! Target CPU:   tbd
//! Compiler:     tbd
//! --------------------------------------------------------------------------->
//! @internal
//! @par Revision History:
//! <PRE>
//!-----------------------------------------------------------------------------
//! Version | Date       | Author | Comment
//!-----------------------------------------------------------------------------
//!
//! </PRE>
//------------------------------------------------------------------------------


/*
 * THIS IS AN EXAMPLE IMPLEMENTATION ACCORDING THE THE HCI SPEC: V1.26
 * FOR FIRMWARE: LoRaWAN_EndNode_Modem
 *
 * SEE FILE: WiMOD_LoRaWAN_EndNode_Modem_HCI_Spec_V1_26.pdf
 */


//------------------------------------------------------------------------------
//
// Section Includes Files
//
//------------------------------------------------------------------------------

#include "WiMOD_SAP_LORAWAN.h"
#include <string.h>

//------------------------------------------------------------------------------
//
// Section public functions
//
//------------------------------------------------------------------------------
static void SetRegion(TLoRaWANregion regionalSetting);
static TWiMODLRResultCodes activateDevice(TWiMODLORAWAN_ActivateDeviceData* activationData, UINT8* statusRsp);
static TWiMODLRResultCodes reactivateDevice(UINT32* devAdr, UINT8* statusRsp);
static TWiMODLRResultCodes setJoinParameter(TWiMODLORAWAN_JoinParams* joinParams, UINT8* statusRsp);
static TWiMODLRResultCodes joinNetwork(UINT8* statusRsp);
static void registerJoinTxIndicationClient(TJoinTxIndicationCallback cb);
static bool Convert2Tx(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_TxIndData* sendIndData);
static bool Convert2Rx(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_Data* loraWanRxData);
static bool Convert2MAC(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_MacCmdData* loraWanMacCmdData);
static bool Convert2Nwk(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_JoinedNwkData* joinedNwkData);
static bool Convert2Ack(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_ACK_Data* ackData);
static bool Convert2Unknown(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_NoData_Data*  info);
static void registerNoDataIndicationClient(TNoDataIndicationCallback cb);
static void registerTxCDataIndicationClient(TTxCDataIndicationCallback cb);
static void registerTxUDataIndicationClient(TTxUDataIndicationCallback cb);
static void registerRxUDataIndicationClient(TRxUDataIndicationCallback cb);
static void registerRxCDataIndicationClient(TRxCDataIndicationCallback cb);
static void registerRxMacCmdIndicationClient(TRxMacCmdIndicationCallback cb);
static void registerJoinedNwkIndicationClient(TJoinedNwkIndicationCallback cb);
static void registerRxAckIndicationClient(TRxAckIndicationCallback cb);
static TWiMODLRResultCodes setRadioStackConfig(TWiMODLORAWAN_RadioStackConfig* data, UINT8* statusRsp);
static TWiMODLRResultCodes getRadioStackConfig(TWiMODLORAWAN_RadioStackConfig* data, UINT8* statusRsp);
static TWiMODLRResultCodes deactivateDevice(UINT8* statusRsp);
static TWiMODLRResultCodes factoryReset(UINT8* statusRsp);
static TWiMODLRResultCodes setDeviceEUI(const UINT8* deviceEUI, UINT8* statusRsp);
static TWiMODLRResultCodes getDeviceEUI (UINT8* deviceEUI, UINT8* statusRsp);
static TWiMODLRResultCodes getNwkStatus(TWiMODLORAWAN_NwkStatus_Data* nwkStatus, UINT8* statusRsp);
static TWiMODLRResultCodes sendMacCmd(const TWiMODLORAWAN_MacCmd* cmd, UINT8* statusRsp);
static TWiMODLRResultCodes setCustomConfig(const INT8 rfGain, UINT8* statusRsp);
static TWiMODLRResultCodes getCustomConfig(INT8* rfGain, UINT8* statusRsp);
static TWiMODLRResultCodes getSupportedBands(TWiMODLORAWAN_SupportedBands* supportedBands, UINT8* statusRsp);
static TWiMODLRResultCodes getTxPowerLimitConfig(TWiMODLORAWAN_TxPwrLimitConfig* txPwrLimitCfg, UINT8* statusRsp);
static TWiMODLRResultCodes setTxPowerLimitConfig(TWiMODLORAWAN_TxPwrLimitConfig* txPwrLimitCfg, UINT8* statusRsp);
static TWiMODLRResultCodes getLinkAdrReqConfig(TWiMODLORAWAN_LinkAdrReqConfig* linkAdrReqCfg, UINT8* statusRsp);
static TWiMODLRResultCodes setLinkAdrReqConfig(TWiMODLORAWAN_LinkAdrReqConfig* linkAdrReqCfg, UINT8* statusRsp);
static TWiMODLRResultCodes setBatteryLevelStatus(UINT8 battStatus, UINT8* statusRsp);
static void dispatchLoRaWANMessage(TWiMODLR_HCIMessage* rxMsg);
static void process (UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg);
static TWiMODLRResultCodes sendUData(const TWiMODLORAWAN_TX_Data* data, UINT8* statusRsp);
static TWiMODLRResultCodes sendCData(const TWiMODLORAWAN_TX_Data* data, UINT8* statusRsp);
//-----------------------------------------------------------------------------

/**
 * @brief Constructor
 *
 * @param hci       Pointer to HCI processor object
 *
 * @param buffer    pointer to storage area for building tx frames; MUST BE VALID
 *
 * @param bufferSize    size of the buffer
 *
 */
WiMOD_SAP_LoRaWAN_t WiMOD_SAP_LoRaWAN = {

		SetRegion,
		activateDevice,
		reactivateDevice,
		setJoinParameter,
		joinNetwork,
		registerJoinTxIndicationClient,
		Convert2Rx,
		Convert2Tx,
		Convert2MAC,
		Convert2Nwk,
		Convert2Ack,
		Convert2Unknown,
		registerNoDataIndicationClient,
		registerTxCDataIndicationClient,
		registerTxUDataIndicationClient,
		registerRxUDataIndicationClient,
		registerRxCDataIndicationClient,
		registerRxMacCmdIndicationClient,
		registerJoinedNwkIndicationClient,
		registerRxAckIndicationClient,
		sendUData,
		sendCData,
		setRadioStackConfig,
		getRadioStackConfig,
		deactivateDevice,
		factoryReset,
		setDeviceEUI,
		getDeviceEUI,
		getNwkStatus,
		sendMacCmd,
		setCustomConfig,
		getCustomConfig,
		getSupportedBands,
		getTxPowerLimitConfig,
		setTxPowerLimitConfig,
		getLinkAdrReqConfig,
		setLinkAdrReqConfig,
		setBatteryLevelStatus,
		dispatchLoRaWANMessage,
		process,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		0x00,
		NULL,
		LoRaWAN_Region_AS923, // default init
};

/*******************************************************************************/

//process declare


UINT8 devEUI[8];
TWiMODLORAWAN_MacCmd cmd;
INT8 rfGain;

TWiMODLORAWAN_ActivateDeviceData activateData;
TWiMODLORAWAN_JoinParams joinData;
TWiMODLORAWAN_NwkStatus_Data networkStatus;
TWiMODLORAWAN_SupportedBands supBands;
TWiMODLORAWAN_TxPwrLimitConfig txPwrLimit;
TWiMODLORAWAN_LinkAdrReqConfig linkAdr = LinkAdrCfg_Option_LoRaWAN_V1_0_2;
TWiMODLORAWAN_RadioStackConfig radioStack = {
		.BandIndex = 0x0A,
		.DataRateIndex = 2,
		.TXPowerLevel = 0x10,
		.Retransmissions = 7,
		.PowerSavingMode = 0,
		.HeaderMacCmdCapacity = 15,
		.Options = 0b11011000
};

LmHandlerAppData_t appData;
extern uint32_t REGION_AS923_FREQ_OFFSET;
extern LmHandlerMsgTypes_t MsgType;
extern LmHandlerParams_t lmHParams;
/******************************************************************************/

//-----------------------------------------------------------------------------
/**
 * @brief Destructor
 */

//WiMOD_SAP_LoRaWAN::~WiMOD_SAP_LoRaWAN(void) {
//
//}

//-----------------------------------------------------------------------------
/**
 * @brief Setup regional settings for the LoRaWAN Firmware of the WiMOD module
 *
 *
 * @param regionalSetting region code for the firmware
 *
 */

void SetRegion(TLoRaWANregion regionalSetting)
{
	WiMOD_SAP_LoRaWAN.region = regionalSetting;
}

//-----------------------------------------------------------------------------
/**
 * @brief Activates the device via the ABP procedure
 *
 *
 * @see TWiMODLRResultCodes for interpretation of the values
 *
 * @param activationData   structure holding the necessary parameters
 *
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes activateDevice(TWiMODLORAWAN_ActivateDeviceData* activationData, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8               offset = 0;

    if ( statusRsp &&
            (WiMOD_SAP_LoRaWAN.txPayloadSize >= (4+ WiMODLORAWAN_NWK_SESSION_KEY_LEN + WiMODLORAWAN_APP_SESSION_KEY_LEN)))
    {
//        HTON32(&WiMOD_SAP_LoRaWAN.txPayload[offset], activationData->DeviceAddress);
//        offset += 0x04;
//        memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], activationData->NwkSKey, WiMODLORAWAN_NWK_SESSION_KEY_LEN);
//        offset += WiMODLORAWAN_NWK_SESSION_KEY_LEN;
//        memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], activationData->AppSKey, WiMODLORAWAN_APP_SESSION_KEY_LEN);
//        offset += WiMODLORAWAN_APP_SESSION_KEY_LEN;

#if 0

        //WiMODLoRaWAN.SapLoRaWan->HciParser = &TWiMODLRHCI;
        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
                                           LORAWAN_MSG_ACTIVATE_DEVICE_REQ,
                                           LORAWAN_MSG_ACTIVATE_DEVICE_RSP,
										   WiMOD_SAP_LoRaWAN.txPayload, offset);
        // copy response status
        if (result == WiMODLR_RESULT_OK) {
            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
        	//*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
        }
#else

        LmHandlerStop();

        memcpy(activateData.AppSKey, activationData->AppSKey, 16);
        memcpy(activateData.NwkSKey, activationData->NwkSKey, 16);
        memcpy((uint8_t *)&activateData.DeviceAddress, (uint8_t *)&activationData->DeviceAddress, 4);

        LmHandlerSetAppSKey((uint8_t *) &activateData.AppSKey );
        LmHandlerSetNwkSKey((uint8_t *) &activateData.NwkSKey );
        LmHandlerSetDevAddr( activateData.DeviceAddress );

    	ActivationType = ACTIVATION_TYPE_ABP;

        //copy response status
        result = WiMODLR_RESULT_OK;
#endif
    } else
    {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_ACTIVATE_DEVICE_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Join), CFG_SEQ_Prio_0);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Re-Activates the device via the ABP procedure
 *
 *
 * @see TWiMODLRResultCodes for interpretation of the values
 *
 * @param devAdr    pointer where to store the "received" device address
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes reactivateDevice(UINT32* devAdr, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;

    if ( statusRsp && devAdr)
    {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_REACTIVATE_DEVICE_REQ,
//                                           LORAWAN_MSG_REACTIVATE_DEVICE_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);

//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();

    	//TODO: Reactivate in LoRa stack
//        *devAdr = NTOH32(&rx->Payload[WiMODLR_HCI_RSP_CMD_PAYLOAD_POS]);
//		LoRaWAN_Init();

//    	LmHandlerSetDevAddr( activateData.DeviceAddress );
            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }  else {
//    	   result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
//       }
    	LmHandlerStop();

    	devAdr = &activateData.DeviceAddress;

        result = WiMODLR_RESULT_OK;
    } else {
    	*devAdr = 0xFFFFFFFF;
 	   	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	
	//32-Bit Device Address(LSB first)
	memcpy(&tx->Payload[1], devAdr, sizeof(UINT32));
	
	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_REACTIVATE_DEVICE_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 4);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Join), CFG_SEQ_Prio_0);

	return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Sets the parameters used for the OTAA activation procedure
 *
 *
 * @see TWiMODLRResultCodes for interpretation of the values
 *
 * @param joinParams   structure holding the necessary parameters
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setJoinParameter(TWiMODLORAWAN_JoinParams* joinParams, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;

    if ( statusRsp && (WiMOD_SAP_LoRaWAN.txPayloadSize >= WiMODLORAWAN_APP_EUI_LEN + WiMODLORAWAN_APP_KEY_LEN)) {


//        memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], joinParams->AppEUI, WiMODLORAWAN_APP_EUI_LEN);
//        offset += WiMODLORAWAN_APP_EUI_LEN;
//        memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], joinParams->AppKey, WiMODLORAWAN_APP_KEY_LEN);
//        offset += WiMODLORAWAN_APP_KEY_LEN;
//
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SET_JOIN_PARAM_REQ,
//                                           LORAWAN_MSG_SET_JOIN_PARAM_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);
//        // copy response status
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

    	LmHandlerStop();

        memcpy(&joinData.AppEUI, joinParams->AppEUI,
				WiMODLORAWAN_APP_EUI_LEN);

		memcpy(&joinData.AppKey, joinParams->AppKey,
				WiMODLORAWAN_APP_KEY_LEN);

		LmHandlerSetAppEUI(joinParams->AppEUI);
		LmHandlerSetAppKey(joinParams->AppKey);
		LmHandlerSetNwkKey(joinParams->AppKey);

		LmHandlerSetTxDatarate(lmHParams.TxDatarate);

		result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
    
    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
    tx->Payload[0] = result; //DeviceInfo.Status;

    *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
    	LORAWAN_SAP_ID,	
    	LORAWAN_MSG_SET_JOIN_PARAM_RSP, 
    	&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 
    	1);
    
    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Start joining the network via the OTAA procedure. Asynchronous process
 *
 * This functions starts an asynchronous process of joining to a network.
 * It will take a time until a (final) result is ready. Therefore the
 * callback interface (e.g. RegisterJoinedNwkIndicationClient) should be used.
 *
 * @see TWiMODLRResultCodes for interpretation of the values
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes joinNetwork(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if ( statusRsp) {

        //TODO: set join parameter


    
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_JOIN_NETWORK_REQ,
//                                           LORAWAN_MSG_JOIN_NETWORK_RSP,
//                                           NULL, 0);
    
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

    	ActivationType = ACTIVATION_TYPE_OTAA;

        result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
	LORAWAN_SAP_ID, LORAWAN_MSG_JOIN_NETWORK_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Join), CFG_SEQ_Prio_0);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Tries to send transmit U-Data to network server via RF link
 *
 *
 * @param data       pointer to data structure containing the TX-data and options.
 *                   @see TWiMODLORAWAN_TX_Data for details
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes sendUData(const TWiMODLORAWAN_TX_Data *data,
		UINT8 *statusRsp) {
	TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//	UINT8 offset = 0;
	UINT32 u32CreditTime;
	LmHandlerAppData_t appData;

	if (data && (data->Length > 0)) {

//        tmpSize = MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), data->Length);
//        if (WiMOD_SAP_LoRaWAN.txPayloadSize >= tmpSize) {
//        	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->Port;
//            memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], data->Payload, MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), tmpSize));
//            offset += tmpSize;
//            result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                               LORAWAN_MSG_SEND_UDATA_REQ,
//                                               LORAWAN_MSG_SEND_UDATA_RSP,
//											   WiMOD_SAP_LoRaWAN.txPayload, offset);
//            // copy response status
//            if (result == WiMODLR_RESULT_OK) {
//                *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//            }
//        } else {
//            result = WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR;
//        }

		//TODO: (LSB first) Register send unreliable data to LORA message queue and expected air-time calculation
		u32CreditTime = 0xFFFFFFFF;

		appData.BufferSize = MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), data->Length);
		appData.Port = data->Port;
		appData.Buffer = (uint8_t *)&data->Payload;
		MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG;
		LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, &u32CreditTime, false);

//		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

		result = WiMODLR_RESULT_OK;

	} else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result;//DeviceInfo.Status;
	memcpy(&tx->Payload[1], &u32CreditTime, sizeof(UINT32));

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_SEND_UDATA_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 4);

	return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Tries to send transmit C-Data to network server via RF link
 *
 *
 * @param data       pointer to data structure containing the TX-data and options.
 *                   @see TWiMODLORAWAN_TX_Data for details
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes sendCData(const TWiMODLORAWAN_TX_Data* data, UINT8* statusRsp)
{
	TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//	UINT8 offset = 0;
	UINT32 u32CreditTime = 0;

    if ( data && (data->Length > 0) && statusRsp) {

//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->Port;
//        memcpy(&WiMOD_SAP_LoRaWAN.txPayload[offset], data->Payload, MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), data->Length));
//        offset += MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), data->Length);
//
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SEND_CDATA_REQ,
//                                           LORAWAN_MSG_SEND_CDATA_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);
//        // copy response status
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
//    } else {
//        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
//    }

		//TODO: Register send unreliable data to LORA message queue and expected air-time calculation
		u32CreditTime = 0xFFFFFFFF;

		appData.BufferSize = MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), data->Length);
		appData.Port = data->Port;
		appData.Buffer = (uint8_t *) &data->Payload;
		MsgType = LORAMAC_HANDLER_CONFIRMED_MSG;
		LmHandlerSend(&appData, LORAMAC_HANDLER_CONFIRMED_MSG, &u32CreditTime, false);

//		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

		result = WiMODLR_RESULT_OK;
	} else {
		//TODO: Something
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	memcpy(&tx->Payload[1], &u32CreditTime, sizeof(UINT32));

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_SEND_CDATA_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 4);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "TX Join Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */

void registerJoinTxIndicationClient(TJoinTxIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.JoinTxIndCallback =  cb;
}


//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level tx ind structure
 *
 * This function should be used by the Tx Data Indication Callback functions
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   sendIndData Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */
bool Convert2Tx(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_TxIndData* sendIndData)
{
    UINT8 offset = 0;


    if (sendIndData) {
        sendIndData->FieldAvailability = LORAWAN_OPT_TX_IND_INFOS_NOT_AVAILABLE;
    }

    if (sendIndData && RxMsg->Length >= 1) {

        sendIndData->StatusFormat = RxMsg->Payload[offset++];

        if (sendIndData->StatusFormat & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {

            // check if extra UDATA indication data is available (without TxPktCounter)
            if  (RxMsg->Length > (uint16_t)(offset+2) ){
                sendIndData->FieldAvailability = LORAWAN_OPT_TX_IND_INFOS_EXCL_PKT_CNT;
                sendIndData->ChannelIndex =  RxMsg->Payload[offset++];
                sendIndData->DataRateIndex = RxMsg->Payload[offset++];

                // check if extra TxPktCounter is available
                if (RxMsg->Length > offset) {
                    sendIndData->FieldAvailability = LORAWAN_OPT_TX_IND_INFOS_INCL_PKT_CNT;
                    sendIndData->NumTxPackets = RxMsg->Payload[offset++];
                }

                sendIndData->PowerLevel = 0;
				// check if extra PowerLevel is available
                if (RxMsg->Length > offset) {
                	sendIndData->PowerLevel = RxMsg->Payload[offset++];
                }

				// check if extra AirTimne is available
                sendIndData->RfMsgAirtime = 0;
                if (RxMsg->Length > offset) {
                	sendIndData->RfMsgAirtime = NTOH32(&RxMsg->Payload[offset]);
                	offset += 4;
                }

            }
        }
        return true;
    }
    return false;
}



//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level Rx Data structure
 *
 * This function should be used by the Rx Data Indication Callback functions
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   loraWanRxData Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */
bool Convert2Rx(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_Data* loraWanRxData)
{
    UINT8 offset = 0;

    INT16 dataLen = RxMsg->Length;
    INT16 i;

    if (loraWanRxData) {
        loraWanRxData->Length = 0;
        loraWanRxData->OptionalInfoAvaiable = false;
    }

    if (loraWanRxData && RxMsg->Length >= 1) {

        loraWanRxData->StatusFormat = RxMsg->Payload[offset++];

        if (loraWanRxData->StatusFormat & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {
            dataLen -= (0x01 + 0x05); // format + rx ch info
            loraWanRxData->OptionalInfoAvaiable = true;
        } else {
            dataLen -= 0x01; // format; only
            loraWanRxData->OptionalInfoAvaiable = false;
        }

        // LoRaWAN port ID
        if (dataLen > 0) {
            loraWanRxData->Port = RxMsg->Payload[offset++];
        }

        // adjust dataLen for the real user defined payload
        dataLen--;

        // copy payload field
        // do not use memcpy here because of potential negative dataLen
        for (i = 0; i < dataLen; i++) {
            loraWanRxData->Payload[i] = RxMsg->Payload[offset++];
            loraWanRxData->Length++;
        }

        // check if optional attributes are present
        if (offset < RxMsg->Length) {
            loraWanRxData->ChannelIndex  = (UINT8) RxMsg->Payload[offset++];
            loraWanRxData->DataRateIndex = (UINT8) RxMsg->Payload[offset++];
            loraWanRxData->RSSI          = (INT8)  RxMsg->Payload[offset++];
            loraWanRxData->SNR           = (INT8)  RxMsg->Payload[offset++];
            loraWanRxData->RxSlot        = (UINT8) RxMsg->Payload[offset++];
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level MAC-Cmd structure
 *
 * This function should be used by the Rx Mac Cmd Indication Callback function
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   loraWanMacCmdData Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */
bool Convert2MAC(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_MacCmdData* loraWanMacCmdData)
{
    UINT8 offset = 0;

    UINT8 format;
    INT16 dataLen = RxMsg->Length;
    INT16 i;

    if (loraWanMacCmdData) {
        loraWanMacCmdData->Length = 0;
        loraWanMacCmdData->OptionalInfoAvaiable = false;
    }

    if (loraWanMacCmdData && RxMsg->Length >= 1) {

        format = RxMsg->Payload[offset++];

        if (format & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {
            dataLen -= (0x01 + 0x05); // format + rx ch info
            loraWanMacCmdData->OptionalInfoAvaiable = true;
        } else {
            dataLen -= 0x01; // format; only
            loraWanMacCmdData->OptionalInfoAvaiable = false;
        }

        // copy MAC cmd data field
        // do not use memcpy here because of potential negative dataLen
        for (i = 0; i < dataLen; i++) {
            loraWanMacCmdData->MacCmdData[i] = RxMsg->Payload[offset++];
            loraWanMacCmdData->Length++;
        }

        // check if optional attributes are present
        if (offset < RxMsg->Length) {
            loraWanMacCmdData->ChannelIndex  = (UINT8) RxMsg->Payload[offset++];
            loraWanMacCmdData->DataRateIndex = (UINT8) RxMsg->Payload[offset++];
            loraWanMacCmdData->RSSI          = (INT8)  RxMsg->Payload[offset++];
            loraWanMacCmdData->SNR           = (INT8)  RxMsg->Payload[offset++];
            loraWanMacCmdData->RxSlot        = (UINT8) RxMsg->Payload[offset++];
        }
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level NwkJoined structure
 *
 * This function should be used by the JoinedNwk Indication Callback function
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   joinedNwkData Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */
bool Convert2Nwk(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_JoinedNwkData* joinedNwkData)
{
    UINT8 offset = 0;

    INT16 dataLen = RxMsg->Length;

    if (joinedNwkData) {
        joinedNwkData->OptionalInfoAvaiable = false;
    }

    if (joinedNwkData && RxMsg->Length >= 1) {

        joinedNwkData->StatusFormat = RxMsg->Payload[offset++];

        if (joinedNwkData->StatusFormat & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {
            dataLen -= (0x01 + 0x05); // format + rx ch info
            joinedNwkData->OptionalInfoAvaiable = true;
        } else {
            dataLen -= 0x01; // format; only
            joinedNwkData->OptionalInfoAvaiable = false;
        }

        // get new device address retrieved from NWK server
        joinedNwkData->DeviceAddress = NTOH32(&RxMsg->Payload[offset]);
        offset += 0x04;


        // check if optional attributes are present
        if (offset < RxMsg->Length) {
            joinedNwkData->ChannelIndex  = (UINT8) RxMsg->Payload[offset++];
            joinedNwkData->DataRateIndex = (UINT8) RxMsg->Payload[offset++];
            joinedNwkData->RSSI          = (INT8)  RxMsg->Payload[offset++];
            joinedNwkData->SNR           = (INT8)  RxMsg->Payload[offset++];
            joinedNwkData->RxSlot        = (UINT8) RxMsg->Payload[offset++];
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level AckData structure
 *
 * This function should be used by the Rx Ack Indication Callback function
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   ackData     Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */
bool Convert2Ack(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_RX_ACK_Data* ackData)
{
    UINT8 offset = 0;

    INT16 dataLen = RxMsg->Length;

    if (ackData) {
        ackData->OptionalInfoAvaiable = false;
    }

    if (ackData && RxMsg->Length >= 1) {

        ackData->StatusFormat = RxMsg->Payload[offset++];

        if (ackData->StatusFormat & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {
            dataLen -= (0x01 + 0x05); // format + rx ch info
            ackData->OptionalInfoAvaiable = true;
        } else {
            dataLen -= 0x01; // format; only
            ackData->OptionalInfoAvaiable = false;
        }

        // check if optional attributes are present
        if (offset < RxMsg->Length) {
            ackData->ChannelIndex  = (UINT8) RxMsg->Payload[offset++];
            ackData->DataRateIndex = (UINT8) RxMsg->Payload[offset++];
            ackData->RSSI          = (INT8)  RxMsg->Payload[offset++];
            ackData->SNR           = (INT8)  RxMsg->Payload[offset++];
            ackData->RxSlot        = (UINT8) RxMsg->Payload[offset++];
        }
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
/**
 * @brief Convert a received low level HCI-Msg to a high-level NoDataIndication structure
 *
 * This function should be used by the NoData Indication Callback function
 * prior processing the received data message.
 *
 * @param   RxMsg       Reference to low-level HCI message.
 *                      @warning DO NOT MANIPULATE THESE VALUES !!!
 *
 * @param   info        Pointer to the buffer where to store the received data
 *
 * @retval true     if the conversion was successful
 */

bool Convert2Unknown(TWiMODLR_HCIMessage* RxMsg, TWiMODLORAWAN_NoData_Data*  info)
{
    UINT8 offset = 0;

    if (info) {
    	info->ErrorCode = 0;
    }

    if (info && RxMsg->Length >= 1) {

    	info->StatusFormat = RxMsg->Payload[offset++];

        if (info->StatusFormat & LORAWAN_FORMAT_EXT_HCI_OUT_ACTIVE) {
        	info->OptionalInfoAvaiable = true;
        } else {
        	info->OptionalInfoAvaiable = false;
        }

        // check if optional attributes are present
        if (offset < RxMsg->Length) {
        	info->ErrorCode  = (UINT8) RxMsg->Payload[offset++];
        }
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "TX Join Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerNoDataIndicationClient(TNoDataIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.NoDataIndCallback =  cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "TX C-Data Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerTxCDataIndicationClient(TTxCDataIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.TxCDataIndCallback =  cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "TX U Data Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerTxUDataIndicationClient(TTxUDataIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.TxUDataIndCallback = cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "RX U-Data Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerRxUDataIndicationClient(TRxUDataIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.RxUDataIndCallback =  cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "RX C-Data Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerRxCDataIndicationClient(TRxCDataIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.RxCDataIndCallback =  cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "RX MAC Cmd Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerRxMacCmdIndicationClient(TRxMacCmdIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.RxMacCmdIndCallback =  cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "Joined Nwk Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerJoinedNwkIndicationClient(TJoinedNwkIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.JoinedNwkIndCallback = cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for the event "RX ACK (data) Indication"
 *
 * This registered callback is called when the specified event is called by
 * the WiMOD stack.
 *
 * @param   cb          pointer to a callback function that should be called
 *                      if the event occurs.
 */
void registerRxAckIndicationClient(TRxAckIndicationCallback cb)
{
	WiMOD_SAP_LoRaWAN.RxAckIndCallback = cb;
}


//-----------------------------------------------------------------------------
/**
 * @brief Sets a new radio config parameter set of the WiMOD
 *
 *
 * @param data       pointer to data structure containing the new parameters
 *                   @see TWiMODLORAWAN_TX_Data for details
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setRadioStackConfig(TWiMODLORAWAN_RadioStackConfig* data, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    GetPhyParams_t getPhy;
	PhyParam_t phyParam;
//    UINT8              offset = 0;

    if ( data && statusRsp) {

//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->DataRateIndex;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->TXPowerLevel;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->Options;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->PowerSavingMode;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->Retransmissions;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->BandIndex;
//
//        if (WiMOD_SAP_LoRaWAN.region == LoRaWAN_Region_US915) {
//        	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->SubBandMask1;
//        	WiMOD_SAP_LoRaWAN.txPayload[offset++] = data->SubBandMask2;
//        }

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SET_RSTACK_CONFIG_REQ,
//                                           LORAWAN_MSG_SET_RSTACK_CONFIG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);

        // clear RX field
        data->WrongParamErrCode = 0x00;

//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            offset = WiMODLR_HCI_RSP_STATUS_POS + 1;
//            if (rx->Length > 1) {
//                data->WrongParamErrCode = rx->Payload[offset++];
//            }
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
//    } else {
//        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
//    }    return result;


		switch (data->BandIndex) {
		case 0x0B: //AS923-2
			REGION_AS923_FREQ_OFFSET = ((~(0xFFFFB9B0) + 1) * 100);
			break;
		case 0x0C: //AS923-3
			REGION_AS923_FREQ_OFFSET = ((~(0xFFFEFE30) + 1) * 100);
			break;
		case 0x0A: //AS923-1
			REGION_AS923_FREQ_OFFSET = 0;
			break;
		default:
			data->WrongParamErrCode |= (0x1 << 5);
			break;
		}

		if (data->WrongParamErrCode != 0)
		{
			result = LoRaWAN_Status_Wrong_parameter;
		}
		else
		{
			if (radioStack.BandIndex != data->BandIndex)
				LmHandlerSetActiveRegion(lmHParams.ActiveRegion);
			else
				radioStack.BandIndex = data->BandIndex;

			radioStack.Options = data->Options;
			lmHParams.AdrEnable =
					(radioStack.Options & 0x1) ?
							LORAMAC_HANDLER_ADR_ON : LORAMAC_HANDLER_ADR_OFF;
			lmHParams.DutyCycleEnabled =
					(radioStack.Options & 0x2) ?
							LORAMAC_HANDLER_SET : LORAMAC_HANDLER_RESET;
			lmHParams.DefaultClass =
					(radioStack.Options & 0x4) ? CLASS_C : CLASS_A;

			LmHandlerSetAdrEnable(lmHParams.AdrEnable);
			LmHandlerSetDutyCycleEnable(lmHParams.DutyCycleEnabled );
			LmHandlerRequestClass(lmHParams.DefaultClass);

			getPhy.Attribute = PHY_DEF_TX_DR;
			phyParam = RegionGetPhyParam(WiMOD_SAP_LoRaWAN.region, &getPhy);
			radioStack.DataRateIndex =
					data->DataRateIndex >= phyParam.Value ?
							data->DataRateIndex : phyParam.Value;
			lmHParams.TxDatarate = (int) radioStack.DataRateIndex;
			LmHandlerSetTxDatarate(lmHParams.TxDatarate);

			getPhy.Attribute = PHY_DEF_MAX_EIRP;
			phyParam = RegionGetPhyParam( WiMOD_SAP_LoRaWAN.region, &getPhy );
			data->TXPowerLevel = data->TXPowerLevel > (UINT8)phyParam.fValue ? (UINT8)phyParam.fValue : data->TXPowerLevel;
			radioStack.TXPowerLevel = ((UINT8)phyParam.fValue - data->TXPowerLevel) / 2;
			LmHandlerSetTxPower(radioStack.TXPowerLevel);

			radioStack.PowerSavingMode = data->PowerSavingMode;
			radioStack.Retransmissions = data->Retransmissions;

			if (WiMOD_SAP_LoRaWAN.region == LoRaWAN_Region_US915) {
				radioStack.SubBandMask1 = data->SubBandMask1;
				radioStack.SubBandMask2 = data->SubBandMask2;
			}

			result = LoRaWAN_Status_Ok;
		}
    } else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	tx->Payload[1] = data->WrongParamErrCode;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_SET_RSTACK_CONFIG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 1);

	return result;
}



//-----------------------------------------------------------------------------
/**
 * @brief Gets the current radio config parameter set of the WiMOD
 *
 *
 * @param data       pointer to data structure for storing the requested information
 *                   @see TWiMODLORAWAN_TX_Data for details
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getRadioStackConfig(TWiMODLORAWAN_RadioStackConfig* data, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    GetPhyParams_t getPhy;
	PhyParam_t phyParam;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( data && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_RSTACK_CONFIG_REQ,
//                                           LORAWAN_MSG_GET_RSTACK_CONFIG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
    		LmHandlerGetTxDatarate((int8_t *)&radioStack.DataRateIndex);
            data->DataRateIndex   		= radioStack.DataRateIndex; //rx->Payload[offset++];

            getPhy.Attribute = PHY_DEF_MAX_EIRP;
            phyParam = RegionGetPhyParam( WiMOD_SAP_LoRaWAN.region, &getPhy );

            LmHandlerGetTxPower((int8_t *)&radioStack.TXPowerLevel);
            data->TXPowerLevel    		= (UINT8) phyParam.fValue - (radioStack.TXPowerLevel * 2); //rx->Payload[offset++];
            bool state = true;
            if(LmHandlerGetAdrEnable(&state) == LORAMAC_HANDLER_SUCCESS )
            	radioStack.Options |=  state << 0;
            else
            	radioStack.Options |=  0x1 << 0;

			if (LmHandlerGetDutyCycleEnable(&state) == LORAMAC_HANDLER_SUCCESS)
				radioStack.Options |= state << 1;
			else
				radioStack.Options |= 0x1 << 1;

			DeviceClass_t currentClass = CLASS_A;
			if (LmHandlerGetCurrentClass(&currentClass) == LORAMAC_HANDLER_SUCCESS)
				if(currentClass == CLASS_C) { radioStack.Options |= (0x1 << 2); }
				else {radioStack.Options &= ~(0x1 << 2);}
			else
				radioStack.Options &= ~(0x1 << 2);

            data->Options         		= radioStack.Options; //rx->Payload[offset++];
            data->PowerSavingMode 		= radioStack.PowerSavingMode; //rx->Payload[offset++];
            data->Retransmissions 		= radioStack.Retransmissions; //rx->Payload[offset++];
            data->BandIndex       		= radioStack.BandIndex; //rx->Payload[offset++];
            data->HeaderMacCmdCapacity 	= radioStack.HeaderMacCmdCapacity; //rx->Payload[offset++];
//
            if (WiMOD_SAP_LoRaWAN.region == LoRaWAN_Region_US915) {
//				if (rx->Length > offset) {
					data->SubBandMask1 = radioStack.SubBandMask1; //rx->Payload[offset++];
//				}
//				if (rx->Length > offset) {
					data->SubBandMask2 = radioStack.SubBandMask2; //rx->Payload[offset++];
//				}
            }
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
    	//TODO: Get radio stack properties

    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result;
	tx->Payload[1] = radioStack.DataRateIndex;
	tx->Payload[2] = radioStack.TXPowerLevel;
	tx->Payload[3] = radioStack.Options;
	tx->Payload[4] = radioStack.PowerSavingMode;
	tx->Payload[5] = radioStack.Retransmissions;
	tx->Payload[6] = radioStack.BandIndex;
	tx->Payload[7] = radioStack.HeaderMacCmdCapacity;
	tx->Payload[8] = radioStack.SubBandMask1;
	tx->Payload[9] = radioStack.SubBandMask2;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_GET_RSTACK_CONFIG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			10);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Deactivate the device (logical disconnect from lora network)
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes deactivateDevice(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if ( statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_DEACTIVATE_DEVICE_REQ,
//                                           LORAWAN_MSG_DEACTIVATE_DEVICE_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//        // copy response status
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
    	LmHandlerStop();
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_DEACTIVATE_DEVICE_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Reset all internal settings to default values (incl. DevEUI)
 *
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes factoryReset(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if ( statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_FACTORY_RESET_REQ,
//                                           LORAWAN_MSG_FACTORY_RESET_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//        // copy response status
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
    	NvmDataMgmtFactoryReset();
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_FACTORY_RESET_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Sets a new DeviceEUI (aka. IEEE-Address) to the WiMOD
 *
 * Note: Setting a new DeviceEUI is requires to switch to customer operation
 *       mode. In "normal" application mode, this command is locked.
 *
 * @param deviceEUI  pointer to data structure containing the new parameters
 *                   (Must be an pointer of a 64bit address)
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setDeviceEUI(const UINT8* deviceEUI, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if ( deviceEUI && statusRsp) {

    	// copy 64 bit DeviceEUI information into tx buffer
//        memcpy(WiMOD_SAP_LoRaWAN.txPayload, deviceEUI, WiMODLORAWAN_DEV_EUI_LEN);
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SET_DEVICE_EUI_REQ,
//                                           LORAWAN_MSG_SET_DEVICE_EUI_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, WiMODLORAWAN_DEV_EUI_LEN);
//
//        // copy response status
//        if (result == WiMODLR_RESULT_OK) {
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
    
    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
    tx->Payload[0] = result; //DeviceInfo.Status;

    *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
		LORAWAN_SAP_ID,	
		LORAWAN_MSG_SET_DEVICE_EUI_RSP, 
		&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 
		1);
    
    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Gets the  DeviceEUI (aka. IEEE-Address) of the WiMOD
 *
 *
 * @param deviceEUI  pointer for storing the received 64bit address
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getDeviceEUI (UINT8* deviceEUI, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( deviceEUI && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_DEVICE_EUI_REQ,
//                                           LORAWAN_MSG_GET_DEVICE_EUI_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // just copy the 8 EUI bytes as octet stream
//            memcpy(deviceEUI, &rx->Payload[offset], WiMODLORAWAN_DEV_EUI_LEN);
//            offset += WiMODLORAWAN_DEV_EUI_LEN;
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
    	LmHandlerGetDevEUI(deviceEUI);
    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	memcpy(&tx->Payload[1], deviceEUI, 8);

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_GET_DEVICE_EUI_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			9);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Gets the  current status of the network "connection"
 *
 *
 * @param nwkStatus  pointer for storing the requested information
 *                   @see LORAWAN_NWK_STATUS defines; e.g.:
 *                     - LORAWAN_NWK_STATUS_INACTIVE,
 *                     - LORAWAN_NWK_STATUS_ACTIVE_ABP,
 *                     - LORAWAN_NWK_STATUS_ACTIVE_OTAA,
 *                     - LORAWAN_NWK_STATUS_JOINING_OTAA
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

// pre V1.14 implementation
//TWiMODLRResultCodes WiMOD_SAP_LoRaWAN::GetNwkStatus(UINT8* nwkStatus,
//        UINT8* statusRsp)
//{
//    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;
//
//    if ( nwkStatus && statusRsp) {
//        result = HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_NWK_STATUS_REQ,
//                                           LORAWAN_MSG_GET_NWK_STATUS_RSP,
//                                           txPayload, 0x00);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage& rx = HciParser->GetRxMessage();
//
//            *nwkStatus = rx.Payload[offset++];
//
//            // copy response status
//            *statusRsp = rx.Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
//    } else {
//        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
//    }
//    return result;
//}

// new implemenation for V1.14
TWiMODLRResultCodes getNwkStatus(TWiMODLORAWAN_NwkStatus_Data* nwkStatus, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( nwkStatus && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_NWK_STATUS_REQ,
//                                           LORAWAN_MSG_GET_NWK_STATUS_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            nwkStatus->NetworkStatus = rx->Payload[offset++];					// @see TLoRaWAN_NwkStatus for meaning
//
//            // set dummy values
//            nwkStatus->DeviceAddress  = 0;
//            nwkStatus->DataRateIndex  = 0;
//            nwkStatus->PowerLevel     = 0;
//            nwkStatus->MaxPayloadSize = 0;
//            // check if optional fields are present
//            if (rx->Length > offset) {
//            	nwkStatus->DeviceAddress = NTOH32(&rx->Payload[offset]);
//            	offset += 0x04;
//            	nwkStatus->DataRateIndex = rx->Payload[offset++];
//            	nwkStatus->PowerLevel    = rx->Payload[offset++];
//            	nwkStatus->MaxPayloadSize= rx->Payload[offset++];
//            }
//
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
    	LoRaMacTxInfo_t txinfo;
    	LmHandlerGetDevAddr(&nwkStatus->DeviceAddress);
    	LmHandlerGetTxDatarate((int8_t *)&nwkStatus->DataRateIndex);
    	LmHandlerGetTxPower((int8_t *)&nwkStatus->PowerLevel);
    	LoRaMacQueryTxPossible( appData.BufferSize, &txinfo);
    	networkStatus.DeviceAddress = nwkStatus->DeviceAddress;
    	networkStatus.DataRateIndex = nwkStatus->DataRateIndex;
    	networkStatus.PowerLevel = 16 - (nwkStatus->PowerLevel * 2);
    	LmHandlerGetMaxPayloadReq(nwkStatus->DataRateIndex, &nwkStatus->MaxPayloadSize);
    	networkStatus.MaxPayloadSize = nwkStatus->MaxPayloadSize;

    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	tx->Payload[1] = (LmHandlerJoinStatus() == LORAMAC_HANDLER_SET) ? ActivationType :
			(ActivationType == ACTIVATION_TYPE_OTAA) ? 0x03 : ACTIVATION_TYPE_NONE;
	memcpy(&tx->Payload[2], &nwkStatus->DeviceAddress, 4);
	tx->Payload[6] = nwkStatus->DataRateIndex;
	tx->Payload[7] = nwkStatus->PowerLevel;
	tx->Payload[8] = nwkStatus->MaxPayloadSize;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_GET_NWK_STATUS_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			2 + 7);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Send a MAC command to the server; expert level only
 *
 * @param cmd      pointer containing the MAC command and parameters
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes sendMacCmd(const TWiMODLORAWAN_MacCmd* cmd, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;
//    UINT8              i      = 0;
	UINT32 u32CreditTime;
    if ( cmd && statusRsp && (cmd->Length <= WiMODLORAWAN_MAC_CMD_PAYLOAD_LENGTH)) {

    	
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = cmd->DataServiceType;
//    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = cmd->MacCmdID;
//        // copy payload / parameters of MAC command
//        while (i < cmd->Length) {
//        	WiMOD_SAP_LoRaWAN.txPayload[offset++] = cmd->Payload[i];
//            i++;
//        }
//
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SEND_MAC_CMD_REQ,
//                                           LORAWAN_MSG_SEND_MAC_CMD_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);
//
//        if (result == WiMODLR_RESULT_OK) {
//            // copy response status
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
    	appData.BufferSize = MIN((WiMOD_LORAWAN_TX_PAYLOAD_SIZE-1), 1);
		appData.Port = 0;
		appData.Buffer = (uint8_t *)&cmd->MacCmdID;

		LmHandlerSend(&appData, cmd->DataServiceType, &u32CreditTime, false);

    	result = WiMODLR_RESULT_OK;
    } else {
        if (cmd == NULL || statusRsp == NULL) {
            result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
        } else {
            result = WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR;
        }
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_SEND_MAC_CMD_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

	//TODO: Prepare MAC command to buffer
	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Setup a custom config for tx power settings; expert level only
 *
 * @param rfGain  new rfGain value for tx power settings
 *
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setCustomConfig(const INT8 rfGain, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = 0;

    if ( statusRsp) {
    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = (UINT8) rfGain;

//        if (result == WiMODLR_RESULT_OK) {
//            // copy response status
//            *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
    
    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
    tx->Payload[0] = result;// DeviceInfo.Status;

    *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
    	LORAWAN_SAP_ID,
    	LORAWAN_MSG_SET_CUSTOM_CFG_RSP, 
    	&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 
    	1);
    
    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Get the current offet for tx power level; expert level only
 *
 * @param rfGain  pointer to store the rfGain info  for tx power settings
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getCustomConfig(INT8* rfGain, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( rfGain && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_CUSTOM_CFG_REQ,
//                                           LORAWAN_MSG_GET_CUSTOM_CFG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            *rfGain = (INT8) rx->Payload[offset++];
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
        result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	tx->Payload[1] = 0;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_GET_CUSTOM_CFG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			2);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Get the supported bands of this firmware
 *
 * @param supportedBands  pointer  to store area for result
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes getSupportedBands(TWiMODLORAWAN_SupportedBands* supportedBands, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( supportedBands && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_GET_SUPPORTED_BANDS_REQ,
//										   LORAWAN_MSG_GET_SUPPORTED_BANDS_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//        supportedBands->NumOfEntries = 0;
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//
//            // get the band infos
//            while (offset < rx->Length  && (supportedBands->NumOfEntries < (WiMODLORAWAN_APP_PAYLOAD_LEN / 2) ) ) {
//            	supportedBands->BandIndex[supportedBands->NumOfEntries] = rx->Payload[offset++];
//            	supportedBands->MaxEIRP[supportedBands->NumOfEntries] = rx->Payload[offset++];
//            	supportedBands->NumOfEntries++;
//            }
//
//       }
    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_GET_SUPPORTED_BANDS_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + 4);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Get the configured TxPowerLimit settings
 *
 * @param txPwrLimitCfg  pointer to store area for result
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes getTxPowerLimitConfig(TWiMODLORAWAN_TxPwrLimitConfig* txPwrLimitCfg, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if (WiMOD_SAP_LoRaWAN.region != LoRaWAN_Region_EU868) {
    	return WiMODLR_RESULT_NO_RESPONSE;
    }

    if ( txPwrLimitCfg && statusRsp) {
//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//        								   LORAWAN_MSG_GET_TXPOWER_LIMIT_CONFIG_REQ,
//										   LORAWAN_MSG_GET_TXPOWER_LIMIT_CONFIG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);

        txPwrLimitCfg->NumOfEntries = 0;

//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//
//            // get the band infos
//            while (offset < rx->Length && (txPwrLimitCfg->NumOfEntries < (WiMODLORAWAN_APP_PAYLOAD_LEN / 3) )) {
//            	txPwrLimitCfg->SubBandIndex[txPwrLimitCfg->NumOfEntries] = rx->Payload[offset++];
//            	txPwrLimitCfg->TxPwrLimitFlag[txPwrLimitCfg->NumOfEntries] = rx->Payload[offset++];
//            	txPwrLimitCfg->TxPwrLimitValue[txPwrLimitCfg->NumOfEntries] = rx->Payload[offset++];
//            	txPwrLimitCfg->NumOfEntries++;
//            }
//
//       }
       //TODO: Get the band info

        result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
		LORAWAN_SAP_ID,
		LORAWAN_MSG_GET_TXPOWER_LIMIT_CONFIG_RSP,
		&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
		1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Set a SINGLE TxPowerLimit entry
 *
 * @param txPwrLimitCfg  reference that holds the data to write
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes setTxPowerLimitConfig(TWiMODLORAWAN_TxPwrLimitConfig* txPwrLimitCfg, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = 0;

    if (WiMOD_SAP_LoRaWAN.region != LoRaWAN_Region_EU868) {
    	return WiMODLR_RESULT_NO_RESPONSE;
    }

    if (statusRsp) {
    	/*
    	 * note: exactly one data set !!!!
    	 *
    	 */
        txPwrLimitCfg->NumOfEntries = 1;
        WiMOD_SAP_LoRaWAN.txPayload[offset++] = (UINT8) txPwrLimitCfg->SubBandIndex[0];
        WiMOD_SAP_LoRaWAN.txPayload[offset++] = (UINT8) txPwrLimitCfg->TxPwrLimitFlag[0];
        WiMOD_SAP_LoRaWAN.txPayload[offset++] = (UINT8) txPwrLimitCfg->TxPwrLimitValue[0];


//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//        								   LORAWAN_MSG_SET_TXPOWER_LIMIT_CONFIG_REQ,
//										   LORAWAN_MSG_SET_TXPOWER_LIMIT_CONFIG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);
//
//
//        offset = WiMODLR_HCI_RSP_STATUS_POS + 1;
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//
//            // try to get an optional error code for wrong parameters
//            txPwrLimitCfg->WrongParamErrCode = 0x00;
//            if (rx->Length >= offset) {
//            	txPwrLimitCfg->WrongParamErrCode =rx->Payload[offset++];
//            }
//       }
        result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
   		LORAWAN_SAP_ID,
		LORAWAN_MSG_SET_TXPOWER_LIMIT_CONFIG_RSP,
   		&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
   		1);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Get the configured LinkAdrReq config setting
 *
 * @param linkAdrReqCfg  pointer to store area for result
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes getLinkAdrReqConfig(TWiMODLORAWAN_LinkAdrReqConfig* linkAdrReqCfg, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS + 1;

    if ( linkAdrReqCfg && statusRsp) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//        								   LORAWAN_MSG_GET_LINKADRREQ_CONFIG_REQ,
//										   LORAWAN_MSG_GET_LINKADRREQ_CONFIG_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, 0x00);
//
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//
//            // get the configured option
//            *linkAdrReqCfg = (TWiMODLORAWAN_LinkAdrReqConfig) rx->Payload[offset++];
//
//       }
    	//TODO: Link ADR Config
    	*linkAdrReqCfg = linkAdr; //Dummy

    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;
	tx->Payload[1] = linkAdr; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
        	 LORAWAN_SAP_ID,
			 LORAWAN_MSG_GET_LINKADRREQ_CONFIG_RSP,
			 &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			 1 + 1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Set LinkAdrReq config setting
 *
 * @param linkAdrReqCfg  reference to the data to set
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes setLinkAdrReqConfig(TWiMODLORAWAN_LinkAdrReqConfig* linkAdrReqCfg, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;

    if (statusRsp && linkAdrReqCfg) {

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//											LORAWAN_MSG_SET_LINKADRREQ_CONFIG_REQ,
//											LORAWAN_MSG_SET_LINKADRREQ_CONFIG_RSP,
//											WiMOD_SAP_LoRaWAN.txPayload, offset);
//
//        offset = WiMODLR_HCI_RSP_STATUS_POS + 1;
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//       }
    	linkAdr = *linkAdrReqCfg;

    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
    
    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
    tx->Payload[0] = result; //DeviceInfo.Status;

    *statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
    	LORAWAN_SAP_ID,	
    	LORAWAN_MSG_SET_LINKADRREQ_CONFIG_RSP, 
    	&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
    	1);
    
    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Set Battery Level Status
 *
 * @param battStatus  status value; see documentation for details.
 *
 * @param statusRsp Status byte contained in the local response of the module
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes setBatteryLevelStatus(UINT8 battStatus, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = 0;

    if (statusRsp) {
    	WiMOD_SAP_LoRaWAN.txPayload[offset++] = (UINT8) battStatus;

//        result = WiMOD_SAP_LoRaWAN.HciParser->SendHCIMessage(LORAWAN_SAP_ID,
//                                           LORAWAN_MSG_SET_BATTERY_LEVEL_REQ,
//                                           LORAWAN_MSG_SET_BATTERY_LEVEL_RSP,
//										   WiMOD_SAP_LoRaWAN.txPayload, offset);
//
//
//        offset = WiMODLR_HCI_RSP_STATUS_POS + 1;
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_LoRaWAN.HciParser->GetRxMessage();
//
//            // copy response status
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//
//       }

        result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_LoRaWAN.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_LoRaWAN.HciParser->PostMessage(
			LORAWAN_SAP_ID,
			LORAWAN_MSG_SET_BATTERY_LEVEL_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}
/**

 * @brief process messages from the User
 *
 * @param rxMsg reference to the complete received HCI message; DO NOT MODIFIY it!

 */
void process (UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg)
{
	 switch (rxMsg->MsgID)
	    {
	        case LORAWAN_MSG_ACTIVATE_DEVICE_REQ:
	        	WiMOD_SAP_LoRaWAN.ActivateDevice((TWiMODLORAWAN_ActivateDeviceData*)&rxMsg->Payload[0], statusRsp);
	            break;
	        case LORAWAN_MSG_REACTIVATE_DEVICE_REQ:
	        	WiMOD_SAP_LoRaWAN.ReactivateDevice((UINT32*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_JOIN_PARAM_REQ:
	        	WiMOD_SAP_LoRaWAN.SetJoinParameter((TWiMODLORAWAN_JoinParams*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_JOIN_NETWORK_REQ:
	        	WiMOD_SAP_LoRaWAN.JoinNetwork(statusRsp);
	        	break;
	        case LORAWAN_MSG_SEND_UDATA_REQ:
	        	WiMOD_SAP_LoRaWAN.SendUData((TWiMODLORAWAN_TX_Data*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_SEND_CDATA_REQ:
	        	WiMOD_SAP_LoRaWAN.SendCData((TWiMODLORAWAN_TX_Data*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_RSTACK_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.SetRadioStackConfig((TWiMODLORAWAN_RadioStackConfig*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_RSTACK_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.GetRadioStackConfig((TWiMODLORAWAN_RadioStackConfig*)&radioStack, statusRsp);
	        	break;
	        case LORAWAN_MSG_DEACTIVATE_DEVICE_REQ:
	        	WiMOD_SAP_LoRaWAN.DeactivateDevice(statusRsp);
	        	break;
	        case LORAWAN_MSG_FACTORY_RESET_REQ:
	        	WiMOD_SAP_LoRaWAN.FactoryReset(statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_DEVICE_EUI_REQ:
	        	WiMOD_SAP_LoRaWAN.SetDeviceEUI(&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_DEVICE_EUI_REQ:
	        	WiMOD_SAP_LoRaWAN.GetDeviceEUI((UINT8*)&devEUI, statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_NWK_STATUS_REQ:
	        	WiMOD_SAP_LoRaWAN.GetNwkStatus(&networkStatus, statusRsp);
	        	break;
	        case LORAWAN_MSG_SEND_MAC_CMD_REQ:
	        	WiMOD_SAP_LoRaWAN.SendMacCmd((TWiMODLORAWAN_MacCmd*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_CUSTOM_CFG_REQ:
	        	WiMOD_SAP_LoRaWAN.SetCustomConfig((INT8)rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_CUSTOM_CFG_REQ:
	        	WiMOD_SAP_LoRaWAN.GetCustomConfig(&rfGain, statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_SUPPORTED_BANDS_REQ:
	        	WiMOD_SAP_LoRaWAN.GetSupportedBands(&supBands, statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_TXPOWER_LIMIT_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.GetTxPowerLimitConfig(&txPwrLimit, statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_TXPOWER_LIMIT_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.SetTxPowerLimitConfig((TWiMODLORAWAN_TxPwrLimitConfig*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        case LORAWAN_MSG_GET_LINKADRREQ_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.GetLinkAdrReqConfig(&linkAdr, statusRsp);
	        	break;
	        case LORAWAN_MSG_SET_LINKADRREQ_CONFIG_REQ:
	        	WiMOD_SAP_LoRaWAN.SetLinkAdrReqConfig((TWiMODLORAWAN_LinkAdrReqConfig*)&rxMsg->Payload[0], statusRsp);
	        	break;
	        default:
	            break;
	    }
	    return;
}
//------------------------------------------------------------------------------
//
// Section protected functions
//
//------------------------------------------------------------------------------

/**
 * @internal
 *
 * @brief Dispatch messages from the WiMOD (aka indications)
 *
 * @param rxMsg reference to the complete received HCI message; DO NOT MODIFIY it!
 *
 * @endinternal
 */
void dispatchLoRaWANMessage(TWiMODLR_HCIMessage* rxMsg)
{
    switch (rxMsg->MsgID)
    {
        case LORAWAN_MSG_JOIN_NETWORK_TX_IND:
            if (WiMOD_SAP_LoRaWAN.JoinTxIndCallback) {
            	WiMOD_SAP_LoRaWAN.JoinTxIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_RECV_NO_DATA_IND:
            if (WiMOD_SAP_LoRaWAN.NoDataIndCallback) {
            	WiMOD_SAP_LoRaWAN.NoDataIndCallback();
            }
            break;
        case LORAWAN_MSG_SEND_CDATA_TX_IND:
            if (WiMOD_SAP_LoRaWAN.TxCDataIndCallback) {
            	WiMOD_SAP_LoRaWAN.TxCDataIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_SEND_UDATA_TX_IND:
            if (WiMOD_SAP_LoRaWAN.TxUDataIndCallback) {
            	WiMOD_SAP_LoRaWAN.TxUDataIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_RECV_UDATA_IND:
            if (WiMOD_SAP_LoRaWAN.RxUDataIndCallback) {
            	WiMOD_SAP_LoRaWAN.RxUDataIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_RECV_CDATA_IND:
            if (WiMOD_SAP_LoRaWAN.RxCDataIndCallback) {
            	WiMOD_SAP_LoRaWAN.RxCDataIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_RECV_MAC_CMD_IND:
            if (WiMOD_SAP_LoRaWAN.RxMacCmdIndCallback) {
            	WiMOD_SAP_LoRaWAN.RxMacCmdIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_JOIN_NETWORK_IND:
            if (WiMOD_SAP_LoRaWAN.JoinedNwkIndCallback) {
            	WiMOD_SAP_LoRaWAN.JoinedNwkIndCallback(rxMsg);
            }
            break;
        case LORAWAN_MSG_RECV_ACK_IND:
            if (WiMOD_SAP_LoRaWAN.RxAckIndCallback) {
            	WiMOD_SAP_LoRaWAN.RxAckIndCallback(rxMsg);
            }
            break;
        default:

//            if (StackErrorClientCB) {
//                StackErrorClientCB(WIMOD_STACK_ERR_UNKNOWN_RX_CMD_ID);
//            }
            break;
    }
    return;
}



//------------------------------------------------------------------------------
//
// Section private functions
//
//------------------------------------------------------------------------------


