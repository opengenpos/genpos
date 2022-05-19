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
* Title       : Multiline Display  Reorder
* Category    : Multiline, NCR 2170 US Hospitarity Application
* Program Name: MLDIMOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   MldItemModifier()    : display modifier key operation
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-14-95 : 03.00.00 : M.Ozawa    : Initial
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
*/

/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemModifier( ITEMMODIFIER *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMMODIFIER    *pItem     -   Modifiered Data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays modifier key operation
*===========================================================================
*/
SHORT MldItemModifier( ITEMMODIFIER *pItem , USHORT usType, USHORT usScroll)
{

    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {

        return (MldPrechkModifier(pItem, usType, usScroll));

    } else if ((uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) &&
               (uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM)) {    /* not 3 scroll */

        return (MldPrechkModifier(pItem, usType, usScroll));

    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : VOID   MldItemModifier( ITEMMODIFIER *pItem, USHORT usType,
*                               USHORT usScroll);
*
*   Input  : ITEMMODIFIER    *pItem     -   Modifiered data
*            USHORT          usType     -   First display position
*            USHORT          usScroll   -   Destination scroll display
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays modifier key operation
*===========================================================================
*/
SHORT MldPrechkModifier( ITEMMODIFIER *pItem , USHORT usType, USHORT usScroll)
{
    ITEMTRANSOPEN   ItemTransOpen;

    ItemTransOpen.uchMinorClass = 0;

    /* display guest no on display#2 */
    switch (pItem->uchMinorClass) {
    case CLASS_TABLENO:
        ItemTransOpen.usTableNo = pItem->usTableNo;
        ItemTransOpen.usNoPerson = pItem->usNoPerson;   /* for Unique No. display */
        ItemTransOpen.uchSeatNo = 0;
        break;

    case CLASS_NOPERSON:
        ItemTransOpen.usNoPerson = pItem->usNoPerson;
        ItemTransOpen.usTableNo = 0;
        ItemTransOpen.uchSeatNo = 0;
        break;

    case CLASS_SEATNO:
        ItemTransOpen.uchSeatNo = pItem->uchLineNo;
        ItemTransOpen.usNoPerson = 0;
        ItemTransOpen.usTableNo = 0;
        if (pItem->uchLineNo == 0) {
            MldScrollClear(MLD_SCROLL_2);   /* clear seat no */
        }
        break;

    default:
        return (MLD_SUCCESS);
    }

    MldWriteGuestNo(&ItemTransOpen);

    return (MLD_SUCCESS);

    usType = usType;
    usScroll = usScroll;
}

/***** End Of Source *****/
