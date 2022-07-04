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
#include <time.h>
#include <timeServer.h>
#include <rtc.h>

static TimerEvent_t timerReset;
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

TWiMODLR_DevMgmt_DevInfo DeviceInfo = {
		WiMODLR_RESULT_OK,
		ModuleType_iMS62F, //WIMOD_MODULE_TYPE_IM880B,
		WIMOD_DEV_ADDR,
		0x00,
		0x01,
};

TWiMODLR_DevMgmt_FwInfo firmwareInfo = {
		WiMODLR_RESULT_OK,
		0x00,
		0x01,
		0x0000,
		"14 Jan 22",
		"eMOD Module by EmOne",
};

TWiMODLR_DevMgmt_SystemStatus SystemInfo = {
		WiMODLR_RESULT_OK,
		HAL_TICK_FREQ_DEFAULT,
		HAL_GetTick,
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

TWiMODLR_DevMgmt_RadioConfig RadioConfig = {0};

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

    		//WiMODLoRaWAN.SapDevMgmt->HciParser = &TWiMODLRHCI;
            TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

            // put data to tx
            tx->Payload[0] = DeviceInfo.Status; //0x55;

            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(
            		DEVMGMT_SAP_ID,DEVMGMT_MSG_PING_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);
            result = WiMODLR_RESULT_OK;
    }
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

			TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

			// put data to tx
			tx->Payload[0] = DeviceInfo.Status; //0x55;

			*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_RESET_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);

			//start timer reset (approx. 200ms)
			TimerInit(&timerReset, HAL_NVIC_SystemReset);
			TimerSetValue(&timerReset, 200);
			TimerStart(&timerReset);

			result = WiMODLR_RESULT_OK;
	}
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

            TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

            // put data to tx
            tx->Payload[WiMODLR_HCI_RSP_STATUS_POS] = info->Status;
            tx->Payload[offset++]					= info->ModuleType;
            tx->Payload[offset]					    = info->DevAdr;
            offset += 0x02;
            tx->Payload[offset++]					= info->GroupAdr;
            // reserved field
            offset++;
            tx->Payload[offset]					    = info->DevID;
            offset += 0x04;

            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,DEVMGMT_MSG_GET_DEVICEINFO_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], sizeof(TWiMODLR_DevMgmt_DevInfo));
            result = WiMODLR_RESULT_OK;

    }
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

			TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

			// put data to tx
            tx->Payload[offset++] 	= info->Status;
            tx->Payload[offset++]	= info->FirmwareMinorVersion;
            tx->Payload[offset++]	= info->FirmwareMayorVersion;
            tx->Payload[offset]		= info->BuildCount;
            offset+= 0x02;

            strncpy((char*) &tx->Payload[offset], (char*)info->BuildDateStr, sizeof(info->BuildDateStr));
            offset += WIMOD_DEVMGMT_BUILDDATE_LEN;

            strncpy((const char*) &tx->Payload[offset], (char*)info->FirmwareName, sizeof(info->FirmwareName));


			*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_GET_FW_VERSION_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], sizeof(TWiMODLR_DevMgmt_FwInfo));
			result = WiMODLR_RESULT_OK;
	}
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

        	TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

            tx->Payload[offset++]	= info->Status;
            tx->Payload[offset++]	= info->SysTickResolution;
            tx->Payload[offset]		= info->SysTickCounter;
            offset += 0x04;
            tx->Payload[offset]		= info->RtcTime;
            offset += 0x04;
            tx->Payload[offset]		= info->NvmStatus;
            offset += 0x02;
            tx->Payload[offset]		= info->BatteryStatus;
            offset += 0x02;
            tx->Payload[offset]		= info->ExtraStatus;
            offset += 0x02;
            tx->Payload[offset]		= info->RxPackets;
            offset += 0x04;
            tx->Payload[offset]		= info->RxAddressMatch;
            offset += 0x04;
            tx->Payload[offset]		= info->RxCRCError;
            offset += 0x04;
            tx->Payload[offset]		= info->TxPackets;
            offset += 0x04;
            tx->Payload[offset]		= info->TxError;
            offset += 0x04;
            tx->Payload[offset]		= info->TxMediaBusyEvents;
            offset += 0x04;

			*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_GET_SYSTEM_STATUS_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], sizeof(TWiMODLR_DevMgmt_SystemStatus));
			result = WiMODLR_RESULT_OK;
    }
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
    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (statusRsp) {

    	TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    	tx->Payload[offset++] = RTCAlarm.AlarmStatus;
    	*((UINT32 *) &tx->Payload[offset])	= *rtcTime = TimerGetCurrentTime();
        offset += 0x04;
		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_GET_RTC_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 5);
		result = WiMODLR_RESULT_OK;

    }
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

//    if (statusRsp && (WiMOD_SAP_DevMgmt.HciParser->Rx.Message.Length >= 4)) {
	if (statusRsp) {
		TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
		time_t raw = RTCTime = rtcTime;
		struct tm ts;
		ts = *localtime(&raw);

		RTC_TimeTypeDef RTC_TimeStruct;
		RTC_DateTypeDef RTC_DateStruct;

		RTC_DateStruct.Year = ts.tm_year;
		RTC_DateStruct.Month = ts.tm_mon;
		RTC_DateStruct.Date = ts.tm_mday;
		RTC_DateStruct.WeekDay = ts.tm_wday;
		HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);

		/*at 0:0:0*/
		RTC_TimeStruct.Hours = ts.tm_hour;
		RTC_TimeStruct.Minutes = ts.tm_min;

		RTC_TimeStruct.Seconds = ts.tm_sec;
		RTC_TimeStruct.TimeFormat = 0;
		RTC_TimeStruct.SubSeconds = 0;
		RTC_TimeStruct.StoreOperation = RTC_DAYLIGHTSAVING_NONE;
		RTC_TimeStruct.DayLightSaving = RTC_STOREOPERATION_RESET;

		HAL_RTC_SetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);

		/*Enable Direct Read of the calendar registers (not through Shadow) */
		HAL_RTCEx_EnableBypassShadow(&hrtc);

		HW_RTC_SetTimerContext();

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_SET_RTC_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 4);

		result = WiMODLR_RESULT_OK;
    }
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
    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (radioCfg && statusRsp) {

//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                                           DEVMGMT_MSG_GET_RADIO_CONFIG_REQ,
//                                           DEVMGMT_MSG_GET_RADIO_CONFIG_RSP,
//                                           NULL, 0);
        if (result == WiMODLR_RESULT_OK) {
//            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
            *statusRsp = RadioConfig.Status = DeviceInfo.Status;// rx->Payload[offset++];

            // status check
            if (*statusRsp == DEVMGMT_STATUS_OK) {
                    radioCfg->Status = *statusRsp;
                    radioCfg->RadioMode = (TRadioCfg_RadioMode) RadioConfig.RadioMode; //rx->Payload[offset++];
                    radioCfg->GroupAddress = RadioConfig.GroupAddress; //rx->Payload[offset++];
                    radioCfg->TxGroupAddress = RadioConfig.TxGroupAddress;//rx->Payload[offset++];
                    radioCfg->DeviceAddress = RadioConfig.DeviceAddress;//NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
                    radioCfg->TxDeviceAddress = RadioConfig.TxDeviceAddress;//NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
                    radioCfg->Modulation = (TRadioCfg_Modulation) RadioConfig.Modulation;//rx->Payload[offset++];
                    radioCfg->RfFreq_LSB = RadioConfig.RfFreq_LSB; //rx->Payload[offset++];
                    radioCfg->RfFreq_MID = RadioConfig.RfFreq_MID; //rx->Payload[offset++];
                    radioCfg->RfFreq_MSB = RadioConfig.RfFreq_MSB; //rx->Payload[offset++];
                    radioCfg->LoRaBandWidth = (TRadioCfg_LoRaBandwidth) RadioConfig.LoRaBandWidth;//rx->Payload[offset++];
                    radioCfg->LoRaSpreadingFactor = (TRadioCfg_LoRaSpreadingFactor) RadioConfig.LoRaSpreadingFactor; // rx->Payload[offset++];;
                    radioCfg->ErrorCoding = (TRadioCfg_ErrorCoding) RadioConfig.ErrorCoding; //rx->Payload[offset++];;
                    radioCfg->PowerLevel =  (TRadioCfg_PowerLevel) RadioConfig.PowerLevel; // rx->Payload[offset++];
                    radioCfg->TxControl = RadioConfig.TxControl; // rx->Payload[offset++];
                    radioCfg->RxControl = (TRadioCfg_RxControl) RadioConfig.RxControl;// rx->Payload[offset++];
                    radioCfg->RxWindowTime = RadioConfig.RxWindowTime; // NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
                    radioCfg->LedControl = RadioConfig.LedControl; //rx->Payload[offset++];
                    radioCfg->MiscOptions = RadioConfig.MiscOptions; //rx->Payload[offset++];
                    radioCfg->FskDatarate = (TRadioCfg_FskDatarate) RadioConfig.FskDatarate; //rx->Payload[offset++];
                    radioCfg->PowerSavingMode = (TRadioCfg_PowerSavingMode) RadioConfig.PowerSavingMode;//rx->Payload[offset++];
                    radioCfg->LbtThreshold = (INT16) RadioConfig.LbtThreshold;//NTOH16(&rx->Payload[offset]);
//                    offset += 0x02;
            }
        }
    }
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
    UINT8              offset = 0;

    if (radioCfg && statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 0x1A)) {
#if 0
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->StoreNwmFlag;
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RadioMode;
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->GroupAddress;
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->TxGroupAddress;
    	HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->DeviceAddress);
    	offset += 0x02;
        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->TxDeviceAddress);
        offset += 0x02;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->Modulation;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_LSB;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_MID;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RfFreq_MSB;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LoRaBandWidth;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LoRaSpreadingFactor;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->ErrorCoding;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->PowerLevel;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->TxControl;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->RxControl;
        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], radioCfg->RxWindowTime);
        offset += 0x02;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->LedControl;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->MiscOptions;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->FskDatarate;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) radioCfg->PowerSavingMode;
        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], (UINT16) radioCfg->LbtThreshold);
        offset += 0x02;

        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                DEVMGMT_MSG_SET_RADIO_CONFIG_REQ,
                DEVMGMT_MSG_SET_RADIO_CONFIG_RSP,
				WiMOD_SAP_DevMgmt.txPayload, offset);
        if (result == WiMODLR_RESULT_OK) {
                   const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
                   offset = WiMODLR_HCI_RSP_STATUS_POS;
                   *statusRsp = rx->Payload[offset++];

                   // status check
                   if (*statusRsp == DEVMGMT_STATUS_OK) {


                   } else {
                   }
               }
#else
    	RadioConfig.StoreNwmFlag = (UINT8) radioCfg->StoreNwmFlag;
    	RadioConfig.RadioMode = (UINT8) radioCfg->RadioMode;
    	RadioConfig.GroupAddress = (UINT8) radioCfg->GroupAddress;
    	RadioConfig.TxGroupAddress = (UINT8) radioCfg->TxGroupAddress;
    	RadioConfig.DeviceAddress = radioCfg->DeviceAddress;
        RadioConfig.TxDeviceAddress = radioCfg->TxDeviceAddress;
        RadioConfig.Modulation = (UINT8) radioCfg->Modulation;
        RadioConfig.RfFreq_LSB = (UINT8) radioCfg->RfFreq_LSB;
        RadioConfig.RfFreq_MID = (UINT8) radioCfg->RfFreq_MID;
        RadioConfig.RfFreq_MSB = (UINT8) radioCfg->RfFreq_MSB;
        RadioConfig.LoRaBandWidth = (UINT8) radioCfg->LoRaBandWidth;
        RadioConfig.LoRaSpreadingFactor = (UINT8) radioCfg->LoRaSpreadingFactor;
        RadioConfig.ErrorCoding = (UINT8) radioCfg->ErrorCoding;
        RadioConfig.PowerLevel = (UINT8) radioCfg->PowerLevel;
        RadioConfig.TxControl = (UINT8) radioCfg->TxControl;
        RadioConfig.RxControl = (UINT8) radioCfg->RxControl;
        RadioConfig.RxWindowTime = radioCfg->RxWindowTime;

        RadioConfig.LedControl = (UINT8) radioCfg->LedControl;
        RadioConfig.MiscOptions = (UINT8) radioCfg->MiscOptions;
        RadioConfig.FskDatarate = (UINT8) radioCfg->FskDatarate;
        RadioConfig.PowerSavingMode = (UINT8) radioCfg->PowerSavingMode;
        RadioConfig.LbtThreshold = (UINT16) radioCfg->LbtThreshold;

        TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
		DEVMGMT_MSG_SET_RADIO_CONFIG_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 4);

		result = WiMODLR_RESULT_OK;
#endif

    }
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
//
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }

        TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

        tx->Payload[0] = DeviceInfo.Status;

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_RESET_RADIO_CONFIG_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);
    }
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

    	TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

		// put data to tx
		tx->Payload[0] = *statusRsp = DeviceInfo.Status;
		tx->Payload[1] = *opMode =OperationMode	;

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_GET_OPMODE_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 2);
		result = WiMODLR_RESULT_OK;

	}
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
//    if (statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 1)) {
//    	WiMOD_SAP_DevMgmt.txPayload[0] = (UINT8) opMode;
//        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
//                DEVMGMT_MSG_SET_OPMODE_REQ,
//                DEVMGMT_MSG_SET_OPMODE_RSP,
//				WiMOD_SAP_DevMgmt.txPayload, 1);
//        // copy response status
//        if (WiMODLR_RESULT_OK == result) {
//            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
//        }
//    }
//    return result;
	if (statusRsp) {

			TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

			// put data to tx
			tx->Payload[0] = DeviceInfo.Status;
			OperationMode = opMode;

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_SET_OPMODE_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);
		result = WiMODLR_RESULT_OK;
	}
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

    	 WiMOD_SAP_DevMgmt.txPayload[0] = (UINT8) radioMode;
#if 0
    	 result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                 DEVMGMT_MSG_SET_RADIO_MODE_REQ,
                 DEVMGMT_MSG_SET_RADIO_MODE_RSP,
				 WiMOD_SAP_DevMgmt.txPayload, 1);
         // copy response status
         if (WiMODLR_RESULT_OK == result) {
             *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
         }
#else
         TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

         // put data to tx
		tx->Payload[0] = *statusRsp = DeviceInfo.Status;
		tx->Payload[1] = DEVMGMT_STATUS_WRONG_PARAMETER; //ERROR CODE

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_SET_RADIO_MODE_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 2);
		 result = WiMODLR_RESULT_OK;
#endif


     }
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

        memcpy(WiMOD_SAP_DevMgmt.txPayload, key, DEVMGMT_AES_KEY_LEN);
#if 0
        memcpy(WiMOD_SAP_DevMgmt.txPayload, key, DEVMGMT_AES_KEY_LEN);
        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                DEVMGMT_MSG_SET_AES_KEY_REQ,
                DEVMGMT_MSG_SET_AES_KEY_RSP,
				WiMOD_SAP_DevMgmt.txPayload, DEVMGMT_AES_KEY_LEN);
        // copy response status
        if (WiMODLR_RESULT_OK == result) {
            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
        }
#else
        TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

		// put data to tx
		tx->Payload[0] = *statusRsp = DeviceInfo.Status;

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_SET_AES_KEY_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);
		result = WiMODLR_RESULT_OK;
#endif

    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
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
#if 0
        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                DEVMGMT_MSG_GET_AES_KEY_REQ,
                DEVMGMT_MSG_GET_AES_KEY_RSP,
                NULL, 0);

        // copy response status
        if (WiMODLR_RESULT_OK == result) {
            *statusRsp = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_STATUS_POS];
            memcpy(key, &WiMOD_SAP_DevMgmt.HciParser->GetRxMessage()->Payload[WiMODLR_HCI_RSP_CMD_PAYLOAD_POS],  DEVMGMT_AES_KEY_LEN);
        }
#else
		TWiMODLR_HCIMessage *tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;

		// put data to tx
		tx->Payload[0] = *statusRsp = DeviceInfo.Status;

		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_GET_AES_KEY_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
				1);
		result = WiMODLR_RESULT_OK;
#endif
    } else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
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
    UINT8 			   offset = 0;

    if (statusRsp && rtcAlarm && (WiMOD_SAP_DevMgmt.HciParser->Rx.ExpectedRsponseMsg.Length >= 0x04)) {

    	TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
#if 0
    	tx->Payload[offset++] = (UINT8) rtcAlarm->Options;
    	tx->Payload[offset++] = (UINT8) rtcAlarm->Hour;
    	tx->Payload[offset++] = (UINT8) rtcAlarm->Minutes;
    	tx->Payload[offset++] = (UINT8) rtcAlarm->Seconds;
    	*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
    					DEVMGMT_MSG_SET_RTC_ALARM_RSP,
    					&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
    					sizeof(TWiMODLR_DevMgmt_RtcAlarm));
#else
		tx->Payload[0] = RTCAlarm.AlarmStatus;
		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
		DEVMGMT_MSG_SET_RTC_ALARM_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
				1);
#endif
		result = WiMODLR_RESULT_OK;
    }
    else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
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
    	TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
#if 0
    	tx->Payload[offset++] = *statusRsp;
		tx->Payload[offset++] = rtcAlarm->AlarmStatus;
		tx->Payload[offset++] = rtcAlarm->Options;
		tx->Payload[offset++] = rtcAlarm->Hour;
		tx->Payload[offset++] = rtcAlarm->Minutes;
		tx->Payload[offset++] = rtcAlarm->Seconds;
#else
		RTC_AlarmTypeDef sAlarm;
		HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
		tx->Payload[offset++] = DeviceInfo.Status;
		tx->Payload[offset++] = rtcAlarm->AlarmStatus = RTCAlarm.AlarmStatus;
		tx->Payload[offset++] = rtcAlarm->Options = RTCAlarm.Options;
		tx->Payload[offset++] = rtcAlarm->Hour = RTCAlarm.Hour = sAlarm.AlarmTime.Hours;
		tx->Payload[offset++] = rtcAlarm->Minutes = RTCAlarm.Minutes = sAlarm.AlarmTime.Minutes;
		tx->Payload[offset++] = rtcAlarm->Seconds = RTCAlarm.Seconds = sAlarm.AlarmTime.Seconds;
#endif
		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID,
				DEVMGMT_MSG_GET_RTC_ALARM_RSP,
				&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS],
				sizeof(TWiMODLR_DevMgmt_RtcAlarm) + 1);
		result = WiMODLR_RESULT_OK;
    }
    else {
        result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }
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
    	TWiMODLR_HCIMessage* tx = &WiMOD_SAP_DevMgmt.HciParser->TxMessage;
    	tx->Payload[0] = 0x55;
		*statusRsp = WiMOD_SAP_DevMgmt.HciParser->PostMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_CLEAR_RTC_ALARM_RSP, &tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);
		result = WiMODLR_RESULT_OK;
	}
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
    UINT8              offset = 0;

    if (hciConfig && statusRsp && (WiMOD_SAP_DevMgmt.txyPayloadSize >= 0x06)) {
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->StoreNwmFlag;
    	WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->BaudrateID;
        HTON16(&WiMOD_SAP_DevMgmt.txPayload[offset], hciConfig->NumWakeUpChars);
        offset += 0x02;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->TxHoldTime;
        WiMOD_SAP_DevMgmt.txPayload[offset++] = (UINT8) hciConfig->RxHoldTime;

        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                                        DEVMGMT_MSG_SET_HCI_CFG_REQ,
                                        DEVMGMT_MSG_SET_HCI_CFG_RSP,
										WiMOD_SAP_DevMgmt.txPayload, offset);

        if (result == WiMODLR_RESULT_OK) {
            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();
            offset = WiMODLR_HCI_RSP_STATUS_POS;
            *statusRsp = rx->Payload[offset++];

            // status check
            if (*statusRsp == DEVMGMT_STATUS_OK) {


            } else {
            }
        }
    }
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
    UINT8              offset = WiMODLR_HCI_RSP_STATUS_POS;

    if (hciConfig && statusRsp) {
        result = WiMOD_SAP_DevMgmt.HciParser->SendHCIMessage(DEVMGMT_SAP_ID,
                                           DEVMGMT_MSG_GET_HCI_CFG_REQ,
                                           DEVMGMT_MSG_GET_HCI_CFG_RSP,
                                           NULL, 0);

        if (result == WiMODLR_RESULT_OK) {
            const TWiMODLR_HCIMessage* rx = WiMOD_SAP_DevMgmt.HciParser->GetRxMessage();

            hciConfig->Status         = rx->Payload[offset++];
            hciConfig->BaudrateID     = (TWiMOD_HCI_Baudrate) rx->Payload[offset++];
            hciConfig->NumWakeUpChars = NTOH16(&rx->Payload[offset]);
            offset += 0x02;
            hciConfig->TxHoldTime     = (UINT8)rx->Payload[offset++];
            hciConfig->RxHoldTime     = (UINT8)rx->Payload[offset++];

            *statusRsp = rx->Payload[WiMODLR_HCI_RSP_STATUS_POS];
        }
    }
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
        	WiMOD_SAP_DevMgmt.SetRtc(NTOH32(rxMsg->Payload[0]), statusRsp);
        	break;
        case DEVMGMT_MSG_GET_RTC_REQ:
        	WiMOD_SAP_DevMgmt.GetRtc(&RTCTime, statusRsp);
        	break;
        case DEVMGMT_MSG_GET_SYSTEM_STATUS_REQ:
        	WiMOD_SAP_DevMgmt.GetSystemStatus(&SystemInfo, statusRsp);
        	break;
        case DEVMGMT_MSG_SET_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.SetRtcAlarm((TWiMODLR_DevMgmt_RtcAlarm*) rxMsg->Payload[0], statusRsp);
        	break;
        case DEVMGMT_MSG_CLEAR_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.ClearRtcAlarm(statusRsp);
        	break;
        case DEVMGMT_MSG_GET_RTC_ALARM_REQ:
        	WiMOD_SAP_DevMgmt.GetRtcAlarm((TWiMODLR_DevMgmt_RtcAlarm* ) rxMsg->Payload[0], statusRsp);
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


