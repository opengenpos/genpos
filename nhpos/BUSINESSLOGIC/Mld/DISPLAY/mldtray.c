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
* Program Name: MLDTRAY.C                                                      
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldTray() : prints total key (Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-13-95 : 03.00.00 : M.Ozawa    : Initial
* Dec-14-15 : 02.02.01 : R.Chambers : allow Cursor Void with Tray Total, fix display issue.
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
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <mld.h>
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
** Format  : VOID MldTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
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
SHORT MldTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {
        return (MldPrechkTrayTotal(pItem, usType, usScroll));
    }

	// for Tray Total operations in which a transaction is broken up into individual trays each
	// with its own total, we need to address an item selection problem in which the list of
	// displayed items in the MLD functionality does not indicate all of the items in the actual
	// transaction data file. the result is that doing a Cursor Void can result in the wrong
	// item being selected.
	// so in function MldTrayTotal() where function MldDummyScrollDisplay() is used to do a dummy
	// write to the receipt window we need to add an indicator so that the dummy item is actually
	// accounted for even if not displayed so that Cursor Void will work correctly.
	return (MldDummyScrollDisplay(usType, usScroll | MLD_SCROLL_ADDED_FILE));
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
SHORT MldPrechkTrayTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR   aszScrollBuff[5][MLD_PRECOLUMN1 + 1] = {0};  /* print data save area */
    LONG    alAttrib[5][MLD_PRECOLUMN1 + 1] = {0};       /* reverse video control flag area */
    USHORT  usScrollLine = 0;             /* number of lines to be printed */

    /* -- set mnemonic and number -- */
    usScrollLine += MldNumber(aszScrollBuff[0], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

    /* -- set total line -- */   
    usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTtlAdr(pItem), pItem->lMI, MLD_PRECHECK_SYSTEM);

    /* -- set counter line -- */   
    usScrollLine += MldTrayCount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_TRAYCO_ADR, pItem->sTrayCo, MLD_PRECHECK_SYSTEM);

    /* -- 1 line feed -- */
    tcharnset(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], _T(' '), MLD_PRECOLUMN1);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/***** End Of Source *****/
