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
* Title       : Multiline Display Transaction Cancel                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDCNCL.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*      
*      MldCancel()    : display transaction cancel
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays transaction cancel.
*===========================================================================
*/
SHORT  MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveCancel(pItem, usType, usScroll));

    } else {
                                                
        return (MldPrechkCancel(pItem, usType, usScroll));

    }
}

/*
*===========================================================================
** Format  : VOID MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays transaction cancel.
*===========================================================================
*/
SHORT  MldPrechkCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[2][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[2][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */


    /* initialize the buffer */
    memset(aszScrollBuff, '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    usScrollLine += MldNumber(aszScrollBuff[0], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

    /* -- set cancel mnemonic -- */
    usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CANCEL_ADR, pItem->lAmount, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays transaction cancel.
*===========================================================================
*/
SHORT MldDriveCancel(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[4][MLD_DR3COLUMN + 1]; /* print data save area */
    LONG   alAttrib[4][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */


    /* initialize the buffer */
    memset(aszScrollBuff, '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));

    usScrollLine += MldNumber(aszScrollBuff[0], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cancel mnemonic -- */
    usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_CANCEL_ADR, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/
