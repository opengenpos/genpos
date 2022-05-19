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
* Title       : Print Common Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSCOM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms common function.
*
*           The provided function names are as follows: 
* 
*               PrtSupHeader()     : form header print format
*               PrtSupTrailer()    : form trailer      
*               PrtSup1stTrailer() : form trailer 1st line print format 
*               PrtSup2ndTrailer() : form trailer 2nd line print format
*               PrtSup3rdTrailer() : form trailer 3rd line print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-30-92: 01.00.00 : J.Ikeda   : initial                                   
* Dec-10-92: 01.00.00 : J.Ikeda   : Del R/J Control                                   
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

/**------- M S - C -------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------ 2170 local ------**/
#include <ecr.h>
#include <rfl.h>
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/


/*
*===========================================================================
** Synopsis:    VOID  PrtSupHeader( MAINTHEADER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTHEADER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms header print format.
*===========================================================================
*/

VOID  PrtSupHeader( MAINTHEADER *pData )
{

    static const TCHAR FARCONST auchPrtSupHead1[] = _T(" %-12s %s");
    static const TCHAR FARCONST auchPrtSupHead2[] = _T("%s");
    static const TCHAR FARCONST auchPrtSupHead3[] = _T("%-4s %u %s");
    static const TCHAR FARCONST auchPrtSupHead4[] = _T("%-4s   %s");
    static const TCHAR FARCONST auchPrtSupHead5[] = _T("%-4s   %s %u");
    static const TCHAR FARCONST auchPrtSupHead6[] = _T("%-4s %u %s %u");
    static const TCHAR FARCONST auchNumber[] = _T("%3d");

    USHORT  usPrtType;
	TCHAR	aszDoubRepoName[PARA_REPORTNAME_LEN * 2 + 1];
	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];


    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrtControl);

	/* 1st Line */
	/* make double wide for report name */
    memset(aszDoubRepoName, '\0', sizeof(aszDoubRepoName));
    PrtDouble(aszDoubRepoName, (PARA_REPORTNAME_LEN * 2 + 1), pData->aszReportName1);

	/* make double wide for report number */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->uchPrgSupNo);
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);
									   
    PmgPrintf(usPrtType, auchPrtSupHead1, aszDoubRepoName, aszDoubRepoNumb);

    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTHEADER_RPT) {

        /* make double wide for report name */
        memset(aszDoubRepoName, '\0', sizeof(aszDoubRepoName));
        PrtDouble(aszDoubRepoName, (PARA_REPORTNAME_LEN * 2 + 1), pData->aszReportName2);

        /* 2nd Line */
        PmgPrintf(usPrtType, auchPrtSupHead2, aszDoubRepoName);

        /* 3rd Line */

        /* check report type */
        if (pData->uchReportType == 0 && pData->uchResetType == 0) {
            PmgPrintf(usPrtType, auchPrtSupHead4, pData->aszReportName3, pData->aszSpeMnemo);      /* special mnemonics */
        } else if (pData->uchReportType != 0 && pData->uchResetType == 0) {
            PmgPrintf(usPrtType, auchPrtSupHead3, pData->aszReportName3, ( USHORT)(pData->uchReportType), pData->aszSpeMnemo);              /* special mnemonics */
        } else if (pData->uchReportType == 0 && pData->uchResetType != 0) {
            PmgPrintf(usPrtType, auchPrtSupHead5, pData->aszReportName3, pData->aszSpeMnemo, ( USHORT)(pData->uchResetType));
        } else {
            PmgPrintf(usPrtType, auchPrtSupHead6, pData->aszReportName3, ( USHORT)(pData->uchReportType), pData->aszSpeMnemo, ( USHORT)(pData->uchResetType));
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID  PrtSupTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer print format.
*===========================================================================
*/
VOID  PrtSupTrailer( MAINTTRAILER *pData )
{
    /* check receipt/journal */
    if (pData->usPrtControl & PRT_RECEIPT) {
        PrtFeed(PMG_PRT_RECEIPT, 1);        /* 1 line feed */  

        PrtSup1stTrailer(pData);            /* print trailer 1st line */ 

        /* check if print electro or not */
        if (CliParaMDCCheck(MDC_SLIP_ADR, EVEN_MDC_BIT3)) { /* do not provide electro */
            PrtSup2ndTrailer(pData);        /* print trailer 2nd line */
            PrtSup3rdTrailer(pData);        /* print trailer 3rd line */
            PrtFeed(PMG_PRT_RECEIPT, 4);    /* 4 line feed */  
            PrtRJHead();                    /* print header */
//            PrtFeed(PMG_PRT_RECEIPT, 2);    /* 2 line feed */
        } else {
            PrtElec();                      /* print electro */
            PrtSup2ndTrailer(pData);        /* print trailer 2nd line */
            PrtSup3rdTrailer(pData);        /* print trailer 3rd line */
            PrtFeed(PMG_PRT_RECEIPT, 10);   /* 10 line feed */   
        }
        PrtCut();                           /* cut */
    } else if (pData->usPrtControl & PRT_JOURNAL) {   
        PrtSup1stTrailer(pData);            /* print trailer 1st line */
        PrtSup2ndTrailer(pData);            /* print trailer 2nd line */
        PrtSup3rdTrailer(pData);            /* print trailer 3rd line */
    }
}

/*
*===========================================================================
** Synopsis:    VOID  PrtSup1stTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer 1st line print format.
*===========================================================================
*/
VOID PrtSup1stTrailer(MAINTTRAILER *pData) 
{
	const TCHAR FARCONST *auchPrtSupTrailer1 = _T("%-4s%04u\t%-4s%04u");
    USHORT usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrtControl);

    /* print 1st trailer line */
    PmgPrintf(usPrtType, auchPrtSupTrailer1, pData->aszDayRstMnemo, pData->usDayRstCount, pData->aszPTDRstMnemo, pData->usPTDRstCount);
}

/*
*===========================================================================
** Synopsis:    VOID  PrtSup2ndTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer 2nd line print format.
*===========================================================================
*/
VOID PrtSup2ndTrailer(MAINTTRAILER *pData)
{
	const TCHAR FARCONST *auchPrtSupTrailer2 = _T("%8.8Mu\t%04u-%03u");
	const TCHAR FARCONST auchPrtSupTrailer5[] = _T("%-4s%04u\t%04u-%03u");
    USHORT usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrtControl);

    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTTRAILER_PRTPRG) {    /* program mode */
        PmgPrintf(usPrtType, auchPrtSupTrailer5, pData->aszPrgCountMnemo, pData->usPrgCount, pData->usStoreNumber, pData->usRegNumber);
    } else {    /* supervisor mode */
        PmgPrintf(usPrtType, auchPrtSupTrailer2, RflTruncateEmployeeNumber(pData->ulSuperNumber), pData->usStoreNumber, pData->usRegNumber);
    }
}
/*
*===========================================================================
** Synopsis:    VOID  PrtSup3rdTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer 3rd line print format.
*===========================================================================
*/

VOID PrtSup3rdTrailer(MAINTTRAILER *pData)
{
	const TCHAR FARCONST *auchPrtSupTrailer3 = _T("%04u\t%2u:%02u %02u/%02u/%02u");
	const TCHAR FARCONST *auchPrtSupTrailer4 = _T("%04u\t%2u:%02u%s %02u/%02u/%02u");
    USHORT usPrtType;
    USHORT usHour;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrtControl);

    /* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
            PmgPrintf(usPrtType, auchPrtSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMDay, pData->usMonth, pData->usYear % 100);
        } else {
            PmgPrintf(usPrtType, auchPrtSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMonth, pData->usMDay, pData->usYear % 100);         /* year */
        }
    } else {
        /* check if Hour is '0' */
        if (pData->usHour == 0) {   /* in case of "0:00 AM" */
            usHour = 12;
        } else {
            usHour = pData->usHour;
        }

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
            PmgPrintf(usPrtType, auchPrtSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM,
                      pData->usMDay, pData->usMonth, pData->usYear % 100);         /* year */
        } else {
            PmgPrintf(usPrtType, auchPrtSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour,
                      pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM, pData->usMonth, pData->usMDay, pData->usYear % 100);
                      
        }
    }
}



