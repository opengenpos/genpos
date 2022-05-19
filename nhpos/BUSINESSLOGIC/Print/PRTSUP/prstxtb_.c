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
* Program Name: PRSTXTB_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupTaxTbl()      : This Function Forms Tax Table Print Format.       
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
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
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtsin.h"



/*
*===========================================================================
** Synopsis:    VOID  PrtSupTaxTbl( MAINTTAXTBL *pData )  
*               
*     Input:    *pData  : Pointer to Structure for MAINTTAXTBL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Tax Table Print Format.
*===========================================================================
*/

VOID  PrtSupTaxTbl( MAINTTAXTBL *pData )  
{
 
    /* Define Format */

    static const TCHAR FARCONST auchPrtSupTaxTbl1[] = _T("  TAX RATE%4u.%02u/%02u%%");
    static const TCHAR FARCONST auchPrtSupTaxTbl2[] = _T("  TAX RATE%4u.%02u/%02u%%\n\tSALE AMOUNT  TAX ");
    static const TCHAR FARCONST auchPrtSupTaxTbl3[] = _T("\t%6l$ %6$");
    static const TCHAR FARCONST auchPrtSupTaxTbl4[] = _T("\t%6l$-%6l$ %6$");

    USHORT usPrtType;


    /* Check Print Control */

    usPrtType = PrtSupChkType(pData->usPrtControl);  

    /* Check Minor Class */

    switch(pData->uchMinorClass) {
    case CLASS_MAINTTAXTBL_PRTWRTINT:                
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupTaxTbl1,                    /* Format */
                  ( USHORT)pData->uchInteger,           /* Integer */
                  ( USHORT)pData->uchNumerator,         /* Numerator */
                  ( USHORT)pData->uchDenominator);      /* Denominator */
        break;

    case CLASS_MAINTTAXTBL_PRTRPTINT:                
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupTaxTbl2,                    /* Format */
                  ( USHORT)pData->uchInteger,           /* Integer */
                  ( USHORT)pData->uchNumerator,         /* Numerator */
                  ( USHORT)pData->uchDenominator);      /* Denominator */
        break;

    case CLASS_MAINTTAXTBL_PRTTAXABLE:                      
        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupTaxTbl3,                    /* Format */
                  (DCURRENCY)pData->ulEndTaxableAmount,  /* Taxable Amount, format of %6l$ requires DCURRENCY */
                  (SHORT)pData->usTaxAmount);            /* Tax Amount, format of %6$ requires SHORT */
        break;

    case CLASS_MAINTTAXTBL_PRTTAX:

        PmgPrintf(usPrtType,                            /* Printer Type */
                  auchPrtSupTaxTbl4,                    /* Format */
                  (DCURRENCY)pData->ulStartTaxableAmount,   /* Start Taxable Amount, format of %6l$ requires DCURRENCY */
                  (DCURRENCY)pData->ulEndTaxableAmount,     /* End Taxable Amount, format of %6l$ requires DCURRENCY */
                  (SHORT)pData->usTaxAmount);               /* Tax Amount, format of %6$ requires SHORT */
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}
