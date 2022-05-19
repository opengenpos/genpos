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
* Title       : Parameter ParaRound Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARARND.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaRoundRead()  : reads ROUNDING DATA
*               ParaRoundWrite() : sets ROUNDING DATA
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
*          :           :                                    
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

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaRoundRead( PARAROUNDTBL *pData )
;       input:  pData->uchAddress
;      output:  pData->uchRoundDelimit
;               pData->uchRoundModules
;               pData->chRoundPosition
;
;      Return:  Nothing
;
; Descruption:  This function reads ROUNDING DATA.
;===============================================================================
**/

VOID ParaRoundRead(PARAROUNDTBL *pData )
{
    UCHAR    i = pData->uchAddress;              
    
    /* calculate for fitting program address to ram address */
    i = ( UCHAR)((i - 1) / 2); /* in case of even number, convert into (even address - 1) */

    pData->uchRoundDelimit = ParaRound[i][0];
    pData->uchRoundModules = ParaRound[i][1];
    pData->chRoundPosition  = ParaRound[i][2];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaRoundWrite( PARAROUNDTBL *pData )
;       input:  pData->uchAddress
;               pData->uchRoundDelimit
;               pData->uchRoundModules
;                          or
;               pData->chRoundPosition
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets ROUNDING DATA.
;===============================================================================
**/

VOID ParaRoundWrite( CONST PARAROUNDTBL *pData )
{
    UCHAR    i = pData->uchAddress;              

    /* calculate for fitting program address to ram address */
    i = ( UCHAR)((i - 1) / 2); /* in case of even number, convert into (even address - 1) */
	if (i < MAX_RNDTBL_SIZE) {
		ParaRound[i][0] = pData->uchRoundDelimit;
		ParaRound[i][1] = pData->uchRoundModules;
		ParaRound[i][2] = pData->chRoundPosition;
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_RNDTBL_SIZE), "==ERROR: ParaRoundWrite() address out of bounds.");
	}
}
