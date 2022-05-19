/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaRestriction Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAREST.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*             ParaRestRead()  : reads PLU Sales Restriction Table
*             ParaRestWrite() : sets PLU Sales Restriction Table
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Mar-10-93:  00.00.01 : M.Ozawa  : initial                                   
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
;    Synopsis:  VOID ParaRestRead( PARARESTRICTION *pData )
;       input:  pData->uchAddress
;      output:  pData->uchDate
;               pData->uchDay
;               pData->uchHour1
;               pData->uchMin1
;               pData->uchHour2
;               pData->uchMin2
;      Return:  Nothing
;
; Descruption:  This function reads PLU Sales Restriction Table.
;===============================================================================
**/

VOID ParaRestRead( PARARESTRICTION *pData )
{
    UCHAR i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    pData->uchDate = ParaRestriction[i].uchDate;
    pData->uchDay = ParaRestriction[i].uchDay;
    pData->uchHour1 = ParaRestriction[i].uchHour1;
    pData->uchMin1 = ParaRestriction[i].uchMin1;
    pData->uchHour2 = ParaRestriction[i].uchHour2;
    pData->uchMin2 = ParaRestriction[i].uchMin2;
}

/**
;=============================================================================
;    Synopsis:  VOID ParaRestWrite( PARARESTRICTION *pData )
;       input:  pData->uchAddress
;      output:  pData->uchDate
;               pData->uchDay
;               pData->uchHour1
;               pData->uchMin1
;               pData->uchHour2
;               pData->uchMin2
;      output:  Nothing
;      Return:  Nothing
;
; Descruption:  This function sets PLU Sales Restriction Table.
;===============================================================================
**/

VOID ParaRestWrite( CONST PARARESTRICTION *pData )
{
    UCHAR i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < MAX_REST_SIZE) {
		ParaRestriction[i].uchDate = pData->uchDate;
		ParaRestriction[i].uchDay = pData->uchDay;
		ParaRestriction[i].uchHour1 = pData->uchHour1;
		ParaRestriction[i].uchMin1 = pData->uchMin1;
		ParaRestriction[i].uchHour2 = pData->uchHour2;
		ParaRestriction[i].uchMin2 = pData->uchMin2;
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_REST_SIZE), "==ERROR: ParaRestWrite() address out of bounds.");
	}
}
