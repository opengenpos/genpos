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
* Title       : Thermal Print Foreign Currency Conversion Rate Format  
*                                                  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSFCT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*            PrtThrmSupCurrency() : forms Foreign Currency Conversion Rate print format 
*                                                
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <csttl.h>
#include <csop.h>
#include <report.h> 
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <pmg.h>

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID PrtThrmSupCurrency( PARACURRENCYTBL *pData )
*               
*     Input:    *pData      : pointer to structure for PARACURRENCYTBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Foreign Currency Conversion Rate print format.
*
*               : SET CURRENCY CONVERSION RATE
*
*===========================================================================
*/

VOID PrtThrmSupCurrency( PARACURRENCYTBL *pData )
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupCurrency[] = _T("%13u  /  %10.*l$");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupCurrency[] = _T("%8u / \t%10.*l$");

    SHORT       sDecPoint;

    if (pData->uchStatus & MAINT_6DECIMAL_DATA) {
        sDecPoint = MAINT_6DECIMAL;
    } else {
        sDecPoint = MAINT_5DECIMAL;
    }

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print ADDRESS / FOREIGN CURRENCY RATE */
        PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                  auchPrtThrmSupCurrency,           /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  sDecPoint,                        /* decimal place position count */
                  (DCURRENCY)pData->ulForeignCurrency);  /* currency rate, format of %10.*l$ requires DCURRENCY */
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        /* print ADDRESS / FOREIGN CURRENCY RATE */
        PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                  auchPrtSupCurrency,               /* format */
                  ( USHORT)pData->uchAddress,       /* address */
                  sDecPoint,                        /* decimal place position count */
                  (DCURRENCY)pData->ulForeignCurrency);  /* currency rate, format of %10.*l$ requires DCURRENCY */
    }
}
/***** End of Source *****/
