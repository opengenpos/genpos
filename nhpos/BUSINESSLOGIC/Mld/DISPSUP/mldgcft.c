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
* Title       : Multiline Display Guest Check File Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program
* Program Name: MLDGCFT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function forms GUEST CHECK FILE print format.
*
*           The provided function names are as follows: 
* 
*               MldRptSupGCF() : form GUEST CHECK FILE print format
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* Jul-24-95 : 03.00.00 : M.Ozawa   : Initial                                   
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
#include <pif.h>
#include <paraequ.h> 
#include <para.h>
#include <cswai.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csstbpar.h>
#include <csop.h>
#include <report.h>
#include <pmg.h>
#include <rfl.h>
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    SHORT MldRptSupGCF( RPTGUEST *pData )
*               
*     Input:    *pData      : pointer to structure for RPTGUEST
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms GUEST CHECK FILE print format.
*
*                   : GUEST CHECK FILE READ REPORT
*                   : GUEST CHECK FILE RESET REPORT
*
*               In case that RESET REPORT is not printed on RECEIPT,
*               RESET REPORT is printed on EJ.
*
*===========================================================================
*/

SHORT MldRptSupGCF( RPTGUEST *pData )
{

    /* define display format */

    static const TCHAR FARCONST auchMldRptSupGCF1[] = _T("%-8s %4u%02u\t%4s  %8.8Mu");
    static const TCHAR FARCONST auchMldRptSupGCF2[] = _T("%-8s %4u\t%4s  %8.8Mu");
    static const TCHAR FARCONST auchMldRptSupGCF3[] = _T("%-8s\t%2u:%02u");
    static const TCHAR FARCONST auchMldRptSupGCF4[] = _T("%-8s\t%2u:%02u%s");
    static const TCHAR FARCONST auchMldRptSupGCF5[] = _T("%-8s\t%3d");
    static const TCHAR FARCONST auchMldRptSupGCF6[] = _T("%-8s\t%12l$");
    static const TCHAR FARCONST auchMldRptSupGCF7[] = _T("%s");
	static const TCHAR FARCONST auchMldRptSupGCF8[] = _T("%-8s %4u *** \t%4s  %8.8Mu");

    TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    USHORT usHour;
    USHORT usRow=0;

    /* distinguish minor class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTGUEST_PRTCHKNOWCD:
        /* print GUEST No./WAIT NAME/No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF1, pData->aszMnemo, pData->usGuestNo, ( USHORT)(pData->uchCdv), pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
        break;
       
	case CLASS_RPTGUEST_PRTCHKNOWOCD_PREAUTH:
        /* print GUEST No./WAIT NAME/No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF8, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));

		break;

    case CLASS_RPTGUEST_PRTCHKNOWOCD:
        /* print GUEST No./WAIT NAME/No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF2, pData->aszMnemo, pData->usGuestNo, pData->aszSpeMnemo, RflTruncateEmployeeNumber(pData->ulCashierId));
        break;                                      
       
    case CLASS_RPTGUEST_PRTCHKOPEN:
        /* check if time is military hour or not */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {   /* time is military hour */
            /* print TIME */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF3, pData->aszMnemo, pData->auchCheckOpen[0], pData->auchCheckOpen[1]);
        } else {
            /* check if Hour is '0' */
            if (pData->auchCheckOpen[0] == 0) {     /* in case of "0:00 AM" */
                usHour = 12;
            } else {
                usHour = ( USHORT)pData->auchCheckOpen[0];
            }
       
            /* print TIME */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF4, pData->aszMnemo, (usHour > 12)? usHour - 12 : usHour, ( USHORT)pData->auchCheckOpen[1], (pData->auchCheckOpen[0] >= 12)? aszPrtPM : aszPrtAM);
        } 
        break;
       
    case CLASS_RPTGUEST_PRTPERSON:
		/* print No.of PERSON / OLD WAITER No.*/
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF5, pData->aszMnemo, pData->usNoPerson);
        break;
       
    case CLASS_RPTGUEST_PRTTBL:
        /* print TABLE No. */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF5, pData->aszSpeMnemo, pData->usNoPerson);
        break;
       
    case CLASS_RPTGUEST_PRTDGG:
    case CLASS_RPTGUEST_PRTTTL:
        /* print TOTAL / TRANS. BALANCE / CHECK TOTAL */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF6, pData->aszMnemo, pData->lCurBalance);
        break;

    case CLASS_RPTGUEST_PRTMNEMO:
        /* print TRAINING ID */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF7, pData->aszSpeMnemo);
        break;

    case CLASS_RPTGUEST_FILEINFO:
        /* print TRAINING ID */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupGCF7, pData->aszSpeMnemo);
            break;

    default:
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_03);
        PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);
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

    MldScroll1Buff.uchCurItem += (UCHAR)usRow;

    pszString = &aszString[0];

    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }

    return (MLD_SUCCESS);
}

SHORT MldRptSupOpeStatus( MAINTOPESTATUS *pData )
{
    const TCHAR FARCONST *auchPrtSupOpeStatus = _T("   %8.8Mu");      /* define EJ print format for Operator/Waiter Id */
    const TCHAR FARCONST *auchPrtSupOpeStatus1 = _T("  %4.4u");       /* define EJ print format for guest check Id */

	TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)] = {0};           /* buffer for formatted data */
	USHORT usRow = 0;

	if (pData->uchMinorClass == 1)
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchPrtSupOpeStatus, pData->ulOperatorId);
	else
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchPrtSupOpeStatus1, pData->ulOperatorId);

    MldScroll1Buff.uchCurItem += (UCHAR)usRow;
    MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, aszString, MLD_SUPER_MODE_CLM_LEN);
	return 0;
}
/***** End of Source *****/
