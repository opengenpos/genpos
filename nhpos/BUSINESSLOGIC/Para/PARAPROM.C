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
* Title       : Parameter ParaPromotion Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAPROM.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaPromotionRead()  : reads PROMOTION SALES HEADER
*               ParaPromotionWrite() : sets PROMOTION SALES HEADER
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
;
;    Synopsis:  VOID ParaPromotionRead( PARAPROMOTION *pData )
;       input:  pData->uchAddress
;      output:  pData->aszProSales[PARA_PROMOTION_LEN+1]
;
;      Return:  Nothing
;
; Descruption:  This function read PROMOTION SALES HEADER.
;               
;===============================================================================
**/

VOID ParaPromotionRead( PARAPROMOTION *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

    _tcsncpy(pData->aszProSales, ParaPromotion[i], PARA_PROMOTION_LEN);
    pData->aszProSales[PARA_PROMOTION_LEN] = '\0';
}

/**
;=============================================================================
;
;    Synopsis:  VOID ParaPromotionWrite( PARAPROMOTION *pData )
;       input:  pData->uchAddress
;               pData->aszProSales[PARA_PROMOTION_LEN+1]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets PROMOTION SALES HEADER.
;               
;===============================================================================
**/

VOID ParaPromotionWrite( CONST PARAPROMOTION *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */           

	if (i < MAX_PROMO_NO) {
		_tcsncpy(ParaPromotion[i], pData->aszProSales, PARA_PROMOTION_LEN);
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_PROMO_NO), "==ERROR: ParaPromotionWrite() address out of bounds.");
	}
}
