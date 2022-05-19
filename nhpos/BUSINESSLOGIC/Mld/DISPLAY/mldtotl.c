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
* Title       : Multiline Display total key (SubTotal, no finalize total, finalize total)               
* Category    : Multiline, NCR 2170 US Hospitarity Application 
* Program Name: MLDTOTL.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldTotal() : prints total key (Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "rfl.h"
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "mld.h"
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key
*===========================================================================
*/
SHORT MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveTotal(pItem, usType, usScroll));
    } else {
        return (MldPrechkTotal(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key
*===========================================================================
*/
SHORT MldPrechkTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[3][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[3][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;             /* number of lines to be printed */

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set mnemonic and number -- */
    usScrollLine += MldNumber(aszScrollBuff[0], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

    /* -- set total line -- */   
    usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lMI, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key
*===========================================================================
*/
SHORT MldDriveTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[4][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[4][MLD_PRECOLUMN1 + 1];                /* reverse video control flag area */
    USHORT  usScrollLine = 0;             /* number of lines to be printed */

    /* initialize the buffer */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    /* -- set mnemonic and number -- */
    usScrollLine += MldNumber(aszScrollBuff[0], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set total line -- */   
    usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lMI, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/