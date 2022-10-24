//------------------------------------------------------------------------------
//! @file WiMOD_SAP_HWTEST_IDs.h
//! @ingroup WiMOD_SAP_HWTEST
//! <!------------------------------------------------------------------------->
//! @brief Supporting IDs and definitions for the hardware testing ServiceAccessPoint
//! @version 0.1
//! <!------------------------------------------------------------------------->
//!
//! <!--------------------------------------------------------------------------
//! Copyright (c) 2022 EmOne
//! --------------------------------------------------------------------------->
//! @author (FB), EmIne
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
//! </PRE>
//------------------------------------------------------------------------------


/*
 * THIS IS AN EXAMPLE IMPLEMENTATION ACCORDING THE THE HCI SPEC: V1.8
 * FOR FIRMWARE: LR-BASE
 *
 * SEE FILE: WiMOD LoRaWAN EndNode Modem Test Mode HCI Specification.docx for detailed information
 */


#ifndef ARDUINO_WIMOD_SAP_HWTEST_IDS_H_
#define ARDUINO_WIMOD_SAP_HWTEST_IDS_H_

//------------------------------------------------------------------------------
//
// Section Includes Files
//
//------------------------------------------------------------------------------

#include "../utils/WMDefs.h"


//------------------------------------------------------------------------------
//
// Service Access Point Identifier
//
//------------------------------------------------------------------------------

/** SAP ID for the HwTest service */
#define HWTEST_SAP_ID                      0xA1

//------------------------------------------------------------------------------
//
//  Device Management Message Identifier
//
//------------------------------------------------------------------------------
//! @cond Doxygen_Suppress

// Status Codes
#define HWTEST_STATUS_OK                   0x00
#define HWTEST_STATUS_ERROR                0x01
#define HWTEST_STATUS_CMD_NOT_SUPPORTED    0x02
#define HWTEST_STATUS_WRONG_PARAMETER      0x03

// Message IDs
#define HWTEST_MSG_RADIO_TEST_REQ                0x01
#define HWTEST_MSG_RADIO_TEST_RSP                0x02

//! @endcond

//------------------------------------------------------------------------------
//
// structures
//
//------------------------------------------------------------------------------
/**
 * @brief Structure containing basic information about the WiMOD device
 */
typedef struct TWiMODLR_HwTest_RadioTestInfo
{
    UINT8       TestMode;                                                     /*!< type test mode 0: test off 1: CW 2: Pseudo Noise Test (PN9) */
    UINT8      	PowerLevel;                                                   /*!< the power level 0-5:5 dBm ... 22:22dBm */
    UINT8       Modulation;                                                   /*!< the modulation 0:LoRA 1:FSK */
    UINT8       FreqLSB;                                                      /*!< the Freq LSB of the LoRa */
    UINT8       FreqMID;                                                      /*!< the Freq MID of the LoRa */
    UINT8       FreqMSB;                                                      /*!< the Freq MSB of the LoRa */
    UINT8      	BandWidth;                                                    /*!< the band width 0:125kHz 0:250kHz 0:500kHz of the WiMOD */
    UINT8      	DataRate;                                                     /*!< the spreading factor 0-5:SF5 6:SF6 7:SF7 8:SF8 9:SF9  10:SF10 11:SF11 12:SF12 of the WiMOD */
    UINT8      	ErrorCoding;                                                  /*!< the error coding 0:4/5 1:4/5 2:4/6 3:4/7 4:4/8 of the LoRa */
    UINT16		Timeout;													  /*!< the testing timeout (ms) */
} TWiMODLR_HwTest_RadioTestInfo;
// @endcond

#endif /* ARDUINO_WIMOD_SAP_HWTEST_IDS_H_ */
