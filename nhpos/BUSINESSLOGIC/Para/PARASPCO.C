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
* Title       : Parameter ParaSpcCo Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASPCO.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSpcCoRead()  : reads SPECIAL COUNTER
*               ParaSpcCoWrite() : sets SPECIAL COUNTER
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
;    Synopsis:  VOID ParaSpcCoRead( PARASPCCO *pData )
;       input:  pData->uchAddress
;      output:  pData->usCounter
;
;      Return:  Nothing
;
; Descruption:  This function reads SPECIAL COUNTER.
;===============================================================================
**/

VOID ParaSpcCoRead( PARASPCCO *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);    /* "-1" fits program address to RAM address */  

    pData->usCounter = Para.ParaSpcCo[i];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSpcCoWrite( PARASPCCO *pData )
;       input:  pData->uchAddress
;               pData->usCounter           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets SPECIAL COUNTER.
;===============================================================================
**/

VOID ParaSpcCoWrite( PARASPCCO *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if( i < MAX_CO_SIZE ) {   // out of bounds guard, ParaSpcCoWrite( PARASPCCO *pData )
		Para.ParaSpcCo[i] = pData->usCounter;
	}
}
