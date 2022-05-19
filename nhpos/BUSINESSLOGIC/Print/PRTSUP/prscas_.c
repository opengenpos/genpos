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
* Title       : Print Cashier No Assignment Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 Int'l Hospitality Application Program        
* Program Name: PRSCAS_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupCashierNo()    : This Function Forms Cashier No Assignment.
*                                   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-28-92: 00.00.01 : K.You     : Initial                                   
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

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupCashierNo( MAINTCASHIERIF *pData )
*               
*     Input:    *pData      : Pointer to Structure for MAINTCASasIF
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Cashier No Asssinment Print Format.
*===========================================================================
*/

VOID  PrtSupCashierNo( MAINTCASHIERIF *pData )
{
    /* Define Print Format */
    const TCHAR FARCONST *auchPrtSupCashier1 = _T("%6s\n\t%20s");
    const TCHAR FARCONST *auchPrtSupCashier2 = _T("%4s %4s %4s %4s");
    const TCHAR FARCONST *auchPrtSupCashier3 = _T("%4d %4d %1s %1s %2s\n\n");
    const TCHAR FARCONST *auchNumber   = _T("%3d");
    const TCHAR FARCONST *auchIdNumber = _T("%8.8Md");

    USHORT  usPrtType;
	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];

    /* Initialize Buffer */
    TCHAR   aszAddress1[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress2[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress3[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress4[6] = { 0, 0, 0, 0, 0, 0 };
    TCHAR   aszAddress5[3] = { 0, 0, 0 };
    TCHAR   aszAddress6[3] = { 0, 0, 0 };
    TCHAR   aszAddress7[4] = { 0, 0, 0, 0 };

    TCHAR   aszAddress8[PARA_CASHIER_LEN + 1];

    /* Check Print Control */
    usPrtType = PrtSupChkType(pData->usPrintControl);

    /* Convert Cashier Status Data to Binary ASCII Data */
    PrtSupItoa(( UCHAR)(pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0x0F), aszAddress1);
    PrtSupItoa(( UCHAR)((pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & 0xF0) >> 4), aszAddress2);
    PrtSupItoa(( UCHAR)(pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0x0F), aszAddress3);
    PrtSupItoa(( UCHAR)((pData->CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_1] & 0xF0) >> 4), aszAddress4);

    _itot(( UCHAR)pData->CashierIf.uchChgTipRate, aszAddress5, 10);
    _itot(( UCHAR)pData->CashierIf.uchTeamNo, aszAddress6, 10);
    _itot(( UCHAR)pData->CashierIf.uchTerminal, aszAddress7, 10);

    /* Convert Cashier No. to Double Wide */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchIdNumber, RflTruncateEmployeeNumber(pData->CashierIf.ulCashierNo));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* Copy Mnemonics Data to Buffer */
    memset(aszAddress8, 0, (PARA_CASHIER_LEN + 1) * sizeof(TCHAR));
    _tcsncpy(aszAddress8, pData->CashierIf.auchCashierName, PARA_CASHIER_LEN);

    /* 1st & 2nd Line */
    PmgPrintf(usPrtType, auchPrtSupCashier1, aszDoubRepoNumb, aszAddress8);
            
    /* 3rd Line */
    PmgPrintf(usPrtType, auchPrtSupCashier2, aszAddress1, aszAddress2, aszAddress3, aszAddress4);

    /* 4th Line */
    PmgPrintf(usPrtType, auchPrtSupCashier3, pData->CashierIf.usGstCheckStartNo, pData->CashierIf.usGstCheckEndNo,
              aszAddress5, aszAddress6, aszAddress7);                     /* Terminal Number */

}
