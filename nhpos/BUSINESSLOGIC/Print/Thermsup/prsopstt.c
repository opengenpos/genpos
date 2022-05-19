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
/* #include <cswai.h> */
#include <maint.h> 
/* #include <regstrct.h> */
/* #include <transact.h> */
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <rfl.h>

#include "prtcom.h"
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
** Description: This function forms Server/GC No. print format.
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
    const TCHAR FARCONST *auchPrtThrmSupOpeStatus = _T("                %8.8Mu");   /* define thermal print format for Operator/Waiter Id */
    const TCHAR FARCONST *auchPrtThrmSupOpeStatus1 = _T("               %4.4u");    /* define thermal print format for guest check Id */
    const TCHAR FARCONST *auchPrtSupOpeStatus = _T("   %8.8Mu");                    /* define EJ print format for Operator/Waiter Id */
    const TCHAR FARCONST *auchPrtSupOpeStatus1 = _T("  %4.4u");                     /* define EJ print format for guest check Id */

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print WAITER ID/GUEST CHECK No. */
		if (pData->uchMinorClass == 1)
			PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupOpeStatus, RflTruncateEmployeeNumber(pData->ulOperatorId));
		else
			PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupOpeStatus1, RflTruncateEmployeeNumber(pData->ulOperatorId));
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
        /* print WAITER ID/GUEST CHECK No. */        
		if (pData->uchMinorClass == 1)
			PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOpeStatus, pData->ulOperatorId);
		else
			PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupOpeStatus1, pData->ulOperatorId);
    }
}
/***** End of Source *****/
