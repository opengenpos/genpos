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
* Title       : Thermal Print Reg Financial File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSREGFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms register financial file print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupRegFin() : form  Reg Financial File print format
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-21-93 : 01.00.12 : J.IKEDA   : Initial                                   
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
/* #include <pif.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtsin.h"
#include "prtcom.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupRegFin( RPTREGFIN *pData )  
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

VOID  PrtThrmSupRegFin( RPTREGFIN *pData )  
{

    /* define thermal print format */
    
    static const TCHAR FARCONST auchPrtThrmSupRegFin1[] = _T("%-9.20s\t%6d   %12l$"); /*8 characters JHHJ //SR 623, increase the max
																						to 20 so that we show all the mnemonics. JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin2[] = _T("%-9.9s\t %02u/%02u  %2u:%02u");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin3[] = _T("%-9.9s\t %02u/%02u  %2u:%02u%s");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin4[] = _T("%-9.9s\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin5[] = _T("%-9.9s\t%L$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin6[] = _T("%3u%%\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin7[] = _T("%-9.9s\t%ld");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin8[] = _T("%-9.9s%13s%6d");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupRegFin9[] = _T("\t%11ld  ");
    static const TCHAR FARCONST auchPrtThrmSupRegFin10[] = _T("\t%12.2l$  ");
    static const TCHAR FARCONST auchPrtThrmSupRegFin11[] = _T("\t%12.3l$  ");
    static const TCHAR FARCONST auchPrtThrmSupRegFin12[] = _T("%9.9s");/*8 characters JHHJ*/

    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usHour;

    /* check print control */

    if (pData->usPrintControl & PRT_RECEIPT) {

        /* check minor class */
       
        switch(pData->uchMinorClass) {
        case CLASS_RPTREGFIN_PRTTTLCNT:      
       
            /* print TRANS MNEMO/COUNTER/AMOUNT */
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin1,       /* format */
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
       
                    PrtPrintf(PMG_PRT_RECEIPT,              /* printer */
                              auchPrtThrmSupRegFin2,        /* format */
                              pData->aszTransMnemo,         /* transaction mnemonics */
                              pData->Date.usMDay,           /* day */
                              pData->Date.usMonth,          /* month */
                              pData->Date.usHour,           /* hour */
                              pData->Date.usMin);           /* minutes */
       
                } else {                                                 /* MM/DD/YY */
       
                    /* print TRANS MNEMO / MM/DD/YY / TIME */
       
                    PrtPrintf(PMG_PRT_RECEIPT,              /* printer */
                              auchPrtThrmSupRegFin2,        /* format */
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
       
                    PrtPrintf(PMG_PRT_RECEIPT,              /* printer */
                              auchPrtThrmSupRegFin3,        /* format */
                              pData->aszTransMnemo,         /* transaction mnemonics */
                              pData->Date.usMDay,           /* day */
                              pData->Date.usMonth,          /* month */
                              (usHour > 12)? usHour - 12 : usHour, /* hour */
                              pData->Date.usMin,                 /* minutes */
                              (pData->Date.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
       
                } else {                                                 /* MM/DD/YY */
       
                    /* print TRANS MNEMO / MM/DD/YY / TIME(AM/PM) */
       
                    PrtPrintf(PMG_PRT_RECEIPT,              /* printer */
                              auchPrtThrmSupRegFin3,        /* format */
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
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin4,       /* format */
                      pData->aszTransMnemo,        /* transaction mnemonics */
                      pData->Total.lAmount);       /* amount */
       
            break;
       
        case CLASS_RPTREGFIN_PRTCGG:
       
            /* print TRANS MNEMO/AMOUNT (13digit) */
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin5,       /* format */
                      pData->aszTransMnemo,        /* transaction mnemonics */
                      pData->Total13D);            /* amount */
       
            break;

        case CLASS_RPTREGFIN_PRTBONUS:
       
            /* print BONUS % / BONUS TOTAL */
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin6,       /* format */
                      pData->Total.sCounter,       /* counter */
                      pData->Total.lAmount);       /* amount */
       
            break;
       
        case CLASS_RPTREGFIN_PRTNO:
       
            /* print TRANS MNEMO / COUNTER */
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin7,       /* format */
                      pData->aszTransMnemo,        /* transaction mnemonics */
                      pData->Total.lAmount);       /* amount */
       
            break;
       
        case CLASS_RPTREGFIN_PRTCNT:

            /* print TRANS MNEMO / COUNTER for FOREIGN */

            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin8,       /* format */
                      pData->aszTransMnemo,        /* transaction mnemonics */
                      aszPrtNull,                  /* null */
                      pData->Total.lAmount);       /* amount */
       
            break;

        case CLASS_RPTREGFIN_PRTFCP0:         /* |           SZZZZZZZZ9 | */

            /* print NATIVE CURRENCY AMOUNT */
       
            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin9,       /* format */
                      pData->Total.lAmount);       /* amount */
            break;
       
        case CLASS_RPTREGFIN_PRTFCP2:         /* |          SZZZZZZ9.99 | */
       
            /* print NATIVE CURRENCY AMOUNT */

            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin10,      /* format */
                      pData->Total.lAmount);       /* amount */
            break;
       
        case CLASS_RPTREGFIN_PRTFCP3:         /* |          SZZZZZ9.999 | */
       
            /* print NATIVE CURRENCY AMOUNT */

            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin11,      /* format */
                      pData->Total.lAmount);       /* amount */
            break;

        case CLASS_RPTREGFIN_MNEMO:

            /* print MENMONIC (TRAINING ID or TOTAL) */

            PrtPrintf(PMG_PRT_RECEIPT,             /* printer type */
                      auchPrtThrmSupRegFin12,      /* format */
                      pData->aszTransMnemo);       /* transaction mnemonics */
            break;
       
        default:
/*          PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
            break;       
        }
    }      

    /* check print control */

    if (pData->usPrintControl & PRT_JOURNAL) {
        return;
    }
}                           
/***** End of Source *****/
