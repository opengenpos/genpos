/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaServiceTime Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASERV.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaServiceTimeRead()  : reads Service Time Parameter
*               ParaServiceTimeWrite() : sets Service Time Parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Nov-10-95:  03.01.00 : M.Ozawa    : Initial
*          :           :            :
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
;    Synopsis:  VOID ParaServiceTimeRead( PARASERVICETIME *pData )
;       input:  pData->uchAddress
;      output:  pData->usBorder
;
;      Return:  Nothing
;
; Descruption:  This function reads Service Time Parameter
;===============================================================================
**/

VOID ParaServiceTimeRead( PARASERVICETIME *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);    /* "-1" fits program address to RAM address */  

    pData->usBorder = ParaServiceTime[i];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaServiceTimeWrite( PARASERVICETIME *pData )
;       input:  pData->uchAddress
;               pData->usBorder
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets Service Time Parameter
;===============================================================================
**/

VOID ParaServiceTimeWrite( CONST PARASERVICETIME *pData )
{
	UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < MAX_SERVICETIME_SIZE) {
		ParaServiceTime[i] = pData->usBorder;
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_SERVICETIME_SIZE), "==ERROR: ParaServiceTimeWrite() address out of bounds.");
	}
}
