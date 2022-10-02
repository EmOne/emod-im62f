//------------------------------------------------------------------------------
//! @file WiMOD_SAP_DEVMGMT.cpp
//! @ingroup WiMOD_SAP_DEVMGMT
//! <!------------------------------------------------------------------------->
//! @brief Implementation of the commands of the DeviceManagement SericeAccessPoint
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
 * THIS IS AN EXAMPLE IMPLEMENTATION ACCORDING THE THE HCI SPEC: V1.8
 * FOR FIRMWARE: LR-BASE
 */

//------------------------------------------------------------------------------
//
// Section Includes Files
//
//------------------------------------------------------------------------------

#include "WiMOD_SAP_DEVMGMT.h"
#include <string.h>

//------------------------------------------------------------------------------
//
// Section public functions
//
//------------------------------------------------------------------------------
static TWiMODLRResultCodes ping(UINT8* statusRsp);
static TWiMODLRResultCodes reset(UINT8* statusRsp);
static TWiMODLRResultCodes getDeviceInfo(TWiMODLR_DevMgmt_DevInfo* info, UINT8* statusRsp);
static TWiMODLRResultCodes getFirmwareInfo(TWiMODLR_DevMgmt_FwInfo* info, UINT8* statusRsp);
static TWiMODLRResultCodes getSystemStatus(TWiMODLR_DevMgmt_SystemStatus* info, UINT8* statusRsp);
static TWiMODLRResultCodes getRtc(UINT32* rtcTime, UINT8* statusRsp);
static TWiMODLRResultCodes setRtc(const UINT32 rtcTime, UINT8* statusRsp);
static TWiMODLRResultCodes getSystemStatus(TWiMODLR_DevMgmt_SystemStatus* info, UINT8* statusRsp);
static TWiMODLRResultCodes getRtc(UINT32* rtcTime, UINT8* statusRsp);
static TWiMODLRResultCodes setRtc(const UINT32 rtcTime, UINT8* statusRsp);
static TWiMODLRResultCodes getRadioConfig(TWiMODLR_DevMgmt_RadioConfig* radioCfg, UINT8* statusRsp);
static TWiMODLRResultCodes setRadioConfig(const TWiMODLR_DevMgmt_RadioConfig* radioCfg, UINT8* statusRsp);
static TWiMODLRResultCodes resetRadioConfig(UINT8* statusRsp);
static TWiMODLRResultCodes getOperationMode(TWiMOD_OperationMode* opMode, UINT8* statusRsp);
static TWiMODLRResultCodes setOperationMode(const TWiMOD_OperationMode opMode, UINT8* statusRsp);
static TWiMODLRResultCodes setRadioMode(const TRadioCfg_RadioMode radioMode, UINT8* statusRsp);
static TWiMODLRResultCodes setAesKey(const UINT8* key, UINT8* statusRsp);
static TWiMODLRResultCodes getAesKey(UINT8* key, UINT8* statusRsp);
static TWiMODLRResultCodes setRtcAlarm(const TWiMODLR_DevMgmt_RtcAlarm* rtcAlarm, UINT8* statusRsp);
static TWiMODLRResultCodes getRtcAlarm(TWiMODLR_DevMgmt_RtcAlarm* rtcAlarm, UINT8* statusRsp);
static TWiMODLRResultCodes clearRtcAlarm(UINT8* statusRsp);
static TWiMODLRResultCodes setHCIConfig(const TWiMODLR_DevMgmt_HciConfig* hciConfig, UINT8* statusRsp);
static TWiMODLRResultCodes getHCIConfig(TWiMODLR_DevMgmt_HciConfig* hciConfig, UINT8* statusRsp);
static void registerPowerUpIndicationClient(TDevMgmtPowerUpCallback cb);
static void WregisterRtcAlarmIndicationClient(TDevMgmtRtcAlarmCallback cb);
static void dispatchDeviceMgmtMessage(TWiMODLR_HCIMessage* rxMsg);
static void process (UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg);
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
TWiMODLR_DevMgmt_HciConfig hciCfg;
UINT8 aesKey [DEVMGMT_AES_KEY_LEN];
TWiMODLR_DevMgmt_RadioConfig radioConfig;

TWiMODLR_DevMgmt_DevInfo DeviceInfo = {
		WiMODLR_RESULT_OK,
		ModuleType_iMS62F,
		WIMOD_DEV_ADDR,
		0x00,
		0x01,
};

TWiMODLR_DevMgmt_FwInfo firmwareInfo = {
		WiMODLR_RESULT_OK,
		0x00,
		0x01,
		0x0000,
		"14 JAN 2022",
		"eMOD Module by EmOne",
};

TWiMODLR_DevMgmt_SystemStatus SystemInfo = {
		WiMODLR_RESULT_OK,
		HAL_TICK_FREQ_DEFAULT,
		(UINT32) HAL_GetTick,
		0x00000000,
	    0x0000,
	    0x0000,
		0x0000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
};

TWiMOD_OperationMode OperationMode = OperationMode_Application;
uint32_t RTCTime;

TWiMODLR_DevMgmt_RtcAlarm RTCAlarm = {
		RTC_Alarm_No_Alarm_Set,
		0x08,
		0x05,
		0x00,
};

WiMOD_SAP_DevMgmt_t WiMOD_SAP_DevMgmt = {
	ping,
	reset,
	getDeviceInfo,
	getFirmwareInfo,
	getSystemStatus,
	getRtc,
	setRtc,
	getRadioConfig,
	setRadioConfig,
	resetRadioConfig,
	getOperationMode,
	setOperationMode,
	setRadioMode,
	setAesKey,
	getAesKey,
	setRtcAlarm,
	getRtcAlarm,
	clearRtcAlarm,
	setHCIConfig,
	getHCIConfig,
	registerPowerUpIndicationClient,
	WregisterRtcAlarmIndicationClient,
	dispatchDeviceMgmtMessage,
	process,
};

//-----------------------------------------------------------------------------
/**
 * @brief Destructor
 */
//WiMOD_SAP_DevMgmt::~WiMOD_SAP_DevMgmt(void)
//{
//
//}

//-----------------------------------------------------------------------------
/**
 * @brief Ping Cmd - Checks if the serial connection of to the WiMOD module is OK
 *
 *
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes ping(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if (statusRsp) {
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    //WiMODLoRaWAN.SapDevMgmt->HciParser = &TWiMODLRHCI;
	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	// put data to tx
	tx->Payload[0] = DeviceInfo.Status = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_PING_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}



//-----------------------------------------------------------------------------
/**
 * @brief Reset Cmd - Do a reset / reboot of the WiMOD
 *
 *
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes reset(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

	if (statusRsp) {
		result = WiMODLR_RESULT_OK;
	} else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	// put data to tx
	tx->Payload[0] = DeviceInfo.Status = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_RESET_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Reset), CFG_SEQ_Prio_0);
    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief Get Device Info - Basic Information about the WiMOD module
 *
 *
 * @param   info        pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getDeviceInfo (TWiMODLR_DevMgmt_DevInfo* info, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = WiMODLR_HCI_RSP_CMD_PAYLOAD_POS;

    if (statusRsp) {
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    memcpy(info, &DeviceInfo, sizeof(TWiMODLR_DevMgmt_DevInfo));
    // put data to tx
    tx->Payload[0] = DeviceInfo.Status = DEVMGMT_STATUS_OK;
    tx->Payload[offset++] = DeviceInfo.ModuleType;
    memcpy(&tx->Payload[offset], &DeviceInfo.DevAdr, 4);
    offset += 0x04;
    memcpy(&tx->Payload[offset], &DeviceInfo.DevID, 4);
    offset += 0x04;

    *statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
    		DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_DEVICEINFO_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			10);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief GetFirmwareInfo Cmd - Get basic info about the running firmware
 *
 *
 * @param   info        pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getFirmwareInfo(TWiMODLR_DevMgmt_FwInfo* info, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

	if (statusRsp) {
		result = WiMODLR_RESULT_OK;
	} else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

	// put data to tx
	tx->Payload[offset++] = firmwareInfo.Status = result;
	tx->Payload[offset++] = firmwareInfo.FirmwareMinorVersion;
	tx->Payload[offset++] = firmwareInfo.FirmwareMayorVersion;
	tx->Payload[offset] = firmwareInfo.BuildCount;
	offset += 0x02;

	strncpy((char*) &tx->Payload[offset], (char*) &info->BuildDateStr,
			WIMOD_DEVMGMT_BUILDDATE_LEN);
	offset += WIMOD_DEVMGMT_BUILDDATE_LEN;

	strncpy((char*) &tx->Payload[offset], (char*) &info->FirmwareName,
			WIMOD_DEVMGMT_FIRMWARE_NAME_LEN);

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_FW_VERSION_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			sizeof(TWiMODLR_DevMgmt_FwInfo));

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief GetSystemStatus Cmd - Get basic info about the system status of WiMOD
 *
 *
 * @param   info        pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getSystemStatus(TWiMODLR_DevMgmt_SystemStatus* info, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (statusRsp) {
		result = WiMODLR_RESULT_OK;
    } else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

	tx->Payload[offset++] = SystemInfo.Status = result;
	tx->Payload[offset++] = SystemInfo.SysTickResolution;
	tx->Payload[offset] = SystemInfo.SysTickCounter;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.RtcTime;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.NvmStatus;
	offset += 0x02;
	tx->Payload[offset] = SystemInfo.BatteryStatus;
	offset += 0x02;
	tx->Payload[offset] = SystemInfo.ExtraStatus;
	offset += 0x02;
	tx->Payload[offset] = SystemInfo.RxPackets;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.RxAddressMatch;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.RxCRCError;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.TxPackets;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.TxError;
	offset += 0x04;
	tx->Payload[offset] = SystemInfo.TxMediaBusyEvents;
	offset += 0x04;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_SYSTEM_STATUS_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			sizeof(TWiMODLR_DevMgmt_SystemStatus));

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief GetRtc Cmd - Get the current RTC time/date from WiMOD
 *
 *
 * @param   rtcTime     pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getRtc(UINT32* rtcTime, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;
    UINT32 u32rtcTime;

    if (statusRsp) {
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    //TODO: Get RTC time
	u32rtcTime = 0xFFFFFFFF; //Dummy
	tx->Payload[0] = RTCAlarm.AlarmStatus = result;
	memcpy(&tx->Payload[1], &u32rtcTime, 4);

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_RTC_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			5);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetRtc Cmd - Set the current RTC time/date of WiMOD
 *
 *
 * @param   rtcTime     32bit data containing the new RTC timestamp
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setRtc(const UINT32 rtcTime, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if (statusRsp && (WiMOD_SAP_DevMgmt.HciParser->Rx.Message.Length >= 4)) {
    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

    //TODO: Set HW RTC
    RTCTime = rtcTime;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_RTC_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

/**
 * @brief GetRadioConfig Cmd - Get the radio settings of the WiMOD
 *
 *
 * @param   radioCfg    pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getRadioConfig(TWiMODLR_DevMgmt_RadioConfig* radioCfg, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (radioCfg && statusRsp) {

//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                                           DEVMGMT_MSG_GET_RADIO_CONFIG_REQ,
//                                           DEVMGMT_MSG_GET_RADIO_CONFIG_RSP,
//                                           NULL, 0);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
//            *statusRsp = rx->Payload[offset++];
//
//            // status check
//            if (*statusRsp == DEVMGMT_STATUS_OK) {
//                    radioCfg->Status = *statusRsp;
//                    radioCfg->RadioMode = (TRadioCfg_RadioMode) rx->Payload[offset++];
//                    radioCfg->GroupAddress = rx->Payload[offset++];
//                    radioCfg->TxGroupAddress = rx->Payload[offset++];
//                    radioCfg->DeviceAddress = NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
//                    radioCfg->TxDeviceAddress = NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
//                    radioCfg->Modulation = (TRadioCfg_Modulation) rx->Payload[offset++];
//                    radioCfg->RfFreq_LSB = rx->Payload[offset++];
//                    radioCfg->RfFreq_MID = rx->Payload[offset++];
//                    radioCfg->RfFreq_MSB = rx->Payload[offset++];
//                    radioCfg->LoRaBandWidth = (TRadioCfg_LoRaBandwidth) rx->Payload[offset++];
//                    radioCfg->LoRaSpreadingFactor = (TRadioCfg_LoRaSpreadingFactor) rx->Payload[offset++];;
//                    radioCfg->ErrorCoding = (TRadioCfg_ErrorCoding) rx->Payload[offset++];;
//                    radioCfg->PowerLevel =  (TRadioCfg_PowerLevel)  rx->Payload[offset++];
//                    radioCfg->TxControl = rx->Payload[offset++];
//                    radioCfg->RxControl = (TRadioCfg_RxControl) rx->Payload[offset++];
//                    radioCfg->RxWindowTime = NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
//                    radioCfg->LedControl = rx->Payload[offset++];
//                    radioCfg->MiscOptions = rx->Payload[offset++];
//                    radioCfg->FskDatarate = (TRadioCfg_FskDatarate) rx->Payload[offset++];
//                    radioCfg->PowerSavingMode = (TRadioCfg_PowerSavingMode) rx->Payload[offset++];
//                    radioCfg->LbtThreshold = (INT16) NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
//            }
//        }

    	radioCfg->Status = radioConfig.Status;
		radioCfg->RadioMode = radioConfig.RadioMode;
		radioCfg->GroupAddress = radioConfig.GroupAddress;
		radioCfg->TxGroupAddress = radioConfig.TxGroupAddress;
		radioCfg->DeviceAddress = radioConfig.DeviceAddress;
		radioCfg->TxDeviceAddress = radioConfig.TxDeviceAddress;
		radioCfg->Modulation = radioConfig.Modulation;
		radioCfg->RfFreq_LSB = radioConfig.RfFreq_LSB;
		radioCfg->RfFreq_MID = radioConfig.RfFreq_MID;
		radioCfg->RfFreq_MSB = radioConfig.RfFreq_MSB;
		radioCfg->LoRaBandWidth = radioConfig.LoRaBandWidth;
		radioCfg->LoRaSpreadingFactor = radioConfig.LoRaSpreadingFactor;
		radioCfg->ErrorCoding = radioConfig.ErrorCoding;
		radioCfg->PowerLevel = radioConfig.PowerLevel;
		radioCfg->TxControl = radioConfig.TxControl;
		radioCfg->RxControl = radioConfig.RxControl;
		radioCfg->RxWindowTime = radioConfig.RxWindowTime;
		radioCfg->LedControl = radioConfig.LedControl;
		radioCfg->MiscOptions = radioConfig.MiscOptions;
		radioCfg->FskDatarate = radioConfig.FskDatarate;
		radioCfg->PowerSavingMode = radioConfig.PowerSavingMode;
		radioCfg->LbtThreshold = radioConfig.LbtThreshold;

    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    tx->Payload[0] = DEVMGMT_STATUS_OK;
    memcpy(&tx->Payload[1], &radioConfig, sizeof(TWiMODLR_DevMgmt_RadioConfig));

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_RADIO_CONFIG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			sizeof(TWiMODLR_DevMgmt_RadioConfig));

	return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetRadioConfig Cmd - Set the radio settings of the WiMOD
 *
 *
 * @param   radioCfg    pointer to the new radio configuration
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setRadioConfig(const TWiMODLR_DevMgmt_RadioConfig* radioCfg, UINT8* statusRsp) {
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;

    if (radioCfg && statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 0x1A)) {
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->StoreNwmFlag;
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RadioMode;
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->GroupAddress;
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->TxGroupAddress;
//        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->DeviceAddress);
//        offset += 0x02;
//        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->TxDeviceAddress);
//        offset += 0x02;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->Modulation;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_LSB;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_MID;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_MSB;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LoRaBandWidth;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LoRaSpreadingFactor;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->ErrorCoding;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->PowerLevel;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->TxControl;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RxControl;
//        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->RxWindowTime);
//        offset += 0x02;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LedControl;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->MiscOptions;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->FskDatarate;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->PowerSavingMode;
//        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], (UINT16) radioCfg->LbtThreshold);
//        offset += 0x02;
//
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_SET_RADIO_CONFIG_REQ,
//                DEVMGMT_MSG_SET_RADIO_CONFIG_RSP,
//				WiMOD_SAP_DevMgmt.txPayload, offset);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
//            offset = WiMODLR_HCI_RSP_STATUS_POS;
//            *statusRsp = rx->Payload[offset++];
//
//            // status check
//            if (*statusRsp == DEVMGMT_STATUS_OK) {
//
//
//            } else {
//            }
//        }
    	radioConfig.StoreNwmFlag = (UINT8) radioCfg->StoreNwmFlag;
		radioConfig.RadioMode = (UINT8) radioCfg->RadioMode;
		radioConfig.GroupAddress = (UINT8) radioCfg->GroupAddress;
		radioConfig.TxGroupAddress = (UINT8) radioCfg->TxGroupAddress;
		radioConfig.DeviceAddress = radioCfg->DeviceAddress;
		radioConfig.TxDeviceAddress = radioCfg->TxDeviceAddress;
		radioConfig.Modulation = (UINT8) radioCfg->Modulation;
		radioConfig.RfFreq_LSB = (UINT8) radioCfg->RfFreq_LSB;
		radioConfig.RfFreq_MID = (UINT8) radioCfg->RfFreq_MID;
		radioConfig.RfFreq_MSB = (UINT8) radioCfg->RfFreq_MSB;
		radioConfig.LoRaBandWidth = (UINT8) radioCfg->LoRaBandWidth;
		radioConfig.LoRaSpreadingFactor = (UINT8) radioCfg->LoRaSpreadingFactor;
		radioConfig.ErrorCoding = (UINT8) radioCfg->ErrorCoding;
		radioConfig.PowerLevel = (UINT8) radioCfg->PowerLevel;
		radioConfig.TxControl = (UINT8) radioCfg->TxControl;
		radioConfig.RxControl = (UINT8) radioCfg->RxControl;
		radioConfig.RxWindowTime = radioCfg->RxWindowTime;
		radioConfig.LedControl = (UINT8) radioCfg->LedControl;
		radioConfig.MiscOptions = (UINT8) radioCfg->MiscOptions;
		radioConfig.FskDatarate = (UINT8) radioCfg->FskDatarate;
		radioConfig.PowerSavingMode = (UINT8) radioCfg->PowerSavingMode;
		radioConfig.LbtThreshold = radioCfg->LbtThreshold;

    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_RADIO_CONFIG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief ResetRadioConfig Cmd - Reset the radio config to factory defaults.
 *
 *
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes resetRadioConfig(UINT8* statusRsp) {
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    if (statusRsp) {
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_RESET_RADIO_CONFIG_REQ,
//                DEVMGMT_MSG_RESET_RADIO_CONFIG_RSP,
//                NULL, 0);
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

    	//TODO: Load default radio configuration from factory setting
//		radioConfig.StoreNwmFlag = (UINT8) radioCfg->StoreNwmFlag;
//		radioConfig.RadioMode = (UINT8) radioCfg->RadioMode;
//		radioConfig.GroupAddress = (UINT8) radioCfg->GroupAddress;
//		radioConfig.TxGroupAddress = (UINT8) radioCfg->TxGroupAddress;
//		radioConfig.DeviceAddress = radioCfg->DeviceAddress;
//		radioConfig.TxDeviceAddress = radioCfg->TxDeviceAddress;
//		radioConfig.Modulation = (UINT8) radioCfg->Modulation;
//		radioConfig.RfFreq_LSB = (UINT8) radioCfg->RfFreq_LSB;
//		radioConfig.RfFreq_MID = (UINT8) radioCfg->RfFreq_MID;
//		radioConfig.RfFreq_MSB = (UINT8) radioCfg->RfFreq_MSB;
//		radioConfig.LoRaBandWidth = (UINT8) radioCfg->LoRaBandWidth;
//		radioConfig.LoRaSpreadingFactor = (UINT8) radioCfg->LoRaSpreadingFactor;
//		radioConfig.ErrorCoding = (UINT8) radioCfg->ErrorCoding;
//		radioConfig.PowerLevel = (UINT8) radioCfg->PowerLevel;
//		radioConfig.TxControl = (UINT8) radioCfg->TxControl;
//		radioConfig.RxControl = (UINT8) radioCfg->RxControl;
//		radioConfig.RxWindowTime = radioCfg->RxWindowTime;
//		radioConfig.LedControl = (UINT8) radioCfg->LedControl;
//		radioConfig.MiscOptions = (UINT8) radioCfg->MiscOptions;
//		radioConfig.FskDatarate = (UINT8) radioCfg->FskDatarate;
//		radioConfig.PowerSavingMode = (UINT8) radioCfg->PowerSavingMode;
//		radioConfig.LbtThreshold = radioCfg->LbtThreshold;

        result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    tx->Payload[0] = DEVMGMT_STATUS_OK;

   	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
   			DEVMGMT_SAP_ID,
   			DEVMGMT_MSG_SET_RADIO_CONFIG_RSP,
   			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
   			1);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief GetOperationMode Cmd - Get the current operation mode of the WiMOD
 *
 *
 * @param   opMode      pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getOperationMode(TWiMOD_OperationMode* opMode, UINT8* statusRsp) {
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

	if (statusRsp && opMode) {
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_GET_OPMODE_REQ,
//                DEVMGMT_MSG_GET_OPMODE_RSP,
//                NULL, 0);
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//            *opMode = (TWiMOD_OperationMode) WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_CMD_PAYLOAD_POS];
//        }
		*opMode = OperationMode;
		result = WiMODLR_RESULT_OK;
	} else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

	// put data to tx
	tx->Payload[0] = DEVMGMT_STATUS_OK;
	tx->Payload[1] = *opMode;
	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_OPMODE_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			2);
	result = WiMODLR_RESULT_OK;

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetOperationMode Cmd - Set the current operation mode of the WiMOD
 *
 *
 * @param   opMode      the new operation mode to set
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setOperationMode(const TWiMOD_OperationMode opMode, UINT8* statusRsp) {
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
	if (statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 1)) {
//    	WiMOD_SAP_DevMgmt.txPayload[0] = (UINT8) opMode;
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_SET_OPMODE_REQ,
//                DEVMGMT_MSG_SET_OPMODE_RSP,
//				WiMOD_SAP_DevMgmt.txPayload, 1);
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

		result = WiMODLR_RESULT_OK;
	} else {

	}

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

	// put data to tx
	OperationMode = opMode;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_OPMODE_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetRadioMode - Set the current radio mode of the WiMOD
 *
 *
 * @param   radioMode    the new radio mode to set
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @warning: This use this command with care!
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setRadioMode(const TRadioCfg_RadioMode radioMode, UINT8* statusRsp) {
     TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
     if (statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 1)) {
//    	 WiMOD_SAP_DevMgmt.txPayload[0] = (UINT8) radioMode;
//         result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                 DEVMGMT_MSG_SET_RADIO_MODE_REQ,
//                 DEVMGMT_MSG_SET_RADIO_MODE_RSP,
//				 WiMOD_SAP_DevMgmt.txPayload, 1);
//         // copy response status
//         if (WiMODLR_RESULT_OK == result) {
//             *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//         }
    	 result = WiMODLR_RESULT_OK;
     } else {
    	 result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
     }

 	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

 	//TODO: Set radio mode
 	tx->Payload[0] = DEVMGMT_STATUS_OK;

 	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
 			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_RADIO_MODE_RSP,
 			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
 			1);

     return result;
 }


//-----------------------------------------------------------------------------
/**
 * @brief SetAesKey Cmd - Set the 128bit AES that is to be used for encryption
 *
 *
 * @param   key         pointer to the 128bit AES key (pointer to array of 16 UINT8 entries)
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setAesKey(const UINT8* key, UINT8* statusRsp) {
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    if (statusRsp && key && (WiMOD_SAP_DevMgmt.txyPayloadSize >= DEVMGMT_AES_KEY_LEN)) {

        memcpy(aesKey, key, DEVMGMT_AES_KEY_LEN);
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_SET_AES_KEY_REQ,
//                DEVMGMT_MSG_SET_AES_KEY_RSP,
//				WiMOD_SAP_DevMgmt.txPayload, DEVMGMT_AES_KEY_LEN);
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
        result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_AES_KEY_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief GetAesKey Cmd - Get the 128bit AES that that is used for encryption
 *
 *
 * @param   key         pointer where to store the 128bit AES key (pointer to array of 16 UINT8 entries)
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getAesKey(UINT8* key, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    if (statusRsp && key) {

//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_GET_AES_KEY_REQ,
//                DEVMGMT_MSG_GET_AES_KEY_RSP,
//                NULL, 0);
//
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//            memcpy(key, &WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_CMD_PAYLOAD_POS],  DEVMGMT_AES_KEY_LEN);
//        }
    	memcpy(key, aesKey,  DEVMGMT_AES_KEY_LEN);
    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

   	//TODO: Get AES key
    tx->Payload[0] = DEVMGMT_STATUS_OK;
   	memcpy(&tx->Payload[1], (UINT8*) &aesKey,  DEVMGMT_AES_KEY_LEN);

   	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
   			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_AES_KEY_RSP,
   			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
   			1 + DEVMGMT_AES_KEY_LEN);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetRtcAlarm Cmd - Set a new RTC alarm config
 *
 *
 * @param   rtcAlarm   pointer to write the RTC alarm relevant parameter set to
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes setRtcAlarm(const TWiMODLR_DevMgmt_RtcAlarm* rtcAlarm, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8 			   offset = 0;

    if (statusRsp && rtcAlarm && (WiMOD_SAP_DevMgmt.HciParser->Rx.ExpectedRsponseMsg.Length >= 0x04)) {

    	//TODO: Set HW RTC alarm
    	RTCAlarm = *rtcAlarm;
    	result = WiMODLR_RESULT_OK;
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_RTC_ALARM_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief GetRtcAlarm Cmd - Get information about RTC alarm feature
 *
 *
 * @param   rtcAlarm    pointer where to store the RTC alarm parameter set
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getRtcAlarm(TWiMODLR_DevMgmt_RtcAlarm* rtcAlarm, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
    UINT8			   offset = WiMODLR_HCI_RSP_STATUS_POS;

    // copy response status
    if (statusRsp) {
    	*rtcAlarm = RTCAlarm;
		result = WiMODLR_RESULT_OK;
    }
    else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[offset++] = DEVMGMT_STATUS_OK;
	tx->Payload[offset++] = RTCAlarm.AlarmStatus;
	tx->Payload[offset++] = RTCAlarm.Options;
	tx->Payload[offset++] = RTCAlarm.Hour;
	tx->Payload[offset++] = RTCAlarm.Minutes;
	tx->Payload[offset++] = RTCAlarm.Seconds;
	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_RTC_ALARM_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			sizeof(TWiMODLR_DevMgmt_RtcAlarm) + 1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief ClearRtcAlarm Cmd - Clear a pending RTC alarm
 *
 *
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes clearRtcAlarm(UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if (statusRsp) {
    	RTCAlarm.AlarmStatus = RTC_Alarm_No_Alarm_Set;
    	RTCAlarm.Hour = 8;
    	RTCAlarm.Minutes = 0;
    	RTCAlarm.Options = 0;
    	RTCAlarm.Seconds = 0;
    	result = WiMODLR_RESULT_OK;
	} else {
		result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
	}

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_CLEAR_RTC_ALARM_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief SetHCIConfig Cmd - Sets the HCI config options for the LR-BASE_PLUS firmware
 *
 *
 * @param   hciConfig   pointer to the new configuration
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */

TWiMODLRResultCodes setHCIConfig(const TWiMODLR_DevMgmt_HciConfig* hciConfig, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = 0;

    if (hciConfig && statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 0x06)) {
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->StoreNwmFlag;
//    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->BaudrateID;
//        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], hciConfig->NumWakeUpChars);
//        offset += 0x02;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->TxHoldTime;
//        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->RxHoldTime;
//
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                                        DEVMGMT_MSG_SET_HCI_CFG_REQ,
//                                        DEVMGMT_MSG_SET_HCI_CFG_RSP,
//										WiMOD_SAP_DevMgmt.txPayload, offset);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
//            offset = WiMODLR_HCI_RSP_STATUS_POS;
//            *statusRsp = rx->Payload[offset++];
//
//            // status check
//            if (*statusRsp == DEVMGMT_STATUS_OK) {
//
//
//            } else {
//            }
//        }
		hciCfg.StoreNwmFlag = (UINT8) hciConfig->StoreNwmFlag;
		hciCfg.BaudrateID = (UINT8) hciConfig->BaudrateID;
		hciCfg.NumWakeUpChars = hciConfig->NumWakeUpChars;
		hciCfg.TxHoldTime = (UINT8) hciConfig->TxHoldTime;
		hciCfg.RxHoldTime = (UINT8) hciConfig->RxHoldTime;

    	result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[0] = DEVMGMT_STATUS_OK;

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_SET_HCI_CFG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1);

    return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief SetHCIConfig Cmd - Gets the HCI config options for the LR-BASE_PLUS firmware
 *
 *
 * @param   hciConfig   pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes getHCIConfig(TWiMODLR_DevMgmt_HciConfig* hciConfig, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;
//    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (hciConfig && statusRsp) {
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                                           DEVMGMT_MSG_GET_HCI_CFG_REQ,
//                                           DEVMGMT_MSG_GET_HCI_CFG_RSP,
//                                           NULL, 0);
//
//        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
//
//            hciConfig->Status         = rx->Payload[offset++];
//            hciConfig->BaudrateID     = (TWiMOD_HCI_Baudrate) rx->Payload[offset++];
//            hciConfig->NumWakeUpChars = NTOH16(&rx->Payload[offset]);
//            offset += 0x02;
//            hciConfig->TxHoldTime     = (UINT8)rx->Payload[offset++];
//            hciConfig->RxHoldTime     = (UINT8)rx->Payload[offset++];
//
//            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

		hciConfig->Status         = hciCfg.Status;;
		hciConfig->BaudrateID     = hciCfg.BaudrateID;
		hciConfig->NumWakeUpChars = hciCfg.NumWakeUpChars;
		hciConfig->TxHoldTime     = hciCfg.TxHoldTime;
		hciConfig->RxHoldTime     = hciCfg.RxHoldTime;

        result = WiMODLR_RESULT_OK;
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
	tx->Payload[0] = DEVMGMT_STATUS_OK;
	memcpy(&tx->Payload[1], &hciCfg, sizeof(TWiMODLR_DevMgmt_HciConfig));

	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
			DEVMGMT_SAP_ID,
			DEVMGMT_MSG_GET_HCI_CFG_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
			1 + sizeof(TWiMODLR_DevMgmt_HciConfig));

    return result;
}


//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for processing a PowerUp Indication message
 *
 *
 * @param   cb          pointer to callback function
 */

void registerPowerUpIndicationClient(TDevMgmtPowerUpCallback cb)
{
	WiMOD_SAP_DevMgmt.PowerUpCallack = cb;
}

//-----------------------------------------------------------------------------
/**
 * @brief Register a callback function for processing a RTC Alarm Indication message
 *
 *
 * @param   cb          pointer to callback function
 */
void WregisterRtcAlarmIndicationClient(TDevMgmtRtcAlarmCallback cb)
{
	WiMOD_SAP_DevMgmt.RtcAlarmCallback = cb;
}

void process (UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg)
{
    switch (rxMsg->MsgID)
    {
        case DEVMGMT_MSG_PING_REQ:
        	WiMOD_SAP_DevMgmt.Ping(statusRsp);

            break;
        case DEVMGMT_MSG_GET_DEVICEINFO_REQ:
        	WiMOD_SAP_DevMgmt.GetDeviceInfo(&DeviceInfo, statusRsp);

        	break;
        case DEVMGMT_MSG_GET_FW_VERSION_REQ:
        	WiMOD_SAP_DevMgmt.GetFirmwareInfo(&firmwareInfo, statusRsp);
        	break;
        case DEVMGMT_MSG_RESET_REQ:
        	WiMOD_SAP_DevMgmt.Reset(statusRsp);
        	break;
        case DEVMGMT_MSG_SET_OPMODE_REQ:
        	WiMOD_SAP_DevMgmt.SetOperationMode(rxMsg->Payload[0], statusRsp);
        	break;
        case DEVMGMT_MSG_GET_OPMODE_REQ:
        	WiMOD_SAP_DevMgmt.GetOperationMode(&OperationMode, statusRsp);
        	break;
        case DEVMGMT_MSG_SET_RTC_REQ:
        	WiMOD_SAP_DevMgmt.SetRtc(NTOH32(&rxMsg->Payload[0]), statusRsp);
        	break;
        case DEVMGMT_MSG_GET_RTC_REQ:
        	WiMOD_SAP_DevMgmt.GetRtc(&RTCTime, statusRsp);
        	break;
        case DEVMGMT_MSG_GET_SYSTEM_STATUS_REQ:
        	WiMOD_SAP_DevMgmt.GetSystemStatus(&SystemInfo, statusRsp);
        	break;
        case DEVMGMT_MSG_SET_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.SetRtcAlarm((TWiMODLR_DevMgmt_RtcAlarm*) &rxMsg->Payload[0], statusRsp);
        	break;
        case DEVMGMT_MSG_CLEAR_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.ClearRtcAlarm(statusRsp);
        	break;
        case DEVMGMT_MSG_GET_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.GetRtcAlarm((TWiMODLR_DevMgmt_RtcAlarm*) &RTCAlarm, statusRsp);
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
void dispatchDeviceMgmtMessage(TWiMODLR_HCIMessage* rxMsg)
{
    switch (rxMsg->MsgID)
    {
        case DEVMGMT_MSG_POWER_UP_IND:
            if (WiMOD_SAP_DevMgmt.PowerUpCallack) {
            	WiMOD_SAP_DevMgmt.PowerUpCallack();
            }
            break;
        case DEVMGMT_MSG_RTC_ALARM_IND:
        	if (WiMOD_SAP_DevMgmt.RtcAlarmCallback) {
        		WiMOD_SAP_DevMgmt.RtcAlarmCallback();
        	}
        	break;
        default:
            break;
    }
    return;
}


//------------------------------------------------------------------------------
//
// Section private functions
//
//------------------------------------------------------------------------------


