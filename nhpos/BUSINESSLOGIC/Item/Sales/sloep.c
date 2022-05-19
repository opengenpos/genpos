/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Order Entry Prompt Process
* Category    : Item Module, 2170 US Hospitality Model Application
* Program Name: SLOEP.C
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-28-95  03.00.00  H.Kato       Initial
* Aug-07-95  03.00.03  O.Nakada     Modified all functions by FVT comment.
* Oct-25-95  03.00.08  O.Nakada     Corrected multiple table number.
* Nov-13-95  03.01.00  M.Ozawa      Enhanced to R3.1 features.
* Jul-22-98  03.03.00  M.Ozawa      Add LCD scroll for OEP
* Dec-18-15  02.02.01  R.Chambers   removed unused PLU number argument from ItemSalesOEPUpdateGroupNo()
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <malloc.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "appllog.h"
#include "uie.h"
#include "fsc.h"
#include "uireg.h"
#include "rfl.h"
#include "regstrct.h"
#include "transact.h"
#include "trans.h"
#include "item.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "csop.h"
#include "csstbopr.h"
#include "display.h"
#include "mld.h"
#include "itmlocal.h"
#include "bl.h"
#include "BlFWif.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
#define SLOEP_ENTRY_NO      11
#define SLOEP_ENTRY_NO_END  99  /* V3.3 */
#define SLOEP_F_INSERT      0x0001
#define SLOEP_F_MULTI       0x0002
#define SLOEP_SCROLL_UP     10000   /* V3.3 */
#define SLOEP_SCROLL_DOWN   10001   /* V3.3 */
#define SLOEP_PLUKEY        10002   /* V3.3 */
#define SLOEP_NUM_GROUP_IDX 5   /* V3.3 */

#define STATIC static

STATIC  ITEMMODLOCAL    SlOepSaveMod;
STATIC  ULONG           flSlOepSaveDescr;
STATIC  ULONG           flSlOepSaveKeep;
STATIC  LONG            lSlOepSaveQTY;
STATIC  LONG            aculSlOepCounterSave[SLOEP_NUM_GROUP_IDX+1];  /* V3.3 */
STATIC  SHORT           sSlOepCounterIndex;                         /* V3.3 */

STATIC  SHORT   ItemSalesOEPNextPlu(UIFREGSALES *pUifRegSales,
                                    ITEMSALES *pItemSales,
                                    UCHAR *pszGroups);
STATIC  SHORT   ItemSalesOEPMakeMenu(UCHAR uchGroupNo, UCHAR uchNounAdj,
                                     ITEMSALESOEPLOCAL *pItem,
                                     MLDPOPORDER_VARY *pDisp, int nNoOrderPmt, USHORT *pusNumItem,
                                     LONG *pculCounterFirst, LONG *pculCounterLast);
STATIC  SHORT   ItemSalesAdjMakeMenu(UCHAR uchGroupNo, UCHAR uchNounAdj,
                                     ITEMSALESOEPLOCAL *pItem,
                                     MLDPOPORDER_VARY *pDisp, int nNoOrderPmt, USHORT *pusNumItem,
                                     LONG *pculCounterFirst, LONG *pculCounterLast);

STATIC  SHORT   ItemSalesOEPCheckObjectPLU(ITEMSALES *pItem);
STATIC  SHORT   ItemSalesOEPOrderPlu(USHORT *pusOrderNo,
                                        UCHAR *puchAdjNo,
                                        MLDPOPORDER_VARY *pDisp,
                                        USHORT usOption1, USHORT usOption2, USHORT usOption3,
                                        TCHAR *pauchPLUNo,
                                        UCHAR *pfbModifier2);
STATIC  SHORT   ItemSalesOEPMakeNewClass(UIFREGSALES *pUifRegSales,
                                         ITEMSALESOEPLOCAL *pItem,
                                         UCHAR *puchNounAdj,
                                         TCHAR *pszMnemonic,
                                         DCURRENCY  *plNounAmount,
                                         UCHAR *puchStatus,
                                         USHORT usOption1, USHORT usOption2, USHORT usOption3,
                                         UCHAR  *auchGroupNo,
                                         UCHAR fbModifier2,
										 UCHAR *puchTableNumber);
STATIC  SHORT   ItemSalesOEPUpdateGroupNo(UCHAR uchStaus,
                                          UCHAR *pszGroup,
                                          USHORT usGroupOff,
                                          USHORT *pfsType,
                                          USHORT usContinue,
                                          USHORT usNoOfGroup,
                                          UCHAR  uchTableNumber);
STATIC  USHORT  ItemSalesOEPGetGroupNo(UCHAR uchTable, UCHAR *pszGroup);
STATIC  VOID    ItemSalesOEPClear2x20(USHORT usType);
STATIC  SHORT   ItemSalesOEPSetNoOfPluKey(UIFDIADATA *pUI, TCHAR *pauchPluNo);
STATIC  SHORT   ItemSalesOEPSetNoOfPluKey(UIFDIADATA *pUI, TCHAR *pauchPluNo);



/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEP(UIFREGSALES *pUifRegSales,
*                                  ITEMSALES *pItemSales);
*     Input:    pUifRegSales - address of UIF data
*     InOut:    pItemSales   - address of sales data
*
** Return:      ITM_SUCCESS - successful
*               ITM_CONT    - first PLU is not item of OEP object
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEP(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales)
{
    SHORT   sResult, giftPos = 0, sReturnStatus;
    USHORT  fsType, usContinue=0, usNoOfGroup=1; /* dummy */
    UCHAR   szGroups[STD_OEP_MAX_NUM + 1];    /* increased TAR #129199 */

    /* --- check first PLU status --- */
    if ((pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS) ||
        (pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) ||
        (pItemSales->ControlCode.auchPluStatus[2] & PLU_HASH) ||
        (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
        return (ITM_CONT);
    }

    /* --- check and create group number of OEP --- */
    memset(szGroups, 0, sizeof(szGroups));
    /* V3.3 */
    memset(aculSlOepCounterSave, 0, sizeof(aculSlOepCounterSave));
    sSlOepCounterIndex = 0;

    sResult = ItemSalesOEPUpdateGroupNo(0, szGroups, 0, &fsType, usContinue, usNoOfGroup, pItemSales->uchTableNumber); 
    if (sResult != ITM_SUCCESS){
        return (ITM_CONT);						
    }

    /* --- check modifier discount --- */
    if (pUifRegSales->uchMinorClass == CLASS_UIMODDISC) {
        return (LDT_PROHBT_ADR);
    }

    /* --- reset compulsory PM and condiment status --- */
/****
    pItemSales->ControlCode.auchStatus[0] &= ~(PLU_COMP_MODKEY |
                                               PLU_COMP_CODKEY);
****/
    /* --- update modififer status --- */

    if ((sResult = ItemSalesModifier(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
        return (sResult);
    }

    /* --- save first PLU status and descriptor status --- */
    ItemModGetLocal(&SlOepSaveMod);
    lSlOepSaveQTY    = pItemSales->lQTY;
    flSlOepSaveDescr = flDispRegDescrControl;
    flSlOepSaveKeep  = flDispRegKeepControl;

    /* --- update totoal --- */
    if ((sResult = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
        return (sResult);
    }

    /* price change, 2172 */
    if ((sResult = ItemSalesPriceChange(pItemSales)) == ITM_CANCEL) {
        return(UIF_CANCEL);
    } else if (sResult == ITM_PRICECHANGE) {
        /* recalculation by changed unit price */
        if (pItemSales->lProduct == 0L) {
            if ((sResult = ItemSalesCalculation(pItemSales)) != ITM_SUCCESS) {
                return(sResult);
            }
        }
    } else if (sResult != ITM_SUCCESS) {
        return(sResult);
    }


	if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
		(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
	{
		ITEMSALESGIFTCARD	WorkGiftCard = {0};

		if(pItemSales->lQTY != 1000)
			return LDT_SEQERR_ADR;

		//Checking to see if the gift card is to be used for FreedomPay
		WorkGiftCard.GiftCard = DetermineGiftCardItem(pItemSales);

 		giftPos = GetNextGCSlot();
		if (giftPos >= NUM_SEAT_CARD)
			return LDT_PROHBT_ADR;

		TrnGetTransactionInvoiceNum (WorkGiftCard.refno.auchReferncNo);
		//obtain gift card nummber and/or track2 data
		if((sReturnStatus = GetGiftCardAccount2(&WorkGiftCard)) != ITM_SUCCESS) 
			return (UIF_CANCEL);

		// conditionals to determine whether the gift card is of type
		// issue, void issue, reload or void reload.
		if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD)
		{
			if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD)
			{
				if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
				{
					if((sReturnStatus = ItemSalesRefEntry(WorkGiftCard.refno.auchReferncNo) != ITM_SUCCESS) )
						return (UIF_CANCEL);
					
					WorkGiftCard.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
					_RflStrncpyUchar2Tchar (pItemSales->aszNumber[1], WorkGiftCard.refno.auchReferncNo, NUM_NUMBER);
				}else
					WorkGiftCard.GiftCardOperation = GIFT_CARD_RELOAD;
			}
			else {
				if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
				{
					if((sReturnStatus = ItemSalesRefEntry(WorkGiftCard.refno.auchReferncNo) != ITM_SUCCESS) )
						return (UIF_CANCEL);
				
					WorkGiftCard.GiftCardOperation = GIFT_CARD_VOID;
					_RflStrncpyUchar2Tchar (pItemSales->aszNumber[1], WorkGiftCard.refno.auchReferncNo, NUM_NUMBER);
				}else
					WorkGiftCard.GiftCardOperation = GIFT_CARD_ISSUE;
			}
		} else if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT)
		{
			if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_RELOAD)
			{
				if(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_0] & PLU_MINUS)
				{
					if((sReturnStatus = ItemSalesRefEntry(WorkGiftCard.refno.auchReferncNo) != ITM_SUCCESS) )
						return (UIF_CANCEL);
					
					WorkGiftCard.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
					_RflStrncpyUchar2Tchar(pItemSales->aszNumber[1], WorkGiftCard.refno.auchReferncNo, NUM_NUMBER);
				}else
					WorkGiftCard.GiftCardOperation = GIFT_CARD_RELOAD;
			} else {
				return LDT_PROHBT_ADR;  // prohibit PLU_EPAYMENT PLU if it is not PLU_GIFT_RELOAD flagged
			}
		}

		// copy gift card and sales information to and from pItemSales
		WorkGiftCard.auchPluStatus = pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7];
		WorkGiftCard.aszPrice = pItemSales->lUnitPrice;
		_tcsncpy(pItemSales->aszNumber[0], WorkGiftCard.aszNumber, NUM_NUMBER);
		WorkGiftCard.usUniqueID = pItemSales->usUniqueID;

		// Once the Gift Number has been obtained we want to mask
		// the number that will be displayed for pre auth.
		GiftPreAuthMaskAcct(pItemSales);

		/* ----- set link plu, 2172 ----- */
		ItemSalesLinkPLU(pItemSales, 0);

		/* --- discharge first PLU data --- */
		if ((sResult = ItemSalesCommonIF(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
			return (sResult);
		}

		// Save working gift card info.
		SaveGiftCard(&WorkGiftCard, giftPos);
		
	}else
	{
		/* ----- set link plu, 2172 ----- */
		ItemSalesLinkPLU(pItemSales, 0);
    
		/* --- discharge first PLU data --- */
		if ((sResult = ItemSalesCommonIF(pUifRegSales, pItemSales)) != ITM_SUCCESS) {
			return (sResult);
		}
	}

    /* ----- send link plu to kds, 2172 ----- */
    ItemSalesLinkPLU(pItemSales, 2);
        
    /* --- initialize condiment group no R3.1 */
    memset(&ItemSalesLocal.auchOepGroupNo, 0, sizeof(ItemSalesLocal.auchOepGroupNo));

    if ((sResult = ItemSalesOEPNextPlu(pUifRegSales, pItemSales, szGroups)) > 0) {
        UieErrorMsg((USHORT)sResult, UIE_WITHOUT);
        ItemSalesOEPClear2x20(0);               /* clear leadthrough */
    }

    return (ITM_SUCCESS);                       /* ignore sResult */
}


/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPNextPlu(UIFREGSALES *pUifRegSales,
*                                         ITEMSALES *pItemSales,
*                                         UCHAR *pszGroups);
*     Input:    pUifRegSales - address of UIF data
*     InOut:    pItemSales   - address of sales data
*     Input:    pszGroups    - address of group number table
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPNextPluCondiment(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales,
                          UCHAR *pszGroups)
{
	return ItemSalesOEPNextPlu(pUifRegSales, pItemSales,pszGroups);
}
/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPNextPlu(UIFREGSALES *pUifRegSales,
*                                         ITEMSALES *pItemSales,
*                                         UCHAR *pszGroups);
*     Input:    pUifRegSales - address of UIF data
*     InOut:    pItemSales   - address of sales data
*     Input:    pszGroups    - address of group number table
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPNextPlu(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales,
                          UCHAR *pszGroups)
{
    SHORT               sResult;
	int                 nNoOrderPmt = 0;
    USHORT              ausOption[SALES_NUM_GROUP + 1], i, usNoOfOption;    /* OEP option save area */
    USHORT              usOption1 =0 , usOption2 = 0, usOption3 = 0;
    USHORT              usGroupPos, usNumMenu, usOrderNo, usContinue, fsType;
    USHORT              usNoOfGroup;
    UCHAR				uchNounAdj, uchChildAdj;
	TCHAR               aszNounMnem[NUM_DEPTPLU + 1] = {0};
    DCURRENCY           lNounAmount;
    LONG                lCounter;
    UCHAR               uchCondStatus;                      /* for 97 option */
    ITEMSALESOEPLOCAL   *pMenu;
	MLDPOPORDER_VARY    *pDisp;
    UCHAR               auchGroupNo[SALES_NUM_GROUP + 1];     /* for 97 option */
	TCHAR               auchPluNo[NUM_PLU_LEN] = {0};
    UCHAR               fbModifier2;
	UCHAR               uchTableNumber;

    uchNounAdj = pItemSales->uchAdjective;
    _tcsncpy(aszNounMnem, pItemSales->aszMnemonic, NUM_DEPTPLU);

	lNounAmount = 0;
	ItemSalesSalesAmtCalc(pItemSales, &lNounAmount);

	pDisp = (MLDPOPORDER_VARY *) alloca (sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
	NHPOS_ASSERT(pDisp);

	pMenu = (ITEMSALESOEPLOCAL *) alloca (sizeof(ITEMSALESOEPLOCAL) * (128));
 	NHPOS_ASSERT(pMenu);

	if (pDisp == NULL || pMenu == NULL) {
		NHPOS_NONASSERT_NOTE("**ERROR", "**ERROR: ItemSalesOEPNextPlu() alloca() failed. Aborting.");
		return ITM_CANCEL;
	}

   /*
		We will build the series of pop up menus one group at a time.  The
		group information is provisioned through AC160 and contains a table
		of two kinds of values, either a PLU group number (0 - 90) or a 
		speciall code (91-99).

		If the group is a special type rather than a group number, we do 
		special processing.

		If the group is SLOEP_MULTI means allow the user to pick multiple
		times until they choose to finish.  Finish is indicated by 00 entered
		by keyboard or the Done button pressed.
		--- relocate group table for multiple item (TAR #129199) ---

		If the group is SLOEP_FREEONE, SLOEP_FREETWO, etc, then we let the
		user choose a given number of free items from the next group displayed
		(SLOEP_FREEONE is one item, etc.).

		If the group is SLOEP_SELFROMKEY then the next group number specifies the
		group of condiments which are legal.  However, a pop up window is not displayed.
		This means that the user must enter PLU numbers by hand using the keyboard or
		an existing window with those PLU numbers must already be displayed.
	*/

    memset(ausOption, 0x00, sizeof(ausOption));
    usNoOfOption=0;

	// Tell Framework what our groups are.
	BlFwIfSetOEPGroups(pszGroups, pItemSales->uchTableNumber);
    for (usGroupPos = 0; pszGroups[usGroupPos] != 0; usGroupPos++)
	{
		USHORT usContinueLoop = 0;

		// If it is a multi choice then shift the table down and continue
        if (pszGroups[usGroupPos] == SLOEP_MULTI) {
			NHPOS_ASSERT(STD_OEP_MAX_NUM >= usGroupPos);
            memmove(pszGroups, &(pszGroups[usGroupPos]), STD_OEP_MAX_NUM - usGroupPos);
            usGroupPos = 0;
			pszGroups[STD_OEP_MAX_NUM - 1] = 0;
			// Tell Framework what our groups are.
			BlFwIfSetOEPGroups(pszGroups, pItemSales->uchTableNumber);
        }

		// If it is a special option, then process the option
        if (pszGroups[usGroupPos] >= SLOEP_FREEONE) { /* with option */
            ausOption[usNoOfOption] = pszGroups[usGroupPos];       /* check option R3.1 */
            if (ausOption[usNoOfOption] == SLOEP_SELFROMKEY) {     /* if 97 option */
                for(usGroupPos++, i=0;
                        pszGroups[usGroupPos] != 0;
                        usGroupPos++) {

                    if (pszGroups[usGroupPos] < SLOEP_FREEONE) {
                        auchGroupNo[i] = pszGroups[usGroupPos];  /* set terget group no */
                        i++;
                    } else {
						if (!i) {
							ausOption[++usNoOfOption] = pszGroups[usGroupPos]; /* set another option */
						} else {
							break;  /* terminate by another option */
						}
					}
                }
                auchGroupNo[i] = 0;  /* terminater */
                usNoOfGroup = i;
                usGroupPos--;   /* decrement for loop control */
                usNoOfOption++;
            } else {
                usNoOfOption++;
                continue;                           /* next ...      */
            }
        }

        /* --- set multiple option (last option is effective) --- */
        usOption1 = 0; usOption2 = 0; usOption3 = 0;
        for (i=0; i<usNoOfOption; i++) {
            if ((ausOption[i] == SLOEP_MULTI) ||
                (ausOption[i] == SLOEP_COMPNO)) {
                usOption1 = ausOption[i];
            }
            if ((ausOption[i] == SLOEP_FREEONE) ||
                (ausOption[i] == SLOEP_FREETWO) ||
                (ausOption[i] == SLOEP_FREETHREE) ||
                (ausOption[i] == SLOEP_FREEFOUR)) {
                usOption2 = ausOption[i];
            }
            if (ausOption[i] == SLOEP_SELFROMKEY) {
                usOption3 = ausOption[i];
            }
        }
        if (i) i--; /* decrement for leadthrough */

        /* ---- return to normal entry process, if condiment entry option ---- */
        if (usOption3 == SLOEP_SELFROMKEY) {
            if (usOption1 == SLOEP_MULTI) {

                ;   /* request condiment by oep dialog */
            } else if (usOption2) {

                ;   /* request condiment by oep dialog */
            } else {

                /* set group no. of condiment */
                //_tcsncpy(ItemSalesLocal.auchOepGroupNo, auchGroupNo, SALES_NUM_GROUP + 1);
                memcpy(&ItemSalesLocal.auchOepGroupNo, &auchGroupNo, SALES_NUM_GROUP + 1);

                /* check group no by normal entry process */
                return(ITM_SUCCESS);
            }
        } else {
            auchGroupNo[0] = 0; /* terminater */

            /* --- reset compulsory PM and condiment status --- */
            pItemSales->ControlCode.auchPluStatus[0] &= ~(PLU_COMP_MODKEY | PLU_COMP_CODKEY);
        }

		/* --- make menu data --- */

		memset (pDisp, 0, sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));

        pDisp->uchMajorClass = CLASS_POPUP;
        pDisp->uchMinorClass = CLASS_POPORDERPMT;

        /* --- set leadthrough mnemonic according to option R3.1 --- */
        if (ausOption[i] >= SLOEP_FREEONE && ausOption[i] <= SLOEP_COMPNO) {
			/* process Special Group, items free or multiple item */
			// map SLOEP_FREEONE through SLOEP_COMPNO to LDT_OEP91_ADR through LDT_OEP98_ADR
            USHORT usAddress = (USHORT)(LDT_OEP91_ADR + (ausOption[i] - SLOEP_FREEONE));
			RflGetLead(pDisp->aszLeadThrough, usAddress);
		}
		else {
			RflGetLead(pDisp->aszLeadThrough, LDT_OEP_ADR);
		}

        /* --- set adjective mnemonic,  Jul/24 --- */
        if (uchNounAdj != 0) {
			RflGetAdj(pDisp->aszAdjective, uchNounAdj);
        }
        /* --- set noun plu mnemonic --- */
        _tcscpy(pDisp->aszPluName, aszNounMnem);
        pDisp->lPrice = lNounAmount;

        /* --- display menu data --- */

        if (! ItemSalesLocalSaleStarted() && !(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
            ItemCommonMldInit();
        }
        if (usOption3 != SLOEP_SELFROMKEY) { /* not create meue at 97 option R3.1 */
            ItemSalesLocal.fbSalesStatus |= SALES_OEP_POPUP;
        } else {
            ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
        }

        /* --- check retry counter by option R3.1 --- */

        if (usGroupPos == 0 ) {
            usContinue = 0;                 /* single item       */
        } else if (usOption2 == SLOEP_FREEONE) {
            usContinue = 1;                     /* free one item     */
        } else if (usOption2 == SLOEP_FREETWO) {
            usContinue = 2;                     /* free two item     */
        } else if (usOption2 == SLOEP_FREETHREE) {
            usContinue = 3;                     /* free three item   */
        } else if (usOption2 == SLOEP_FREEFOUR) {
            usContinue = 4;                     /* free four item    */
        } else if (usOption1 == SLOEP_MULTI) {
            usContinue = 1;                     /* multiple item     */
        } else {
            usContinue = 0;                 /* single item       */
        }

		// Check to make sure that we have not reached the end of
		// the list of groups.  If this is group 0 then we will just
		// break out and exit normally.
		if (pszGroups[usGroupPos] < 1) {
			break;
		}

        if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
			VOSRECT   VosRect = {0};

			// Create a Popup window, only to determine the window button
			//	dimensions. This is used to find out how many OEP items
			//	should be populated in the OEP menu list (ItemSalesOEPMakeMenu).
			// We specify standard window size for an OEP window but the size
			//  in the Layout will override this size so we need to know what
			//  the actual size from the layout is.
			VosRect.usRow = 0;    /* start row */
			VosRect.usCol = 0;   /* start colum */
			VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length MLD_NOR_POP_ROW_LEN */
			VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */

			nNoOrderPmt = MldCreatePopUpRectWindowOnly(&VosRect);

			if (nNoOrderPmt < 1) {
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
				return (ITM_SUCCESS);                       /* exit ...          */
			}
		}

        usOrderNo = 0;
        for (usContinueLoop = 1; usContinueLoop == 1; ) {
            /* --- order PLU item in menu list --- */

            pDisp->lPrice = lNounAmount;  /* display price with condiment */

            /* V3.3 */
            if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
                /* --- make menu data for OEP scroll --- */
                if (usOrderNo == SLOEP_SCROLL_UP) {
                    pDisp->uchMinorClass = CLASS_POPORDERPMT_UP;
                } else if (usOrderNo == SLOEP_SCROLL_DOWN) {
                    pDisp->uchMinorClass = CLASS_POPORDERPMT_DOWN;
                } else {
                    pDisp->uchMinorClass = CLASS_POPORDERPMT;
                    sSlOepCounterIndex = 0;
                }
                memset(pDisp->OrderPmt, 0, sizeof(ORDERPMT) * (128));  /* V3.3 */
                sResult = ItemSalesOEPMakeMenu(pszGroups[usGroupPos],
                                           uchNounAdj,
                                           pMenu,
                                           pDisp, nNoOrderPmt,
                                           &usNumMenu,
                                           &aculSlOepCounterSave[sSlOepCounterIndex],
                                           &aculSlOepCounterSave[sSlOepCounterIndex+1]);
				if (sResult == ITM_SUCCESS) {
					pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_DONE;
					pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_BACK;
					if (sSlOepCounterIndex > 0) {
						pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_BACK;
					}
                    if (usNumMenu > 0) {
						MldUpdatePopUp(pDisp);
					}
					else {
						PifLog(MODULE_SALES, LOG_EVENT_SALES_MAKEMENU_ERR_1);
						BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
						usContinueLoop = 2;
						break;
					}
				}
				else {
					if (usOrderNo == SLOEP_SCROLL_UP) {
						if (pDisp->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
							sSlOepCounterIndex++;
						}
					} else if (usOrderNo == SLOEP_SCROLL_DOWN) {
						if (pDisp->ulDisplayFlags & MLD_DISPLAYFLAGS_BACK) {
							sSlOepCounterIndex--;
						}
					} else {
						// If there was a problem with the PLUs that we tried
						// to put into the OEP window from this group, then lets
						// just ignore this group and go to the next in the
						// group table.  Indicate there was a problem and
						// this group is being ignored by setting the
						// variable usContinueLoop = 5;
						{
							char aszErrorBuffer[128];

							sprintf (aszErrorBuffer, "ItemSalesOEPMakeMenu() error making OEP.  GroupPos: %d  Group: %d. ", usGroupPos, pszGroups[usGroupPos]);
							NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
						}
						usContinueLoop = 5;
					}
				}
            }

			// Obtain the user's menu selection.  This function is called regardless
			// of whether we have displayed a popup menu or not (if group number is 97,
			// we prompt for a condiment which we will check later and no popup is displayed).
			// If there is an error, then we will break out of the loop and return an error
			// indication that is in sResult.
			if (usContinueLoop < 5) {
				sResult = ItemSalesOEPOrderPlu(&usOrderNo, &uchChildAdj, pDisp,
											   usOption1, usOption2, usOption3, &auchPluNo[0], &fbModifier2);

				if (sResult != ITM_SUCCESS) {
					PifLog (MODULE_SALES, LOG_EVENT_SALES_ORDERPLU_ERR_1);
					{
						char  xBuff[128];
						sprintf (xBuff, "ItemSalesOEPNextPlu(): ItemSalesOEPOrderPlu failed %d. Resetting.", sResult);
						NHPOS_ASSERT_TEXT((sResult == ITM_SUCCESS), xBuff);
					}
					BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
					usContinueLoop = 2;
					break;
				}
			}
			else {
				PifLog(MODULE_SALES, LOG_EVENT_SALES_EMPTY_OEP);
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
				break;
			}

            /* --- check order number --- */

            if ((usOrderNo == SLOEP_SCROLL_UP) || (usOrderNo == SLOEP_SCROLL_DOWN)) {  /* scroll up/down */
                continue;
            }

            if (usOrderNo == 0) {               /* skip by user      */
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_INCREMENT, 0);

                ItemSalesOEPClear2x20(1);       /* display leadthrough */
                break;                          /* exit loop         */
            }

            /* --- set plu no at 97 option R3.1 --- */

			// if the usOption is greater than 90 there is no need to 
			//	increment the counter, the OEP window does not need 
			//	to be changed - CSMALL 1/26/06
			if((usOption1 < SLOEP_BEGIN_SPL) &&
				(usOption2 < SLOEP_BEGIN_SPL) &&
				(usOption3 < SLOEP_BEGIN_SPL))
			{
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_INCREMENT, 0);
			}

            if (usOption3 == SLOEP_SELFROMKEY) {
                _tcsncpy(pMenu[0].auchPluNo, auchPluNo, NUM_PLU_LEN);
                pMenu[0].uchAdj = uchChildAdj;
                usOrderNo = 0;  /* V3.3 */
            } else {
                usNoOfGroup = 1;    /* for insert control */
            }

            /* V3.3 */
            if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
				TCHAR      auchDummy[NUM_PLU_LEN] = {0};

                for (i=(nNoOrderPmt-1); i>0; i--) {
                    if (pDisp->OrderPmt[i].uchOrder) break;
                }
                if (((UCHAR)usOrderNo >= pDisp->OrderPmt[0].uchOrder) && ((UCHAR)usOrderNo <= pDisp->OrderPmt[i].uchOrder)) {
                    /* entered order no. exists on menu */
                    ;
                } else {
                    /* --- remake PLU data class --- */
                    lCounter = 0L;
                    for (i=SLOEP_ENTRY_NO; i <= usOrderNo; i+=nNoOrderPmt ){
                        /* remake "Menu" data, if usOrderNo is out of scroll display */
                        memset(pMenu, 0, sizeof(ITEMSALESOEPLOCAL) * (128));
                        sResult = ItemSalesOEPMakeMenu(pszGroups[usGroupPos],
                                                   uchNounAdj,
                                                   pMenu,
                                                   pDisp, nNoOrderPmt,
                                                   &usNumMenu,
                                                   &lCounter,
                                                   &lCounter);
                    }
                }

                /* check entry data is valid or not */
                usOrderNo -= SLOEP_ENTRY_NO;
                while(usOrderNo >= nNoOrderPmt)
				{
					usOrderNo-= nNoOrderPmt;
				}
                if (_tcsncmp(pMenu[usOrderNo].auchPluNo, auchDummy, NUM_PLU_LEN) == 0) { /* 2172 */
                    UieErrorMsg(LDT_NTINFL_ADR, UIE_WITHOUT);

					// set indicator to 102 because of input error - CSMALL 2/7/06
					BlFwIfSetOEPGroups(BL_OEPGROUP_STR_DECREMENT, 0);

                    continue;
                }
            }

            sResult = ItemSalesOEPMakeNewClass(pUifRegSales,
                                               &pMenu[usOrderNo],
                                               &uchNounAdj,
                                               &aszNounMnem[0],
                                               &lNounAmount,
                                               &uchCondStatus,
                                               usOption1, usOption2, usOption3,
                                               &auchGroupNo[0],
                                               fbModifier2,
                                               &uchTableNumber);
			if(uchTableNumber)
			{
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			}
            if (sResult == UIF_CANCEL) {        /* cancel spv. int.  */
                continue;                       /* retry ...         */
            } else if (sResult == ITM_CONT) {   /* 97 option error */
				
				// fix for SR 867 - prevents moving forward in sequence
				//	of OEP Groups when an error is incountered.  CSMALL 1/31/06
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_DECREMENT, 0);

                UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                continue;
            } else if (sResult != ITM_SUCCESS) {
                usContinueLoop = 2;
				break;
            }

            /* --- check free option --- */
            if (usOption2) {

                /* change free option for multiple selection  */

                for (i=0; i<usNoOfOption; i++) {
                    if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREEONE) {
                        pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_DUMMY;
                        break;
                    }
                    if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREETWO) {
                        pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREEONE;
                        break;
                    }
                    if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREETHREE) {
                        pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREETWO;
                        break;
                    }
                    if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREEFOUR) {
                        pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREETHREE;
                        break;
                    }
                }

                /* decrement loop conter */

                usContinue--;
                if (!usContinue) {                  /* after free sales */
                    if (usOption1 == SLOEP_MULTI) { /* if multiple sales */
                        usOption2 = 0;              /* not free sales */
                        usContinue++;
                    }
                }
            }

            /* --- update group number of OEP --- */

            sResult = ItemSalesOEPUpdateGroupNo(uchCondStatus, /* 2172 */
                                                pszGroups,
                                                usGroupPos,
                                                &fsType,
                                                usContinue,
                                                usNoOfGroup,
												uchTableNumber);
            if (sResult != ITM_SUCCESS && sResult != ITM_CANCEL) {
                usContinueLoop = 2;
				break;
            }

            /* --- inserted new group number ? --- */

            if (fsType & SLOEP_F_INSERT) {
                if (fsType & SLOEP_F_MULTI) {   /* multiple item     */
                    usGroupPos -= (usNoOfOption+usNoOfGroup);
                }
                break;                          /* exit loop         */
            }

            /* --- check loop function  --- */

            if (!usContinue) {                  /* single item       */
                break;                          /* exit loop         */
            }
        }

		if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
			MldDeletePopUp();  // Delete the popup managed within this loop
			ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
		}
		if (usContinueLoop == 2) {
			return (sResult);               /* exit ...          */
		}

        memset(ausOption, 0x00, sizeof(ausOption));
        usNoOfOption=0;
		//SR 609, reset the index back to 0, because we are starting on a new group JHHJ
		sSlOepCounterIndex = 0;
    } // for (usGroupPos = 0; pszGroups[usGroupPos] != 0; usGroupPos++)

    if (usOption3 == SLOEP_SELFROMKEY) {    /* for "00" exit case */
        /* set group no. of condiment */
        memcpy(&ItemSalesLocal.auchOepGroupNo, &auchGroupNo, SALES_NUM_GROUP + 1);
    }
	BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
    
	return (ITM_SUCCESS);                       /* exit ...          */
}


/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPMakeMenu(UCHAR uchGroupNo,
*                                          UCHAR uchNounAdj,
*                                          ITEMSALESOEPLOCAL *pItem,
*                                          MLDPOPORDER *pDisp,
*                                          USHORT *pusNumItem);
*     Input:    uchGroupNo - group number
*               uchNounAdj - adjective number of noun
*    Output:    pMenu      - address of item data table
*               pDisp      - address of display data
*               pusNumItem - address of number of item
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
STATIC SHORT ItemSalesOEPMakeMenu(UCHAR uchGroupNo, UCHAR uchNounAdj,
                           ITEMSALESOEPLOCAL *pMenu, MLDPOPORDER_VARY *pDisp,
                           int nNoOrderPmt, USHORT *pusNumItem,
                           LONG *pculCounterFirst, LONG *pculCounterLast)
{
    /* SHORT               sResult; */
	int                 iPluOepIf, jPluOepIf;
    USHORT              usOff, usOffOepIf;
    PLUIF_OEP_VARY      *pIndex = 0;
    PLUIF               Plu;
    PARAADJMNEMO        Adj;
    ITEMSALESOEPLOCAL   *pWorkMenu;
    ORDERPMT            *pWorkDisp;
	TCHAR               auchDummy[NUM_PLU_LEN] = {0};
	OPPLUOEPIF		PluOepIf[5];
    
    /* --- initialization --- */
	pIndex = (PLUIF_OEP_VARY *) alloca (sizeof(PLUIF_OEP_VARY) + sizeof(PLU_PARA) * (nNoOrderPmt + 1));
 	NHPOS_ASSERT(pIndex);
	if (!pIndex) return (ITM_CANCEL);    // return doing nothing if alloca() fails.

	memset (pIndex, 0, sizeof(PLUIF_OEP_VARY) + sizeof(PLU_PARA) * (nNoOrderPmt + 1));
	pIndex->usGroupNo = uchGroupNo;
	pIndex->culCounter = *pculCounterFirst;

    for (*pusNumItem = 0;;) {

        /* --- get index record of OEP object --- */

		memset (pIndex->PluPara20, 0,  sizeof(PLU_PARA) * (nNoOrderPmt + 1));
        if (CliOpOepPluRead(pIndex, 0, nNoOrderPmt) != OP_PERFORM) {
			PifLog (MODULE_SALES, LOG_EVENT_SALES_MAKEMENU_PLU_1);
            return (ITM_CANCEL);
        }

		if (pIndex->PluPara20[0].auchPluNo[0] == 0) {
			// there are no more PLUs in the group to process
			break;
		}

		memset(&PluOepIf, 0, sizeof(PluOepIf));
		for (iPluOepIf = jPluOepIf = 0; iPluOepIf < nNoOrderPmt; iPluOepIf += OP_OEP_PLU_SIZE, jPluOepIf++) {
			SHORT  sRetStatus;
			for (usOff = 0; usOff < OP_OEP_PLU_SIZE; usOff++) {
				if (iPluOepIf + usOff >= nNoOrderPmt)
					break;
				_tcsncpy(&PluOepIf[jPluOepIf].PluNo.auchPluNo[usOff][0], pIndex->PluPara20[iPluOepIf + usOff].auchPluNo, NUM_PLU_LEN);
			}
			sRetStatus = CliOpPluOepRead(&PluOepIf[jPluOepIf], 0);
			if (sRetStatus != OP_PERFORM) {
				char aszErrorBuffer[128];
				PifLog (MODULE_SALES, LOG_EVENT_SALES_MAKEMENU_PLU_2);
				sprintf (aszErrorBuffer, "ItemSalesOEPMakeMenu() error from CliOpPluOepRead().  PLU: %S  jPluOepIf: %d. ", PluOepIf[jPluOepIf].PluNo.auchPluNo, jPluOepIf);
				NHPOS_ASSERT_TEXT(sRetStatus == OP_PERFORM, aszErrorBuffer);
				return (ITM_CANCEL);
			}
		}

        for (usOff = 0; usOff < nNoOrderPmt; usOff++) { /* OP_OEP_PLU_SIZE changed to nNoOrderPmt for exact number of items - CSMALL */

			jPluOepIf = usOff / OP_OEP_PLU_SIZE;
			usOffOepIf = usOff % OP_OEP_PLU_SIZE;
            /* --- check end of index record --- */

            if ((_tcsncmp(pIndex->PluPara20[usOff].auchPluNo, auchDummy, NUM_PLU_LEN) == 0) ||  /* finalize       */
            /* if ((Index.PluPara20[usOff].usPluNo == 0) ||  / finalize       */
                (SLOEP_ENTRY_NO + *pusNumItem + sSlOepCounterIndex * nNoOrderPmt > SLOEP_ENTRY_NO_END)) { /* V3.3 */
                if (*pusNumItem) {          /* already OEP object found   */
                    *pculCounterLast = pIndex->culCounter;
                    return (ITM_SUCCESS);
                } else {                    /* OEP object not found       */
                    return (ITM_CANCEL);
                }
            }

            /* --- set OEP object for item data --- */

            pWorkMenu          = &pMenu[*pusNumItem];
            _tcsncpy(pWorkMenu->auchPluNo, pIndex->PluPara20[usOff].auchPluNo, NUM_PLU_LEN);
            /* pWorkMenu->usPluNo = Plu.usPluNo; */
            pWorkMenu->uchAdj  = pIndex->PluPara20[usOff].uchPluAdj;

			//SR 336 Begin Fix for prohibited adjective with OEP display
			if(pWorkMenu->uchAdj)
			{
				//Check to see if the adjective has been prohibited by the PLU, if so, we will just continue
				//on to the next PLU
				if(LDT_PROHBT_ADR == ItemSalesSetCheckValidation(PluOepIf[jPluOepIf].PluName.auchContOther[2][usOffOepIf],pWorkMenu->uchAdj))
				{
					continue;
				}
			}
			//SR 336 END

            /* --- set OEP object for display interface --- */

 
            /* --- check adjective item --- */

			pWorkDisp           = &pDisp->OrderPmt[*pusNumItem];

            if ((PluOepIf[jPluOepIf].PluName.uchItemType[usOffOepIf] & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {

	            memset(&Plu, 0, sizeof(Plu));
    	        _tcsncpy(Plu.auchPluNo, pIndex->PluPara20[usOff].auchPluNo, NUM_PLU_LEN);
            	Plu.uchPluAdj = pIndex->PluPara20[usOff].uchPluAdj;
	    		Plu.ParaPlu.ContPlu.uchItemType = PluOepIf[jPluOepIf].PluName.uchItemType[usOffOepIf];
    			Plu.ParaPlu.ContPlu.auchContOther[2] = PluOepIf[jPluOepIf].PluName.auchContOther[0][usOffOepIf];
    			Plu.ParaPlu.ContPlu.auchContOther[3] = PluOepIf[jPluOepIf].PluName.auchContOther[1][usOffOepIf];
	
	            /* --- check menu status --- */

                if (Plu.ParaPlu.ContPlu.auchContOther[2] & PLU_CONDIMENT) {
	                if (uchNounAdj == 0) {      /* non adjective for noun     */
    	                continue;               /* next ...                   */
        	        }

            	    if (((uchNounAdj - 1) % 5 + 1) != Plu.uchPluAdj) {
                	    continue;               /* next ...                   */
                	}
                	
	                /* --- get adjective mnemonic --- */

	                Adj.uchMajorClass = CLASS_PARAADJMNEMO;
    	            Adj.uchAddress    = uchNounAdj;
        	        CliParaRead(&Adj);
                	
            	} else {
            	
	                /* --- get adjective mnemonic --- */

	                Adj.uchMajorClass = CLASS_PARAADJMNEMO;
    	            Adj.uchAddress    = ItemSalesOEPGetAdjNo(&Plu);
        	        CliParaRead(&Adj);
				}
                /* --- set adjective mnemonic --- */

                _tcscpy(pWorkDisp->aszAdjective, Adj.aszMnemonics);
            } 

			//Fix for SR 513, we move this down below the Adjective information so that we do not copy information
			//that is not needed into the buffer for displaying OEP buttons.  When the information was before the adjective
			//checking, it would put the name of the item into the buffer whether it was needed or not.  So that means if
			//the item was not needed, it still would have been in the buffer. JHHJ
			pWorkDisp->uchOrder = (UCHAR)(SLOEP_ENTRY_NO + * pusNumItem + sSlOepCounterIndex * nNoOrderPmt); /* V3.3 */
			_tcsncpy(pWorkDisp->aszPluName, &PluOepIf[jPluOepIf].PluName.auchPluName[usOffOepIf][0], STD_PLU_MNEMONIC_LEN);
			if (pWorkDisp->aszPluName[0] == 0) {
				pWorkDisp->aszPluName[0] = _T(' ');
			}
			
            /* --- check number of OEP object --- */

			pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_MORE;
            if (++(*pusNumItem) >= nNoOrderPmt) { /* all object found */
                /* V3.3 */
                *pculCounterLast = pIndex->culCounter;

                /* read next index for continuous display, mnemonic TRN_CONTINUE_ADR.  */
                pIndex->culCounter = *pculCounterLast;
				memset (pIndex->PluPara20, 0,  sizeof(PLU_PARA) * (nNoOrderPmt + 1));
                if (CliOpOepPluRead(pIndex, 0, nNoOrderPmt) == OP_PERFORM) { /* added nNoOrderPmt to function to pass number of items
																				requested - CSMALL */
                    if (_tcsncmp(pIndex->PluPara20[0].auchPluNo, auchDummy, NUM_PLU_LEN) != 0) {
						pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_MORE;
					}
                }
                return (ITM_SUCCESS);
            }
        }
    }
	return (ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPCheckObjectPLU(PLUIF *pPlu);
*     Input:    pPlu - address of PLU record
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPCheckObjectPLU(ITEMSALES *pItem)
{
    UCHAR           fchMask;
    PARAMENUPLUTBL  SetMenu;

    /* --- check scalable/hash PLU --- */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE || pItem->ControlCode.auchPluStatus[2] & PLU_HASH) {
        return (LDT_PROHBT_ADR);
    }

    /* --- check open PLU --- */
    if ((pItem->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_OPEN_TYP) {
        return (LDT_PROHBT_ADR);
    }

    /* --- check adjective item --- */
    if ((pItem->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
        fchMask = (UCHAR)(0x08 << (pItem->uchAdjective - 1));
        if (pItem->ControlCode.auchPluStatus[4] & fchMask) {
            return (LDT_PROHBT_ADR);
        }
    }

    /* --- check set menu --- */

    memset(&SetMenu, 0, sizeof(SetMenu));
    SetMenu.uchMajorClass   = CLASS_PARAMENUPLUTBL;
    _tcsncpy(SetMenu.SetPLU[0].aszPLUNumber, pItem->auchPLUNo, NUM_PLU_LEN);
    SetMenu.uchAddress      = 0;

    if (CliParaMenuPLURead(&SetMenu) != LDT_NTINFL_ADR) {
        return (LDT_PROHBT_ADR);
    }

    return (ITM_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPOrderPlu(USHORT usNumber,
*                                          USHORT *pusOrderNo);
*     Input:    usNumber   - number of item data
*               pusOrderNo - address of order number
*
** Return:      ITM_SUCCESS - successful
*               UIF_CANCEL  - cancel
*===========================================================================
*/
SHORT ItemSalesOEPOrderPlu(USHORT *pusOrderNo, UCHAR *puchAdjNo,
                           MLDPOPORDER_VARY *pDisp,
                           USHORT usOption1, USHORT usOption2, USHORT usOption3, TCHAR *pauchPluNo, UCHAR *pfbModifier2)
{
	UIFDIADATA      UI = { 0 };
	REGDISPMSG      Message = { 0 };
    ORDERPMT        PopupData;
    PARACTLTBLMENU  MenuRcvBuff;    /* Menu page data receive buffer */
    UCHAR           uchAdjSave=0;   /* default 1 adjective */

    *pfbModifier2 = 0;

    /* --- display lead through mnemonic --- */
    Message.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    if (PifSysConfig()->uchOperType == DISP_LCD) {
        /* display only leadthrough at lcd */
        Message.uchMinorClass = CLASS_REGDISP_OEPNO;
    } else {
        /* display noun mnemonic & price at 2x20 */
        Message.uchMinorClass = CLASS_REGDISP_OEP_LEAD;
        _tcsncpy(Message.aszAdjMnem, pDisp->aszAdjective, STD_ADJMNEMO_LEN);
        _tcsncpy(Message.aszStringData, pDisp->aszPluName, PARA_PLU_LEN);
        Message.lAmount = pDisp->lPrice;
    }
    _tcsncpy(Message.aszMnemonic, pDisp->aszLeadThrough, PARA_LEADTHRU_LEN);
    Message.fbSaveControl = DISP_SAVE_TYPE_4;

    for (;;) {
        if (uchAdjSave) {
			REGDISPMSG      DispAdj = { 0 };

			DispAdj.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
            DispAdj.uchMinorClass = CLASS_UIFDISP_ADJ;   
            DispAdj.uchFsc = uchAdjSave;
            DispWrite(&DispAdj);                        /* display adjective */
        } else {
            if (((PifSysConfig()->uchOperType == DISP_2X20) ||
                 (PifSysConfig()->uchOperType == DISP_10N10D))
                && (usOption3 != SLOEP_SELFROMKEY)) {

                /* display leadthrough of 2x20 */
                MldPopUpRedisplay(MLD_ONLY_DISPLAY);
            } else {
                /* display leadthrough of LCD */
                flDispRegDescrControl |= flSlOepSaveDescr & (TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);
                flDispRegKeepControl  |= flSlOepSaveKeep  & (TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);
                DispWrite(&Message);
                flDispRegDescrControl &= ~(TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);
                flDispRegKeepControl  &= ~(TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL); 
            }
        }

        memset(&UI, 0, sizeof(UI));

        /* --- get target item --- */
        if (usOption3 == SLOEP_SELFROMKEY) {
            /* allow 13 digit plu no entry, 2172 */
            if (UifDiaOEPKey(&UI) != UIF_SUCCESS) {    /* abort by user     */
                ItemOtherClear();                   /* clear descriptor  */
                return (UIF_CANCEL);                /* exit ...          */
            }
        } else {
            /* usControl = UieNotonFile(UIE_ENABLE);                   / disable using scanner */
            if (UifDiaOEP(&UI, 0) != UIF_SUCCESS) {    /* abort by user     */
                ItemOtherClear();                   /* clear descriptor  */
                /* UieNotonFile(usControl);                      / enable scanner */
                return (UIF_CANCEL);                /* exit ...          */
            }
            /* UieNotonFile(usControl);                      / enable scanner */
        }
        
        /* --- read plu key at 97 option R3.1 --- */
        if (usOption3 == SLOEP_SELFROMKEY) {
            if ((UI.auchFsc[0] == FSC_KEYED_PLU) ||   /* direct plu key entry */
                (UI.auchFsc[0] == FSC_PLU)) {   /* direct plu key entry */

                if (ItemSalesOEPSetNoOfPluKey(&UI, pauchPluNo) == ITM_SUCCESS) {
                /* if (ItemSalesOEPSetNoOfPluKey(UI.auchFsc[1], pusOrderNo) == ITM_SUCCESS) { */
                    *puchAdjNo = uchAdjSave;
                    *pusOrderNo = SLOEP_PLUKEY;
                    return (ITM_SUCCESS);
                } else {
                    UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                    continue;
                }
/*****      delete adjective key process for only condiment entry
            } else if (UI.auchFsc[0] == FSC_ADJECTIVE) {    / adjective entry /
                uchAdjSave = UI.auchFsc[1];
                continue;
******/
            } else if ((UI.auchFsc[0] == FSC_MENU_SHIFT) ||
                       (UI.auchFsc[0] == FSC_D_MENU_SHIFT) ||
                       (UI.auchFsc[0] == FSC_CLEAR)) {
                ;   /* same as other option */
            } else {
                UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                continue;
            }
        } else {
            if ((UI.auchFsc[0] == FSC_KEYED_PLU) ||
                (UI.auchFsc[0] == FSC_PLU) ||
                (UI.auchFsc[0] == FSC_ADJECTIVE)) {

                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
                continue;
            }
        }

        /* --- scroll up/down, or press a/c at 2x20 scroll R3.1 */
        if (UI.auchFsc[0] == FSC_SCROLL_DOWN) {

            /* V3.3 */
            if (MldPopUpDownCursor() == MLD_CURSOR_BOTTOM) {
                if (sSlOepCounterIndex < (SLOEP_NUM_GROUP_IDX-1)) {
                    sSlOepCounterIndex++;
                }
                *pusOrderNo = SLOEP_SCROLL_DOWN;
                return (ITM_SUCCESS);
            }
            continue;

        } else if (UI.auchFsc[0] == FSC_SCROLL_UP) {

            /* V3.3 */
            if (MldPopUpUpCursor() == MLD_CURSOR_TOP) {
                if (!sSlOepCounterIndex) continue;  /* for 2x20 scroll */
                if (sSlOepCounterIndex) {
                    sSlOepCounterIndex--;
                }
                *pusOrderNo = SLOEP_SCROLL_UP;
                return (ITM_SUCCESS);
            }
            continue;

        } else if (UI.auchFsc[0] == FSC_AC) {
            if (MldPopUpGetCursorDisplay(&PopupData) == MLD_SUCCESS) {
                UI.ulData = (ULONG)PopupData.uchOrder;
            } else {
                /* --- display error message --- */
                UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                continue;
            }
        } else if (UI.auchFsc[0] == FSC_CLEAR) {
			// FSC_CLEAR is being used to indicate that NONE/DONE button
			// in the OEP window was pressed - CFrameworkWndButton::SPL_BTN_DONE
			UI.ulData = 0;
            *pusOrderNo = 0;
            return (ITM_SUCCESS);
		}

        /* ---- shift page for direct plu key entry ---- */
        if (UI.auchFsc[0] == FSC_MENU_SHIFT) {   /* shift key */
            if (UI.ulData > (LONG)MAX_PAGE_NO) {
                UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
                continue;
            }
            if (!UI.ulData) {
                /* reset default page */
                MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
                MenuRcvBuff.uchAddress = CTLPG_STAND_ADR;
                CliParaRead(&MenuRcvBuff);
                uchDispCurrMenuPage = MenuRcvBuff.uchPageNumber;
            } else {
                uchDispCurrMenuPage = (UCHAR)UI.ulData;
            }
            ItemCommonShiftPage(uchDispCurrMenuPage);
            
            /* default adjective by shift page, 2172 */
            MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
            MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + CTLPG_ADJ_PAGE1_ADR - 1);
            CliParaRead(&MenuRcvBuff);
            if (ItemSalesLocal.uchAdjKeyShift == 0) {   /* not use adj. shift key */
                ItemSalesLocal.uchAdjCurShift = MenuRcvBuff.uchPageNumber;
            }
            continue;
        } else if (UI.auchFsc[0] == FSC_D_MENU_SHIFT) { /* shift key */
            uchDispCurrMenuPage = UI.auchFsc[1];
            ItemCommonShiftPage(uchDispCurrMenuPage);

            /* default adjective by shift page, 2172 */
            MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
            MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + CTLPG_ADJ_PAGE1_ADR - 1);
            CliParaRead(&MenuRcvBuff);
            if (ItemSalesLocal.uchAdjKeyShift == 0) {   /* not use adj. shift key */
                ItemSalesLocal.uchAdjCurShift = MenuRcvBuff.uchPageNumber;
            }
            continue;
        } else if (UI.auchFsc[0] == FSC_CLEAR || UI.auchFsc[0] == FSC_ERROR) {
            uchAdjSave = 0;
            continue;
        } else if (UI.auchFsc[0] == FSC_PRICE_CHECK) {
            *pfbModifier2 |= PCHK_MODIFIER;
            continue;
        }else if (pDisp->uchMinorClass == CLASS_POPORDERPMT_ADJ)
		{
			*puchAdjNo = UI.ulData;
			break;
		}

        /* --- check valid order number --- */
        if ((ULONG)SLOEP_ENTRY_NO <= UI.ulData &&
            UI.ulData <= (ULONG)(SLOEP_ENTRY_NO_END)) {  /* V3.3 */
            break;                              /* exit loop ...     */
        } else if (UI.ulData == 0L) {           /* skip current menu */
            if (usOption1 != SLOEP_COMPNO) {     /* compulsory option R3.1 */
                break;                          /* exit loop ...     */
            }
        }

        /* --- display error message --- */
        UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);
    }

    *pusOrderNo = (USHORT)UI.ulData;            /* set order number  */

    return (ITM_SUCCESS);

    usOption2 = usOption2;
}


/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPMakeNewClass(UIFREGSALES *pUifRegSales,
*                                              ITEMSALESOEPLOCAL *pItem,
*                                              UCHAR *puchNounAdj);
*     Input:    pUifRegSales - address of UIF data class
*     Input:    pItem        - address of order item
*     InOut:    puchNounAdj  - address of adjective number of noun PLU
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPMakeNewClass(UIFREGSALES *pUifRegSales,
                               ITEMSALESOEPLOCAL *pItem, UCHAR *puchNounAdj,
                               TCHAR *pszNounMnem, DCURRENCY *plNounAmount, UCHAR *puchStatus,
                               USHORT usOption1, USHORT usOption2, USHORT usOption3,
                               UCHAR  *puchGroupNo, UCHAR fbModifier2, UCHAR *puchTableNumber)
{
    DCURRENCY       lAmount;
	TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
    ITEMMODLOCAL    *pWorkMod = ItemModGetLocalPtr();
	UIFREGSALES     WorkReg = {0};
	ITEMSALES       WorkItem = {0};
    SHORT           sResult, sSaved;
    USHORT          usLinkNo;
    UCHAR           uchRestrict, uchDummy;
    UCHAR           uchGroup;

    /* --- replace data class of UIF --- */
    /* Check CDV System */
    WorkReg.uchMajorClass = pUifRegSales->uchMajorClass;
    WorkReg.uchMinorClass = pUifRegSales->uchMinorClass;
    _tcsncpy(WorkReg.aszPLUNo, pItem->auchPluNo, NUM_PLU_LEN);
    /* if (! (WorkPlu.ParaPlu.ContPlu.auchContOther[2] & PLU_CONDIMENT)) { */
		WorkReg.uchAdjective= pItem->uchAdj;
        WorkReg.lQTY         = lSlOepSaveQTY;
        WorkReg.fbModifier   = pUifRegSales->fbModifier;

        /* --- repalce taxable status in local modifier --- */
        pWorkMod->fsTaxable |= SlOepSaveMod.fsTaxable & (MOD_TAXABLE1 | MOD_TAXABLE2 | MOD_TAXABLE3);
    /* } */
    WorkReg.fbModifier2 = fbModifier2; /* price check status 2172 */

    WorkItem.uchMajorClass = CLASS_ITEMSALES;
    WorkItem.uchMinorClass = CLASS_PLU;
    _tcsncpy(WorkItem.auchPLUNo, pItem->auchPluNo, NUM_PLU_LEN);
    /* WorkItem.uchAdjective = ItemSalesOEPGetAdjNo(&WorkPlu); */

    if ((sResult = ItemSalesPrepare(&WorkReg, &WorkItem, &uchRestrict, &usLinkNo, &uchDummy)) != ITM_SUCCESS) {
        return (sResult);
    }

    if ((WorkItem.ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
    /* if ((WorkPlu.ParaPlu.ContPlu.uchDept & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) { */
        if (pItem->uchAdj == 0) {   /* check adjective entry */
            return(ITM_CONT);
		}
    	if (WorkItem.ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
			ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

			//if (pItem->uchAdj )
    		if (pItem->uchAdj != ((pCommonLocalRcvBuff->ItemSales.uchAdjective - 1) % 5 + 1)) {
				if (pItem->uchAdj == ((pCommonLocalRcvBuff->CondimentEdit.uchAdjective - 1) % 5 + 1)) {
					WorkReg.uchAdjective =0;
					WorkItem.uchAdjective = pCommonLocalRcvBuff->CondimentEdit.uchAdjective;	
				}else if((pCommonLocalRcvBuff->CondimentEdit.uchAdjective == 0) && (pCommonLocalRcvBuff->ItemSales.uchAdjective == 0)){
					WorkReg.uchAdjective =0;
					WorkItem.uchAdjective = pItem->uchAdj;	
				}
				else {
					return (LDT_PROHBT_ADR);
				}
    		}
			WorkReg.uchAdjective =0;
			WorkItem.uchAdjective = pCommonLocalRcvBuff->CondimentEdit.uchAdjective;
		} else {
			WorkItem.uchAdjective = pItem->uchAdj;
	    	uchGroup = (UCHAR)(WorkItem.ControlCode.auchPluStatus[3] >> 4);

		    if (uchGroup & 0x01) {                  /* group number 1 */
    		    ;
	    	} else if (uchGroup & 0x02) {           /* group number 2 */
    	    	WorkItem.uchAdjective += 5;
		    } else if (uchGroup & 0x04) {           /* group number 3 */
    		    WorkItem.uchAdjective += 10;
	    	} else {                                /* group number 4 */
    	    	WorkItem.uchAdjective += 15;
		    }
    	    WorkReg.uchAdjective = WorkItem.uchAdjective;
		}
    } else {
    	WorkReg.uchAdjective =0;
    }

    /* --- check PLU status --- */
    if (ItemSalesOEPCheckObjectPLU(&WorkItem) != ITM_SUCCESS) {
    /* if (ItemSalesOEPCheckObjectPLU(&WorkPlu) != ITM_SUCCESS) { */
        return(ITM_CONT);
    }

    if ((WorkItem.ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
		WorkItem.lQTY = 0;
	}
	else {
		WorkItem.usUniqueID = ItemSalesGetUniqueId(); //unique id for condiment editing JHHJ
		WorkItem.usKdsLineNo = 0;                     //reset KDS line number so that a new one will be assigned by ItemSendKdsItemGetLineNumber()
	}

    /* --- verify PLU group number at 97 option --- */
    if (usOption3 == SLOEP_SELFROMKEY) {

        /* request only condiment plu */
        if (! (WorkItem.ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
            return(ITM_CONT);
        }

        while(*puchGroupNo != 0) {
            if (WorkItem.uchGroupNumber == *puchGroupNo) {
                break;
            }
            puchGroupNo++;
        }
        if (*puchGroupNo == 0) { /* not terget plu, retry */
            return(ITM_CONT);
        }
    }

    /* ----- check sales restriction, 2172 ----- */

    if (ItemSalesSalesRestriction(&WorkItem, uchRestrict) != ITM_SUCCESS) {
        return (LDT_SALESRESTRICTED);
    }

    if ((sResult = ItemSalesCommon(&WorkReg, &WorkItem)) != ITM_SUCCESS) {
        return (sResult);
    }

    if (! (WorkItem.ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {

        /* --- check print modifier or comdiment compulsory status --- */

        if ((sResult = ItemCommonCheckComp()) != ITM_SUCCESS) {
            return (sResult);
        }

        switch(WorkItem.ControlCode.uchItemType & PLU_TYPE_MASK_REG) {
        /* switch(WorkItem.ControlCode.uchDeptNo & PLU_TYPE_MASK_REG) { */
        case PLU_NON_ADJ_TYP:
            sResult = ItemSalesNonAdj(&WorkReg);
            break;
        case PLU_ADJ_TYP:
            sResult = ItemSalesAdj(&WorkReg, &WorkItem);
            break;
        default:
            sResult = ItemSalesDeptOpen(&WorkReg, &WorkItem);
            break;
        }
    } else  if((pWorkCur->fbOepCondimentEdit == OEP_CONDIMENT_EDIT) || //SR 192 JHHJ
		(pWorkCur->fbOepCondimentEdit == OEP_CONDIMENT_ADD)) {
		ITEMCOND        CondimentData = {0};
		REGDISPMSG      DisplayData = {0};

#if 1
		CondimentData = ItemCondimentData(&WorkItem);
#else
		/* --- condiment process --- */
		//The user has chosen to add a condiment to an OEP
		//item.  We set the new item to the Condiment data structure
		//and send it to the function ItemPreviousCondiment.
		/*----- set condiment data to previous item -----*/
		
		// The auchPLUNo is exactly sized to NUM_PLU_LEN hence is not treated as
		// a zero terminated character string.
		memcpy(CondimentData.auchPLUNo, &WorkItem.auchPLUNo, sizeof(CondimentData.auchPLUNo));          
		CondimentData.usDeptNo = WorkItem.usDeptNo;      
		CondimentData.lUnitPrice = WorkItem.lUnitPrice; 
		CondimentData.uchPrintPriority = WorkItem.uchPrintPriority; /* R3.0 */
		CondimentData.uchCondColorPaletteCode = WorkItem.uchColorPaletteCode;

		memcpy(CondimentData.aszMnemonic, &WorkItem.aszMnemonic, sizeof(CondimentData.aszMnemonic)); 
		memcpy(CondimentData.aszMnemonicAlt, &WorkItem.aszMnemonicAlt, sizeof(CondimentData.aszMnemonicAlt)); 
		CondimentData.ControlCode = WorkItem.ControlCode;
#endif

		if (pWorkCur->fbOepCondimentEdit == OEP_CONDIMENT_EDIT) {
			//We check to see if the function returns success, if it does not we log an
			//error and return out.
			if(!ItemPreviousCondiment(&CondimentData, MLD_CONDIMENT_UPDATE_PREVIOUS))
			{
				PifLog(MODULE_SALES, LOG_EVENT_SALES_PREV_CONDIMENT);
				return ITM_CANCEL;
			}
		}
		else if (pWorkCur->fbOepCondimentEdit == OEP_CONDIMENT_ADD) {
			//We check to see if the function returns success, if it does not we log an
			//error and return out.
			if(!ItemPreviousCondiment(&CondimentData, MLD_CONDIMENT_UPDATE))
			{
				PifLog(MODULE_SALES, LOG_EVENT_SALES_PREV_CONDIMENT);
				return ITM_CANCEL;
			}
		}
		//We have returned success, so we will turn off the 
		//pop up OEP window and change the status bit to off
		if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) 
		{
            MldDeletePopUp();
            ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
		}
		//set the variable back to false
		pWorkCur->fbOepCondimentEdit = OEP_CONDIMENT_RESET;
	    /* Check CDV System */

//		ItemSalesSalesAmtCalc(&ItemSalesLocal, &DisplayData.lAmount);
		DisplayData.lAmount = 1;
		WorkReg.uchMinorClass = CLASS_PLU;
		ItemSalesDisplay(&WorkReg, &WorkItem, &DisplayData);
		
		return ITM_PREVIOUS;
	}
	else {
        /* --- condiment process --- */
        sResult = ItemSalesCondiment(&WorkReg, &WorkItem);
    }

    if (sResult != ITM_SUCCESS) {
        if (sResult != ITM_COMP_ADJ) {
            return (sResult);
        }
        sSaved = LDT_PROHBT_ADR;
    } else {
        sSaved = ITM_SUCCESS;
    }

    if ((sResult = ItemSalesSetMenu(&WorkReg, &WorkItem)) == ITM_CANCEL) {
        if (sSaved != ITM_SUCCESS) {
            return (sSaved);
        }
    } else if (sResult != ITM_SUCCESS) {
        return (sResult);
    }

    /* ----- link plu, 2172 ----- */
    sResult = ItemSalesLinkPLU(&WorkItem, 1); /* check link plu is acceptable */
    if (sResult != ITM_SUCCESS) {                     
        return (LDT_PROHBT_ADR);
    }
        
    /* --- reset compulsory PM and condiment status --- */

    if (usOption3 != SLOEP_SELFROMKEY) {
        WorkItem.ControlCode.auchPluStatus[0] &= ~(PLU_COMP_MODKEY | PLU_COMP_CODKEY);
    }

    if ((sResult = ItemSalesModifier(&WorkReg, &WorkItem)) != ITM_SUCCESS) {
        return (sResult);
    }

    /* --- set free amount by option R3.1 --- */

    if ((usOption2 == SLOEP_FREEONE) || (usOption2 == SLOEP_FREETWO) ||
        (usOption2 == SLOEP_FREETHREE) || (usOption2 == SLOEP_FREEFOUR)) {

        WorkItem.lUnitPrice = 0L;
        WorkItem.lProduct = 0L;
        WorkItem.uchPPICode = 0;    /* not calculate ppi */
    } else {
        if ((sResult = ItemSalesCalculation(&WorkItem)) != ITM_SUCCESS) {
            return (sResult);
        }
        
        /* price change, 2172 */
        if ((sResult = ItemSalesPriceChange(&WorkItem)) == ITM_CANCEL) {
            return(UIF_CANCEL);
        } else if (sResult == ITM_PRICECHANGE) {
            /* recalculation by changed unit price */
            if (WorkItem.lProduct == 0L) {
                if ((sResult = ItemSalesCalculation(&WorkItem)) != ITM_SUCCESS) {
                    return(sResult);
                }
            }
        } else if (sResult != ITM_SUCCESS) {
            return(sResult);
        }
    }

    /* ----- link plu, 2172 ----- */
    ItemSalesLinkPLU(&WorkItem, 0); /* set link plu */

    if ((sResult = ItemSalesCommonIF(&WorkReg, &WorkItem)) != ITM_SUCCESS) {
        return (sResult);
    }
    
    /* ----- link plu, 2172 ----- */
    ItemSalesLinkPLU(&WorkItem, 2); /* send link plu to kds */

    if (!( WorkItem.ControlCode.auchPluStatus[2] & PLU_CONDIMENT)) {
        *puchNounAdj = WorkReg.uchAdjective;
        _tcsncpy(pszNounMnem, WorkItem.aszMnemonic, NUM_DEPTPLU);

		lAmount = 0;
		ItemSalesSalesAmtCalc(&WorkItem, &lAmount);
        *puchStatus = 0;
    } else {
		ITEMCOMMONLOCAL    *pCommonLocalRcvBuff = ItemCommonGetLocalPointer();

		lAmount = 0;
		ItemSalesSalesAmtCalc(&(pCommonLocalRcvBuff->ItemSales), &lAmount);
        *puchStatus = PLU_CONDIMENT;    /* set condiment status to ignor 97 option */
    }
    *plNounAmount = lAmount;
    
    *puchTableNumber = WorkItem.uchTableNumber;

    return (sResult);

    usOption1 = usOption1;
}
/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPUpdateGroupNo(USHORT usPlu,
*                                               UCHAR *pszGroup,
*                                               USHORT *pusNumber,
*                                               USHORT *pfsInsert);
*     Input:    usPlu     - PLU number
*    Output:    pszGroup  - address of group number table
*     InOut:    pusNumber - address of number of group number table
*               pfsInsert - address of insert flag
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPUpdateGroupNoCondiment(TCHAR *pauchPlu, UCHAR uchStatus, UCHAR *pszGroup,
   USHORT usGroupPos, USHORT *pfsType, USHORT usContinue, USHORT usNoOfGroup, UCHAR uchTableNumber)
{
	return ItemSalesOEPUpdateGroupNo(uchStatus, pszGroup,usGroupPos,pfsType,usContinue,
									usNoOfGroup, uchTableNumber);
}

/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPUpdateGroupNo(USHORT usPlu,
*                                               UCHAR *pszGroup,
*                                               USHORT *pusNumber,
*                                               USHORT *pfsInsert);
*     Input:    usPlu     - PLU number
*    Output:    pszGroup  - address of group number table
*     InOut:    pusNumber - address of number of group number table
*               pfsInsert - address of insert flag
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPUpdateGroupNo(UCHAR uchStatus, UCHAR *pszGroup,
   USHORT usGroupPos, USHORT *pfsType, USHORT usContinue, USHORT usNoOfGroup, UCHAR uchTableNumber)
{
    SHORT   /*sResult,*/ i;
    USHORT  usNumNew, usLeave, usInsert;
    /* UCHAR   uchTableNo; */
    UCHAR   szWorkGroup[STD_OEP_MAX_NUM + 1]; /* increased TAR #129199 */
    UCHAR   *pszCurrent;

    *pfsType = 0;                           /* clear flag               */

    /* --- get OEP table number --- */

    if (uchTableNumber > MAX_TABLE_NO) {
        return (LDT_PROHBT_ADR);
    } else if (uchTableNumber == 0) {
        return (ITM_CANCEL);
    }
    /* --- get OEP group number --- */

    usNumNew = ItemSalesOEPGetGroupNo(uchTableNumber, szWorkGroup);
    if (usNumNew == 0) {                    /* group number not found   */
        return (ITM_CANCEL);                /* exit ...                 */
    }

    /* --- check condiment status ---- */
    if (uchStatus & PLU_CONDIMENT) {
        for (i=0; i<STD_OEP_MAX_NUM; i++) {       /* increased TAR #129199 */
            if (szWorkGroup[i] == SLOEP_SELFROMKEY) {
                return (ITM_CANCEL);    /* if "97" option at condiment, ignor */
            }
        }
    }

    /* --- create group number ? --- */

    if (*(pszGroup + usGroupPos) == '\0') { /* group number not found   */
        memcpy(pszGroup, szWorkGroup, STD_OEP_MAX_NUM);   /* increased TAR #129199 */
        return (ITM_SUCCESS);               /* exit ...                 */
    }

    /* --- add or insert group number --- */

    if (usGroupPos != 0 && (usContinue)) {  /* multi. selection */
        for (i=usNoOfGroup; ((SHORT)usGroupPos - i) > 0; i++) {
            if (*(pszGroup + usGroupPos - i) < SLOEP_FREEONE) {

                i--;                            /* insert next table in front of multi. TAR #129199 */
                break;
            }
        }
        pszCurrent = pszGroup + usGroupPos - i; /* multi. selection */
        *pfsType  |= SLOEP_F_MULTI;
    } else {
        pszCurrent = pszGroup + usGroupPos + 1; /* single selection */
    }

    /* --- check number of current group --- */

    usLeave = STD_OEP_MAX_NUM - strlen(pszGroup);     /* increased TAR #129199 */

    if (usLeave == 0) {                     /* free space not found     */
        return (ITM_CANCEL);                /* exit ...                 */
    }

    /* --- check end of group number --- */

    usInsert = (usNumNew <= usLeave) ? (usNumNew) : (usLeave);

    while (szWorkGroup[usInsert - 1] >= SLOEP_FREEONE) { /* last group no is option */
        if (--usInsert == 0) {              /* new group not exist      */
            return (ITM_CANCEL);            /* exit ...                 */
        }
    }

    if (*pszCurrent) {                      /* exist group number       */
        *pfsType |= SLOEP_F_INSERT;         /* inserted new group no.   */

        /* --- check "97" option --- */
        for(i=(SHORT)(usInsert-1); i>=0; i--) {
            if (szWorkGroup[i] == SLOEP_SELFROMKEY) break;
        }

        if ((i>=0) && (*pszCurrent < SLOEP_FREEONE)) {
            memmove(pszCurrent + 1, pszCurrent, strlen(pszCurrent));
            *pszCurrent = SLOEP_DUMMY;  /* set dummy option to terminate group no */
        }
        memmove(pszCurrent + usInsert, pszCurrent, strlen(pszCurrent));
    }

    /* --- insert new group number --- */

    memcpy(pszCurrent, szWorkGroup, usInsert);

    return (ITM_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    USHORT ItemSalesOEPGetGroupNo(UCHAR uchTable,
*                                             UCHAR *pszGroup);
*     Input:    uchTable - OEP table number
*    Output:    pszGroup - address of group number table
*
** Return:      number of OEP group
*===========================================================================
*/
USHORT ItemSalesOEPGetGroupNo(UCHAR uchTable, UCHAR *pszGroup)
{
    PARAOEPTBL  OepTable;

    /* --- get OEP table --- */

    memset(&OepTable, 0, sizeof(PARAOEPTBL));
    OepTable.uchMajorClass = CLASS_PARAOEPTBL;
    OepTable.uchTblNumber  = uchTable;
    OepTable.uchTblAddr    = 0;

    CliParaOepRead(&OepTable);

    /* --- set OEP group number --- */

    memset(pszGroup, 0, STD_OEP_MAX_NUM + 1); /* increased TAR #129199 */
    memcpy(pszGroup, OepTable.uchOepData, SALES_NUM_GROUP);

    return ((USHORT)strlen(pszGroup));
}


/*
*===========================================================================
** Synopsis:    UCHAR ItemSalesOEPGetAdjNo(PLUIF *pPlu);
*     Input:    address of PLU record
*
** Return:      the adjective mnemonic address or zero.
*
*               determine the adjective mnemonic address for the adjective
*               specified for an Adjective PLU by using the adjective group
*               number specified for the PLU along with the adjective number
*               of this PLU.
*===========================================================================
*/
UCHAR ItemSalesOEPGetAdjNo(PLUIF *pPlu)
{
    UCHAR   uchAdj, uchGroup;

    /* --- check if this is an Adjective PLU --- */
    if ((pPlu->ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) != PLU_ADJ_TYP) {
        return (0);
    }

    uchGroup = (UCHAR)(pPlu->ParaPlu.ContPlu.auchContOther[3] >> 4);
    uchAdj   = pPlu->uchPluAdj;
	if (uchAdj == 0) return 0;    // if no Adjective then we can not figure out the mnemonic

	// determine the adjective mnemonic address using adjective group number
	// and the actual adjective number, 1 - 5, of the Adjective PLU.
    if (uchGroup & 0x01) {                  /* adjective group number 1 */
        ;
    } else if (uchGroup & 0x02) {           /* adjective group number 2 */
        uchAdj += 5;
    } else if (uchGroup & 0x04) {           /* adjective group number 3 */
        uchAdj += 10;
    } else {                                /* adjective group number 4 */
        uchAdj += 15;
    }
        
    return (uchAdj);
}

/*
*===========================================================================
** Synopsis:    UCHAR ItemSalesOEPGetAdjNoImplied(PLUIF *pPlu);
*     Input:    address of PLU record
*
** Return:      the adjective mnemonic address or zero.
*
*               determine the adjective mnemonic address for the adjective
*               specified for an Adjective PLU by using the adjective group
*               number specified for the PLU along with the implied adjective number
*               of this PLU.
*
*               This function is used to determine the adjective mnemonic for
*               the implied or default adjective rather than the actual or
*               specified adjective.
*===========================================================================
*/
UCHAR ItemSalesOEPGetAdjNoImplied(PLUIF *pPlu)
{
    UCHAR   uchAdj, uchGroup;

    /* --- check if this is an Adjective PLU --- */
    if ((pPlu->ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) != PLU_ADJ_TYP) {
        return (0);
    }

	// determine the Adjective Group for this PLU and the provisioned Implied Adjective Number
	// which is the default adjective number.
    uchGroup = (UCHAR)(pPlu->ParaPlu.ContPlu.auchContOther[PLU_CONTROLCODE_ADR_3] >> 4);
    uchAdj   = (UCHAR)(pPlu->ParaPlu.ContPlu.auchContOther[PLU_CONTROLCODE_ADR_4] & PLU_IMP_ADJKEY_MASK);

	if (uchAdj == PLU_COMP_ADJKEY) return 0;    // if no Implied Adjective then we can not figure out the mnemonic

	// determine the adjective mnemonic address using adjective group number
	// and the actual adjective number, 1 - 5, of the Adjective PLU.
    if (uchGroup & 0x01) {                  /* adjective group number 1 */
        ;
    } else if (uchGroup & 0x02) {           /* adjective group number 2 */
        uchAdj += 5;
    } else if (uchGroup & 0x04) {           /* adjective group number 3 */
        uchAdj += 10;
    } else {                                /* adjective group number 4 */
        uchAdj += 15;
    }
        
    return (uchAdj);
}

/*
*===========================================================================
** Synopsis:    VOID ItemSalesOEPClear2x20(USHORT usType);
*
** Return:      nothing
*===========================================================================
*/
VOID ItemSalesOEPClear2x20(USHORT usType)
{
	REGDISPMSG  Message = {0};

    flDispRegDescrControl |= flSlOepSaveDescr & (TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);
    flDispRegKeepControl  |= flSlOepSaveKeep  & (TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);

    Message.uchMajorClass  = CLASS_UIFREGMODIFIER;  /* dummy               */
    Message.uchMinorClass  = CLASS_UITABLENO;       /* dummy               */
    Message.lAmount        = 0L;                    /* dummy               */

	/* --- set leadthrough mnemonic after OEP R3.1 --- */
    if (usType) {
		RflGetLead (Message.aszMnemonic, LDT_NEXTENT_ADR);
	}

    Message.fbSaveControl  = 0;
                                                                
    DispWrite(&Message);

    if (!usType) {
        memset(&Message, 0, sizeof(REGDISPMSG));
        Message.uchMajorClass = CLASS_UIFREGOTHER;      /* set major class     */
        Message.uchMinorClass = CLASS_UICLEAR;          /* set minor class     */
        DispWrite(&Message);
    }

    flDispRegDescrControl &= ~(TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL);
    flDispRegKeepControl  &= ~(TAXMOD_CNTRL | FOODSTAMP_CTL | VOID_CNTRL); 
}


/*
*===========================================================================
** Synopsis:    VOID ItemSalesOEPSetNoOfPluKey(UCHAR uchFsc, USHORT *usPluNo);
*
** Return:      
*===========================================================================
*/
SHORT   ItemSalesOEPSetNoOfPluKey(UIFDIADATA *pUI, TCHAR *pauchPLUNo)
{
    UIFREGSALES     RegSales = {0};
	LABELANALYSIS   PluLabel = {0};
    SHORT   sReturnStatus, sOffset;
    
    RegSales.uchMajorFsc = pUI->auchFsc[0];
    RegSales.uchFsc = pUI->auchFsc[1];

    if (RegSales.uchMajorFsc == FSC_KEYED_PLU) {
        if ((sReturnStatus = ItemSalesCheckKeyedPLU(&RegSales)) != ITM_SUCCESS) {
            return(sReturnStatus);
        }
    } else {
        _tcscpy(RegSales.aszPLUNo, pUI->aszMnemonics);
    }
    
    /* ----- set plu number for label analysis ----- */
    _tcscpy(PluLabel.aszScanPlu, RegSales.aszPLUNo);

    /* ----- check E-version key ----- */
    if (RegSales.fbModifier2 & EVER_MODIFIER) {
        PluLabel.fchModifier |= LA_EMOD;
    }

    /* ----- check UPC Velocity key ----- */
    if (RegSales.fbModifier2 & VELO_MODIFIER) {
        PluLabel.fchModifier |= LA_UPC_VEL;
    }

    /* ----- PLU is entered by Scanner or Key ? ----- */
    if (RegSales.uchMajorFsc == FSC_SCANNER) {
        PluLabel.fchModifier |= LA_SCANNER;             /* by Scanner */
        sOffset = ItemCommonLabelPosi(RegSales.aszPLUNo);
        if (sOffset < 0) {
            return(LDT_KEYOVER_ADR);
        }
    }

    /* ----- analyze PLU number from UI ----- */
    if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
        ;
    } else {
        return(LDT_KEYOVER_ADR);
    }

    switch (PluLabel.uchType) {
    case    LABEL_RANDOM:
#if 0
        if (PluLabel.fsBitMap & LA_F_FREQ) {
            return LDT_PROHBT_ADR;

        } else {
#endif
        if (PluLabel.uchLookup & LA_EXE_LOOKUP) {
            /* allow only article no. at oep */
            if (PluLabel.fsBitMap & (LA_F_PRICE|LA_F_WEIGHT|LA_F_QUANTITY)) {
                return LDT_PROHBT_ADR;
            } else {
                _tcsncpy(pauchPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
            }
        } else {
            return LDT_PROHBT_ADR;
        }
#if 0
        }
#endif
        break;

    case    LABEL_UPCA:
    case    LABEL_UPCE:
    case    LABEL_EAN13:
    case    LABEL_EAN8:
    case    LABEL_VELOC:
        _tcsncpy(pauchPLUNo, PluLabel.aszLookPlu, NUM_PLU_LEN);
        break;
        
    case    LABEL_COUPON:
        /* not support UPC vender coupon */
        return (LDT_PROHBT_ADR);
        break;
        
        break;

    default:
        return(LDT_KEYOVER_ADR);
    }
    
    return(ITM_SUCCESS);

}
UCHAR  adjTable[] =   
{
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	3,
	3,
	3,
	3,
	3,
	4,
	4,
	4,
	4,
	4
};
/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPNextPlu(UIFREGSALES *pUifRegSales,
*                                         ITEMSALES *pItemSales,
*                                         UCHAR *pszGroups);
*     Input:    pUifRegSales - address of UIF data
*     InOut:    pItemSales   - address of sales data
*     Input:    pszGroups    - address of group number table
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*===========================================================================
*/
SHORT ItemSalesOEPAdjective(UIFREGSALES *pUifRegSales,
                                         ITEMSALES *pItemSales,
                                         UCHAR *pszGroups)
{
    SHORT               sResult;
	int                 nNoOrderPmt = 0;
    USHORT              ausOption[SALES_NUM_GROUP + 1], i, usNoOfOption;    /* OEP option save area */
    USHORT              usOption1 =0 , usOption2 = 0, usOption3 = 0;
    USHORT              usGroupPos, usNumMenu, usOrderNo, usContinue, fsType;
    USHORT              usNoOfGroup;
    UCHAR				uchNounAdj, uchChildAdj;
	UCHAR               uchAdjProhibitMask[] = { PLU_PROHIBIT_VAL1, PLU_PROHIBIT_VAL2, PLU_PROHIBIT_VAL3, PLU_PROHIBIT_VAL4, PLU_PROHIBIT_VAL5 };
	TCHAR               aszNounMnem[NUM_DEPTPLU + 1] = {0};
    DCURRENCY           lNounAmount;
    LONG                lCounter;
    UCHAR               uchCondStatus;                      /* for 97 option */
    ITEMSALESOEPLOCAL   *pMenu;
	MLDPOPORDER_VARY    *pDisp;
    UCHAR               auchGroupNo[SALES_NUM_GROUP + 1];     /* for 97 option */
    TCHAR               auchPluNo[NUM_PLU_LEN];
    UCHAR               fbModifier2;
	UCHAR               uchTableNumber;
	UCHAR				uchAdjGroup;
	USHORT usContinueLoop = 0;

    uchNounAdj = pItemSales->uchAdjective;
    _tcsncpy(aszNounMnem, pItemSales->aszMnemonic, NUM_DEPTPLU);

	pDisp = (MLDPOPORDER_VARY *) alloca (sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));
	NHPOS_ASSERT(pDisp);

	pMenu = (ITEMSALESOEPLOCAL *) alloca (sizeof(ITEMSALESOEPLOCAL) * (128));
	NHPOS_ASSERT(pMenu);

    /*
		We will build the series of pop up menus one group at a time.  The
		group information is provisioned through AC160 and contains a table
		of two kinds of values, either a PLU group number (0 - 90) or a 
		speciall code (91-99).

		If the group is a special type rather than a group number, we do 
		special processing.

		If the group is SLOEP_MULTI means allow the user to pick multiple
		times until they choose to finish.  Finish is indicated by 00 entered
		by keyboard or the Done button pressed.
		--- relocate group table for multiple item (TAR #129199) ---

		If the group is SLOEP_FREEONE, SLOEP_FREETWO, etc, then we let the
		user choose a given number of free items from the next group displayed
		(SLOEP_FREEONE is one item, etc.).

		If the group is SLOEP_SELFROMKEY then the next group number specifies the
		group of condiments which are legal.  However, a pop up window is not displayed.
		This means that the user must enter PLU numbers by hand using the keyboard or
		an existing window with those PLU numbers must already be displayed.
	*/

    memset(ausOption, 0x00, sizeof(ausOption));
    usNoOfOption=0;

	// Tell Framework what our groups are.
	BlFwIfSetOEPGroups(pszGroups, pItemSales->uchTableNumber);

        
	/* --- make menu data --- */
	memset (pDisp, 0, sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (128));

    pDisp->uchMajorClass = CLASS_POPUP;
    pDisp->uchMinorClass = CLASS_POPORDERPMT_ADJ;

    /* --- set leadthrough mnemonic according to option R3.1 --- */
	RflGetLead (pDisp->aszLeadThrough, LDT_NUM_ADR);

    /* --- set adjective mnemonic,  Jul/24 --- */
    if (uchNounAdj != 0) {
		RflGetAdj (pDisp->aszAdjective, uchNounAdj);
    }
    /* --- set noun plu mnemonic --- */
    _tcscpy(pDisp->aszPluName, aszNounMnem);

    /* --- display menu data --- */

    if (! ItemSalesLocalSaleStarted() && !(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)) {
        ItemCommonMldInit();
    }
    if (usOption3 != SLOEP_SELFROMKEY) { /* not create meue at 97 option R3.1 */
        ItemSalesLocal.fbSalesStatus |= SALES_OEP_POPUP;
    } else {
        ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
    }

    /* --- check retry counter by option R3.1 --- */

    usContinue = 0;                 /* single item       */


    if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
		VOSRECT     VosRect = {0};

		// Create a Popup window, only to determine the window button
		//	dimensions. This is used to find out how many OEP items
		//	should be populated in the OEP menu list (ItemSalesOEPMakeMenu).
		// We specify standard window size for an OEP window but the size
		//  in the Layout will override this size so we need to know what
		//  the actual size from the layout is.
		VosRect.usRow = 0;    /* start row */
		VosRect.usCol = 0;   /* start colum */
		VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length MLD_NOR_POP_ROW_LEN */
		VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */

		nNoOrderPmt = MldCreatePopUpRectWindowOnly(&VosRect);

		if (nNoOrderPmt < 1) {
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			return (ITM_SUCCESS);                       /* exit ...          */
		}
	}

    usOrderNo = 0;
    for (usContinueLoop = 1; usContinueLoop == 1; ) {
        /* --- order PLU item in menu list --- */

//        pDisp->lPrice = lNounAmount;  /* display price with condiment */

        /* V3.3 */
        if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
            /* --- make menu data for OEP scroll --- */
            if (usOrderNo == SLOEP_SCROLL_UP) {
                pDisp->uchMinorClass = CLASS_POPORDERPMT_UP;
            } else
            if (usOrderNo == SLOEP_SCROLL_DOWN) {
                pDisp->uchMinorClass = CLASS_POPORDERPMT_DOWN;
            } else {
                pDisp->uchMinorClass = CLASS_POPORDERPMT; 
                sSlOepCounterIndex = 0;
            }
            memset(pDisp->OrderPmt, 0, sizeof(ORDERPMT) * (128));  /* V3.3 */
            sResult = ItemSalesAdjMakeMenu(0,
                                       uchNounAdj,
                                       pMenu,
                                       pDisp, nNoOrderPmt,
                                       &usNumMenu,
                                       &aculSlOepCounterSave[sSlOepCounterIndex],
			                           &aculSlOepCounterSave[sSlOepCounterIndex+1]);
			for(i = 0; i < STD_ADJNUM_GROUP; i++){
				PLUIF    PLUBuff = { 0 };        /* PLU record buffer */

				_tcsncpy(PLUBuff.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
				PLUBuff.uchPluAdj = i+1;
				CliOpPluIndRead(&PLUBuff, 0);
				if((PLUBuff.ParaPlu.ContPlu.auchContOther[4] & uchAdjProhibitMask[i]) != 0) {
					// this adjective variation is prohibitted for this PLU so we will not display it.
					memset(&pDisp->OrderPmt[i], 0x00, sizeof(ORDERPMT));
				}
			}
			if (sResult == ITM_SUCCESS) {
				pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_DONE;
				pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_BACK;
				if (sSlOepCounterIndex > 0) {
					pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_BACK;
				}
                if (usNumMenu > 0) {
					MldUpdatePopUp(pDisp);
				}
				else {
					BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
					usContinueLoop = 2;
					break;
				}
			}
			else {
				if (usOrderNo == SLOEP_SCROLL_UP) {
					if (pDisp->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
						sSlOepCounterIndex++;
					}
				} else if (usOrderNo == SLOEP_SCROLL_DOWN) {
					if (sSlOepCounterIndex > 0)
						sSlOepCounterIndex--;
				}
			}
        }

		// Obtain the user's menu selection.  This function is called regardless
		// of whether we have displayed a popup menu or not (if group number is 97,
		// we prompt for a condiment which we will check later and no popup is displayed).
		// If there is an error, then we will break out of the loop and return an error
		// indication that is in sResult.
		    pDisp->uchMinorClass = CLASS_POPORDERPMT_ADJ;

        sResult = ItemSalesOEPOrderPlu(&usOrderNo, &uchChildAdj, pDisp,
                                       usOption1, usOption2, usOption3, &auchPluNo[0], &fbModifier2);

		if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
			MldDeletePopUp();  // Delete the popup managed within this loop
			ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
		}

		uchAdjGroup = adjTable[uchNounAdj - 1];
		switch(uchAdjGroup)
		{
		case 1:
			uchChildAdj = 0 + (uchChildAdj);
			break;
		case 2:
			uchChildAdj = 6 + (uchChildAdj - 1);
			break;
		case 3:
			uchChildAdj = 11 + (uchChildAdj - 1);
			break;
		case 4:
			uchChildAdj = 16 + (uchChildAdj - 1);
			break;
		default:
			return -1;
		}

		return uchChildAdj;

        if (sResult != ITM_SUCCESS) {
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			usContinueLoop = 2;
			break;
        }

        /* --- check order number --- */

        if ((usOrderNo == SLOEP_SCROLL_UP) || (usOrderNo == SLOEP_SCROLL_DOWN)) {  /* scroll up/down */
            continue;
        }

        if (usOrderNo == 0) {               /* skip by user      */
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_INCREMENT, 0);

            ItemSalesOEPClear2x20(1);       /* display leadthrough */
            break;                          /* exit loop         */
        }

        /* --- set plu no at 97 option R3.1 --- */

		// if the usOption is greater than 90 there is no need to 
		//	increment the counter, the OEP window does not need 
		//	to be changed - CSMALL 1/26/06
		if((usOption1 < SLOEP_BEGIN_SPL) &&
			(usOption2 < SLOEP_BEGIN_SPL) &&
			(usOption3 < SLOEP_BEGIN_SPL))
		{
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_INCREMENT, 0);
		}

        if (usOption3 == SLOEP_SELFROMKEY) {
            _tcsncpy(pMenu[0].auchPluNo, auchPluNo, NUM_PLU_LEN);
            pMenu[0].uchAdj = uchChildAdj;
            usOrderNo = 0;  /* V3.3 */
        } else {
            usNoOfGroup = 1;    /* for insert control */
        }

        /* V3.3 */
        if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
			TCHAR   auchDummy[NUM_PLU_LEN] = {0};

            for (i=(nNoOrderPmt-1); i>0; i--) {
                if (pDisp->OrderPmt[i].uchOrder) break;
            }
            if (((UCHAR)usOrderNo >= pDisp->OrderPmt[0].uchOrder) && ((UCHAR)usOrderNo <= pDisp->OrderPmt[i].uchOrder)) {
                /* entered order no. exists on menu */
                ;
            } else {
                /* --- remake PLU data class --- */
                lCounter = 0L;
                for (i=SLOEP_ENTRY_NO; i <= usOrderNo; i+=nNoOrderPmt ){
                    /* remake "Menu" data, if usOrderNo is out of scroll display */
                    memset(pMenu, 0, sizeof(ITEMSALESOEPLOCAL) * (128));
                    sResult = ItemSalesOEPMakeMenu(pszGroups[usGroupPos],
                                               uchNounAdj,
                                               pMenu,
                                               pDisp, nNoOrderPmt,
                                               &usNumMenu,
                                               &lCounter,
                                               &lCounter);
                }
            }

            /* check entry data is valid or not */
            usOrderNo -= SLOEP_ENTRY_NO;
            while(usOrderNo >= nNoOrderPmt)
			{
				usOrderNo-= nNoOrderPmt;
			}
            if (_tcsncmp(pMenu[usOrderNo].auchPluNo, auchDummy, NUM_PLU_LEN) == 0) { /* 2172 */
                UieErrorMsg(LDT_NTINFL_ADR, UIE_WITHOUT);

				// set indicator to 102 because of input error - CSMALL 2/7/06
				BlFwIfSetOEPGroups(BL_OEPGROUP_STR_DECREMENT, 0);

                continue;
            }
        }

        sResult = ItemSalesOEPMakeNewClass(pUifRegSales,
                                           &pMenu[usOrderNo],
                                           &uchNounAdj,
                                           &aszNounMnem[0],
                                           &lNounAmount,
                                           &uchCondStatus,
                                           usOption1, usOption2, usOption3,
                                           &auchGroupNo[0],
                                           fbModifier2,
                                           &uchTableNumber);
		if(uchTableNumber)
		{
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
		}
        if (sResult == UIF_CANCEL) {        /* cancel spv. int.  */
            continue;                       /* retry ...         */
        } else if (sResult == ITM_CONT) {   /* 97 option error */
			
			// fix for SR 867 - prevents moving forward in sequence
			//	of OEP Groups when an error is incountered.  CSMALL 1/31/06
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_DECREMENT, 0);

            UieErrorMsg(LDT_PROHBT_ADR, UIE_WITHOUT);
            continue;
        } else if (sResult != ITM_SUCCESS) {
            usContinueLoop = 2;
			break;
        }

        /* --- check free option --- */
        if (usOption2) {

            /* change free option for multiple selection  */

            for (i=0; i<usNoOfOption; i++) {
                if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREEONE) {
                    pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_DUMMY;
                    break;
                }
                if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREETWO) {
                    pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREEONE;
                    break;
                }
                if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREETHREE) {
                    pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREETWO;
                    break;
                }
                if (pszGroups[usGroupPos-(usNoOfOption-i)] == SLOEP_FREEFOUR) {
                    pszGroups[usGroupPos-(usNoOfOption-i)] = SLOEP_FREETHREE;
                    break;
                }
            }

            /* decrement loop conter */

            usContinue--;
            if (!usContinue) {                  /* after free sales */
                if (usOption1 == SLOEP_MULTI) { /* if multiple sales */
                    usOption2 = 0;              /* not free sales */
                    usContinue++;
                }
            }
        }

        /* --- update group number of OEP --- */

        sResult = ItemSalesOEPUpdateGroupNo(uchCondStatus, /* 2172 */
                                            pszGroups,
                                            usGroupPos,
                                            &fsType,
                                            usContinue,
                                            usNoOfGroup,
											uchTableNumber);
        if (sResult != ITM_SUCCESS && sResult != ITM_CANCEL) {
            usContinueLoop = 2;
			break;
        }

        /* --- inserted new group number ? --- */

        if (fsType & SLOEP_F_INSERT) {
            if (fsType & SLOEP_F_MULTI) {   /* multiple item     */
                usGroupPos -= (usNoOfOption+usNoOfGroup);
            }
            break;                          /* exit loop         */
        }

        /* --- check loop function  --- */

        if (!usContinue) {                  /* single item       */
            break;                          /* exit loop         */
        }
    }

	if (ItemSalesLocal.fbSalesStatus & SALES_OEP_POPUP) {
		MldDeletePopUp();  // Delete the popup managed within this loop
		ItemSalesLocal.fbSalesStatus &= ~SALES_OEP_POPUP;
	}
	if (usContinueLoop == 2) {
		return (sResult);               /* exit ...          */
	}

    memset(ausOption, 0x00, sizeof(ausOption));
    usNoOfOption=0;
	//SR 609, reset the index back to 0, because we are starting on a new group JHHJ
	sSlOepCounterIndex = 0;
  
    if (usOption3 == SLOEP_SELFROMKEY) {    /* for "00" exit case */
    }
	BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
    
	return (ITM_SUCCESS);                       /* exit ...          */
}
/*
*===========================================================================
** Synopsis:    SHORT ItemSalesOEPMakeMenu(UCHAR uchGroupNo,
*                                          UCHAR uchNounAdj,
*                                          ITEMSALESOEPLOCAL *pItem,
*                                          MLDPOPORDER *pDisp,
*                                          USHORT *pusNumItem);
*     Input:    uchGroupNo - group number
*               uchNounAdj - adjective number of noun
*    Output:    pMenu      - address of item data table
*               pDisp      - address of display data
*               pusNumItem - address of number of item
*
** Return:      ITM_SUCCESS - successful
*               other       - error
*
** Description:  There are a total of MAX_ADJM_NO adjective mnemonics in the memory
*                resident database in Para.ParaAdjMnemo[] which are divided into
*                into groups of STD_ADJNUM_GROUP mnemonics for each type of adjective.
*                In other words we may have a type of adjective for Adjective Group #1,
*                say drink size, which has several variations: SM (small), MD (medium),
*                LG (large). This Adjective Group is then used with the various drink PLUs
*                which have drink sizes in order to print the drink size with the PLU mnemonic
*                on the customer receipt.
*
*                Currently, Feb-2020, there are 20 total adjective mnemonics which are
*                divided up into 4 groups with each group having 5 mnemonics. Each
*                Adjective PLU specifies which Adjective Group it is using as well as
*                which, if any, Adjective Variations are Prohibitted along with the
*                pricing for each Adjective Variation.
*===========================================================================
*/
STATIC SHORT ItemSalesAdjMakeMenu(UCHAR uchGroupNo, UCHAR uchNounAdj,
                           ITEMSALESOEPLOCAL *pMenu, MLDPOPORDER_VARY *pDisp,
                           int nNoOrderPmt, USHORT *pusNumItem,
                           LONG *pculCounterFirst, LONG *pculCounterLast)
{
    USHORT              usOff, i, usMax;
    
	NHPOS_ASSERT(uchNounAdj > 0);

    /* --- initialization --- */
	if (uchNounAdj > 0) uchNounAdj--;
	usOff = 1;
	switch(adjTable[uchNounAdj])
	{
	case 4:             // Adjective Group #4 mnemonics starting address
		usOff += STD_ADJNUM_GROUP;
		// fall through to allow the correct offset to be calculated
	case 3:             // Adjective Group #3 mnemonics starting address
		usOff += STD_ADJNUM_GROUP;
		// fall through to allow the correct offset to be calculated
	case 2:             // Adjective Group #2 mnemonics starting address
		usOff += STD_ADJNUM_GROUP;
		// fall through to allow the correct offset to be calculated
	case 1:             // Adjective Group #1 mnemonics starting address
		break;
	default:
		return -1;
	}

	usMax = usOff + STD_ADJNUM_GROUP;
	for(i = 0; usOff < usMax; usOff++, i++)
	{
		RflGetAdj(pDisp->OrderPmt[i].aszPluName, usOff);
		pDisp->OrderPmt[i].uchOrder = (i + 1);
	}
		
	return ITM_SUCCESS;
}
/* ==================================== */
/* ========== End of SLOEP.C ========== */
/* ==================================== */
