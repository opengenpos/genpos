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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Multiline Display Charge Tips                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDTIP.c                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldChrgTip() : prints charge tips
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-02-95 : 03.00.00 : M.Ozawa    : Initial
* Aug-11-99 : 03.05.00 :  M.Teraki  : Remove WAITER_MODEL
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
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <mld.h>
#include <rfl.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s 
;========================================================================
**/

/*
*===========================================================================
** Format  : ULONG  PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : USHORT            usID      -waiter ID or '0' 
*            
** Synopsis: This function prints charge tips
*===========================================================================
*/
static ULONG  MldChrgTipID(ITEMDISC *pItem, USHORT usScroll)
{
    if (RflGetSystemType() == FLEX_STORE_RECALL) {
        return (0); /* not display server no in a Store Recall system */
    }

    if (usScroll == MLD_SCROLL_1) {
		TRANINFORMATION    *WorkTranInf;

        TrnGetTranInformation(&WorkTranInf);
        if (WorkTranInf->TranGCFQual.ulCashierID == WorkTranInf->TranModeQual.ulCashierID) {
            MldSetSpecialStatus(usScroll, MLD_NEWCHEK_CASHIER);
            return (0); /* not display server no */
        }
    }

    return (pItem->ulID);   /* display server/previous cashier no */
}

/*
*===========================================================================
** Format  : VOID  MldChrgTip(ITEMDISC *pItem, USHORT usType);      
*
*   Input  : ITEMDISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays charge tips
*===========================================================================
*/
static SHORT MldPrechkChrgTip(ITEMDISC *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[6][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG   alAttrib[6][MLD_PRECOLUMN1 + 1] = {0};                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if ((pItem->fbDiscModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbDiscModifier, MLD_PRECHECK_SYSTEM);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_PRECHECK_SYSTEM);

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbDiscModifier, MLD_PRECHECK_SYSTEM);

    /* -- set server id -- */
    usScrollLine += MldChargeTips(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], MldChrgTipID(pItem, usScroll), MLD_PRECHECK_SYSTEM);

    /* -- set discount mnemonic, rate, and amount -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->uchSeatNo, RflChkDiscAdr(pItem), pItem->uchRate, pItem->lAmount, MLD_PRECHECK_SYSTEM);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID  MldChrgTip(ITEMDISC *pItem, USHORT usType);      
*
*   Input  : ITEMDISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays charge tips
*===========================================================================
*/
static SHORT MldDriveChrgTip(ITEMDISC *pItem, USHORT usType, USHORT usScroll)
{
    TCHAR  aszScrollBuff[8][MLD_DR3COLUMN + 1] = {0}; /* print data save area */
    LONG   alAttrib[8][MLD_PRECOLUMN1 + 1] = {0};                /* reverse video control flag area */
    USHORT  usScrollLine = 0;            /* number of lines to be printed */

    if ((pItem->fbDiscModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbDiscModifier, MLD_DRIVE_THROUGH_3SCROLL);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set tax modifier mnemonic -- */
    usScrollLine += MldTaxMod(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, pItem->fbDiscModifier, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set seat no -- */
    if ((uchMldSystem2 != MLD_SPLIT_CHECK_SYSTEM) || (usScroll == MLD_SCROLL_1)) {
        usScrollLine += MldSeatNo(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->uchSeatNo, MLD_DRIVE_THROUGH_3SCROLL);
    }

    /* -- set server id -- */
    usScrollLine += MldChargeTips(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set discount mnemonic, rate, and amount -- */
    usScrollLine += MldDiscount(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], 0, RflChkDiscAdr(pItem), pItem->uchRate, pItem->lAmount, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID  MldChrgTip(ITEMDISC *pItem, USHORT usType);      
*
*   Input  : ITEMDISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays charge tips
*===========================================================================
*/
SHORT MldChrgTip(ITEMDISC *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveChrgTip(pItem, usType, usScroll));
    } else {
        return (MldPrechkChrgTip(pItem, usType, usScroll));
    }
}
/***** End Of Source *****/
