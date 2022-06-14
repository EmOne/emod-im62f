/*!
 * \file      NvmCtxMgmt.c
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
 */

#include <stdio.h>
#include "nvmm.h"
#include "NvmCtxMgmt.h"
#include "utilities.h"



/*!
 * Enables/Disables the context storage management storage at all. Must be enabled for LoRaWAN 1.1.x.
 * WARNING: Still under development and not tested yet.
 */
#define CONTEXT_MANAGEMENT_ENABLED         1

/*!
 * Enables/Disables maximum persistent context storage management. All module contexts will be saved on a non-volatile memory.
 * WARNING: Still under development and not tested yet.
 */
#define MAX_PERSISTENT_CTX_MGMT_ENABLED    1

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
#define NVM_CTX_STORAGE_MASK               0xFF
#else
#define NVM_CTX_STORAGE_MASK               0x8C
#endif

#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
/*!
 * LoRaMAC Structure holding contexts changed status
 * in case of a \ref MLME_NVM_CTXS_UPDATE indication.
 */
typedef union uLoRaMacCtxsUpdateInfo
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * The according context bit will be set to one
     * if the context changed or 0 otherwise.
     */
    struct sElements
    {
        /*!
         * Mac core nvm context
         */
        uint8_t Mac : 1;
        /*!
         * Region module nvm contexts
         */
        uint8_t Region : 1;
        /*!
         * Cryto module context
         */
        uint8_t Crypto : 1;
        /*!
         * Secure Element driver context
         */
        uint8_t SecureElement : 1;
        /*!
         * MAC commands module context
         */
        uint8_t Commands : 1;
        /*!
         * Class B module context
         */
        uint8_t ClassB : 1;
        /*!
         * Confirm queue module context
         */
        uint8_t ConfirmQueue : 1;
        /*!
		 * Confirm queue module context
		 */
		uint8_t UserSetting : 1;
        /*!
         * FCnt Handler module context
         */
        uint8_t FCntHandlerNvmCtx : 1;
    }Elements;
}LoRaMacCtxUpdateStatus_t;

LoRaMacCtxUpdateStatus_t CtxUpdateStatus = { .Value = 0 };

/*
 * Nvmm handles
 */
static NvmmDataBlock_t SecureElementNvmCtxDataBlock = 0x0000;
static NvmmDataBlock_t CryptoNvmCtxDataBlock 		= 0x0100;
#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
static NvmmDataBlock_t MacNvmCtxDataBlock 			= 0x0200;
static NvmmDataBlock_t RegionNvmCtxDataBlock 		= 0x0300;
static NvmmDataBlock_t CommandsNvmCtxDataBlock 		= 0x0400;
static NvmmDataBlock_t ConfirmQueueNvmCtxDataBlock 	= 0x0500;
static NvmmDataBlock_t ClassBNvmCtxDataBlock 		= 0x0600;
#endif
static NvmmDataBlock_t UsersettingNvmCtxDataBlock 	= 0x0700;
#endif

void NvmCtxMgmtEvent( LoRaMacNvmCtxModule_t module )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    switch( module )
    {
        case LORAMAC_NVMCTXMODULE_MAC:
        {
            CtxUpdateStatus.Elements.Mac = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_REGION:
        {
            CtxUpdateStatus.Elements.Region = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CRYPTO:
        {
            CtxUpdateStatus.Elements.Crypto = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_SECURE_ELEMENT:
        {
            CtxUpdateStatus.Elements.SecureElement = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_COMMANDS:
        {
            CtxUpdateStatus.Elements.Commands = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CLASS_B:
        {
            CtxUpdateStatus.Elements.ClassB = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE:
        {
            CtxUpdateStatus.Elements.ConfirmQueue = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_USER_SETTING:
		{
			CtxUpdateStatus.Elements.UserSetting = 1;
			break;
		}
        default:
        {
            break;
        }
    }
#endif
}

NvmCtxMgmtStatus_t NvmCtxMgmtStore( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    // Read out the contexts lengths and pointers
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );
    LoRaMacCtxs_t* MacContexts = mibReq.Param.Contexts;

    // Input checks
    if( ( CtxUpdateStatus.Value & NVM_CTX_STORAGE_MASK ) == 0 )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
    if( LoRaMacStop( ) != LORAMAC_STATUS_OK )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }

    // Write
    if( CtxUpdateStatus.Elements.Crypto == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->CryptoNvmCtx, MacContexts->CryptoNvmCtxSize, CryptoNvmCtxDataBlock) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_CRYPTO;
        }
    }

    if( CtxUpdateStatus.Elements.SecureElement == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->SecureElementNvmCtx, MacContexts->SecureElementNvmCtxSize, SecureElementNvmCtxDataBlock ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_SECURE;
        }
    }

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    if( CtxUpdateStatus.Elements.Mac == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->MacNvmCtx, MacContexts->MacNvmCtxSize, MacNvmCtxDataBlock ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_MAC;
        }
    }

    if( CtxUpdateStatus.Elements.Region == 1 )
    {
        if( NvmmWrite( (uint8_t*)  MacContexts->RegionNvmCtx, MacContexts->RegionNvmCtxSize, RegionNvmCtxDataBlock ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_REGION;
        }
    }

    if( CtxUpdateStatus.Elements.Commands == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->CommandsNvmCtx, MacContexts->CommandsNvmCtxSize, CommandsNvmCtxDataBlock) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_CMD;
        }
    }

    if( CtxUpdateStatus.Elements.ClassB == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->ClassBNvmCtx, MacContexts->ClassBNvmCtxSize, ClassBNvmCtxDataBlock ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_CLASS_B;
        }
    }

    if( CtxUpdateStatus.Elements.ConfirmQueue == 1 )
    {
        if( NvmmWrite( (uint8_t*) MacContexts->ConfirmQueueNvmCtx, MacContexts->ConfirmQueueNvmCtxSize, ConfirmQueueNvmCtxDataBlock ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL_CONFIRM_Q;
        }
    }

    //TODO: USER data store
    if( NvmmWrite( (uint8_t*) MacContexts->UserSettingNvmCtx, MacContexts->UserSettingNvmCtxSize, UsersettingNvmCtxDataBlock ) != NVMM_SUCCESS )
	{
		return NVMCTXMGMT_STATUS_FAIL_USER;
	}

#endif

    CtxUpdateStatus.Value = 0x00;

    // Resume LoRaMac
    LoRaMacStart( );

    return NVMCTXMGMT_STATUS_SUCCESS;
#else
    return NVMCTXMGMT_STATUS_FAIL;
#endif
}

NvmCtxMgmtStatus_t NvmCtxMgmtRestore( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    MibRequestConfirm_t mibReq;
    LoRaMacCtxs_t contexts = { 0 };
    NvmCtxMgmtStatus_t status = NVMCTXMGMT_STATUS_SUCCESS;

    // Read out the contexts lengths
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );


    uint8_t NvmCryptoCtxRestore[mibReq.Param.Contexts->CryptoNvmCtxSize];
    uint8_t NvmSecureElementCtxRestore[mibReq.Param.Contexts->SecureElementNvmCtxSize];
#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    uint8_t NvmMacCtxRestore[mibReq.Param.Contexts->MacNvmCtxSize];
    uint8_t NvmRegionCtxRestore[mibReq.Param.Contexts->RegionNvmCtxSize];
    uint8_t NvmCommandsCtxRestore[mibReq.Param.Contexts->CommandsNvmCtxSize];
    uint8_t NvmClassBCtxRestore[mibReq.Param.Contexts->ClassBNvmCtxSize];
    uint8_t NvmConfirmQueueCtxRestore[mibReq.Param.Contexts->ConfirmQueueNvmCtxSize];
#endif
    uint8_t NvmUserSettingCtxRestore[mibReq.Param.Contexts->UserSettingNvmCtxSize];

    if ( NvmmDeclare( mibReq.Param.Contexts->CryptoNvmCtxSize, CryptoNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmCryptoCtxRestore, mibReq.Param.Contexts->CryptoNvmCtxSize, CryptoNvmCtxDataBlock );
        contexts.CryptoNvmCtx = &NvmCryptoCtxRestore;
        contexts.CryptoNvmCtxSize = mibReq.Param.Contexts->CryptoNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare(  mibReq.Param.Contexts->SecureElementNvmCtxSize, SecureElementNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmSecureElementCtxRestore, mibReq.Param.Contexts->SecureElementNvmCtxSize, SecureElementNvmCtxDataBlock );
        contexts.SecureElementNvmCtx = &NvmSecureElementCtxRestore;
        contexts.SecureElementNvmCtxSize = mibReq.Param.Contexts->SecureElementNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    if( NvmmDeclare( mibReq.Param.Contexts->MacNvmCtxSize, MacNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmMacCtxRestore, mibReq.Param.Contexts->MacNvmCtxSize, MacNvmCtxDataBlock );
        contexts.MacNvmCtx = &NvmMacCtxRestore;
        contexts.MacNvmCtxSize = mibReq.Param.Contexts->MacNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( mibReq.Param.Contexts->RegionNvmCtxSize, RegionNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead(  NvmRegionCtxRestore, mibReq.Param.Contexts->RegionNvmCtxSize, RegionNvmCtxDataBlock );
        contexts.RegionNvmCtx = &NvmRegionCtxRestore;
        contexts.RegionNvmCtxSize = mibReq.Param.Contexts->RegionNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( mibReq.Param.Contexts->CommandsNvmCtxSize, CommandsNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmCommandsCtxRestore, mibReq.Param.Contexts->CommandsNvmCtxSize, CommandsNvmCtxDataBlock );
        contexts.CommandsNvmCtx = &NvmCommandsCtxRestore;
        contexts.CommandsNvmCtxSize = mibReq.Param.Contexts->CommandsNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare(mibReq.Param.Contexts->ClassBNvmCtxSize, ClassBNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmClassBCtxRestore, mibReq.Param.Contexts->ClassBNvmCtxSize, ClassBNvmCtxDataBlock );
        contexts.ClassBNvmCtx = &NvmClassBCtxRestore;
        contexts.ClassBNvmCtxSize = mibReq.Param.Contexts->ClassBNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( mibReq.Param.Contexts->ConfirmQueueNvmCtxSize, ConfirmQueueNvmCtxDataBlock ) == NVMM_SUCCESS )
    {
        NvmmRead( NvmConfirmQueueCtxRestore, mibReq.Param.Contexts->ConfirmQueueNvmCtxSize, ConfirmQueueNvmCtxDataBlock );
        contexts.ConfirmQueueNvmCtx = &NvmConfirmQueueCtxRestore;
        contexts.ConfirmQueueNvmCtxSize = mibReq.Param.Contexts->ConfirmQueueNvmCtxSize;
    }
    else
    {
        status |= NVMCTXMGMT_STATUS_FAIL;
    }
#endif

    //TODO: User setting
    if ( NvmmDeclare( mibReq.Param.Contexts->UserSettingNvmCtxSize, UsersettingNvmCtxDataBlock ) == NVMM_SUCCESS )
	{
		NvmmRead(NvmUserSettingCtxRestore, mibReq.Param.Contexts->UserSettingNvmCtxSize, UsersettingNvmCtxDataBlock );
		contexts.UserSettingNvmCtx = &NvmUserSettingCtxRestore;
		contexts.UserSettingNvmCtxSize = mibReq.Param.Contexts->UserSettingNvmCtxSize;
	}
	else
	{
		status |= NVMCTXMGMT_STATUS_FAIL;
	}

    // Enforce storing all contexts
    if( status & NVMCTXMGMT_STATUS_FAIL )
    {
        CtxUpdateStatus.Value = 0xFF;
        NvmCtxMgmtStore( );
    }
    else
    {  // If successful query the mac to restore contexts
        mibReq.Type = MIB_NVM_CTXS;
        mibReq.Param.Contexts = &contexts;
        LoRaMacMibSetRequestConfirm( &mibReq );
    }

    return status;
#else
    return NVMCTXMGMT_STATUS_FAIL;
#endif
}

NvmCtxMgmtStatus_t NvmDataMgmtFactoryReset( void )
{
//    uint16_t offset = 0;
    MibRequestConfirm_t mibReq;

    // Read out the contexts lengths
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );

    LoRaMacCtxs_t *contexts = mibReq.Param.Contexts;


#if( CONTEXT_MANAGEMENT_ENABLED == 1 )
    // Crypto
    if( NvmmReset( contexts->CryptoNvmCtxSize, CryptoNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->CryptoNvmCtxSize;

    // Secure
    if( NvmmReset( contexts->SecureElementNvmCtxSize, SecureElementNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->SecureElementNvmCtxSize;

    // Mac
    if( NvmmReset( contexts->MacNvmCtxSize, MacNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->MacNvmCtxSize;

    // Region
    if( NvmmReset( contexts->RegionNvmCtxSize, RegionNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->RegionNvmCtxSize;

    // Command
    if( NvmmReset( contexts->CommandsNvmCtxSize, CommandsNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->CommandsNvmCtxSize;

    // Class B
    if( NvmmReset( contexts->ClassBNvmCtxSize, ClassBNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->ClassBNvmCtxSize;

    // Confirm
    if( NvmmReset( contexts->ConfirmQueueNvmCtxSize, ConfirmQueueNvmCtxDataBlock ) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
//    offset += contexts->ConfirmQueueNvmCtxSize;

    // Confirm
	if( NvmmReset( contexts->UserSettingNvmCtxSize, UsersettingNvmCtxDataBlock ) == false )
	{
		return NVMCTXMGMT_STATUS_FAIL;
	}
//	offset += contexts->UserSettingNvmCtxSize;

    if( NvmmReset( 8U, 0x0FF8) == false )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }

#endif
    return NVMCTXMGMT_STATUS_SUCCESS;
}
