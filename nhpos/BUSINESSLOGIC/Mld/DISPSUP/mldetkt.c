/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Employee File Format ( SUPER & PROGRAM MODE ) 
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program 
* Program Name: MLDETKT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupETKFile()    : ETK print format.
*               MldRptSupMakeString() : make print data
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jul-27-95 : 03.00.03 : M.Ozawa   : Initial             
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
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csstbpar.h>
#include <csttl.h>
#include <csop.h>
#include <csetk.h>
#include <report.h>
#include <pmg.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldmenu.h>
#include <mldlocal.h>


/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/


extern const TCHAR FARCONST auchTime[];
extern const TCHAR FARCONST auchDate[];
extern const TCHAR FARCONST auchAMPM[];

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupETKFile( RPTEMPLOYEE *pData )  
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
*
*===========================================================================
*/

SHORT  MldRptSupETKFile( RPTEMPLOYEE *pData )  
{
    /* define thermal print format */
    static const TCHAR  auchMldRptSupETKFile1[] = _T("%6s%-8.8s %-8.8s");/*8 characters JHHJ*/
    static const TCHAR  auchMldRptSupETKFile2[] = _T("%5s %7s %7s %2s\t%13s");/*8 characters JHHJ*/
    static const TCHAR  auchMldRptSupETKFile3[] = _T("%-8.8s\t%5u:%02u %7l$");/*8 characters JHHJ*/
    static const TCHAR  auchMldRptSupETKFile4[] = _T("%-8.8s\t***** *******");/*8 characters JHHJ*/

	TCHAR aszMldInTime[7 + 1] = { 0 };
	TCHAR aszMldOutTime[7 + 1] = { 0 };
	TCHAR aszMldDate[5 + 1] = { 0 };
	TCHAR aszMldInAMPM[2 + 1] = { 0 };
	TCHAR aszMldOutAMPM[2 + 1] = { 0 };
	TCHAR aszMldWorkTime[5 + 1 + 7 + 1] = { 0 };     /* R3.1 */

	/* See NHPOS_ASSERT() check on usRow value as well. */
	TCHAR  aszString[3 * (MLD_SUPER_MODE_CLM_LEN+1)] = {0};           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {'\0'};
    TCHAR  aszBuffer[3] = { 0, 0, 0 };
    USHORT usRow=0;

    /* check minor class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTEMPLOYEE_PRTMNEMO:      
        /* print TRANS MNEMO */
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupETKFile1,      /* format */
                  aszPrtNull,                  /* null */
                  pData->aszTimeinMnemo,       /* mnemonic(TIME-IN) */
                  pData->aszTimeoutMnemo);     /* mnemonic(TIME-OUT) */
   
        break;

    case CLASS_RPTEMPLOYEE_PRTTIME:      
    case CLASS_RPTEMPLOYEE_PRTNOOUT:    /* without TIME-OUT */      
    case CLASS_RPTEMPLOYEE_PRTNOIN:     /* without TIME-IN */        
        /* make data */
        MldRptSupMakeString(pData);

        if(pData->uchJobCode) {
            _itot(pData->uchJobCode, aszBuffer, 10);
        }

        /* print DATE/TIME/JOB CODE/WORKED HOURS */
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupETKFile2,      /* format */
                  aszMldDate,               /* date */
                  aszMldInTime,             /* time-in */
                  aszMldOutTime,            /* time-out */
                  aszBuffer,                   /* job code */
                  aszMldWorkTime);          /* work time */

        break;

    case CLASS_RPTEMPLOYEE_PRTWKTTL:   
        /* print WORK TOTAL */
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupETKFile3,      /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usWorkTime,           /* total work hour */
                  pData->usWorkMinute,         /* total work minute */
				 (DCURRENCY)pData->ulWorkWage);  /* total work wage, format of %7l$ requires DCURRENCY */
        break;

    case CLASS_RPTEMPLOYEE_PRTTOTAL:  
        /* print TOTAL */
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupETKFile3,      /* format */
                  pData->aszTimeinMnemo,       /* mnemonic(TOTAL) */
                  pData->usTotalTime,          /* total hour */
                  pData->usTotalMinute,        /* total minute */
                  (DCURRENCY)pData->ulTotalWage);  /* total wage, format of %7l$ requires DCURRENCY */
        break;

    case CLASS_RPTEMPLOYEE_PRTWKNOTTL: 
    case CLASS_RPTEMPLOYEE_PRTNOTOTAL:
        /* print ILLEGAL WORK TOTAL/TOTAL */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupETKFile4, pData->aszTimeinMnemo);
        break;

    default:
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_02);
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);
    }

    if (!usRow) {
        return (MLD_SUCCESS);               /* if format is not created */
    }

    /* check format data is displayed in the scroll page */
    if (usRow + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
        MldScroll1Buff.uchCurItem = 0;  /* new page */
        /* create dialog */
        if (MldContinueCheck() == MLD_ABORTED) {
            return (MLD_ABORTED);
        }
    }

	NHPOS_ASSERT(usRow < 3);         // Assert that usRow is less than or equal to size of aszString;

    MldScroll1Buff.uchCurItem += (UCHAR)usRow;
    pszString = &aszString[0];
    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }

    return (MLD_SUCCESS);
}           

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupMakeString( RPTEMPLOYEE *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTEMPLOYEE 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes string data.
*===========================================================================
*/
VOID MldRptSupMakeString(RPTEMPLOYEE *pData) 
{
    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usInHour;
    USHORT usOutHour;

	TCHAR aszMldInTime[7 + 1] = { 0 };
	TCHAR aszMldOutTime[7 + 1] = { 0 };
	TCHAR aszMldDate[5 + 1] = { 0 };
	TCHAR aszMldInAMPM[2 + 1] = { 0 };
	TCHAR aszMldOutAMPM[2 + 1] = { 0 };
	TCHAR aszMldWorkTime[5 + 1 + 7 + 1] = { 0 };     /* R3.1 */


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

        RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchAMPM, aszPrtNull); 
        RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchAMPM, aszPrtNull); 
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
                RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchAMPM, aszPrtPM);          /* set PM */
            } else {
                RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchAMPM, aszPrtAM);          /* set AM */
            }
        } else {
            tcharnset(aszMldInAMPM, _T('*'), 2);
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
                RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchAMPM, aszPrtPM);             /* set PM */
            } else {
                RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchAMPM, aszPrtAM);             /* set AM */
            }
        } else {
            tcharnset(aszMldOutAMPM, _T('*'), 2);
        }
    }

    /* check if TIME-IN is Illegal */
    if (pData->usTimeInTime == ETK_TIME_NOT_IN) {
        tcharnset(aszMldInTime, _T('*'), 5);
        tcharnset(aszMldWorkTime, _T('*'), 13);
        aszMldWorkTime[5] = _T(' ');
    }

    /* check if TIME-OUT is illegal */
    if (pData->usTimeOutTime == ETK_TIME_NOT_IN) {
        tcharnset(aszMldOutTime, _T('*'), 5);
        tcharnset(aszMldWorkTime, _T('*'), 13);
        aszMldWorkTime[5] = _T(' ');
    }

    /* check if TIME-IN is exist */
    if (pData->usTimeInTime != ETK_TIME_NOT_IN) {
        /* set TIME-IN */
        RflSPrintf(aszMldInTime, TCHARSIZEOF(aszMldInTime), auchTime, usInHour, pData->usTimeInMinute);     
    }

    /* check if TIME-OUT is exist */
    if (pData->usTimeOutTime != ETK_TIME_NOT_IN) {
        /* set TIME-OUT */
        RflSPrintf(aszMldOutTime, TCHARSIZEOF(aszMldOutTime), auchTime, usOutHour, pData->usTimeOutMinute);    
    }

    /* check if WORK TIME is exist */
    if (pData->uchMinorClass == CLASS_RPTEMPLOYEE_PRTTIME) {
		static const TCHAR FARCONST auchTimeWage[] = _T("%2u:%02u %7l$");

		/* set WORK TIME */
        RflSPrintf(aszMldWorkTime, TCHARSIZEOF(aszMldWorkTime), auchTimeWage, pData->usWorkTime, pData->usWorkMinute, (DCURRENCY)pData->ulWorkWage);
    }

    _tcscat(aszMldInTime, aszMldInAMPM);   
    _tcscat(aszMldOutTime, aszMldOutAMPM);   

    /* check MDC */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszMldDate, TCHARSIZEOF(aszMldDate), auchDate, pData->usDay, pData->usMonth);
    } else {    /* MM/DD/YY */
        RflSPrintf(aszMldDate, TCHARSIZEOF(aszMldDate), auchDate, pData->usMonth, pData->usDay);    
    }
}

/***** End of Source *****/
