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
* Title       : Multiline Display Report Common Format  ( SUPER & PROGRAM MODE )                       
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDCOMRP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms common function.
*
*           The provided function names are as follows: 
* 
*               MldRptSupHeader()      : form header print format
*               MldRptSupTrailer()     : form trailer      
*               MldRptSupFormTrailer() : form trailer thermal line print format 
*               MldEJSupFormTrailer()   : form trailer EJ line print format 
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -  : 03.00.00 :           : Initial                                   
* Oct-31-95: 03.00.10 : M.Ozawa   : Change display pause function
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
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <fsc.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <shr.h>
#include <prt.h>
#include <mld.h>
#include <mldlocal.h>
#include <mldsup.h>

#include <mldmenu.h>

/*
;==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

/* define print format for MLD */

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupHeader( MAINTHEADER *pData )
*               
*     Input:    *pData    : pointer to structure for MAINTHEADER
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function controls header.
*===========================================================================
*/
VOID  MldRptSupHeader( MAINTHEADER *pData )
{
    const TCHAR  *auchMldRptSupHead1 = _T("  %-12s    %-6s    %-12s");

	/* See NHPOS_ASSERT() check on usRow value as well. */
    TCHAR   aszString[2 * (MLD_SUPER_MODE_CLM_LEN + 1)] = {0};           /* buffer for formatted data */
	TCHAR   aszRepoNumb[8 + 1] = {0};
    USHORT  usRow=0;

    /* make double wide for report number */
    if (pData->uchPrgSupNo) {
		const TCHAR  *auchNumber = _T("%3d");

        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->uchPrgSupNo);
    }

    /* 1st line  - print ACTION No. / PROG No. / REPORT NAME */
    usRow = MldSPrintf(aszString,           /* display buffer */
              MLD_SUPER_MODE_CLM_LEN,           /* column length */
              auchMldRptSupHead1,               /* format */
              pData->aszReportName1,            /* action or program number */
              aszRepoNumb,                      /* number */
              pData->aszReportName2);           /* report name */

    if (usRow) {
		TCHAR  *pszString = &aszString[0];

		NHPOS_ASSERT(usRow < 2);         // Assert that usRow is less than or equal to size of aszString;

        MldScroll1Buff.uchCurItem += (UCHAR)usRow;
        while (usRow--) {
            /* display format data */
            MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
            pszString += (MLD_SUPER_MODE_CLM_LEN+1);
        }
    }

    /* 2nd line  - check minor class */
    if ((pData->uchMinorClass == CLASS_MAINTHEADER_RPT) || (pData->uchMinorClass == CLASS_MAINTHEADER_NOTSEND)) {
		const TCHAR  *auchMldRptSupHead2 = _T("  %-6s  %6s  %4s  %-16s");

		TCHAR   aszReportType[7] = {0};
		TCHAR   aszResetType[7] = {0};

        /* print REPORT NAME/REPORT TYPE / SPEMNEMO/RESET TYPE */ 
        /* convert REPORT TYPE to binary ASCII data */
        if (pData->uchReportType) {                   
            _itot(pData->uchReportType, aszReportType, 10);
        }

        /* convert RESET TYPE to binary ASCII data */
        if (pData->uchResetType) {                   
            _itot(pData->uchResetType, aszResetType, 10);
        }

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupHead2,          /* format */
                  pData->aszReportName3,        /* report name */
                  aszReportType,                /* report type */
                  pData->aszSpeMnemo,           /* special mnemonics */
                  aszResetType);                /* reset type */

		if (usRow) {
			TCHAR  *pszString = &aszString[0];

			NHPOS_ASSERT(usRow < 2);         // Assert that usRow is less than or equal to size of aszString;

			MldScroll1Buff.uchCurItem += (UCHAR)usRow;
			while (usRow--) {
				/* display format data */
				MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
				pszString += (MLD_SUPER_MODE_CLM_LEN+1);
			}
		}
    }
}

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupTrailer( MAINTTRAILER *pData )
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

VOID  MldRptSupTrailer( MAINTTRAILER *pData )
{
    /* check trailer line is able to dispaly or not */
    if (3 + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
        MldScroll1Buff.uchCurItem = 0;  /* new page */
        /* create dialog */
        MldContinueCheck();
    }

    MldRptSupFormTrailer(pData);       /* print trailer line */ 
    MaintDisp(uchUifTrailerOnOff,             /* A/C Number */
              0,                                /* Descriptor */
              0,                                /* Page Number */
              uchUifACDataType,                 /* PTD Type */
              uchUifACRptType,                  /* Report Type */
              0,                                /* Reset Type */
              0L,                               /* Amount Data */
              0);                               /* Disable  Lead Through */

    MldPause();                         /* Report End Pause */
}

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupFormTrailer( MAINTTRAILER *pData )
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

VOID MldRptSupFormTrailer(MAINTTRAILER *pData) 
{
	const TCHAR *auchMldRptSupTrailer2 = _T("%8.8Mu\t%-4s%04u    %-4s%04u");   /* supervisor mode */
	/* common trailer */
    USHORT usHour;

    /* 1 line feed before trailer */
    MldIRptItemOther(1);    /* feed 1 line */

    /* print 1st line */
    /* check minor class */
    MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, auchMldRptSupTrailer2, RflTruncateEmployeeNumber(pData->ulSuperNumber), pData->aszPTDRstMnemo, pData->usPTDRstCount, pData->aszDayRstMnemo, pData->usDayRstCount);

    /* print 2nd line */
    /* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {
		const TCHAR *auchMldRptSupTrailer3 = _T("%04u\t%2u:%02u   %02u/%02u/%02u    %04u-%03u");

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) { /* MM/DD/YY */
            MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, auchMldRptSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMDay, pData->usMonth, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        } else {                                              /* DD/MM/YY */
            MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, auchMldRptSupTrailer3, pData->usConsCount, pData->usHour, pData->usMin, pData->usMonth, pData->usMDay, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        }
    } else {
		const TCHAR *auchMldRptSupTrailer4 = _T("%04u\t%2u:%02u%s   %02u/%02u/%02u    %04u-%03u");

        /* check if Hour is '0' */
        if (pData->usHour == 0) {   /* in case of "0:00 AM" */
            usHour = 12;
        } else {
            usHour = pData->usHour;
        }

        /* check date */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {   /* DD/MM/YY */
            MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, auchMldRptSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM, pData->usMDay, pData->usMonth, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        } else {                                                /* MM/DD/YY */
            MldPrintf(MLD_SCROLL_1, MLD_SCROLL_APPEND, auchMldRptSupTrailer4, pData->usConsCount, (usHour > 12)? usHour - 12 : usHour, pData->usMin, (pData->usHour >= 12)? aszPrtPM : aszPrtAM, pData->usMonth, pData->usMDay, pData->usYear % 100, pData->usStoreNumber, pData->usRegNumber);
        }
    }
}

/*
*===========================================================================
** Format  : VOID   MldIRptItemOther(ITEMOTHER *pItem, USHORT usScroll);      
*               
*    Input : ITEMOTHER        *pItem     -Item Data address (Dummy)
*    Input : USHORT           usScroll   Scroll Up Count
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is Scroll Up Display
*===========================================================================
*/

SHORT MldIRptItemOther(USHORT usScroll)
{
	TCHAR  aszString[MLD_SUPER_MODE_CLM_LEN + 1] = {0};           /* buffer for formatted data */

    if (usScroll + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
        MldScroll1Buff.uchCurItem = 0;  /* new page */
        /* create dialog */
        if (MldContinueCheck() == MLD_ABORTED) {
            return (MLD_ABORTED);
        }
    }

    MldScroll1Buff.uchCurItem += (UCHAR)usScroll;

    tcharnset(aszString, _T(' '), MLD_SUPER_MODE_CLM_LEN);

    for ( ; usScroll>0 ; usScroll-- ){
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, aszString, MLD_SUPER_MODE_CLM_LEN);
    }

    return(MLD_SUCCESS);
}


/*
*=============================================================================
**  Synopsis: VOID MldPause(VOID) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: Nothing
*
**  Description: Report End Pause Proccess.  
*===============================================================================
*/
VOID MldPause(VOID) 
{
    if (RptCheckReportOnMld() != RPT_DISPLAY_ON) {     /* output not on LCD so nothing to do. */
        return;
    }

	// since we are displaying to the MLD window, put up a pause message and wait for input.
    UieErrorMsg(LDT_PROGRES_ADR, UIE_EM_AC|UIE_EM_TONE);

    return;
}

/*
*=============================================================================
**  Synopsis: SHORT MldContinueCheck(VOID) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS
*           MLD_ABORT 
*
**  Description: Report Continue or Abort Proccess.  
*===============================================================================
*/
SHORT MldContinueCheck(VOID) 
{
    SHORT   asRet = UieErrorMsg(LDT_CLRABRT_ADR, UIE_EM_AC | UIE_EM_TONE | UIE_ABORT);

    if (asRet == FSC_CANCEL) {
        asRet = MLD_ABORTED;
    } else {
        asRet = MLD_SUCCESS;
    }

    return(asRet);
}

/***** End of Source *****/
