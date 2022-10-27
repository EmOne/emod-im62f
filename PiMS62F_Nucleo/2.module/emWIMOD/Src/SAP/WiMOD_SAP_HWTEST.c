//------------------------------------------------------------------------------
//! @file WiMOD_SAP_HWTEST.c
//! @ingroup WIMOD_SAP_Generic
//! <!------------------------------------------------------------------------->
//! @brief Hardware test HCI commmand interface
//! @version 0.1
//! <!------------------------------------------------------------------------->
//!
//!
//!
//! <!--------------------------------------------------------------------------
//! Copyright (c) 2017
//! IMST GmbH
//! Carl-Friedrich Gauss Str. 2
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
//! 0.1.0b-RC2 | 20221023 | Anol P. <anol.p@emone.co.th> | N/A
//!
//! </PRE>
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
// Section Includes Files
//
//------------------------------------------------------------------------------

#include <string.h>
#include "../SAP/WiMOD_SAP_HWTEST.h"

//------------------------------------------------------------------------------
//
// Section public functions
//
//------------------------------------------------------------------------------
static TWiMODLRResultCodes ExecuteHwTestCmd(TWiMODLR_HwTest_RadioTestInfo* info, UINT8* statusRsp);
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
WiMOD_SAP_HwTest_t WiMOD_SAP_HwTest = {
		ExecuteHwTestCmd,
		process,
		NULL,
		0x00,
};

////-----------------------------------------------------------------------------
///**
// * @brief Destructor
// */
//WiMOD_SAP_Generic::~WiMOD_SAP_Generic(void)
//{
//
//}


//-----------------------------------------------------------------------------
/**
 * @brief Reset Cmd - Do a reset / reboot of the WiMOD
 *
 *
 * @param   info        pointer to store the received information
 * @param   statusRsp   pointer to store status byte of response mesg from WiMOD
 *
 * @retval WiMODLR_RESULT_OK     if command transmit to WiMOD was ok
 */
TWiMODLRResultCodes ExecuteHwTestCmd(TWiMODLR_HwTest_RadioTestInfo* info, UINT8* statusRsp)
{
    TWiMODLRResultCodes result = WiMODLR_RESULT_TRANMIT_ERROR;

    if (info && statusRsp) {

    	switch (info->TestMode) {
			case 0x1:
				LmHandlerStop();
				result = HWTEST_STATUS_OK;
				break;
			case 0x2:
				UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
				LmHandlerStartTXCW(info->Timeout);
				result = HWTEST_STATUS_OK;
				break;
			case 0x3:
				UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
				LmHandlerStartTXCW1(info->PowerLevel,
						FreqCalc_calcRegisterToFreq(info->FreqLSB, info->FreqMID, info->FreqMSB),
						info->Timeout);
				break;
			default:
				result = HWTEST_STATUS_WRONG_PARAMETER;
				break;
		}
    } else {
    	result = WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    TWiMODLR_HCIMessage *tx = &WiMOD_SAP_HwTest.HciParser->TxMessage;
	tx->Payload[0] = result; //DeviceInfo.Status;

	*statusRsp = WiMOD_SAP_HwTest.HciParser->PostMessage(HWTEST_SAP_ID,
			HWTEST_MSG_RADIO_TEST_RSP,
			&tx->Payload[WiMODLR_HCI_RSP_STATUS_POS], 1);

	UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_ENABLE);
    return result;
}

/**

 * @brief process messages from the User
 *
 * @param rxMsg reference to the complete received HCI message; DO NOT MODIFIY it!

 */
static void process (UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg)
{
	 switch (rxMsg->MsgID)
	    {
	        case HWTEST_MSG_RADIO_TEST_REQ:
	        	WiMOD_SAP_HwTest.ExecuteHwTestCmd((TWiMODLR_HwTest_RadioTestInfo*)&rxMsg->Payload[0], statusRsp);
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


//------------------------------------------------------------------------------
//
// Section private functions
//
//------------------------------------------------------------------------------
