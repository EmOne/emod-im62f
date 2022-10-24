//------------------------------------------------------------------------------
//! @file WiMOD_SAP_Generic.h
//! @ingroup WIMOD_SAP_Generic
//! <!------------------------------------------------------------------------->
//! @brief Generic HCI commmand interface
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
//!
//! </PRE>
//------------------------------------------------------------------------------


#ifndef SAP_WIMOD_SAP_HWTEST_H_
#define SAP_WIMOD_SAP_HWTEST_H_


//------------------------------------------------------------------------------
//
// Section Includes Files
//
//------------------------------------------------------------------------------

#include "../SAP/WiMOD_SAP_HWTEST_IDs.h"
#include "../HCI/WiMODLRHCI.h"
#include "utilities_def.h"
#include "stm32_seq.h"
#include "LmHandler.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "NvmDataMgmt.h"
#include "stm32_lpm.h"
#include "FreqCalc.h"
/*
 * C++11 supports a better way for function pointers / function objects
 * But C++11 mode is not supported by all platforms.
 */
#ifdef WIMOD_USE_CPP11
#include <functional>
#endif

//------------------------------------------------------------------------------
//
// Section defines
//
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//
// types for callback functions
//
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
// Section class
//
//------------------------------------------------------------------------------

/**
 * @brief Implementation for the ServiceAccessPoint DeviceManagement
 */
typedef struct {
//public:
//    WiMOD_SAP_Generic(TWiMODLRHCI* hci, UINT8* buffer, UINT16 bufferSize);
//    ~WiMOD_SAP_Generic(void);

    TWiMODLRResultCodes (*ExecuteHwTestCmd)(TWiMODLR_HwTest_RadioTestInfo* info, UINT8* statusRsp);
    void (*Process)(UINT8* statusRsp, TWiMODLR_HCIMessage* rxMsg);
//protected:
//
//private:
    //! @cond Doxygen_Suppress
    UINT8*              txPayload;
    UINT16              txyPayloadSize;

    TWiMODLRHCI_t*       HciParser;
    //! @endcond
}WiMOD_SAP_HwTest_t;

extern WiMOD_SAP_HwTest_t WiMOD_SAP_HwTest;





#endif /* SAP_WIMOD_SAP_HWTEST_H_ */
