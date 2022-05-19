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
* Title       : Reg Financial File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSREGF_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function forms register financial file print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupRegFin() : form Reg Financial File Format     
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-14-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-22-93: 01.00.12 : J.IKEDA   : Adds CLASS_RPTREGFIN_PRTCNT, CLASS_RPTREGFIN_MNEMO                                   
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
** Synopsis:    VOID  PrtSupRegFin( RPTREGFIN *pData )  
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms register financial file print format.
*===========================================================================
*/

VOID  PrtSupRegFin( RPTREGFIN *pData )  
{
    
    static const TCHAR FARCONST auchPrtSupRegFin1[] = _T("%-9.9s\t%6d\n\t%.*l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin2[] = _T("%-9.9s\t%02u/%02u %2u:%02u");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin3[] = _T("%-9.9s\t%02u/%02u %2u:%02u%s");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin4[] = _T("%-9.9s\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin5[] = _T("%-9.9s\t%L$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin6[] = _T("%3u%%\t%l$");
    static const TCHAR FARCONST auchPrtSupRegFin7[] = _T("%-9.9s\t%ld");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupRegFin8[] = _T("\t%11ld  ");
    static const TCHAR FARCONST auchPrtSupRegFin9[] = _T("\t%12.2l$  ");
    static const TCHAR FARCONST auchPrtSupRegFin10[] = _T("\t%12.3l$  ");
    static const TCHAR FARCONST auchPrtSupRegFin11[] = _T("%s");

    UCHAR  uchDecimal = 2;
    USHORT usPrtType;
    USHORT usHour;

    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* check minor class */
    switch(pData->uchMinorClass) {

    case CLASS_RPTREGFIN_PRTTTLCNT:
        PmgPrintf(usPrtType, auchPrtSupRegFin1, pData->aszTransMnemo, pData->Total.sCounter, uchDecimal, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTTIME:
        /* check time */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */

            /* check date */
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
                PmgPrintf(usPrtType, auchPrtSupRegFin2, pData->aszTransMnemo, pData->Date.usMDay, pData->Date.usMonth, pData->Date.usHour, pData->Date.usMin);
            } else {
                PmgPrintf(usPrtType, auchPrtSupRegFin2, pData->aszTransMnemo, pData->Date.usMonth, pData->Date.usMDay, pData->Date.usHour, pData->Date.usMin);
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
                PmgPrintf(usPrtType, auchPrtSupRegFin3, pData->aszTransMnemo, pData->Date.usMDay, pData->Date.usMonth, (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->Date.usMin, (pData->Date.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
            } else {
                PmgPrintf(usPrtType, auchPrtSupRegFin3, pData->aszTransMnemo, pData->Date.usMonth, pData->Date.usMDay, (usHour > 12)? usHour - 12 : usHour, /* hour */
                          pData->Date.usMin, (pData->Date.usHour >= 12)? aszPrtPM : aszPrtAM);  /* AM/PM */
            }
        }
        break;

    case CLASS_RPTREGFIN_PRTDGG:
        PmgPrintf(usPrtType, auchPrtSupRegFin4, pData->aszTransMnemo, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTCGG:
        PmgPrintf(usPrtType, auchPrtSupRegFin5, pData->aszTransMnemo, pData->Total13D);
        break;

    case CLASS_RPTREGFIN_PRTBONUS:
        PmgPrintf(usPrtType, auchPrtSupRegFin6, pData->Total.sCounter, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTNO:
    case CLASS_RPTREGFIN_PRTCNT:
        PmgPrintf(usPrtType, auchPrtSupRegFin7, pData->aszTransMnemo, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTFCP0:         /* |           SZZZZZZZZ9 | */
        PmgPrintf(usPrtType, auchPrtSupRegFin8, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTFCP2:         /* |          SZZZZZZ9.99 | */
        PmgPrintf(usPrtType, auchPrtSupRegFin9, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_PRTFCP3:         /* |          SZZZZZ9.999 | */
        PmgPrintf(usPrtType, auchPrtSupRegFin10, pData->Total.lAmount);
        break;

    case CLASS_RPTREGFIN_MNEMO:                
        PmgPrintf(usPrtType, auchPrtSupRegFin11, pData->aszTransMnemo);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}

