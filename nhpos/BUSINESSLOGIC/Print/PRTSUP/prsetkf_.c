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
* Program Name: PRSETKF_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupMaintETKFl()    : ETK print format.
*               PrtSupMakeEtkFl()     : make print ETK File data
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Oct-04-93 : 02.00.01 : J.IKEDA   : Initial                                   
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
#include <csetk.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"


/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

TCHAR aszPrtInTime[7 + 1];
TCHAR aszPrtOutTime[7 + 1];
TCHAR aszPrtInAMPM[2 + 1];
TCHAR aszPrtOutAMPM[2 + 1];
TCHAR aszPrtDate[5 + 1];
TCHAR aszPrtWorkTime[5 + 1 + 7 + 1];    /* R3.1 */

const TCHAR FARCONST auchEtkTime[] = _T("%2u:%02u");
const TCHAR FARCONST auchEtkDate[] = _T("%02u/%02u");
const TCHAR FARCONST auchEtkAMPM[] = _T("%s");

/*
*===========================================================================
** Synopsis:    VOID  PrtSupMaintETKFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms ETK print format.
*
*             : EMPLOYEE TIME KEEPING FILE MAINTENANCE  
*
*===========================================================================
*/

VOID  PrtSupMaintETKFl( MAINTETKFL *pData )  
{
    /* define print format */
    static const TCHAR FARCONST auchPrtSupMaintETKFl[] = _T("%4s  %2s  %5s  %7s\n\t%7s");
    static const TCHAR FARCONST auchNumber[] = _T("%d");

    TCHAR   aszBuffer[3] = { 0, 0, 0 };
	TCHAR   aszDoubBlockNo[2 * 2 + 1] = { 0 };
	TCHAR   aszBlockNo[2 + 1] = { 0 };
    USHORT  usPrtType;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* make data */
    PrtSupMakeEtkFl(pData);

    if(pData->EtkField.uchJobCode) {
        _itot(pData->EtkField.uchJobCode, aszBuffer, 10);
    }

    /* convert Block No. to double wide */
    RflSPrintf(aszBlockNo, TCHARSIZEOF(aszBlockNo), auchNumber, pData->usBlockNo);
    PrtDouble(aszDoubBlockNo, (2 * 2 +1), aszBlockNo);

    PmgPrintf(usPrtType, auchPrtSupMaintETKFl,        /* format */
              aszDoubBlockNo,              /* block number */
              aszBuffer,                   /* job code */
              aszPrtDate,                  /* date */
              aszPrtInTime,                /* time-in */
              aszPrtOutTime);              /* time-out */
}

/*
*===========================================================================
** Synopsis:    VOID  PrtSupMakeEtkFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes ETK File data.
*===========================================================================
*/

VOID PrtSupMakeEtkFl(MAINTETKFL *pData) 
{
    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usInHour;
    USHORT usOutHour;

    /* initialize */
    memset(aszPrtInTime, '\0', sizeof(aszPrtInTime));
    memset(aszPrtOutTime, '\0', sizeof(aszPrtOutTime));
    memset(aszPrtInAMPM, '\0', sizeof(aszPrtInAMPM));
    memset(aszPrtOutAMPM, '\0', sizeof(aszPrtOutAMPM));
    memset(aszPrtDate, '\0', sizeof(aszPrtDate));

    /* check time */    
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* MILITARY type */
        /* check if TIME-IN is exist */
        if (pData->EtkField.usTimeinTime != ETK_TIME_NOT_IN) {
            usInHour = pData->EtkField.usTimeinTime;
        }

        /* check if TIME-OUT is exist */
        if (pData->EtkField.usTimeOutTime != ETK_TIME_NOT_IN) {
            usOutHour = pData->EtkField.usTimeOutTime;
        }

        RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchEtkAMPM, aszPrtNull); 
        RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchEtkAMPM, aszPrtNull); 
    } else {    /* AM/PM type */
        /* check if TIME-IN is exist */
        if (pData->EtkField.usTimeinTime != ETK_TIME_NOT_IN) {
            /* check if Time-in Hour is '0' */
            if (pData->EtkField.usTimeinTime == 0) {   /* in case of "0:00 AM" */
                usInHour = 12;
            } else {
                usInHour = pData->EtkField.usTimeinTime;
            }

            if (usInHour > 12) {
                usInHour -= 12;
            }

            /* AM/PM ? */
            if (pData->EtkField.usTimeinTime >= 12) {
                RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchEtkAMPM, aszPrtPM);          /* set PM */
            } else {
                RflSPrintf(aszPrtInAMPM, TCHARSIZEOF(aszPrtInAMPM), auchEtkAMPM, aszPrtAM);          /* set AM */
            }
        } else {
			tcharnset(aszPrtInAMPM, '*', 2);
        }

        /* check if TIME-OUT is exist */
        if (pData->EtkField.usTimeOutTime != ETK_TIME_NOT_IN) {
            /* check if Time-out Hour is '0' */
            if (pData->EtkField.usTimeOutTime == 0) {   /* in case of "0:00 AM" */
                usOutHour = 12;
            } else {
                usOutHour = pData->EtkField.usTimeOutTime;
            }

            if (usOutHour > 12) {
                usOutHour -= 12;
            }

            /* AM/PM ? */
            if (pData->EtkField.usTimeOutTime >= 12) {
                RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchEtkAMPM, aszPrtPM);             /* set PM */
            } else {
                RflSPrintf(aszPrtOutAMPM, TCHARSIZEOF(aszPrtOutAMPM), auchEtkAMPM, aszPrtAM);             /* set AM */
            }
        } else {
			tcharnset(aszPrtOutAMPM, '*', 2);
        }
    }

    /* check if TIME-IN is Illegal */
    if (pData->EtkField.usTimeinTime == ETK_TIME_NOT_IN) {
		tcharnset (aszPrtInTime, '*', 5);
    }

    /* check if TIME-OUT is illegal */
    if (pData->EtkField.usTimeOutTime == ETK_TIME_NOT_IN) {
		tcharnset (aszPrtOutTime, '*', 5);
    }

    /* check if TIME-IN is exist */
    if (pData->EtkField.usTimeinTime != ETK_TIME_NOT_IN) {
        /* set TIME-IN */
        RflSPrintf(aszPrtInTime, TCHARSIZEOF(aszPrtInTime), auchEtkTime, usInHour, pData->EtkField.usTimeinMinute);
    }

    /* check if TIME-OUT is exist */
    if (pData->EtkField.usTimeOutTime != ETK_TIME_NOT_IN) {
        /* set TIME-OUT */
        RflSPrintf(aszPrtOutTime, TCHARSIZEOF(aszPrtOutTime), auchEtkTime, usOutHour, pData->EtkField.usTimeOutMinute);
    }
    _tcscat(aszPrtInTime, aszPrtInAMPM);   
    _tcscat(aszPrtOutTime, aszPrtOutAMPM);   

    /* check MDC */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszPrtDate, TCHARSIZEOF(aszPrtDate), auchEtkDate, pData->EtkField.usDay, pData->EtkField.usMonth);
    } else {
        RflSPrintf(aszPrtDate, TCHARSIZEOF(aszPrtDate), auchEtkDate, pData->EtkField.usMonth, pData->EtkField.usDay);
    }
}
/***** End of Source *****/
