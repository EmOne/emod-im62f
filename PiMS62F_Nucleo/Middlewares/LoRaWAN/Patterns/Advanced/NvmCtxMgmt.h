/*!
 * \file      NvmCtxMgmt.h
 *
 * \brief     NVM context management implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * \defgroup  NVMCTXMGMT NVM context management implementation
 *            This module implements the NVM context handling
 * \{
 */
#ifndef __NVMCTXMGMT_H__
#define __NVMCTXMGMT_H__

#include "LoRaMac.h"


typedef uint16_t NvmmDataBlock_t;

/*!
 * Data structure containing the status of a operation
 */
typedef enum NvmCtxMgmtStatus_e
{
    /*!
     * Operation was successful
     */
    NVMCTXMGMT_STATUS_SUCCESS = 0,
	/*!
	 * Crypto Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_CRYPTO = 0x01,
	/*!
	 * Secure Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_SECURE = 0x02,
	/*!
	 * Mac Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_MAC = 0x04,
	/*!
	 * Region Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_REGION = 0x08,
	/*!
	 * Command Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_CMD = 0x10,
	/*!
	 * CLASS B Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_CLASS_B = 0x20,
	/*!
	 * Confirm queue Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_CONFIRM_Q = 0x40,
	/*!
	 * User setting was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL_USER = 0x80,
	/*!
	 * Operation was not successful
	 */
	NVMCTXMGMT_STATUS_FAIL = 0xFF,
}NvmCtxMgmtStatus_t;

/*!
 * \brief Calculates the next datarate to set, when ADR is on or off.
 *
 * \param [IN] adrNext Pointer to the function parameters.
 *
 */
void NvmCtxMgmtEvent( LoRaMacNvmCtxModule_t module );

/*!
 * \brief Function which stores the MAC data into NVM, if required.
 *
 * \retval Number of bytes which were stored.
 */
NvmCtxMgmtStatus_t NvmCtxMgmtStore( void );

/*!
 * \brief Function which restores the MAC data from NVM, if required.
 *
 * \retval Number of bytes which were restored.
 */
NvmCtxMgmtStatus_t NvmCtxMgmtRestore(void );

/*!
 * \brief Resets the NVM data.
 *
 * \retval Returns Number of bytes, if successful.
 */
NvmCtxMgmtStatus_t NvmDataMgmtFactoryReset( void );

#endif // __NVMCTXMGMT_H__
