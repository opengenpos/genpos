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
* Program Name: PRSTOD_.C
* --------------------------------------------------------------------------
* Abstruct: This function forms common function.
*
*           The provided function names are as follows:
*
*               PrtSupTOD()     :
*               PrtSupTOD_RJ()  : form tod print format
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Mar-19-93: 00.00.01 : M.Ozawa   : initial
* Jan-07-00: 01.00.00 : H.Endo    : WeekOfDay data change
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
/* #include <rfl.h> */
/* #include <pif.h> */
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <csstbpar.h>
#include <pmg.h>
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
** Synopsis:    VOID  PrtSupTOD( PARATOD *pData )
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

VOID PrtSupTOD(PARATOD *pData)
{
	/*endo update(add) 2000/1/7*/
	static TCHAR const szSUN[] = _T("SUN\0");
	static TCHAR const szMON[] = _T("MON\0");
	static TCHAR const szTUS[] = _T("TUS\0");
	static TCHAR const szWED[] = _T("WED\0");
	static TCHAR const szTHR[] = _T("THR\0");
	static TCHAR const szFRI[] = _T("FRI\0");
	static TCHAR const szSAT[] = _T("SAT\0");

	const TCHAR  auchPrtSupRJTOD1[] = _T("%02u/%02u/%02u  %-3s\t%2u:%02u");
	const TCHAR  auchPrtSupRJTOD2[] = _T("%02u/%02u/%02u  %-3s\t%2u:%02u%s");

	USHORT usPrtType;
    USHORT usHour;
	TCHAR  aszMnem[8] = { 0 };      /*endo update(change size) 2000/1/7 */

    /* set day of week mnemonic */
    switch(pData->usWDay){
    case TOD_WEEK_SUN:
        _tcsncpy(aszMnem, szSUN, _tcslen(szSUN));/* endo update 2000/1/7 */
        break;
    case TOD_WEEK_MON:
        _tcsncpy(aszMnem, szMON, _tcslen(szMON));/*endo update 2000/1/7*/
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

	usPrtType = PrtSupChkType(pData->usPrintControl);

    /* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {
            PmgPrintf(usPrtType,                    /* printer */
                          auchPrtSupRJTOD1,               /* format */
                          pData->usMDay,                /* day */
                          pData->usMonth,               /* month */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          pData->usHour,                /* hour */
                          pData->usMin);                /* minutes */
        } else {
            PmgPrintf(usPrtType,                    /* printer */
                          auchPrtSupRJTOD1,               /* format */
                          pData->usMonth,               /* month */
                          pData->usMDay,                /* day */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          pData->usHour,                /* hour */
                          pData->usMin);                /* minutes */
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
            PmgPrintf(usPrtType,                    /* printer */
                          auchPrtSupRJTOD2,              /* format */
                          pData->usMDay,                /* day */
                          pData->usMonth,               /* month */
                          pData->usYear % 100,          /* year */
                          aszMnem,                      /* day of week */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->usMin,                 /* minutes */
                          (pData->usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
        } else {
            PmgPrintf(usPrtType,                    /* printer */
                          auchPrtSupRJTOD2,               /* format */
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

