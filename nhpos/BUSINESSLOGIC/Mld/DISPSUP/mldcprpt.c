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
* Title       : Multilile display Coupon File Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program 
* Program Name: MLDCPRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*       MldRptSupCPNFile()    : This Function Forms Coupon File Print Format For LCD
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-26-95: 03.00.03 : M.Ozawa   : initial                               
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
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldmenu.h>
#include <mldlocal.h>


/*
*===========================================================================
** Synopsis:    VOID  MldRptSupCPNFile( RPTCPN *pData )  
*               
*     Input:    *pData  : Pointer to Structure for RPTCPN 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This Function Forms Coupon File Print Format For LCD
*===========================================================================
*/
SHORT  MldRptSupCPNFile( RPTCPN *pData, USHORT usStatus)
{
    static const TCHAR FARCONST auchMldRptSupCPNFile1[] = _T("        %6s         %-12s\n\t%12ld   %12l$");
    static const TCHAR FARCONST auchMldRptSupCPNFile2[] = _T("%-8s\t%12ld   %12l$");
    static const TCHAR FARCONST auchNumber[] = _T("%3d");

	/* See NHPOS_ASSERT() check on usRow value as well. */
	TCHAR  aszString[4 * (MLD_SUPER_MODE_CLM_LEN + 1)] = {0};    /* buffer for formatted data. See NHPOS_ASSERT() check on usRow value as well. */
    TCHAR  *pszString;
    TCHAR  aszRepoNumb[8 + 1] = {0};
    USHORT usRow = 0;

    /* Check Print Control */
    /* Check Minor Class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTCPN_ITEM:                      
        /* Convert CPN No. to Double Wide */
        RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usCpnNumber);

        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupCPNFile1, aszRepoNumb, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    case CLASS_RPTCPN_TOTAL:
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupCPNFile2, pData->aszMnemo, pData->CpnTotal.lCounter, pData->CpnTotal.lAmount);
        break;

    default:
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_01);
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);
		break;

    }
    if (!usRow) {
        return (MLD_SUCCESS);               /* if format is not created */
    }

    /* check format data is displayed in the scroll page */
    if (usStatus == RPT_IND_READ) {
        MldScroll1Buff.uchCurItem = 0;  /* not pause at indivisual read */
    } else {
        if (usRow + (USHORT)MldScroll1Buff.uchCurItem > MLD_SUPER_MODE_ROW_LEN) {
            MldScroll1Buff.uchCurItem = 0;  /* new page */
            /* create dialog */
            if (MldContinueCheck() == MLD_ABORTED) {
                return (MLD_ABORTED);
            }
        }

        MldScroll1Buff.uchCurItem += (UCHAR)usRow;
    }

	NHPOS_ASSERT(usRow < 4);         // Assert that usRow is less than or equal to size of aszString;

    pszString = &aszString[0];
    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN + 1);
    }

    return (MLD_SUCCESS);
}

/**** End of File ****/
