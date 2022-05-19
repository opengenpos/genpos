/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaPresetAmount Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAPAMT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                       
*               ParaPresetAmountRead()  : reads Preset Amount Cash Tender
*               ParaPresetAmountWrite() : sets Preset Amount Cash Tender
*                                                                 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-02-93:  01.00.12 : K.You    : initial                                   
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaPresetAmountRead( PARAPRESETAMOUNT *pData )
;       input:  pData->uchAddress
;      output:  pData->ulAmount
;
;      Return:  Nothing
;
; Descruption:  This function reads Preset Amount cash Tender.
;===============================================================================
**/

VOID ParaPresetAmountRead( PARAPRESETAMOUNT *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    pData->ulAmount = ParaPresetAmount[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaPresetAmountWrite( PARAPRESETAMOUNT *pData )
;       input:  pData->uchAddress
;               pData->ulAmount           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets Preset Amount Cash Tender.
;===============================================================================
**/

VOID ParaPresetAmountWrite( PARAPRESETAMOUNT *pData )
{

    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    ParaPresetAmount[i] = pData->ulAmount;

}

/****** End of Source ******/
