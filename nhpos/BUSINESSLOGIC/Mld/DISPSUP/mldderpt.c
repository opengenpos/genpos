/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display DEPT/MDEPT File Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDDERPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupDEPTFile() : forms DEPT/MDEPT File print format.       
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 03.00.00 :           : Initial                                   
*  12-27-99: 01.00.00 : hrkato    : Saratoga
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
** Synopsis:    VOID  MldRptSupDEPTFile( RPTDEPT *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTDEPT 
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function forms DEPT/MDEPT File print format.
*
*                  : DEPARTMENT SALES READ REPORT        (except START/END TIME)
*                  : MAJOR DEPART MENT SALES READ REPORT (except START/END TIME)
*
*             cf. START/END TIME formats are same as REGISTER FINANCIAL REPORT.
*
*===========================================================================
*/

SHORT  MldRptSupDEPTFile( RPTDEPT *pData , USHORT usStatus)  
{
    /* define thermal print format */ 
    static const TCHAR FARCONST auchMldRptSupDEPTFile1[] = _T("%17u %s");
    static const TCHAR FARCONST auchMldRptSupDEPTFile2A0[] = _T("%8s %-20s\t*****%%\n\t%6ld   %12l$");    /* Saratoga */
    static const TCHAR FARCONST auchMldRptSupDEPTFile2B0[] = _T("%8s %-20s\t%6.2$%%\n\t%6ld   %12l$");   /* Saratoga */
    static const TCHAR FARCONST auchMldRptSupDEPTFile30[] = _T("%4s   %-20s\n\t%6ld   %12l$");              /* Saratoga */
    static const TCHAR FARCONST auchMldRptSupDEPTFile4[] = _T("%-8s\t%6ld   %12l$");
    static const TCHAR FARCONST auchMldRptSupDEPTFile50[] = _T("%-8s\t%6ld   %12l$");
    static const TCHAR FARCONST auchNumber[] = _T("%4d");           /* Saratoga */

	/* See NHPOS_ASSERT() check on usRow value as well. */
	TCHAR  aszString[3 * (MLD_SUPER_MODE_CLM_LEN+1)] = {0};           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {'\0'};
    TCHAR  aszRepoNumb[8 + 1] = {0};
    USHORT usRow=0;
       
    /* check minor class */
    switch(pData->uchMinorClass) {
    case CLASS_RPTDEPT_PRTMAJOR:                
        /* print MDEPT in dept report */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile1, pData->uchMajorDEPTNo, pData->aszMnemo);
        break;
       
   case CLASS_RPTDEPT_PRTTTLCNT1:                      
       /* convert DEPT number to double wide */
       RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);     /* Saratoga */
  
       /* check if PERCENT is illegal */
       if (pData->usPercent > PRT_MAX_PERCENT) {           /* in case that percent data is illegal,    */
                                                           /* pData->usPercent is set over 600.00 data */
           /* print PLUS DEPT with illegal percent data */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile2A0, aszRepoNumb, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
       } else {
           /* print PLUS DEPT with legal percent data */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile2B0, aszRepoNumb, pData->aszMnemo, ( SHORT)pData->usPercent,  pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
       }
       break;
  
   case CLASS_RPTDEPT_PRTTTLCNT2:
       /* convert DEPT Number to double wide */
       RflSPrintf(aszRepoNumb, TCHARSIZEOF(aszRepoNumb), auchNumber, pData->usDEPTNumber);     /* Saratoga */
  
       /* print MINUS DEPT */ 
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile30, aszRepoNumb, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
       break;
  
   case CLASS_RPTDEPT_PRTTTLCNT3:
       /* print SUBTOTAL/PLUS/MINUS/HASH/CIGARETTE/GRAND TOTAL */
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile50, pData->aszMnemo, pData->DEPTTotal.lCounter, pData->DEPTTotal.lAmount);
       break;
  
   case CLASS_RPTDEPT_PRTMDEPTTTL:
       /* print MDEPT */ 
        usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupDEPTFile4, pData->aszMnemo, pData->DEPTTotal.lCounter,  pData->DEPTTotal.lAmount);
       break;
  
   default:
       PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_CODE_06);
       PifLog(MODULE_PRINT_SUPPRG_ID, LOG_ERROR_PRT_SUPPRG_ERROR);
	   break;
   }

    if (!usRow) {
        return (MLD_SUCCESS);               /* if format is not created */
    }

    /* check format data is displayed in the scroll page */
    if (usStatus == RPT_DEPTIND_READ) {
        MldScroll1Buff.uchCurItem = 0;  /* not pause at indivisual read */
    } else if ((usStatus == RPT_TYPE_MDEPTIND) && (pData->uchMinorClass == CLASS_RPTDEPT_PRTMAJOR)) {
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

	NHPOS_ASSERT(usRow <= 3);         // Assert that usRow is less than or equal to size of aszString;

    pszString = &aszString[0];
    while (usRow--) {
        /* display format data */
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }

    return (MLD_SUCCESS);
}
                           
/***** End of Source *****/