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
* Title       : Thermal Print ETK File Maintenance Format ( SUPER & PROGRAM MODE ) 
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSETKFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupMaintETKFl()      : ETK print format.
*               PrtThrmSupMakeETKFl() : make print ETK File data
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
/* #include <cswai.h> */
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <csetk.h>
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

TCHAR aszPrThrmInTime[7 + 1];     
TCHAR aszPrThrmOutTime[7 + 1];
TCHAR aszPrThrmDate[5 + 1];
TCHAR aszPrThrmInAMPM[2 + 1];
TCHAR aszPrThrmOutAMPM[2 + 1];
TCHAR aszPrThrmWorkTime[5 + 1 + 7 + 1];     /* R3.1 */

const TCHAR FARCONST auchThrmEtkTime[] = _T("%2u:%02u");
const TCHAR FARCONST auchThrmEtkDate[] = _T("%02u/%02u");
const TCHAR FARCONST auchThrmEtkAMPM[] = _T("%s");

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupMaintETKFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms ETK File print format.
*
*             : EMPLOYEE TIME KEEPING FILE MAINTENANCE  
*
*===========================================================================
*/

VOID  PrtThrmSupMaintETKFl( MAINTETKFL *pData )  
{
    /* define thermal print format */
    static const TCHAR  auchPrtThrmSupMaintETKFl[] = _T("%4s    %2s    %5s     %7s    %7s");

    /* define EJ print format */
    static const TCHAR  auchPrtSupMaintETKFl[] = _T("%4s  %2s  %5s  %7s\t%7s");

    /* define common print format */
    static const TCHAR  auchNumber[] = _T("%d");

    TCHAR  aszBuffer[3] = { 0, 0, 0 };
	TCHAR  aszDoubBlockNo[2 * 2 + 1] = { 0 };
	TCHAR  aszBlockNo[2 + 1] = { 0 };
       
    /* convert Block No. to double wide */
    RflSPrintf(aszBlockNo, TCHARSIZEOF(aszBlockNo), auchNumber, pData->usBlockNo);
    PrtDouble(aszDoubBlockNo, TCHARSIZEOF(aszDoubBlockNo), aszBlockNo);

    /* make data */
    PrtThrmSupMakeETKFl(pData);

    if(pData->EtkField.uchJobCode) {
        _itot(pData->EtkField.uchJobCode, aszBuffer, 10);
    }

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupMaintETKFl,     /* format */
                  aszDoubBlockNo,               /* block number */
                  aszBuffer,                    /* job code */
                  aszPrThrmDate,                /* date */
                  aszPrThrmInTime,              /* time-in */
                  aszPrThrmOutTime);            /* time-out */
    } 

    /* check print control */
    if (pData->usPrintControl & PRT_JOURNAL) {
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupMaintETKFl,        /* format */
                  aszDoubBlockNo,              /* block number */
                  aszBuffer,                   /* job code */
                  aszPrThrmDate,               /* date */
                  aszPrThrmInTime,             /* time-in */
                  aszPrThrmOutTime);           /* time-out */
    }
}           

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupMakeETKFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes ETK File string data.
*===========================================================================
*/

VOID PrtThrmSupMakeETKFl(MAINTETKFL *pData) 
{
    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usInHour;
    USHORT usOutHour;

    /* initialize */
    memset(aszPrThrmInTime, '\0', sizeof(aszPrThrmInTime));
    memset(aszPrThrmOutTime, '\0', sizeof(aszPrThrmOutTime));
    memset(aszPrThrmInAMPM, '\0', sizeof(aszPrThrmInAMPM));
    memset(aszPrThrmOutAMPM, '\0', sizeof(aszPrThrmOutAMPM));
    memset(aszPrThrmDate, '\0', sizeof(aszPrThrmDate));

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

        RflSPrintf(aszPrThrmInAMPM, TCHARSIZEOF(aszPrThrmInAMPM), auchThrmEtkAMPM, aszPrtNull); 
        RflSPrintf(aszPrThrmOutAMPM, TCHARSIZEOF(aszPrThrmOutAMPM), auchThrmEtkAMPM, aszPrtNull); 
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
                RflSPrintf(aszPrThrmInAMPM, TCHARSIZEOF(aszPrThrmInAMPM), auchThrmEtkAMPM, aszPrtPM);          /* set PM */
            } else {
                RflSPrintf(aszPrThrmInAMPM, TCHARSIZEOF(aszPrThrmInAMPM), auchThrmEtkAMPM, aszPrtAM);          /* set AM */
            }
        } else {
            tcharnset(aszPrThrmInAMPM, _T('*'), 2);
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
                RflSPrintf(aszPrThrmOutAMPM, TCHARSIZEOF(aszPrThrmOutAMPM), auchThrmEtkAMPM, aszPrtPM);             /* set PM */
            } else {
                RflSPrintf(aszPrThrmOutAMPM, TCHARSIZEOF(aszPrThrmOutAMPM), auchThrmEtkAMPM, aszPrtAM);             /* set AM */
            }
        } else {
            tcharnset(aszPrThrmOutAMPM, _T('*'), 2);
        }
    }

    /* check if TIME-IN is illegal */
    if (pData->EtkField.usTimeinTime == ETK_TIME_NOT_IN) {
        tcharnset(aszPrThrmInTime, _T('*'), 5);
    }

    /* check if TIME-OUT is illegal */
    if (pData->EtkField.usTimeOutTime == ETK_TIME_NOT_IN) {
        tcharnset(aszPrThrmOutTime, _T('*'), 5);
    }

    /* check if TIME-IN is exist */
    if (pData->EtkField.usTimeinTime != ETK_TIME_NOT_IN) {
        /* set TIME-IN */
        RflSPrintf(aszPrThrmInTime, TCHARSIZEOF(aszPrThrmInTime), auchThrmEtkTime, usInHour, pData->EtkField.usTimeinMinute);
    }

    /* check if TIME-OUT is exist */
    if (pData->EtkField.usTimeOutTime != ETK_TIME_NOT_IN) {
        /* set TIME-OUT */
        RflSPrintf(aszPrThrmOutTime, TCHARSIZEOF(aszPrThrmOutTime), auchThrmEtkTime, usOutHour, pData->EtkField.usTimeOutMinute);
    }
    _tcscat(aszPrThrmInTime, aszPrThrmInAMPM);   
    _tcscat(aszPrThrmOutTime, aszPrThrmOutAMPM);   

    /* check MDC */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszPrThrmDate, TCHARSIZEOF(aszPrThrmDate), auchThrmEtkDate, pData->EtkField.usDay, pData->EtkField.usMonth);
    } else {
        RflSPrintf(aszPrThrmDate, TCHARSIZEOF(aszPrThrmDate), auchThrmEtkDate, pData->EtkField.usMonth, pData->EtkField.usDay);
    }
}

/***** End of Source *****/
