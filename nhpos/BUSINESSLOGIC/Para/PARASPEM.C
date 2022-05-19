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
* Title       : Parameter ParaSpeMnemo Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASPEM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSpeMnemoRead()  : reads SPECIAL MNEMONICS
*               ParaSpeMnemoWrite() : sets SPECIAL MNEMONICS
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaSpeMnemoRead( PARASPEMNEMO *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_SPEMNEMO_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads SPECIAL MNEMONICS of appointed address.
;===============================================================================
**/

VOID ParaSpeMnemoRead( PARASPEMNEMO *pData )
{
    USHORT    i = (pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

	if (i < MAX_SPEM_NO ) {   // out of bounds guard, ParaSpeMnemoRead( PARASPEMNEMO *pData )
		_tcsncpy(pData->aszMnemonics, Para.ParaSpeMnemo[i], PARA_SPEMNEMO_LEN);
		pData->aszMnemonics[PARA_SPEMNEMO_LEN] = '\0';
	}
	else{
		NHPOS_ASSERT(pData->uchAddress > 0);
		NHPOS_ASSERT(pData->uchAddress <= MAX_SPEM_NO);
		pData->aszMnemonics[0] = 0;
	}
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSpeMnemoWrite( PARASPEMNEMO *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_SPEMNEMO_LEN+1]
;      output:  Nothing
;       
;      Return:  Nothing
;
; Descruption:  This function sets SPECIAL MNEMONICS in appointed address.
;===============================================================================
**/

VOID ParaSpeMnemoWrite( PARASPEMNEMO *pData )
{
    USHORT    i = (pData->uchAddress - 1);   /* "-1" fits program address to RAM address */

	if (i < MAX_SPEM_NO ) {   // out of bounds guard, ParaSpeMnemoWrite( PARASPEMNEMO *pData )
		_tcsncpy(Para.ParaSpeMnemo[i], pData->aszMnemonics, PARA_SPEMNEMO_LEN);
	} else {
		NHPOS_ASSERT(pData->uchAddress > 0);
		NHPOS_ASSERT(pData->uchAddress <= MAX_SPEM_NO);
	}
}
