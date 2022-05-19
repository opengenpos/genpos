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
* Title       : Thermal Print Common Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRTSCOMT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms common function.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupHeader()      : form header print format
*               PrtThrmSupTrailer()     : form trailer      
*               PrtThrmSupFormTrailer() : form trailer thermal line print format 
*               PrtEJSupFormTrailer()   : form trailer EJ line print format 
*               PrtSupItoa()            : convert 4 but binary data to ASCII.
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-18-93: 01.00.12 : J.IKEDA   : Initial                                   
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
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <shr.h>
#include <prt.h>

#include "prtcom.h"
#include "prtshr.h"
#include "prtrin.h"
#include "prtsin.h"

/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

/* define print format for TRAILER */

/*-- thermal --*/
/* program mode */

/* supervisor mode */

/* common trailer */

/*-- EJ --*/

VOID  PrtThrmSupOutputLine( MAINTPRINTLINE *pData )
{
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */
		PrtPrintf(PMG_PRT_JOURNAL, _T("%-40s"), pData->aszPrintLine);
	}

    if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        PrtPrintf(PMG_PRT_RECEIPT, _T("%-40s"), pData->aszPrintLine);
	}

}

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupHeader( MAINTHEADER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTHEADER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function controls header.
*                1. check and print receipt header. 
*                2. form header print format for PROG/SUPER MODE.
*                3. send header data to EJ file
*===========================================================================
*/

VOID  PrtThrmSupHeader( MAINTHEADER *pData )
{

    /* define thermal print format */

    static const TCHAR FARCONST auchPrtThrmSupHead1[] = _T(" %-12s   %-6s   %-12s");
    static const TCHAR FARCONST auchPrtThrmSupHead2[] = _T(" %-6s %2s %s %s");

    /* define EJ print format */

    static const TCHAR FARCONST auchPrtSupHead1[] = _T(" %-12s %s");
    static const TCHAR FARCONST auchPrtSupHead2[] = _T("%s");
    static const TCHAR FARCONST auchPrtSupHead3[] = _T("%-4s %s %s %s");

    /* define thermal/EJ common print format */

    static const TCHAR FARCONST auchNumber[] = _T("%3d");                     

    TCHAR   aszReportType[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszResetType[7] = { 0, 0, 0, 0, 0, 0, 0 };
    TCHAR   aszDoubRepoName1[PARA_REPORTNAME_LEN * 2 + 1];
    TCHAR   aszDoubRepoName2[PARA_REPORTNAME_LEN * 2 + 1];
    TCHAR   aszDoubRepoNumb[8 * 2 + 1];
    TCHAR   aszRepoNumb[8 + 1];
    TRANINFORMATION     Tran;
    MAINTSPHEADER       SPHead;

    /* set transaction information */
	{
		PARASTOREGNO   StRegNoRcvBuff;
		ULONG          ulStoreRegNo;

		StRegNoRcvBuff.uchMajorClass = CLASS_PARASTOREGNO;    /* get store/ reg No. */
		StRegNoRcvBuff.uchAddress = SREG_NO_ADR;
		CliParaRead(&StRegNoRcvBuff);
	    
		ulStoreRegNo = (ULONG) StRegNoRcvBuff.usStoreNo * 1000L + (ULONG) StRegNoRcvBuff.usRegisterNo;

		Tran.TranCurQual.ulStoreregNo = ulStoreRegNo;
	}

	{
	    PARASPCCO ParaSpcCo;

		ParaSpcCo.uchMajorClass = CLASS_PARASPCCO;
		ParaSpcCo.uchAddress = SPCO_CONSEC_ADR;                    
		CliParaRead(&ParaSpcCo);                                     /* call ParaSpcCoRead() */
		Tran.TranCurQual.usConsNo = ParaSpcCo.usCounter;
	}

    /* initialize EJ BUFFER & print PRT_RECEIPT HEADER */

	/* modified for slip printer is not connected case, 09/19/01 */
    PrtPortion(pData->usPrtControl);
    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
/*    if (fsPrtCompul & PRT_SLIP) {       /* Optional Slip */
        memset(&SPHead, 0, sizeof(SPHead));
        SPHead.usACNumber   = pData->uchPrgSupNo;
        PrtSupSPHead(&Tran, &SPHead);
    } else {
        if (pData->usPrtControl & PRT_RECEIPT){
			PrtTransactionBegin(PRT_REGULARPRINTING,0);
            PrtTHHead(&Tran);
        }
    }

    PrtForceEJInit(&Tran);

    /* initialize */
    memset(aszRepoNumb, '\0', sizeof(aszRepoNumb));
    memset(aszDoubRepoName1, '\0', sizeof(aszDoubRepoName1));
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    memset(aszDoubRepoName2, '\0', sizeof(aszDoubRepoName2));

    /* make double wide for report name */
    PrtDouble(aszDoubRepoName1, TCHARSIZEOF(aszDoubRepoName1), pData->aszReportName1);

    /* make double wide for report number */
    if (pData->uchPrgSupNo) {
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->uchPrgSupNo);
    }
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* check minor class */
    if ((pData->uchMinorClass == CLASS_MAINTHEADER_RPT) || (pData->uchMinorClass == CLASS_MAINTHEADER_NOTSEND)) {
        /* make double wide for report name */
        PrtDouble(aszDoubRepoName2, TCHARSIZEOF(aszDoubRepoName2), pData->aszReportName2);

        /* convert REPORT TYPE to binary ASCII data */
        if (pData->uchReportType) {                   
            _itot(pData->uchReportType, aszReportType, 10);
        }
        /* convert RESET TYPE to binary ASCII data */
        if (pData->uchResetType) {                   
            _itot(pData->uchResetType, aszResetType, 10);
        }
    }

    if ( !(fsPrtPrintPort & PRT_SLIP) ) {              /* receipt print           */
        /* check print control */
        if (pData->usPrtControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
            /* 1st line */
            /* print ACTION No. / PROG No. / REPORT NAME */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupHead1, aszDoubRepoName1, aszDoubRepoNumb, aszDoubRepoName2);

            /* 2nd line */
            /* check minor class */
            if ((pData->uchMinorClass == CLASS_MAINTHEADER_RPT) || (pData->uchMinorClass == CLASS_MAINTHEADER_NOTSEND)) {
                /* print REPORT NAME/REPORT TYPE / SPEMNEMO/RESET TYPE */ 
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupHead2, pData->aszReportName3, aszReportType, pData->aszSpeMnemo, aszResetType);
            }
        }
    }

    /* check print control */
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */
        /* check minor class */
        if (pData->uchMinorClass == CLASS_MAINTHEADER_NOTSEND) {
            return;
        }
        /* 1st line */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupHead1, aszDoubRepoName1, aszDoubRepoNumb);                 /* number */

        /* 2nd line */
        /* check minor class */
        if (pData->uchMinorClass == CLASS_MAINTHEADER_RPT) {
            /* print REPORT NAME */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupHead2, aszDoubRepoName2);
            /* 3rd ine */
            /* print REPORT NAME/REPORT TYPE / SPEMNEMO/RESET TYPE */ 
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupHead3, pData->aszReportName3, aszReportType, pData->aszSpeMnemo, aszResetType);
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupTrailer( MAINTTRAILER *pData )
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

VOID  PrtThrmSupTrailer( MAINTTRAILER *pData )
{
	/* modified for slip printer is not connected case, 09/19/01 */
    PrtPortion(pData->usPrtControl);
    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
        PrtSupSPTrailer(pData);
    } else {
        /* check print control */
        if (pData->usPrtControl & PRT_RECEIPT) {   /* THERMAL PRINTER */
            PrtFeed(PMG_PRT_RECEIPT, 1);        /* 1 line feed */  
            PrtThrmSupFormTrailer(pData);       /* print trailer line */ 
 //           PrtFeed(PMG_PRT_RECEIPT, 5+1);        /* 5 line feed, for 7158, saratoga */  
            //we comment out cut, because the transactionend function does it for us
			PrtTransactionEnd(PRT_REGULARPRINTING,0);
			//PrtCut();                           /* cut */

            /* check if EJ File read/reset report is */
            if (pData->uchMinorClass != CLASS_MAINTTRAILER_NOTSEND) {
                /* send shared data even if shared buffer is not full */
                PrtShrEnd();
            }
        }
    }

    /* check print control */
    if (pData->usPrtControl & PRT_JOURNAL) {  /* EJ */
        PrtEJSupFormTrailer(pData);        /* print trailer line */
    }
    /* initialize receipt header flag */
    fbPrtTHHead = 0;
    PrtRectCompul();		/* set receipt compulsory after pickup/loan slip, 09/19/01 */
}

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupFormTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer line thermal print format.
*===========================================================================
*/

VOID PrtThrmSupFormTrailer(MAINTTRAILER *pData) 
{
    /* print 1st line */
    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTTRAILER_PRTPRG) {    /* program mode */
		const TCHAR FARCONST auchPrtThrmSupTrailer1[] = _T("%-4s%04u\t%-4s%04u    %-4s%04u");               
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer1, pData->aszPrgCountMnemo, pData->usPrgCount, pData->aszPTDRstMnemo, pData->usPTDRstCount, pData->aszDayRstMnemo, pData->usDayRstCount);
    } else {                                                    /* supervisor mode */    
		const TCHAR FARCONST auchPrtThrmSupTrailer2[] = _T("%8.8Mu\t%-4s%04u    %-4s%04u");
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer2, RflTruncateEmployeeNumber(pData->ulSuperNumber), pData->aszPTDRstMnemo, pData->usPTDRstCount, pData->aszDayRstMnemo, pData->usDayRstCount);
    }

    /* print 2nd line */
    /* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
		const TCHAR FARCONST auchPrtThrmSupTrailer3[] = _T("%04u\t%2u:%02u   %02u/%02u/%02u    %04u-%03u");
        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) { /* MM/DD/YY */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMDay, pData->usMonth, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        } else {                                              /* DD/MM/YY */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMonth, pData->usMDay, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        }
    } else {
		const TCHAR FARCONST auchPrtThrmSupTrailer4[] = _T("%04u\t%2u:%02u%s   %02u/%02u/%02u    %04u-%03u");
		USHORT usHour;

        /* check if Hour is '0' */
        if (pData->usHour == 0) {   /* in case of "0:00 AM" */
            usHour = 12;
        } else {
            usHour = pData->usHour;
        }

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {   /* DD/MM/YY */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM,
                      pData->usMDay, pData->usMonth, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        } else {                                                /* MM/DD/YY */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM,
                      pData->usMonth, pData->usMDay, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID  PrtEJSupFormTrailer( MAINTTRAILER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTTRAILER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms trailer line EJ print format.
*===========================================================================
*/

VOID  PrtEJSupFormTrailer( MAINTTRAILER *pData )
{
    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTTRAILER_NOTSEND || pData->uchMinorClass == CLASS_MAINTTRAILER_SENDSHR) {
        return;
    }

    /* print 1st line */
	{
		const TCHAR FARCONST auchPrtSupTrailer1[] = _T("%-4s%04u\t%-4s%04u");
		PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer1, pData->aszDayRstMnemo, pData->usDayRstCount, pData->aszPTDRstMnemo, pData->usPTDRstCount);
	}

    /* print 2nd line */
    /* check minor class */
    if (pData->uchMinorClass == CLASS_MAINTTRAILER_PRTPRG) {    /* program mode */
		const TCHAR FARCONST auchPrtSupTrailer5[] = _T("%-4s%04u\t%04u-%03u");
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer5, pData->aszPrgCountMnemo, pData->usPrgCount, pData->usStoreNumber, pData->usRegNumber);
    } else {    /* supervisor mode */
		const TCHAR FARCONST auchPrtSupTrailer2[] = _T("%8.8Mu\t%04u-%03u");
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer2, pData->ulSuperNumber, pData->usStoreNumber, pData->usRegNumber);
    }
        
    /* print 3rd line */
    /* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
		const TCHAR FARCONST auchPrtSupTrailer3[] = _T("%04u\t%2u:%02u %02u/%02u/%02u");
        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMDay, pData->usMonth, pData->usYear % 100);
        } else {      
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMonth, pData->usMDay, pData->usYear % 100);
      
        }
    } else {
		const TCHAR FARCONST auchPrtSupTrailer4[] = _T("%04u\t%2u:%02u%s %02u/%02u/%02u");
		USHORT  usHour;

        /* check if Hour is '0' */
        if (pData->usHour == 0) {   /* in case of "0:00 AM" */
            usHour = 12;
        } else {
            usHour = pData->usHour;
        }

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {

            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM,
                      pData->usMDay, pData->usMonth, pData->usYear % 100);
        } else {
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM,
                      pData->usMonth, pData->usMDay, pData->usYear % 100);         /* year */
        }
    }

    /* send input data to EJ file */
    PrtEJSend();
}

/***** End of Source *****/
