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
* Title       : Multiline Display Item  misc
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDMISC.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      MldItemMisc() : print items specified class "CLASS_ITEMMISC"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-02-95 : 03.00.00 : M.Ozawa    : Initial
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
/**------- 2170 local------**/
#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "display.h"
#include "mld.h"
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
*===========================================================================
** Format  : VOID   MldItemMisc(ITEMMISC *pItem, USHORT usType);
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays item classfied "CLASS_ITEMMISC"
*===========================================================================
*/
SHORT   MldItemMisc(ITEMMISC  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {

    case CLASS_CANCEL:                  /* cancel */
        sStatus = MldCancel(pItem, usType, usScroll);
        break;

    case CLASS_MONEY:                   /* Money,   Saratoga */
        sStatus = MldMoney(pItem, usType, usScroll);
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;
    }

    return (sStatus);
}

/*
*===========================================================================
** Format  : VOID  MldMoney(ITEMMISC *pItem, USHORT usType, USHORT usScroll)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation              Saratoga
*===========================================================================
*/
SHORT   MldMoney(ITEMMISC *pItem, USHORT usType, USHORT usScroll)
{
    SHORT   sStatus;

    switch (pItem->uchTendMinor) {
    case CLASS_TENDER1:
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:
    case CLASS_TENDER10:
    case CLASS_TENDER11:
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
        sStatus = MldMoneyTender(pItem, usType, usScroll);
        break;

    case CLASS_FOREIGN1:
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        sStatus = MldMoneyForeign(pItem, usType, usScroll);
        break;

    default:
        sStatus = MldDummyScrollDisplay(usType, usScroll);
        break;
    }

    return(sStatus);
}

/*
*===========================================================================
** Format  : VOID  MldTender(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays tender operation            Saratoga
*===========================================================================
*/
SHORT   MldMoneyTender(ITEMMISC *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveMoney(pItem, usType, usScroll));
    } else {
        return (MldPrechkMoney(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID   MldPrechkMoney(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays tender operation            Saratoga
*===========================================================================
*/
SHORT   MldPrechkMoney(ITEMMISC *pItemMisc, USHORT usType, USHORT usScroll)
{
	TCHAR   aszScrollBuff[15][MLD_PRECOLUMN1 + 1] = {0};
    LONG    alAttrib[15][MLD_PRECOLUMN1 + 1] = {0};
    USHORT  usScrollLine = 0;
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItemMisc->uchTendMinor, &usTranAddr, &uchSymAddr);

    if ((pItemMisc->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->fbModifier, MLD_PRECHECK_SYSTEM);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->aszNumber, MLD_PRECHECK_SYSTEM);
	usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], usTranAddr, pItemMisc->lAmount, MLD_PRECHECK_SYSTEM);

    if (! CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) {
        memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));
    }

    /* -- set cursor position & display all data in the buffer -- */
    return(MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID   MldDriveMoney(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function displays tender operation            Saratoga
*===========================================================================
*/
SHORT   MldDriveMoney(ITEMMISC *pItemMisc, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[18][MLD_DR3COLUMN + 1] = {0};
    LONG    alAttrib[18][MLD_PRECOLUMN1 + 1] = {0};
    USHORT  usScrollLine = 0;
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItemMisc->uchTendMinor, &usTranAddr, &uchSymAddr);

    if ((pItemMisc->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);
    usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], usTranAddr, pItemMisc->lAmount, MLD_DRIVE_THROUGH_3SCROLL);
                                                    
    /* -- set cursor position & display all data in the buffer -- */
    return(MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID   MldMoneyForeign(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation
*===========================================================================
*/
SHORT   MldMoneyForeign(ITEMMISC *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveMoneyForeign(pItem, usType, usScroll));
    } else {
        return (MldPrechkMoneyForeign(pItem, usType, usScroll));
    }
}

/*
*===========================================================================
** Format  : VOID   MldPrechkMoneyForeign(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation        Saratoga
*===========================================================================
*/
SHORT   MldPrechkMoneyForeign(ITEMMISC  *pItemMisc, USHORT usType, USHORT usScroll)
{
	TCHAR   aszScrollBuff[8][MLD_PRECOLUMN1 + 1] = {0};
    LONG    alAttrib[8][MLD_PRECOLUMN1 + 1] = {0};
    USHORT  usScrollLine = 0;
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItemMisc->uchTendMinor, &usTranAddr, &uchSymAddr);

    if ((pItemMisc->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->fbModifier, MLD_PRECHECK_SYSTEM);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->aszNumber, MLD_PRECHECK_SYSTEM);
    usScrollLine += MldForeign(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->lAmount, uchSymAddr, pItemMisc->fchStatus, 0L, 0, MLD_PRECHECK_SYSTEM);
    usScrollLine--;                                                       /* Not display FC rate */
    /* -- set cursor position & display all data in the buffer -- */
    return(MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID   MldDriveMoneyForeign(ITEMMISC *pItem, USHORT usType)
*
*   Input  : ITEMMISC         *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays foreign tender operation        Saratoga
*===========================================================================
*/
SHORT   MldDriveMoneyForeign(ITEMMISC *pItemMisc, USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[10][MLD_DR3COLUMN + 1] = {0};
	LONG    alAttrib[10][MLD_PRECOLUMN1 + 1] = {0};
    USHORT  usScrollLine = 0;
	USHORT  usTranAddr;
	UCHAR   uchSymAddr;

    /* get foreign 1 or 2 mnemonic addresses */
	MldGetMoneyMnem(pItemMisc->uchTendMinor, &usTranAddr, &uchSymAddr);

    if ((pItemMisc->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);
    usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);
    usScrollLine += MldForeign(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItemMisc->lAmount, uchSymAddr, pItemMisc->fchStatus, 0L, 0, MLD_DRIVE_THROUGH_3SCROLL);

    /* -- set cursor position & display all data in the buffer -- */
    return(MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}

/***** End Of Source *****/