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
* Title       : Display Any Data & Any Mnemonics To First Display Line                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPFL20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows: 
*               
*               DispSupFline20() : display any mnemonics to first display line
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* May-12-92: 00.00.01 : J.Ikeda   : initial                                   
* Dec-08-95: 03.01.00 : M.Ozawa   : Enhanced to 2x20 customer display
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
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "disp20.h"


/*
*=====================================================================================
** Synopsis:    VOID DispSupFLine20( MAINTDSPFLINE *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays any mnemonics to first display line.
*======================================================================================
*/
 
VOID DispSupFLine20( MAINTDSPFLINE *pData )
{
    static const TCHAR  auchDispSupFLine20OP[] = _T("\t%s\n%-18s\t%s");
    static const TCHAR  auchDispSupFLine20CUS[] = _T("                    \n%-20s");
    static const TCHAR  auchDispSupFLine10CUS[] = _T("%20s");   /* Addition for V3.3 */


    TCHAR   aszDspNull[1] = {0};
    TCHAR   aszDspPage[3] = {_T(' '),_T(' '),_T('\0')};

    switch(pData->uchMinorClass) {
    case CLASS_MAINTPREMD_OP:
    case CLASS_MAINTPREMD_OPPROG:                   /* Addition for V3.3 */
        DispSupSaveData.MaintDspFLine = *pData;      /* Save Data for Redisplay if needed. */

        if(uchMaintMenuPage){
            _itot(uchMaintMenuPage, aszDspPage, 10);
        }

        /* Display Foreground */
        UiePrintf(UIE_OPER,                         /* operator display */
                  0,                                /* row */
                  0,                                /* column */
                  auchDispSupFLine20OP,             /* format */
                  pData->aszMnemonics,              /* data */
                  aszDspNull,                       /* NULL */
                  aszDspPage);                      /* Page Number */

        /* Display Background */
        UieEchoBackGround(UIE_KEEP,                 /* descriptor control */
                          UIE_ATTR_NORMAL,          /* attribute */
                          auchDispSupFLine20OP,     /* format */
                          pData->aszMnemonics,      /* mnemonics */
                          aszDspNull,               /* null   */
                          aszDspPage);              /* Page Number */
        break;

    default: /* CLASS_MAINTPREMD_CUS */
        /* Display Foreground */
        if((PifSysConfig()->uchCustType == DISP_2X20) || /* 2X20 customer display ? */
            (PifSysConfig()->uchCustType == DISP_4X20)) { /* 2172 */
            UiePrintf(UIE_CUST,                         /* custmer display */
                      0,                                /* row */
                      0,                                /* column */
                      auchDispSupFLine20CUS,            /* format (Change for V3.3) */
                      pData->aszMnemonics);             /* data */

        } else if (PifSysConfig()->uchCustType == DISP_10N10D) {
			TCHAR   aszDspCus[PARA_LEADTHRU_LEN + 1] = _T(" . . . . . . . . . .");   /* Addition for V3.3 */

            UiePrintf(UIE_CUST,                         /* customer display */
                      0,                                /* row */
                      0,                                /* column */
                      auchDispSupFLine10CUS,            /* format (Change for V3.3) */
                      aszDspCus);
        }    
        break;

    }
}
