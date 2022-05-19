/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaTransMnemo Module
* Category    : User Interface For Supervisor, NCR 2170 System Application
* Program Name: PARATRAN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               ParaTransMnemoRead()  : reads TRANSACTION MNEMONICS
*               ParaTransMnemoWrite() : sets TRANSACTION MNEMONICS
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
==============================================================================
;                      I N C L U D E     F I L E s
;=============================================================================
**/

#include	<tchar.h>
#include	<string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaTransMnemoRead( PARATRANSMNEMO *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_TRANSMNEMO_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads TRANSACTION MNEMONICS of appointed address.
;===============================================================================
**/
VOID ParaTransMnemoRead( PARATRANSMNEMO *pData )
{
    USHORT    i = ( USHORT)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */

	if( i < MAX_TRANSM_NO ) {   // out of bounds guard, ParaTransMnemoRead( PARATRANSMNEMO *pData )
		_tcsncpy(pData->aszMnemonics, Para.ParaTransMnemo[i], PARA_TRANSMNEMO_LEN );
		pData->aszMnemonics[PARA_TRANSMNEMO_LEN] = '\0';    // ensure zero termination of string.
	}
	else{
		memset(pData->aszMnemonics, 0x00, PARA_TRANSMNEMO_LEN * sizeof(pData->aszMnemonics[0]));
	}
}

/**
;=============================================================================
;    Synopsis:  VOID ParaTransMnemoWrite( PARATRANSMNEMO *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_TRANSMNEMO_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets TRANSACTION MNEMONICS in appointed address.
;===============================================================================
**/

VOID ParaTransMnemoWrite( PARATRANSMNEMO *pData )
{
    USHORT    i = ( USHORT)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */

	if (i < MAX_TRANSM_NO ) {   // out of bounds guard, ParaTransMnemoWrite( PARATRANSMNEMO *pData )
		_tcsncpy( Para.ParaTransMnemo[i], pData->aszMnemonics, PARA_TRANSMNEMO_LEN );
	}
}
