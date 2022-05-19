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
* Title       : Multiline Display Hourly Activity Report Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: MLDHORPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupHourlyAct() : forms Hourly Activity Report format        
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 03.00.   :           : Initial                                   
*          :          :           :                                    
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
#include <ecr.h>
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>


/*
*===========================================================================
** Synopsis:    VOID  MldRptSupHourlyAct( RPTHOURLY *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTHOURLY 
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms TIME SEGMENT         
*                                   ILLEGAL SALE/PERSON  formats
*               for Hourly Activity Report.
*
*               The other print formats for Hourly Activity Report
*               are same as REGISTER FINANCIAL REPORT.
*
*               : HOURLY ACTIVITY REPORT
*
*===========================================================================
*/

SHORT  MldRptSupHourlyAct( RPTHOURLY *pData )  
{
    
    /* define thermal print format */

    static const TCHAR FARCONST auchMldRptSupHourlyAct1[] = _T("\t%2u:%02u");
    static const TCHAR FARCONST auchMldRptSupHourlyAct2[] = _T("\t%2u:%02u%s");
    static const TCHAR FARCONST auchMldRptSupHourlyAct3[] = _T("%-8s\t**********");
    static const TCHAR FARCONST auchMldRptSupHourlyAct4[] = _T("%-8s\t%.2l$%%");
    static const TCHAR FARCONST auchMldRptSupHourlyAct5[] = _T("%-8s\t*****%%");

    TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    USHORT usHour;
    USHORT usRow=0;

    /* check minor class */
   
    switch(pData->uchMinorClass) {  
    case CLASS_RPTHOURLY_PRTTIME:
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {           /* military format */
   
            /* print TIME(MILITARY) */
   
            usRow = MldSPrintf(aszString,          /* display buffer */
                      MLD_SUPER_MODE_CLM_LEN,      /* column length */
                      auchMldRptSupHourlyAct1,     /* format */
                      pData->StartTime.usHour,      /* hour */
                      pData->StartTime.usMin);      /* minutes */
   
        } else {                                    /* time is AM/PM */
   
            /* check if Hour is '0' */
   
            if (pData->StartTime.usHour == 0) {     /* in case of "0:00 AM" */
                usHour = 12;
            } else {
                usHour = pData->StartTime.usHour;
            }
   
            /* print TIME(AM/PM) */
   
            usRow = MldSPrintf(aszString,          /* display buffer */
                      MLD_SUPER_MODE_CLM_LEN,      /* column length */
                      auchMldRptSupHourlyAct2,     /* format */
                      (usHour > 12)? usHour - 12 : usHour, /* hour */
                      pData->StartTime.usMin,                 /* minutes */
                      (pData->StartTime.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
                      
        }
        break;
   
        case CLASS_RPTHOURLY_PERCENT:

            if (pData->Total.lAmount == RPT_OVERFLOW) {

                /* print illegal Labor Cost % */
       
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupHourlyAct5,     /* format */
                          pData->aszTransMnemo);       /* transaction mnemonics */

            } else {

                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupHourlyAct4,     /* format */
                          pData->aszTransMnemo,        /* transaction mnemonics */
                          pData->Total.lAmount);       /* labor Cost % */
            }
            break;

    default:                                    /* CLASS_RPTHOURLY_PRTASTER */
   
        /* print illegal SALE/PERSON */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupHourlyAct3,     /* format */
                  pData->aszTransMnemo);        /* transaction mnemonics */
   
        break;
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

    MldScroll1Buff.uchCurItem += (UCHAR)usRow;

    pszString = &aszString[0];

    while (usRow--) {

        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);

        pszString += (MLD_SUPER_MODE_CLM_LEN+1);

    }

    return (MLD_SUCCESS);
}

/***** End of Source *****/

