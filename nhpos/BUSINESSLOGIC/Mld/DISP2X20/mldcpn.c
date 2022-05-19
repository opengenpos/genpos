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
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
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

#if defined(POSSIBLE_DEAD_CODE)

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
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <rfl.h>
#include <display.h>
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
        return (MLD_NOT_DISPLAY);
    }

    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:

        sStatus = MldCoupon(pItem, usType, usScroll);
        break;

    default:

        sStatus = MLD_NOT_DISPLAY;
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
    REGDISPMSG      Display;
    TRANMODEQUAL    WorkMode;
    TRANCURQUAL     WorkCur;
    
    memset(&Display, 0, sizeof(REGDISPMSG));
    Display.uchMajorClass = CLASS_UIFREGCOUPON;
    Display.uchMinorClass = CLASS_UICOMCOUPON;
    _tcsncpy(Display.aszMnemonic, pItem->aszMnemonic, /*sizeof(pItem->aszMnemonic)*/NUM_DEPTPLU);
    //memcpy(Display.aszMnemonic, pItem->aszMnemonic, sizeof(pItem->aszMnemonic));
    Display.lAmount = pItem->lAmount;
    Display.uchFsc = pItem->uchSeatNo;

    flDispRegDescrControl |= ITEMDISC_CNTRL;
    if (pItem->fbModifier & VOID_MODIFIER) {
        flDispRegDescrControl |= VOID_CNTRL;
    }
    if (pItem->fbModifier & TAX_MOD_MASK) {
        flDispRegDescrControl |= TAXMOD_CNTRL;
    }

    TrnGetModeQual(&WorkMode);
    TrnGetCurQual(&WorkCur);

    /*--- Set Descriptor ---*/
    if ((WorkCur.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {
        flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL |
                                  TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    } else {
        flDispRegKeepControl &= ~(VOID_CNTRL | ITEMDISC_CNTRL |
                                  TAXMOD_CNTRL | TAXEXMPT_CNTRL | SUBTTL_CNTRL);
    }

    /* set scroll symbol */
    Display.uchArrow = MldSetScrollSymbol(usType);

    Display.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&Display);

    /*--- Set Descriptor ---*/
    if ((WorkCur.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER) {
        flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                   TAXMOD_CNTRL | TOTAL_CNTRL | SUBTTL_CNTRL);
    } else {
        flDispRegDescrControl &= ~(CRED_CNTRL | VOID_CNTRL | ITEMDISC_CNTRL |
                                   TAXMOD_CNTRL | TAXEXMPT_CNTRL | TOTAL_CNTRL |
                                   SUBTTL_CNTRL);
    }

    return(MLD_SUCCESS);

    usScroll = usScroll;
}
#endif
/* ===== End of File ===== */
