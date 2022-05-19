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
* Title       : Print Employee File Format ( SUPER & PROGRAM MODE ) 
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSETK_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupETKFile()    : ETK print format.
*               PrtSupMakeData() : make print data
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-30-93 : 01.00.02 : J.IKEDA   : Initial                                   
* Nov-27-02 :          : R.Chambers: Add PrtSupETKFileJCSummary for LePeeps SR07
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
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csetk.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
/* #include "prtrin.h" */
#include "prtsin.h"


/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

const TCHAR FARCONST auchTime[] = _T("%2u:%02u");
const TCHAR FARCONST auchDate[] = _T("%02u/%02u");
const TCHAR FARCONST auchAMPM[] = _T("%s");

/*
*===========================================================================
** Synopsis:    VOID  PrtSupETKFile( RPTEMPLOYEE *pData )  
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

VOID  PrtSupETKFile( RPTEMPLOYEE *pData )  
{
    /* define print format */
    static const TCHAR  auchPrtSupETKFile1[] = _T("\t%-8.8s  %-8.8s");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupETKFile2[] = _T("%5s  %7s\t%7s\n\t%2s %13s");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupETKFile3[] = _T("%-8.8s\t%5u:%02u %7l$");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupETKFile4[] = _T("%-8.8s\t***** *******");/*8 characters JHHJ*/
    static const TCHAR  auchNumber[] = _T("%d");

    TCHAR   aszBuffer[3] = { 0, 0, 0 };
    USHORT  usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* check minor class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTEMPLOYEE_PRTMNEMO:      
        /* print TRANS MNEMO */
        PmgPrintf(usPrtType, auchPrtSupETKFile1,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TIME-IN) */
                  pData->aszTimeoutMnemo);     /* mnemonic(TIME-OUT) */
       
        break;

    case CLASS_RPTEMPLOYEE_PRTTIME:      
    case CLASS_RPTEMPLOYEE_PRTNOOUT:    /* without TIME-OUT */      
    case CLASS_RPTEMPLOYEE_PRTNOIN:     /* without TIME-IN */      
        /* make data */
        PrtSupMakeData(pData);

        if (pData->uchJobCode) {
            _itot(pData->uchJobCode, aszBuffer, 10);
        }

        /* print DATE/TIME/JOB CODE/WORKED HOURS */
        PmgPrintf(usPrtType, auchPrtSupETKFile2,          /* format */
                  aszPrtDate,                  /* date */
                  aszPrtInTime,                /* time-in */
                  aszPrtOutTime,               /* time-out */
                  aszBuffer,                   /* job code */
                  aszPrtWorkTime);             /* work time */

        break;

    case CLASS_RPTEMPLOYEE_PRTWKTTL:   
        /* print WORK TOTAL */
        PmgPrintf(usPrtType, auchPrtSupETKFile3,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usWorkTime,           /* total work hour */
                  pData->usWorkMinute,         /* total work minute */
				(DCURRENCY)pData->ulWorkWage);          /* total work wage */
        break;

    case CLASS_RPTEMPLOYEE_PRTTOTAL:  
        /* print TOTAL */
        PmgPrintf(usPrtType, auchPrtSupETKFile3,          /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usTotalTime,          /* total hour */
                  pData->usTotalMinute,        /* total minute */
				(DCURRENCY)pData->ulTotalWage);         /* total wage */
        break;

    case CLASS_RPTEMPLOYEE_PRTWKNOTTL: 
    case CLASS_RPTEMPLOYEE_PRTNOTOTAL:
        /* print TOTAL/ILLEGAL TOTAL WORKED HOURS */
        PmgPrintf(usPrtType, auchPrtSupETKFile4, pData->aszTimeinMnemo);      /* mnemonic(TOTAL) */
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
} 


/*
*===========================================================================
** Synopsis:    VOID  PrtSupETKFileJCSummary( PRPTJCSUMMARY pData )  
*               
*     Input:    *pData  : pointer to structure for RPTJCSUMMARY 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints out the job code summary
*               information.  It prints out the following data fields
*                   - Job code number
*                   - wage rate for job code
*                   - total hours in job code
*                   - total minutes in job code
*                   - total wages in job code 
*
* The printout format for the line items is designed to be 25 characters
* long thereby allowing the Header/Special Information (P57) mnemonic
* that we are using for the header to the list to be formatted.  This
* message length is 24 characters.
*
* Any changes made here should be reflected in PrtThrmSupETKFileJCSummary
* in file BusinessLogic\Print\Thermsup\prsetkt.c as well.
*
*===========================================================================
*/

VOID  PrtSupETKFileJCSummary( PRPTJCSUMMARY  pData )  
{
    /* define print formats for line items and the title */
	//                                                       JC   Wage Hr:Mn   Cost
    static const TCHAR auchPrtSupETKFile[]       = _T("%2.2d %6$ %4u:%02u %7$");
    static const TCHAR auchPrtSupETKFileTitle[]  = _T("%-s");
    static const TCHAR auchPrtSupETKFileTotals[] = _T("          %4u:%02u %7$");
    USHORT  usPrtType;

	// We use the print format above in variable auchPrtSupETKFile1
	// to determine the field sizes and types.

    /* check print control */

	usPrtType = PrtSupChkType(pData->usPrintControl);
       
	if (pData->uchMajorClass == CLASS_RPTJCSUMMARYTITLES) {
		PARACHAR24  Char24Data;
        Char24Data.uchStatus = 0;                           /* Set W/ Amount Status */
        Char24Data.uchAddress = CH24_JCSUMTITLE_ADR;        /* Set Address 1 */
        Char24Data.uchMajorClass = CLASS_PARACHAR24;        /* Set Major Class */
        ParaChar24Read(&Char24Data);                        /* Execute Read Procedure */
		PmgPrintf(usPrtType, auchPrtSupETKFileTitle, Char24Data.aszMnemonics);
	}
	else if (pData->uchMajorClass == CLASS_RPTJCSUMMARYTOTALS) {
		PmgPrintf(usPrtType, auchPrtSupETKFileTotals, 
				pData->usTotalTime,      /* hours of time for the job code */
				pData->usTotalMinute,    /* minutes of time for the job code */
				(DCURRENCY)pData->ulTotalWage);     /* total wage for the job code*/
	}
	else {
		PmgPrintf(usPrtType, auchPrtSupETKFile, 
				pData->uchJobCode,       /* the job code */
				(DCURRENCY)pData->usLaborCost,      /* wage cost for the job code */
				pData->usTotalTime,      /* hours of time for the job code */
				pData->usTotalMinute,    /* minutes of time for the job code */
				(DCURRENCY)pData->ulTotalWage);     /* total wage for the job code*/
	}
}

/*
*===========================================================================
** Synopsis:    VOID  PrtSupMakeData( RPTEMPLOYEE *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTEMPLOYEE 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes data.
*===========================================================================
*/

VOID PrtSupMakeData(RPTEMPLOYEE *pData) 
{
    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usInHour;
    USHORT usOutHour;

    /* initialize */
    memset(aszPrtInTime, '\0', sizeof(aszPrtInTime));
    memset(aszPrtOutTime, '\0', sizeof(aszPrtOutTime));
    memset(aszPrtWorkTime, '\0', sizeof(aszPrtWorkTime));
    memset(aszPrtInAMPM, '\0', sizeof(aszPrtInAMPM));
    memset(aszPrtOutAMPM, '\0', sizeof(aszPrtOutAMPM));
    memset(aszPrtDate, '\0', sizeof(aszPrtDate));

    /* check time */    
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* MILITARY type */
        /* check if TIME-IN is exist */
        if (pData->usTimeInTime != ETK_TIME_NOT_IN) {
            usInHour = pData->usTimeInTime;
        }

        /* check if TIME-OUT is exist */
        if (pData->usTimeOutTime != ETK_TIME_NOT_IN) {
            usOutHour = pData->usTimeOutTime;
        }

        RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchAMPM, aszPrtNull); 
        RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchAMPM, aszPrtNull); 
    } else {    /* AM/PM type */
        /* check if TIME-IN is exist */
        if (pData->usTimeInTime != ETK_TIME_NOT_IN) {
            /* check if Time-in Hour is '0' */
            if (pData->usTimeInTime == 0) {   /* in case of "0:00 AM" */
                usInHour = 12;
            } else {
                usInHour = pData->usTimeInTime;
            }

            if (usInHour > 12) {
                usInHour -= 12;
            }

            /* AM/PM ? */
            if (pData->usTimeInTime >= 12) {
                RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchAMPM, aszPrtPM);          /* set PM */
            } else {
                RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchAMPM, aszPrtAM);          /* set AM */
            }
        } else {
            tcharnset(aszPrtInAMPM, _T('*'), 2);
        }

        /* check if TIME-OUT is exist */
        if (pData->usTimeOutTime != ETK_TIME_NOT_IN) {
            /* check if Time-out Hour is '0' */
            if (pData->usTimeOutTime == 0) {   /* in case of "0:00 AM" */
                usOutHour = 12;
            } else {
                usOutHour = pData->usTimeOutTime;
            }

            if (usOutHour > 12) {
                usOutHour -= 12;
            }

            /* AM/PM ? */
            if (pData->usTimeOutTime >= 12) {
                RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchAMPM, aszPrtPM);             /* set PM */
            } else {
                RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchAMPM, aszPrtAM);             /* set AM */
            }
        } else {
            tcharnset(aszPrtOutAMPM, _T('*'), 2);
        }
    }

    /* check if TIME-IN is Illegal */
    if (pData->usTimeInTime == ETK_TIME_NOT_IN) {
        tcharnset(aszPrtInTime, _T('*'), 5);
        tcharnset(aszPrtWorkTime, _T('*'), 13);
        aszPrtWorkTime[5] = (' ');
    }

    /* check if TIME-OUT is illegal */
    if (pData->usTimeOutTime == ETK_TIME_NOT_IN) {
        tcharnset(aszPrtOutTime, _T('*'), 5);
        tcharnset(aszPrtWorkTime, _T('*'), 13);
        aszPrtWorkTime[5] = _T(' ');
    }

    /* check if TIME-IN is exist */
    if (pData->usTimeInTime != ETK_TIME_NOT_IN) {
        /* set TIME-IN */
        RflSPrintf(aszPrtInTime, TCHARSIZEOF(aszPrtInTime), auchTime, usInHour, pData->usTimeInMinute);     
    }

    /* check if TIME-OUT is exist */
    if (pData->usTimeOutTime != ETK_TIME_NOT_IN) {
        /* set TIME-OUT */
        RflSPrintf(aszPrtOutTime, TCHARSIZEOF(aszPrtOutTime), auchTime, usOutHour, pData->usTimeOutMinute);    
    }

    /* check if WORK TIME is exist */
    if (pData->uchMinorClass == CLASS_RPTEMPLOYEE_PRTTIME) {
		static const TCHAR FARCONST auchTimeWage[] = _T("%2u:%02u %7l$");

		/* set WORK TIME */
        RflSPrintf(aszPrtWorkTime, TCHARSIZEOF(aszPrtWorkTime), auchTimeWage, 
                   pData->usWorkTime,
                   pData->usWorkMinute,    
                   (DCURRENCY)pData->ulWorkWage);
    }

    _tcscat(aszPrtInTime, aszPrtInAMPM);   
    _tcscat(aszPrtOutTime, aszPrtOutAMPM);   

    /* check MDC */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszPrtDate, TCHARSIZEOF(aszPrtDate), auchDate, pData->usDay, pData->usMonth);
    } else {    /* MM/DD/YY */
        RflSPrintf(aszPrtDate, TCHARSIZEOF(aszPrtDate), auchDate, pData->usMonth, pData->usDay);    
    }
}
/***** End of Source *****/
