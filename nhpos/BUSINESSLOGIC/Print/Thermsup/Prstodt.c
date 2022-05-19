/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print TOD Report Format  ( SUPER & PROGRAM MODE )
* Category    : Print, NCR 2170 GP R2.0 Application Program
* Program Name: PrsTodT.C
* --------------------------------------------------------------------------
* Abstruct: This function forms common function.
*
*           The provided function names are as follows:
*
*               PrtSupTOD_TH()     : form tod print format
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-25-97: 02.00.04 : S.Kubota  : Initial
* Jan-07-00: 01.00.00 : H.Endo    : add WeekOfDay data 
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
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <pmg.h>
#include <csop.h>
#include <report.h>
#include "prtcom.h"
#include "prtsin.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A
;=============================================================================
*/


/*
*===========================================================================
** Synopsis:    VOID  PrtSupTOD_TH( PARATOD *pData )
*
*     Input:    *pData    : pointer to structure for PARATOD
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms tod report format.
*===========================================================================
*/

VOID PrtThrmSupTOD(PARATOD *pData)
{
	/*endo update(add) 2000/1/7*/
	static TCHAR const szSUN[] = _T("SUN\0");
	static TCHAR const szMON[] = _T("MON\0");
	static TCHAR const szTUS[] = _T("TUS\0");
	static TCHAR const szWED[] = _T("WED\0");
	static TCHAR const szTHR[] = _T("THR\0");
	static TCHAR const szFRI[] = _T("FRI\0");
	static TCHAR const szSAT[] = _T("SAT\0");

	const TCHAR  auchPrtSupTHTOD1[] = _T("      %02u/%02u/%02u  %-3s%14u:%02u");  /* R2.0 */
																				  // Above was previously -> _T("%8u/%02u/%02u  %-3s%14u:%02u") -CSMALL
																				  // SR 984 - Format change to show leading zeros for dates
	const TCHAR  auchPrtSupTHTOD2[] = _T("      %02u/%02u/%02u  %-3s%14u:%02u%s");/* R2.0 */
																				  // Above was previously -> _T("%8u/%02u/%02u  %-3s%14u:%02u%s")
																				  // SR 984 - Format change to show leading zeros for dates
	const TCHAR  auchPrtSupEJTOD1[] = _T("%02u/%02u/%02u  %-3s\t%2u:%02u");
	const TCHAR  auchPrtSupEJTOD2[] = _T("%02u/%02u/%02u  %-3s\t%2u:%02u%s");

	USHORT usHour;
	TCHAR  aszMnem[8] = { 0 };        /*endo update(change size) 2000/1/7*/

    /* set day of week mnemonic */
    switch(pData->usWDay){
    case TOD_WEEK_SUN:
        _tcsncpy(aszMnem, szSUN, _tcslen(szSUN));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_MON:
        _tcsncpy(aszMnem, szMON, _tcslen(szMON));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_TUS:
        _tcsncpy(aszMnem, szTUS, _tcslen(szTUS));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_WED:
        _tcsncpy(aszMnem, szWED, _tcslen(szWED));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_THR:
        _tcsncpy(aszMnem, szTHR, _tcslen(szTHR));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_FRI:
        _tcsncpy(aszMnem, szFRI, _tcslen(szFRI));/*endo update 2000/1/7 */
        break;
    case TOD_WEEK_SAT:
        _tcsncpy(aszMnem, szSAT, _tcslen(szSAT));/*endo update 2000/1/7 */
        break;
    }

    /* check time */
    if ( CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3) ) {
        /* check date */
        if ( CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2) ) {
            if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
                PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupTHTOD1,             /* format R2.0  */
                      pData->usMDay,                /* day          */
                      pData->usMonth,               /* month        */
                      (pData->usYear % 100),        /* year         */
                      aszMnem,                     /* day of week  */
                      pData->usHour,                /* hour         */
                      pData->usMin);                /* minutes      */
            }
            if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
                PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                          auchPrtSupEJTOD1,               /* format */
                          pData->usMDay,                /* day */
                          pData->usMonth,               /* month */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          pData->usHour,                /* hour */
                          pData->usMin);                /* minutes */
            }
        }
        else {
            if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
                PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupTHTOD1,             /* format R2.0  */
                      pData->usMonth,               /* month        */
                      pData->usMDay,                /* day          */
                      (pData->usYear % 100),        /* year         */
                      aszMnem,                     /* day of week  */
                      pData->usHour,                /* hour         */
                      pData->usMin);                /* minutes      */
            }
            if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
                PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                          auchPrtSupEJTOD1,               /* format */
                          pData->usMonth,               /* month */
                          pData->usMDay,                /* day */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          pData->usHour,                /* hour */
                          pData->usMin);                /* minutes */
            }
        }
    }
    else {
        /* check if Hour is '0' */
        if ( pData->usHour == 0 ) { /* in case of "0:00 AM" */
            usHour = 12;
        }
        else {
            usHour = pData->usHour;
        }

        /* check date */
        if ( CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2) ) {
            if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
                PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupTHTOD2,                             /* format R2.0  */
                      pData->usMDay,                                /* day          */
                      pData->usMonth,                               /* month        */
                      pData->usYear % 100,                          /* year         */
                      aszMnem,                                     /* day of week  */
                      ((usHour > 12)? usHour - 12 : usHour),        /* hour         */
                      pData->usMin,                                 /* minutes      */
                      ((pData->usHour >= 12)? aszPrtPM : aszPrtAM));/* AM/PM        */
            }
            if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
                PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                          auchPrtSupEJTOD2,              /* format */
                          pData->usMDay,                /* day */
                          pData->usMonth,               /* month */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->usMin,                 /* minutes */
                          (pData->usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
            }
        }
        else {
            if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
                PrtPrintf(PMG_PRT_RECEIPT,                  /* printer type */
                      auchPrtSupTHTOD2,                             /* format R2.0  */
                      pData->usMonth,                               /* month        */
                      pData->usMDay,                                /* day          */
                      pData->usYear % 100,                          /* year         */
                      aszMnem,                                     /* day of week  */
                      ((usHour > 12)? usHour - 12 : usHour),        /* hour         */
                      pData->usMin,                                 /* minutes      */
                      ((pData->usHour >= 12)? aszPrtPM : aszPrtAM));/* AM/PM        */
            }
            if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
                PrtPrintf(PMG_PRT_JOURNAL,                  /* printer type */
                          auchPrtSupEJTOD2,               /* format */
                          pData->usMonth,               /* month */
                          pData->usMDay,                /* day */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->usMin,                 /* minutes */
                          (pData->usHour >= 12)? aszPrtPM : aszPrtAM);   /* AM/PM */
            }
        }
    }
}


/* === End of PrsTodT_.C === */
