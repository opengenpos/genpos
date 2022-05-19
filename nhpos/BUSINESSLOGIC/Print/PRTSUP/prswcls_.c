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
* Title       : Emergent Cashier/Waiter File Close Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSWCLS_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtSupWFClose() : forms Cashier/Waiter Name/No. Print Format.       
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-21-93: 00.00.01 : J.IKEDA   : Initial                                   
* Aug-08-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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
#include <string.h>
#include <ecr.h>
/* #include <pif.h> */
#include <rfl.h>
/* #include <log.h> */
#include <paraequ.h> 
#include <para.h>
/* #include <cswai.h> */
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
** Synopsis:    VOID  PrtSupWFClose( VOID *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTWAITER/RPTCASHIER 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CASHIER/WAITER NAME/NUMBER print format.
*===========================================================================
*/

VOID  PrtSupWFClose( VOID *pData )  
{
    /* define print format */
    static const TCHAR FARCONST auchPrtSupWFClose[] = _T("%3s%-10s\t%6s");
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");

    TCHAR   aszPrtNull[1] = {'\0'};
	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];
    USHORT  usPrtType;
    TCHAR   *uchMnemo;
    UCHAR   uchMajorClass;
    ULONG   ulNumber = 0;

    /* check major class */
    uchMajorClass = *(( UCHAR *)pData);

    switch(uchMajorClass) {
    case CLASS_RPTOPENCAS:
        ulNumber = RflTruncateEmployeeNumber((( RPTCASHIER *)pData)->ulCashierNumber); 
        uchMnemo = (( RPTCASHIER *)pData)->aszCashMnemo;
        usPrtType = PrtSupChkType((( RPTCASHIER *)pData)->usPrintControl);
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    /* Convert Waiter No. to Double Wide */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, ulNumber);
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* Print Format */
    PmgPrintf(usPrtType, auchPrtSupWFClose, aszPrtNull, uchMnemo, aszDoubRepoNumb);
}

