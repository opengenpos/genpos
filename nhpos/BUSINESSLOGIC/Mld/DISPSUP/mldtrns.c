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
* Title       : mld display Mnemonics Format  ( SUPER & PROGRAM MODE )                       
* Category    : NCR 2170 US Hospitality Application Program        
* Program Name: MLDTRNT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
* 
*               MldRptSupTrans() : Transaction Mnemonics 
*                                   Special Mnemonics     print format
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 00.00.00 : M.Waki    : Initial                                   
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
#include <stdlib.h>
#include <pif.h>
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
#include <mld.h>
#include <mldlocal.h>
#include <mldsup.h>


/*
*===========================================================================
** Synopsis:    VOID  MldRptSupTrans( MAINTTRANS *pData )
*               
*     Input:    *pData      : pointer to structure for MAINTTRANS
*    Output:    Nothing 
*
** Return:      Nothing
*
** Description: This function forms Transaction Mnemonics 
*                                   Special Mnemonics     print format.
*
*                    : OPERATOR/GUEST CHECK STATUS REPORT
*                    : "ABORTED"
*                    : "ADDITION"
*                    : "DELETION"
*                    : TRAINING ID for EMERGENT WAITER FILE CLOSE,
*                                      CASHIER/WAITER RESET REPORT -FUNCTION TYPE 3-
*
*===========================================================================
*/

VOID  MldRptSupTrans( MAINTTRANS *pData, USHORT usStatus )
{

    static const TCHAR FARCONST auchMldRptSupTrans[] = _T("%s");

    TCHAR  aszString[MLD_SUPER_MODE_CLM_LEN+1];           /* buffer for formatted data */

    MldSPrintf(aszString,          /* display buffer */
              MLD_SUPER_MODE_CLM_LEN,      /* column length */
              auchMldRptSupTrans,          /* format */
              pData->aszTransMnemo);        /* mnemonics */

    /* display format data */
    MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, aszString, MLD_SUPER_MODE_CLM_LEN);

    if (usStatus) {                 /* aborted */
        MldScroll1Buff.uchCurItem = 0;  /* new page */
    }
}
/***** End of Source *****/
