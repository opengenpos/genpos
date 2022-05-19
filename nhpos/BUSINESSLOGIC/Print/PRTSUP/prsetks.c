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
* Title       : Print(21CHAR) Employee File Format ( SUPER & PROGRAM MODE ) 
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSETKS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupETKFile_s()    : ETK print format.
*               PrtSupMakeData_s() : make print data
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-30-93 : 01.00.02 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
/* #include <pif.h> */
/* #include <rfl.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupETKFile_s( RPTEMPLOYEE *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTEMPLOYEE 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms ETK print format.
*
*             : EMPLOYEE TIME KEEPING FILE READ REPORT  
*             : EMPLOYEE TIME KEEPING FILE RESET REPORT 
*             : EMPLOYEE TIME KEEPING FILE MAINTENANCE 
*
*===========================================================================
*/

VOID  PrtSupETKFile_s( RPTEMPLOYEE *pData )  
{
    /* define print format */
    static const TCHAR  auchPrtSupETKFile1[] = _T("\t%-8.8s%-8.8s");
    static const TCHAR  auchPrtSupETKFile2[] = _T("%5s %7s %7s\n\t%2s %13s");
    static const TCHAR  auchPrtSupETKFile3[] = _T("%-8.8s\t%5u:%02u %7l$");
    static const TCHAR  auchPrtSupETKFile4[] = _T("%-8.8s\t***** *******");
    static const TCHAR  auchNumber[] = _T("%d");

    TCHAR   aszBuffer[3] = { 0, 0, 0 };
    USHORT  usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    switch(pData->uchMinorClass) {
    case CLASS_RPTEMPLOYEE_PRTMNEMO:      
        /* print TRANS MNEMO */
        PmgPrintf(usPrtType,                   /* printer type */
                  auchPrtSupETKFile1,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TIME-IN) */
                  pData->aszTimeoutMnemo);     /* mnemonic(TIME-OUT) */
       
        break;

    case CLASS_RPTEMPLOYEE_PRTTIME:      
    case CLASS_RPTEMPLOYEE_PRTNOOUT:    /* without TIME-OUT */      
    case CLASS_RPTEMPLOYEE_PRTNOIN:     /* without TIME-IN */      

        /* make data */

        PrtSupMakeData(pData);

        if(pData->uchJobCode) {
            _itot(pData->uchJobCode, aszBuffer, 10);
        }

        /* print DATE/TIME/JOB CODE/WORKED HOURS */

        PmgPrintf(usPrtType,                   /* printer type */
                  auchPrtSupETKFile2,          /* format */
                  aszPrtDate,                  /* date */
                  aszPrtInTime,                /* time-in */
                  aszPrtOutTime,               /* time-out */
                  aszBuffer,                   /* job code */
                  aszPrtWorkTime);             /* work time */

        break;

    case CLASS_RPTEMPLOYEE_PRTWKTTL:   

        /* print WORK TOTAL */
       
        PmgPrintf(usPrtType,                   /* printer type */
                  auchPrtSupETKFile3,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usWorkTime,           /* total work hour */
                  pData->usWorkMinute,         /* total work minute */
                  (DCURRENCY)pData->ulWorkWage); /* total work wage, format of %7l$ requires DCURRENCY */
        break;

    case CLASS_RPTEMPLOYEE_PRTTOTAL:  

        /* print TOTAL */
       
        PmgPrintf(usPrtType,                   /* printer type */
                  auchPrtSupETKFile3,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usTotalTime,          /* total hour */
                  pData->usTotalMinute,        /* total minute */
                  (DCURRENCY)pData->ulTotalWage); /* total wage, format of %7l$ requires DCURRENCY */
        break;

    case CLASS_RPTEMPLOYEE_PRTWKNOTTL: 
    case CLASS_RPTEMPLOYEE_PRTNOTOTAL:

        /* print ILLEGAL WORK TOTAL/TOTAL */
       
        PmgPrintf(usPrtType,                   /* printer type */
                  auchPrtSupETKFile4,          /* format */
                  pData->aszTimeinMnemo);      /* mnemonic(TOTAL) */
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
} 
/***** End of Source *****/
