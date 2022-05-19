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
* Title       : Parameter ParaMajorDEPT Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAMDEP.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaMajorDEPTRead()  : reads MAJOR DEPARTMENT MNEMONICS
*               ParaMajorDEPTWrite() : sets MAJOR DEPARTMENT MNEMONICS
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
* 2171 for Win32
* Aug-26-99:  01.00.00 : K.Iwata  : V1.0 Initial
*
** GenPOS **
* Feb-13-18: 02.02.02 : R.Chambers  : add out of bounds check to write.
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
#include <string.h>

#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaMajorDEPTRead( PARAMAJORDEPT *pData )
;       input:  pData->uchAddress
;      output:  pData->aszMnemonics[PARA_MAJORDEPT_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function reads MAJOR DEPARTMENT MNEMONICS of appointed address.
;===============================================================================
**/
VOID ParaMajorDEPTRead( PARAMAJORDEPT *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

    _tcsncpy(pData->aszMnemonics, ParaMajorDEPT[i], PARA_MAJORDEPT_LEN);
    pData->aszMnemonics[PARA_MAJORDEPT_LEN] = '\0';
}

/**
;=============================================================================
;    Synopsis:  VOID ParaMajorDEPTWrite( PARAMAJORDEPT *pData )
;       input:  pData->uchAddress
;               pData->aszMnemonics[PARA_MAJORDEPT_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets MAJOR DEPARTMENT MNEMONICS in appointed address.
;===============================================================================
**/

VOID ParaMajorDEPTWrite( PARAMAJORDEPT *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

	if (i < MAX_MDEPT_NO) {
		_tcsncpy(ParaMajorDEPT[i], pData->aszMnemonics, PARA_MAJORDEPT_LEN);
	} else {
		NHPOS_ASSERT(i < MAX_MDEPT_NO);
	}
}
