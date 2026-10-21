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
* Title       : Thermal Print Server/GC No. for Operator/Guest Check Status 
*             : Report( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSOPSTT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupOpeStatus() : form Server/GC No. print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-16-93: 01.00.12 : J.IKEDA   : Initial                                   
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
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <prt.h>
#include <rfl.h>

#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupOpeStatus( MAINTOPESTATUS *pData )
*               
*     Input:    *pData          : pointer to structure for MAINTOPESTATUS          
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Server/GC No. print format. See also MldRptSupOpeStatus()
*               for display version.
* 
*               MNEMONICS are formed in unique MNEMONIC format function .
*                                           < PrtThrmSupTrans() >
*
*                   Mnemonic for Server      = SPECIAL MNEMONICS
*                   Mnemonic for Guest Check = TRANSACTION MNEMONICS
*
*                : OPERATOR / GUEST CHECK STATUS REPORT
*
*===========================================================================
*/

VOID  PrtThrmSupOpeStatus( MAINTOPESTATUS *pData )
{

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        static const TCHAR  auchPrtThrmSupOpeStatus[] = _T("                %8.8Mu");   /* define thermal print format for Operator/Waiter Id */
        static const TCHAR  auchPrtThrmSupOpeStatus1[] = _T("               %4.4u");    /* define thermal print format for guest check Id */

        /* print Cashier ID or GUEST CHECK No. */
        switch (pData->uchMinorClass) {
        case CLASS_PARAOPESTATUS_CASHIER:
			PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupOpeStatus, RflTruncateEmployeeNumber(pData->ulOperatorId));
            break;
        case CLASS_PARAOPESTATUS_GCNO:
			PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupOpeStatus1, (USHORT)pData->ulOperatorId);
            break;
        default:
            NHPOS_ASSERT_TEXT(0, "**ERROR: Unknown uchMinorClass in PrtThrmSupOpeStatus().");
            break;
        }
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        static const TCHAR  auchPrtSupOpeStatus[] = _T("   %8.8Mu");                    /* define EJ print format for Operator/Waiter Id */
        static const TCHAR  auchPrtSupOpeStatus1[] = _T("  %4.4u");                     /* define EJ print format for guest check Id */

        /* print Cashier ID or GUEST CHECK No. */        
        switch (pData->uchMinorClass) {
        case CLASS_PARAOPESTATUS_CASHIER:
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOpeStatus, pData->ulOperatorId);
            break;
        case CLASS_PARAOPESTATUS_GCNO:
            PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOpeStatus1, (USHORT)pData->ulOperatorId);
            break;
        default:
            NHPOS_ASSERT_TEXT(0, "**ERROR: Unknown uchMinorClass in PrtThrmSupOpeStatus().");
            break;
        }
    }
}
/***** End of Source *****/
