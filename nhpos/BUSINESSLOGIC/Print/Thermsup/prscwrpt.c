/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Thermal Print Cashier/Waiter/Employee File Format  
*             :                                    ( SUPER & PROGRAM MODE )                       
* Category    : Print, NCR 2170 US Hospitality Application Program        
* Program Name: PRSCWRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               PrtThrmSupCashWaitFile() : form  Cashier  NAME/No.
*                                                Waiter   NAME/No.        
*                                                Employee No.      print format.
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jun-16-93 : 01.00.12 : J.IKEDA   : Initial                                   
* Nov-27-95 : 03.01.00 : M.Ozawa   : Add Employee mnemonic print
* Aug-11-99 : 03.05.00 : M.Teraki  : Remove WAITER_MODEL
* Dec-16-99 : 01.00.00 : hrkato    : Saratoga
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

#include "ecr.h"
/* #include <pif.h> */
#include "rfl.h"
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "cswai.h"
#include "maint.h"
#include "regstrct.h"
#include "transact.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "pmg.h"
/* #include <appllog.h> */

#include "prtrin.h"
#include "prtsin.h"
         
/*
*===========================================================================
** Synopsis:    VOID  PrtThrmSupCashWaitFile( VOID *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTCASHIER/RPTWAITER/RPTEMPLOYEE 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms Cashier/Waiter NAME/No. 
*                                   Employee No.           print format.
*
*                    CASHIER/WAITER NAME is single size.
*                    CASHIER/WAITER/EMPLOYEE No.  is doble wide size.
*
*              In case of CASHIER/WAITER REPORT, the other print formats 
*              are same as REGISTER FINANCIAL REPORT.
*
*              In case of EMPLOYEE REPORT, the other print formats are 
*              formed in other function.
*                     Start/End Time             - PRSREGFT.C
*                     Time In/Out Data, Job Code - PRSETKT.C  
*
* < THERMAL & E/J >
*             : CASHIER FILE READ REPORT   (only CASHIER NAME/No. format)
*             : CASHIER FILE RESET REPORT  (only CASHIER NAME/No. format)
*             : WAITER FILE READ REPORT    (only WAITER NAME/No. format)
*             : WAITER FILE RESET REPORT   (only WAITER NAME/No. format)
*             : EMPLOYEE TIME KEEPING FILE READ REPORT  (only EMPLOYEE No. format)
*             : EMPLOYEE TIME KEEPING FILE RESET REPORT (only EMPLOYEE No. format)
*             : EMPLOYEE TIME KEEPING FILE MAINTENANCE  (only EMPLOYEE No. format)
*
*   NOTE : CASHIER/WAITER/EMPLOYEE ID on EJ is printed at ONLY RESET REPORT 
*          and at EMPLOYEE TIME KEEPING FILE MAINTENANCE
*                                           
*===========================================================================
*/

VOID  PrtThrmSupCashWaitFile( VOID *pData )  
{
    /* define thermal print format */
    static const TCHAR FARCONST auchPrtThrmSupCashWaitFile[] = _T("%6s%-20s%2s%8s");    /* V3.3 */
    static const TCHAR FARCONST auchPrtThrmSupEmployeeNo[] = _T("%6s%-16s%3s%8.8Mu");

    /* define EJ print format */
    static const TCHAR FARCONST auchPrtSupCashWaitFile[] = _T("%3s%s\t%s");
    static const TCHAR FARCONST auchPrtSupEmployeeNo[] = _T("%-s\t%8.8Mu");

    /* define thermal/EJ common print format */
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");
    static const TCHAR FARCONST auchNumber1[] = _T("%3u");

    TCHAR   aszPrtNull[1] = {'\0'};
	TCHAR	aszDoubNumb[8 * 2 + 1];
	TCHAR	aszNumb[8 + 1];
    UCHAR   uchMajorClass;
    UCHAR   uchMinorClass;
    TCHAR   *uchMnemo;
    ULONG   ulNumber;
    USHORT  usPrtControl;

    /* check major class */
    uchMajorClass = *(( UCHAR *)pData);

    switch(uchMajorClass) {
    case CLASS_RPTCASHIER:  /* In case of CASHIER/EMPLOYEE */
        uchMinorClass = (( RPTCASHIER *)pData)->uchMinorClass;
        ulNumber = (( RPTCASHIER *)pData)->ulCashierNumber;

		if (uchMinorClass != CLASS_RPTCASHIER_TERMINAL)
			ulNumber = RflTruncateEmployeeNumber (ulNumber);

        uchMnemo = (( RPTCASHIER *)pData)->aszCashMnemo;
        usPrtControl = (( RPTCASHIER *)pData)->usPrintControl;
		/* modified for slip printer is not connected case, 09/19/01 */
    	PrtPortion(usPrtControl);
	    if ((fsPrtPrintPort & PRT_SLIP) && (uchMinorClass == CLASS_MAINTLOAN || uchMinorClass == CLASS_MAINTPICKUP)) {
            usPrtControl = PRT_JOURNAL | PRT_SLIP;
        }
        break;

    case CLASS_RPTEMPLOYEENO: /* In case of EMPLOYEE */                   
        uchMinorClass = (( RPTEMPLOYEENO *)pData)->uchMinorClass;
        uchMnemo = (( RPTEMPLOYEENO *)pData)->aszEmpMnemo;
        usPrtControl = (( RPTEMPLOYEENO *)pData)->usPrintControl;
        break;

    default:
/*        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }

    if (uchMajorClass == CLASS_RPTEMPLOYEENO) {
        /* check print control */
        if (usPrtControl & PRT_RECEIPT) {
            /* print EMPLOYEE No.*/
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupEmployeeNo, aszPrtNull, uchMnemo, aszPrtNull, RflTruncateEmployeeNumber((( RPTEMPLOYEENO *)pData)->ulEmployeeNumber));
        }

        /* --- Save Employee# and Name into EJ File,    TAR#116110 --- */
        if (uchMinorClass == CLASS_RPTEMPLOYEE_RESET || uchMinorClass == CLASS_RPTEMPLOYEE_NUMBER) {
            /* check print control */
            if (usPrtControl & PRT_JOURNAL) {
                /* print EMPLOYEE No.*/
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupEmployeeNo, uchMnemo, (( RPTEMPLOYEENO *)pData)->ulEmployeeNumber);
            }            
        }
    } else {
        /* convert Cashier/Waiter No. to double wide */
		if ((( RPTCASHIER *)pData)->uchMinorClass == CLASS_RPTCASHIER_TERMINAL)
			RflSPrintf(aszNumb, TCHARSIZEOF(aszNumb), auchNumber1, ulNumber);
		else
			RflSPrintf(aszNumb, TCHARSIZEOF(aszNumb), auchNumber, ulNumber);
        memset(aszDoubNumb, '\0', sizeof(aszDoubNumb));
        PrtDouble(aszDoubNumb, TCHARSIZEOF(aszDoubNumb), aszNumb);

        /* check print control */
        if (usPrtControl & PRT_RECEIPT) {
            /* print CASHIER/WAITER NAME/No. */
            PrtPrintf(PMG_PRT_RECEIPT, auchPrtThrmSupCashWaitFile, aszPrtNull, uchMnemo, aszPrtNull, aszDoubNumb);
        }

        /* check minor class */
        if (uchMinorClass == CLASS_RPTWAITER_RESET || uchMinorClass == CLASS_RPTCASHIER_TERMINAL || uchMinorClass == CLASS_RPTCASHIER_RESET || uchMinorClass == CLASS_MAINTLOAN || uchMinorClass == CLASS_MAINTPICKUP) {
            /* check print control */
            if (usPrtControl & PRT_JOURNAL) {
                /* print CASHIER/WAITER NAME/No. */
                PrtPrintf(PMG_PRT_JOURNAL, auchPrtSupCashWaitFile, aszPrtNull, uchMnemo, aszDoubNumb);
            }            
        }

        /* check print control, Saratoga */
        if ((usPrtControl & PRT_SLIP) && (uchMinorClass == CLASS_MAINTLOAN || uchMinorClass == CLASS_MAINTPICKUP)) {
            /* print CASHIER/WAITER NAME/No. */
            PrtPrintf(PMG_PRT_SLIP, auchPrtThrmSupCashWaitFile, aszPrtNull, uchMnemo, aszPrtNull, aszDoubNumb);
        }
    }
}

/***** End of Source *****/