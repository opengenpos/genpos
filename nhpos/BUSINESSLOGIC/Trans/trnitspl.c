/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION ITEMIZERS MODULE
:   Category       : TRANSACTION MODULE, NCR 2170 US Hospitality Application
:   Program Name   : TRNITSPL.C
:  ---------------------------------------------------------------------
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
:   Memory Model       : Medium Model
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp
:  ---------------------------------------------------------------------
:  Abstract:
:
:  ---------------------------------------------------------------------
:  Update Histories
:  Date     : Ver.Rev. :   Name     : Description
: Nov-27-95 : 03.01.00 :  hkato     : R3.1
: Aug-18-98 : 03.03.00 :  hrkato    : V3.3 (VAT/Service)
: Aug-13-99 : 03.05.00 :  K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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

#include    <ecr.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <rfl.h>
#include    <pif.h>


/*==========================================================================
**    Synopsis: VOID    TrnItemSales(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by sales
==========================================================================*/

VOID    TrnItemSalesSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    switch(ItemSales->uchMinorClass) {
    case CLASS_DEPT:
    case CLASS_DEPTITEMDISC:
        TrnSalesDeptSpl(ItemSales, sType);
        break;

    case CLASS_PLU:
    case CLASS_PLUITEMDISC:
    case CLASS_SETMENU:
    case CLASS_SETITEMDISC:
    case CLASS_OEPPLU:
    case CLASS_OEPITEMDISC:
        TrnSalesPLUSpl(ItemSales, sType);
        break;

    case CLASS_DEPTMODDISC:
    case CLASS_PLUMODDISC:
    case CLASS_SETMODDISC:
    case CLASS_OEPMODDISC:
        TrnSalesModSpl(ItemSales, sType);
        break;

    default:
        break;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSalesDeptSpl(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by dept, set menu
==========================================================================*/

VOID    TrnSalesDeptSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += ItemSales->lProduct;

    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        pSpl->TranItemizers.lDiscountable[0] += ItemSales->lProduct;
    }
    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        pSpl->TranItemizers.lDiscountable[1] += ItemSales->lProduct;
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnSalesDeptCanComn(ItemSales, &pSpl->TranItemizers, &pSpl->TranGCFQual, TRN_ITEMIZE_NORMAL);

        /*----- Discount/Taxable Itemizer -----*/
    } else if (TrnTaxSystem() == TRN_TAX_US) {                      /* V3.3 */
        TrnSalesDeptUSComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);

    } else {                                        /* Int'l VAT,   V3.3 */
        TrnSalesDeptIntlComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSalesPLUSpl(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by PLU, promotional PLU
==========================================================================*/

VOID    TrnSalesPLUSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    USHORT  i;
    USHORT  usMaxChild;
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);
	TCHAR   auchDummy[NUM_PLU_LEN] = {0};

    pSpl->TranItemizers.lMI += ItemSales->lProduct;

    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        pSpl->TranItemizers.lDiscountable[0] += ItemSales->lProduct;
    }
    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        pSpl->TranItemizers.lDiscountable[1] += ItemSales->lProduct;
    }

    /* --- itemize condiment PLU --- */
    usMaxChild = (ItemSales->uchChildNo + ItemSales->uchCondNo + ItemSales->uchPrintModNo);

	NHPOS_ASSERT(usMaxChild <= sizeof(ItemSales->Condiment)/sizeof(ItemSales->Condiment[0]));

    /* link plu, saratoga */
    if (ItemSales->usLinkNo) {
        i = 0;
    } else {
        i = ItemSales->uchChildNo;
    }
    for (; i < usMaxChild; i++) {
		DCURRENCY    lChildAmount;

        if ( _tcsncmp(ItemSales->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0) {
            continue;
        }
        lChildAmount = (DCURRENCY)TrnQTY (ItemSales) * ItemSales->Condiment[i].lUnitPrice;

        pSpl->TranItemizers.lMI += lChildAmount;
        if (ItemSales->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
            pSpl->TranItemizers.lDiscountable[0] += lChildAmount;
        }
        if (ItemSales->Condiment[i].ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
            pSpl->TranItemizers.lDiscountable[1] += lChildAmount;
        }
    }

    TrnTaxableSpl(ItemSales, sType);
    TrnDiscTaxSpl(ItemSales, sType);
}

/*==========================================================================
**    Synopsis: VOID    TrnTaxableSpl(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update taxable itemizer
==========================================================================*/

VOID    TrnTaxableSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                         /* V3.3 */
        TrnSalesDeptCanComn(ItemSales, &pSpl->TranItemizers, &pSpl->TranGCFQual, TRN_ITEMIZE_NORMAL);
        TrnTaxableCanComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                      /* V3.3 */
        TrnTaxableUSComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnTaxableIntlComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnDiscTaxSpl(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   update discountable/taxable itemizer
==========================================================================*/

VOID    TrnDiscTaxSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                         /* V3.3 */
        TrnDiscTaxCanComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                      /* V3.3 */
        TrnDiscTaxUSComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                            /* Int'l VAT,   V3.3 */
        /* not use */
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnSalesModSpl(ITEMSALES *ItemSales, TrnStorageType sType)
*
*    Input:     ITEMSALES   *ItemSales
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by modifier discount
==========================================================================*/

VOID    TrnSalesModSpl(ITEMSALES *ItemSales, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += ItemSales->lDiscountAmount;

    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC1) {
        pSpl->TranItemizers.lDiscountable[0] += ItemSales->lDiscountAmount;
    }
    if (ItemSales->ControlCode.auchPluStatus[1] & PLU_AFFECT_DISC2) {
        pSpl->TranItemizers.lDiscountable[1] += ItemSales->lDiscountAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                         /* V3.3 */
        TrnSalesModCanComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);

    } else if (TrnTaxSystem() == TRN_TAX_US) {                      /* V3.3 */
        TrnSalesModUSComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);

    } else {                                            /* Int'l VAT,   V3.3 */
        TrnSalesModIntlComn(ItemSales, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemCouponSpl(ITEMCOUPON *pItem, TrnStorageType sType)
*
*    Input:     ITEMCOUPON  *pItemCoupon
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by combination coupon
==========================================================================*/

VOID    TrnItemCouponSpl(ITEMCOUPON *pItem, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += pItem->lAmount;

    if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC1) {
        pSpl->TranItemizers.lDiscountable[0] += pItem->lAmount;
    }
    if (pItem->fbStatus[1] & ITM_COUPON_AFFECT_DISC2) {
        pSpl->TranItemizers.lDiscountable[1] += pItem->lAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnItemCouponCanComn(pItem, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        /* --- update taxable itemizers --- */
        TrnItemCouponUSComn(pItem, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                        /* Int'l VAT,   V3.3 */
        TrnItemCouponIntlComn(pItem, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemDiscSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by discount
==========================================================================*/

VOID    TrnItemDiscSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
{
    switch(ItemDisc->uchMinorClass) {
    case CLASS_ITEMDISC1:
        TrnItemDiscountSpl(ItemDisc, sType);
        break;

    case CLASS_REGDISC1:
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        TrnRegDiscountSpl(ItemDisc, sType);
        break;

    case CLASS_CHARGETIP:
    case CLASS_CHARGETIP2:  /* V3.3 */
    case CLASS_CHARGETIP3:
    case CLASS_AUTOCHARGETIP:
    case CLASS_AUTOCHARGETIP2:
    case CLASS_AUTOCHARGETIP3:
        TrnChargeTipSpl(ItemDisc, sType);
        break;

    default:
        break;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemDiscountSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by item discount
==========================================================================*/

VOID    TrnItemDiscountSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
{
/*    UCHAR   uchWorkMDC, uchWorkBit; */
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += ItemDisc->lAmount;

    if (ItemDisc->auchDiscStatus[2] & DISC_STATUS_2_REGDISC1) {
        pSpl->TranItemizers.lDiscountable[0] += ItemDisc->lAmount;
    }
    if (ItemDisc->auchDiscStatus[2] & DISC_STATUS_2_REGDISC2) {
        pSpl->TranItemizers.lDiscountable[1] += ItemDisc->lAmount;
    }

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
        TrnItemDiscountCanComn(ItemDisc, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        TrnItemDiscountUSComn(ItemDisc, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
    } else {                                            /* Int'l VAT,   V3.3 */
        TrnItemDiscountIntlComn(ItemDisc, &pSpl->TranItemizers, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnRegDiscountSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by regular discount
==========================================================================*/

VOID    TrnRegDiscountSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += ItemDisc->lAmount;

    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
		/* TAR191625 */
        /* if ((ItemDisc->fbModifier & VOID_MODIFIER) == 0) { */
            TrnRegDiscTaxCanComn(ItemDisc, sType, TRN_ITEMIZE_NORMAL);
        /*}*/

    } else if (TrnTaxSystem() == TRN_TAX_US) {                  /* V3.3 */
        if ((ItemDisc->fbDiscModifier & VOID_MODIFIER) == 0) {
            if (ItemDisc->uchRate == 0) {
                TrnRegDiscTaxAmtComn(ItemDisc, sType, TRN_ITEMIZE_NORMAL);
            } else {
                TrnRegDiscTaxRateComn(ItemDisc, sType, TRN_ITEMIZE_NORMAL);
            }
        } else {
			/* TAR191625 */
            TrnRegDiscTaxAmtVoidComn(ItemDisc, sType, TRN_ITEMIZE_NORMAL);
        }

    } else {                                    /* Int'l VAT,   V3.3 */
        TrnRegDiscVATCalcComn(ItemDisc, sType, TRN_ITEMIZE_NORMAL);
        /* Kato */
    }

	/* TAR191625 */
	if ((ItemDisc->uchSeatNo == 0) || (ItemDisc->uchSeatNo == 'B')) {	/* Base Transaction */
	    if (!(ItemDisc->fbDiscModifier & VOID_MODIFIER)) {
    		pSpl->TranItemizers.lVoidDiscount[ItemDisc->uchMinorClass - CLASS_REGDISC1] += ItemDisc->lAmount;
	    } else {
    		pSpl->TranItemizers.lVoidDiscount[ItemDisc->uchMinorClass - CLASS_REGDISC1] = 0;
    	}
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnChargeTipSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
*
*    Input:     ITEMDISC    *ItemDisc
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   itemize by charge tip
==========================================================================*/

VOID    TrnChargeTipSpl(ITEMDISC *ItemDisc, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    pSpl->TranItemizers.lMI += ItemDisc->lAmount;
    if (TrnTaxSystem() == TRN_TAX_CANADA) {                     /* V3.3 */
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

        pSpl->TranItemizers.Itemizers.TranCanadaItemizers.lTaxable[usWorkMod - 1] += ItemDisc->lAmount;
    }
}

/*==========================================================================
**    Synopsis: VOID    TrnItemAffectionSpl(ITEMAFFECTION *pItem, TrnStorageType sType)
*
*    Input:     ITEMAFFECTION   *ItemAffect
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Iitemize by Tax(Multi Seat# Tender)
==========================================================================*/

VOID    TrnItemAffectionSpl(ITEMAFFECTION *pItem, TrnStorageType sType)
{
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sType);

    if (pItem->uchMinorClass == CLASS_TAXAFFECT) {
		SHORT   i, j;
        pSpl->TranItemizers.lMI += pItem->USCanVAT.USCanTax.lTaxAmount[0]
            + pItem->USCanVAT.USCanTax.lTaxAmount[1] + pItem->USCanVAT.USCanTax.lTaxAmount[2]
            + pItem->USCanVAT.USCanTax.lTaxAmount[3];

        if (TrnTaxSystem() == TRN_TAX_US) {                     /* V3.3 */
            for (i = 0; i < STD_TAX_ITEMIZERS_MAX; i++) {
                pSpl->TranItemizers.Itemizers.TranUsItemizers.lNonAffectTaxable[i] = 0L;
            }
			// zero both first and second half of the array where discount #1 and #2 totals are maintained.
            for (i = 0; i < STD_TAX_ITEMIZERS_MAX * 2; i++) {
                pSpl->TranItemizers.Itemizers.TranUsItemizers.lDiscTaxable[i] = 0L;
            }

        } else if (TrnTaxSystem() == TRN_TAX_CANADA) {                  /* V3.3 */
            for (i = 0; i < STD_PLU_ITEMIZERS; i++) {
                pSpl->TranItemizers.Itemizers.TranCanadaItemizers.lTaxable[i] = 0L;
            }
            for (i = 0; i < 2; i++) {
                for (j = 0; j < STD_PLU_ITEMIZERS+2+3; j++) {    // should this be STD_PLU_ITEMIZERS+2+3 rather than 9+2+3?
                    pSpl->TranItemizers.Itemizers.TranCanadaItemizers.lDiscTaxable[i][j] = 0L;
                }        
            }
        }
    } else if (pItem->uchMinorClass == CLASS_SERVICE_VAT ||
        pItem->uchMinorClass == CLASS_VATAFFECT ||
        pItem->uchMinorClass == CLASS_SERVICE_NON) {
            /* Kato */
    }
}



/****** End of Source ******/
