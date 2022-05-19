/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998-2000      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION ITEMIZERS MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNITEM.C
:  ---------------------------------------------------------------------
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
:   Memory Model       : Medium Model
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp
:  ---------------------------------------------------------------------
:  Abstract:
:       TrnItemizer()       ;   transaction itemizer
:       TrnItemTrans()      ;   itemize by transaction open
:       TrnItemRO()         ;   itemize by reorder
:       TrnItemSales()      ;   itemize by sales
:       TrnSalesDept()      ;   itemize by dept, set menu
:       TrnQTY()            ;   calculate QTY
:       TrnSalesPLU()       ;   itemize by PLU item
:       TrnTaxable()        ;   update taxable itemizer
:       TrnDiscTax()        ;   update taxable/discountable itemizer
:       TrnItemCoupon()     ;   itemize by coupon item
:       TrnSalesMod()       ;   itemize by modifier discount
:       TrnItemDisc()       ;   itemize by discount
:       TrnItemDiscount()   ;   itemize by item discount
:       TrnRegDiscount()    ;   itemize by regular discount
:       TrnRegDiscTax()     ;   calculate discout/taxable
:       TrnChargeTip()      ;   itemize by charge tip
:       TrnItemAC()         ;   itemize by manual addcheck
:       TrnGetTI()          ;   get transaction itemizers
:       TrnPutTI()          ;   put transaction itemizers
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Jun-01-92 : 00.00.01 : H.Kato     : Initial
: Mar-31-95 : 03.00.00 : T.Nakahata : Add Combination Coupon Feature and
:           :          :            : condiment of promotional PLU
: Apr-24-95 : 03.00.00 : T.Nakahata : Add Order Entry Prompt PLU
: Jun-15-95 : 03.00.00 : T.Nakahata : Modify TrnRegDiscTax for manual entry
: Jul-05-95 : 03.00.01 : T.Nakahata : save total amount at store/recall
: Nov-28-95 : 03.01.00 : hkato      : R3.1
: Aug-05-98 : 03.03.00 : hrkato     : V3.3
:
:** NCR2172 **
:
: Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
: May-17-00 : 01.00.00 : hrkato     : Saratoga FVT Comments
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "log.h"
#include    "appllog.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "item.h"
#include    "rfl.h"
#include    "uie.h"
#include    "pif.h"
#include    "csetk.h"
#include    "uireg.h"
#include    "../item/include/itmlocal.h"


/*==========================================================================
**   Synopsis:  UCHAR TrnSalesItemIsFoodStamp (CONST UCHAR auchPluStatus[ITM_PLU_CONT_OTHER])
*
*    Input:     CONST UCHAR auchPluStatus[12] - PLU settings data, ControlCode.auchPluStatus
*    Output:    none
*    InOut:     none
*
*    Return:    UCHAR  0 not food stamp, 1 is food stamp
*
*    Description:   Determine if the PLU settings data provided indicates that
*                   a PLU is a food stamp item or not.
*
*                   The PLU settings data is a copy of the PLU record Settings
*                   stored in the auchPluStatus member of the ITEMCONTCODE struct.
*
*                   We accept the PLU status array containing the various PLU
*                   settings so that this function can be used with either an
*                   ITEMSALES struct PLU or a condiment that is stored in an
*                   ITEMSALES struct.
==========================================================================*/
UCHAR TrnSalesItemIsFoodStamp (CONST UCHAR auchPluStatus[ITM_PLU_CONT_OTHER])
{
	// If this is a US tax system, MDC 11 and 15 are set correctly, and the PLU status
	// indicates that this is a food stamp item then return true. Otherwise return false.
	return (TrnTaxSystem() == TRN_TAX_US && (auchPluStatus[1] & PLU_AFFECT_FS));
}

/*==========================================================================
**    Synopsis: SHORT   TrnItemizer( VOID *TrnItemizers )
*
*    Input:     VOID    *TrnItemizers
*    Output:    none
*    InOut:     none
*
*    Return:    TRN_SUCCESS
*               TRN_ERROR
*
*    Description:   transaction itemizers method
==========================================================================*/

SHORT    TrnItemizer( VOID *TrnItemizers )
{
	switch( ITEMSTORAGENONPTR(TrnItemizers)->uchMajorClass ) {
    case CLASS_ITEMTRANSOPEN :
        TrnItemTrans( TrnItemizers );        /* transaction open method */
        break;

    case CLASS_ITEMSALES :
        TrnItemSales( TrnItemizers );            /* sales method */
        break;

    case CLASS_ITEMDISC :
        TrnItemDisc( TrnItemizers );              /* discount method */
        break;

    case CLASS_ITEMMISC:
        TrnItemMisc( TrnItemizers );                  /* Misc,    Saratoga */
        break;

    case CLASS_ITEMAFFECTION :
        TrnItemAC( TrnItemizers );           /* affection method */
        break;

    case CLASS_ITEMTOTAL:                                       /* save total amount */
        TrnItemTotal( TrnItemizers );
        break;

/*** For REL 3.0 (95-2-20) ***/
    case CLASS_ITEMCOUPON:                                      /* combi. coupon */
        TrnItemCoupon( TrnItemizers );
        break;
/*** For REL 3.0 (95-2-20) ***/

    default : 
        return( TRN_ERROR );                                    /* illegal major class */
    }

    return( TRN_SUCCESS );                                      /* return */
}



/*==========================================================================
**    Synopsis: VOID    TrnItemTrans( ITEMTRANSOPEN *ItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by transaction open
==========================================================================*/

VOID    TrnItemTrans( ITEMTRANSOPEN *ItemTransOpen )
{
    switch ( ItemTransOpen->uchMinorClass ) {
	case CLASS_OPEN_TRANSBATCH:
    case CLASS_REORDER :
    case CLASS_MANUALPB :
        TrnItemRO( ItemTransOpen );                             /* reorder */
        break;

    default:
        break;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemRO( ITEMTRANSOPEN *ItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by manual PB
==========================================================================*/

VOID    TrnItemRO( ITEMTRANSOPEN *ItemTransOpen )
{
    if ( ItemTransOpen->lAmount != 0L ) {
        TrnInformation.TranItemizers.lMI = ItemTransOpen->lAmount;      /* main itemizer */
        if (TrnTaxSystem() == TRN_TAX_US && ItemTransOpen->uchMinorClass == CLASS_MANUALPB) {
			USHORT  usWorkMDC = ItemTransOpen->uchStatus;                      /* MDC */
			USHORT  usWorkMod = ItemTransOpen->fbModifier;                     /* modifier */
			USHORT  usWorkBit;
			SHORT   i;

            usWorkBit = 0x02;            // affect TAX_MODIFIER1 indicator
            for ( i = 0; i < STD_TAX_ITEMIZERS_US; i++, usWorkBit <<= 1 ) {
                if ( ( usWorkMDC ^ usWorkMod ) & usWorkBit ) {       /* check taxable */
                    TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lNonAffectTaxable[i] = ItemTransOpen->lAmount;
                }
            }
        }
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemSales( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by sales
==========================================================================*/

VOID    TrnItemSales( ITEMSALES *ItemSales )
{
    switch( ItemSales->uchMinorClass ) {

    case CLASS_DEPT :               /* dept */
    case CLASS_DEPTITEMDISC :       /* dept w/item discount */
        TrnSalesDept( ItemSales );
        break;

    case CLASS_PLU :                /* PLU */
    case CLASS_PLUITEMDISC :        /* PLU w/item discount */
    case CLASS_SETMENU :            /* set menu */
    case CLASS_SETITEMDISC :        /* set menu w/item discount */
    case CLASS_OEPPLU:              /* order enrty prompt PLU */
    case CLASS_OEPITEMDISC:         /* o.e.p. PLU w/ item discount */
	case CLASS_ITEMORDERDEC:
        TrnSalesPLU( ItemSales );
        break;

    case CLASS_DEPTMODDISC :        /* dept w/modifier discount */
    case CLASS_PLUMODDISC :         /* PLU w/modifier discount */
    case CLASS_SETMODDISC :         /* set menu w/modifier discount */
    case CLASS_OEPMODDISC:          /* o.e.p. PLU w/ modifier discount */
        TrnSalesMod( ItemSales );
        break;

    default :                       /* illegal */
        break;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSalesDept( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*    Return:    none
*
*    Description:   itemize by dept, set menu,                      V3.3
==========================================================================*/

VOID    TrnSalesDept(CONST ITEMSALES *ItemSales )
{
    UCHAR   fchFood = 0;

    TrnInformation.TranItemizers.lMI += ItemSales->lProduct;       /* main itemizer */

    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (ItemSales->ControlCode.auchPluStatus);
	
    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1 ) {
        TrnInformation.TranItemizers.lDiscountable[0] += ItemSales->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] += ItemSales->lProduct;
        }
    }
    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2 ) {
        TrnInformation.TranItemizers.lDiscountable[1] += ItemSales->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += ItemSales->lProduct;
        }
    }
    if ((ItemSales->ControlCode.auchPluStatus[5] & ITM_SALES_HOURLY)
        && !(ItemSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {      /* R3.1 */
        TrnInformation.TranItemizers.lHourly += ItemSales->lProduct; 
		
		//if we are modifying an item, we will not want to increase 
		//the item counter, because we have not added a new item, duh!
		if(ItemSales->uchMinorClass != CLASS_ITEMMODIFICATION)
		{
			TrnInformation.TranItemizers.sItemCounter += TrnQTY( ItemSales );
		}

        /* affect to hourly bonus total, V3.3 */
        if (ItemSales->ControlCode.usBonusIndex) {
            TrnInformation.TranItemizers.lHourlyBonus[ItemSales->ControlCode.usBonusIndex] += ItemSales->lProduct; 
        }
    }
    if (!(ItemSales->fbReduceStatus & REDUCE_ITEM) && ItemSales->ControlCode.usBonusIndex) {
        TrnInformation.TranItemizers.lTransBonus[ItemSales->ControlCode.usBonusIndex] += ItemSales->lProduct; 
    }

    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        TrnSalesDeptUSComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnSalesDeptCanComn(ItemSales, &TrnInformation.TranItemizers, &TrnInformation.TranGCFQual, TRN_ITEMIZE_NORMAL);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnSalesDeptIntlComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    }
}

VOID    TrnSalesAdjustItemizers ( ITEMGENERICHEADER *pItem )
{
	if (pItem->uchMajorClass == CLASS_ITEMTENDER) {
		DCURRENCY        lGratuitySave;
		ITEMTENDER       *pItemTender = (ITEMTENDER *)pItem;
		ITEMCOMMONLOCAL  *pItemCommonLocal = ItemCommonGetLocalPointer();

		TrnInformation.TranItemizers.lMI += pItemTender->lGratuity;       /* main itemizer */
		pItemCommonLocal->lChargeTip += pItemTender->lGratuity;
		lGratuitySave = pItemCommonLocal->ReturnsTenderChargeTips.lGratuity;
		pItemCommonLocal->ReturnsTenderChargeTips = ITEMTENDER_CHARGETIPS(pItemTender);
		pItemCommonLocal->ReturnsTenderChargeTips.lGratuity += lGratuitySave;
		memcpy (pItemCommonLocal->aszCardLabelReturnsTenderChargeTips, pItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], sizeof(pItemCommonLocal->aszCardLabelReturnsTenderChargeTips));
	}
}

/*==========================================================================
**   Synopsis:  SHORT   TrnQTY( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    QTY
*
*    Description:   Calculate quantity of an item by performing the correct
*                   method of generating a quantity.
*
*                   For scalable items we return a count of 1 or -1 however
*                   for non-scalable items we do the standard division by 1000L.
*                   The item quantity is scaled by 1000L because weight for
*                   scalable items is in thousands of a unit (lb or kg).
==========================================================================*/

SHORT    TrnQTY( CONST ITEMSALES *ItemSales )
{
    SHORT    sCount = 0;

    if ( ItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE ) {
		// set the quantity to be either -1, 1, or 0 depending on whether the
		// quantity is negative, positive, or zero.
        if (ItemSales->lQTY < 0L) {               /* minus */
            sCount = -1;
        } else if (ItemSales->lQTY > 0L) {
            sCount = 1;                           /* scalable item */
        }
	} else {
		sCount = ( SHORT )( ItemSales->lQTY / ITM_SL_QTY );
	}

    return( sCount );
}

/*==========================================================================
**    Synopsis: VOID    TrnSalesPLU( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by PLU, promotional PLU
==========================================================================*/

VOID    TrnSalesPLU( ITEMSALES *ItemSales )
{
    USHORT      i;
    USHORT      usMaxChild;
    DCURRENCY   lChildAmount;
	UCHAR	    fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (ItemSales->ControlCode.auchPluStatus);

    TrnInformation.TranItemizers.lMI += ItemSales->lProduct;           /* main itemizer */
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += ItemSales->lProduct;
        /* --- Taxable #1-3 ---  */
        if ((ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX1) ||
            (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX2) ||
            (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX3)) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += ItemSales->lProduct;
        }
    }
	if(couponAmount < 0)
	{
		ItemSales->lDiscountAmount = couponAmount;
	}

    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1 ) {
        TrnInformation.TranItemizers.lDiscountable[0] += ItemSales->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0]  += ItemSales->lProduct;
        }
    }
    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2 ) {
        TrnInformation.TranItemizers.lDiscountable[1] += ItemSales->lProduct;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += ItemSales->lProduct;
        }
    }

    if ((ItemSales->ControlCode.auchPluStatus[5] & ITM_SALES_HOURLY)
        && !(ItemSales->fbReduceStatus & REDUCE_NOT_HOURLY)
		&& (ItemSales->uchMinorClass != CLASS_ITEMORDERDEC)) {      /* R3.1 */
        TrnInformation.TranItemizers.lHourly += ItemSales->lProduct;
		
		//if we are modifying an item, we will not want to increase 
		//the item counter, because we have not added a new item, duh!
		if(ItemSales->uchMinorClass != CLASS_ITEMMODIFICATION)
		{
			TrnInformation.TranItemizers.sItemCounter += TrnQTY( ItemSales );
		}

        /* affect to hourly bonus total, V3.3 */
        if (ItemSales->ControlCode.usBonusIndex) {
            TrnInformation.TranItemizers.lHourlyBonus[(ItemSales->ControlCode.usBonusIndex - 1)] += ItemSales->lProduct; 
        }
    }

    if (!(ItemSales->fbReduceStatus & REDUCE_ITEM)
		&& (ItemSales->uchMinorClass != CLASS_ITEMORDERDEC)
		&& ItemSales->ControlCode.usBonusIndex) {
        TrnInformation.TranItemizers.lTransBonus[(ItemSales->ControlCode.usBonusIndex - 1)] += ItemSales->lProduct; 
    }

    /* --- itemize condiment PLU --- */
    usMaxChild = ItemSales->uchChildNo + ItemSales->uchCondNo + ItemSales->uchPrintModNo;

	NHPOS_ASSERT(usMaxChild <= sizeof(ItemSales->Condiment)/sizeof(ItemSales->Condiment[0]));

    /* link plu, saratoga */
    if (ItemSales->usLinkNo) {
        i = 0;
    } else {
        i = ItemSales->uchChildNo;
    }
    for (; i < usMaxChild; i++ ) {

        if (RflIsSpecialPlu(ItemSales->Condiment[ i ].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) == 0) {
            continue;
        }

	    /* correct fs affection, 02/14/01 */
		fchFood = TrnSalesItemIsFoodStamp (ItemSales->Condiment[i].ControlCode.auchPluStatus);

        lChildAmount = (DCURRENCY)TrnQTY(ItemSales) * ItemSales->Condiment[i].lUnitPrice;

        TrnInformation.TranItemizers.lMI += lChildAmount;

        /* --- Food Stampable --- */
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += lChildAmount;
            /* --- Taxable #1-3 ---  */
            if ((ItemSales->Condiment->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX1) ||
                (ItemSales->Condiment->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX2) ||
                (ItemSales->Condiment->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX3)) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += lChildAmount;
            }
        }

        if ( ItemSales->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1 ) {
            TrnInformation.TranItemizers.lDiscountable[0] += lChildAmount;
            if (fchFood) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] += lChildAmount;
            }
        }

        if ( ItemSales->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2 ) {
            TrnInformation.TranItemizers.lDiscountable[1] += lChildAmount;
            if (fchFood) {
                TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += lChildAmount;
            }
        }

        if ((ItemSales->Condiment[i].ControlCode.auchPluStatus[5] & ITM_SALES_HOURLY)
            && !(ItemSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {      /* R3.1 */
            TrnInformation.TranItemizers.lHourly += lChildAmount;

            /* affect to hourly bonus total, V3.3 */
            if (ItemSales->Condiment[i].ControlCode.usBonusIndex) {
                TrnInformation.TranItemizers.lHourlyBonus[ItemSales->Condiment[i].ControlCode.usBonusIndex] += lChildAmount;
            }
        }

		if (!(ItemSales->fbReduceStatus & REDUCE_ITEM) && ItemSales->Condiment[i].ControlCode.usBonusIndex) {
			TrnInformation.TranItemizers.lTransBonus[ItemSales->Condiment[i].ControlCode.usBonusIndex] += lChildAmount;
		}
    }

	if(ItemSales->uchMinorClass != CLASS_ITEMORDERDEC)
	{
		TrnTaxable( ItemSales );
		TrnDiscTax( ItemSales );
	}
}

/*
*==========================================================================
**    Synopsis: VOID    TrnTaxable( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update taxable itemizer                         V3.3
*==========================================================================
*/
VOID    TrnTaxable(ITEMSALES *ItemSales)
{
    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        TrnTaxableUSComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnSalesDeptCanComn(ItemSales, &TrnInformation.TranItemizers, &TrnInformation.TranGCFQual, TRN_ITEMIZE_NORMAL);
        TrnTaxableCanComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                           /* Int'l VAT,   V3.3 */
        TrnTaxableIntlComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    }
}

/*
*==========================================================================
**    Synopsis: VOID    TrnDiscTax( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update discountable/taxable itemizer            V3.3
*==========================================================================
*/
VOID    TrnDiscTax(ITEMSALES *ItemSales)
{
    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        TrnDiscTaxUSComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnDiscTaxCanComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                        /* Int'l VAT,   V3.3 */
        /* not use */
    }
}

/*
*==========================================================================
**    Synopsis: VOID    TrnSalesMod( ITEMSALES *ItemSales )
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by modifier discount                    V3.3
*==========================================================================
*/
VOID    TrnSalesMod(ITEMSALES *ItemSales)
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = TrnSalesItemIsFoodStamp (ItemSales->ControlCode.auchPluStatus);
	
    TrnInformation.TranItemizers.lMI += ItemSales->lDiscountAmount;
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += ItemSales->lDiscountAmount;
        /* --- Taxable #1-3 ---  */
        if ((ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX1) ||
            (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX2) ||
            (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_TAX3)) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += ItemSales->lDiscountAmount;
        }
    }

    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1 ) {
        TrnInformation.TranItemizers.lDiscountable[0] += ItemSales->lDiscountAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] += ItemSales->lDiscountAmount;
        }
    }
    if ( ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2 ) {
        TrnInformation.TranItemizers.lDiscountable[1] += ItemSales->lDiscountAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += ItemSales->lDiscountAmount;
        }
    }

    if ((ItemSales->ControlCode.auchPluStatus[5] & ITM_SALES_HOURLY)
        && !(ItemSales->fbReduceStatus & REDUCE_NOT_HOURLY)) {      /* R3.1 */
        TrnInformation.TranItemizers.lHourly += ItemSales->lDiscountAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        TrnSalesModUSComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnSalesModCanComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                            /* Int'l VAT,   V3.3 */
        TrnSalesModIntlComn(ItemSales, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemDisc( ITEMDISC *ItemDisc )
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by discount                        R3.1
==========================================================================*/

VOID    TrnItemDisc( ITEMDISC *ItemDisc )
{
    switch( ItemDisc->uchMinorClass ) {

    case CLASS_ITEMDISC1 :                                          /* item discount */
        TrnItemDiscount( ItemDisc );
        break;

    case CLASS_REGDISC1 :                                           /* regular discount 1 */
    case CLASS_REGDISC2 :                                           /* regular discount 2 */
    case CLASS_REGDISC3 :                                           /* regular discount 3 */
    case CLASS_REGDISC4 :                                           /* regular discount 4 */
    case CLASS_REGDISC5 :                                           /* regular discount 5 */
    case CLASS_REGDISC6 :                                           /* regular discount 6 */
        TrnRegDiscount( ItemDisc );
        break;

    case CLASS_CHARGETIP :                                          /* charge tip */
    case CLASS_CHARGETIP2 :                                         /* charge tip, V3.3 */
    case CLASS_CHARGETIP3 :                                         /* charge tip, V3.3 */
    case CLASS_AUTOCHARGETIP :                                      /* charge tip, V3.3 */
    case CLASS_AUTOCHARGETIP2 :                                     /* charge tip, V3.3 */
    case CLASS_AUTOCHARGETIP3 :                                     /* charge tip, V3.3 */
        TrnChargeTip( ItemDisc );
        break;

    default :
        break;
    }
}



/*==========================================================================
**    Synopsis: VOID    TrnItemDiscount( ITEMDISC *ItemDisc )
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by item discount,                       V3.3
==========================================================================*/

VOID    TrnItemDiscount( ITEMDISC *ItemDisc )
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (ItemDisc->auchDiscStatus[1] & DISC_AFFECT_FOODSTAMP));

    TrnInformation.TranItemizers.lMI += ItemDisc->lAmount;
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += ItemDisc->lAmount;
        /* --- Taxable #1-3 ---  */
        if ((ItemDisc->auchDiscStatus[0] & 0x04) ||                 /* Tax #1, DISC_AFFECT_TAXABLE1 */
            (ItemDisc->auchDiscStatus[0] & 0x08) ||                 /* Tax #2, DISC_AFFECT_TAXABLE2 */
            (ItemDisc->auchDiscStatus[0] & 0x10)) {                 /* Tax #3, DISC_AFFECT_TAXABLE3 */
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += ItemDisc->lAmount;
        }
    }

    if ( ItemDisc->auchDiscStatus[2] & DISC_STATUS_2_REGDISC1 ) {                     /* regular discountable 1 */
        TrnInformation.TranItemizers.lDiscountable[0] += ItemDisc->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] += ItemDisc->lAmount;
        }
    }
    if ( ItemDisc->auchDiscStatus[2] & DISC_STATUS_2_REGDISC2 ) {                     /* regular discountable 2 */
        TrnInformation.TranItemizers.lDiscountable[1] += ItemDisc->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += ItemDisc->lAmount;
        }
    }
    if ((ItemDisc->auchDiscStatus[0] & DISC_NOTAFFECT_HOURLY) == 0 && !(ItemDisc->fbReduceStatus & REDUCE_NOT_HOURLY)) {
        TrnInformation.TranItemizers.lHourly += ItemDisc->lAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        TrnItemDiscountUSComn(ItemDisc, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnItemDiscountCanComn(ItemDisc, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnItemDiscountIntlComn(ItemDisc, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnRegDiscount(ITEMDISC *pItem)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by regular discount                R3.1
*    TAR191625 Canadian Tax is not correct after voided discount operation.
==========================================================================*/

VOID    TrnRegDiscount(ITEMDISC *pItem)
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (pItem->auchDiscStatus[1] & DISC_AFFECT_REGDISC_FOOD));   // Transaction Discount Food Stamp indicator

	TrnInformation.TranItemizers.lMI += pItem->lAmount;
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += pItem->lAmount;
	}

    if ((pItem->auchDiscStatus[0] & DISC_NOTAFFECT_HOURLY) == 0 && !(pItem->fbReduceStatus & REDUCE_NOT_HOURLY)) {          /* R3.1 */
        TrnInformation.TranItemizers.lHourly += pItem->lAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        if ((pItem->fbDiscModifier & VOID_MODIFIER) == 0) {
            if (pItem->uchRate == 0) {
                TrnRegDiscTaxAmtComn(pItem, 0, TRN_ITEMIZE_NORMAL);
            } else {
                TrnRegDiscTaxRateComn(pItem, 0, TRN_ITEMIZE_NORMAL);
            }
        } else {
			/* TAR191625 */
            TrnRegDiscTaxAmtVoidComn(pItem, 0, TRN_ITEMIZE_NORMAL);
        }

    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
		/* TAR191625 */
        /* if ((pItem->fbModifier & VOID_MODIFIER) == 0) { */
            TrnRegDiscTaxCanComn(pItem, 0, TRN_ITEMIZE_NORMAL);
        /* } */

    } else {                                        /* Int'l VAT,   V3.3 */
        /*-- Update VAT itemizer --*/
        TrnRegDiscVATCalcComn(pItem, 0, TRN_ITEMIZE_NORMAL);
    }

	/* TAR191625 */
	if ((pItem->uchSeatNo == 0) || (pItem->uchSeatNo == 'B')) {	/* Base Transaction */
	    if (!(pItem->fbDiscModifier & VOID_MODIFIER)) {
    		TrnInformation.TranItemizers.lVoidDiscount[pItem->uchMinorClass - CLASS_REGDISC1] += pItem->lAmount;
	    } else {
    		TrnInformation.TranItemizers.lVoidDiscount[pItem->uchMinorClass - CLASS_REGDISC1] = 0;
    	}
    }
}


/*==========================================================================
**    Synopsis: VOID    TrnChargeTip( ITEMDISC *ItemDisc )
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by charge tip
==========================================================================*/

VOID    TrnChargeTip( ITEMDISC *ItemDisc )
{

    TrnInformation.TranItemizers.lMI += ItemDisc->lAmount;

    if (!(ItemDisc->fbReduceStatus & REDUCE_NOT_HOURLY)) {          /* R3.1 */
       TrnInformation.TranItemizers.lHourly += ItemDisc->lAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                         /* V3.3 */
		USHORT    usWorkMod = ItemDisc->fbDiscModifier;

        usWorkMod >>= 1;
        usWorkMod &= TRN_MASKSTATCAN;
        if (usWorkMod == 0) {
            usWorkMod = ItemDisc->auchDiscStatus[1];
            usWorkMod &= TRN_MASKSTATCAN;
			if (usWorkMod > STD_PLU_ITEMIZERS_MOD) {
				usWorkMod = STD_PLU_ITEMIZERS_MOD;
			}
            usWorkMod++;
        }

        TrnInformation.TranItemizers.Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += ItemDisc->lAmount;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemCoupon( ITEMCOUPON *pItemCoupon )
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by combination coupon                 R3.1, V3.3
==========================================================================*/

VOID    TrnItemCoupon(ITEMCOUPON *pItemCoupon)
{
    UCHAR   fchFood = 0;

    /* correct fs affection, 02/14/01 */
	fchFood = (TrnTaxSystem() == TRN_TAX_US && (pItemCoupon->fbStatus[0] & FOOD_MODIFIER));

    TrnInformation.TranItemizers.lMI += pItemCoupon->lAmount;
    /* --- Food Stampable --- */
    if (fchFood) {
        TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodStampable += pItemCoupon->lAmount;
        /* --- Taxable #1-3 ---  */
        if ((pItemCoupon->fbStatus[0] & TAX_MODIFIER1) ||           /* Tax #1 */
            (pItemCoupon->fbStatus[0] & TAX_MODIFIER2) ||           /* Tax #2 */
            (pItemCoupon->fbStatus[0] & TAX_MODIFIER3)) {           /* Tax #3 */
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFoodWholeTax += pItemCoupon->lAmount;
        }
    }

    /* --- update discountable itemizer # 1 , 2 --- */
    if ( pItemCoupon->fbStatus[ 1 ] & ITM_COUPON_AFFECT_DISC1 ) {
        TrnInformation.TranItemizers.lDiscountable[ 0 ] += pItemCoupon->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[0] += pItemCoupon->lAmount;
        }
    }
    if ( pItemCoupon->fbStatus[ 1 ] & ITM_COUPON_AFFECT_DISC2 ) {
        TrnInformation.TranItemizers.lDiscountable[ 1 ] += pItemCoupon->lAmount;
        if (fchFood) {
            TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lFSDiscountable[1] += pItemCoupon->lAmount;
        }
    }

    /* --- update hourly itemizer --- */
    if (( pItemCoupon->fbStatus[ 1 ] & ITM_COUPON_HOURLY ) && !(pItemCoupon->fbReduceStatus & REDUCE_NOT_HOURLY)) {/* R3.1 */
        TrnInformation.TranItemizers.lHourly += pItemCoupon->lAmount;

        /* affect to hourly bonus total, V3.3 */
        if (pItemCoupon->usBonusIndex) {
            TrnInformation.TranItemizers.lHourlyBonus[(pItemCoupon->usBonusIndex - 1)] += pItemCoupon->lAmount; 
        }
    }

    if (!(pItemCoupon->fbReduceStatus & REDUCE_ITEM) && pItemCoupon->usBonusIndex) {
        TrnInformation.TranItemizers.lTransBonus[(pItemCoupon->usBonusIndex - 1)] += pItemCoupon->lAmount; 
    }

    if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
        /* --- update taxable itemizers --- */
        TrnItemCouponUSComn(pItemCoupon, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
        TrnItemCouponCanComn(pItemCoupon, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                    /* Int'l VAT,   V3.3 */
        TrnItemCouponIntlComn(pItemCoupon, &TrnInformation.TranItemizers, TRN_ITEMIZE_NORMAL);
    }
}

/*
*==========================================================================
**    Synopsis: VOID    TrnItemMisc(ITEMMISC *pItem)
*
*    Input:     ITEMMISC    *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Save Money Amount
*==========================================================================
*/
VOID    TrnItemMisc(ITEMMISC *pData)
{
    TRANITEMIZERS   *pItem = &TrnInformation.TranItemizers;

    if (pData->uchMinorClass == CLASS_MONEY) {
        switch (pData->uchTendMinor) {
        case    CLASS_TENDER1:
        case    CLASS_TENDER2:
        case    CLASS_TENDER3:
        case    CLASS_TENDER4:
        case    CLASS_TENDER5:
        case    CLASS_TENDER6:
        case    CLASS_TENDER7:
        case    CLASS_TENDER8:
        case    CLASS_TENDER9:
        case    CLASS_TENDER10:
        case    CLASS_TENDER11:
        case    CLASS_TENDER12:
        case    CLASS_TENDER13:
        case    CLASS_TENDER14:
        case    CLASS_TENDER15:
        case    CLASS_TENDER16:
        case    CLASS_TENDER17:
        case    CLASS_TENDER18:
        case    CLASS_TENDER19:
        case    CLASS_TENDER20:
            /* ----- local tender on hand total  ----- */
            pItem->lTenderMoney[pData->uchTendMinor - CLASS_TENDER1] += pData->lAmount;
            break;

		case	CLASS_FOREIGN1:
		case	CLASS_FOREIGN2:
		case	CLASS_FOREIGN3:
		case	CLASS_FOREIGN4:
		case	CLASS_FOREIGN5:
		case	CLASS_FOREIGN6:
		case	CLASS_FOREIGN7:
		case	CLASS_FOREIGN8:
            /* ----- FC tender on hand total  ----- */
            pItem->lFCMoney[pData->uchTendMinor - CLASS_FOREIGN1] += pData->lAmount;
            break;

        default:
			NHPOS_ASSERT_TEXT(0, "**ERROR: TrnItemMisc() unknown tender type.");
            break;
        }
    }
}


/*==========================================================================
**    Synopsis: VOID    TrnItemTotal( ITEMTOTAL *pItemTotal )
*
*    Input:     ITEMTOTAL *pItemTotal
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   save total amount at store/recall system
==========================================================================*/

VOID    TrnItemTotal( ITEMTOTAL *pItemTotal )
{
    ULONG   fulGCSystem;

    /* --- determine current sales system is store/recall or not --- */
    fulGCSystem = TrnInformation.TranCurQual.flPrintStatus;
#if 0
	// this source is removed with a change made to function ItemTotalCTAmount()
	// so that the calculation of the value of pItemTotal->lMI is changed.
	// we no longer want to update the lMI itemizer with the current total
	// as in some cases multiple presses of a total key would result in the
	// value of pItemTotal->lMI and the value of TrnInformation.TranItemizers.lMI
	// changing each other through multiple key presses resulting in the working
	// or current subtotal being slowly decremented each time by the ammount in
	// ITEMTENDERLOCAL WorkTend.lTenderizer in the case of split tenders.
	//    Richard Chambers for Amtrak project, June 25, 2013
	//
	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
    if (( fulGCSystem & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL ) {
		UCHAR   uchTotalType;

        /* --- save total amount to main itemizer, if this total key is
            check total, non-finalize total --- */
        switch ( pItemTotal->uchMinorClass ) {
        case CLASS_TOTAL2:          /* check total */
            TrnInformation.TranItemizers.lMI = pItemTotal->lMI;
            break;

        case CLASS_TOTAL3:
        case CLASS_TOTAL4:
        case CLASS_TOTAL5:
        case CLASS_TOTAL6:
        case CLASS_TOTAL7:
        case CLASS_TOTAL8:
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

            uchTotalType = pItemTotal->auchTotalStatus[ 0 ];

            switch ( ITM_TTL_GET_TTLTYPE( uchTotalType )) {

            case ITM_TTL_NOFINALIZE:    /* non-finalize total */
                TrnInformation.TranItemizers.lMI = pItemTotal->lMI;
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }
#endif
}

/*==========================================================================
**    Synopsis: VOID    TrnItemAC( ITEMAFFECTION *ItemAffection )
*
*    Input:     ITEMAFFECTION   *ItemAffection
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by manual addcheck
==========================================================================*/

VOID    TrnItemAC( ITEMAFFECTION *ItemAffection )
{
    if ( ItemAffection->uchMinorClass == CLASS_MANADDCHECKTOTAL ) {     /* manual addcheck */
        TrnInformation.TranItemizers.lMI += ItemAffection->lAmount;     /* main itemizer */
        if (TrnTaxSystem() == TRN_TAX_US) {                             /* V3.3 */
			USHORT  usWorkMDC = ItemAffection->uchAffecMDCstatus;       /* MDC. see ItemTransACManAff() */
			USHORT  usWorkMod = ItemAffection->fbModifier;              /* modifier */
			USHORT  usWorkBit;
			SHORT   i;

			usWorkBit = 0x02;            // affect TAX_MODIFIER1 indicator
            for ( i = 0; i < STD_TAX_ITEMIZERS_US; i++, usWorkBit <<= 1 ) {
                if ( ( usWorkMDC ^ usWorkMod ) & usWorkBit ) {       /* check taxable */
                    TrnInformation.TranItemizers.Itemizers.TranUsItemizers.lNonAffectTaxable[i] += ItemAffection->lAmount;
                }
            }
        }
    }
}


/*
*==========================================================================
**    Synopsis: SHORT TrnStoCouponSearch( ITEMCOUPONSEARCH *pItemOrderDecSearch,
*                                         SHORT             sStorageType )
*
*       Input:  ITEMCOUPONSEARCH *pItemOrderDecSearch - coupon item structure
*               SHORT             sStorageType   - type of trans storage
*                       TRN_TYPE_ITEMSTORAGE, TRN_TYPE_CONSSTORAGE
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    TRN_SUCCESS     : item found and consolidate it
*               TRN_ERROR       : not found
*
**  Description:
*           coupon search for target "sales item" in item/consoli. storage
*==========================================================================
*/

SHORT TrnStoOrderDecSearch( ITEMSALES *pItemOrderDecSearch,
                          SHORT             sStorageType )
{
    SHORT   sReturn;
    USHORT  usAllItemFound;
    SHORT   hsStorage;
    USHORT  usStorageVli;
    USHORT  usStorageHdrSize;
    ULONG   ulMaxStoSize;
    ULONG   ulReadPosition;
    UCHAR   auchTranStorageWork[ TRN_STOSIZE_BLOCK ];
    TRANSTORAGESALESNON *pTran;

    sReturn = TrnStoGetStorageInfo( sStorageType, &hsStorage, &usStorageHdrSize, &usStorageVli );
    if ( sReturn != TRN_SUCCESS ) {
        return ( sReturn );
    }

	ulReadPosition  = usStorageHdrSize;
    ulMaxStoSize    = ulReadPosition + usStorageVli;
    usAllItemFound  = FALSE;

    pTran = TRANSTORAGENONPTR(auchTranStorageWork);

    /* --- retrieve item data which is stored in transaction storage --- */
    while ( ulReadPosition < ulMaxStoSize ) {
		USHORT  usTargetItemSize;

        /* --- read item data, and store to work buffer --- */
		sReturn = TrnStoIsItemInWorkBuff(hsStorage, ulReadPosition, auchTranStorageWork, sizeof( auchTranStorageWork ), &usTargetItemSize);
		// RJC temporary work around as a zero value resulting in an infinite loop has been seen in Rel 2.1
		if (usTargetItemSize < 1)
			break;

		if (sReturn == TRN_ERROR) {
			NHPOS_ASSERT_TEXT((sReturn != TRN_ERROR), "**ERROR: TrnStoIsItemInWorkBuff() item record size exceeds buffer");
			break;
		}

		/* --- determine target item is sales item or not --- */
		if ( pTran->uchMajorClass == CLASS_ITEMSALES) {
			if (pTran->uchMinorClass == CLASS_ITEMORDERDEC ) {
				// if this is an Order Declare item then uncompress it. even with RFL_WITHOUT_MNEM
				// specified, we will still get the Order Declare mnemonic since that is stored with
				// the rest of the item data and does not require a lookup.
				RflGetStorageItem( pItemOrderDecSearch, auchTranStorageWork, RFL_WITHOUT_MNEM );
			}
			if (RflIsSalesItemDisc(pTran)) {
				// if this is a sales item with a following discount then lets take the length of the discount
				// data into consideration in order to move to the next transaction data record.
				USHORT usTemp = RflGetStorageItemLen (auchTranStorageWork + usTargetItemSize, -1);

				usTargetItemSize += usTemp;
			}
        }

        /* --- increment read position to read next work buffer --- */
        ulReadPosition += usTargetItemSize;
    }

	usAllItemFound = (pItemOrderDecSearch->aszMnemonic[0] != 0);

    /* --- return success, if all of coupon target item is found --- */
    return (( usAllItemFound ) ? ( TRN_SUCCESS ) : ( TRN_ERROR ));
}
/****** End of Source ******/