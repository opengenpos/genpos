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
* Title       : Multiline Display Coupon Item
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDCPN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldItemCoupon() : main procedure for display coupon item
*               MldCoupon()     : display coupon scroll display
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-02-95 : 03.00.00 : M.Ozawa    : Initial
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
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtItemCoupon(ITEMCOUPON *pItem ,USHORT usType);
*
*   Input  : ITEMCOUPON         *pItem  -   Coupon Item Data address
*            USHORT             usType  -   First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function is main procedure for display coupon item.
*===========================================================================
*/
SHORT  MldItemCoupon(ITEMCOUPON *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    if (pItem->fbReduceStatus & REDUCE_ITEM) {
        return (MldDummyScrollDisplay(usType, usScroll));
    }

    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:

        sStatus = MldCoupon(pItem, usType, usScroll);
        break;

    default:

        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;
    }

    return (sStatus);
}

/*
*===========================================================================
** Format  : VOID MldCoupon(ITEMCOUPON *pItem, USHORT usType)
*
*    Input : ITEMCOUPON         *pItem  - coupon data address
*            USHORT             usType  - First Display Position
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays coupon item
*===========================================================================
*/
SHORT MldCoupon(ITEMCOUPON *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {

        return (MldDriveCoupon(pItem, usType, usScroll));

    } else {

        return (MldPrechkCoupon(pItem, usType, usScroll));

    }
}

/*
*===========================================================================
** Format  : VOID MldCoupon(ITEMCOUPON *pItem, USHORT usType)
*
*    Input : ITEMCOUPON         *pItem  - coupon data address
*            USHORT             usType  - First Display Position
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays coupon item
*===========================================================================
*/
SHORT MldPrechkCoupon(ITEMCOUPON *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[5][MLD_PRECOLUMN1 + 1 ]; /* print data save area */
    LONG   alAttrib[5][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;         /* number of lines to be printed */
/*    TRANMODEQUAL TranModeQual; */

/*    TrnGetModeQual(&TranModeQual); */

    /* -- set void mnemonic and number -- */

    memset( aszScrollBuff[ 0 ], 0x00, sizeof( aszScrollBuff ));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    usScrollLine += MldNumber( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

    /* -- set tax modifier mnemonic -- */

    usScrollLine += MldTaxMod( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    /* -- set coupon mnemonic, and its amount --- */
    usScrollLine += MldCpn( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->uchSeatNo, pItem->aszMnemonic, pItem->lAmount, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine,  MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID MldCoupon(ITEMCOUPON *pItem, USHORT usType)
*
*    Input : ITEMCOUPON         *pItem  - coupon data address
*            USHORT             usType  - First Display Position
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays coupon item
*===========================================================================
*/
SHORT MldDriveCoupon(ITEMCOUPON *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[8][MLD_DR3COLUMN + 1 ]; /* print data save area */
    LONG   alAttrib[8][MLD_PRECOLUMN1 + 1];                 /* reverse video control flag area */
    USHORT  usScrollLine = 0;         /* number of lines to be printed */
/*    TRANMODEQUAL TranModeQual; */

/*    TrnGetModeQual(&TranModeQual); */

    /* -- set void mnemonic and number -- */

    memset( aszScrollBuff[ 0 ], 0x00, sizeof( aszScrollBuff ));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    usScrollLine += MldNumber( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set tax modifier mnemonic -- */

    usScrollLine += MldTaxMod( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set seat no -- */
    if ((uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM) ||
        (usScroll == MLD_SCROLL_1)) {

        usScrollLine += MldSeatNo(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->uchSeatNo, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set coupon mnemonic, and its amount --- */
    usScrollLine += MldCpn( aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->aszMnemonic, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/* ===== End of File ===== */
