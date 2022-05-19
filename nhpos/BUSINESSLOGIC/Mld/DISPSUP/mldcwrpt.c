/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Cashier/Waiter/Employee File Format  
*             :                                    ( SUPER & PROGRAM MODE )                       
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDCWRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldSupCashWaitFile() : form  Cashier  NAME/No.
*                                                Waiter   NAME/No.        
*                                                Employee No.      print format.
*                                       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jul-24-95 : 03.00.00 : M.Ozawa   : Initial                                   
* Aug-11-99 : 03.05.00 : M.Teraki  : Remove WAITER_MODEL
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
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    VOID  MldSupCashWaitFile( VOID *pData )  
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
SHORT MldRptSupCashWaitFile( VOID *pData )  
{
    /* define thermal print format */
    static const TCHAR FARCONST auchMldRptSupCashWaitFile[] = _T("%6s%-10s%11s%8s     ");
    static const TCHAR FARCONST auchMldRptSupEmployeeNo[] = _T("%6s%-16s%4s%8.8Mu");
    static const TCHAR FARCONST auchNumber[] = _T("%8.8Mu");
    static const TCHAR FARCONST auchNumber1[] = _T("%3u");

	/* See NHPOS_ASSERT() check on usRow value as well. */
	TCHAR   aszString[3 * (MLD_SUPER_MODE_CLM_LEN+1)] = {0};           /* buffer for formatted data */
    TCHAR   aszPrtNull[1] = {'\0'};
	TCHAR	aszNumb[8 + 1] = {0};
    UCHAR   uchMajorClass = *(( UCHAR *)pData);
    TCHAR   *uchMnemo;
    ULONG   ulNumber;
    TCHAR   *pszString;
    USHORT  usRow=0;

    switch(uchMajorClass) {
    case CLASS_RPTCASHIER:  /* In case of CASHIER/EMPLOYEE */
        ulNumber = (( RPTCASHIER *)pData)->ulCashierNumber;

		if ((( RPTCASHIER *)pData)->uchMinorClass != CLASS_RPTCASHIER_TERMINAL)
			ulNumber = RflTruncateEmployeeNumber (ulNumber);

        uchMnemo = (( RPTCASHIER *)pData)->aszCashMnemo;
        break;

    case CLASS_RPTEMPLOYEENO:   /* In case of EMPLOYEE */                   
        uchMnemo = (( RPTEMPLOYEENO *)pData)->aszEmpMnemo;
        break;

    default:
/*        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR); */
        break;
    }


    if (uchMajorClass == CLASS_RPTEMPLOYEENO) {
        /* print EMPLOYEE NAME/No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupEmployeeNo, aszPrtNull, uchMnemo, aszPrtNull, RflTruncateEmployeeNumber(((( RPTEMPLOYEENO *)pData)->ulEmployeeNumber)));
    } else {
        /* convert Cashier/Waiter No. to double wide */
		if ((( RPTCASHIER *)pData)->uchMinorClass == CLASS_RPTCASHIER_TERMINAL)
			RflSPrintf(aszNumb, TCHARSIZEOF(aszNumb), auchNumber1, ulNumber);
		else
			RflSPrintf(aszNumb, TCHARSIZEOF(aszNumb), auchNumber, ulNumber);
       
        /* print CASHIER/WAITER NAME/No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupCashWaitFile, aszPrtNull, uchMnemo, aszPrtNull, aszNumb);
    }

    if (!usRow) {
        return (MLD_SUCCESS);               /* if format is not created */
    }

    /* check format data is displayed in the scroll page */
    if (usRow + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
        MldScroll1Buff.uchCurItem = 0;  /* new page */

        /* create dialog */
        if (MldContinueCheck() == MLD_ABORTED) {
            return (MLD_ABORTED);
        }
    }

	NHPOS_ASSERT(usRow < 3);         // Assert that usRow is less than or equal to size of aszString;

    MldScroll1Buff.uchCurItem += (UCHAR)usRow;

    pszString = &aszString[0];
    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }

    return (MLD_SUCCESS);
}

/***** End of Source *****/
