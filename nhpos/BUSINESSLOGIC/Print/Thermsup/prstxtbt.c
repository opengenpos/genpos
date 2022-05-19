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
* Title       : Tax Table Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSTXTBT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupTaxTbl()      : forms Tax Table print format       
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
** Synopsis:    VOID  PrtThrmSupTaxTbl( MAINTTAXTBL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTTAXTBL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Tax Table print format.
*
*                 : TAX TABLE #1/#2/#3/#4 PROGRAMMING
*
*===========================================================================
*/

VOID  PrtThrmSupTaxTbl( MAINTTAXTBL *pData )  
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupTaxTbl1[] = _T("%6sTAX RATE  %2u.%02u/%02u%%");
    static const TCHAR FARCONST auchPrtThrmSupTaxTbl2[] = _T("%6sTAX RATE  %2u.%02u/%02u%%\n%12sSALE AMOUNT%10sTAX ");
    static const TCHAR FARCONST auchPrtThrmSupTaxTbl3[] = _T("%19s%6l$%6s%6$");
    static const TCHAR FARCONST auchPrtThrmSupTaxTbl4[] = _T("%10s%6l$ - %6l$%6s%6$");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupTaxTbl1[] = _T("  TAX RATE%4u.%02u/%02u%%");
    static const TCHAR FARCONST auchPrtSupTaxTbl2[] = _T("  TAX RATE%4u.%02u/%02u%%\n\tSALE AMOUNT  TAX ");
    static const TCHAR FARCONST auchPrtSupTaxTbl3[] = _T("\t%6l$ %6$");
    static const TCHAR FARCONST auchPrtSupTaxTbl4[] = _T("\t%6l$-%6l$ %6$");

    TCHAR   aszPrtNull[1] = {'\0'};


    /* check print control */
    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_MAINTTAXTBL_PRTWRTINT:                
            /* print TAX RATE */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupTaxTbl1,                /* format */
                      aszPrtNull,                           /* null */
                      ( USHORT)pData->uchInteger,           /* integer */
                      ( USHORT)pData->uchNumerator,         /* numerator */
                      ( USHORT)pData->uchDenominator);      /* denominator */
            break;

        case CLASS_MAINTTAXTBL_PRTRPTINT:                
            /* print TAX RATE for REPORT */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupTaxTbl2,                /* format */
                      aszPrtNull,                           /* null */
                      ( USHORT)pData->uchInteger,           /* integer */
                      ( USHORT)pData->uchNumerator,         /* numerator */
                      ( USHORT)pData->uchDenominator,       /* denominator */
                      aszPrtNull,                           /* null */
                      aszPrtNull);                          /* null */
            break;

        case CLASS_MAINTTAXTBL_PRTTAXABLE:                      
            /* print TAXABLE AMOUNT/TAX */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupTaxTbl3,                /* format */
                      aszPrtNull,                           /* null */
                      (DCURRENCY)pData->ulEndTaxableAmount,  /* taxable amount, format of %6l$ requires DCURRENCY */
                      aszPrtNull,                           /* null */
                      (SHORT)pData->usTaxAmount);            /* tax amount, format of %6$ requires SHORT */
            break;

        case CLASS_MAINTTAXTBL_PRTTAX:
            /* print START/END TAXABLE AMOUNT & TAX */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupTaxTbl4,                /* format */
                      aszPrtNull,                           /* null */
                      (DCURRENCY)pData->ulStartTaxableAmount,  /* start taxable amount, format of %6l$ requires DCURRENCY */
                      (DCURRENCY)pData->ulEndTaxableAmount,    /* end taxable amount, format of %6l$ requires DCURRENCY */
                      aszPrtNull,                           /* null */
                      (SHORT)pData->usTaxAmount);              /* tax amount, format of %6$ requires SHORT */
            break;

        default:
/*          PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
            break;
        }
    }

    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */
        switch(pData->uchMinorClass) {
        case CLASS_MAINTTAXTBL_PRTWRTINT:                
            /* print TAX RATE */
            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupTaxTbl1,                    /* Format */
                      ( USHORT)pData->uchInteger,           /* Integer */
                      ( USHORT)pData->uchNumerator,         /* Numerator */
                      ( USHORT)pData->uchDenominator);      /* Denominator */
            break;

        case CLASS_MAINTTAXTBL_PRTRPTINT:                
            /* print TAX RATE for REPORT */
            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupTaxTbl2,                    /* Format */
                      ( USHORT)pData->uchInteger,           /* Integer */
                      ( USHORT)pData->uchNumerator,         /* Numerator */
                      ( USHORT)pData->uchDenominator);      /* Denominator */
            break;

        case CLASS_MAINTTAXTBL_PRTTAXABLE:                      
            /* print TAXABLE AMOUNT/TAX */
            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupTaxTbl3,                    /* Format */
                      (DCURRENCY)pData->ulEndTaxableAmount,  /* Taxable Amount, format of %6l$ requires DCURRENCY */
                      (SHORT)pData->usTaxAmount);            /* Tax Amount, format of %6$ requires SHORT */
            break;

        case CLASS_MAINTTAXTBL_PRTTAX:
            /* print START/END TAXABLE AMOUNT & TAX */
            PrtPrintf(PMG_PRT_JOURNAL,                      /* printer type */
                      auchPrtSupTaxTbl4,                    /* Format */
                      (DCURRENCY)pData->ulStartTaxableAmount, /* Start Taxable Amount, format of %6l$ requires DCURRENCY */
                      (DCURRENCY)pData->ulEndTaxableAmount,   /* End Taxable Amount, format of %6l$ requires DCURRENCY */
                      (SHORT)pData->usTaxAmount);             /* Tax Amount, format of %6$ requires SHORT */
            break;

        default:
/*          PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
            break;
        }
    }
}
/***** End of Source *****/
