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
* Title       : Multiline Display Reg Financial File Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program
* Program Name: MLDREGFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms register financial file print format.
*
*           The provided function names are as follows: 
* 
*               MldRptSupRegFin() : form  Reg Financial File print format
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Apr-12-95 : 03.00.00 :           : Initial
* Jul-20-95 : 03.00.01 : M.Ozawa   : Modify for scroll control by page pause
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
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupRegFin( RPTREGFIN *pData )  
*               
*     Input:    *pData   : pointer to structure for RPTREGFIN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms REGISTER FINANCIAL/WAITER FILE print format.
*
*    < THERMAL >
*                  : REGISTER FINANCIAL REPORT
*                  : CASHIER FILE READ REPORT    (except WAITER NAME/No., TRAINING ID for FUNCTION TYPE 3)
*                  : CASHIER FILE RESET REPORT   (except WAITER NAME/No., TRAINING ID for FUNCTION TYPE 3)
*                  : WAITER FILE READ REPORT    (except WAITER NAME/No., TRAINING ID for FUNCTION TYPE 3)
*                  : WAITER FILE RESET REPORT   (except WAITER NAME/No., TRAINING ID for FUNCTION TYPE 3)
*                  : HOURLY ACTIVITY REPORT     (except REPORT TIME, 
*                                                       illegal SALE/PERSON)
*                  : DEPARTMENT SALES READ REPORT (only START/END TIME)
*                  : MAJOR DEPARTMENT SALES READ REPORT (only START/END TIME)
*                  : PLU SALES READ REPORT  (only START/END TIME)
*                  : MEDIA FLASH REPORT     (START/END TIME & MEDIA)
*                  : EMPLOYEE TIME KEEPING FILE READ REPORT  (only START/END TIME)
*                  : EMPLOYEE TIME KEEPING FILE RESET REPORT (only START/END TIME)
*
*===========================================================================
*/

SHORT MldRptSupRegFin( RPTREGFIN *pData )  
{

    /* define thermal print format */
    
    static const TCHAR FARCONST auchMldRptSupRegFin1[] = _T("%-9.9s\t%6d   %12l$");  /*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin2[] = _T("%-9.9s\t%02u/%02u  %2u:%02u"); /*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin3[] = _T("%-9.9s\t%02u/%02u  %2u:%02u%s");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin4[] = _T("%-9.9s\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin5[] = _T("%-9.9s\t%L$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin6[] = _T("%3u%%\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin7[] = _T("%-9.9s\t%ld");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin8[] = _T("%-9.9s%11s%6d\n");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchMldRptSupRegFin9[] = _T("\t%11ld");
    static const TCHAR FARCONST auchMldRptSupRegFin10[] = _T("\t%12.2l$");
    static const TCHAR FARCONST auchMldRptSupRegFin11[] = _T("\t%12.3l$");
    static const TCHAR FARCONST auchMldRptSupRegFin12[] = _T("%9.9s\n");/*8 characters JHHJ*/

    TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
    USHORT usRow=0;

    /* check minor class */
   
    switch(pData->uchMinorClass) {
    case CLASS_RPTREGFIN_PRTTTLCNT:      
   
        /* print TRANS MNEMO/COUNTER/AMOUNT */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin1,       /* format */
                  pData->aszTransMnemo,        /* transaction mnemonics */
                  pData->Total.sCounter,       /* counter */
                  pData->Total.lAmount);       /* amount */
   
        break;
   
    case CLASS_RPTREGFIN_PRTTIME:
   
        /* check time */
   
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
   
            /* check date and print line */
   
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
   
                /* print TRANS MNEMO / DD/MM/YY / TIME */
   
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupRegFin2,        /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Date.usMDay,           /* day */
                          pData->Date.usMonth,          /* month */
                          pData->Date.usHour,           /* hour */
                          pData->Date.usMin);           /* minutes */
   
            } else {                                                 /* MM/DD/YY */
   
                /* print TRANS MNEMO / MM/DD/YY / TIME */
   
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupRegFin2,        /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Date.usMonth,          /* month */
                          pData->Date.usMDay,           /* day */
                          pData->Date.usHour,           /* hour */
                          pData->Date.usMin);           /* minutes */
              
            }
        } else {
            /* check if Hour is '0' */
   
            if (pData->Date.usHour == 0) {   /* in case of "0:00 AM" */
                usHour = 12;
            } else {
                usHour = pData->Date.usHour;
            }
   
            /* check date */
   
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
   
                /* print TRANS MNEMO / DD/MM/YY / TIME(AM/PM) */
   
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupRegFin3,        /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Date.usMDay,           /* day */
                          pData->Date.usMonth,          /* month */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->Date.usMin,                 /* minutes */
                          (pData->Date.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
   
            } else {                                                 /* MM/DD/YY */
   
                /* print TRANS MNEMO / MM/DD/YY / TIME(AM/PM) */
   
                usRow = MldSPrintf(aszString,          /* display buffer */
                          MLD_SUPER_MODE_CLM_LEN,      /* column length */
                          auchMldRptSupRegFin3,        /* format */
                          pData->aszTransMnemo,         /* transaction mnemonics */
                          pData->Date.usMonth,          /* month */
                          pData->Date.usMDay,           /* day */
                          (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->Date.usMin,            /* minutes */
                          (pData->Date.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
                          
            }
        }
        break;
   
    case CLASS_RPTREGFIN_PRTDGG:
   
        /* print TRANS MNEMO / AMOUNT or FOREIGN CURRENCY AMOUNT */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin4,       /* format */
                  pData->aszTransMnemo,        /* transaction mnemonics */
                  pData->Total.lAmount);       /* amount */
   
        break;
   
    case CLASS_RPTREGFIN_PRTCGG:
   
        /* print TRANS MNEMO/AMOUNT (13digit) */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin5,       /* format */
                  pData->aszTransMnemo,        /* transaction mnemonics */
                  pData->Total13D);            /* amount */
   
        break;

    case CLASS_RPTREGFIN_PRTBONUS:
   
        /* print BONUS % / BONUS TOTAL */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin6,       /* format */
                  pData->Total.sCounter,       /* counter */
                  pData->Total.lAmount);       /* amount */
   
        break;
   
    case CLASS_RPTREGFIN_PRTNO:
   
        /* print TRANS MNEMO / COUNTER */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin7,       /* format */
                  pData->aszTransMnemo,        /* transaction mnemonics */
                  pData->Total.lAmount);       /* amount */
   
        break;
   
    case CLASS_RPTREGFIN_PRTCNT:

        /* print TRANS MNEMO / COUNTER for FOREIGN */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin8,       /* format */
                  pData->aszTransMnemo,        /* transaction mnemonics */
                  aszPrtNull,                  /* null */
                  pData->Total.lAmount);       /* amount */
   
        break;

    case CLASS_RPTREGFIN_PRTFCP0:         /* |           SZZZZZZZZ9 | */

        /* print NATIVE CURRENCY AMOUNT */
   
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin9,       /* format */
                  pData->Total.lAmount);       /* amount */
        break;
   
    case CLASS_RPTREGFIN_PRTFCP2:         /* |          SZZZZZZ9.99 | */
   
        /* print NATIVE CURRENCY AMOUNT */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin10,      /* format */
                  pData->Total.lAmount);       /* amount */
        break;
   
    case CLASS_RPTREGFIN_PRTFCP3:         /* |          SZZZZZ9.999 | */
   
        /* print NATIVE CURRENCY AMOUNT */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin11,      /* format */
                  pData->Total.lAmount);       /* amount */
        break;

    case CLASS_RPTREGFIN_MNEMO:

        /* print MENMONIC (TRAINING ID or TOTAL) */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupRegFin12,      /* format */
                  pData->aszTransMnemo);       /* transaction mnemonics */
        break;
   
    default:
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_05);
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
