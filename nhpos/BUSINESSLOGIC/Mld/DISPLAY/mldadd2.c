/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display  addcheck                   
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDADD2.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldAddChk2() : display addcheck
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include<ecr.h>
#include<paraequ.h>
#include<para.h>
#include<regstrct.h>
#include<transact.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID  MldAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays addcheck line.
*===========================================================================
*/
SHORT  MldAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {

        return (MldPrechkAddChk2(pItem, usType, usScroll));

    }

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID  MldAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll);
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays addcheck line.
*===========================================================================
*/
SHORT  MldPrechkAddChk2(ITEMAFFECTION *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[2][MLD_PRECOLUMN1 + 1];    /* print data save area */
    LONG   alAttrib[2][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
/*    TRANMODEQUAL TranModeQual; */

    /* initialize the area */
    memset(aszScrollBuff, '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    if ( pItem->lAmount == 0L ) {               /* if amount "0", not display */
        return (MldDummyScrollDisplay(usType, usScroll));
    }
    if (MldCheckSpecialStatus(usScroll, MLD_REORDER2_STATUS)) {   /* if post check, not display */
        return (MldDummyScrollDisplay(usType, usScroll));
    }

/*    TrnGetModeQual(&TranModeQual); */

    /* set add check, tax# 1-3 mnemonic and amount */

    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (UCHAR)(pItem->uchAddNum + TRN_ADCK1_ADR), pItem->lAmount, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/***** End Of Source *****/
