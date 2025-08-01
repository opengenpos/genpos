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
* Title       : Thermal Print Guest Check File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSGCFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               PrtThrmSupGCF() : form GUEST CHECK FILE print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-22-93 : 01.00.12 : J.IKEDA   : Initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <csstbpar.h>
#include <pmg.h>
#include <prt.h>
#include <rfl.h>

#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupGCF( RPTGUEST *pData )
*               
*     Input:    *pData      : pointer to structure for RPTGUEST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHECK FILE print format.
*
*                   : GUEST CHECK FILE READ REPORT
*                   : GUEST CHECK FILE RESET REPORT
*
*               In case that RESET REPORT is not printed on RECEIPT,
*               RESET REPORT is printed on EJ.
*
*===========================================================================
*/

VOID  PrtThrmSupGCF( RPTGUEST *pData )
{
    USHORT usHour;

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {
		/* define thermal print format */
		const TCHAR  *auchPrtThrmSupGCF1 = _T("%-8.8s %4u%02u\t%4s  %8.8Mu");
		const TCHAR  *auchPrtThrmSupGCF2 = _T("%-8.8s %4u\t%4s  %8.8Mu");
		const TCHAR  *auchPrtThrmSupGCF3 = _T("%-8.8s\t%2u:%02u");
		const TCHAR  *auchPrtThrmSupGCF4 = _T("%-8.8s\t%2u:%02u%s");
		const TCHAR  *auchPrtThrmSupGCF5 = _T("%-8.8s\t%3d");
		const TCHAR  *auchPrtThrmSupGCF6 = _T("%-8.8s\t%12l$");
		const TCHAR  *auchPrtThrmSupGCF7 = _T("%s");
		const TCHAR  *auchPrtThrmSupGCF8 = _T("%-8.8s %4u *** \t%4s  %8.8Mu");
		const TCHAR  *auchPrtThrmSupGCF9 = _T("%-8.8s\t%8.8Mu");

        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_RPTGUEST_PRTCHKNOWCD:
            /* print GUEST No./WAIT NAME/No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv), pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
            break;
       
		case CLASS_RPTGUEST_PRTCHKNOWOCD_PREAUTH:
            /* print GUEST No./WAIT NAME/No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF8, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
			break;

        case CLASS_RPTGUEST_PRTCHKNOWOCD:
            /* print GUEST No./WAIT NAME/No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF2, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
            break;                                      
       
        case CLASS_RPTGUEST_PRTCHKOPEN:
            /* check if time is military hour or not */
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* time is military hour */
                /* print TIME */
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF3, pData->aszMnemo, pData->auchCheckOpen[0], pData->auchCheckOpen[1]);
       
            } else {
                /* check if Hour is '0' */
                if (pData->auchCheckOpen[0] == 0) {     /* in case of "0:00 AM" */
                    usHour = 12;
                } else {
                    usHour = ( USHORT)pData->auchCheckOpen[0];
                }
       
                /* print TIME */
                PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF4, pData->aszMnemo, (usHour > 12)? usHour - 12 : usHour, ( USHORT)pData->auchCheckOpen[1], (pData->auchCheckOpen[0] >= 12)? aszPrtPM : aszPrtAM);
       
            } 
            break;
       
        case CLASS_RPTGUEST_PRTPERSON:
            /* print No.of PERSON / OLD WAITER No.*/
			if (pData->usNoPerson > 0)
				PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF5, pData->aszMnemo, pData->usNoPerson);
			else
				PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF9, pData->aszMnemo, pData->ulCashierId);
            break;
       
        case CLASS_RPTGUEST_PRTTBL:
            /* print TABLE No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF5, pData->aszSpeMnemo, pData->usNoPerson);
            break;
       
        case CLASS_RPTGUEST_PRTDGG:
        case CLASS_RPTGUEST_PRTTTL:
            /* print TOTAL / TRANS. BALANCE / CHECK TOTAL */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF6, pData->aszMnemo, pData->lCurBalance);
            break;

        case CLASS_RPTGUEST_PRTMNEMO:
            /* print TRAINING ID */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF7, pData->aszSpeMnemo);         /* special mnemonics */
            break;

        case CLASS_RPTGUEST_FILEINFO:
            /* print TRAINING ID */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF7, pData->aszSpeMnemo);         /* special mnemonics */
			break;


		case CLASS_RPTGUEST_FILE_OPTIMIZED:
			/* print pData->aszMnemonic*/
			PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupGCF7, pData->aszMnemo);
            break;

        default:
/*            PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
            return;
        }
    }      
    
    /* check print control */
    if (!(pData->usPrintControl & PRT_RECEIPT)) {        
        /* check status */
        if (pData->uchStatus == RPT_GCF_RESET) {            /* in case of RESET REPORT */
			/* define E/J print format */
			const TCHAR  *auchPrtSupGCF1 = _T("%-8.8s %4u%02u\t%4s %8.8Mu");
			const TCHAR  *auchPrtSupGCF2 = _T("%-8.8s %4u\t%4s %8.8Mu");
			const TCHAR  *auchPrtSupGCF3 = _T("%-8.8s\t%2u:%02u");
			const TCHAR  *auchPrtSupGCF4 = _T("%-8.8s\t%2u:%02u%s");
			const TCHAR  *auchPrtSupGCF5 = _T("%-8.8s\t%3d");
			const TCHAR  *auchPrtSupGCF6 = _T("%-8.8s\n\t%l$");
			const TCHAR  *auchPrtSupGCF7 = _T("%-8.8s\t%l$");        
			const TCHAR  *auchPrtSupGCF8 = _T("%s");                
			const TCHAR  *auchPrtSupGCF9 = _T("%-8.8s\t%8.8Mu");

            /* distinguish minor class */
            switch(pData->uchMinorClass) {
            case CLASS_RPTGUEST_PRTCHKNOWCD:
                /* print GUEST No./WAIT NAME/No. */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv), pData->aszSpeMnemo, pData->ulCashierId);
                break;                                                  
            
            case CLASS_RPTGUEST_PRTCHKNOWOCD:
                /* print GUEST No./WAIT NAME/No. */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF2, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, pData->ulCashierId);
                break;                                      
            
            case CLASS_RPTGUEST_PRTCHKOPEN:
                /* check if time is military hour or not */
                if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* time is military hour */
                    /* print TIME */
                    PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF3, pData->aszMnemo, pData->auchCheckOpen[0], pData->auchCheckOpen[1]);
                } else {
                    /* check if Hour is '0' */
                    if (pData->auchCheckOpen[0] == 0) {     /* in case of "0:00 AM" */
                        usHour = 12;
                    } else {
                        usHour = ( USHORT)pData->auchCheckOpen[0];
                    }
            
                    /* print TIME */
                    PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF4, pData->aszMnemo, (usHour > 12)? usHour - 12 : usHour, /* hour */
                              ( USHORT)pData->auchCheckOpen[1], (pData->auchCheckOpen[0] >= 12)? aszPrtPM : aszPrtAM);   /* AM/PM */
                } 
                break;
            
            case CLASS_RPTGUEST_PRTPERSON:
                /* print No.of PERSON / OLD WAITER No.*/
				if (pData->usNoPerson > 0)
					PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF5, pData->aszMnemo, pData->usNoPerson);
				else
					PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF9, pData->aszMnemo, pData->ulCashierId);
                break;
            
            case CLASS_RPTGUEST_PRTTBL:
                /* print TABLE No. */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF5, pData->aszSpeMnemo, pData->usNoPerson);
                break;
            
            case CLASS_RPTGUEST_PRTTTL:
                /* print TOTAL/SUB TOTAL */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF6, pData->aszMnemo, pData->lCurBalance);
                break;
            
            case CLASS_RPTGUEST_PRTDGG:
                /* print TRANS. BALANCE / CHECK TOTAL */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF7, pData->aszMnemo, pData->lCurBalance);
                break;
            
            case CLASS_RPTGUEST_PRTMNEMO:
                /* print TRAINING ID */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupGCF8, pData->aszSpeMnemo);              /* special mnemonics */
                break;
            
            default:
/*                PifAbort(MODULE_PRINT_SUPPRG_ID, FAULT_INVALID_DATA); */
                break;                                      
            }
        }           
    }
}
/***** End of Source *****/
