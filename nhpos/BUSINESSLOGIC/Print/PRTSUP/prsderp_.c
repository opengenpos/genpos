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
* Title       : Department File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSDERP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupDEPTFile()    : This Function Forms Department File Print Format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-27-92: 00.00.01 : K.You     : initial
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
** Synopsis:    VOID  PrtSupDEPTFile( RPTDEPT *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTDEPT 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Department File Print Format.
*===========================================================================
*/
VOID  PrtSupDEPTFile( RPTDEPT *pData )  
{
    static const TCHAR FARCONST auchPrtSupDEPTFile1[] = _T("%5s%2u %s");
    static const TCHAR FARCONST auchPrtSupDEPTFile2A0[] = _T("%-s %s\t*****%%\n%12ld\t%l$");
    static const TCHAR FARCONST auchPrtSupDEPTFile2B0[] = _T("%-s %s\t%l$%%\n%12ld\t%l$");
    static const TCHAR FARCONST auchPrtSupDEPTFile30[] = _T("%-s %s\n%12ld\t%l$");
    static const TCHAR FARCONST auchPrtSupDEPTFile4[] = _T("%-8.8s\t%ld\n\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupDEPTFile60[] = _T("%-8.8s\n%12ld\t%l$"); /*8 characters JHHJ*/
    static const TCHAR FARCONST auchNumber[] = _T("%4d");           /* Saratoga */

    UCHAR  uchDecimal = 2;
    TCHAR  aszPrtNull[1] = {'\0'};
    /* CHAR  aszDoubRepoNumb[2 * 2 + 1]; */
    TCHAR  aszRepoNumb[8 + 1];
    USHORT usPrtType;

    /* Initialize aszDoubRepoNumb */
    /* memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb)); */

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);  

    /* Check Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTDEPT_PRTMAJOR:                
        PmgPrintf(usPrtType, auchPrtSupDEPTFile1, aszPrtNull, pData->uchMajorDEPTNo, pData->aszMnemo);
        break;

    case CLASS_RPTDEPT_PRTTTLCNT1:
        /* Convert DEPT No. to Double Wide, Saratoga */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);

        /* check if PERCENT is over 600.00% */
        if (pData->usPercent > PRT_MAX_PERCENT) {
            PmgPrintf(usPrtType, auchPrtSupDEPTFile2A0, aszRepoNumb, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
        } else {
            PmgPrintf(usPrtType, auchPrtSupDEPTFile2B0, aszRepoNumb, pData->aszMnemo, (DCURRENCY)pData->usPercent, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
        }
        break;

    case CLASS_RPTDEPT_PRTTTLCNT2:
        /* Convert DEPT No. to Double Wide, Saratoga */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);
        PmgPrintf(usPrtType, auchPrtSupDEPTFile30, aszRepoNumb, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
        break;

    case CLASS_RPTDEPT_PRTTTLCNT3:
        PmgPrintf(usPrtType, auchPrtSupDEPTFile60, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
        break;

    case CLASS_RPTDEPT_PRTMDEPTTTL:
        PmgPrintf(usPrtType, auchPrtSupDEPTFile4, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}

/* --- End of Source File --- */