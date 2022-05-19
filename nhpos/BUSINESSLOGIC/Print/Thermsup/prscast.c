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
* Title       : Thermal Print Cashier No Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 INT'L Hospitality Application Program        
* Program Name: PRSCAST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupCashierNo() : forms Cashier No Assignment print format
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jan-29-93: 00.00.01 : J.IKEDA   : Initial                                   
* Aug-18-98: 03.03.00 : H.Iwata   :                                    
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
#include <string.h>
#include <ecr.h>
#include <stdlib.h>
#include <pif.h>
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <csop.h>
#include <cscas.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <report.h>
#include <pmg.h>

#include "prtcom.h"
#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupCashierNo( MAINTCASHIERIF *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTCASHIERIF
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Cashier No. Assignment print format.
*
*                    : ASSIGNMENT OF RANGE OF GC. NUMBER FOR CASHIER
*                    : CASHIER No. ASSIGNMENT
*
*===========================================================================
*/

VOID  PrtThrmSupCashierNo( MAINTCASHIERIF *pData )
{
    /* define thermal print format */
	static const TCHAR FARCONST auchPrtThrmSupCashier1[] = _T("%6s  %-20s");
    static const TCHAR FARCONST auchPrtThrmSupCashier2[] = _T("%4s %4s %4s %4s %4u %4u %1s %1s %2s");

    /* define EJ print format */
    const TCHAR FARCONST *auchPrtSupCashier1 = _T("%6s\n\t%20s");
    const TCHAR FARCONST *auchPrtSupCashier2 = _T("%4s %4s %4s");
    const TCHAR FARCONST *auchPrtSupCashier3 = _T("%4s %4u %4u\n%1s %1s %2s");

    /* define thermal/EJ common print format */
    const TCHAR FARCONST *auchIdNumber = _T("%8.8Mu");

	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];

    /* initialize buffer */
    TCHAR   aszAddress1[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress2[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress3[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress4[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress5[3] = { 0, 0, 0 };
    TCHAR   aszAddress6[3] = { 0, 0, 0 };
    TCHAR   aszAddress7[4] = { 0, 0, 0, 0 };
    TCHAR   aszAddress8[PARA_CASHIER_LEN + 1];

    /* convert cashier status data to binary ASCII data */
    PrtSupItoa(( UCHAR)(pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0x0F), aszAddress1);
    PrtSupItoa(( UCHAR)((pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0xF0) >> 4), aszAddress2);
    PrtSupItoa(( UCHAR)(pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0x0F), aszAddress3);
    PrtSupItoa(( UCHAR)((pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0xF0) >> 4), aszAddress4);

    /* Add aszAddress5 - 7 980813 */
    _itot(( UCHAR)(pData->CashierIf.uchChgTipRate), aszAddress5, 10);
    _itot(( UCHAR)(pData->CashierIf.uchTeamNo), aszAddress6, 10);
    _itot(( UCHAR)(pData->CashierIf.uchTerminal), aszAddress7, 10);

    /* convert cashier no. to double wide */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchIdNumber, RflTruncateEmployeeNumber(pData->CashierIf.ulCashierNo));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* copy mnemonics data to buffer */
    memset(aszAddress8, 0, (PARA_CASHIER_LEN + 1) * sizeof(TCHAR));
    _tcsncpy(aszAddress8, pData->CashierIf.auchCashierName, PARA_CASHIER_LEN);

    /* check print control */
    if (pData->usPrintControl & PRT_RECEIPT) {  /* THERMAL PRINTER */
        /* print CASHIER NO./NAME / START/END GC NO. */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCashier1, aszDoubRepoNumb, aszAddress8, pData->CashierIf.usGstCheckStartNo, pData->CashierIf.usGstCheckEndNo);
            
        /* print STATUS */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCashier2, aszAddress1, aszAddress2, aszAddress3, aszAddress4,
					pData->CashierIf.usGstCheckStartNo, pData->CashierIf.usGstCheckEndNo, aszAddress5, aszAddress6, aszAddress7);
    } 
    
    if (pData->usPrintControl & PRT_JOURNAL) {  /* EJ */
		// If to the Electronic Journal then we do not do masking of the operator number
		RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchIdNumber, pData->CashierIf.ulCashierNo);
		PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

        /* print CASHIER NO./NAME */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCashier1, aszDoubRepoNumb, aszAddress8);
                
        /* print STATUS */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCashier2, aszAddress1, aszAddress2, aszAddress3);
        
        /* print START/END GC No. / Charge Tip Rate / Team No. /Terminal No. */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCashier3, aszAddress4, pData->CashierIf. usGstCheckStartNo, pData->CashierIf.usGstCheckEndNo,
                  aszAddress5, aszAddress6,  aszAddress7);

    }

}       
        
