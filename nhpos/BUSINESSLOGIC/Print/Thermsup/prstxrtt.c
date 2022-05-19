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
* Title       : Print Tax Rate Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTXRTT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:            
* 
*               PrtThrmSupTaxRate()     : forms Tax Rate Table print format     
*                                              
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-17-93: 01.00.12 : J.IKEDA   : initial                                   
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
** Synopsis:    VOID PrtThrmSupTaxRate( PARATAXRATETBL *pData )
*               
*     Input:    *pData      : pointer to structure for PARATAXRATETBL
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Tax Rate Table data print format.
*
*                : SET TAX RATES
*
*===========================================================================
*/

VOID PrtThrmSupTaxRate( PARATAXRATETBL *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupTaxRate1[] = _T("%15u  /  %7.4l$%%");
    static const TCHAR FARCONST auchPrtThrmSupTaxRate2[] = _T("%15u  /     %5lu");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupTaxRate1[] = _T("%6u / \t%.4l$%%");
    static const TCHAR FARCONST auchPrtSupTaxRate2[] = _T("%6u / \t%5lu");


    /* distinguish minor class */ 
    
    switch(pData->uchMinorClass) {
    case CLASS_PARATAXRATETBL_RATE:             /* tax rate */

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ADDRESS / RATE */
                                           
            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupTaxRate1,           /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulTaxRate);                /* tax rate */
        } 

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ADDRESS / RATE */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupTaxRate1,               /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulTaxRate);                /* tax rate */
        }
        break;

    case CLASS_PARATAXRATETBL_LIMIT:            /* Exempt Limit */

        /* check print control */

        if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */

            /* print ADDRESS / TAX EXEMPT LIMIT */
                                           
            PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtThrmSupTaxRate2,           /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulTaxRate);                /* tax exempt limit */
        } 

        if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */

            /* print ADDRESS / TAX EXEMPT LIMIT */

            PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                      auchPrtSupTaxRate2,               /* format */
                      ( USHORT)pData->uchAddress,       /* address */
                      pData->ulTaxRate);                /* tax exempt limit */
        }
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;                                      
    }
}
/***** End of Source *****/
