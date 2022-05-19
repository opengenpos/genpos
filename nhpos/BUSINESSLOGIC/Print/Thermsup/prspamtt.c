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
* Title       : Print Preset Amount Cash Tender Format ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSPAMTT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*             PrtThrmSupPresetAmount() : forms Preset Amount print format     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-05-93: 01.00.12 : J.IKEDA   : initial                                   
*          :          :           :                                    
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
=============================================================================
**/

#include	<tchar.h>
#include <ecr.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
/* #include <cswai.h> */
#include <csop.h>
#include <report.h> 
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupPresetAmount( PARAPRESETAMOUNT *pData )
*               
*     Input:    *pData      : pointer to structure for PARAPRESETAMOUNT
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Preset Amount data print format.
*
*                : PRESET AMOUNT FOR PRESET CASH KEY
*
*===========================================================================
*/

VOID PrtThrmSupPresetAmount( PARAPRESETAMOUNT *pData )
{
    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
		static const TCHAR FARCONST auchPrtThrmSupPresetAmount[] = _T("%18u  /  %8l$");

        /* print ADDRESS / PRESET AMOUNT */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupPresetAmount,       /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  (DCURRENCY)pData->ulAmount);      /* preset amount, format of %8l$ requires DCURRENCY */
    } 

    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
		static const TCHAR FARCONST auchPrtSupPresetAmount[] = _T("%8u / %8l$");

        /* print ADDRESS / PRESET AMOUNT */
        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupPresetAmount,           /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  (DCURRENCY)pData->ulAmount);       /* preset amount, format of %8l$ requires DCURRENCY */
    }
}

/****** End of Source ******/

