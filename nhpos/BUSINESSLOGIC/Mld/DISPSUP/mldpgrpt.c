/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Programable Report Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 Hospitality Application Program        
* Program Name: MLDPGRPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupProgRpt(): This Function Forms Programable Report Print Format
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Feb-23-96: 03.01.00 : M.Ozawa   : Initial 
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
#include <appllog.h>
#include <mld.h>
#include <mldsup.h>
#include <mldlocal.h>

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupProgRpt( RPTPROGRAM *pData )
*               
*     Input:    *pData      : Pointer to Structure for RPTPROGRAM
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms Programable Report Print Format.
*===========================================================================
*/

SHORT  MldRptSupProgRpt( RPTPROGRAM *pData )
{

    static const TCHAR FARCONST auchMldSupProgRpt[] = _T("%-40s");

    TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {'\0'};
    USHORT usRow=0;

    /* Edit Programable Report Format */

    usRow = MldSPrintf(aszString,          /* display buffer */
              MLD_SUPER_MODE_CLM_LEN,      /* column length */
              auchMldSupProgRpt,           /* Format */
              pData->aszLineData);         /* Record Data */

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

/**** End of File ****/
