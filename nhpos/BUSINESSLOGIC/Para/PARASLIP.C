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
* Title       : Parameter ParaSlipFeedCtl Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASLIP.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSlipFeedCtlRead()  : reads FEED CONTROL DATA
*               ParaSlipFeedCtlWrite() : sets FEED CONTROL DATA
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
;    Synopsis:  VOID ParaSlipFeedCtlRead( PARASLIPFEEDCTL *pData )
;       input:  pData->uchAddress
;      output:  pData->uchFeedCtl
;
;      Return:  Nothing
;
; Descruption:  This function reads FEED CONTROL DATA.
;===============================================================================
**/

VOID ParaSlipFeedCtlRead( PARASLIPFEEDCTL *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

    pData->uchFeedCtl = ParaSlipFeedCtl[i];
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSlipFeedCtlWrite( PARASLIPFEEDCTL *pData )
;       input:  pData->uchAddress
;               pData->uchFeedCtl           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets FEED CONTROL DATA.
;===============================================================================
**/

VOID ParaSlipFeedCtlWrite( PARASLIPFEEDCTL *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */             

	if( i < MAX_SLIP_SIZE ) {   // out of bounds guard, ParaSlipFeedCtlWrite( PARASLIPFEEDCTL *pData )
		ParaSlipFeedCtl[i] = pData->uchFeedCtl;
	}
}
