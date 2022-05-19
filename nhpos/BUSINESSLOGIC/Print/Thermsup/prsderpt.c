/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Thermal Print DEPT/MDEPT File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDERPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupDEPTFile() : forms DEPT/MDEPT File print format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
* Dec-28-99: 01.00.00 : hrkato    : Saratoga
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
#include <string.h>
#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupDEPTFile( RPTDEPT *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTDEPT 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms DEPT/MDEPT File print format.
*
*                  : DEPARTMENT SALES READ REPORT        (except START/END TIME)
*                  : MAJOR DEPART MENT SALES READ REPORT (except START/END TIME)
*
*             cf. START/END TIME formats are same as REGISTER FINANCIAL REPORT.
*
*===========================================================================
*/

VOID  PrtThrmSupDEPTFile( RPTDEPT *pData )  
{
    /* define thermal print format */ 
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile1[] = _T("%17u %s");
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile2A0[] = _T("%8s %s\t*****%%\n\t%6ld   %12l$");
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile2B0[] = _T("%8s %s\t%6.2$%%\n\t%6ld   %12l$");
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile30[] = _T("%8s %s\n\t%6ld   %12l$");
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile4[] = _T("%-8s\t%6ld   %12l$");
    static const TCHAR FARCONST auchPrtThrmSupDEPTFile50[] = _T("%-8s\t%6ld   %12l$");
    static const TCHAR FARCONST auchNumber[] = _T("%4d");                   /* Saratoga */

    TCHAR  aszPrtNull[1] = {'\0'};
	TCHAR  aszDoubRepoNumb[8 * 2 + 1] = { 0 };
	TCHAR  aszRepoNumb[8 + 1] = { 0 };

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
        /* initialize */
        memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
       
        /* check minor class */
        switch(pData->uchMinorClass) {
        case CLASS_RPTDEPT_PRTMAJOR:                
            /* print MDEPT in dept report */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupDEPTFile1, pData->uchMajorDEPTNo, pData->aszMnemo);             /* MDEPT name */
            break;

        case CLASS_RPTDEPT_PRTTTLCNT1:
            /* convert DEPT number to double wide,  Saratoga */
            RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);
            PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

            /* check if PERCENT is illegal */
            if (pData->usPercent > PRT_MAX_PERCENT) {           /* in case that percent data is illegal,    */
                                                                /* pData->usPercent is set over 600.00 data */
                /* print PLUS DEPT with illegal percent data */
                PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                          auchPrtThrmSupDEPTFile2A0,            /* format */
                          aszDoubRepoNumb,                      /* DEPT number */
                          pData->aszMnemo,                      /* DEPT name */
                          pData->DEPTTotal.lCounter,            /* counter */
                          pData->DEPTTotal.lAmount);            /* amount */
            } else {
                /* print PLUS DEPT with legal percent data */
                PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                          auchPrtThrmSupDEPTFile2B0,            /* format */
                          aszDoubRepoNumb,                      /* DEPT number */
                          pData->aszMnemo,                      /* DEPT name */
                          (SHORT)pData->usPercent,              /* percent, format of %6.2$ requires SHORT */
                          pData->DEPTTotal.lCounter,            /* counter */
                          pData->DEPTTotal.lAmount);            /* amount */
            }
            break;
       
        case CLASS_RPTDEPT_PRTTTLCNT2:
            /* convert DEPT number to double wide,  Saratoga */
            RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);
            PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

            /* print MINUS DEPT */ 
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupDEPTFile30,             /* format */
                      aszDoubRepoNumb,                      /* DEPT number */
                      pData->aszMnemo,                      /* DEPT name */
                      pData->DEPTTotal.lCounter,            /* counter */
                      pData->DEPTTotal.lAmount);            /* amount */
            break;

        case CLASS_RPTDEPT_PRTTTLCNT3:
            /* print SUBTOTAL/PLUS/MINUS/HASH/CIGARETTE/GRAND TOTAL */
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupDEPTFile50,             /* format */
                      pData->aszMnemo,                      /* transaction mnemonics */
                      pData->DEPTTotal.lCounter,            /* counter */
                      pData->DEPTTotal.lAmount);            /* amount */
            break;
       
        case CLASS_RPTDEPT_PRTMDEPTTTL:
            /* print MDEPT */ 
            PrtPrintf(PMG_PRT_RECEIPT,                      /* printer type */
                      auchPrtThrmSupDEPTFile4,              /* format */
                      pData->aszMnemo,                      /* MDEPT name */
                      pData->DEPTTotal.lCounter,            /* counter */
                      pData->DEPTTotal.lAmount);            /* amount */
            break;

        default:
/*          PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
            break;
        }
    }

    /* check print control */
    if (pData->usPrintControl & PRT_JOURNAL) {
        return;
    }
}

/***** End of Source *****/