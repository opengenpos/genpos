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
* Title       : Multiline Display Service Time Report Format  ( SUPER & PROGRAM MODE )                       
* Category    : Multiline Display, NCR 2170 US Hospitality Application Program        
* Program Name: MLDSERVT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupServiceTime() : forms Service Time Report format        
*                      
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 03.00.   :           : Initial                                   
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
#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>


/*
*===========================================================================
** Synopsis:    VOID  MldRptSupServiceTime( RPTSERVICE *pData )  
*               
*     Input:    *pData  : pointer to structure for RPTSERVICE 
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms SALES COUNTER, AVERAGE SERVICE TIME formats
*               for Service Time Report.
*
*               The other print formats for Service Time Report
*               are same as HOURLY ACTIVITY REPORT.
*
*               : SERVICE TIME REPORT
*
*===========================================================================
*/

SHORT  MldRptSupServiceTime( RPTSERVICE *pData )  
{
    
    /* define thermal print format */
#if defined(DCURRENCY_LONGLONG)
	static const TCHAR FARCONST auchMldRptSupService1[] = _T("#%1u\t%5lu  %9lu.%02u");
	static const TCHAR FARCONST auchMldRptSupService2[] = _T("\t%5lu  %9lu.%02u");
#else
	static const TCHAR FARCONST auchMldRptSupService1[] = _T("#%1u\t%5u  %9lu.%02u");
    static const TCHAR FARCONST auchMldRptSupService2[] = _T("\t%5u  %9lu.%02u");
#endif

    TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    USHORT usRow=0;

    /* check minor class */
   
    switch(pData->uchMinorClass) {  
    case CLASS_RPTSERVICE_TTL:                  /* each service total element */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupService1,       /* format */
                  pData->sElement,              /* Element # */
                  pData->sCounter,              /* Sales Counter */
                  pData->lMinute,               /* Service Time(minute) */
                  ( USHORT)pData->chSecond);    /* Service Time(second) */
        break;

    case CLASS_RPTSERVICE_SEGTTL:               /* service total */

        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldRptSupService2,       /* format */
                  pData->sCounter,              /* Sales Counter */
                  pData->lMinute,               /* Service Time(minute) */
                  ( USHORT)pData->chSecond);    /* Service Time(second) */
        break;

    default:
        break;
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

/***** End of Source *****/

