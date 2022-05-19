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
* Title       : Multiline Display Electoric Journal Report Format  ( SUPER & PROGRAM MODE )
* Category    : Multiline Display, NCR 2170 Hospitality Application Program        
* Program Name: MLDEJT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupEJ()      : This Function Forms EJ Report Print Format
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Aug-08-95: 03.00.04 : M.Ozawa   : Initial 
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
#include <memory.h>
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
#include "rfl.h"

#include <mldmenu.h>

/*
*===========================================================================
** Synopsis:    VOID  MldRptSupEJ( RPTEJ *pData )
*               
*     Input:    *pData      : Pointer to Structure for RPTEJ
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This Function Forms EJ Report Print Format.
*===========================================================================
*/
SHORT  MldRptSupEJ( RPTEJ *pData )
{
    static const TCHAR FARCONST auchMldSupEJ[] = _T("%-8s%-32s");

	/* See NHPOS_ASSERT() check on usRow value as well. */
	TCHAR  aszString[3 * (MLD_SUPER_MODE_CLM_LEN+1)] = {0};           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usRow=0;

    switch(pData->uchMinorClass) {
    case CLASS_RPTEJ_PAGE:
        /* not display */
        break;

    case CLASS_RPTEJ_REVERSE:
        /* not display */
        break;

    default:                            /* CLASS_RPTEJ_LINE */
        /* Edit EJ Report Format */
        usRow = MldSPrintf(aszString,          /* display buffer */
                  MLD_SUPER_MODE_CLM_LEN,      /* column length */
                  auchMldSupEJ,                /* Format */
                  aszPrtNull,
                  pData->aszLineData);         /* EJ Record Data */
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

/**** End of File ****/
