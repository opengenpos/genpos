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
* Category    : 21 Character Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCFS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               PrtSupGCF_s()    : form GUEST CHECK FILE print format for 21 char 
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Oct-01-92: 00.00.01 : J.Ikeda   : initial                                   
* Nov-26-92: 01.00.00 : K.You     : Chg Function Name                                   
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

#include <tchar.h>
#include <stdlib.h>

#include <ecr.h>
/* #include <pif.h> */
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
** Synopsis:    VOID  PrtSupGCF_s( RPTGUEST *pData )
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
VOID  PrtSupGCF_s( RPTGUEST *pData )
{
    /* Define Format Type */
    static const TCHAR  auchPrtSupGCF1[] = _T("%-8.8s %4u%02u\n\t%4s %8.8Mu");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF2[] = _T("%-8.8s %4u\n\t%4s %8.8Mu");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF3[] = _T("%-8.8s\t%2u:%02u");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF4[] = _T("%-8.8s\t%2u:%02u%s");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF5[] = _T("%-8.8s\t%3d");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF6[] = _T("%-8.8s\n\t%l$");/*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF7[] = _T("%-8.8s\t%l$");        /*8 characters JHHJ*/
    static const TCHAR  auchPrtSupGCF8[] = _T("%s");                

    USHORT usPrtType;


    /* check print control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Distinguish Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTGUEST_PRTCHKNOWCD:
        PmgPrintf(usPrtType, auchPrtSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv), pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
        break;

    case CLASS_RPTGUEST_PRTCHKNOWOCD:
        PmgPrintf(usPrtType, auchPrtSupGCF2, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));              /* cashier ID */
        break;                                      

    case CLASS_RPTGUEST_PRTCHKOPEN:
        /* check if time is military hour or not */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* time is military hour */
            PmgPrintf(usPrtType, auchPrtSupGCF3, pData->aszMnemo, pData->auchCheckOpen[0], pData->auchCheckOpen[1]);     /* minutes */
        } else {
			CONST TCHAR  *pPrtAmPm = aszPrtAM;
			USHORT usHour = (USHORT)pData->auchCheckOpen[0];

			if (usHour >= 12) {
				if (usHour > 12) usHour -= 12;
				pPrtAmPm = aszPrtPM;
			}

            /* check if Hour is '0' */
            if (pData->auchCheckOpen[0] == 0) {     /* in case of "0:00 AM" hour is 12 AM */
                usHour = 12;
            }

            PmgPrintf(usPrtType, auchPrtSupGCF4, pData->aszMnemo, usHour, ( USHORT)pData->auchCheckOpen[1], pPrtAmPm);
        } 
        break;

    case CLASS_RPTGUEST_PRTPERSON:
        PmgPrintf(usPrtType, auchPrtSupGCF5, pData->aszMnemo, pData->usNoPerson);     /* No of Person, CLASS_RPTGUEST_PRTPERSON*/
        break;

    case CLASS_RPTGUEST_PRTTBL:
        PmgPrintf(usPrtType, auchPrtSupGCF5, pData->aszSpeMnemo, pData->usNoPerson);  /* print number of persons, CLASS_RPTGUEST_PRTTBL */
        break;

    case CLASS_RPTGUEST_PRTTTL:
        PmgPrintf(usPrtType, auchPrtSupGCF6, pData->aszMnemo, pData->lCurBalance);   /* print Total, CLASS_RPTGUEST_PRTTTL */
        break;
            
    case CLASS_RPTGUEST_PRTDGG:
        PmgPrintf(usPrtType, auchPrtSupGCF7, pData->aszMnemo, pData->lCurBalance);   /* print TRANS. BALANCE / CHECK TOTAL, CLASS_RPTGUEST_PRTDGG */
        break;
            
    case CLASS_RPTGUEST_PRTMNEMO:
        PmgPrintf(usPrtType, auchPrtSupGCF8, pData->aszSpeMnemo);    /* print TRAINING ID, CLASS_RPTGUEST_PRTMNEMO */
        break;

    default:
/*        PifAbort(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }
}
