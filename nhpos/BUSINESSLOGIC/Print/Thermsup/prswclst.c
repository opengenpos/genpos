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
* Title       : Thermal Print Emergent Cashier/Waiter File Close Format  ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSWCLST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupWFClose() : form Cashier/Waiter NAME/No. print format.       
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-21-93: 01.00.12 : J.IKEDA   : Initial                                   
* Aug-11-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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
** Synopsis:    VOID  PrtThrmSupWFClose( VOID *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTWAITER/RPTCASHIER 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms CASHIER/WAITER NAME/No. print format.
*                    CASHIER/WAITER NAME is single size.
*                    CASHIER/WAITER No.  is doble wide size.
*
*             : EMERGENT CASHIER FILE CLOSE
*             : EMERGENT WAITER FILE CLOSE
*
*===========================================================================
*/
VOID  PrtThrmSupWFClose( VOID *pData )
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupWFClose[] = _T("%6s%-10s%13s%6s");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupWFClose[] = _T("%3s%-10s\t%6s");

    /* define thermal/EJ common print format */
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");

    TCHAR   aszPrtNull[1] = {_T('\0')};
	TCHAR	aszDoubRepoNumb[8 * 2 + 1];
	TCHAR	aszRepoNumb[8 + 1];
    TCHAR   *uchMnemo;
    UCHAR   uchMajorClass;
    USHORT  usPrtControl;
    ULONG   ulNumber;

    /* check major class */
    uchMajorClass = *(( UCHAR *)pData);

    switch(uchMajorClass) {
    case CLASS_RPTOPENCAS:
        ulNumber = RflTruncateEmployeeNumber((( RPTCASHIER *)pData)->ulCashierNumber); 
        uchMnemo = (( RPTCASHIER *)pData)->aszCashMnemo;
        usPrtControl = (( RPTCASHIER *)pData)->usPrintControl;
        break;

    default:
/*      PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }            

    /* convert Cashier/Waiter No. to double wide */
    RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, ulNumber);
    memset(aszDoubRepoNumb, '\0', sizeof(aszDoubRepoNumb));
    PrtDouble(aszDoubRepoNumb, TCHARSIZEOF(aszDoubRepoNumb), aszRepoNumb);

    /* check print control */
    if (usPrtControl & PRT_RECEIPT) {
        /* print CASHIER/WAITER NAME/No. */
        PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupWFClose, aszPrtNull, uchMnemo, aszPrtNull, aszDoubRepoNumb);
    }

    if (usPrtControl & PRT_JOURNAL) {
        /* print CASHIER/WAITER NAME/No. */
        PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupWFClose, aszPrtNull, uchMnemo, aszDoubRepoNumb);
    }
}
/***** End of Source *****/
