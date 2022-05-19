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
* Title       : Waiter/Cashier/Employee File Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCWRP_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupCashWaitFile()    : Cashier/Waiter/Employee File Print Format.       
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-29-92: 00.00.01 : K.You     : initial
* Jun-23-93: 02.00.00 : M.Yamamoto: Adds CLASS_RPTEMPLOYEE
* Jun-29-93: 01.00.12 : J.IKEDA   : Add Employee No. print format                                   
* Nov-27-94: 03.01.00 : M.Ozawa   : Distinguish employee no format
* Aug-08-99: 03.05.00 : M.Teraki   : Remove WAITER_MODEL
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
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"

/*
*===========================================================================
** Synopsis:    VOID  PrtSupCashWaitFile( VOID *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTCASHIER,RPTWAITER, RPTEMPLOYEE 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Cashier/Waiter/Employee File Print Format.
*===========================================================================
*/

VOID  PrtSupCashWaitFile( VOID *pData )  
{
    static const TCHAR FARCONST auchPrtSupCashWaitFile[] = _T("%3s%s\t%s");
    static const TCHAR FARCONST auchPrtSupEmployeeNo[] = _T("%-s\t%8.8Mu");
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");

    TCHAR   aszPrtNull[1] = {'\0'};
	TCHAR	aszDoubNumb[8 * 2 + 1];
	TCHAR	aszNumb[12];
    TCHAR   *uchMnemo;
    UCHAR   uchMajorClass;
    ULONG   ulNumber = 0;
    USHORT  usPrtType;

    /* Check Major Class */
    uchMajorClass = *(( UCHAR *)pData);

    switch(uchMajorClass) {
    case CLASS_RPTCASHIER:  /* CASHIER */
        ulNumber = RflTruncateEmployeeNumber((( RPTCASHIER *)pData)->ulCashierNumber); 
        uchMnemo = (( RPTCASHIER *)pData)->aszCashMnemo;
        usPrtType = PrtSupChkType((( RPTCASHIER *)pData)->usPrintControl);
        break;

    case CLASS_RPTEMPLOYEENO:   /* EMPLOYEE */                   
        uchMnemo = (( RPTEMPLOYEENO *)pData)->aszEmpMnemo;
        usPrtType = PrtSupChkType((( RPTEMPLOYEENO *)pData)->usPrintControl);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        return;
    }

    if (uchMajorClass == CLASS_RPTEMPLOYEENO) {
        PmgPrintf(usPrtType, auchPrtSupEmployeeNo, uchMnemo, RflTruncateEmployeeNumber((( RPTEMPLOYEENO *)pData)->ulEmployeeNumber));
    } else {
        /* Convert Cashier/Waiter No. to Double Wide */
        RflSPrintf(aszNumb, TCHARSIZEOF(aszNumb), auchNumber, ulNumber);
        memset(aszDoubNumb, '\0', sizeof(aszDoubNumb));
        PrtDouble(aszDoubNumb, TCHARSIZEOF(aszDoubNumb), aszNumb);

        /* Print Format */
        PmgPrintf(usPrtType, auchPrtSupCashWaitFile, aszPrtNull, uchMnemo, aszDoubNumb);
    }
}
