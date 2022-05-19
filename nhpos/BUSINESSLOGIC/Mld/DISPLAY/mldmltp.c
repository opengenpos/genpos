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
* Title       : Multiline Display  MultiCheck                
* Category    : Multiline Display, NCR 2170  US Hospitarity Application
* Program Name: MLDMLPT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      MldMultiPostTotal() : display items specified minor class "CLASS_MULTIPOSTTOTAL"
*      MldMultiPost() : display items specified minor class "CLASS_MULTIPOSTCHECK"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 : M.Ozawa    : Initial
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

/*
============================================================================
+                        I N C L U D E   F I L E s
============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <dfl.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID   MldMultiPostTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*            USHORT            usType     -First Display Position
*            USHORT            usScroll   -Destination Display
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays multi check.
*===========================================================================
*/
SHORT MldMultiPostTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll)
{
    if ((uchMldSystem == MLD_PRECHECK_SYSTEM) ||
        (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {
        return (MldPrechkMultiPostTotal(pItem, usType, usScroll));
    }

    return (MldDummyScrollDisplay(usType, usScroll));
}

/*
*===========================================================================
** Format  : VOID   MldMultiPostTotal(ITEMMULTI *pItem, USHORT usType,
*                                       USHORT usScroll);
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*            USHORT            usType     -First Display Position
*            USHORT            usScroll   -Destination Display
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays multi check.
*===========================================================================
*/
SHORT MldPrechkMultiPostTotal(ITEMMULTI *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[3][MLD_PRECOLUMN1 + 1]; /* print data save area */
    LONG   alAttrib[3][MLD_PRECOLUMN1 + 1];                  /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));
    memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));
    
    /* -- set check paid mnemonic and guest check No. -- */
    usScrollLine += MldGstChkNo(aszScrollBuff[0], pItem, MLD_PRECHECK_SYSTEM);

    /* -- set check total mnemonic and amount -- */
    usScrollLine += MldDoubleMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_AMTTL_ADR, pItem->lMI, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/***** End Of Source *****/

