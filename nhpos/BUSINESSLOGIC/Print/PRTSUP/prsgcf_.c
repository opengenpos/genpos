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
* Title       : Print Guest Check File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCF_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupGCF()    : form GUEST CHECK FILE print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-29-92: 00.00.01 : J.Ikeda   : initial                                   
* Jun-22-93: 01.00.12 : J.IKEDA   : Adds CLASS_RPTGUEST_PRTDGG, CLASS_RPTGUEST_PRTMNEMO                                 
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
#include <ecr.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h>
#include <para.h>
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <csstbpar.h>
#include <pmg.h>
#include <rfl.h>
/* #include <appllog.h> */
#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupGCF( RPTGUEST *pData )
*               
*     Input:    *pData      : Pointer to Structure for RPTGUEST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHEFCK FILE print format.
*===========================================================================
*/

VOID  PrtSupGCF( RPTGUEST *pData )
{
    /* Define Format Type */
    static const TCHAR FARCONST auchPrtSupGCF1[] = _T("%-8.8s %4u%02u\t%4s %8.8Mu");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF2[] = _T("%-8.8s %4u\t%4s %8.8Mu");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF3[] = _T("%-8.8s\t%2u:%02u");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF4[] = _T("%-8.8s\t%2u:%02u%s");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF5[] = _T("%-8.8s\t%3d");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF6[] = _T("%-8.8s\n\t%l$");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF7[] = _T("%-8.8s\t%l$");        /*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF8[] = _T("%s");                

    USHORT usPrtType;
    USHORT usHour;


    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Distinguish Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTGUEST_PRTCHKNOWCD:
        PmgPrintf(usPrtType, auchPrtSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv), pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
        break;

    case CLASS_RPTGUEST_PRTCHKNOWOCD:
        PmgPrintf(usPrtType, auchPrtSupGCF2, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
        break;                                      

    case CLASS_RPTGUEST_PRTCHKOPEN:
        /* check if time is military hour or not */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* time is military hour */
            PmgPrintf(usPrtType, auchPrtSupGCF3, pData->aszMnemo, pData->auchCheckOpen[0], pData->auchCheckOpen[1]);
        } else {
            /* check if Hour is '0' */
            if (pData->auchCheckOpen[0] == 0) {     /* in case of "0:00 AM" */
                usHour = 12;
            } else {
                usHour = ( USHORT)pData->auchCheckOpen[0];
            }

            PmgPrintf(usPrtType, auchPrtSupGCF4, pData->aszMnemo, (usHour > 12)? usHour - 12 : usHour, ( USHORT)pData->auchCheckOpen[1], (pData->auchCheckOpen[0] >= 12)? aszPrtPM : aszPrtAM);
        } 
        break;

    case CLASS_RPTGUEST_PRTPERSON:
        PmgPrintf(usPrtType, auchPrtSupGCF5, pData->aszMnemo, pData->usNoPerson);

        break;

    case CLASS_RPTGUEST_PRTTBL:
        PmgPrintf(usPrtType, auchPrtSupGCF5, pData->aszSpeMnemo, pData->usNoPerson);
        break;

    case CLASS_RPTGUEST_PRTTTL:
        PmgPrintf(usPrtType, auchPrtSupGCF6, pData->aszMnemo, pData->lCurBalance);
        break;
            
    case CLASS_RPTGUEST_PRTDGG:
        /* print TRANS. BALANCE / CHECK TOTAL */
        PmgPrintf(usPrtType, auchPrtSupGCF7, pData->aszMnemo, pData->lCurBalance);
        break;
            
    case CLASS_RPTGUEST_PRTMNEMO:
        /* print TRAINING ID */
        PmgPrintf(usPrtType, auchPrtSupGCF8, pData->aszSpeMnemo);
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
        break;
    }
}
