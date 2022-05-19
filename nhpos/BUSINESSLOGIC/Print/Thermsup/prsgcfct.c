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
* Title       : Thermal Print Emergent Guest Check File Close Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCFCT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupGCFClose() : form EMERGENT GUEST CHECK FILE CLOSE
*                                    : print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-21-93: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <stdlib.h>
#include <ecr.h>
/* #include <pif.h> */
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
/* #include <csstbpar.h> */
#include <pmg.h>
/* #include <appllog.h> */

#include "prtcom.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupGCFClose( RPTGUEST *pData )
*               
*     Input:    *pData      : pointer to structure for RPTGUEST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHEFCK FILE print format.
*
*                   : EMERGENT GUEST CHECK FILE CLOSE
*
*===========================================================================
*/

VOID  PrtThrmSupGCFClose( RPTGUEST *pData )
{

    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupGCF1[] = _T("%9s%-8.8s     %4u%02u");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtThrmSupGCF2[] = _T("%9s%-8.8s     %4u");/*8 characters JHHJ*/

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupGCF1[] = _T("   %-8.8s\t%4u%02u   ");/*8 characters JHHJ*/
    static const TCHAR FARCONST auchPrtSupGCF2[] = _T("   %-8.8s\t%4u   ");/*8 characters JHHJ*/

    TCHAR   aszNull[1] = {'\0'};

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_RPTGUEST_PRTCHKNOWCD:
            /* print TRANSACTION MNEMONIC/GUEST No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF1, aszNull, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv));
            break;

        case CLASS_RPTGUEST_PRTCHKNOWOCD:
            /* print TRANSACTION MNEMONIC/GUEST No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF2, aszNull, pData->aszMnemo, pData->usGuestNo);
            break;                                      

        default:
/*            PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
            return;
        }
    }

    if (pData->usPrintControl & PRT_JOURNAL) {
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_RPTGUEST_PRTCHKNOWCD:
            /* print TRANSACTION MNEMONIC/GUEST No. */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv));
            break;

        case CLASS_RPTGUEST_PRTCHKNOWOCD:
            /* print TRANSACTION MNEMONIC/GUEST No. */
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF2, pData->aszMnemo, pData->usGuestNo);
            break;                                      

        default:
/*            PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
            break;                                      
        }
    }
}
/***** End of Source *****/
