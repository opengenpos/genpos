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
* Title       : Multiline Display ETK File Maintenance Format ( SUPER & PROGRAM MODE ) 
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program    
* Program Name: MLDETKFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls Employee No.
*
*    The provided function names are as follows: 
* 
*               MldMaintETKFl()      : ETK print format.
*               MldMaintMakeETKFl() : make print ETK File data
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-29-95: 03.00.03 : M.Ozawa   : initial
* 
*** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  : initial (for Win32)
*
*** OpenGENPOS **
* Jul-15-25: 02.04.00 : R.Chambers : moved aszMldAC153 to here and made static.
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


/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/


extern const TCHAR  auchThrmEtkTime[];
extern const TCHAR  auchThrmEtkDate[];
extern const TCHAR  auchThrmEtkAMPM[];

static VOID MldMaintMakeETKFl(MAINTETKFL *pData);

static CONST TCHAR aszMldAC153[][64] = {
                            _T("....*....1....*....2....*....3....*....4"),
                            _T("%-16s\t%9lu "),
                            _T("%s%s%s  IN    OUT"),
/*                            "BLOCK JOB\tDATE  IN    OUT   ", */
                            _T("%s%s%s  IN      OUT"),
/*                            "BLOCK JOB\tDATE  IN      OUT     ", */
                            _T("%2d       %02d    %s    %s %s "),
                            _T("")
};

static TCHAR aszMldInTime[7 + 1] = { 0 };
static TCHAR aszMldOutTime[7 + 1] = { 0 };
static TCHAR aszMldDate[5 + 1] = { 0 };
static TCHAR aszMldInAMPM[2 + 1] = { 0 };
static TCHAR aszMldOutAMPM[2 + 1] = { 0 };
static TCHAR aszMldWorkTime[5 + 1 + 7 + 1] = { 0 };     /* R3.1 */

/*
*===========================================================================
** Synopsis:    VOID  MldMaintETKFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*               UCHAR          uchClear ( 0 : During Edit, 1 : When Read )
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/
VOID  MldMaintETKFl( MAINTETKFL *pData, USHORT usClear)
{
    TCHAR   aszBlock[PARA_TRANSMNEMO_LEN+1] = { 0 };
	TCHAR   aszJob[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    TCHAR   aszDate[PARA_TRANSMNEMO_LEN+1] = { 0 };

    /********************************************************/
    /* Clear Scroll Display and Display Item telop,         */
    /*                      when ONLY after Parameter Read. */
    /********************************************************/
    if(usClear != 0){
        MldScrollClear(MLD_SCROLL_1);

        MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP,&(aszMldAC153[1][0]),
                            pData->aszMnemonics, pData->ulEmployeeNo);

        /* get programmable mnemonics, V3.3 */
		RflGetTranMnem(aszBlock, TRN_BLOCK_ADR);
		RflGetTranMnem(aszJob, TRN_JOB_ADR);
		RflGetTranMnem(aszDate, TRN_DATE_ADR);

        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* MILITARY type */
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,&(aszMldAC153[2][0]), aszBlock, aszJob, aszDate);
        } else {                                                /* AM/PM type   */
            MldPrintf(MLD_SCROLL_1,MLD_SCROLL_TOP+1,&(aszMldAC153[3][0]), aszBlock, aszJob, aszDate);
        }
    }

    MldMaintMakeETKFl(pData);

    /*********************/
    /* Display Parameter */
    /*********************/
    MldPrintf(MLD_SCROLL_1,(SHORT)(MLD_SCROLL_TOP+1+pData->usBlockNo), &(aszMldAC153[4][0]),
			pData->usBlockNo, (USHORT)(pData->EtkField.uchJobCode), aszMldDate, aszMldInTime, aszMldOutTime);            /* time-out */
    return;
}

/*
*===========================================================================
** Synopsis:    VOID  MldMaintMakeETKFl( MAINTETKFL *pData )  
*               
*     Input:    *pData  : pointer to structure for MAINTETKFL 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function makes ETK File string data.
*
*               See also function PrtThrmSupMakeETKFl() which does basically
*               the same thing.
*===========================================================================
*/
static VOID MldMaintMakeETKFl(MAINTETKFL *pData)
{
    TCHAR  aszPrtNull[1] = {'\0'};

    USHORT usInHour = 0;
    USHORT usOutHour = 0;

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

        RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchThrmEtkAMPM, aszPrtNull);
        RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchThrmEtkAMPM, aszPrtNull);
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
                RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchThrmEtkAMPM, aszPrtPM);          /* set PM */
            } else {
                RflSPrintf(aszMldInAMPM, TCHARSIZEOF(aszMldInAMPM), auchThrmEtkAMPM, aszPrtAM);          /* set AM */
            }
        } else {
            tcharnset(aszMldInAMPM, _T('*'), 2);
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
                RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchThrmEtkAMPM, aszPrtPM);             /* set PM */
            } else {
                RflSPrintf(aszMldOutAMPM, TCHARSIZEOF(aszMldOutAMPM), auchThrmEtkAMPM, aszPrtAM);             /* set AM */
            }
        } else {
            tcharnset(aszMldOutAMPM, _T('*'), 2);
        }
    }

    /* check if TIME-IN is illegal */
    if (pData->EtkField.usTimeinTime == ETK_TIME_NOT_IN) {
        tcharnset(aszMldInTime, _T('*'), 5);
    }

    /* check if TIME-OUT is illegal */
    if (pData->EtkField.usTimeOutTime == ETK_TIME_NOT_IN) {
        tcharnset(aszMldOutTime, _T('*'), 5);
    }

    /* check if TIME-IN is exist */
    if (pData->EtkField.usTimeinTime != ETK_TIME_NOT_IN) {
        /* set TIME-IN */
        RflSPrintf(aszMldInTime, TCHARSIZEOF(aszMldInTime), auchThrmEtkTime, usInHour, pData->EtkField.usTimeinMinute);
    }

    /* check if TIME-OUT is exist */
    if (pData->EtkField.usTimeOutTime != ETK_TIME_NOT_IN) {
        /* set TIME-OUT */
        RflSPrintf(aszMldOutTime, TCHARSIZEOF(aszMldOutTime), auchThrmEtkTime, usOutHour, pData->EtkField.usTimeOutMinute);
    }
    _tcscat(aszMldInTime, aszMldInAMPM);   
    _tcscat(aszMldOutTime, aszMldOutAMPM);   

    /* check MDC */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
        RflSPrintf(aszMldDate, TCHARSIZEOF(aszMldDate), auchThrmEtkDate, pData->EtkField.usDay, pData->EtkField.usMonth);
    } else {
        RflSPrintf(aszMldDate, TCHARSIZEOF(aszMldDate), auchThrmEtkDate,  pData->EtkField.usMonth, pData->EtkField.usDay);
    }
}

/***** End of Source *****/

