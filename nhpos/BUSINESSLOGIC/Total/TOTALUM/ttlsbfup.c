/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update Module Subroutine
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: Ttlsbfuf.c
* --------------------------------------------------------------------------
* Abstract:
*           VOID TtlFinupdate()     - Affect Financial Total
*           VOID TtlFSales()        - Affect Financial Total in Sales Data
*           VOID TtlFModDiscount()  - Affect Financial Total in Modifier Discount Data
*           VOID TtlFDiscount()     - Affect Financial Total in Discount Data
*           VOID TtlFCpn()          - Affect Financial Total in Coupon Data,  R3.0
*           VOID TtlFmisc()         - Affect Financial Total in Misc Data
*           VOID TtlFAffect()       - Affect Financial Total in Affction Data
*           VOID TtlUpFinCGGTotal() - Affect Gross Group Total of Financial
*           VOID TtlUpFinTaxable()  - Affect Tax Total of Financial
*           VOID TtlUpFinTGGTotal() - Affect Training Gross Group Total of Financial
*           VOID TtlUpFinDeclaredTips() - Affect Declared Tips Total of Financial 
*           VOID TtlUpFinDGGTotal() - Affect Daily Gross Group Total of Financial
*           VOID TtlUpFinPlusVoid() - Affect Plus Void Total of Financial
*           VOID TtlUpFinPreVoid()  - Affect Preselect Void Total of Financial
*           VOID TtlUpFinConsCoupon() - Affect Consolidated Coupon Total of Financial
*           VOID TtlUpFinItemDisc() - Affect Item Discount Total of Financial
*           VOID TtlUpFinModiDisc() - Affect Modifier Discount Total of Financial
*           VOID TtlUpFinRegDisc()  - Affect Regular Discount Total of Financial
*           VOID TtlUpFinCpn()      - Affect Coupon Total of Financial,   R3.0
*           VOID TtlUpFinPaidOut()  - Affect Paid Out Total of Financial
*           VOID TtlUpFinRecvAcount() - Affect Received on Account Total of Financial
*           VOID TtlUpFinTipsPaid()  - Affect Tips Paid Out Total of Financial
*           VOID TtlUpFinCashTender() - Affect Cash Tender Total of Financial
*           VOID TtlUpFinCheckTender() - Affect Check Tender Total of Financial
*           VOID TtlUpFinChargeTender() - Affect Charge Tender Total of Financial
*           VOID TtlUpFinMiscTender() - Affect Misc Tender Total of Financial
*           VOID TtlUpFinForeignTotal() - Affect Foregin Total of Financial
*           VOID TtlUpFinServiceTotal() - Affect Service Total of Financial
*           VOID TtlUpFinAddCheckTotal() - Affect AddCheck Total of Financial
*           VOID TtlUpFinChargeTips() - Affect Charge Tips Total of Financial
*           VOID TtlUpFinTransCancel() - Affect Transaction Cancel Total of Financial
*           VOID TtlUpFinMiscVoid() - Affect Misc Void Total of Financial
*           VOID TtlUpFinAudaction() - Affect Audaction Total of Financial
*           VOID TtlUpFinNoSaleCount() - Affect Nosale of Financial
*           VOID TtlUpFinItemProduct() - Affect Item Productivity of Financial
*           VOID TtlUpFinNoOfPerson()  - Affect No of Person of Finanicial
*           VOID TtlUpFinNoOfChkOpen() - Affect No Check Open of Financial
*           VOID TtlUpFinNoOfChkClose() - Affect No of Check Close of Financial
*           VOID TtlUpFinCustomer() - Affect Customer Counter of Finanicial
*           VOID TtlUpFinHashDept() - Affect Hash Dept Total of Financial
*           VOID TtlUpFinBonus()    - Affect Bonus Total of Financial
*           VOID TtlUpFinConsTax()  - Affect Consolidate Tax Total of Financial
*           VOID TtlUpFinNetTotal() - Affect Net Toal of Financial,     R3.1
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:Kterai     : 
* Dec-03-93:02.00.03:K.You      : add 4 tender total
* Apr-08-94:00.00.04: Yoshiko.J : delete OffTend, add Offline element
* Apr-19-94:00.00.05:K.You      : bug fixed (E-26)(mod. TtlUpFinMiscTender)
* Mar-03-95:03.00.00:hkato      : R3.0
* Nov-16-95:03.00.01:T.Nakahata : Bug Fixed (not affect coupon at training)
* Dec-26-95:03.01.00:T.Nakahata : R3.1 Initial
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   TTLCSREGFIN data => TTLCSREGFIN FAR data
* Jan-30-96:03.01.00:hkato      : Regular Discount(w/Total Key).
* Nov-30-99:01.00.00:hrkato     : Saratoga
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
#include <memory.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "pif.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "csttl.h"
#include "ttl.h"
#include "regstrct.h"
#include "transact.h"
#include "ttltum.h"

/*
*============================================================================
**Synopsis:     VOID TtlFinupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                 TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                 TTLCSREGFIN FAR *pTmpBuff);
*
*    Input:     TTLTUNTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                  Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSREGFIN FAR *pTmpBuff        - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff        - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function affects Financial Total.            R3.1
*============================================================================
*/
VOID TtlFinupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                  TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN  *pTmpBuff)
{
	USHORT      fbMod = 0;
    SHORT       sSign;              /* Buffer for Calculate Sign */
    DCURRENCY   lAmount;            /* Buffer for Calculate Amount */
    SHORT       sCounter;           /* Buffer for Counter */
    USHORT      usStatus;           /* Buffer for Affection Status */
    UCHAR       i;
    TCHAR       auchDummy[NUM_PLU_LEN] = {0};

    usStatus = 0;                   /* Set Status */
                                    /* Preselect Void (TTL_TUP_PVOID) */
                                    /* Training       (TTL_TUP_TRAIN) */
                                    /* Momentary Void (TTL_TUP_MVOID) */
    sSign = 1;                      /* Set Sign */
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {
                                    /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
        usStatus |= TTL_TUP_IGNORE;   /* Set ignore data status to be reset only if this ia returned item */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {
                                    /* Check Training Operation */
        usStatus |= TTL_TUP_TRAIN;  /* Set Training Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_ADDCHECK_SERVT
        && pTtlTranUpBuff->TtlClass.uchMajorClass == CLASS_ITEMAFFECTION
        && pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_ADDCHECKTOTAL) {
        return;
    }

    switch(pTtlTranUpBuff->TtlClass.uchMajorClass) {
/* -------------- Transaction Open Affection --------------- */
    case CLASS_ITEMTRANSOPEN:       
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NEWCHECK:                  /* New Check */
            TtlUpFinNoOfChkOpen(pTmpBuff, 1);    /* Affect No of Checks Opened Counter */
            break;                               /* End Affection */
/*      default:    Not Used */
        }
        return;
   
/* -------------- Sales(Dept/PLU) Affection --------------- */
/* -------------- Modifier Discount Affection --------------- */
    case CLASS_ITEMSALES:           /* sales */
		fbMod = pTtlTranUpBuff->TtlItemSales.fbModifier;
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) {                 /* Check VOID Modifier */
            sSign *= -1;                      /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemSales.fbModifier;
			} else {
				usStatus |= TTL_TUP_MVOID;        /* Set Momentary Void Status */
			}
		} else {
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemSales.fbModifier;
			}
		}

		if (pTtlTranUpBuff->TtlItemSales.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        /* ----- Sales Affection ----- */
        case CLASS_DEPT:            /* Dept */
        case CLASS_DEPTITEMDISC:    /* Dept W/Item Discount */
        case CLASS_PLU:             /* PLU */
        case CLASS_PLUITEMDISC:     /* PLU W/Item Discount */
        case CLASS_SETMENU:         /* Set Menu */
        case CLASS_SETITEMDISC:     /* Set Menu W/Item Discount */
        case CLASS_OEPPLU:          /* OEP Menu,                  R3.0 */
        case CLASS_OEPITEMDISC:     /* OEP Menu W/Item Discount,  R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {                  /* Check Credit Dept/PLU Status */
                usStatus |= TTL_TUP_CREDT;                /* Set Credit Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {                   /* Check Hash Dept/PLU Status */
                usStatus |= TTL_TUP_HASH;                 /* Set Hash Dept/PLU Status. See the comments titled What is a Hash Department in rptdept.c */
            }
            if ((pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_HASH_FINANC) == 0) {     /* Check Hash Dept/PLU Not Affection Status */
                usStatus |= TTL_TUP_HASHAFFECT;           /* Set Hash Dept/PLU Not Affection Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                                    /* Check Scalable */
                if (pTtlTranUpBuff->TtlItemSales.lQTY < 0) {
                    sCounter = -1;                                                 /* Set Quantity is -1 */
                } else {
                    sCounter = 1;                                                  /* Set Quantity is 1 */
                }
            } else {
                sCounter = (SHORT)(pTtlTranUpBuff->TtlItemSales.lQTY / 1000L);     /* Set Quantity */
            }
					//SR47 Affect Double/Triple Coupon to Bonus
			if((ParaMDCCheck(MDC_DOUBLE2_ADR, ODD_MDC_BIT0) || ParaMDCCheck(MDC_TRIPLE2_ADR, ODD_MDC_BIT0)) && (pTtlTranUpBuff->TtlItemSales.lDiscountAmount < 0)){
				if (pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex > 0)
					pTmpBuff->Bonus[(pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex - 1)].lAmount += pTtlTranUpBuff->TtlItemSales.lDiscountAmount;
			}

            TtlFSales(fbMod, usStatus, pTtlTranUpBuff->TtlItemSales.lProduct, sCounter, (USHORT)(pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex - 1), pTmpBuff);
                                    /* Update Main Product */
            if (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPT || pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPTITEMDISC) {
                /* Check Dept Menu Affection */
                break;
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                                    /* Check Dept Menu Affection */
                                    /* Check Scale Item */
                if (pTtlTranUpBuff->TtlItemSales.usLinkNo) {    /* saratoga */
                    if (sCounter >= 0) {
                        sCounter = 1L;
                    } else {
                        sCounter = -1L;
                    }
                } else {
                    break;
                }
            }
            for (i = 0; i < (pTtlTranUpBuff->TtlItemSales.uchCondNo
                 + pTtlTranUpBuff->TtlItemSales.uchChildNo           /* R3.0 */
                 + pTtlTranUpBuff->TtlItemSales.uchPrintModNo); i++) {
                if ( _tcsncmp(pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) {   /* 2172 */
                    continue;
                }
                usStatus &= ~TTL_TUP_CREDT;
                                    /* Reset Credit Status */
                lAmount = (DCURRENCY)sSign * labs((LONG)sCounter) * (DCURRENCY)pTtlTranUpBuff->TtlItemSales.Condiment[i].lUnitPrice;    /* Calculate Condiment Product */
                if (pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[0] & PLU_MINUS) {
                    usStatus |= TTL_TUP_CREDT;        /* Set Credit Dept/PLU Status */
                }
                if ((pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[5] & ITM_HASH_FINANC) == 0) {          /* Check Hash Dept/PLU Not Affection Status */
                    usStatus &= ~TTL_TUP_CREDT;       /* Reset Credit Dept/PLU Status */
                }
                TtlFSales(fbMod, usStatus, lAmount, sCounter, (USHORT)(pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.usBonusIndex - 1),pTmpBuff);    /* Update Condiment Product */
            }
            break;                  /* End Affection */

        /* ----- Modifier Discount Affection ----- */
        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,    R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_MOD_DISC_SUR) {                         /* Check Surcharge Operation */
                usStatus |= TTL_TUP_SURCHARGE;              /* Set Surcharge Status */
            }
            TtlFModDiscount(usStatus, pTtlTranUpBuff->TtlItemSales.lDiscountAmount, sSign, pTmpBuff);     /* Update Modifier Discount Product */
            break;                  /* End Affection */
	/*      default:    Not Used */
        }
        return;

	/* -------------- Discount Affection --------------- */
    case CLASS_ITEMDISC:            /* Discount */
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) {    /* Check VOID Modifier */
            sSign *= -1;                      /* Set Sign */
			if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemDisc.fbDiscModifier;
			} else {
				usStatus |= TTL_TUP_MVOID;        /* Set Momentary Void Status */
			}
		} else {
			if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemDisc.fbDiscModifier;
			}
        }

		if (pTtlTranUpBuff->TtlItemDisc.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */
		}

		if (usStatus & TTL_TUP_IGNORE)
			break;   // skip this discount data

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_ITEMDISC1:       /* Item Discount */
			{
				int iDiscNo = pTtlTranUpBuff->TtlItemDisc.uchDiscountNo;
				SHORT  sCounter = (SHORT)(pTtlTranUpBuff->TtlItemDisc.lQTY * sSign);

				// Item Discount #1 or Item Discount #3 through #6 (the new item discounts added for Rel 2.x increasing from 2 discounts to 6).
				// Item Discount #3 through #6 are a subtype of Item Discount #1.
				if (iDiscNo < 2)
					TtlUpdateDiscountTotal (&(pTmpBuff->ItemDisc), pTtlTranUpBuff->TtlItemDisc.lAmount, sCounter);      /* Affect Item Discount Total/Counter */
				else if (iDiscNo < 3)
					;
				else if (iDiscNo < 7)
					TtlUpdateDiscountTotal (&(pTmpBuff->ItemDiscExtra.TtlIAmount[iDiscNo - 3]), pTtlTranUpBuff->TtlItemDisc.lAmount, sCounter);      /* Affect Item Discount Total/Counter */
			}
			// continue on down to do the surcharge update
        case CLASS_REGDISC1:        /* Regular Discount 1 */
        case CLASS_REGDISC2:        /* Regular Discount 2 */
        case CLASS_REGDISC3:        /* Regular Discount 3,  R3.1 */
        case CLASS_REGDISC4:        /* Regular Discount 4,  R3.1 */
        case CLASS_REGDISC5:        /* Regular Discount 5,  R3.1 */
        case CLASS_REGDISC6:        /* Regular Discount 6,  R3.1 */
            if (pTtlTranUpBuff->TtlItemDisc.auchDiscStatus[1] & TTL_MDC_SURCHARGE) {    /* Check Surcharge Operation */
                usStatus |= TTL_TUP_SURCHARGE;                                      /* Set Surcharge Status */
            }
            TtlFDiscount(fbMod, usStatus, pTtlTranUpBuff->TtlItemDisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);    /* Update Discount Product */
            break;                  /* End Affection */

        case CLASS_CHARGETIP:       /* Charge tip */
        case CLASS_CHARGETIP2:      /* Charge tip, V3.3 */
        case CLASS_CHARGETIP3:      /* Charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP:   /* Charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP2:  /* Charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP3:  /* Charge tip, V3.3 */
            TtlFDiscount(fbMod, usStatus, pTtlTranUpBuff->TtlItemDisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);    /* Update Discount Product */
            break;                  /* End Affection */
/*      default:    Not Used */
        }
        return;

/* -------------- Coupon Affection, R3.0 --------------- */
    case CLASS_ITEMCOUPON:          /* Coupon */
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & VOID_MODIFIER) {    /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemCoupon.fbModifier;
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
			}
		} else {
			if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemCoupon.fbModifier;
			}
        }

		if (pTtlTranUpBuff->TtlItemCoupon.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status as this ia returned item */

			if((sSign > 0))
			{
				TtlUpFinBonus(pTmpBuff, (pTtlTranUpBuff->TtlItemCoupon.lAmount * -1),
					pTtlTranUpBuff->TtlItemCoupon.uchNoOfItem, (USHORT)(pTtlTranUpBuff->TtlItemCoupon.usBonusIndex-1));
			}else
			{
				TtlUpFinBonus(pTmpBuff, (pTtlTranUpBuff->TtlItemCoupon.lAmount * -1),
					(SHORT)((SHORT)pTtlTranUpBuff->TtlItemCoupon.uchNoOfItem * -1), (USHORT)(pTtlTranUpBuff->TtlItemCoupon.usBonusIndex-1));
			}
		}

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_COMCOUPON:
            TtlFCpn(fbMod, usStatus, pTtlTranUpBuff->TtlItemCoupon.lAmount, sSign, pTmpBuff);    /* Update Coupon Product */
            break;
        case CLASS_UPCCOUPON:
            TtlFUCpn(fbMod, usStatus, pTtlTranUpBuff->TtlItemCoupon.lAmount, sSign, pTmpBuff);   /* Update Coupon Product */
            break;
        }
        return;

/* -------------- Total Affection --------------- */
    case CLASS_ITEMTOTAL:           /* Total key */
        TtlFTotal(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
        return;

/* -------------- Tender Affection --------------- */
    case CLASS_ITEMTENDER:          /* tender */
        if (pTtlTranUpBuff->TtlItemTender.fbModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemTender.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
			}
        }
		if (pTtlTranUpBuff->TtlItemTender.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
			usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
		}
		if (pTtlTranUpBuff->TtlItemTender.fbReduceStatus & REDUCE_ITEM) {  // Set ignore status for the tender on Preauth tender item of a Preauth Capture transaction
			usStatus |= TTL_TUP_IGNORE;     // Set ignore status for the tender on Preauth tender item of a Preauth Capture transaction
		}
		if (pTtlTranUpBuff->TtlItemTender.fbReduceStatus & REDUCE_RETURNED) {
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status for the tender on returned items */
		}
        TtlFTender(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
        return;

/* -------------- Misc Affection --------------- */
    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
            TtlUpFinNoSaleCount(pTmpBuff);
                                    /* Affect No Sale Counter */
            break;                  /* End Affection */

        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips paid out */
        case CLASS_TIPSDECLARED:    /* Tips Declared */
            if (pTtlTranUpBuff->TtlItemMisc.fbModifier & VOID_MODIFIER) {
                                    /* Check VOID Modifier */
                sSign *= -1;        /* Set Sign */
				if (pTtlTranUpBuff->TtlItemMisc.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
				}
            }
            TtlFmisc(usStatus, pTtlTranUpBuff->TtlItemMisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);
            break;                  /* Execute Affection */
/*      default:    Not Used */
        }
        return;

/* -------------- Other Affection --------------- */
    case CLASS_ITEMAFFECTION:       /* Affection */
        /* VAT affection, V3.3 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_SERVICE_VAT:          /* VATable Service Total */
        case CLASS_SERVICE_NON:          /* Non VATable Service Total */
        case CLASS_VATAFFECT:            /* VAT Affection */
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status for the tzx on returned items */
            if (pTtlTranUpBuff->TtlItemAffect.fbModifier & EXCLUDE_VAT_SER) {
                usStatus |= TTL_TUP_EXCLUDE;
            }
            TtlFVatService(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
            break;

        case CLASS_LOANAFFECT:          /* Loan,    Saratoga */
        case CLASS_PICKAFFECT:          /* Pickup,  Saratoga */
            TtlFLoanPick(pTtlTranUpBuff, pTmpBuff);
            break;

        case CLASS_LOANCLOSE:           /* Saratoga */
        case CLASS_PICKCLOSE:
            TtlFLoanPickClose(pTtlTranUpBuff, pTmpBuff);
            break;

		case CLASS_EPT_ERROR_AFFECT:
			TtlFEptError(pTtlTranUpBuff, pTmpBuff);
			break;

        default:
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status for the tzx on returned items */
            TtlFAffect(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
            break;
        }
        return;
/*  default: Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFSales(USHORT usStatus, LONG lAmount, 
*                                SHORT sCounter,  UCHAR uchBOffset, 
*                                TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchBOffset        - Bounus Total Offset
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function affects Financial in ItemSales of Major Class.
*
*============================================================================
*/
VOID TtlFSales(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, 
               USHORT uchBOffset, TTLCSREGFIN  *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    if (usStatus & TTL_TUP_TRAIN) {        /* Check Training Operation */
        if (usStatus & TTL_TUP_CREDT) {    /* Check Credit Dept/PLU Status */ 
            return;
        }
        if ((usStatus & (TTL_TUP_HASH + TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH +TTL_TUP_HASHAFFECT)) {   /* Check Hash Item & Not Affection Control */
#if 0
            TtlUpFinCGGTotal(pTmpBuff, lAmount);    /* Affect Current Gross Total */
            TtlUpFinDGGTotal(pTmpBuff, lAmount);    /* Affect Daily Gross Total */
            TtlUpFinTGGTotal(pTmpBuff, lAmount);    /* Affect Training Gross Total */
#else
			pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
			pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
			pTmpBuff->lTGGTotal -= lAmount;     /* Affect Training Gross Group Total */
#endif
        }
        return;
    }

    /* === Add New Element - Net Total (Release 3.1) BEGIN === */
    TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
    /* === Add New Element - Net Total (Release 3.1) END === */

    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == (TTL_TUP_TRETURN)) {      /* Check transaction return */
        TtlUpFinTransactionReturn(fbMod, pTmpBuff, lAmount, sCounter);        /* Affect transaction return Total/Counter */
    }

    if ((usStatus & TTL_TUP_CREDT) == 0) {    /* Check Credit Dept/PLU Status */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
//		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {    /* Check Hash Item & Not Affection Control */
#if 0
                TtlUpFinCGGTotal(pTmpBuff, lAmount);    /* Affect Current Gross Total */
                TtlUpFinDGGTotal(pTmpBuff, lAmount);    /* Affect Daily Gross Total */
#else
				pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
				pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
#endif
            }
            break;

        case TTL_TUP_MVOID:
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {     /* Check Hash Item & Not Affection Control */
                TtlUpFinPlusVoid(pTmpBuff, lAmount);     /* Affect Plus Void Total/Counter */
            }
            break;

        case TTL_TUP_PVOID:
            if ((usStatus & (TTL_TUP_HASH + TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH +TTL_TUP_HASHAFFECT)) { /* Check Hash Item & Not Affection Control */
                TtlUpFinPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
            }
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {
                TtlUpFinPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
                TtlUpFinMiscVoid(pTmpBuff, lAmount);      /* Affect Misc Void Total/Counter */
            }
            break;
/*      default:    Not Used */
        }

        if (usStatus & TTL_TUP_HASH) {      /* Check Hash Dept/PLU Status */
            TtlUpFinHashDept(pTmpBuff, lAmount, sCounter);        /* Affect Hash Dept Total/Counter */
		}
        TtlUpFinBonus(pTmpBuff, lAmount, sCounter, uchBOffset);   /* V3.3, Affect Bouns Total/Counter */

    } else {
        /* V3.3 */
        if (usStatus & TTL_TUP_HASH) {         /* Check Hash Dept/PLU Status */
            TtlUpFinHashDept(pTmpBuff, lAmount, sCounter);     /* Affect Hash Dept Total/Counter */
        } else {
            TtlUpFinConsCoupon(pTmpBuff, lAmount, sCounter);
        }
        TtlUpFinBonus(pTmpBuff, lAmount, sCounter, uchBOffset);
                                    /* V3.3, Affect Bouns Total/Counter */
                                    /* Affect Consol. Coupon Total/Counter */
        switch (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {
        case TTL_TUP_MVOID:
        case TTL_TUP_PVOID:
        case (TTL_TUP_MVOID + TTL_TUP_PVOID):
            TtlUpFinMiscVoid(pTmpBuff, lAmount);    /* Affect Misc Void Total/Counter */
            break;
/*      default:    Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFModDiscount(USHORT usStatus, LONG lAmount, 
*                                   SHORT sSign, TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Discount of Major Class.
*
*============================================================================
*/
VOID TtlFModDiscount(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, 
                     TTLCSREGFIN  *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    if (usStatus & TTL_TUP_TRAIN) {            /* Check Training Operation */
#if 0
        TtlUpFinCGGTotal(pTmpBuff, lAmount);   /* Affect Current Gross Total */
        TtlUpFinDGGTotal(pTmpBuff, lAmount);   /* Affect Daily Gross Total */
        TtlUpFinTGGTotal(pTmpBuff, lAmount);   /* Affect Training Gross Total */
#else
		pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
		pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
		pTmpBuff->lTGGTotal -= lAmount;     /* Affect Training Gross Group Total */
#endif
        return;
    }

    /* === Add New Element - Net Total (Release 3.1) BEGIN === */
    TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
    /* === Add New Element - Net Total (Release 3.1) END === */

    TtlUpFinModiDisc(pTmpBuff, lAmount, sSign);      /* Affect Modifier Discount Total/Counter */

    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {      /* Check transaction return */
        TtlUpFinTransactionReturn(0, pTmpBuff, lAmount, 0);        /* Affect transaction return Total/Counter */
    }

    if ((usStatus & TTL_TUP_SURCHARGE) == 0) {
		/* --- Only Discount Affection --- */ 
        if (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID)) {  /* Check Momentary Void */
            TtlUpFinMiscVoid(pTmpBuff, lAmount);     /* Affect Misc Void Total/Counter */
        }
    } else {
		/* --- Only Surcharge Affection --- */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
//		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
#if 0
            TtlUpFinCGGTotal(pTmpBuff, lAmount);     /* Affect Current Gross Total */
            TtlUpFinDGGTotal(pTmpBuff, lAmount);     /* Affect Daily Gross Total */
#else
			pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
			pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
#endif
            break;

        case TTL_TUP_MVOID:
            TtlUpFinPlusVoid(pTmpBuff, lAmount);     /* Affect Plus Void Total/Counter */
            break;

        case TTL_TUP_PVOID:
            TtlUpFinPreVoid(pTmpBuff, lAmount, 0);   /* Affect Preselect Void Total/Counter */
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            TtlUpFinPreVoid(pTmpBuff, lAmount, 0);   /* Affect Preselect Void Total/Counter */
            TtlUpFinMiscVoid(pTmpBuff, lAmount);     /* Affect Misc Void Total/Counter */
            break;

/*      default: Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFDiscount(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, UCHAR uchMinorClass, 
*                                 TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               UCHAR uchMinorClass     - Minor Class
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Discount of Major Class.
*
*============================================================================
*/
VOID TtlFDiscount(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, 
                    UCHAR uchMinorClass, TTLCSREGFIN  *pTmpBuff)
{
    UCHAR   uchOffset;

	if (usStatus & TTL_TUP_IGNORE) return;

    if (usStatus & TTL_TUP_TRAIN) {            /* Check Training Operation */
#if 0
        TtlUpFinCGGTotal(pTmpBuff, lAmount);      /* Affect Current Gross Total */
        TtlUpFinDGGTotal(pTmpBuff, lAmount);      /* Affect Daily Gross Total */
        TtlUpFinTGGTotal(pTmpBuff, lAmount);      /* Affect Training Gross Total */
#else
		pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
		pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
		pTmpBuff->lTGGTotal -= lAmount;     /* Affect Training Gross Group Total */
#endif
        return;
     }

    /* === Add New Element - Net Total (Release 3.1) BEGIN === */
    TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
    /* === Add New Element - Net Total (Release 3.1) END === */

    switch (uchMinorClass) {
    case CLASS_ITEMDISC1:                                   /* Affection Item Discount */
//        TtlUpFinItemDisc(pTmpBuff, lAmount, sSign);       /* Affect Item Discount Total/Counter */
        break;

    case CLASS_REGDISC1:            /* Affection Regular Discount 1 */
    case CLASS_REGDISC2:            /* Affection Regular Discount 2 */
    /* === New Elements - Regular Discount 3 - 6 (Release 3.1) BEGIN === */
    case CLASS_REGDISC3:            /* Affection Regular Discount 3 */
    case CLASS_REGDISC4:            /* Affection Regular Discount 4 */
    case CLASS_REGDISC5:            /* Affection Regular Discount 5 */
    case CLASS_REGDISC6:            /* Affection Regular Discount 6 */
    /* === New Elements - Regular Discount 3 - 6 (Release 3.1) END === */

        uchOffset = (UCHAR)(uchMinorClass - CLASS_REGDISC1);
        
        TtlUpFinRegDisc(pTmpBuff, lAmount, sCounter, uchOffset);          /* Affect Regular Discount Total/Counter */
        break;

    case CLASS_CHARGETIP:           /* Affection Charge Tips */
    case CLASS_CHARGETIP2:          /* Affection Charge Tips, V3.3 */
    case CLASS_CHARGETIP3:          /* Affection Charge Tips, V3.3 */
        uchOffset = (UCHAR)(uchMinorClass - CLASS_CHARGETIP);
        TtlUpFinChargeTips(pTmpBuff, lAmount, sCounter, uchOffset);        /* Affect Charge Tips Total/Counter */
        break;

    case CLASS_AUTOCHARGETIP:       /* Affection Charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:      /* Affection Charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:      /* Affection Charge Tips, V3.3 */

        uchOffset = (UCHAR)(uchMinorClass - CLASS_AUTOCHARGETIP);
        
        TtlUpFinChargeTips(pTmpBuff, lAmount, sCounter, uchOffset);
                                    /* Affect Charge Tips Total/Counter */
        break;
		/*  default: Not Used */
    }

    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {      /* Check transaction return */
        TtlUpFinTransactionReturn(fbMod, pTmpBuff, lAmount, 0);        /* Affect transaction return Total/Counter */
    }

    if ((usStatus & TTL_TUP_SURCHARGE) == 0) {
		/* --- Only Discount Affection --- */ 
        if (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID)) {       /* Check Momentary Void */
            TtlUpFinMiscVoid(pTmpBuff, lAmount);                /* Affect Misc Void Total/Counter */
        }
    } else {
		/* --- Only Surcharge Affection --- */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
//		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
#if 0
            TtlUpFinCGGTotal(pTmpBuff, lAmount);                /* Affect Current Gross Total */
            TtlUpFinDGGTotal(pTmpBuff, lAmount);                /* Affect Daily Gross Total */
#else
			pTmpBuff->mlCGGTotal += lAmount;    /* Affect Current Gross Total */
			pTmpBuff->lDGGtotal += lAmount;     /* Affect Daily Gross Group Total */
#endif
            break;

        case TTL_TUP_MVOID:
            TtlUpFinPlusVoid(pTmpBuff, lAmount);                /* Affect Plus Void Total/Counter */
            break;

        case TTL_TUP_PVOID:
            TtlUpFinPreVoid(pTmpBuff, lAmount, 0);              /* Affect Preselect Void Total/Counter */
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            TtlUpFinPreVoid(pTmpBuff, lAmount, 0);              /* Affect Preselect Void Total/Counter */
            TtlUpFinMiscVoid(pTmpBuff, lAmount);                /* Affect Misc Void Total/Counter */
            break;

		/*      default: Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFCpn(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, TTLCSREGFIN FAR  *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Coupon of Major Class.
*                    R3.0
*============================================================================
*/
VOID TtlFCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSREGFIN  *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    /* === Fixed a glitch (11/16/95, Rel3.1) BEGIN === */
    if (usStatus & TTL_TUP_TRAIN) { /* Is operator training ? */
        return;
    }
    /* === Fixed a glitch (11/16/95, Rel3.1) END === */

    TtlUpFinCpn(pTmpBuff, lAmount, sSign);
    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {      /* Check transaction return */
        TtlUpFinTransactionReturn(fbMod, pTmpBuff, lAmount, 0);        /* Affect transaction return Total/Counter */
    }
    if (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {
        TtlUpFinMiscVoid(pTmpBuff, lAmount); /* Affect Misc Void Total/Counter */
    }

    /* === Add New Element - Net Total (Release 3.1) BEGIN === */
    TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
    /* === Add New Element - Net Total (Release 3.1) END === */
}
/*
*============================================================================
**Synopsis:     VOID TtlFCpn(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, TTLCSREGFIN FAR  *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Coupon of Major Class.
*                    R3.0
*============================================================================
*/
VOID TtlFUCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSREGFIN  *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    /* === Fixed a glitch (11/16/95, Rel3.1) BEGIN === */
    if (usStatus & TTL_TUP_TRAIN) { /* Is operator training ? */
        return;
    }
    /* === Fixed a glitch (11/16/95, Rel3.1) END === */

    TtlUpFinUCpn(pTmpBuff, lAmount, sSign);
    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {      /* Check transaction return */
        TtlUpFinTransactionReturn(fbMod, pTmpBuff, lAmount, 0);        /* Affect transaction return Total/Counter */
    }
    if (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {
        TtlUpFinMiscVoid(pTmpBuff, lAmount); /* Affect Misc Void Total/Counter */
    }

    /* === Add New Element - Net Total (Release 3.1) BEGIN === */
    TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
    /* === Add New Element - Net Total (Release 3.1) END === */
}
/*
*============================================================================
**Synopsis:     VOID TtlFTotal(USHORT usStatus, SHORT sSign, 
*                              TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                              TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Total of Major Class.
*
*============================================================================
*/
VOID TtlFTotal(USHORT usStatus, SHORT sSign, 
               TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN  *pTmpBuff)
{
    UCHAR uchOffset;
    UCHAR uchTtlType;

	if (usStatus & TTL_TUP_IGNORE) return;

	if (usStatus & CURQUAL_TRETURN) {
		// force increment of counters if this is a return
		// which may have negative sign
		sSign = 1;
	}

    TtlUpFinServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, pTtlTranUpBuff->TtlItemTotal.uchMinorClass);    /* Affect Service Total Total/Counter */
    uchTtlType = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE);
    if ((uchTtlType == PRT_NOFINALIZE) || (uchTtlType == PRT_TRAY1) || (uchTtlType == PRT_TRAY2)) {  /* Check Not Finalize Total, Check Tray Total */
        return;
    }
    switch(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] % CHECK_TOTAL_TYPE) {
    case CLASS_TENDER1:                         /* Tender #1 */
        TtlUpFinCashTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, TTL_ONLINE);       /* Affect Cash Tender  */
        break;

    case CLASS_TENDER2:                         /* Tender #2 */
        TtlUpFinCheckTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, TTL_ONLINE);      /* Affect Check Tender */
        break;

    case CLASS_TENDER3:                         /* Tender #3 */
        TtlUpFinChargeTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, TTL_ONLINE);    /* Affect Charge Tender */
        break;

    case CLASS_TENDER4:                         /* Tender #4 */
    case CLASS_TENDER5:                         /* Tender #5 */
    case CLASS_TENDER6:                         /* Tender #6 */
    case CLASS_TENDER7:                         /* Tender #7 */
    case CLASS_TENDER8:                         /* Tender #8 */
    case CLASS_TENDER10:                        /* Tender #10 */
    case CLASS_TENDER11:                        /* Tender #11 */
    case CLASS_TENDER12:                        /* Tender #12 */
    case CLASS_TENDER13:                        /* Tender #13 */
    case CLASS_TENDER14:                        /* Tender #14 */
    case CLASS_TENDER15:                        /* Tender #15 */
    case CLASS_TENDER16:                        /* Tender #16 */
    case CLASS_TENDER17:                        /* Tender #17 */
    case CLASS_TENDER18:                        /* Tender #18 */
    case CLASS_TENDER19:                        /* Tender #19 */
    case CLASS_TENDER20:                        /* Tender #20 */
        uchOffset = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] % CHECK_TOTAL_TYPE) - (UCHAR)CLASS_TENDER4;
        TtlUpFinMiscTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, uchOffset, TTL_ONLINE);
                                    /* Affect Misc Tender 1 */
                                    /* Affect Misc Tender 2 */
                                    /* Affect Misc Tender 3 */
                                    /* Affect Misc Tender 4 */
                                    /* Affect Misc Tender 5 */
        break;

/*      default: Not Used */
    }
	if ((usStatus & (CURQUAL_PRESELECT_MASK | TTL_TUP_TRETURN)) == 0)  // do not update the customer counter if Transaction Void (CURQUAL_PVOID) or transaction return (CURQUAL_TRETURN) was done
		TtlUpFinCustomer(pTmpBuff, sSign);      /* Affect Customer Counter */

	/*
	*      Audaction is a total of the number of times that a tender amount
	*      went to either zero or less than zero along with the tender amount
	*      added to the audaction amount total.
	*      The audaction total provides an audit check point to see how many
	*      tenders are resulting in a zero or negative amount and may be
	*      an indication of stealing much like too many No Sales being done.
	*/
    switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
    case 0:
        if (pTtlTranUpBuff->TtlItemTotal.lMI <= 0L) {                        /* Check Audaction Opeartion */
            TtlUpFinAudaction(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI);   /* Affect Audaction Total/Counter */
        }
        break;

    case TTL_TUP_PVOID:
        if (pTtlTranUpBuff->TtlItemTotal.lMI >= 0L) {                                /* Check Audaction Opeartion */
            TtlUpFinAudaction(pTmpBuff, (pTtlTranUpBuff->TtlItemTotal.lMI * -1L));   /* Affect Audaction Total/Counter */
        }
        break;
/*      default: Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFTender(USHORT usStatus, SHORT sSign, 
*                               TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                               TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Tender of Major Class.
*
*============================================================================
*/
VOID TtlFTender(USHORT usStatus, SHORT sSign, 
                TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN  *pTmpBuff)
{
    UCHAR   uchOffset;
    UCHAR   uchOnOff = TTL_ONLINE;                  /* set online status    */

	if (usStatus & TTL_TUP_IGNORE) return;

	if (usStatus & (TTL_TUP_MRETURN | TTL_TUP_TRETURN | TTL_TUP_PVOID | TTL_TUP_MVOID)) {
		// force increment of counters if this is a return
		// which may have negative sign
		sSign = 1;
	}

	if (pTtlTranUpBuff->TtlItemTender.fbReduceStatus & REDUCE_PREAUTH) {
		TtlUpFinServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, pTtlTranUpBuff->TtlItemTender.uchMinorClass);    /* Affect Service Total Total/Counter */
		return;
	}

    if ((CLASS_TENDER1 <= pTtlTranUpBuff->TtlClass.uchMinorClass) && (pTtlTranUpBuff->TtlClass.uchMinorClass <= CLASS_TENDER20) &&
        ((pTtlTranUpBuff->TtlItemTender.fbModifier & OFFCPTEND_MODIF) || (pTtlTranUpBuff->TtlItemTender.fbModifier & OFFEPTTEND_MODIF))) {
        uchOnOff = TTL_OFFLINE;                     /* set offline status   */
    }

    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
    case CLASS_TENDER1:                         /* Tender #1            */
        TtlUpFinCashTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, uchOnOff);
        break;                                  /* Affect Cash Tender   */
    
    case CLASS_TENDER2:                         /* Tender #2            */
        TtlUpFinCheckTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, uchOnOff);
        break;                                  /* Affect Check Tender  */
    
    case CLASS_TENDER3:                         /* Tender #3            */
        TtlUpFinChargeTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, uchOnOff);
        break;                                  /* Affect Charge Tender */
    
    case CLASS_TENDER4:                         /* Tender #4            */
    case CLASS_TENDER5:                         /* Tender #5            */
    case CLASS_TENDER6:                         /* Tender #6            */
    case CLASS_TENDER7:                         /* Tender #7            */
    case CLASS_TENDER8:                         /* Tender #8            */
    case CLASS_TENDER9:                         /* Tender #9            */
    case CLASS_TENDER10:                        /* Tender #10           */
    case CLASS_TENDER11:                        /* Tender #11           */
    case CLASS_TENDER12:
    case CLASS_TENDER13:
    case CLASS_TENDER14:
    case CLASS_TENDER15:
    case CLASS_TENDER16:
    case CLASS_TENDER17:
    case CLASS_TENDER18:
    case CLASS_TENDER19:     /* Saratoga */
    case CLASS_TENDER20:
        uchOffset = pTtlTranUpBuff->TtlClass.uchMinorClass - (UCHAR)CLASS_TENDER4;
        TtlUpFinMiscTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, uchOffset, uchOnOff);   /* Affect Misc Tender n */

    case CLASS_TEND_FSCHANGE:
        TtlUpFinFSChange(pTmpBuff, &pTtlTranUpBuff->TtlItemTender, sSign);
        break;

    case CLASS_TEND_TIPS_RETURN:       /* FS Change,   Saratoga */
		{
			ITEMTENDERCHARGETIPS  *pChargeTips = &(ITEMTENDER_CHARGETIPS(&(pTtlTranUpBuff->TtlItemTender)));
            TtlFDiscount(0, usStatus, pTtlTranUpBuff->TtlItemTender.lGratuity, sSign, pChargeTips->uchMinorClass, pTmpBuff);    /* Update Discount Product */
		}
		break;

    case CLASS_FOREIGN1:                        /* FC Tender #1         */
    case CLASS_FOREIGN2:                        /* FC Tender #2         */
    case CLASS_FOREIGN3:                        /* FC Tender #3, Saratoga */
    case CLASS_FOREIGN4:                        /* FC Tender #4, Saratoga */
    case CLASS_FOREIGN5:                        /* FC Tender #5, Saratoga */
    case CLASS_FOREIGN6:                        /* FC Tender #6, Saratoga */
    case CLASS_FOREIGN7:                        /* FC Tender #7, Saratoga */
    case CLASS_FOREIGN8:                        /* FC Tender #8, Saratoga */
        uchOffset = pTtlTranUpBuff->TtlClass.uchMinorClass - (UCHAR)CLASS_FOREIGN1;
        TtlUpFinForeignTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lForeignAmount, sSign, uchOffset);
        break;

     /* default: Not Used */
    }

    /* --- Saratoga Do not Coutup in case of FS Change,  Dec/4/2000 --- */
    if (pTtlTranUpBuff->TtlClass.uchMinorClass != CLASS_TEND_FSCHANGE) {
        if (usStatus & TTL_TUP_MVOID) { /* Check Momentary Void */
            TtlUpFinMiscVoid(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount);   /* Affect Misc Void Total/Counter */
        } else {
            if (pTtlTranUpBuff->TtlItemTender.lBalanceDue == 0L) {    /* Check Balance Due */
				if ((usStatus & (CURQUAL_PRESELECT_MASK | TTL_TUP_TRETURN)) == 0)  // do not update the customer counter if Transaction Void (CURQUAL_PVOID) or transaction return (CURQUAL_TRETURN) was done
					TtlUpFinCustomer(pTmpBuff, sSign);                    /* Affect Customer Counter */
            }
        }
    }

    if (pTtlTranUpBuff->TtlItemTender.lChange != 0L) {                /* Check Change Operation */
        TtlUpFinCashTender(pTmpBuff, (pTtlTranUpBuff->TtlItemTender.lChange * -1L), 0, TTL_ONLINE);    /* Affect Change Total */
    }

	/*
	*      Audaction is a total of the number of times that a tender amount
	*      went to either zero or less than zero along with the tender amount
	*      added to the audaction amount total.
	*      The audaction total provides an audit check point to see how many
	*      tenders are resulting in a zero or negative amount and may be
	*      an indication of stealing much like too many No Sales being done.
	*/
    switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
    case 0:
        if (pTtlTranUpBuff->TtlItemTender.lTenderAmount <= 0L) {      /* Check Audaction Opeartion */
            TtlUpFinAudaction(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount);            /* Affect Audaction Total/Counter */
        }
        break;

    case TTL_TUP_PVOID:
        if (pTtlTranUpBuff->TtlItemTender.lTenderAmount >= 0L) {      /* Check Audaction Opeartion */
            TtlUpFinAudaction(pTmpBuff, (pTtlTranUpBuff->TtlItemTender.lTenderAmount * -1L));    /* Affect Audaction Total/Counter */
        }
        break;
/*      default: Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFMisc(USHORT usStatus, LONG lAmount, 
*                             SHORT sSign, UCHAR uchMinorClass, 
*                             TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               UCHAR uchMinorClass     - Minor Class
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Misc of Major Class.
*
*============================================================================
*/
VOID TtlFmisc(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, 
              UCHAR uchMinorClass, TTLCSREGFIN  *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    switch (uchMinorClass) {
    case CLASS_PO:                  /* Affection Paid Out */
        TtlUpFinPaidOut(pTmpBuff, lAmount, sSign);                   /* Affect Paid Out Total/Counter */
        TtlUpFinCashTender(pTmpBuff, lAmount, sSign, TTL_ONLINE);    /* Affect Cash Tender Total/Counter */ 

        /* === Add New Element - Net Total (Release 3.1) BEGIN === */
        TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
        /* === Add New Element - Net Total (Release 3.1) END === */
        break;

    case CLASS_RA:                  /* Affection Received on Account */
        TtlUpFinRecvAcount(pTmpBuff, lAmount, sSign);                /* Affect Received on Account Total/Counter */
        TtlUpFinCashTender(pTmpBuff, lAmount, sSign, TTL_ONLINE);    /* Affect Cash Tender Total/Counter */ 

        /* === Add New Element - Net Total (Release 3.1) BEGIN === */
        TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
        /* === Add New Element - Net Total (Release 3.1) END === */
        break;

    case CLASS_TIPSPO:              /* Affection Tips paid out */
        TtlUpFinTipsPaid(pTmpBuff, lAmount, sSign);                  /* Affect Tips Paid Out Total/Counter */
        TtlUpFinCashTender(pTmpBuff, lAmount, sSign, TTL_ONLINE);    /* Affect Cash Tender Total/Counter */ 

        /* === Add New Element - Net Total (Release 3.1) BEGIN === */
        TtlUpFinNetTotal(pTmpBuff, lAmount);    /* Affect Net Total, R3.1 */    
        /* === Add New Element - Net Total (Release 3.1) END === */
        break;

    case CLASS_TIPSDECLARED:        /* Affection Tips Declared */
        TtlUpFinDeclaredTips(pTmpBuff, lAmount, sSign);              /* Affect Declared Tips Total/Counter */
        break;

/*  default: Not Used */
    }
    if (usStatus & TTL_TUP_MVOID) {              /* Check Momentary Void */
        TtlUpFinMiscVoid(pTmpBuff, lAmount);     /* Affect Misc Void Total/Counter */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFAffect(USHORT usStatus, SHORT sSign, 
*                               TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                               TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Affection of Major Class.
*
*============================================================================
*/
VOID TtlFAffect(USHORT usStatus, SHORT sSign, 
                TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN  *pTmpBuff)
{
    UCHAR i;

	if (usStatus & TTL_TUP_IGNORE) return;

    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
    case CLASS_HOURLY:              /* Hourly Total */
        TtlUpFinNoOfPerson(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.sNoPerson);             /* Affect No of Person Counter */
        TtlUpFinItemProduct(pTmpBuff, (LONG)pTtlTranUpBuff->TtlItemAffect.sItemCounter);   /* Affect Item Productivite Counter */
        if (usStatus & TTL_TUP_PVOID) {
            TtlUpFinPreVoid(pTmpBuff, 0L, 1);                                              /* Affect Preselect Void Total/Counter */
        }

        if (usStatus & TTL_TUP_TRETURN) {      /* Check transaction return */
            TtlUpFinTransactionReturn(0, pTmpBuff, 0L, 1);   /* Affect transaction return Total/Counter */
        }

        break;                      /* End Affection */

    case CLASS_SERVICE:             /* Service Total */
        TtlUpFinServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign, pTtlTranUpBuff->TtlItemAffect.uchTotalID);
                                    /* Affect Service Total Total/Counter */
        break;                     /* End Affection */

    case CLASS_CANCELTOTAL:         /* Cancel Total */
        TtlUpFinTransCancel(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lCancel);       /* Affect Cancel Transaction Total/Counter) */
        break;                      /* End Affection */

    case CLASS_CASHIERCHECK:        /* Cashier Close Check */
    case CLASS_WAITERCHECK:         /* Waiter Close Check */
        TtlUpFinNoOfChkClose(pTmpBuff, 1);                                      /* Affect No of Check Close Counter */
        break;                      /* End Affection */

    case CLASS_ADDCHECKTOTAL:       /* Addcheck Total */
    case CLASS_MANADDCHECKTOTAL:    /* Manual Addcheck Total */
        TtlUpFinAddCheckTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, pTtlTranUpBuff->TtlItemAffect.sItemCounter, pTtlTranUpBuff->TtlItemAffect.uchAddNum);
                                    /* Affect Cancel Transaction Total/Counter) */
        break;                      /* End Affection */

    case CLASS_CASHIEROPENCHECK: /* Casher open check */
        TtlUpFinNoOfChkOpen(pTmpBuff, 1);                                      /* Affect No of Checks Opened Counter */
        break;                  /* End Affection */

    case CLASS_TAXAFFECT:           /* US/Canadian Tax for Affection, V3.3 */
        for ( i = 0; i < 4; i++) {
            if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i] >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {  /* Check Tax Sign */
#if 0
                TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);     /* Affect Current Gross Total */
                TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);     /* Affect Daily Gross Total */
#else
				pTmpBuff->mlCGGTotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i];    /* Affect Current Gross Total */
				pTmpBuff->lDGGtotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i];     /* Affect Daily Gross Group Total */
#endif
                if (usStatus & TTL_TUP_TRAIN) {       /* Check Training Operation */
                    TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Training Gross Total */
                }
            } else {
#if 0
				if (usStatus & TTL_TUP_TRETURN) {
					TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);     /* Affect Current Gross Total */
					TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);     /* Affect Daily Gross Total */
					if (usStatus & TTL_TUP_TRAIN) {       /* Check Training Operation */
						TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Training Gross Total */
					}
				} else if ((usStatus & (TTL_TUP_PVOID + TTL_TUP_TRAIN)) == 0) {  /* Check Training Operation */
                    TtlUpFinPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Plus Void Total/Counter */
                }
#else
				if ((usStatus & (TTL_TUP_PVOID | TTL_TUP_TRAIN | TTL_TUP_TRETURN)) == 0) {  /* Check not a Training Operation or a return transaction */
                    TtlUpFinPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Plus Void Total/Counter */
                }
#endif
            }
        }      
        if ((usStatus & TTL_TUP_TRAIN) == 0) {    /* Check Training Operation */
            for (i = 0; i < 4; i++) {
                TtlUpFinTaxable(pTmpBuff, 
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxable[i],
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i],
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxExempt[i],
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lFSExempt[i],   /* Saratoga */
                                i);
                                    /* Affect Tax Total/Counter */
                TtlUpFinConsTax(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);
                                    /* Affcet Consolidation Tax Total */

                /* === Add New Element - Net Total (Release 3.1) BEGIN === */
				if ((usStatus & TTL_TUP_TRETURN) == 0) {
					TtlUpFinNetTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);  /* Affect Net Total, R3.1 */
				}
                /* === Add New Element - Net Total (Release 3.1) END === */

                if (usStatus & TTL_TUP_PVOID) {
                    TtlUpFinPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i], 0);
                                    /* Affect Preselect Void Total/Counter */
                }
            }
			pTmpBuff->NonTaxable += pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxable[4];
        }
        break;
/*  default:    Not Used */
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlFVatService(USHORT usStatus, SHORT sSign, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                   TTLCSREGFIN *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSREGFIN *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Financial in Affection of Major Class. V3.3
*
*============================================================================
*/
VOID TtlFVatService(USHORT usStatus, SHORT sSign, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN *pTmpBuff)
{
    UCHAR i;

	if (usStatus & TTL_TUP_IGNORE) return;

    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {

    case CLASS_SERVICE_VAT:          /* VATable Service Total */
    case CLASS_SERVICE_NON:          /* Non VATable Service Total */
        if (usStatus & TTL_TUP_EXCLUDE) {
            if (usStatus & TTL_TUP_TRAIN) {
                if ((pTtlTranUpBuff->TtlItemAffect.lAmount >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
                    TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
                } else if ((pTtlTranUpBuff->TtlItemAffect.lAmount < 0) && ((usStatus & TTL_TUP_TRETURN) != 0)) {
                    TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
                }
            } else {
                if ((pTtlTranUpBuff->TtlItemAffect.lAmount >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
#if 0
                    TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
                    TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
#else
					pTmpBuff->mlCGGTotal += pTtlTranUpBuff->TtlItemAffect.lAmount;    /* Affect Current Gross Total */
					pTmpBuff->lDGGtotal += pTtlTranUpBuff->TtlItemAffect.lAmount;     /* Affect Daily Gross Group Total */
#endif
				} else if ((pTtlTranUpBuff->TtlItemAffect.lAmount < 0) && ((usStatus & TTL_TUP_TRETURN) != 0)) {
#if 0
                    TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
                    TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
#else
					pTmpBuff->mlCGGTotal += pTtlTranUpBuff->TtlItemAffect.lAmount;    /* Affect Current Gross Total */
					pTmpBuff->lDGGtotal += pTtlTranUpBuff->TtlItemAffect.lAmount;     /* Affect Daily Gross Group Total */
#endif
                } else if (usStatus & TTL_TUP_PVOID) {
                    TtlUpFinPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, 0);
                } else {
                    TtlUpFinPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
                }
                TtlUpFinVATServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign);
            }
        } else {
            if ( 0 == (usStatus & TTL_TUP_TRAIN) ) {
                TtlUpFinVATServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign);
            }
        }
                                    /* Affect Service Total Total/Counter */
        break;                      /* End Affection */

    case CLASS_VATAFFECT:           /* VAT Affection */
        if (usStatus & TTL_TUP_EXCLUDE) {
            for (i=0; i < NUM_VAT; i++) {
                if (usStatus & TTL_TUP_TRAIN) {
                    if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
                        TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                    } else if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt < 0) && ((usStatus & TTL_TUP_TRETURN) != 0)) {
                        TtlUpFinTGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                    }
                } else {
                    if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
#if 0
                        TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                        TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
#else
						pTmpBuff->mlCGGTotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt;    /* Affect Current Gross Total */
						pTmpBuff->lDGGtotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt;     /* Affect Daily Gross Group Total */
#endif
					} else if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt < 0) && ((usStatus & TTL_TUP_TRETURN) != 0)) {
#if 0
                        TtlUpFinCGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                        TtlUpFinDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
#else
						pTmpBuff->mlCGGTotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt;    /* Affect Current Gross Total */
						pTmpBuff->lDGGtotal += pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt;     /* Affect Daily Gross Group Total */
#endif
                    } else if (usStatus & TTL_TUP_PVOID) {
                        TtlUpFinPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt, 0);

                    } else {
                        TtlUpFinPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                    }   
                    TtlUpFinTaxable(pTmpBuff, 
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lAppAmt,
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt,
                                0L,
                                0L,             /* Saratoga */
                                i);
                    TtlUpFinConsTax(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                                    /* Affcet Consolidation Tax Total */
                }
            }
        } else {
            if ( 0 == (usStatus & TTL_TUP_TRAIN) ) {
                for (i=0; i < NUM_VAT; i++) {
                    TtlUpFinTaxable(pTmpBuff, 
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lAppAmt,
                                pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt,
                                0L,
                                0L,             /* Saratoga */
                                i);
                }
            }
        }
        break;                    

/*  default:    Not Used */
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlUpFinTaxable(TTLCSREGFIN FAR *pTmpBuff, LONG lTaxable,
*                            LONG lTaxAmount, LONG lTaxExempt, LONG lFSExempt,
*                                    UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lTaxable           - Taxable Amount
*               LONG lTaxAmount         - Tax Amount
*               LONG lTaxExempt         - Tax Exempt Amount
*               UCHAR uchOffset         - Offset of Tax
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Tax of Financial.
*
*============================================================================
*/
VOID TtlUpFinTaxable(TTLCSREGFIN  *pTmpBuff, DCURRENCY lTaxable, DCURRENCY lTaxAmount,
                     DCURRENCY lTaxExempt, DCURRENCY lFSExempt, UCHAR uchOffset)
{

    if (uchOffset > 3) {
        return;
    }

	pTmpBuff->Taxable[uchOffset].mlTaxableAmount += lTaxable;  /* Affect Taxable Amount, reset at EOD governed by MDC_SUPER_ADR, ODD_MDC_BIT2 see TtlTresetFin() */
    pTmpBuff->Taxable[uchOffset].lTaxAmount += lTaxAmount;     /* Affect Tax Amount */
    pTmpBuff->Taxable[uchOffset].lTaxExempt += lTaxExempt;     /* Affect Tax Amount */
    pTmpBuff->Taxable[uchOffset].lFSTaxExempt += lFSExempt;    /* Affect FS Tax Exempt Amount, Saratoga */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinVATServiceTotal(TTLCSREGFIN *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter)
*
*    Input:     TTLCSREGFIN *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Service Total of Financial. V3.3
*
*============================================================================
*/
VOID TtlUpFinVATServiceTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->Taxable[3].lTaxAmount += lAmount;   /* Affect Service Total to Tax #4 */
    return;

    sCounter = sCounter;
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinTGGTotal(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Training of Financial.
*
*============================================================================
*/
VOID TtlUpFinTGGTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lTGGTotal -= lAmount;    /* Affect Training Gross Group Total */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpFinDeclaredTips(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount,
*                                         SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Declared Tips of Financial.
*
*============================================================================
*/
VOID TtlUpFinDeclaredTips(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->DeclaredTips.lAmount += lAmount;     /* Affect Decleared Tips Total */
    pTmpBuff->DeclaredTips.sCounter += sCounter;   /* Affect Decleared Tips Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpFinDGGTotal(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Daily Gross Group of Financial.
*
*============================================================================
*/
VOID TtlUpFinDGGTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lDGGtotal += lAmount;    /* Affect Daily Gross Group Total */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinPlusVoid(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Plus Void of Financial.
*
*============================================================================
*/
VOID TtlUpFinPlusVoid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->PlusVoid.lAmount += lAmount;   /* Affect Plus Void Total */
    pTmpBuff->PlusVoid.sCounter += 1;        /* Affect Plus Void Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinPreVoid(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Preselect Void of Financial.
*
*============================================================================
*/
VOID TtlUpFinPreVoid(TTLCSREGFIN *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->PreselectVoid.lAmount += lAmount;      /* Affect Preselect Void Total */
    pTmpBuff->PreselectVoid.sCounter += sCounter;    /* Affect Preselect Void Counter */
}

VOID TtlUpFinTransactionReturn(USHORT fbMod, TTLCSREGFIN *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
//	sCounter = abs(sCounter);
	sCounter *= -1;
	if (fbMod & RETURNS_MODIFIER_1) {
		pTmpBuff->TransactionReturn.lAmount += lAmount;      /* Affect transaction return Total */
		pTmpBuff->TransactionReturn.sCounter += sCounter;    /* Affect transaction return Counter */
	} else if (fbMod & RETURNS_MODIFIER_2) {
		pTmpBuff->TransactionExchng.lAmount += lAmount;       /* Update transaction return Total */
		pTmpBuff->TransactionExchng.sCounter += sCounter;     /* Update transaction return Counter */
	} else if (fbMod & RETURNS_MODIFIER_3){
		pTmpBuff->TransactionRefund.lAmount += lAmount;       /* Update transaction return Total */
		pTmpBuff->TransactionRefund.sCounter += sCounter;     /* Update transaction return Counter */
	}
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinConsCoupon(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Consolidated Coupon of Financial.
*
*============================================================================
*/
VOID TtlUpFinConsCoupon(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ConsCoupon.lAmount += lAmount;     /* Affect Consolidated Coupon Total */
    pTmpBuff->ConsCoupon.sCounter += sCounter;   /* Affect Consolidated Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinItemDisc(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Item Discount of Financial.
*
*============================================================================
*/
VOID TtlUpFinItemDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ItemDisc.lAmount += lAmount;     /* Affect Item Discount Total */
    pTmpBuff->ItemDisc.sCounter += sCounter;   /* Affect Item Discount Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinModiDisc(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Modifier Discount of Financial.
*
*============================================================================
*/
VOID TtlUpFinModiDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ModiDisc.lAmount += lAmount;     /* Affect Modified Item Discount Total */
    pTmpBuff->ModiDisc.sCounter += sCounter;   /* Affect Modified Item Discount Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinRegDisc(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Regular Discount Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Regular Discount of Financial.
*
*============================================================================
*/
VOID TtlUpFinRegDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                     UCHAR uchOffset)
{
    /* === New Element - Regular Discount 1 - 6 (Release 3.1) BEGIN === */
    if (uchOffset > 5) {
    /*    if (uchOffset > 1) { */
        return;
    }
    /* === New Element - Regular Discount 1 - 6 (Release 3.1) END === */

    pTmpBuff->RegDisc[uchOffset].lAmount += lAmount;     /* Affect Regular Discount 1 - 6 Total */
    pTmpBuff->RegDisc[uchOffset].sCounter += sCounter;   /* Affect Regular Discount 1 - 6 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinCpn(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Coupon of Financial.        R3.0
*
*============================================================================
*/
VOID TtlUpFinCpn(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->Coupon.lAmount += lAmount;     /* Update Coupon Total */
    pTmpBuff->Coupon.sCounter += sCounter;   /* Update Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinCpn(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Coupon of Financial.        R3.0
*
*============================================================================
*/
VOID TtlUpFinUCpn(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->aUPCCoupon[0].lAmount += lAmount;     /* Update Coupon Total */
    pTmpBuff->aUPCCoupon[0].sCounter += sCounter;   /* Update Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinPaidOut(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Paid Out of Financial.
*
*============================================================================
*/
VOID TtlUpFinPaidOut(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->PaidOut.lAmount += lAmount;      /* Affect Paid Out Total */
    pTmpBuff->PaidOut.sCounter += sCounter;    /* Affect Paid Out Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinRecvAcount(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Recieved on Account of Financial.
*
*============================================================================
*/
VOID TtlUpFinRecvAcount(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->RecvAcount.lAmount += lAmount;     /* Affect Received on Account Total */
    pTmpBuff->RecvAcount.sCounter += sCounter;   /* Affect Recevied on Account Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpFinTipsPaid(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount,
*                                     SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Tips Paid Out of Financial.
*
*============================================================================
*/
VOID TtlUpFinTipsPaid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->TipsPaid.lAmount += lAmount;     /* Affect Tips Paid Out Total */
    pTmpBuff->TipsPaid.sCounter += sCounter;   /* Affect Tips Paid Out Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpFinCashTender(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount,
*                                       SHORT sCounter, UCHAR uchOnOff)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOnOff          - TTL_ONLINE or TTL_OFFLINE
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cash Tender of Financial.
*
*============================================================================
*/
VOID TtlUpFinCashTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount,
                        SHORT sCounter, UCHAR uchOnOff)
{
    if (uchOnOff == TTL_ONLINE) {                   /* Online in CashTend   */
        pTmpBuff->CashTender.OnlineTotal.lAmount += lAmount;
        pTmpBuff->CashTender.OnlineTotal.sCounter += sCounter;
    } else {                                        /* Offline in CashTend  */
        pTmpBuff->CashTender.OfflineTotal.lAmount += lAmount;
        pTmpBuff->CashTender.OfflineTotal.sCounter += sCounter;
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinCheckTender(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                        SHORT sCounter, UCHAR uchOnOff)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOnOff          - TTL_ONLINE or TTL_OFFLINE
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Check Tender of Financial.
*
*============================================================================
*/
VOID TtlUpFinCheckTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount,
                         SHORT sCounter, UCHAR uchOnOff)
{
    if (uchOnOff == TTL_ONLINE) {                   /* Online in CheckTend  */
        pTmpBuff->CheckTender.OnlineTotal.lAmount += lAmount;
        pTmpBuff->CheckTender.OnlineTotal.sCounter += sCounter;
    } else {                                        /* Offline in CheckTend */
        pTmpBuff->CheckTender.OfflineTotal.lAmount += lAmount;
        pTmpBuff->CheckTender.OfflineTotal.sCounter += sCounter;
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlFinUpFSChange(TTLCSCASTOTAL *pBuff, LONG lAmount, SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cash Tender of Cashier.   Saratoga
*============================================================================
*/
VOID    TtlUpFinFSChange(TTLCSREGFIN  *pBuff, ITEMTENDER *pItem, SHORT sSign)
{
	/* for P-void */
    pBuff->CheckTender.OnlineTotal.lAmount  -= pItem->lFoodStamp;
    pBuff->FoodStampCredit.lAmount          += pItem->lFSChange;
    /* pBuff->CheckTender.OnlineTotal.lAmount  -= pItem->lFoodStamp * sSign;
    pBuff->FoodStampCredit.lAmount          += pItem->lFSChange * sSign; */
    if (pItem->lFSChange != 0L) {
        pBuff->FoodStampCredit.sCounter     += 1 * sSign;
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinChargeTender(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter, UCHAR uchOnOff)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOnOff          - TTL_ONLINE or TTL_OFFLINE
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Charge Tender of Financial.
*
*============================================================================
*/
VOID TtlUpFinChargeTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount,
                          SHORT sCounter, UCHAR uchOnOff)
{
    if (uchOnOff == TTL_ONLINE) {                   /* Online in ChargeTend */
        pTmpBuff->ChargeTender.OnlineTotal.lAmount += lAmount;
        pTmpBuff->ChargeTender.OnlineTotal.sCounter += sCounter;
    } else {                                        /* Offline in ChargeTend*/
        pTmpBuff->ChargeTender.OfflineTotal.lAmount += lAmount;
        pTmpBuff->ChargeTender.OfflineTotal.sCounter += sCounter;
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinMiscTender(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter, UCHAR uchOffset,
*                                       UCHAR uchOnOff)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Misc Tender Total
*               UCHAR uchOnOff          - TTL_ONLINE or TTL_OFFLINE
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Misc Tender of Financial.
*
*============================================================================
*/
VOID TtlUpFinMiscTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                        UCHAR uchOffset, UCHAR uchOnOff)
{
    if (uchOffset > sizeof(pTmpBuff->MiscTender)/sizeof(pTmpBuff->MiscTender[0])) {
        return;
    }

    if (uchOnOff == TTL_ONLINE) {                   /* Online in MiscTend   */
        pTmpBuff->MiscTender[uchOffset].OnlineTotal.lAmount += lAmount;
        pTmpBuff->MiscTender[uchOffset].OnlineTotal.sCounter += sCounter;
    } else {                                        /* Offline in MiscTend  */
        pTmpBuff->MiscTender[uchOffset].OfflineTotal.lAmount += lAmount;
        pTmpBuff->MiscTender[uchOffset].OfflineTotal.sCounter += sCounter;
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinForeignTotal(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount,
*                                         SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Foreign Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Foreign Total of Financial.
*
*============================================================================
*/
VOID TtlUpFinForeignTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                          UCHAR uchOffset)
{
    if (uchOffset > 7) {            /* Saratoga */
        return;
    }
    pTmpBuff->ForeignTotal[uchOffset].lAmount += lAmount;    /* Affect Foreign Total 1 to 8 Total */
    pTmpBuff->ForeignTotal[uchOffset].sCounter += sCounter;  /* Affect Foreign Total 1 to 8 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinServiceTotal(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Service Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Service Total of Financial.
*
*============================================================================
*/
VOID TtlUpFinServiceTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchTotalID)
{
	UCHAR  uchOffset = 0;

	// We map Total Key #3 through Total Key #8 to the Service Total Array elements 0 thru 5.
	// We map Total Key #10 through Total Key #20 to the Service Total Array elements 6 thru 16.
	// Total #1 is Cash, Total #2 and Total #9 are both food stamp.  None of these have service totals.
	if(uchTotalID >= CLASS_TOTAL3 && uchTotalID <= CLASS_TOTAL8)
	{
		uchOffset = uchTotalID - (UCHAR)CLASS_TOTAL3;
	}else if(uchTotalID >= CLASS_TOTAL10 && uchTotalID <= CLASS_TOTAL20)
	{
		uchOffset = uchTotalID - CLASS_TOTAL10 + 6;
	} else {
		return;
	}

    if (uchOffset >= sizeof(pTmpBuff->ServiceTotal)/sizeof(pTmpBuff->ServiceTotal[0])) {  //Ensure that the offset is within bounds of array
        return;
    }

    pTmpBuff->ServiceTotal[uchOffset].lAmount += lAmount;    /* Affect Service Total 1 to 6 Total */
    pTmpBuff->ServiceTotal[uchOffset].sCounter += sCounter;  /* Affect Service Total 1 to 6 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinAddCheckTotal(TTLCSREGFIN FAR *pTmpBuff, 
*                                          LONG lAmount, 
*                                          SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of AddCheck Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects AddCheck Total of Financial.
*
*============================================================================
*/
VOID TtlUpFinAddCheckTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, 
                           SHORT sCounter, UCHAR uchOffset)
{
    if (uchOffset > 2) {
        return;
    }
    pTmpBuff->AddCheckTotal[uchOffset].lAmount += lAmount;     /* Affect Add Check Total 1 to 3 Total */
    pTmpBuff->AddCheckTotal[uchOffset].sCounter += sCounter;   /* Affect add Check Total 1 to 3 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinChargeTips(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Charge Tips of Financial.
*
*============================================================================
*/
VOID TtlUpFinChargeTips(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount,
                                        SHORT sCounter, UCHAR uchOffset)
{
    if (uchOffset > 2) {
        return;
    }
    pTmpBuff->ChargeTips[uchOffset].lAmount += lAmount;     /* Affect Charge Tips Total */
    pTmpBuff->ChargeTips[uchOffset].sCounter += sCounter;   /* Affect Charge Tips Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinTransCancel(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Transaction Cancel of Financial.
*
*============================================================================
*/
VOID TtlUpFinTransCancel(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
	if (lAmount < 0) lAmount *= -1;
    pTmpBuff->TransCancel.lAmount += lAmount;  /* Affect Transaction Cancel Total */
    pTmpBuff->TransCancel.sCounter += 1;       /* Affect Transaction Cancel Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinMiscVoid(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Misc Void of Financial.
*
*============================================================================
*/
VOID TtlUpFinMiscVoid(TTLCSREGFIN *pTmpBuff, DCURRENCY lAmount)
{
	if (lAmount < 0) lAmount *= -1;
    pTmpBuff->MiscVoid.lAmount += lAmount;   /* Affect Misc Void Total */
    pTmpBuff->MiscVoid.sCounter += 1;              /* Affect Misc Void Counter */ 
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinAudaction(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Audaction of Financial.
*               Audaction is a total of the number of times that a tender amount
*               went to either zero or less than zero along with the tender amount
*               added to the audaction amount total.
*               The audaction total provides an audit check point to see how many
*               tenders are resulting in a zero or negative amount and may be
*               an indication of stealing much like too many No Sales being done.
*
*============================================================================
*/
VOID TtlUpFinAudaction(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->Audaction.lAmount += lAmount;   /* Affect Audaction Total */
    pTmpBuff->Audaction.sCounter += 1;        /* Affect Audaction Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinNoSaleCount(TTLCSREGFIN FAR *pTmpBuff)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No Sales of Financial.
*
*============================================================================
*/
VOID TtlUpFinNoSaleCount(TTLCSREGFIN  *pTmpBuff)
{
    pTmpBuff->sNoSaleCount += 1;       /* Affect No Sale Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinItemProduct(TTLCSREGFIN FAR *pTmpBuff, LONG lCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lCounter           - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Item Productivity of Financial.
*
*============================================================================
*/
VOID TtlUpFinItemProduct(TTLCSREGFIN  *pTmpBuff, LONG lCounter)
{
    pTmpBuff->lItemProductivityCount += lCounter;   /* Affect item Productivity Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinNoOfPerson(TTLCSREGFIN FAR *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Person of Financial.
*
*============================================================================
*/
VOID TtlUpFinNoOfPerson(TTLCSREGFIN  *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfPerson += sCounter;    /* Affect No of Person Counter */
}
/*                               
*============================================================================
**Synopsis:     VOID TtlUpFinNoOfChkOpen(TTLCSREGFIN FAR *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Checks Open Financial.
*
*============================================================================
*/
VOID TtlUpFinNoOfChkOpen(TTLCSREGFIN  *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfChkOpen += sCounter;    /* Affect No of Checks Open Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinNoOfChkClose(TTLCSREGFIN FAR *pTmpBuff, 
*                                         SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Checks Close Counter of Financial.
*
*============================================================================
*/
VOID TtlUpFinNoOfChkClose(TTLCSREGFIN  *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfChkClose += sCounter;    /* Affect No of Checks Close Counter */
}
/*               
*============================================================================
**Synopsis:     VOID TtlUpFinCustomer(TTLCSREGFIN FAR *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Customer Counter of Financial.
*
*============================================================================
*/
VOID TtlUpFinCustomer(TTLCSREGFIN  *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sCustomerCount += sCounter;    /* Affect Customer Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinHashDept(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Hash Department of Financial.
*
*============================================================================
*/
VOID TtlUpFinHashDept(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->HashDepartment.lAmount += lAmount;     /* Affect Hash Department Total */
    pTmpBuff->HashDepartment.sCounter += sCounter;   /* Affect Hash Department Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinBonus(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount, 
*                                  SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Bonus Total/Counter
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Bonus Total 1 to 8 of Financial.
*
*============================================================================
*/
VOID TtlUpFinBonus(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                   USHORT uchOffset)
{
    if (uchOffset >= STD_NO_BONUS_TTL) {
        return;
    }
    pTmpBuff->Bonus[uchOffset].lAmount += lAmount;     /* Affect Bonus Total 1 to 8 Total */
    pTmpBuff->Bonus[uchOffset].sCounter += sCounter;   /* Affect Bonus Total 1 to 8 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpFinConsTax(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Consolidation Tax Total of Financial.
*
*============================================================================
*/
VOID TtlUpFinConsTax(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lConsTax += lAmount;
                                    /* Affect Consolidation Tax Total */
}

/******************** New Functions (Release 3.1) BEGIN ********************/
/*
*============================================================================
**Synopsis:     VOID TtlUpFinNetTotal(TTLCSREGFIN FAR *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSREGFIN FAR *pTmpBuff   - Pointer of Finincial Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSREGFIN FAR *pTmpBuff   - Affected Finincial Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Net Total of Financial.
*
*============================================================================
*/
VOID TtlUpFinNetTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lNetTotal += lAmount;
}

/*
*============================================================================
**Synopsis: VOID    TtlFLoanPick(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN *pFin)
*
*    Input:     TTLTUMTRANUPDATE    *pClass
*   Output:     TTLCSREGFIN         *pFin
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Loan/Pickup.              Saratoga
*============================================================================
*/
VOID    TtlFLoanPick(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN  *pFin)
{
    if (pClass->TtlClass.uchMinorClass == CLASS_LOANAFFECT) {
        switch (pClass->TtlItemAffect.uchTotalID) {
        case    CLASS_MAINTTENDER1:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER2:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER3:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER4:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER5:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER6:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER7:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER8:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER9:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER10:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER11:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER12:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER13:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER14:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER15:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER16:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER17:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER18:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER19:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER20:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
            pFin->Loan.lAmount += pClass->TtlItemAffect.lAmount;
            break;
        case    CLASS_MAINTFOREIGN1:
        case    CLASS_MAINTFOREIGN2:
        case    CLASS_MAINTFOREIGN3:
        case    CLASS_MAINTFOREIGN4:
        case    CLASS_MAINTFOREIGN5:
        case    CLASS_MAINTFOREIGN6:
        case    CLASS_MAINTFOREIGN7:
        case    CLASS_MAINTFOREIGN8:
            pFin->Loan.lAmount += pClass->TtlItemAffect.lService[0];
            break;
        default:
            return;
        }

    } else {
        switch (pClass->TtlItemAffect.uchTotalID) {
        case    CLASS_MAINTTENDER1:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER2:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER3:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER4:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER5:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER6:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER7:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER8:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER9:          // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER10:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER11:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER12:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER13:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER14:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER15:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER16:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER17:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER18:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER19:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
        case    CLASS_MAINTTENDER20:         // function TtlFLoanPick() for AC10/AC11.  See also TtlCLoanPick()
            pFin->Pickup.lAmount += pClass->TtlItemAffect.lAmount;
            break;
        case    CLASS_MAINTFOREIGN1:
        case    CLASS_MAINTFOREIGN2:
        case    CLASS_MAINTFOREIGN3:
        case    CLASS_MAINTFOREIGN4:
        case    CLASS_MAINTFOREIGN5:
        case    CLASS_MAINTFOREIGN6:
        case    CLASS_MAINTFOREIGN7:
        case    CLASS_MAINTFOREIGN8:
            pFin->Pickup.lAmount += pClass->TtlItemAffect.lService[0];
            break;
        default:
            return;
        }
    }
}

/*
*============================================================================
**Synopsis: VOID    TtlFLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN *pFin)
*
*    Input:     TTLTUMTRANUPDATE    *pClass
*   Output:     TTLCSREGFIN         *pFin
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Loan/Pickup.              Saratoga
*               uchMajorClass == CLASS_LOANCLOSE    - TtlFLoanPickClose()
*               uchMajorClass == CLASS_PICKCLOSE    - TtlFLoanPickClose()
*============================================================================
*/
VOID    TtlFLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN  *pFin)
{
    if (pClass->TtlItemAffect.uchMinorClass == CLASS_LOANCLOSE) {
        pFin->Loan.sCounter++;
    } else {
        pFin->Pickup.sCounter++;
    }
}

/*
*============================================================================
**Synopsis: VOID    TtlFEptError(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN *pFin)
*
*    Input:     TTLTUMTRANUPDATE    *pClass
*   Output:     TTLCSREGFIN         *pFin
*    InOut:     Nothing
*
** Return:      Nothing
*               Nothing
*
** Description  This function Affects Electronic Payment Error totals.
*               uchMajorClass == CLASS_EPT_ERROR_AFFECT    - TtlFEptError()
*/
VOID    TtlFEptError(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN *pFin)
{
	int i;

	for (i = BONUS_ERROR_AFFECT_SALE; i <= BONUS_ERROR_AFFECT_VOIDRETURN; i++) {
		pFin->Bonus[i].lAmount += pClass->TtlItemAffect.lBonus[i];
		if (pClass->TtlItemAffect.lBonus[i]) {
			pFin->Bonus[i].sCounter += pClass->TtlItemAffect.sItemCounter;
		}
	}
}

/******************** New Functions (Release 3.1) END ***********************/

/* End of File */