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
* Title       : Parameter ParaPrtModi Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAPRTM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaPrtModiRead()  : reads PRINT MODIFIER MNEMONICS
*               ParaPrtModiWrite() : sets PRINT MODIFIER MNEMONICS
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
#include	<stdio.h>
#include	<string.h>

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;
;    Synopsis:  VOID ParaPrtModiRead( PARAPRTMODI *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_PRTMODI_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads PRINT MODIFIER MNEMONICS of appointed address.
;               
;===============================================================================
**/

VOID ParaPrtModiRead( PARAPRTMODI *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

    _tcsncpy(pData->aszMnemonics, ParaPrtModi[i], PARA_PRTMODI_LEN);
    pData->aszMnemonics[PARA_PRTMODI_LEN] = '\0';
}

/**
;=============================================================================
;
;    Synopsis:  VOID ParaPrtModiWrite( PARAPRTMODI *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_PRTMODI_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function set PRINT MODIFIER MNEMONICS in appointed address.
;               
;===============================================================================
**/

VOID ParaPrtModiWrite( CONST PARAPRTMODI *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

	if (i < MAX_PRTMODI_NO) {
		_tcsncpy(ParaPrtModi[i], pData->aszMnemonics, PARA_PRTMODI_LEN);
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_PRTMODI_NO), "==ERROR: ParaPrtModiWrite() address out of bounds.");
	}
}
