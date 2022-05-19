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
* Program Name: Ttlsbfup.c
* --------------------------------------------------------------------------
* Abstract:
*           VOID TtlCasupdate()     - Affect Cashier Total
*           VOID TtlCSales()        - Affect Cashier Total in Sales Data
*           VOID TtlCModDiscount()  - Affect Cashier Total in Modifier Discount Data
*           VOID TtlCDiscount()     - Affect Cashier Total in Discount Data
*           VOID TtlCCpn()          - Affect Cashier Total in Coupon Data. R3.0
*           VOID TtlCmisc()         - Affect Cashier Total in Misc Data
*           VOID TtlCAffect()       - Affect Cashier Total in Affction Data
*           VOID TtlCPostReceipt()  - Affect Cashier Total in PostReceipt, R3.1
*           VOID TtlUpCasDGGTotal() - Affect Daily Gross Group Total of Cashier
*           VOID TtlUpCasPlusVoid() - Affect Plus Void Total of Cashier
*           VOID TtlUpCasPreVoid()  - Affect Preselect Void Total of Cashier
*           VOID TtlUpCasConsCoupon() - Affect Consolidated Coupon Total of Cashier
*           VOID TtlUpCasItemDisc() - Affect Item Discount Total of Cashier
*           VOID TtlUpCasModiDisc() - Affect Modifier Discount Total of Cashier
*           VOID TtlUpCasRegDisc()  - Affect Regular Discount Total of Cashier
*           VOID TtlUpCasCpn()      - Affect Coupon Total of Cashier,      R3.0
*           VOID TtlUpCasPaidOut()  - Affect Paid Out Total of Cashier
*           VOID TtlUpCasRecvAcount() - Affect Received on Account Total of Cashier
*           VOID TtlUpCasTipsPaid()  - Affect Tips Paid Out Total of Cashier
*           VOID TtlUpCasCashTender() - Affect Cash Tender Total of Cashier
*           VOID TtlUpCasCheckTender() - Affect Check Tender Total of Cashier
*           VOID TtlUpCasChargeTender() - Affect Charge Tender Total of Cashier
*           VOID TtlUpCasMiscTender() - Affect Misc Tender Total of Cashier
*           VOID TtlUpCasForeignTotal() - Affect Foregin Total of Cashier
*           VOID TtlUpCasServiceTotal() - Affect Service Total of Cashier
*           VOID TtlUpCasAddCheckTotal() - Affect AddCheck Total of Cashier
*           VOID TtlUpCasChargeTips() - Affect Charge Tips Total of Cashier
*           VOID TtlUpCasTransCancel() - Affect Transaction Cancel Total of Cashier
*           VOID TtlUpCasMiscVoid() - Affect Misc Void Total of Cashier
*           VOID TtlUpCasAudaction() - Affect Audaction Total of Cashier
*           VOID TtlUpCasNoSaleCount() - Affect Nosale of Cashier
*           VOID TtlUpCasItemProduct() - Affect Item Productivity of Cashier
*           VOID TtlUpCasNoOfPerson()  - Affect No of Person of Casanicial
*           VOID TtlUpCasNoOfChkOpen() - Affect No Check Open of Cashier
*           VOID TtlUpCasNoOfChkClose() - Affect No of Check Close of Cashier
*           VOID TtlUpCasCustomer() - Affect Customer Counter of Casanicial
*           VOID TtlUpCasHashDept() - Affect Hash Dept Total of Cashier
*           VOID TtlUpCasBonus()    - Affect Bonus Total of Cashier
*           VOID TtlUpCasConsTax()  - Affect Consolidate Tax Total of Cashier
*           VOID TtlUpCasPostReceipt() - Affect PostReceipt Co., R3.1
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:Kteari     :
* Dec-03-93:02.00.03:K.You      : add 4 tender total
* Apr-08-94:00.00.04: Yoshiko.J : delete OffTend
* Apr-19-94:00.00.05:K.You      : bug fixed (E-26)(mod. TtlUpCasMiscTender)
* Mar-03-95:03.00.00:hkato      : R3.0
* Dec-26-95:03.01.00:T.Nakahata : R3.1 Initial (Add Some Elements)
*   Regular Discount (2 -> 6)
*   Add Post Receipt Count at Server/Cashier Total
* Jun-13-97:03.01.12:M.Suzuki   : Correct Regular Discount 3-6 Update(TAR 57146)
* Nov-30-99:01.00.00: hrkato    : Saratoga (Money)
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
**Synopsis:     VOID TtlCasupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, 
*                                  TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                  TTLCSCASTOTAL *pTmpBuff);
*
*    Input:     TTLTUNTRANQUAL *pTtlTranQualBuff - Pointer of Transaction 
*                                                  Qualifier Data
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSCASTOTAL *pTmpBuff          - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff          - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*
** Description  This function updates the Cashier Total File.        R3.1
*============================================================================
*/
VOID TtlCasupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, 
                  TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
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
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_PVOID) {   /* Check Preselect Void */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_PVOID;  /* Set Preselect Void Status */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) {   /* Check transaction return */
        sSign *= -1;                /* Set Sign */
        usStatus |= TTL_TUP_TRETURN;  /* Set Preselect Void Status */
        usStatus |= TTL_TUP_IGNORE;   /* Set ignore data status to be reset only if this ia returned item */
    }
    if (pTtlTranQualBuff->TtlTranCurQual.fsCurStatus & CURQUAL_TRAINING) {  /* Check Training Operation */
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
        case CLASS_NEWCHECK:                   /* New Check */
            TtlUpCasNoOfChkOpen(pTmpBuff, 1);    /* Affect No of Checks Opened Counter */
            break;                               /* End Affection */
/*      default:    Not Used */
        }
        return;                     

/* -------------- Sales(Dept/PLU) Affection --------------- */
/* -------------- Modifier Discount Affection --------------- */
    case CLASS_ITEMSALES:           /* sales */
		pTmpBuff->uchResetStatus |= TTL_NOTRESET;      // indicate that this data has been changed.

		fbMod = pTtlTranUpBuff->TtlItemSales.fbModifier;    // prepare to pass the modifier data into totaling functions
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemSales.fbModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
            sSign *= -1;                   /* Set Sign */
			if (pTtlTranUpBuff->TtlItemSales.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemSales.fbModifier;
			} else {
				// standard void so mark it as such
				usStatus |= TTL_TUP_MVOID;     /* Set Momentary Void Status */
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
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[0] & PLU_MINUS) {
                                    /* Check Credit Dept/PLU Status */
                usStatus |= TTL_TUP_CREDT;       /* Set Credit Dept/PLU Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_HASH) {
                /* Check Hash Dept/PLU Status. See the comments titled What is a Hash Department in rptdept.c */
                usStatus |= TTL_TUP_HASH;        /* Set Hash Dept/PLU Status */
            }
            if ((pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_HASH_FINANC) == 0) {
                                    /* Check Hash Dept/PLU Not Affection Status */
                usStatus |= TTL_TUP_HASHAFFECT;  /* Set Hash Dept/PLU Not Affection Status */
            }
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
                                    /* Check Scalable */
                if (pTtlTranUpBuff->TtlItemSales.lQTY < 0) {
                    sCounter = -1;       /* Set Quantity is -1 */
                } else {
                    sCounter = 1;       /* Set Quantity is 1 */
                }
            } else {
                sCounter = (SHORT)(pTtlTranUpBuff->TtlItemSales.lQTY / 1000L);
                                    /* Set Quantity */
            }
			//SR47 Affect Double/Triple Coupon to Bonus
			if((ParaMDCCheck(MDC_DOUBLE2_ADR, ODD_MDC_BIT0) || ParaMDCCheck(MDC_TRIPLE2_ADR, ODD_MDC_BIT0)) && (pTtlTranUpBuff->TtlItemSales.lDiscountAmount < 0)){
				if (pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex > 0)
					pTmpBuff->Bonus[(pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex -1)].lAmount += pTtlTranUpBuff->TtlItemSales.lDiscountAmount;
			}
            TtlCSales(fbMod, usStatus, pTtlTranUpBuff->TtlItemSales.lProduct, sCounter, (USHORT)(pTtlTranUpBuff->TtlItemSales.ControlCode.usBonusIndex -1), pTmpBuff);
                                    /* Update Main Product */
            if (pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPT || /* R3.0 */
                pTtlTranUpBuff->TtlClass.uchMinorClass == CLASS_DEPTITEMDISC) {
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
                 + pTtlTranUpBuff->TtlItemSales.uchChildNo              /* R3.0 */
                 + pTtlTranUpBuff->TtlItemSales.uchPrintModNo); i++) {
                if ( _tcsncmp(pTtlTranUpBuff->TtlItemSales.Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) == 0 ) {   /* 2172 */
                    continue;
                }
                usStatus &= ~TTL_TUP_CREDT;             /* Reset Credit Status */
                lAmount = (DCURRENCY)sSign * labs((LONG)sCounter) * (DCURRENCY)pTtlTranUpBuff->TtlItemSales.Condiment[i].lUnitPrice;   /* Calculate Condiment Product */
                if (pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[0] & PLU_MINUS) {
                    usStatus |= TTL_TUP_CREDT;
                                    /* Set Credit Dept/PLU Status */
                }
                if ((pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.auchPluStatus[5] & ITM_HASH_FINANC) == 0) {
                    /* Check Hash Dept/PLU Not Affection Status */
                    usStatus &= ~TTL_TUP_CREDT;     /* Reset Credit Dept/PLU Status */
                }
                TtlCSales(fbMod, usStatus, lAmount, sCounter, (USHORT)(pTtlTranUpBuff->TtlItemSales.Condiment[i].ControlCode.usBonusIndex -1), pTmpBuff);
                                    /* Update Condiment Product */
            }
            break;                  /* End Affection */

        /* ----- Modifier Discount Affection ----- */
        case CLASS_DEPTMODDISC:     /* Dept W/Modifier Discount */
        case CLASS_PLUMODDISC:      /* PLU W/Modifier Discount */
        case CLASS_SETMODDISC:      /* Set Menu W/Modifier Discount */
        case CLASS_OEPMODDISC:      /* OEP Menu W/Modifier Discount,    R3.0 */
            if (pTtlTranUpBuff->TtlItemSales.ControlCode.auchPluStatus[5] & ITM_MOD_DISC_SUR) {             /* Check Surcharge Operation */
                usStatus |= TTL_TUP_SURCHARGE;    /* Set Surcharge Status */
            }
            TtlCModDiscount(usStatus, pTtlTranUpBuff->TtlItemSales.lDiscountAmount, sSign, pTmpBuff);    /* Update Modifier Discount Product */
            break;                  /* End Affection */
/*      default:    Not Used */
        }
        return;

/* -------------- Discount Affection --------------- */
    case CLASS_ITEMDISC:            /* Discount */
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & VOID_MODIFIER) { /* Check VOID Modifier */
            sSign *= -1;                         /* Set Sign */
			if (pTtlTranUpBuff->TtlItemDisc.fbDiscModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				fbMod = pTtlTranUpBuff->TtlItemDisc.fbDiscModifier;
			} else {
				usStatus |= TTL_TUP_MVOID;       /* Set Momentary Void Status */
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
        case CLASS_REGDISC3:        /* Regular Discount 3, R3.1 */
        case CLASS_REGDISC4:        /* Regular Discount 4, R3.1 */
        case CLASS_REGDISC5:        /* Regular Discount 5, R3.1 */
        case CLASS_REGDISC6:        /* Regular Discount 6, R3.1 */
            if (pTtlTranUpBuff->TtlItemDisc.auchDiscStatus[1] & TTL_MDC_SURCHARGE) {
                                    /* Check Surcharge Operation */
                usStatus |= TTL_TUP_SURCHARGE;       /* Set Surcharge Status */
            }
            TtlCDiscount(fbMod, usStatus, pTtlTranUpBuff->TtlItemDisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);
                                    /* Update Discount Product */
            break;                  /* End Affection */

        case CLASS_CHARGETIP:       /* charge tip */
        case CLASS_CHARGETIP2:      /* charge tip, V3.3 */
        case CLASS_CHARGETIP3:      /* charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP:   /* charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP2:  /* charge tip, V3.3 */
        case CLASS_AUTOCHARGETIP3:  /* charge tip, V3.3 */
            TtlCDiscount(fbMod, usStatus, pTtlTranUpBuff->TtlItemDisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);
                                    /* Update Discount Product */
            break;                  /* End Affection */
/*      default:    Not Used */
        }
        return;

/* -------------- Coupon Affection, R3.0 --------------- */
    case CLASS_ITEMCOUPON:          /* Coupon,        R3.0 */
		fbMod = 0;
        if (pTtlTranUpBuff->TtlItemCoupon.fbModifier & VOID_MODIFIER) {   /* Check VOID Modifier */
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
				TtlUpCasBonus(pTmpBuff, (pTtlTranUpBuff->TtlItemCoupon.lAmount * -1), pTtlTranUpBuff->TtlItemCoupon.uchNoOfItem, (USHORT)(pTtlTranUpBuff->TtlItemCoupon.usBonusIndex-1));
			}
			else
			{
				TtlUpCasBonus(pTmpBuff, (pTtlTranUpBuff->TtlItemCoupon.lAmount * -1), (SHORT)((SHORT)pTtlTranUpBuff->TtlItemCoupon.uchNoOfItem * -1), (USHORT)(pTtlTranUpBuff->TtlItemCoupon.usBonusIndex-1));
			}
		}

        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_COMCOUPON:
            TtlCCpn(fbMod, usStatus, pTtlTranUpBuff->TtlItemCoupon.lAmount, sSign, pTmpBuff);    /* Update Coupon Product */
            break;
        case CLASS_UPCCOUPON:
            TtlCUCpn(fbMod, usStatus, pTtlTranUpBuff->TtlItemCoupon.lAmount, sSign, pTmpBuff);   /* Update Coupon Product */
            break;
        }
        return;

/* -------------- Total Affection --------------- */
    case CLASS_ITEMTOTAL:           /* Total key */
        TtlCTotal(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
        return;                     /* End Affection */
        
/* -------------- Tender Affection --------------- */
    case CLASS_ITEMTENDER:          /* tender */
        if (pTtlTranUpBuff->TtlItemTender.fbModifier & VOID_MODIFIER) {
                                    /* Check VOID Modifier */
            sSign *= -1;            /* Set Sign */
			if (pTtlTranUpBuff->TtlItemTender.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
				usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
			} else {
				usStatus |= TTL_TUP_MVOID;         /* Set Momentary Void Status */
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
        TtlCTender(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
        return;                     /* End Affection */

/* -------------- Misc Affection --------------- */
    case CLASS_ITEMMISC:            /* Misc. Transaction */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_NOSALE:          /* No Sale */
            TtlUpCasNoSaleCount(pTmpBuff);     /* Affect No Sale Counter */
            break;                  /* End Affection */

        case CLASS_PO:              /* Paid Out */
        case CLASS_RA:              /* Received on Account */
        case CLASS_TIPSPO:          /* Tips paid out */
        case CLASS_TIPSDECLARED:    /* Tips Declared, V3.3 */
            if (pTtlTranUpBuff->TtlItemMisc.fbModifier & VOID_MODIFIER) {
                                    /* Check VOID Modifier */
                sSign *= -1;        /* Set Sign */
				if (pTtlTranUpBuff->TtlItemMisc.fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {   /* Check Returns Modifier */
					usStatus |= TTL_TUP_MRETURN;     /* Set Momentary Return Status */
				} else {
					usStatus |= TTL_TUP_MVOID;     /* Set Momentary Void Status */
				}
            }
            TtlCmisc(usStatus, pTtlTranUpBuff->TtlItemMisc.lAmount, sSign, pTtlTranUpBuff->TtlClass.uchMinorClass, pTmpBuff);
            break;                  /* Execute Affection */

        case CLASS_MONEY:           /* Money,   Saratoga */
            if (pTtlTranUpBuff->TtlItemMisc.fbModifier & VOID_MODIFIER) {
                pTtlTranUpBuff->TtlItemMisc.lAmount *= -1L;
            }
            TtlUpCasMoney(pTmpBuff, pTtlTranUpBuff->TtlItemMisc.lAmount, pTtlTranUpBuff->TtlItemMisc.uchTendMinor);
            break;

/*      default:    Not Used */
        }
        return;

/* -------------- Other Affection --------------- */
    case CLASS_ITEMAFFECTION:       /* affection */
        /* VAT affection, V3.3 */
        switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
        case CLASS_SERVICE_VAT:          /* VATable Service Total */
        case CLASS_SERVICE_NON:          /* Non VATable Service Total */
        case CLASS_VATAFFECT:            /* VAT Affection */
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status for the tzx on returned items */
            if (pTtlTranUpBuff->TtlItemAffect.fbModifier & EXCLUDE_VAT_SER) {
                usStatus |= TTL_TUP_EXCLUDE;
            }
            TtlCVatService(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
            break;
        case CLASS_LOANAFFECT:          /* Loan,    Saratoga */
        case CLASS_PICKAFFECT:          /* Pickup,  Saratoga */
            TtlCLoanPick(pTtlTranUpBuff, pTmpBuff);
            break;

        case CLASS_LOANCLOSE:           /* Saratoga */
        case CLASS_PICKCLOSE:
            TtlCLoanPickClose(pTtlTranUpBuff, pTmpBuff);
            break;

		case CLASS_EPT_ERROR_AFFECT:
			TtlCEptError(pTtlTranUpBuff, pTmpBuff);
			break;

        default:
			usStatus &= ~TTL_TUP_IGNORE;     /* Reset ignore status for the tzx on returned items */
            TtlCAffect(usStatus, sSign, pTtlTranUpBuff, pTmpBuff);
            break;
        }
        return;

    /* === Add Post Receipt Count (Release 3.1) BEGIN === */
    case CLASS_ITEMOTHER:
        TtlCPostReceipt( pTtlTranUpBuff, pTmpBuff );
        return;
    /* === Add Post Receipt Count (Release 3.1) END === */

/*  default: Not Used */
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlCSales(USHORT usStatus, LONG lAmount, 
*                              SHORT sCounter,  USHORT uchBOffset, 
*                              TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchBOffset        - Bounus Total Offset
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function affects Cashier in ItemSales of Major Class.
*
*============================================================================
*/
VOID TtlCSales(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, 
               USHORT uchBOffset, TTLCSCASTOTAL *pTmpBuff)
{

	if (usStatus & TTL_TUP_IGNORE) return;

    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == (TTL_TUP_TRETURN)) {   /* Check transaction return Status */
        TtlUpCasTransactionReturn(fbMod, pTmpBuff, lAmount, sCounter);   /* Affect transaction return Total/Counter */
	}

    if ((usStatus & TTL_TUP_CREDT) == 0) {    /* Check Credit Dept/PLU Status */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {     /* Check Hash Item & Not Affection Control */
                TtlUpCasDGGTotal(pTmpBuff, lAmount);      /* Affect Daily Gross Total */
            }
            break;

        case TTL_TUP_MVOID:
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {     /* Check Hash Item & Not Affection Control */
                TtlUpCasPlusVoid(pTmpBuff, lAmount);      /* Affect Plus Void Total/Counter */
            }
            break;

        case TTL_TUP_PVOID:
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {     /* Check Hash Item & Not Affection Control */
                TtlUpCasPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
            }
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            if ((usStatus & (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) != (TTL_TUP_HASH | TTL_TUP_HASHAFFECT)) {
                TtlUpCasPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
                TtlUpCasMiscVoid(pTmpBuff, lAmount);      /* Affect Misc Void Total/Counter */
            }
            break;
/*      default:    Not Used */
        }
        if (usStatus & TTL_TUP_HASH) {                       /* Check Hash Dept/PLU Status */
            TtlUpCasHashDept(pTmpBuff, lAmount, sCounter);   /* Affect Hash Dept Total/Counter */
/*        } else {
            TtlUpCasBonus(pTmpBuff, lAmount, sCounter, uchBOffset);
*/                                    /* Affect Bouns Total/Counter */
        }
        TtlUpCasBonus(pTmpBuff, lAmount, sCounter, uchBOffset); /* V3.3, FVT#11 */

    } else {
        /* V3.3 */
        if (usStatus & TTL_TUP_HASH) {                       /* Check Hash Dept/PLU Status */
            TtlUpCasHashDept(pTmpBuff, lAmount, sCounter);   /* Affect Hash Dept Total/Counter */
        } else {
            TtlUpCasConsCoupon(pTmpBuff, lAmount, sCounter); /* Affect Consol. Coupon Total/Counter */
        }
        TtlUpCasBonus(pTmpBuff, lAmount, sCounter, uchBOffset); /* V3.3, FVT#11 */
        
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID)) {
        case TTL_TUP_MVOID:
        case TTL_TUP_PVOID:
        case (TTL_TUP_MVOID | TTL_TUP_PVOID):
            TtlUpCasMiscVoid(pTmpBuff, lAmount);             /* Affect Misc Void Total/Counter */
            break;
/*      default:    Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCModDiscount(USHORT usStatus, LONG lAmount, 
*                                   SHORT sSign, TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Discount of Major Class.
*
*============================================================================
*/
VOID TtlCModDiscount(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, 
                     TTLCSCASTOTAL *pTmpBuff)
{
//    TtlUpCasModiDisc(pTmpBuff, lAmount, sSign);    /* Affect Modifier Discount Total/Counter */

	if (usStatus & TTL_TUP_IGNORE) return;

    TtlUpdateDiscountTotal(&(pTmpBuff->ModiDisc), lAmount, sSign);    /* Affect Modifier Discount Total/Counter */


    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {   /* Check transaction return Status */
		USHORT  fbMod = 0;
        TtlUpCasTransactionReturn(fbMod, pTmpBuff, lAmount, 0);   /* Affect transaction return Total/Counter */
	}

    if ((usStatus & TTL_TUP_SURCHARGE) == 0) {
		/* --- Only Discount Affection --- */ 
        if (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {    /* Check Momentary Void */
            TtlUpCasMiscVoid(pTmpBuff, lAmount);             /* Affect Misc Void Total/Counter */
        }
    } else {
		/* --- Only Surcharge Affection --- */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
            TtlUpCasDGGTotal(pTmpBuff, lAmount);             /* Affect Daily Gross Total */
            break;

        case TTL_TUP_MVOID:
            TtlUpCasPlusVoid(pTmpBuff, lAmount);             /* Affect Plus Void Total/Counter */
            break;

        case TTL_TUP_PVOID:
            TtlUpCasPreVoid(pTmpBuff, lAmount, 0);           /* Affect Preselect Void Total/Counter */
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            TtlUpCasPreVoid(pTmpBuff, lAmount, 0);           /* Affect Preselect Void Total/Counter */
            TtlUpCasMiscVoid(pTmpBuff, lAmount);             /* Affect Misc Void Total/Counter */
            break;
/*      default: Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCDiscount(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, UCHAR uchMinorClass, 
*                                 TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               UCHAR uchMinorClass     - Minor Class
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Discount of Major Class.
*
*============================================================================
*/
VOID TtlCDiscount(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, 
                    UCHAR uchMinorClass, TTLCSCASTOTAL *pTmpBuff)
{
    UCHAR   uchOffset;

	if (usStatus & TTL_TUP_IGNORE) return;

    switch (uchMinorClass) {
    case CLASS_ITEMDISC1:           /* Affection Item Discount */
		// pass on through and do the update below.
//        TtlUpCasItemDisc(pTmpBuff, lAmount, sSign);                  /* Affect Item Discount Total/Counter */
        break;

    case CLASS_REGDISC1:            /* Affection Regular Discount 1 */
    case CLASS_REGDISC2:            /* Affection Regular Discount 2 */
    case CLASS_REGDISC3:            /* Affection Regular Discount 3 */
    case CLASS_REGDISC4:            /* Affection Regular Discount 4 */
    case CLASS_REGDISC5:            /* Affection Regular Discount 5 */
    case CLASS_REGDISC6:            /* Affection Regular Discount 6 */
        uchOffset = (UCHAR)(uchMinorClass - CLASS_REGDISC1);
//        TtlUpCasRegDisc(pTmpBuff, lAmount, sCounter, uchOffset);        /* Affect Regular Discount Total/Counter */
		TtlUpdateDiscountTotal (&(pTmpBuff->RegDisc[uchOffset]), lAmount, sCounter);      /* Affect Item Discount Total/Counter */
        break;

    case CLASS_CHARGETIP:           /* Affection Charge Tips */
    case CLASS_CHARGETIP2:          /* Affection Charge Tips, V3.3 */
    case CLASS_CHARGETIP3:          /* Affection Charge Tips, V3.3 */
        uchOffset = (UCHAR)(uchMinorClass - CLASS_CHARGETIP);
//        TtlUpCasChargeTips(pTmpBuff, lAmount, sCounter, uchOffset);     /* Affect Charge Tips Total/Counter */
		TtlUpdateDiscountTotal (&(pTmpBuff->ChargeTips[uchOffset]), lAmount, sCounter);      /* Affect Item Discount Total/Counter */
        break;

    case CLASS_AUTOCHARGETIP:       /* Affection Charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP2:      /* Affection Charge Tips, V3.3 */
    case CLASS_AUTOCHARGETIP3:      /* Affection Charge Tips, V3.3 */
        uchOffset = (UCHAR)(uchMinorClass - CLASS_AUTOCHARGETIP);
//        TtlUpCasChargeTips(pTmpBuff, lAmount, sCounter, uchOffset);     /* Affect Charge Tips Total/Counter */
		TtlUpdateDiscountTotal (&(pTmpBuff->ChargeTips[uchOffset]), lAmount, sCounter);      /* Affect Item Discount Total/Counter */
        break;
	/*  default: Not Used */
    }

    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {  /* Check transaction return Status */
        TtlUpCasTransactionReturn(fbMod, pTmpBuff, lAmount, 0);   /* Affect transaction return Total/Counter */
	}

    if ((usStatus & TTL_TUP_SURCHARGE) == 0) {
		/* --- Only Discount Affection --- */ 
        if (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID)) {   /* Check Momentary Void */
            TtlUpCasMiscVoid(pTmpBuff, lAmount);            /* Affect Misc Void Total/Counter */
        }
    } else {
		/* --- Only Surcharge Affection --- */ 
        switch (usStatus & (TTL_TUP_PVOID | TTL_TUP_MVOID | TTL_TUP_TRETURN | TTL_TUP_IGNORE)) {
		case TTL_TUP_TRETURN:
			// fall through as this is a new item added for an exchange
        case 0:
            TtlUpCasDGGTotal(pTmpBuff, lAmount);      /* Affect Daily Gross Total */
            break;

        case TTL_TUP_MVOID:
            TtlUpCasPlusVoid(pTmpBuff, lAmount);      /* Affect Plus Void Total/Counter */
            break;

        case TTL_TUP_PVOID:
            TtlUpCasPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
            break;

        case (TTL_TUP_PVOID + TTL_TUP_MVOID):
            TtlUpCasPreVoid(pTmpBuff, lAmount, 0);    /* Affect Preselect Void Total/Counter */
            TtlUpCasMiscVoid(pTmpBuff, lAmount);      /* Affect Misc Void Total/Counter */
            break;
		/*      default: Not Used */
        }
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCCpn(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Coupon of Major Class.
*                    R3.0
*============================================================================
*/
VOID TtlCCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSCASTOTAL *pTmpBuff)
{
    TtlUpCasCpn(pTmpBuff, lAmount, sSign);
    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {  /* Check transaction return Status */
        TtlUpCasTransactionReturn(fbMod, pTmpBuff, lAmount, 0);   /* Affect transaction return Total/Counter */
	}
    if (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {
        TtlUpCasMiscVoid(pTmpBuff, lAmount); /* Affect Misc Void Total/Counter */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlUCpn(USHORT usStatus, LONG lAmount, 
*                                 SHORT sSign, TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Coupon of Major Class.
*                    R3.0
*============================================================================
*/
VOID TtlCUCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSCASTOTAL *pTmpBuff)
{
    TtlUpCasUCpn(pTmpBuff, lAmount, sSign);
    if ((usStatus & (TTL_TUP_TRETURN | TTL_TUP_IGNORE)) == TTL_TUP_TRETURN) {  /* Check transaction return Status */
        TtlUpCasTransactionReturn(fbMod, pTmpBuff, lAmount, 0);   /* Affect transaction return Total/Counter */
	}
    if (usStatus & (TTL_TUP_PVOID + TTL_TUP_MVOID)) {
        TtlUpCasMiscVoid(pTmpBuff, lAmount); /* Affect Misc Void Total/Counter */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCTotal(USHORT usStatus, SHORT sSign, 
*                              TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                              TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Total of Major Class.
*
*============================================================================
*/
VOID TtlCTotal(USHORT usStatus, SHORT sSign, 
               TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
{
    UCHAR   uchOffset;
    UCHAR uchTtlType;

	if (usStatus & TTL_TUP_IGNORE) return;

	if (usStatus & CURQUAL_TRETURN) {
		// force increment of counters if this is a return
		// which may have negative sign
		sSign = 1;
	}

    TtlUpCasServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, pTtlTranUpBuff->TtlItemTotal.uchMinorClass);   /* Affect Service Total Total/Counter */
    uchTtlType = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] / CHECK_TOTAL_TYPE);
    if ((uchTtlType == PRT_NOFINALIZE) || (uchTtlType == PRT_TRAY1) || (uchTtlType == PRT_TRAY2)) {
                                    /* Check Not Casalize Total */
        return;
    }
    switch(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] % CHECK_TOTAL_TYPE) {
    case CLASS_TENDER1:                                                           /* Tender #1 */
        TtlUpCasCashTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign);    /* Affect Cash Tender */
        break;

    case CLASS_TENDER2:                                                           /* Tender #2 */
        TtlUpCasCheckTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign);   /* Affect Check Tender */
        break;

    case CLASS_TENDER3:                                                           /* Tender #3 */
        TtlUpCasChargeTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign);  /* Affect Charge Tender */
        break;

    case CLASS_TENDER4:                         /* Tender #4 */
    case CLASS_TENDER5:                         /* Tender #5 */
    case CLASS_TENDER6:                         /* Tender #6 */
    case CLASS_TENDER7:                         /* Tender #7 */
    case CLASS_TENDER8:                         /* Tender #8 */
    case CLASS_TENDER10:                        /* Tender #8 */
    case CLASS_TENDER11:                        /* Tender #8 */
    case CLASS_TENDER12:                        /* Tender #8 */
    case CLASS_TENDER13:                        /* Tender #8 */
    case CLASS_TENDER14:                        /* Tender #8 */
    case CLASS_TENDER15:                        /* Tender #8 */
    case CLASS_TENDER16:                        /* Tender #8 */
    case CLASS_TENDER17:                        /* Tender #8 */
    case CLASS_TENDER18:                        /* Tender #8 */
    case CLASS_TENDER19:                        /* Tender #8 */
    case CLASS_TENDER20:                        /* Tender #8 */
        uchOffset = (UCHAR)(pTtlTranUpBuff->TtlItemTotal.auchTotalStatus[0] % CHECK_TOTAL_TYPE) - (UCHAR)CLASS_TENDER4;
        TtlUpCasMiscTender(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI, sSign, uchOffset);
                                    /* Affect Misc Tender 1 */
                                    /* Affect Misc Tender 2 */
                                    /* Affect Misc Tender 3 */
                                    /* Affect Misc Tender 4 */
                                    /* Affect Misc Tender 5 */
        break;

	/*      default: Not Used */
    }

	if ((usStatus & (CURQUAL_PRESELECT_MASK | TTL_TUP_TRETURN)) == 0)  // do not update the customer counter if Transaction Void (CURQUAL_PVOID) or transaction return (CURQUAL_TRETURN) was done
		TtlUpCasCustomer(pTmpBuff, sSign);      /* Affect Customer Counter */

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
        if (pTtlTranUpBuff->TtlItemTotal.lMI <= 0L) {                               /* Check Audaction Opeartion */
            TtlUpCasAudaction(pTmpBuff, pTtlTranUpBuff->TtlItemTotal.lMI);          /* Affect Audaction Total/Counter */
        }
        break;

    case TTL_TUP_PVOID:
        if (pTtlTranUpBuff->TtlItemTotal.lMI >= 0L) {                                /* Check Audaction Opeartion */
            TtlUpCasAudaction(pTmpBuff, (pTtlTranUpBuff->TtlItemTotal.lMI * -1L));   /* Affect Audaction Total/Counter */
        }
        break;
/*      default: Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCTender(USHORT usStatus, SHORT sSign, 
*                               TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                               TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Tender of Major Class.
*                                                           Saratoga
*============================================================================
*/
VOID TtlCTender(USHORT usStatus, SHORT sSign, 
                TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
{
    UCHAR   uchOffset;

	if (usStatus & TTL_TUP_IGNORE) return;

	if (usStatus & (TTL_TUP_MRETURN | TTL_TUP_TRETURN | TTL_TUP_PVOID | TTL_TUP_MVOID)) {
		// force increment of counters if this is a return
		// which may have negative sign
		sSign = 1;
	}

	if (pTtlTranUpBuff->TtlItemTender.fbReduceStatus & REDUCE_PREAUTH) {
		TtlUpCasServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, pTtlTranUpBuff->TtlItemTender.uchMinorClass);    /* Affect Service Total Total/Counter */
		return;
	}

    /* check off line tender */
    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
    case CLASS_TENDER1:             /* Tender #1 */
        TtlUpCasCashTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign);     /* Affect Cash Tender */
        break;

    case CLASS_TENDER2:             /* Tender #2 */
        TtlUpCasCheckTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign);    /* Affect Check Tender */
        break;

    case CLASS_TEND_FSCHANGE:       /* FS Change,   Saratoga */
        TtlUpCasFSChange(pTmpBuff, &pTtlTranUpBuff->TtlItemTender, sSign);
        break;

    case CLASS_TEND_TIPS_RETURN:       /* FS Change,   Saratoga */
		{
			ITEMTENDERCHARGETIPS  *pChargeTips = &(ITEMTENDER_CHARGETIPS(&(pTtlTranUpBuff->TtlItemTender)));
			TtlCDiscount(0, usStatus, pTtlTranUpBuff->TtlItemTender.lGratuity, sSign, pChargeTips->uchMinorClass, pTmpBuff);
		}
        break;

    case CLASS_TENDER3:             /* Tender #3 */
        TtlUpCasChargeTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign);   /* Affect Charge Tender */
        break;

    case CLASS_TENDER4:             /* Tender #4 */
    case CLASS_TENDER5:             /* Tender #5 */
    case CLASS_TENDER6:             /* Tender #6 */
    case CLASS_TENDER7:             /* Tender #7 */
    case CLASS_TENDER8:             /* Tender #8 */
    case CLASS_TENDER9:             /* Tender #9 */
    case CLASS_TENDER10:            /* Tender #10 */
    case CLASS_TENDER11:            /* Tender #11 */
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
        TtlUpCasMiscTender(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount, sSign, uchOffset);
        break;

    case CLASS_FOREIGN1:            /* FC Tender #1 */
    case CLASS_FOREIGN2:            /* FC Tender #2 */
    case CLASS_FOREIGN3:            /* FC Tender #3,    Saratoga */
    case CLASS_FOREIGN4:            /* FC Tender #4,    Saratoga */
    case CLASS_FOREIGN5:            /* FC Tender #5,    Saratoga */
    case CLASS_FOREIGN6:            /* FC Tender #6,    Saratoga */
    case CLASS_FOREIGN7:            /* FC Tender #7,    Saratoga */
    case CLASS_FOREIGN8:            /* FC Tender #8,    Saratoga */
        uchOffset = pTtlTranUpBuff->TtlClass.uchMinorClass - (UCHAR)CLASS_FOREIGN1;
        TtlUpCasForeignTotal(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lForeignAmount, sSign, uchOffset);
        break;

/*      default: Not Used */
    }    

    /* --- Saratoga Do not Coutup in case of FS Change,  Dec/4/2000 --- */
    if (pTtlTranUpBuff->TtlClass.uchMinorClass != CLASS_TEND_FSCHANGE) {
        if (usStatus & TTL_TUP_MVOID) {                                                /* Check Momentary Void */
            TtlUpCasMiscVoid(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount);   /* Affect Misc Void Total/Counter */
        } else {
            if (pTtlTranUpBuff->TtlItemTender.lBalanceDue == 0L) {                     /* Check Balance Due */
				if ((usStatus & (CURQUAL_PRESELECT_MASK | TTL_TUP_TRETURN)) == 0)  // do not update the customer counter if Transaction Void (CURQUAL_PVOID) or transaction return (CURQUAL_TRETURN) was done
					TtlUpCasCustomer(pTmpBuff, sSign);                                     /* Affect Customer Counter */
            }
        }
    }

    if (pTtlTranUpBuff->TtlItemTender.lChange != 0L) {                                         /* Check Change Operation */
        TtlUpCasCashTender(pTmpBuff, (pTtlTranUpBuff->TtlItemTender.lChange * -1L), 0);        /* Affect Change Total */
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
        if (pTtlTranUpBuff->TtlItemTender.lTenderAmount <= 0L) {                               /* Check Audaction Opeartion */
            TtlUpCasAudaction(pTmpBuff, pTtlTranUpBuff->TtlItemTender.lTenderAmount);          /* Affect Audaction Total/Counter */
        }
        break;

    case TTL_TUP_PVOID:
        if (pTtlTranUpBuff->TtlItemTender.lTenderAmount >= 0L) {                               /* Check Audaction Opeartion */
            TtlUpCasAudaction(pTmpBuff, (pTtlTranUpBuff->TtlItemTender.lTenderAmount * -1L));  /* Affect Audaction Total/Counter */
        }
        break;
/*      default: Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCMisc(USHORT usStatus, LONG lAmount, 
*                             SHORT sSign, UCHAR uchMinorClass, 
*                             TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               LONG lAmount            - Affection Amount
*               SHORT sSign             - Affection Sign for Counter
*               UCHAR uchMinorClass     - Minor Class
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Misc of Major Class.
*
*============================================================================
*/
VOID TtlCmisc(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, 
              UCHAR uchMinorClass, TTLCSCASTOTAL *pTmpBuff)
{
	if (usStatus & TTL_TUP_IGNORE) return;

    switch (uchMinorClass) {
    case CLASS_PO:                                        /* Affection Paid Out */
        TtlUpCasPaidOut(pTmpBuff, lAmount, sSign);        /* Affect Paid Out Total/Counter */
        TtlUpCasCashTender(pTmpBuff, lAmount, sSign);     /* Affect Cash Tender Total/Counter */ 
        break;

    case CLASS_RA:                                        /* Affection Received on Account */
        TtlUpCasRecvAcount(pTmpBuff, lAmount, sSign);     /* Affect Received on Account Total/Counter */
        TtlUpCasCashTender(pTmpBuff, lAmount, sSign);     /* Affect Cash Tender Total/Counter */ 
        break;

    case CLASS_TIPSPO:                                    /* Affection Tips paid out */
        TtlUpCasTipsPaid(pTmpBuff, lAmount, sSign);       /* Affect Tips Paid Out Total/Counter */
        TtlUpCasCashTender(pTmpBuff, lAmount, sSign);     /* Affect Cash Tender Total/Counter */ 
        break;

    case CLASS_TIPSDECLARED:                              /* Tips Declared, V3.3 */
        TtlUpCasDeclaredTips(pTmpBuff, lAmount, sSign);   /* Affect Declared Tips Total/Counter */
        break;

/*  default: Not Used */
    }
    if (usStatus & TTL_TUP_MVOID) {                /* Check Momentary Void */
        TtlUpCasMiscVoid(pTmpBuff, lAmount);       /* Affect Misc Void Total/Counter */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCAffect(USHORT usStatus, SHORT sSign, 
*                               TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                               TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Affection of Major Class.
*
*============================================================================
*/
VOID TtlCAffect(USHORT usStatus, SHORT sSign, 
                TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
{
    UCHAR i, uchOffset;
    SHORT sCount;

	if (usStatus & TTL_TUP_IGNORE) return;

    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {
    case CLASS_HOURLY:              /* Hourly Total */
        TtlUpCasNoOfPerson(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.sNoPerson);            /* Affect No of Person Counter */
        TtlUpCasItemProduct(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.sItemCounter);  /* Affect Item Productivite Counter */
        if (usStatus & TTL_TUP_PVOID) {
            TtlUpCasPreVoid(pTmpBuff, 0L, 1);      /* Affect Preselect Void Total/Counter */
        }
        break;                      /* End Affection */

    case CLASS_SERVICE:             /* Service Total */
        TtlUpCasServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign, pTtlTranUpBuff->TtlItemAffect.uchTotalID);
                                    /* Affect Service Total Total/Counter */
        break;                      /* End Affection */

    case CLASS_CANCELTOTAL:         /* Cancel Total */
        TtlUpCasTransCancel(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lCancel);    /* Affect Cancel Transaction Total/Counter) */
        break;                      /* End Affection */

    case CLASS_CASHIERCHECK:                   /* Cashier close check */
        TtlUpCasNoOfChkClose(pTmpBuff, 1);        /* Affect No of Check Close Counter */
        break;                                    /* End Affection */

    case CLASS_ADDCHECKTOTAL:       /* Addcheck total */
    case CLASS_MANADDCHECKTOTAL:    /* Manual Addcheck Total */
        TtlUpCasAddCheckTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, pTtlTranUpBuff->TtlItemAffect.sItemCounter, pTtlTranUpBuff->TtlItemAffect.uchAddNum);
                                    /* Affect Cancel Transaction Total/Counter) */
        break;                      /* End Affection */

    case CLASS_CASHIEROPENCHECK:          /* Casher open check */
        TtlUpCasNoOfChkOpen(pTmpBuff, 1);      /* Affect No of Checks Opened Counter */
        break;                                 /* End Affection */

    case CLASS_TAXAFFECT:           /* US/Canadian Tax for affection, V3.3 */
        for ( i = 0; i < 4; i++) {
            if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i] >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
                                     /* Check Tax Sign */
                TtlUpCasDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Daily Gross Total */
            } else {
				if (usStatus & TTL_TUP_TRETURN) {
					TtlUpCasDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Daily Gross Total */
				} else if ((usStatus & TTL_TUP_PVOID) == 0) {
                    TtlUpCasPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]); /* Affect Plus Void Total/Counter */
                }
            }
            TtlUpCasTaxable(pTmpBuff, 
                            pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxable[i],
                            pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i],
                            pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxExempt[i],
                            pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lFSExempt[i],   /* Saratoga */
                            i);
            TtlUpCasConsTax(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i]);         /* Affcet Consolidation Tax Total */
            if (usStatus & TTL_TUP_PVOID) {
                 TtlUpCasPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxAmount[i], 0); /* Affect Preselect Void Total/Counter */
            }
            pTmpBuff->NonTaxable += pTtlTranUpBuff->TtlItemAffect.USCanVAT.USCanTax.lTaxable[4];    /* Affect Non Taxable Amount */
        }
        break;
/* add check transfer of cashier, V3.3 */
    case CLASS_NEWWAITER:           /* Check Transfer New Cashier */
        if (pTtlTranUpBuff->TtlItemAffect.sCheckOpen >= 0) {
            sCount = 1;
        } else {
            sCount = -1;
        }
        TtlUpCasCheckTransTo(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sCount);

		/*Subtract one customer from the "New" Waiter, SR 209, this will make sure that the waiter that
		Input the order will get credit for the customer, previously it was set that the cashier who closes
		the check got the customer*/
		TtlUpCasCustomer(pTmpBuff, -1);
                                    /* Affect Checked Transfered To */
		
		//JHHJ SR 141, This adds to the open checks counter on a cashier who 
		// didnt actually open  the check account, commented out
        //TtlUpCasNoOfChkOpen(pTmpBuff, 
          //                 pTtlTranUpBuff->TtlItemAffect.sCheckOpen);
                                    /* Affect No of Checks Opened Counter */
        
		/*Removed, SR 209, was giving the "New" Cashier credit for the people*/
		/*TtlUpCasNoOfPerson(pTmpBuff, 
                           pTtlTranUpBuff->TtlItemAffect.sNoPerson);
                                    /* Affect No of Person Counter */
        break;

    /* add check transfer of cashier, V3.3 */
    case CLASS_OLDWAITER:           /* Check Transfer Old Cashier */
        if (pTtlTranUpBuff->TtlItemAffect.sCheckOpen <= 0) {
            sCount = 1;
        } else {
            sCount = -1;
        }
        TtlUpCasCheckTransFrom(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sCount);    /* Affect Checked Transfered From */

		/*Add one customer to the "Old" Waiter, SR 209, this will make sure that the waiter that
		Input the order will get credit for the customer, previously it was set that the cashier who closes
		the check got the customer*/
		TtlUpCasCustomer(pTmpBuff, 1);

		//JHHJ SR 141, This subtracts from the open checks counter on a cashier who 
		//actually opened the check account, commented out
		//TtlUpCasNoOfChkOpen(pTmpBuff,
          //                  pTtlTranUpBuff->TtlItemAffect.sCheckOpen);
                                    /* Affect No of Checks Opened Counter */

		/*Removed, SR 209, Was giving the new waiter the people that the old waiter had,*/
        /*TtlUpCasNoOfPerson(pTmpBuff, 
                           pTtlTranUpBuff->TtlItemAffect.sNoPerson);*/
                                    /* Affect No of Person Counter */
        break;
    case CLASS_CHARGETIPWAITER:     /* Charge Tip Amount for Waiter, V3.3 */
    case CLASS_CHARGETIPWAITER2:     /* Charge Tip Amount for Waiter, V3.3 */
    case CLASS_CHARGETIPWAITER3:     /* Charge Tip Amount for Waiter, V3.3 */
        uchOffset = (UCHAR)(pTtlTranUpBuff->TtlItemAffect.uchMinorClass - CLASS_CHARGETIPWAITER);
        
        TtlUpCasChargeTips(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign, uchOffset);   /* Affect Charge Tips Total/Counter */
        break;

/*  default:    Not Used */
    }
}
/*
*============================================================================
**Synopsis:     VOID TtlCVatService(USHORT usStatus, SHORT sSign, 
*                                   TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                   TTLCSWAITOTAL *pTmpBuff)
*
*    Input:     USHORT usStatus         - Affection Status
*               SHORT sSign             - Affection Sign for Counter
*               TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSWAITOTAL *pTmpBuff   - Pointer of Waiter Total
*
*   Output:     TTLCSWAITOTAL *pTmpBuff   - Affected Waiter Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Affection of Major Class. V3.3
*
*============================================================================
*/
VOID TtlCVatService(USHORT usStatus, SHORT sSign, 
                    TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
{
    UCHAR i;

	if (usStatus & TTL_TUP_IGNORE) return;

    switch(pTtlTranUpBuff->TtlClass.uchMinorClass) {

    case CLASS_SERVICE_VAT:          /* VATable Service Total */
    case CLASS_SERVICE_NON:          /* Non VATable Service Total */
        if (usStatus & TTL_TUP_EXCLUDE) {
            if ((pTtlTranUpBuff->TtlItemAffect.lAmount >= 0) &&
                ((usStatus & TTL_TUP_PVOID) == 0)) {
                TtlUpCasDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);

            } else if (usStatus & TTL_TUP_PVOID) {
                TtlUpCasPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, 0);

            } else {
                TtlUpCasPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount);
            }
        }
        TtlUpCasVATServiceTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.lAmount, sSign);  /* TtlCVatService(): Affect Service Total Total/Counter */
        break;                      /* End Affection */

    case CLASS_VATAFFECT:           /* VAT Affection */
        if (usStatus & TTL_TUP_EXCLUDE) {
            for (i=0; i < NUM_VAT; i++) {
                if ((pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt >= 0) && ((usStatus & TTL_TUP_PVOID) == 0)) {
                    TtlUpCasDGGTotal(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                } else if (usStatus & TTL_TUP_PVOID) {
                    TtlUpCasPreVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt, 0);
                } else {
                    TtlUpCasPlusVoid(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt);
                }
                TtlUpCasConsTax(pTmpBuff, pTtlTranUpBuff->TtlItemAffect.USCanVAT.ItemVAT[i].lVATAmt); /* TtlCVatService(): Affcet Consolidation Tax Total */
            }
        }
        break;                    

/*  default:    Not Used */
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasDGGTotal(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Daily Gross Group of Cashier.
*
*============================================================================
*/
VOID TtlUpCasDGGTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lDGGtotal += lAmount;      /* Update Daily Gross Group Total */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasVATServiceTotal(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Service Total
*
*   Output:     TTLCSWAITOTAL *pTmpBuff   - Affected Waiter Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Service Total of Cashier V3.3
*
*============================================================================
*/
VOID TtlUpCasVATServiceTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->lVATServiceTotal += lAmount;      /* Affect VAT Service Total */
    return;

    sCounter = sCounter;
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasPlusVoid(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Plus Void of Cashier.
*
*============================================================================
*/
VOID TtlUpCasPlusVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->PlusVoid.lAmount += lAmount;      /* Update Plus Void Total */
    pTmpBuff->PlusVoid.sCounter += 1;           /* Update Plus Void Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasPreVoid(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
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
** Description  This function Affects Preselect Void of Cashier.
*
*============================================================================
*/
VOID TtlUpCasPreVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->PreselectVoid.lAmount += lAmount;       /* Update Preselect Void Total */
    pTmpBuff->PreselectVoid.sCounter += sCounter;     /* Update Preselect Void Counter */
}
VOID TtlUpCasTransactionReturn(USHORT fbMod, TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
//	sCounter = abs(sCounter);
	sCounter *= -1;
	if (fbMod & RETURNS_MODIFIER_1) {
		pTmpBuff->TransactionReturn.lAmount += lAmount;       /* Update transaction return Total */
		pTmpBuff->TransactionReturn.sCounter += sCounter;     /* Update transaction return Counter */
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
**Synopsis:     VOID TtlUpCasConsCoupon(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, SHORT sCounter)
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
** Description  This function Affects Consolidated Coupon of Cashier.
*
*============================================================================
*/
VOID TtlUpCasConsCoupon(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ConsCoupon.lAmount += lAmount;        /* Update Consolidated Coupon Total */
    pTmpBuff->ConsCoupon.sCounter += sCounter;      /* Update Consolidated Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasItemDisc(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
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
** Description  This function Affects Item Discount of Cashier.
*
*============================================================================
*/
VOID TtlUpCasItemDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ItemDisc.lAmount += lAmount;       /* Update Item Discount Total */
    pTmpBuff->ItemDisc.sCounter += sCounter;     /* Update Item Discount Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasModiDisc(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
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
** Description  This function Affects Modifier Discount of Cashier.
*
*============================================================================
*/
VOID TtlUpCasModiDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ModiDisc.lAmount += lAmount;        /* Update Modified Item Discount Total */
    pTmpBuff->ModiDisc.sCounter += sCounter;      /* Update Modified Item Discount Counter */
}

TOTAL  *TtlUpdateDiscountTotal (TOTAL *pTotal, DCURRENCY lAmount, SHORT sCounter)
{
    pTotal->lAmount += lAmount;     /* Update Modified Item Discount Total */
    pTotal->sCounter += sCounter;   /* Update Modified Item Discount Counter */
	return pTotal;
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasRegDisc(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Regular Discount Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Regular Discount of Cashier.
*
*============================================================================
*/
VOID TtlUpCasRegDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                     UCHAR uchOffset)
{
    /* === New Element - Regular Discount 1 - 6 (Release 3.1) BEGIN 6-13-97 === */
    if (uchOffset > 5) {
    /*    if (uchOffset > 1) { */
        return;
    }
    /* === New Element - Regular Discount 1 - 6 (Release 3.1) END 6-13-97 === */

    pTmpBuff->RegDisc[uchOffset].lAmount += lAmount;     /* Update Regular Discount 1 or 2 Total */
    pTmpBuff->RegDisc[uchOffset].sCounter += sCounter;   /* Update Regular Discount 1 or 2 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasCpn(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
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
** Description  This function Affects Coupon of Cashier.          R3.0
*
*============================================================================
*/
VOID TtlUpCasCpn(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->Coupon.lAmount += lAmount;     /* Update Coupon Total */
    pTmpBuff->Coupon.sCounter += sCounter;   /* Update Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasUCpn(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                    SHORT sCounter)
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
** Description  This function Affects Coupon of Cashier.          R3.0
*
*============================================================================
*/
VOID TtlUpCasUCpn(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->aUPCCoupon[0].lAmount += lAmount;     /* Update Coupon Total */
    pTmpBuff->aUPCCoupon[0].sCounter += sCounter;   /* Update Coupon Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasPaidOut(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, SHORT sCounter)
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
** Description  This function Affects Paid Out of Cashier.
*
*============================================================================
*/
VOID TtlUpCasPaidOut(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->PaidOut.lAmount += lAmount;      /* Update Paid Out Total */
    pTmpBuff->PaidOut.sCounter += sCounter;    /* Update Paid Out Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasRecvAcount(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter)
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
** Description  This function Affects Recieved on Account of Cashier.
*
*============================================================================
*/
VOID TtlUpCasRecvAcount(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->RecvAcount.lAmount += lAmount;      /* Update Received on Account Total */
    pTmpBuff->RecvAcount.sCounter += sCounter;    /* Update Recevied on Account Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasTipsPaid(TTLCSCASTOTAL *pTmpBuff, LONG lAmount,
*                                     SHORT sCounter)
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
** Description  This function Affects Tips Paid Out of Cashier.
*
*============================================================================
*/
VOID TtlUpCasTipsPaid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->TipsPaid.lAmount += lAmount;     /* Update Tips Paid Out Total */
    pTmpBuff->TipsPaid.sCounter += sCounter;   /* Update Tips Paid Out Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasDeclaredTips(TTLCSCASTOTAL *pTmpBuff, LONG lAmount,
*                                         SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Waiter Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Waiter Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Declared Tips of Cashier, V3.3
*
*============================================================================
*/
VOID TtlUpCasDeclaredTips(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->DeclaredTips.lAmount += lAmount;      /* Update Decleared Tips Total */
    pTmpBuff->DeclaredTips.sCounter += sCounter;    /* Update Decleared Tips Counter */
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasMoney(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, UCHAR uchTenderClass)

*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Waiter Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Waiter Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Declared Tips of Cashier, Saratoga
*
*============================================================================
*/
VOID TtlUpCasMoney(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, UCHAR uchTenderClass)
{
    switch (uchTenderClass) {
    case CLASS_TENDER1:
        pTmpBuff->CashTender.lHandTotal = lAmount;          /* Replace */
        break;

    case CLASS_TENDER2:
        pTmpBuff->CheckTender.lHandTotal = lAmount;         /* Replace */
        break;

    case CLASS_TENDER3:
        pTmpBuff->ChargeTender.lHandTotal = lAmount;        /* Replace */
        break;

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
    case CLASS_TENDER19:
    case CLASS_TENDER20:
        pTmpBuff->MiscTender[(UCHAR)(uchTenderClass - CLASS_TENDER4)].lHandTotal = lAmount;
        break;

    case CLASS_FOREIGN1:
    case CLASS_FOREIGN2:
    case CLASS_FOREIGN3:
    case CLASS_FOREIGN4:
    case CLASS_FOREIGN5:
    case CLASS_FOREIGN6:
    case CLASS_FOREIGN7:
    case CLASS_FOREIGN8:
        pTmpBuff->ForeignTotal[(UCHAR)(uchTenderClass - CLASS_FOREIGN1)].lHandTotal = lAmount;
        break;

    default:
        break;
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasCashTender(TTLCSCASTOTAL *pTmpBuff, LONG lAmount,
*                                       SHORT sCounter)
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
VOID TtlUpCasCashTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->CashTender.Total.lAmount += lAmount;     /* Update Cash Tender Total */
    pTmpBuff->CashTender.Total.sCounter += sCounter;   /* Update Cash Tender Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasCheckTender(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                        SHORT sCounter)
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
** Description  This function Affects Check Tender of Cashier.  Saratoga
*============================================================================
*/
VOID TtlUpCasCheckTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->CheckTender.Total.lAmount += lAmount;     /* Update Check Tender Total */
    pTmpBuff->CheckTender.Total.sCounter += sCounter;   /* Update Check Tneder Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasFSChange(TTLCSCASTOTAL *pBuff, ItemTender *pItem, SHORT sSign)
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
VOID TtlUpCasFSChange(TTLCSCASTOTAL *pBuff, ITEMTENDER *pItem, SHORT sSign)
{
    pBuff->CheckTender.Total.lAmount    -= pItem->lFoodStamp;
    pBuff->FoodStampCredit.lAmount      += pItem->lFSChange;
/*    pBuff->CheckTender.Total.lAmount    -= pItem->lFoodStamp * sSign;
    pBuff->FoodStampCredit.lAmount      += pItem->lFSChange * sSign; */
    if (pItem->lFSChange != 0L) {
        pBuff->FoodStampCredit.sCounter     += 1 * sSign;
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasChargeTender(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter)
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
** Description  This function Affects Charge Tender of Cashier. Saratoga
*============================================================================
*/
VOID TtlUpCasChargeTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->ChargeTender.Total.lAmount += lAmount;      /* Update Charge Tender Total */
    pTmpBuff->ChargeTender.Total.sCounter += sCounter;    /* Update Charge Tender Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasMiscTender(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Misc Tender Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Misc Tender of Cashier.   Saratoga
*============================================================================
*/
VOID TtlUpCasMiscTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                        UCHAR uchOffset)
{
    if (uchOffset > sizeof(pTmpBuff->MiscTender)/sizeof(pTmpBuff->MiscTender[0])) {
        return;
    }
    pTmpBuff->MiscTender[uchOffset].Total.lAmount += lAmount;    /* Update Misc Tender 1 to 5 Total */
    pTmpBuff->MiscTender[uchOffset].Total.sCounter += sCounter;  /* Update Misc Tender 1 to 5 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasForeignTotal(TTLCSCASTOTAL *pTmpBuff, LONG lAmount,
*                                         SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Foreign Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Foreign Total of Cashier. Saratoga
*============================================================================
*/
VOID TtlUpCasForeignTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter,
                          UCHAR uchOffset)
{
    if (uchOffset > sizeof(pTmpBuff->ForeignTotal)/sizeof(pTmpBuff->ForeignTotal[0])) {            /* Saratoga */
        return;
    }
    pTmpBuff->ForeignTotal[uchOffset].Total.lAmount += lAmount;    /* Update Foreign Total 1 to 8 Total */
    pTmpBuff->ForeignTotal[uchOffset].Total.sCounter += sCounter;  /* Update Foreign Total 1 to 8 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasServiceTotal(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                         SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Service Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Service Total of Cashier.
*
*============================================================================
*/
VOID TtlUpCasServiceTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchTotalID)
{
	UCHAR  uchOffset = 0;

	// We map Total Key #3 through Total Key #8 to the Service Total Array elements 0 thru 5.
	// We map Total Key #10 through Total Key #20 to the Service Total Array elements 6 thru 16.
	// Total #1 is Cash, Total #2 and Total #9 are both food stamp.  None of these have service totals.
	if(uchTotalID >= CLASS_TOTAL3 && uchTotalID <= CLASS_TOTAL8)
	{
		uchOffset = uchTotalID - (UCHAR)CLASS_TOTAL3;
	}else if (uchTotalID >= CLASS_TOTAL10 && uchTotalID <= CLASS_TOTAL20)
	{
		uchOffset = uchTotalID - (UCHAR)CLASS_TOTAL10 + 6;
	} else {
		return;
	}

	if (uchOffset > sizeof(pTmpBuff->ServiceTotal)/sizeof(pTmpBuff->ServiceTotal[0])) {
        return;
    }

    pTmpBuff->ServiceTotal[uchOffset].lAmount += lAmount;     /* Update Service Total 1 to 6 Total */
    pTmpBuff->ServiceTotal[uchOffset].sCounter += sCounter;   /* Update Service Total 1 to 6 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasAddCheckTotal(TTLCSCASTOTAL *pTmpBuff, 
*                                          LONG lAmount, 
*                                          SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of AddCheck Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects AddCheck Total of Cashier.
*
*============================================================================
*/
VOID TtlUpCasAddCheckTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, 
                           SHORT sCounter, UCHAR uchOffset)
{
    if (uchOffset > 2) {
        return;
    }
    pTmpBuff->AddCheckTotal[uchOffset].lAmount += lAmount;    /* Update Add Check Total 1 to 2 Total */
    pTmpBuff->AddCheckTotal[uchOffset].sCounter += sCounter;  /* Update add Check Total 1 to 2 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasChargeTips(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                       SHORT sCounter)
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
** Description  This function Affects Charge Tips of Cashier.
*
*============================================================================
*/
VOID TtlUpCasChargeTips(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount,
                                            SHORT sCounter, UCHAR uchOffset)
{
    /* V3.3 */
    if (uchOffset > 2) {
        return;
    }
    pTmpBuff->ChargeTips[uchOffset].lAmount += lAmount;     /* Update Charge Tips Total */
    pTmpBuff->ChargeTips[uchOffset].sCounter += sCounter;   /* Update Charge Tips Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasTransCancel(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Transaction Cancel of Cashier.
*
*============================================================================
*/
VOID TtlUpCasTransCancel(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
	if (lAmount < 0) lAmount *= -1;
    pTmpBuff->TransCancel.lAmount += lAmount;   /* Update Transaction Cancel Total */
    pTmpBuff->TransCancel.sCounter += 1;        /* Update Transaction Cancel Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasMiscVoid(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Misc Void of Cashier.
*
*============================================================================
*/
VOID TtlUpCasMiscVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
	if (lAmount < 0) lAmount *= -1;
    pTmpBuff->MiscVoid.lAmount += lAmount;     /* Update Misc Void Total */
    pTmpBuff->MiscVoid.sCounter += 1;          /* Update Misc Void Counter */ 
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasAudaction(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Audaction of Cashier.
*               Audaction is a total of the number of times that a tender amount
*               went to either zero or less than zero along with the tender amount
*               added to the audaction amount total.
*               The audaction total provides an audit check point to see how many
*               tenders are resulting in a zero or negative amount and may be
*               an indication of stealing much like too many No Sales being done.
*
*============================================================================
*/
VOID TtlUpCasAudaction(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->Audaction.lAmount += lAmount;      /* Update Audaction Total */
    pTmpBuff->Audaction.sCounter += 1;           /* Update Audaction Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasNoSaleCount(TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No Sales of Cashier.
*
*============================================================================
*/
VOID TtlUpCasNoSaleCount(TTLCSCASTOTAL *pTmpBuff)
{
    pTmpBuff->sNoSaleCount += 1;       /* Update No Sale Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasItemProduct(TTLCSCASTOTAL *pTmpBuff, LONG lCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lCounter           - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Item Productivity of Cashier.
*
*============================================================================
*/
VOID TtlUpCasItemProduct(TTLCSCASTOTAL *pTmpBuff, LONG lCounter)
{
    pTmpBuff->lItemProductivityCount += lCounter;      /* Update item Productivity Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasNoOfPerson(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Person of Cashier.
*
*============================================================================
*/
VOID TtlUpCasNoOfPerson(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfPerson += sCounter;      /* Update No of Person Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasNoOfChkOpen(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Checks Open Cashier.
*
*============================================================================
*/
VOID TtlUpCasNoOfChkOpen(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfChkOpen += sCounter;      /* Update No of Checks Open Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasNoOfChkClose(TTLCSCASTOTAL *pTmpBuff, 
*                                         SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects No of Checks Close Counter of Cashier.
*
*============================================================================
*/
VOID TtlUpCasNoOfChkClose(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sNoOfChkClose += sCounter;      /* Update no of Checks Close Counter */
}
/*               
*============================================================================
**Synopsis:     VOID TtlUpCasCustomer(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               SHORT sCounter          - Affection Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing 
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Customer Counter of Cashier.
*
*============================================================================
*/
VOID TtlUpCasCustomer(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter)
{
    pTmpBuff->sCustomerCount += sCounter;      /* Update Customer Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasHashDept(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                     SHORT sCounter)
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
** Description  This function Affects Hash Department of Cashier.
*
*============================================================================
*/
VOID TtlUpCasHashDept(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->HashDepartment.lAmount += lAmount;      /* Update Hash Department Total */
    pTmpBuff->HashDepartment.sCounter += sCounter;    /* Update Hash department Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasBonus(TTLCSCASTOTAL *pTmpBuff, LONG lAmount, 
*                                  SHORT sCounter, UCHAR uchOffset)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*               SHORT sCounter          - Affection Counter
*               UCHAR uchOffset         - Offset of Bonus Total/Counter
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Bonus Total 1 to 8 of Cashier.
*
*============================================================================
*/
VOID TtlUpCasBonus(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, USHORT uchOffset)
{
    if (uchOffset >= STD_NO_BONUS_TTL) {
        return;
    }
    pTmpBuff->Bonus[uchOffset].lAmount += lAmount;      /* Update Bonus Total 1 to 8 Total */
    pTmpBuff->Bonus[uchOffset].sCounter += sCounter;    /* Update Bonus Total 1 to 8 Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasConsTax(TTLCSCASTOTAL *pTmpBuff, LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Consolidation Tax Total of Cashier.
*
*============================================================================
*/
VOID TtlUpCasConsTax(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount)
{
    pTmpBuff->lConsTax += lAmount;      /* Update Consolidation Tax Total */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasTaxable(TTLCSCASTOTAL FAR *pTmpBuff, LONG lTaxable,
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
VOID TtlUpCasTaxable(TTLCSCASTOTAL  *pTmpBuff, DCURRENCY lTaxable, DCURRENCY lTaxAmount,
                     DCURRENCY lTaxExempt, DCURRENCY lFSExempt, UCHAR uchOffset)
{

    if (uchOffset > 3) {
        return;
    }
	pTmpBuff->Taxable[uchOffset].mlTaxableAmount += lTaxable;    /* Affect Taxable Amount */
    pTmpBuff->Taxable[uchOffset].lTaxAmount += lTaxAmount;     /* Affect Tax Amount */
    pTmpBuff->Taxable[uchOffset].lTaxExempt += lTaxExempt;     /* Affect Tax Amount */
    pTmpBuff->Taxable[uchOffset].lFSTaxExempt += lFSExempt;    /* Affect FS Tax Exempt Amount, Saratoga */
}
/*********************** New Functions (Release 3.1) BEGIN *****************/
/*
*============================================================================
**Synopsis:     VOID TtlCPostReceipt(TTLTUMTRANUPDATE *pTtlTranUpBuff,
*                                    TLCSCASTOTAL *pTmpBuff )
*
*    Input:     TTLTUMTRANUPDATE *pTtlTranUpBuff - Pointer of Transaction
*                                                  Item Data
*               TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Cashier in Post Receipt Operation.
*
*============================================================================
*/
VOID TtlCPostReceipt(TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff)
{
    switch (pTtlTranUpBuff->TtlClass.uchMinorClass) {
    case CLASS_POSTRECEIPT:     /* Post Receipt */
    case CLASS_POSTRECPREBUF:   /* Post Receipt for Precheck */
    case CLASS_PARKING:         /* Parking Print */
        TtlUpCasPostReceipt(pTmpBuff);
        break;
    default:
        break;
    }
}

/*
*============================================================================
**Synopsis:     VOID TtlUpCasPostReceipt(TTLCSCASTOTAL *pTmpBuff)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Repost Receipt Conter of Cashier.
*
*============================================================================
*/
VOID TtlUpCasPostReceipt(TTLCSCASTOTAL *pTmpBuff)
{
    pTmpBuff->usPostRecCo++;
}
/*********************** New Functions (Release 3.1) END *******************/

/*
*============================================================================
**Synopsis:     VOID TtlUpCasCheckTransTo(TTLCSCASTOTAL *pTmpBuff,
*                                         LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Checks Transfered To Total of Cashier, V3.3
*
*============================================================================
*/
VOID TtlUpCasCheckTransTo(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->CheckTransTo.lAmount += lAmount;      /* Update Check Transferred to Total */
    pTmpBuff->CheckTransTo.sCounter += sCounter;    /* Update Transferred to Counter */
}
/*
*============================================================================
**Synopsis:     VOID TtlUpCasCheckTransFrom(TTLCSCASTOTAL *pTmpBuff, 
*                                           LONG lAmount)
*
*    Input:     TTLCSCASTOTAL *pTmpBuff   - Pointer of Cashier Total
*               LONG lAmount            - Affection Amount
*
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Checks Transfered From Total of Cashier, V3.3.
*
*============================================================================
*/
VOID TtlUpCasCheckTransFrom(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter)
{
    pTmpBuff->CheckTransFrom.lAmount += lAmount;    /* Update Check Transferred from Total */
    pTmpBuff->CheckTransFrom.sCounter += sCounter;  /* Update Transferred from Counter */
}

/*
*============================================================================
**Synopsis: VOID    TtlCLoanPick(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
*
*    Input:     TTLCSCASTOTAL *pBuff    - Pointer of Cashier Total
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Loan/Pickup.              Saratoga
*============================================================================
*/
VOID    TtlCLoanPick(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
{
    if (pClass->TtlClass.uchMinorClass == CLASS_LOANAFFECT) {
        switch (pClass->TtlItemAffect.uchTotalID) {
        case    CLASS_MAINTTENDER1:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER2:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER3:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER4:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER5:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER6:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER7:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER8:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER9:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER10:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER11:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER12:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER13:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER14:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER15:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER16:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER17:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER18:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER19:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER20:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
            pCas->Loan.lAmount += pClass->TtlItemAffect.lAmount;
            break;
        case    CLASS_MAINTFOREIGN1:
        case    CLASS_MAINTFOREIGN2:
        case    CLASS_MAINTFOREIGN3:
        case    CLASS_MAINTFOREIGN4:
        case    CLASS_MAINTFOREIGN5:
        case    CLASS_MAINTFOREIGN6:
        case    CLASS_MAINTFOREIGN7:
        case    CLASS_MAINTFOREIGN8:
            pCas->Loan.lAmount += pClass->TtlItemAffect.lService[0];
            break;
        default:
            return;
        }
        TtlCLoanPickTender(pClass->TtlItemAffect.uchTotalID, pClass->TtlItemAffect.lAmount, pCas);

    } else {
        switch (pClass->TtlItemAffect.uchTotalID) {
        case    CLASS_MAINTTENDER1:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER2:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER3:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER4:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER5:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER6:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER7:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER8:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER9:          // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER10:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER11:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER12:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER13:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER14:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER15:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER16:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER17:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER18:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER19:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
        case    CLASS_MAINTTENDER20:         // function TtlCLoanPick() for AC10/AC11.  See also TtlFLoanPick()
            pCas->Pickup.lAmount += pClass->TtlItemAffect.lAmount;
            break;
        case    CLASS_MAINTFOREIGN1:
        case    CLASS_MAINTFOREIGN2:
        case    CLASS_MAINTFOREIGN3:
        case    CLASS_MAINTFOREIGN4:
        case    CLASS_MAINTFOREIGN5:
        case    CLASS_MAINTFOREIGN6:
        case    CLASS_MAINTFOREIGN7:
        case    CLASS_MAINTFOREIGN8:
            pCas->Pickup.lAmount += pClass->TtlItemAffect.lService[0];
            break;
        default:
            return;
        }
        TtlCLoanPickTender(pClass->TtlItemAffect.uchTotalID, pClass->TtlItemAffect.lAmount, pCas);
    }
}

/*
*============================================================================
**Synopsis: VOID    TtlCLoanPickTender(UCHAR uchTotalID, LONG lAmount, TTLCSCASTOTAL *pCas)
*
*    Input:     
*   Output:     
*    InOut:     Nothing
*
** Return:      Nothing  
*               Nothing
*
** Description  This function Affects Loan/Pickup.              Saratoga
*============================================================================
*/
VOID    TtlCLoanPickTender(UCHAR uchTotalID, DCURRENCY lAmount, TTLCSCASTOTAL *pCas)
{
    UCHAR   uchOffset;

    switch (uchTotalID) {
    case CLASS_MAINTTENDER1:
        pCas->CashTender.Total.lAmount += lAmount;
        break;

    case CLASS_MAINTTENDER2:
        pCas->CheckTender.Total.lAmount += lAmount;
        break;

    case CLASS_MAINTTENDER3:
        pCas->ChargeTender.Total.lAmount += lAmount;
        break;

    case CLASS_MAINTTENDER4:
    case CLASS_MAINTTENDER5:
    case CLASS_MAINTTENDER6:
    case CLASS_MAINTTENDER7:
    case CLASS_MAINTTENDER8:
    case CLASS_MAINTTENDER9:
    case CLASS_MAINTTENDER10:
    case CLASS_MAINTTENDER11:
    case CLASS_MAINTTENDER12:
    case CLASS_MAINTTENDER13:
    case CLASS_MAINTTENDER14:
    case CLASS_MAINTTENDER15:
    case CLASS_MAINTTENDER16:
    case CLASS_MAINTTENDER17:
    case CLASS_MAINTTENDER18:
    case CLASS_MAINTTENDER19:
    case CLASS_MAINTTENDER20:
        uchOffset = uchTotalID - CLASS_MAINTTENDER4;
        pCas->MiscTender[uchOffset].Total.lAmount += lAmount;
        break;

    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        uchOffset = uchTotalID - CLASS_MAINTFOREIGN1;
        pCas->ForeignTotal[uchOffset].Total.lAmount += lAmount;
        break;
    }
}

/*
*============================================================================
**Synopsis: VOID    TtlCLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
*
*    Input:     TTLCSCASTOTAL *pBuff    - Pointer of Cashier Total
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
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
VOID    TtlCLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
{
    if (pClass->TtlItemAffect.uchMinorClass == CLASS_LOANCLOSE) {
        pCas->Loan.sCounter++;
    } else {
        pCas->Pickup.sCounter++;
    }
}

/*
*============================================================================
**Synopsis: VOID    TtlCEptError(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
*
*    Input:     TTLCSCASTOTAL *pBuff    - Pointer of Cashier Total
*   Output:     TTLCSCASTOTAL *pTmpBuff   - Affected Cashier Total
*    InOut:     Nothing
*
** Return:      Nothing
*               Nothing
*
** Description  This function Affects Loan/Pickup.              Saratoga
*               uchMajorClass == CLASS_EPT_ERROR_AFFECT    - TtlFEptError()
*============================================================================
*/
VOID    TtlCEptError(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas)
{
	int i;

	for (i = BONUS_ERROR_AFFECT_SALE; i <= BONUS_ERROR_AFFECT_VOIDRETURN; i++) {
		pCas->Bonus[i].lAmount += pClass->TtlItemAffect.lBonus[i];
		if (pClass->TtlItemAffect.lBonus[i]) {
			pCas->Bonus[i].sCounter += pClass->TtlItemAffect.sItemCounter;
		}
	}
}

/* ===== End of File (ttlsbcup.c) ===== */