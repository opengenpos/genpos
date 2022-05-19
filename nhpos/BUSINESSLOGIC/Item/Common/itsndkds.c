/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ItemSendKds() Module
* Category    : Item Common Module, NCR 2170 US Hsopitality Model Application
* Program Name: ITSNDKDS.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: ItemSendKds()
*  SHORT   ItemSendKds(VOID *pItem, USHORT fsEC);
*  SHORT   ItemSendKdsSales(ITEMSALES *pItem, USHORT fsEC);
*  SHORT   ItemSendKdsOrderDeclaration(ITEMSALES *pItem);
*  USHORT  ItemSendKdsItemSetLineNumber(ITEMSALES *pItem);
*
* The primary entry point for the functionality is in this file is the function
* SHORT   ItemSendKds( VOID *pItem, USHORT fsEC) located at the bottom of the file.
*
* Most of the functions in this file are marked as static to eliminate global visibility
* though a couple are used directly for some reason.
*
*     This functionality is used with the Select Electronics kitchen display system
*     using the Oasys iPAD with bump bar and the ROD Gold software server interface.
*     There are also some installations that use this as a data stream to combine
*     with surveillance video footage in order to show the cashier as well as the
*     point of sale transaction actions of the cashier.
*
*     In a recent conversation with Select Electronics the Oasys iPAD with bump bar
*     is an older technology that has been discontinued in favor of using the
*     kitchen printer output directly instead.  However there are still customers
*     using this technology.  Richard Chambers, Mar-24-2015
*
* NOTE: this is the old binary KDS protocol which probably should be eliminated
*       however there may be some applications such as cashier monitoring which
*       is using the old binary KDS protocol to capture transaction data
*       alongside of video. Dan Parliman made such an arrangement with a third
*       party supplier before his retirement from NCR sometime around 2012 or so.
*           Richard Chambers, Jul-07-2021
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver. Rev. :   Name      :  Description
* Dec-13-99: 00.00.01  : M.Ozawa     : initial by NCR
* Mar-23-15: 02.02.01  : R.Chambers  : source cleanup, elimination of compiler warnings.
* Jul-07-21: 02.04.00  : R.Chambers  : using KDS_LONG_CAST() for 64 bit totals conversion.
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
#include	<tchar.h>
#include    <memory.h>
#include    <stdlib.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "uie.h"
#include    "rfl.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "mld.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "cpm.h"
#include    "eept.h"
#include    "prt.h"
#include    "enhkds.h"
#include    "itmlocal.h"

/* main item definition */
#define MAIN_ITEM   1
#define MAIN_DISC   2
#define MAIN_TOTAL  3
#define MAIN_TAX    4
#define MAIN_TENDER 5
#define MAIN_CANCEL 6
#define MAIN_SINGLE 7
#define MAIN_MODIFIER   8
#define MAIN_OTHER  9

/* sub item definition for sales */
#define SUB_PLU			1
#define SUB_COND		2
#define SUB_PRTMOD		3
#define SUB_ADJ			4
#define SUB_WEIGHT		5
#define SUB_DEPT		6
#define SUB_PREV_COND	7
#define SUB_ORDER_DEC	8

/* sub item definition for discount/coupon */
#define SUB_ITEMDISC 1
#define SUB_TRANDISC 2
#define SUB_COUPON   3
#define SUB_PPIDISC  4
#define SUB_CHGTIPS  5
#define SUB_UPCCOUPON 6


/* sub item definition for tender */
#define NORMAL_TENDER  1
#define FOREIGN_TENDER 2

/* sub item definition for single transaction */
#define SUB_NOSALE 1
#define SUB_PO     2
#define SUB_ROA    3

/* sub item definition for modifier transactioon */
#define SUB_TRANSNO 1
#define SUB_NOPERSON 2
#define SUB_TABLENO 3
#define SUB_TAXMOD 4
#define SUB_SEATNO 5

/* sub item definition for other */
#define SUB_GCNO 1
#define SUB_SRNO 2
#define SUB_CUSTNAME 3
#define SUB_AGELOG 4
#define SUB_NUMBER 5
#define SUB_VOID 6
#define SUB_EC 7
#define SUB_ANY 9

static KDSSENDDATA ItemKdsSendData;
static USHORT      usItemKdsPrvLine = 0;    // it appears this is never updated (update commented out in ItemSendKdsSales()) and is always zero.

static DCURRENCY lCurTransTax;          /* transaction tax save area */
static DCURRENCY lPrevModDiscAmount;    /* modifierd item discount amount save area */
static DCURRENCY lParentAmount;

typedef struct {
	USHORT usLineNumber;
	USHORT usUniqueID;
}KDSLINENUM;
static KDSLINENUM	usLineNumberContainer[256];

#if defined(DCURRENCY_LONGLONG)
#define KDS_LONG_CAST(x) (LONG)(x)
#else
#define KDS_LONG_CAST(x) (x)
#endif

//  Utility functions used below for various operations.

static SHORT ItemSendKdsSetTotalType(CONST ITEMTOTAL *pItem)
{
    USHORT usType;

    if (pItem->uchMinorClass == CLASS_TOTAL1) {
        return 0;
    } else if (pItem->uchMinorClass == CLASS_TOTAL2) {
        return PRT_NOFINALIZE;
    } else {
        usType = (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE);
        if (usType == PRT_CASINT1) {
            usType = PRT_SERVICE1;
        } else if (usType == PRT_CASINT2) {
            usType = PRT_SERVICE2;
        }
    }

    return usType;
}

#define NORMAL      0
#define TRAINING    1
#define TRANVOID    3    // KDS message indicates CURQUAL_PVOID
#define TRANRETURN  4    // KDS message indicates CURQUAL_TRETURN

static UCHAR ItemSendKdsSetTransMode1(VOID)
{
    USHORT sReturn = NORMAL;

    if ((TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) || (TranModeQualPtr->fbWaiterStatus & MODEQUAL_WAITER_TRAINING)) {
        sReturn += TRAINING;
    }
    if (TranCurQualPtr->fsCurStatus & CURQUAL_PVOID) { /* preselect void trans. */
        sReturn += TRANVOID;
    }
    if (TranCurQualPtr->fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) { /* transaction return */
        sReturn += TRANRETURN;
    }

    /* transaction void, training status */
    return (UCHAR)sReturn;
}

#define NEWSALES    0
#define ADDON       1
#define FINALIZE    2
#define SINGLE      3
#define CLOSE       9

static UCHAR ItemSendKdsSetTransMode2(CONST VOID *pVoid)
{
    CONST ITEMSALES    * const pItem = pVoid;
    CONST ITEMTENDER   * const pItemTender = pVoid;
    SHORT        sReturn = 0;

    switch (pItem->uchMajorClass) {
    case CLASS_ITEMTRANSOPEN:

        switch(pItem->uchMinorClass) {
        case CLASS_CASHIER:
        case CLASS_NEWCHECK:
        case CLASS_CASINTOPEN:
            sReturn = NEWSALES;
            break;
        case CLASS_REORDER:
        case CLASS_ADDCHECK:
        case CLASS_OPENPOSTCHECK:
        case CLASS_CASINTRECALL:
        case CLASS_MANUALPB:
        case CLASS_OPENSEAT:
            sReturn = ADDON;
            break;

        case CLASS_STORE_RECALL:
        case CLASS_STORE_RECALL_DELIV:
            if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) ||
                ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)) {
                sReturn = ADDON;
            } else {
                sReturn = NEWSALES;
            }
            break;

        case CLASS_OPENNOSALE:
        case CLASS_OPENPO:
        case CLASS_OPENRA:
        case CLASS_OPENTIPSPO:
        case CLASS_OPENTIPSDECLARED:
        case CLASS_OPENCHECKTRANSFER:
        case CLASS_OPENMONEY:
            sReturn = SINGLE;
            break;
        }
        break;

    case CLASS_ITEMSALES:
    case CLASS_ITEMDISC:
    case CLASS_ITEMCOUPON:
    case CLASS_ITEMPRINT:
    case CLASS_ITEMMULTI:
    case CLASS_ITEMAFFECTION:
        if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) ||
            ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)) {
            sReturn = ADDON;
        } else {
            sReturn = NEWSALES;
        }
        break;
    case CLASS_ITEMTOTAL:
        if ((sReturn = ItemSendKdsSetTotalType(pVoid)) == PRT_FINALIZE1 || sReturn == PRT_FINALIZE2) {
            sReturn = FINALIZE;
        } else {
            if (((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_REORDER) ||
                ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)) {
                sReturn = ADDON;
            } else {
                sReturn = NEWSALES;
            }
        }
        break;

    case CLASS_ITEMTENDER:
        if (pItemTender->lBalanceDue ) {
            sReturn = ADDON;
        } else {
            sReturn = FINALIZE;
        }
        break;

    case CLASS_ITEMTRANSCLOSE:
        sReturn = CLOSE;
        break;

    case CLASS_ITEMMISC:
        sReturn = SINGLE;
        break;
    }

    /* new item, add-on, etc status */
    return (UCHAR)sReturn;
}

static ULONG ItemSendKdsSetOpeId(VOID)
{
    /* if waiter id set then return that other wise return operator id */
    if (TranModeQualPtr->ulWaiterID) {
        return TranModeQualPtr->ulWaiterID;
    }
    return  TranModeQualPtr->ulCashierID;

}

static LONG ItemSendKdsSetTotal(VOID)
{
    /* transaction total */
    return KDS_LONG_CAST(TranItemizersPtr->lMI + ItemTransLocalPtr->lWorkMI);
}

static USHORT ItemSendKdsSetForm(VOID *pVoid, TCHAR *puchBuffer)
{
    TRANINFORMATION *pTrnInfo = TrnGetTranInformationPointer();
    USHORT usReturn;

    /* ascii format of each item */
    usReturn = PrtDispItemForm(pTrnInfo, pVoid, puchBuffer);    /* call display on the fly manager */
    return usReturn;
}

static UCHAR  ItemSendKdsGetNo(UCHAR *puchCRTNo)
{
	// convert a binary hex value in a nibble to an ASCII character
	// representing the hex digit for the hex value.
    if ( *puchCRTNo <= 9 ) {
        *puchCRTNo += 0x30;  // convert 0 - 9 to ASCII '0' to '9'
    } else {
        *puchCRTNo += 0x37;  // convert 0x0A - 0x0F to ASCII 'A' to 'F'
    }

	return *puchCRTNo;
}

/*
*===========================================================================
**Synopsis:  VOID ItemSendKdsSetCRTNo(VOID *pItem, UCHAR uchCRTNo[4])
*
*    Input: VOID   *pItem       - pointer to ITEMSALES, ITEMTOTAL, etc.
*
*   Output: UCHAR  uchCRTNo[4]  - array of CRT numbers from KDSSENDDATA
*
*    InOut: None
*
**Return:   None
*
** Description: Using the item data provided then calculate CRT numbers for
*               the display of the item based on the provisioning information.
*
*               Currently it appears that the KDS message can specify up to four
*               CRT positions however only two CRT positions are in use, one for CRT
*               numbers 1, 2, 3, and 4 and the second one for CRT number 5, 6, 7, and 8.
*               The actual CRT position value is a bit map in which the least significant
*               bits of the least significant nibble are used to indicate which of four
*               CRTs in a particular position are to be used to display the item.
*               With multiple CRTs, the bits are bitwise ored together.
*
*               For CRT #1 the bit mask would be 0x01 in uchCRTNo[0].
*               For CRT #2 the bit mask would be 0x02 in uchCRTNo[0].
*               For CRT #3 the bit mask would be 0x04 in uchCRTNo[0].
*               For CRT #4 the bit mask would be 0x08 in uchCRTNo[0].
*
*               For CRT #5 through #8, a similar bit map would be used for uchCRTNo[1].
*               uchCRTNo[2] and uchCRTNo[3] are unused and reserved and should be set to 0.
*
*               When sent to the Kitchen Display System display manager, each of the nibble
*               values is turned into a character representing the hex value meaning that
*               the result will be a character of '0', '1', ..., '9', 'A', ..., 'F'
*===========================================================================
*/
static VOID   ItemSendKdsSetCRTNo(CONST VOID *pItem, UCHAR uchCRTNo[KDS_ROUTE_LEN])
{
    CONST ITEMSALES   * const pItemSales = pItem;
	CONST ITEMTOTAL   * const pItemTotal = pItem;
	int         iLoop;

	for (iLoop = 0; iLoop < KDS_ROUTE_LEN; iLoop++) uchCRTNo[iLoop] = 0;

    if (pItemSales->uchMajorClass == CLASS_ITEMSALES) {
        uchCRTNo[0] = (UCHAR)((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) >> 4);   /* CRT No1 - No4 */
        uchCRTNo[1] = (UCHAR)(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8);          /* CRT No5 - No 8 */

        if (pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) {
            /* with noun CRT */
			ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
            uchCRTNo[0] |= (UCHAR)((pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) >> 4);
            uchCRTNo[1] |= (UCHAR)(pCommonLocalRcvBuff->ItemSales.ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8);
        }

        if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
            (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
            (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

            uchCRTNo[0] = 0;
            uchCRTNo[1] = 0;
        }
    } else if (pItemTotal->uchMajorClass == CLASS_ITEMTOTAL) {
        uchCRTNo[0] = (UCHAR) ITM_TTL_FLAG_CRT_1_4(pItemTotal);   /* CRT No1 - No4 */
        uchCRTNo[1] = (UCHAR) ITM_TTL_FLAG_CRT_5_8(pItemTotal);   /* CRT No5 - No8 */
    }

    /* convert the hex digits of the bit mask representing CRTs to ASCII code */
	for (iLoop = 0; iLoop < KDS_ROUTE_LEN; iLoop++) ItemSendKdsGetNo(uchCRTNo + iLoop);
}

static USHORT ItemSendKdsItemGetLineNumber(VOID *pData)
{
	ITEMSALES *pItem = pData;

    switch(pItem->uchMajorClass) {
		case CLASS_ITEMSALES:
			if(pItem->usUniqueID)
			{
				USHORT i;
				if (pItem->usKdsLineNo != 0) {
					if (TrnInformation.TranGCFQual.usItemKdsCurLine < pItem->usKdsLineNo)
						TrnInformation.TranGCFQual.usItemKdsCurLine = pItem->usKdsLineNo;
					return pItem->usKdsLineNo;
				}

				for( i = 0; i < 256 && usLineNumberContainer[i].usLineNumber != 0; i++)
				{
					if(pItem->usUniqueID == usLineNumberContainer[i].usUniqueID)
					{
						return (pItem->usKdsLineNo = usLineNumberContainer[i].usLineNumber);
					}
				}
			}
			return (pItem->usKdsLineNo = ItemSendKdsItemSetLineNumber(pItem));

		default:
			return ++(TrnInformation.TranGCFQual.usItemKdsCurLine);
	}
}

//  End of utility functions.

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsModifier( ITEMMODIFIER *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsModifier(ITEMMODIFIER *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_MODIFIER;
    switch(pItem->uchMinorClass) {
    case CLASS_TABLENO:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_MODIFIER;

        if (CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT0)) {
            ItemKdsSendData.usSubType1 = SUB_TRANSNO;
        } else {
            ItemKdsSendData.usSubType1 = SUB_TABLENO;
        }
        ItemKdsSendData.ulSubType2 = pItem->usTableNo;
        break;

    case CLASS_NOPERSON:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_MODIFIER;
        ItemKdsSendData.usSubType1 = SUB_NOPERSON;
        ItemKdsSendData.ulSubType2 = pItem->usNoPerson;
        break;

    case CLASS_SEATNO:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_MODIFIER;
        ItemKdsSendData.usSubType1 = SUB_SEATNO;
        ItemKdsSendData.ulSubType2 = pItem->uchLineNo;
        break;

    case CLASS_ALPHANAME:
        /* current line no in the transaction */
        ItemKdsSendData.usLineMainType = MAIN_OTHER;
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_OTHER;
        ItemKdsSendData.usSubType1 = SUB_CUSTNAME;
//CHANGE FOR WIDE CONVERSION ?
        memcpy(ItemKdsSendData.auchSubType2, pItem->aszName, _tcslen(pItem->aszName));
        break;

    case CLASS_TAXMODIFIER:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);   /* not increment current line */
#if 0
		// RJC use the standard line number generator
        ItemKdsSendData.usLineNum = usItemKdsCurLine;   /* not increment current line */
        ItemKdsSendData.usLineNum++;                    /* increment for parent */
        /* ItemKdsSendData.usLineNum = ++usItemKdsCurLine; */
#endif
        ItemKdsSendData.usLineMainType = MAIN_MODIFIER;
        ItemKdsSendData.usSubType1 = SUB_TAXMOD;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchLineNo - CLASS_UITAXMODIFIER1 +1);
        break;

    case CLASS_MODNUMBER:
        if ( pItem->aszName[0] == '\0' ) {
            return 0;
        }
        /* current line no in the transaction */
        ItemKdsSendData.usLineMainType = MAIN_OTHER;
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);   /* not increment current line */
#if 0
		// RJC use the standard line number generator
        ItemKdsSendData.usLineNum = usItemKdsCurLine;   /* not increment current line */
        ItemKdsSendData.usLineNum++;                    /* increment for parent */
        /* ItemKdsSendData.usLineNum = ++usItemKdsCurLine; */
#endif
        ItemKdsSendData.usSubType1 = SUB_NUMBER;
        ItemKdsSendData.ulSubType2 = 0;
//CHANGE FOR WIDE CONVERSION ?
        memcpy(ItemKdsSendData.auchSubType2, pItem->aszName, _tcslen(pItem->aszName));
        break;

    default:
        return KDS_PERFORM;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    if ((pItem->uchMinorClass == CLASS_TAXMODIFIER) ||
        (pItem->uchMinorClass == CLASS_MODNUMBER)) {

        ;   /* not send line data */
    } else {
        ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);
    }

    /* amount */
    ItemKdsSendData.lItemTotal = 0;
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}


static VOID ItemSendKdsTaxModifier(USHORT fbModifier)
{
	ITEMMODIFIER    ItemModifier = {0};

    ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemModifier.uchMinorClass = CLASS_TAXMODIFIER;
    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
		// CANTAX_MEAL and other bit masks shifted as they start at 0x01 but
		// TAX_MODIFIER1, etc. start at 0x02 due to VOID_MODIFIER bit is 0x01 in pItemSales->fbModifier.
        switch (fbModifier & TAX_MOD_MASK) {
        case  CANTAX_MEAL << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER1;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_CARBON << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER2;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_SNACK << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER3;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_BEER << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER4;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_LIQUOR << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER5;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_GROCERY << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER6;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_TIP << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER7;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_PST1 << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER8;
            ItemSendKdsModifier(&ItemModifier);
            break;
        case  CANTAX_BAKED << 1:
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER9;
            ItemSendKdsModifier(&ItemModifier);
            break;
        default:
            break;
        }
    } else {
        if (fbModifier & TAX_MODIFIER1) {
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER1;
            ItemSendKdsModifier(&ItemModifier);
        }
        if (fbModifier & TAX_MODIFIER2) {
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER2;
            ItemSendKdsModifier(&ItemModifier);
        }
        if (fbModifier & TAX_MODIFIER3) {
            ItemModifier.uchLineNo = CLASS_UITAXMODIFIER3;
            ItemSendKdsModifier(&ItemModifier);
        }
    }
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsModSales( VOID)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsModSales(UIFREGSALES *pItem)
{
    DCURRENCY  lAmount = 0;
    SHORT      sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(&ItemCommonLocal.ItemSales);

    /* item type */
    if ((ItemCommonLocal.ItemSales.uchMinorClass == CLASS_DEPTMODDISC) ||
        (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_PLUMODDISC) ||
        (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_OEPMODDISC) ||  /* R3.0 */
        (ItemCommonLocal.ItemSales.uchMinorClass == CLASS_SETMODDISC)) {

        /* modified item discount */

        ItemKdsSendData.usLineMainType = MAIN_DISC;
        ItemKdsSendData.usSubType1 = SUB_ITEMDISC;
        ItemKdsSendData.ulSubType2 = 2;
        ItemKdsSendData.lItemTotal = 0;
    } else {
        ItemKdsSendData.usLineMainType = MAIN_ITEM;
        ItemKdsSendData.usSubType1 = SUB_PRTMOD;
        ItemKdsSendData.ulSubType2 = (ULONG)usItemKdsPrvLine;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    if (ItemCommonLocal.ItemSales.fbModifier & VOID_MODIFIER) {
        pItem->fbModifier &= VOID_MODIFIER;
    }
    ItemSendKdsSetForm(&ItemCommonLocal.ItemSales, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = 0;

    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemSalesSalesAmtCalc(&ItemCommonLocal.ItemSales, &lAmount);
    ItemKdsSendData.lTransTotal += KDS_LONG_CAST(lAmount);

    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(&ItemCommonLocal.ItemSales, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    lParentAmount = 0;
    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsSales( ITEMSALES *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
SHORT   ItemSendKdsSales(ITEMSALES *pItem, USHORT fsEC)
{
	PARACOLORPALETTE  myColorPalette = {0};
	TCHAR  auchDummy[NUM_PLU_LEN+1] = {0};
    SHORT  sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_ITEM;
    if (pItem->uchMinorClass == CLASS_DEPT) {
        ItemKdsSendData.usSubType1 = SUB_DEPT;
        ItemKdsSendData.ulSubType2 = (ULONG)pItem->usDeptNo;
    } else if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        ItemKdsSendData.usSubType1 = SUB_WEIGHT;
        ItemKdsSendData.ulSubType2 = pItem->lQTY;
        if (ItemKdsSendData.ulSubType2 < 0) {
            ItemKdsSendData.ulSubType2 *= -1;
        }
        if (!CliParaMDCCheck(MDC_SCALE_ADR, ODD_MDC_BIT0)) {    /* 2 decimal point */
            ItemKdsSendData.ulSubType2 /= 10;
        }
    } else if (pItem->uchAdjective && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {
        ItemKdsSendData.usSubType1 = SUB_ADJ;
        ItemKdsSendData.ulSubType2 = (ULONG)((pItem->uchAdjective - 1) % 5 + 1); /* adj valiation */
    } else if (pItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
		if(pItem->usUniqueID)
		{
			ItemKdsSendData.usSubType1 = SUB_PREV_COND;
			ItemKdsSendData.ulSubType2 = ItemKdsSendData.usLineNum; //make the function for line data
		}else
		{
			ItemKdsSendData.usSubType1 = SUB_COND;
			ItemKdsSendData.ulSubType2 = (ULONG)usItemKdsPrvLine;
		}
    } else if (pItem->fbStorageStatus & IS_CONDIMENT_EDIT) {
		ItemKdsSendData.usSubType1 = SUB_PREV_COND;
		ItemKdsSendData.ulSubType2 = ItemKdsSendData.usLineNum; //make the function for line data
    } else {
        ItemKdsSendData.usSubType1 = SUB_PLU;
        ItemKdsSendData.ulSubType2 = 0;
    }

#if 0
    if (ItemKdsSendData.usSubType1 != (SUB_COND | SUB_PREV_COND)) {
        usItemKdsPrvLine = usItemKdsCurLine;    /* noun item line */

        if (pItem->fsLabelStatus & ITM_CONTROL_LINK) {
            usItemKdsPrvLine--; /* skip link line */
        }
    }
#endif

    //Read color data for the item, and send it to the KDS system
	myColorPalette.uchMajorClass = CLASS_PARACOLORPALETTE;
	myColorPalette.uchAddress = pItem->uchColorPaletteCode;

	//send the request to read the information
	CliParaRead(&myColorPalette);

	///set the colors to the appropriate data.
	ItemKdsSendData.ulForeGroundColor = myColorPalette.crForeground;
	ItemKdsSendData.ulBackGroundColor = myColorPalette.crBackground;

	if (fsEC) {
        /* for display format */
        pItem->lQTY *= -1;
        pItem->lProduct *= -1;
        pItem->lDiscountAmount *= -1;
        pItem->fbModifier |= VOID_MODIFIER;
    }

    /* item data */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        ItemKdsSendData.sQty = 1;
    } else {
        ItemKdsSendData.sQty = (SHORT)(pItem->lQTY/1000L);
    }
    if (ItemKdsSendData.usSubType1 == (SUB_COND | SUB_PREV_COND)) {
        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
            ItemKdsSendData.sQty = (SHORT)(pItem->lQTY/1000L);
        }
    }
    if (ItemKdsSendData.sQty < 0) {
        ItemKdsSendData.sQty *= -1;
    }
    if (_tcsncmp(pItem->auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
        tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    } else {
        _tcsncpy(ItemKdsSendData.aszPluNo, pItem->auchPLUNo, NUM_PLU_LEN);
    }
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
	sReturn = ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);
	NHPOS_ASSERT_TEXT(sReturn != 0, "ItemSendKdsSetForm() returned 0");

    /* amount */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItem->uchMinorClass == CLASS_OEPMODDISC) ||
        (pItem->uchMinorClass == CLASS_SETMODDISC)) {

        /* modified item discount */
        ItemKdsSendData.usLineMainType = MAIN_DISC;
        ItemKdsSendData.usSubType1 = SUB_ITEMDISC;
        ItemKdsSendData.ulSubType2 = 2;
        ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lDiscountAmount);
        if (pItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
            ItemKdsSendData.lItemTotal -= KDS_LONG_CAST(lPrevModDiscAmount);
        } else {
            lPrevModDiscAmount = pItem->lDiscountAmount;
        }
    } else if (pItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        /* 02/01/01 */
        if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) == 0) {
	        ItemKdsSendData.lItemTotal = pItem->lUnitPrice * ItemKdsSendData.sQty;
	    } else {
	        ItemKdsSendData.lItemTotal = pItem->lUnitPrice;
	    }
    } else {
		SHORT        i;
        ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lProduct);
//        ItemKdsSendData.lItemTotal = (ItemKdsSendData.sQty * pItem->lUnitPrice);
		for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
			ItemKdsSendData.lItemTotal += (ItemKdsSendData.sQty * pItem->Condiment[i].lUnitPrice);
		}
    }
    if (fsEC) {
        ItemKdsSendData.lItemTotal *= -1L;
    }

    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItem->uchMinorClass == CLASS_OEPMODDISC) ||  /* R3.0 */
        (pItem->uchMinorClass == CLASS_SETMODDISC)) {

        ItemKdsSendData.lTransTotal = KDS_LONG_CAST(pItem->lDiscountAmount);
    } else {
        if (pItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
			DCURRENCY         lAmount = 0;

           ItemSalesSalesAmtCalc(pItem, &lAmount);
           ItemKdsSendData.lTransTotal = KDS_LONG_CAST(lAmount);
        } else {
           ItemKdsSendData.lTransTotal = KDS_LONG_CAST(pItem->lProduct);
        }
    }
    if (fsEC) {
        ItemKdsSendData.lTransTotal = 0;
    }
    ItemKdsSendData.lTransTotal += ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTotal += KDS_LONG_CAST(lParentAmount);    /* for link plu */
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
	NHPOS_ASSERT(ItemKdsSendData.aszLineData[0] != 0);
    sReturn = KdsSendData(&ItemKdsSendData);

    if ((pItem->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) == 0) {
        /* for repeat, cursor void operation */
        if (pItem->uchPrintModNo) {
			UIFREGSALES  UifRegSales = {0};
			SHORT        i;

            /* send print modifier */
            UifRegSales.uchMajorClass = CLASS_UIFREGSALES;
            UifRegSales.uchMinorClass = CLASS_UIPRINTMODIFIER;
            for (i = 0; i < STD_MAX_COND_NUM; i++) {
                if (pItem->auchPrintModifier[i]) {
                    UifRegSales.uchPrintModifier = pItem->auchPrintModifier[i];
                    sReturn = ItemSendKdsModSales(&UifRegSales);
                }
            }
        }
    }

    if ((pItem->usLinkNo) && !(pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_CONDIMENT) && !(fsEC)) {
        /* save parent amount for link plu */
        if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
            (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
            (pItem->uchMinorClass == CLASS_OEPMODDISC) ||
            (pItem->uchMinorClass == CLASS_SETMODDISC)) {

            lParentAmount = pItem->lDiscountAmount;
        } else {
            lParentAmount = pItem->lProduct;
        }
    } else {
        lParentAmount = 0;
    }

    if (fsEC) {
        /* for recovery */
        pItem->lQTY *= -1;
        pItem->lProduct *= -1;
        pItem->lDiscountAmount *= -1;
        pItem->fbModifier &= ~VOID_MODIFIER;
    }

    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsDisc( ITEMDISC *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsDisc(ITEMDISC *pItem, USHORT fsEC)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_DISC;
    switch(pItem->uchMinorClass) {
    case CLASS_ITEMDISC1:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = SUB_ITEMDISC;
        ItemKdsSendData.ulSubType2 = pItem->uchDiscountNo;  // set the discount type for CLASS_ITEMDISC1 (1-6 as of 08/2009)
        break;

    case CLASS_REGDISC1:
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = SUB_TRANDISC;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchMinorClass - CLASS_REGDISC1 + 1);
        break;

    case CLASS_CHARGETIP:
    case CLASS_CHARGETIP2:
    case CLASS_CHARGETIP3:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = SUB_CHGTIPS;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchMinorClass - CLASS_CHARGETIP + 1);
        break;

    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = SUB_CHGTIPS;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchMinorClass - CLASS_AUTOCHARGETIP + 1);
        break;

    default:
        return KDS_PERFORM;
    }
    if (fsEC) {
        /* for display */
        pItem->lAmount *= -1;
        pItem->fbDiscModifier |= VOID_MODIFIER;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    //memset(ItemKdsSendData.aszPluNo, '0', NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal() + KDS_LONG_CAST(pItem->lAmount);
    if (pItem->uchMinorClass == CLASS_ITEMDISC1) {
		DCURRENCY         lAmount = 0;
		ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
        ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &lAmount);
        if (fsEC) {
            ItemKdsSendData.lTransTotal = KDS_LONG_CAST(lAmount);
        } else {
            ItemKdsSendData.lTransTotal += KDS_LONG_CAST(lAmount);
        }
    }
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    if (fsEC) {
        /* for recovery */
        pItem->lAmount *= -1;
        pItem->fbDiscModifier &= ~VOID_MODIFIER;
    }
    return sReturn;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsCoupon( ITEMCOUPON *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsCoupon(ITEMCOUPON *pItem, USHORT fsEC)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_DISC;
    if (pItem->uchMinorClass == CLASS_UPCCOUPON) {
        ItemKdsSendData.usSubType1 = SUB_UPCCOUPON;
        //_tcsncpy(ItemKdsSendData.auchSubType2, pItem->auchUPCCouponNo, NUM_PLU_LEN);
        memcpy(ItemKdsSendData.auchSubType2, pItem->auchUPCCouponNo, NUM_PLU_LEN);

    } else {
        ItemKdsSendData.usSubType1 = SUB_COUPON;
        ItemKdsSendData.ulSubType2 = (ULONG)pItem->uchCouponNo;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal() + KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTax = 0;

    if (fsEC) {
        ItemKdsSendData.lItemTotal *= -1L;
    }

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsPPI( ITEMSALES *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsPPI(ITEMSALES *pItem, USHORT fsEC)
{
	DCURRENCY  lAmount, lProduct;
	DUNITPRICE lUnitPrice;
	LONG       lQTY;
	SHORT      sReturn;

    lUnitPrice = pItem->lUnitPrice;
    if (lUnitPrice < 0L) lUnitPrice *= -1L;
    lProduct = pItem->lProduct;
    if (lProduct < 0L) lProduct *= -1L;
    lQTY = pItem->lQTY/1000L;
    if (lQTY < 0L) lQTY *= -1L;

    lAmount = (lUnitPrice * lQTY) - lProduct;

    if (lAmount == 0) {
        return KDS_PERFORM;
    }

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_DISC;
    ItemKdsSendData.usSubType1 = SUB_PPIDISC;
    ItemKdsSendData.ulSubType2 = (ULONG)pItem->uchPPICode;

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(lAmount);
    if (fsEC) {
        ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    } else {
        ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal() + KDS_LONG_CAST(pItem->lProduct);
    }
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;

}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsTotal( ITEMTOTAL *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsTotal(ITEMTOTAL *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_TOTAL;
    switch (pItem->uchMinorClass) {
    case CLASS_TOTAL1:
    case CLASS_TOTAL2:
    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
    case CLASS_TOTAL9:                      /* for FS */
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = (pItem->uchMinorClass - CLASS_TOTAL1 + 1);
        break;


	case CLASS_TOTAL10:
	case CLASS_TOTAL11:
	case CLASS_TOTAL12:
	case CLASS_TOTAL13:
	case CLASS_TOTAL14:
	case CLASS_TOTAL15:
	case CLASS_TOTAL16:
	case CLASS_TOTAL17:
	case CLASS_TOTAL18:
	case CLASS_TOTAL19:
	case CLASS_TOTAL20:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = (pItem->uchMinorClass - CLASS_TOTAL10 + 1);
        break;	

    case CLASS_TOTAL2_FS:                   /* FS */
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
		// if total uchMinorClass == CLASS_TOTAL2_FS then ulID contains total type, see ItemTendTotal()
        if (pItem->ulID != 0) {
            ItemKdsSendData.usSubType1 = ((UCHAR)pItem->ulID - CLASS_TOTAL1 + 1);
        } else {
            ItemKdsSendData.usSubType1 = CLASS_TOTAL2;
        }
        break;

    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
    case CLASS_SOFTCHK_TOTAL9:
    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = (pItem->uchMinorClass - CLASS_SOFTCHK_TOTAL3 + 3);
        break;
    default:
        return KDS_PERFORM;
    }
    ItemKdsSendData.ulSubType2 = (ULONG)ItemSendKdsSetTotalType(pItem);

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    if ((ItemKdsSendData.ulSubType2 == PRT_SERVICE1) || (ItemKdsSendData.ulSubType2 == PRT_SERVICE2)) {
        if (((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) &&
            !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3) &&
            !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
            /* split check total */
            ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lMI);
        } else {
            ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lService);
        }
    } else {
        ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lMI);
    }
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(pItem->lTax);

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}

static SHORT   ItemSendKdsUSCanTax(ITEMAFFECTION *pItem, USHORT usTax)
{
    SHORT sReturn;
    SHORT sItemCounter;

    if (usTax >= STD_TAX_ITEMIZERS_MAX || pItem->USCanVAT.USCanTax.lTaxable[usTax] == 0) {
        return KDS_PERFORM;
    }

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_TAX;
    ItemKdsSendData.usSubType1 = usTax+1;
    ItemKdsSendData.ulSubType2 = 0;

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    sItemCounter = pItem->sItemCounter;
    pItem->sItemCounter = usTax;
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);
    pItem->sItemCounter = sItemCounter;

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->USCanVAT.USCanTax.lTaxAmount[usTax]);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(pItem->USCanVAT.USCanTax.lTaxAmount[0]
                            + pItem->USCanVAT.USCanTax.lTaxAmount[1]
                            + pItem->USCanVAT.USCanTax.lTaxAmount[2]
                            + pItem->USCanVAT.USCanTax.lTaxAmount[3]);

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    /* save transaction tax for tender display */
    lCurTransTax += pItem->USCanVAT.USCanTax.lTaxAmount[usTax];

    return sReturn;
}

static SHORT   ItemSendKdsVATService(ITEMAFFECTION *pItem)
{
    SHORT sReturn;

    if (pItem->lAmount == 0) {
        return KDS_PERFORM;
    }

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_TAX;
    ItemKdsSendData.usSubType1 = 0;
    ItemKdsSendData.ulSubType2 = 0;

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(pItem->lAmount + pItem->USCanVAT.ItemVAT[0].lVATAmt
                            + pItem->USCanVAT.ItemVAT[1].lVATAmt
                            + pItem->USCanVAT.ItemVAT[2].lVATAmt);

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    /* save transaction tax for tender display */
    lCurTransTax += pItem->lAmount;

    return sReturn;
}

static SHORT   ItemSendKdsVAT(ITEMAFFECTION *pItem, USHORT usTax)
{
    SHORT sReturn;
    SHORT sItemCounter;

    if (usTax >= NUM_VAT || pItem->USCanVAT.ItemVAT[usTax].lVATAmt == 0) {
        return KDS_PERFORM;
    }

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_TAX;
    ItemKdsSendData.usSubType1 = usTax+1;
    ItemKdsSendData.ulSubType2 = 0;

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    sItemCounter = pItem->sItemCounter;
    pItem->sItemCounter = usTax;
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);
    pItem->sItemCounter = sItemCounter;

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->USCanVAT.ItemVAT[usTax].lVATAmt);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(pItem->lAmount + pItem->USCanVAT.ItemVAT[0].lVATAmt
                            + pItem->USCanVAT.ItemVAT[1].lVATAmt
                            + pItem->USCanVAT.ItemVAT[2].lVATAmt);

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    /* save transaction tax for tender display */
    lCurTransTax += pItem->USCanVAT.ItemVAT[usTax].lVATAmt;

    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsTax( ITEMTAX *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsTax(ITEMAFFECTION *pItem)
{
    SHORT sReturn = KDS_PERFORM;

    if (pItem->uchMinorClass == CLASS_TAXAFFECT) {
		USHORT i;
        for (i = 0; i < 4; i++) {
            sReturn = ItemSendKdsUSCanTax(pItem, i);
        }
    } else if (pItem->uchMinorClass == CLASS_SERVICE_VAT) {
        sReturn = ItemSendKdsVATService(pItem);
    } else if (pItem->uchMinorClass == CLASS_VATAFFECT) {
		USHORT i;
        for (i = 0; i < 3; i++) {
            sReturn = ItemSendKdsVAT(pItem, i);
        }
    }

    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsTender( ITEMTENDER *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsTender(ITEMTENDER *pItem, USHORT fsEC)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_TENDER;
    switch(pItem->uchMinorClass) {
    case CLASS_TENDER1:
    case CLASS_TENDER2:
    case CLASS_TENDER3:
    case CLASS_TENDER4:
    case CLASS_TENDER5:
    case CLASS_TENDER6:
    case CLASS_TENDER7:
    case CLASS_TENDER8:
    case CLASS_TENDER9:     /* Saratoga */
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
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = NORMAL_TENDER;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchMinorClass - CLASS_TENDER1 + 1);
        break;

    case CLASS_TEND_FSCHANGE:           /* Food Stamp Change, assume as tender 2 */
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = NORMAL_TENDER;
        ItemKdsSendData.ulSubType2 = CLASS_TENDER2; /* transform CLASS_TEND_FSCHANGE into Tender #2 */
        break;

    case CLASS_FOREIGN1:
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usSubType1 = FOREIGN_TENDER;
        ItemKdsSendData.ulSubType2 = (ULONG)(pItem->uchMinorClass - CLASS_FOREIGN1 + 1);
        break;

    default:
        return KDS_PERFORM;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    switch(pItem->uchMinorClass) {
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
        ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lTenderAmount);
        break;

    case CLASS_TEND_FSCHANGE:           /* FS Change */
        ItemKdsSendData.lItemTotal = 0;
        break;

    default:
        ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lForeignAmount);
		break;
    }
    /*
		SR 613
		DP Request:
		Change transaction total field to include the value of the amount 
		total field, located on the Sub Window, EXCEPT, that after first
		tendering starts, this field should be populated with the Balance
		Due amount, and finally with Change amount.
		ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
	*/
	if (pItem->lBalanceDue) {
		ItemKdsSendData.lTransTotal = KDS_LONG_CAST(pItem->lBalanceDue);
	} else {
		ItemKdsSendData.lTransTotal = KDS_LONG_CAST(pItem->lChange);
	}
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(lCurTransTax);
    if (fsEC) {
        ItemKdsSendData.lItemTotal *= -1L;
    }

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsMisc( ITEMMISC *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsMisc(ITEMMISC *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    switch(pItem->uchMinorClass) {
    case CLASS_CANCEL:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_CANCEL;
        ItemKdsSendData.usSubType1 = 0;
        ItemKdsSendData.ulSubType2 = 0;
        break;

    case CLASS_NOSALE:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_SINGLE;
        ItemKdsSendData.usSubType1 = SUB_NOSALE;
        ItemKdsSendData.ulSubType2 = 0;
        break;

    case CLASS_PO:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_SINGLE;
        ItemKdsSendData.usSubType1 = SUB_PO;
        ItemKdsSendData.ulSubType2 = 0;
        break;

    case CLASS_RA:
        /* current line no in the transaction */
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineMainType = MAIN_SINGLE;
        ItemKdsSendData.usSubType1 = SUB_ROA;
        ItemKdsSendData.ulSubType2 = 0;
        break;

    default:
        return KDS_PERFORM;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsTransOpen( ITEMTRANSOPEN *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsTransOpen(ITEMTRANSOPEN *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_OTHER;
    switch(pItem->uchMinorClass) {
    case CLASS_NEWCHECK:
    case CLASS_CASINTOPEN:
    case CLASS_REORDER:
    case CLASS_ADDCHECK:
    case CLASS_OPENPOSTCHECK:
    case CLASS_CASINTRECALL:
        /* current line no in the transaction */
		// Select Electronics has requested that a transaction open
		// message to the KDS always have a 1 as the line number.
		// We also want to make sure that the line number will not be
		// a one in any other KDS message.
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineNum = 1;
        if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
            ItemKdsSendData.usSubType1 = SUB_SRNO;
        } else {
            ItemKdsSendData.usSubType1 = SUB_GCNO;
        }
        ItemKdsSendData.ulSubType2 = (ULONG)pItem->usGuestNo;
        break;

    case CLASS_STORE_RECALL:
    /* case CLASS_STORE_RECALL_DELIV: */
        /* current line no in the transaction */
		// Select Electronics has requested that a transaction open
		// message to the KDS always have a 1 as the line number.
		// We also want to make sure that the line number will not be
		// a one in any other KDS message.
        ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
        ItemKdsSendData.usLineNum = 1;
        ItemKdsSendData.usSubType1 = SUB_SRNO;
        ItemKdsSendData.ulSubType2 = (ULONG)pItem->usGuestNo;
        break;

    default:
        return KDS_PERFORM;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = KDS_LONG_CAST(pItem->lAmount);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsPrint( ITEMPRINT *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsPrint(ITEMPRINT *pItem)
{
    SHORT sReturn;
	SHORT numCounter; //US Customs

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_OTHER;
    switch(pItem->uchMinorClass) {
    case CLASS_AGELOG:
		//US Customs
		if(pItem->aszNumber[0][0] != '\0'){
		//End US Customs
			/* current line no in the transaction */
			ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
			ItemKdsSendData.usSubType1 = SUB_AGELOG;
			ItemKdsSendData.ulSubType2 = 0;
//CHANGE FOR WIDE CONVERSION ?
			memcpy(ItemKdsSendData.auchSubType2, pItem->aszNumber[0], _tcslen(pItem->aszNumber[0]));
		}
        break;

    case CLASS_NUMBER:
		//US Customs
		for(numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
			if(pItem->aszNumber[numCounter][0] == '\0'){ //empty slot found
				break;
			}
		}
		if(numCounter >= NUM_OF_NUMTYPE_ENT) {
			return 0;
		}
		if((pItem->aszNumber[numCounter-1][0] != '\0') && numCounter != 0){
		//END US Customs
			/* current line no in the transaction */
			ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
			ItemKdsSendData.usSubType1 = SUB_NUMBER;
			ItemKdsSendData.ulSubType2 = 0;
//CHANGE FOR WIDE CONVERSION ?
			memcpy(ItemKdsSendData.auchSubType2, pItem->aszNumber[0], _tcslen(pItem->aszNumber[0]));
		}
		break;

    default:
        return KDS_PERFORM;
    }

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = 0;
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}

/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsTransClose(ITEMTRANSCLOSE *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
static SHORT   ItemSendKdsTransClose(ITEMTRANSCLOSE *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_OTHER;
    ItemKdsSendData.usSubType1 = SUB_ANY;
    ItemKdsSendData.ulSubType2 = 0;

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
    /* ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]); */

    /* amount */
    ItemKdsSendData.lItemTotal = 0;
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = KDS_LONG_CAST(lCurTransTax);

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    /* inititalize transaction line no */
    TrnInformation.TranGCFQual.usItemKdsCurLine = 0;
    usItemKdsPrvLine = 0;

    lCurTransTax = 0;
	memset(&usLineNumberContainer, 0x00, sizeof(usLineNumberContainer));

    return sReturn;
}

static VOID ItemSendKdsVoidEC(USHORT fsVoidEC, USHORT fsTender, DCURRENCY dAmount, VOID *pItem)
{
    SHORT sReturn;
	LONG lAmount = KDS_LONG_CAST(dAmount);   // convert to LONG for the KDS binary protocol

    if (fsVoidEC == SUB_EC) {
        if (!fsTender) lAmount *= -1;
    }

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* current line no in the transaction */
    ItemKdsSendData.usLineNum = TrnInformation.TranGCFQual.usItemKdsCurLine;   /* not increment current line */
    ItemKdsSendData.usLineNum++;                    /* increment for parent */

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_OTHER;
    if (fsVoidEC == SUB_EC) {
        ItemKdsSendData.usSubType1 = SUB_EC;
    } else {
        ItemKdsSendData.usSubType1 = SUB_VOID;
    }
    ItemKdsSendData.ulSubType2 = labs(lAmount);

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* amount */
    ItemKdsSendData.lItemTotal = labs(lAmount);
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    if (((ITEMSALES *)pItem)->uchMinorClass == CLASS_ITEMDISC1) {
		DCURRENCY         lItemAmount = 0;
		ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();
        ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &lItemAmount);
        ItemKdsSendData.lTransTotal += KDS_LONG_CAST(lItemAmount);
    }
    if ((fsVoidEC == SUB_VOID) && (fsTender == 0)) {
        ItemKdsSendData.lTransTotal += lAmount;
    }
    if (fsTender) {
        ItemKdsSendData.lTransTax = KDS_LONG_CAST(lCurTransTax);
    } else {
        ItemKdsSendData.lTransTax = 0;
    }

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

}

static VOID ItemSendKdsNumber(CONST TCHAR *aszNumber)
{
	ITEMMODIFIER    ItemModifier = {0};

    ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
    ItemModifier.uchMinorClass = CLASS_MODNUMBER;
    _tcsncpy(&ItemModifier.aszName[0], aszNumber, NUM_NUMBER);
    ItemSendKdsModifier(&ItemModifier);
}


USHORT ItemSendKdsItemSetLineNumber(CONST ITEMSALES *pItem)
{
	USHORT i;
	for(i = 0; i < 256; i++)
	{
		if(usLineNumberContainer[i].usLineNumber == 0)
		{
			usLineNumberContainer[i].usLineNumber = ++(TrnInformation.TranGCFQual.usItemKdsCurLine);
			usLineNumberContainer[i].usUniqueID = pItem->usUniqueID;
			return TrnInformation.TranGCFQual.usItemKdsCurLine;
		}
	}
	return -1;

}
/*
*===========================================================================
**Synopsis: SHORT   ItemSendKdsOrderDeclaration(ITEMSALES *pItem)
*
*    Input: VOID   *pItem
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*
*
*===========================================================================
*/
SHORT   ItemSendKdsOrderDeclaration(ITEMSALES *pItem)
{
    SHORT sReturn;

    memset(&ItemKdsSendData, 0, sizeof(ItemKdsSendData));

    /* transaction mode */
    ItemKdsSendData.uchTransMode1 = ItemSendKdsSetTransMode1();
    ItemKdsSendData.uchTransMode2 = ItemSendKdsSetTransMode2(pItem);

    /* item type */
    ItemKdsSendData.usLineMainType = MAIN_OTHER;
    ItemKdsSendData.usLineNum = ItemSendKdsItemGetLineNumber(pItem);
    
	ItemKdsSendData.usSubType1 = SUB_ORDER_DEC;
    ItemKdsSendData.ulSubType2 = pItem->uchMinorClass;
	ItemKdsSendData.uchOrderDec = pItem->uchAdjective;   // uchAdjective is reused as order declare number, CLASS_ITEMORDERDEC

    /* item data */
    ItemKdsSendData.sQty = 0;
    tcharnset(ItemKdsSendData.aszPluNo, _T('0'), NUM_PLU_LEN);
    ItemKdsSendData.ulOperatorId = ItemSendKdsSetOpeId();

    /* create line data */
	memcpy(&ItemKdsSendData.aszLineData[0], pItem->aszMnemonic, sizeof(pItem->aszMnemonic));
    //ItemSendKdsSetForm(pItem, &ItemKdsSendData.aszLineData[0]);

    /* amount */
    ItemKdsSendData.lItemTotal = 0;
    ItemKdsSendData.lTransTotal = ItemSendKdsSetTotal();
    ItemKdsSendData.lTransTax = 0;

    /* -- set destination CRT -- */
    ItemSendKdsSetCRTNo(pItem, ItemKdsSendData.auchCRTNo);

    /* send through UDP port */
    sReturn = KdsSendData(&ItemKdsSendData);

    return sReturn;
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSendKds( VOID *pItem, USHORT fsEC)
*
*    Input: VOID   *pItem
*           USHORT fsEC
*
*   Output: None
*
*    InOut: None
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_DFLERR_ADR : KDS Send error
*
** Description: Send buffered item to TCP/IP KDS System
*               This functionality is used with the Select Electronics kitchen display system
*               using the Oasys iPAD with bump bar and the ROD Gold software server interface.
*               There are also some installations that use this as a data stream to combine
*               with surveillance video footage in order to show the cashier as well as the
*               point of sale transaction actions of the cashier.
*
*               WARNING: Provisioning of the ROD Gold software using the ROD Gold Maintenance utility
*                        must be correct so far as IP addresses and options.
*
*                        Important: in ROD Global settings the Enhanced Record Format check box
*                                   must be checked in order to use the new KDS record format.
*                                   The System Type must be NCR NHPOS.
*
*===========================================================================
*/
SHORT   ItemSendKds(VOID *pItem, USHORT fsEC)
{
    ITEMSALES       *pItemSales = pItem;      /* pointer save buffer for casted receive item */
    ITEMDISC        *pItemDisc = pItem;
    ITEMCOUPON      *pItemCoupon = pItem;
    ITEMTOTAL       *pItemTotal = pItem;
    ITEMTENDER      *pItemTender = pItem;
	UIFREGSALES     *pUifRegSales = pItem;
    SHORT           sReturn;

    if (CliParaMDCCheck(MDC_DSP_ON_FLY3, ODD_MDC_BIT0) == 0) {
        /* RS232C KDS */
        return ITM_SUCCESS;
    }

    /*----- CURRENT PRINT MOD., CONDIMENT MODIFIER -----*/
	// all of the various ITEMaaaa structs have the same initial members of uchMajorClass and uchMinorClass
    switch(pItemSales->uchMajorClass) {
    case CLASS_ITEMSALES:
		if(pItemSales->uchMinorClass == CLASS_ITEMORDERDEC)
		{
			sReturn = ItemSendKdsOrderDeclaration(pItemSales);
			return sReturn;
		}

        if (fsEC) {
            if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
                (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
                (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

                ItemSendKdsVoidEC(SUB_EC, 0, pItemSales->lDiscountAmount, pItem);
            } else {
                ItemSendKdsVoidEC(SUB_EC, 0, pItemSales->lProduct, pItem);
            }
        } else if (pItemSales->fbModifier & VOID_MODIFIER) {
            if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
                (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
                (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

                ItemSendKdsVoidEC(SUB_VOID, 0, pItemSales->lDiscountAmount, pItem);
            } else {
                ItemSendKdsVoidEC(SUB_VOID, 0, pItemSales->lProduct, pItem);
            }
        }
        if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) == 0) {
            if (pItemSales->uchPPICode) {
                sReturn = ItemSendKdsPPI(pItemSales, fsEC);
            }
        }
        if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) == 0) {
			SHORT   numCounter;	 //US Customs cwunn 4/10/03
			for(numCounter = 0; numCounter < NUM_OF_NUMTYPE_ENT; numCounter++){
				if (pItemSales->aszNumber[numCounter][0]) {
					ItemSendKdsNumber(pItemSales->aszNumber[numCounter]);
				}
				else{
					break;
				}
			}
			//End US Customs

            if (pItemSales->fbModifier & TAX_MOD_MASK) {
                ItemSendKdsTaxModifier(pItemSales->fbModifier);
            }
        }
        sReturn = ItemSendKdsSales(pItemSales, fsEC);
        break;

    case CLASS_UIFREGSALES:
        if (fsEC) {
            ItemSendKdsVoidEC(SUB_EC, 0, 0, pUifRegSales);
        } else if (pUifRegSales->fbModifier & VOID_MODIFIER) {
            ItemSendKdsVoidEC(SUB_VOID, 0, 0, pUifRegSales);
        }
        if (pUifRegSales->uchMinorClass == CLASS_UIPRINTMODIFIER) {
            sReturn = ItemSendKdsModSales(pUifRegSales);
        }
        break;

    case CLASS_ITEMDISC:
        if (fsEC) {
            ItemSendKdsVoidEC(SUB_EC, 0, pItemDisc->lAmount, pItem);
        } else if (pItemDisc->fbDiscModifier & VOID_MODIFIER) {
            ItemSendKdsVoidEC(SUB_VOID, 0, pItemDisc->lAmount, pItem);
        }
        if (pItemDisc->aszNumber[0]) {
            ItemSendKdsNumber(pItemDisc->aszNumber);
        }
        if (pItemDisc->fbDiscModifier & TAX_MOD_MASK) {
            ItemSendKdsTaxModifier(pItemDisc->fbDiscModifier);
        }
        sReturn = ItemSendKdsDisc(pItemDisc, fsEC);
        break;

    case CLASS_ITEMCOUPON:
        if (fsEC) {
            ItemSendKdsVoidEC(SUB_EC, 0, pItemCoupon->lAmount, pItem);
        } else if (pItemCoupon->fbModifier & VOID_MODIFIER) {
            ItemSendKdsVoidEC(SUB_VOID, 0, pItemCoupon->lAmount, pItem);
        }
        if (pItemCoupon->aszNumber[0]) {
            ItemSendKdsNumber(pItemCoupon->aszNumber);
        }
        if (pItemCoupon->fbModifier & TAX_MOD_MASK) {
            ItemSendKdsTaxModifier(pItemCoupon->fbModifier);
        }
        sReturn = ItemSendKdsCoupon(pItemCoupon, fsEC);
        break;

    case CLASS_ITEMTOTAL:
        if (pItemTotal->aszNumber[0]) {
            ItemSendKdsNumber(pItemTotal->aszNumber);
        }
        sReturn = ItemSendKdsTotal(pItemTotal);
        break;

    case CLASS_ITEMAFFECTION:
        sReturn = ItemSendKdsTax(pItem);
        break;

    case CLASS_ITEMTENDER:
        if (fsEC) {
            switch (pItemTender->uchMinorClass) {
            case CLASS_FOREIGN1:
            case CLASS_FOREIGN2:
            case CLASS_FOREIGN3:
            case CLASS_FOREIGN4:
            case CLASS_FOREIGN5:
            case CLASS_FOREIGN6:
            case CLASS_FOREIGN7:
            case CLASS_FOREIGN8:
                ItemSendKdsVoidEC(SUB_EC, 1, pItemTender->lForeignAmount, pItem);
                break;
            default:
                ItemSendKdsVoidEC(SUB_EC, 1, pItemTender->lTenderAmount, pItem);
            }
        } else if (pItemTender->fbModifier & VOID_MODIFIER) {
            switch (pItemTender->uchMinorClass) {
            case CLASS_FOREIGN1:
            case CLASS_FOREIGN2:
            case CLASS_FOREIGN3:
            case CLASS_FOREIGN4:
            case CLASS_FOREIGN5:
            case CLASS_FOREIGN6:
            case CLASS_FOREIGN7:
            case CLASS_FOREIGN8:
                ItemSendKdsVoidEC(SUB_VOID, 1, pItemTender->lForeignAmount, pItem);
                break;
            default:
                ItemSendKdsVoidEC(SUB_VOID, 1, pItemTender->lTenderAmount, pItem);
            }
        }
        if (pItemTender->aszNumber[0]) {
            ItemSendKdsNumber(pItemTender->aszNumber);
        }
        sReturn = ItemSendKdsTender(pItemTender, fsEC);
        break;

    case CLASS_ITEMMISC:
        sReturn = ItemSendKdsMisc(pItem);
        break;

    case CLASS_ITEMMODIFIER:
        sReturn = ItemSendKdsModifier(pItem);
        break;

    case CLASS_ITEMTRANSOPEN:
        sReturn = ItemSendKdsTransOpen(pItem);
        break;

    case CLASS_ITEMPRINT:
        sReturn = ItemSendKdsPrint(pItem);
        break;

    case CLASS_ITEMTRANSCLOSE:
        sReturn = ItemSendKdsTransClose(pItem);
        break;

    default:
        sReturn = KDS_PERFORM;
    }

    if (sReturn == KDS_PERFORM) {
        sReturn = ITM_SUCCESS;
    } else {
        sReturn = LDT_DFLERR_ADR;
    }
    return sReturn;

}

/****** End of Source ******/