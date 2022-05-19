/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Cursor void module                          
* Category    : Item Module, NCR 2170 Hsopitality US Model Application         
* Program Name: SLVOID.C
* --------------------------------------------------------------------------
* Abstruct: ItemSalesCursorVoid() : Execute Cursor Void for Sales Item
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* Nov-10-95: 03.01.00  : M.Ozawa   : initial                                   
* Nov-8-99 :           : M.Ozawa   : move ItemSalesPriceCheck() to slpcheck.c
*          :           :           :                         
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
#include    <malloc.h>
#include    <string.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <uie.h>
#include    <pif.h>
#include    <log.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <item.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <display.h>
#include    <appllog.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <cpm.h>
#include    <eept.h>
#include    <mld.h>
#include    "itmlocal.h"
#include    "fsc.h"
#include	"rfl.h"	
#include	<bl.h>	

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCursorVoid(ITEMDISC *pData);
*
*    Input: ITEMDISC *pData
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  Cursor Void of Sales.                   R3.1
*===========================================================================
*/
static SHORT   ItemSalesCursorVoidReturnsVoid (ITEMSALES *pItemSales, SHORT sQTY)
{
    SHORT   sStatus;
    UCHAR  uchItemStatus;

	/*----- recalculate price at void search -----*/
    if ((pItemSales->uchPPICode) && (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG)) {
        if (sQTY == 0) {
            pItemSales->lProduct *= -1L;    /* use changed price */ 
            if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
                pItemSales->lProduct *= -1L;                  /* turn over sign of ppi */
            }
        } else {
            ItemSalesCalculation(pItemSales);
        }
    } else if ((pItemSales->uchPM) && (pItemSales->fsLabelStatus & ITM_CONTROL_PCHG)) {
        if (sQTY == 0) {
            pItemSales->lProduct = RflLLabs(pItemSales->lProduct);   /* use changed price */ 
        } else {
            ItemSalesCalculation(pItemSales);
        }
    } else if( pItemSales->usFor != 0 && pItemSales->usForQty !=0 )
	{
		//SR 143 @/For Key cwunn 2-2-04 SR 214 Fix cursor void JHHJ
		//Capture @/For operations and trigger ItemSalesCalculation to properly void @/For events
		pItemSales->lProduct = ((pItemSales->usFor / pItemSales->usForQty) * pItemSales->sCouponQTY);
		//ItemSalesCalculation(pItemSales);
	}
	else
	{
		ItemSalesCalculation(pItemSales);
    }

    pItemSales->lQTY *= -1L;
    pItemSales->sCouponQTY *= -1L;
    /* --- Saratoga,    Nov/27/2000 --- */
    if (pItemSales->uchPM && pItemSales->uchPPICode == 0) {
        pItemSales->lProduct *= -1L;
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            pItemSales->lProduct *= -1L;
        }
    } else if (pItemSales->uchPPICode == 0) {  /* not turn over sign at ppi */
        pItemSales->lProduct *= -1L;
    } else {
        if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) { /* preselect void trans. */
            pItemSales->lProduct *= -1L;                  /* turn over sign of ppi */
        }
    }
    pItemSales->lDiscountAmount *= -1L;

	sStatus = ItemSalesRemoveGiftCard(pItemSales);
	if (sStatus != ITM_SUCCESS) {
		return sStatus;
	}

	{
		UIFREGSALES UifRegSales = {0};
		REGDISPMSG  DisplayData = {0};

		/* prohibit condiment/print modifier entry, 10/12/98 */
		uchDispRepeatCo = 0;                    /* init repeat counter */
		/* uchDispRepeatCo = 1;                    / init repeat counter */

		sStatus = ItemSalesSalesAmtCalc(pItemSales, &DisplayData.lAmount);
		if (sStatus != ITM_SUCCESS) {
			/* calculate sales amount regardress sign */
			return(sStatus);
		}

		UifRegSales.uchMajorClass = CLASS_UIFREGSALES;
		UifRegSales.uchMinorClass = CLASS_UIDEPT;

		if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
			(pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
			(pItemSales->uchMinorClass == CLASS_OEPMODDISC) ||     /* R3.0 */
			(pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

			UifRegSales.uchMinorClass = CLASS_UIMODDISC;
			DisplayData.lAmount = pItemSales->lDiscountAmount;
		}

		ItemSalesDisplay(&UifRegSales, pItemSales, &DisplayData);

		//Preserve writing when US Customs #/type entry is off cwunn 5/5/03
	//	if(!CliParaMDCCheck(MDC_USCUSTOMS_ADR, ODD_MDC_BIT2)) {
			MldScrollWrite(pItemSales, MLD_NEW_ITEMIZE);
			MldScrollFileWrite(pItemSales);
	//	}
	}

    pItemSales->fsPrintStatus |= PRT_JOURNAL;  /* turn on journal bit */
	if (pItemSales->fbModifier & (RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3))
		pItemSales->fsPrintStatus |= PRT_RECEIPT;  /* turn on receipt bit if this is a return */

    /* send to enhanced kds, 2172 */
	pItemSales->usKdsLineNo = ItemSendKdsItemSetLineNumber(pItemSales);
    if (ItemSendKds(pItemSales, 0) != ITM_SUCCESS) {
		NHPOS_ASSERT_TEXT(0, "**ERROR: ItemSendKds() failed in ItemSalesCursorVoidReturnsVoid().  Continuing.");
    }
    
	// Update the running total with the voided price
	pItemSales->fbStorageStatus |= IS_CONDIMENT_EDIT;
	ItemCommonSubTotalVoid (pItemSales);
    ItemCommonDispSubTotal(pItemSales);
	pItemSales->fbStorageStatus &= ~IS_CONDIMENT_EDIT;

    /*----- VALIDATION PRINT FOR MINUS ITEM -----*/
    uchItemStatus = 0;                      /* clear work status */

    if ((pItemSales->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItemSales->uchMinorClass == CLASS_OEPMODDISC) ||     /* R3.0 */
        (pItemSales->uchMinorClass == CLASS_SETMODDISC)) {

        uchItemStatus &= ~COMP_VAL;         /* not print val. for minus item */
    } else if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
        uchItemStatus &= ~COMP_VAL;         /* not print val. for condiment */
    } else {
        /* 9/21/93 if (pItemSales->ControlCode.auchStatus[0] & PLU_MINUS) { */
            if (pItemSales->ControlCode.auchPluStatus[0] & PLU_PRT_VAL) {
                uchItemStatus = COMP_VAL;
            } else if ((pItemSales->fbModifier & VOID_MODIFIER) && (CliParaMDCCheck(MDC_VOID_ADR, ODD_MDC_BIT1))) { 
                if (!(pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS)) {   /* comp. validation for void */
                    uchItemStatus = COMP_VAL;
                }
            }
        /* 9/21/93 } */
    }
    if (uchItemStatus == COMP_VAL) {        /* req. validation */
		USHORT  usBuffer;

        usBuffer = pItemSales->fsPrintStatus;       /* store print status */
        pItemSales->fsPrintStatus = PRT_VALIDATION;
        TrnThrough(pItemSales);                    /* print validation (VOID function) */
    
        pItemSales->fsPrintStatus = (usBuffer & ~PRT_VALIDATION);  /* restore print status */
    }

    /* ----- send link plu to kds, 2172 ----- */
    ItemSalesLinkPLU(pItemSales, 3);

    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: SHORT   ItemSalesCursorVoidReturns(ITEMSALES *pItemSales, SHORT sQTY)
*
*    Input: ITEMSALES  *pItemSales     - the item sales data for the item to Void/Return
*           SHORT      sQTY            - quantity of the item
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  if there is a Reason Code Mnemonics data then we will display the list to the
*                Operator to allow them to choose a reason code for this return. The Operator
*                can then chose a Reason Code or they can cancel out of the action.
*
*                If there are no Reason Code Mnemonics then we will just go ahead and process
*                the Void or Return.
*===========================================================================
*/
SHORT   ItemSalesCursorVoidReturns(ITEMSALES *pItemSales, SHORT sQTY)
{
	{
		PARAREASONCODEINFO  DataReasonCode = {0};

		DataReasonCode.uchAddress = pItemSales->usReasonCode;
		ParaReasonCodeInfoParaRead (&DataReasonCode);

		// if there is a Reason Code Mnemonics data then we will display the list to the Operator to
		// allow them to choose a reason code for this return. If there are no Reason Code Mnemonics
		// then we will just go ahead and process the Void or Return.
		if (DataReasonCode.uchAddress > 0 && DataReasonCode.usMnemonicStartAddr > 0 && DataReasonCode.usMnemonicCount > 0)
		{
			USHORT  usNonemptyCount = 0;
			USHORT  usI;
			OPMNEMONICFILE MnemonicFile = {0};
			TCHAR  *pMnemonics[50];
			TCHAR  Mnemonics[50*32] = {0};
			TCHAR   *pTemp = Mnemonics;

			MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
			MnemonicFile.usMnemonicAddress = DataReasonCode.usMnemonicStartAddr;
			for (usI = 0, pTemp = Mnemonics; usI < DataReasonCode.usMnemonicCount && usI < 50; usI++) {
				OpMnemonicsFileIndRead(&MnemonicFile, 0);
				_tcsncpy (pTemp, MnemonicFile.aszMnemonicValue, 30);
				pMnemonics[usI] = pTemp;
				if (pTemp[0]) usNonemptyCount++;
				pTemp += 32;
				MnemonicFile.usMnemonicAddress++;
			}

			pItemSales->usReasonCode = 0;
			if (usNonemptyCount) {
				TCHAR  *pLeadThru = _T("Reason");
				SHORT  sRetStatus = 0;

				sRetStatus = ItemDisplayOepChooseDialog (pLeadThru, pMnemonics, DataReasonCode.usMnemonicCount);
				if (sRetStatus > 0) {
					pItemSales->usReasonCode = (sRetStatus - 1) + DataReasonCode.usMnemonicStartAddr;
				} else {
					return LDT_KEYOVER_ADR;
				}
			}
		}
	}

	return ItemSalesCursorVoidReturnsVoid (pItemSales, sQTY);
}


SHORT   ItemSalesCursorVoid(ITEMSALES *pItemSales, SHORT sQTY)
{
    SHORT   sStatus;

    /*----- SUPERVISOR INTERVENTION -----*/
    if ((sStatus = ItemSalesSpvInt(pItemSales)) != ITM_SUCCESS) {
		// Supervisor intervention did not work so do not use the void modifier.
        return(sStatus);
    }

	pItemSales->fbModifier ^= VOID_MODIFIER;   // toggle the void modifier

	return ItemSalesCursorVoidReturnsVoid (pItemSales, sQTY);
}

SHORT   ItemSTenderCursorVoidReturns(ITEMTENDER *pItemTender)
{
#if 0
    SHORT   sStatus;
    UCHAR  uchItemStatus;
    UIFREGSALES UifRegSales;
    REGDISPMSG  DisplayData;
    TRANCURQUAL    CurQualRcvBuff;
	TRANGCFQUAL    *pWorkGCF;

	// Antrak has requested that when a charge tip is being returned that the OEP window not be
	// displayed.  They just want it to be marked as returned and that is all.
	//   Richard Chambers, Nov 27, 2012.
	{
		PARAREASONCODEINFO  DataReasonCode;
		SHORT  sItemNumber = 4, sRetStatus = 0;
		TCHAR  *pLeadThru = _T("Reason");
		TCHAR  *pMnemonics[50];
		TCHAR  Mnemonics[50*32];

		memset (&DataReasonCode, 0, sizeof(DataReasonCode));
		DataReasonCode.uchAddress = pItemTender->usReasonCode;
		ParaReasonCodeInfoParaRead (&DataReasonCode);

		if (DataReasonCode.uchAddress > 0 && DataReasonCode.usMnemonicStartAddr > 0 && DataReasonCode.usMnemonicCount > 0)
		{
			USHORT  usI;
			TCHAR   *pTemp;
			OPMNEMONICFILE MnemonicFile;

			MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
			MnemonicFile.usMnemonicAddress = DataReasonCode.usMnemonicStartAddr;
			memset (Mnemonics, 0, sizeof(Mnemonics));
			pTemp = Mnemonics;
			for (usI = 0; usI < DataReasonCode.usMnemonicCount; usI++) {
				OpMnemonicsFileIndRead(&MnemonicFile, 0);
				_tcsncpy (pTemp, MnemonicFile.aszMnemonicValue, 30);
				pMnemonics[usI] = pTemp;
				pTemp += 32;
				MnemonicFile.usMnemonicAddress++;
			}

			sRetStatus = ItemDisplayOepChooseDialog (pLeadThru, pMnemonics, DataReasonCode.usMnemonicCount);
			if (sRetStatus > 0) {
				pItemTender->usReasonCode = (sRetStatus - 1) + DataReasonCode.usMnemonicStartAddr;
			} else {
				pItemTender->usReasonCode = 0;
				return LDT_KEYOVER_ADR;
			}
		}
	}
#endif

	TrnSalesAdjustItemizers ((ITEMGENERICHEADER *)pItemTender);
	ItemCommonDispSubTotal(pItemTender);

	MldScrollWrite(pItemTender, MLD_NEW_ITEMIZE);
	MldScrollFileWrite(pItemTender);

	return(ITM_SUCCESS);
}

/*
*===========================================================================
**Synopsis: VOID	ItemSalesRemoveGiftCard(ITEMSALES *pItemSales)
*
*    Input: TRANGCFQUAL *WorkGCF,ITEMSALES *pItemSales
*
*   Output: None
*
*    InOut: None
*
**Return: 
*
** Description:  This function will remove a gift card when a cursor void is executed
**				on gift card item. Gift Card Items such as issue, void issue, reload
**				and void reload.
*===========================================================================
*/
SHORT	ItemSalesRemoveGiftCard(CONST ITEMSALES *pItemSales)
{
	SHORT           error;

	// check to make sure that the current item is a gift card item
	if((pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_GIFT_CARD) || 
		(pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_7] & PLU_EPAYMENT))
	{
		TRANGCFQUAL    *WorkGCF = TrnGetGCFQualPtr();
		SHORT          currentIndex = 0;
		USHORT         usNo = 1;

		// Loop through all gift card entries to see if any of the unique ID's match
		// the unique ID of the item in pItemSales.
		for(currentIndex = 0; currentIndex <= NUM_SEAT_CARD; currentIndex++)
		{
			//if the Gift Card is already Issued/Reloaded then we do a void on what was done earlier.
			if(pItemSales->usUniqueID == WorkGCF->TrnGiftCard[currentIndex].usUniqueID) {
				ITEMTENDER      ItemTender;
				UIFREGTENDER	UifRegTender;

				if(WorkGCF->TrnGiftCard[currentIndex].Used == GIFT_CARD_USED){
					ItemTender = WorkGCF->TrnGiftCard[currentIndex].ItemTender;
					UifRegTender.GiftCard = WorkGCF->TrnGiftCard[currentIndex].GiftCard;
					ItemTender.uchMajorClass = CLASS_ITEMTENDER;
					if(WorkGCF->TrnGiftCard[currentIndex].GiftCardOperation == GIFT_CARD_ISSUE){
						UifRegTender.GiftCardOperation = GIFT_CARD_VOID;
					}
					else if(WorkGCF->TrnGiftCard[currentIndex].GiftCardOperation == GIFT_CARD_RELOAD){
						UifRegTender.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
					}
					else if(WorkGCF->TrnGiftCard[currentIndex].GiftCardOperation == GIFT_CARD_VOID){
						UifRegTender.GiftCardOperation = GIFT_CARD_ISSUE;
					}
					else if (WorkGCF->TrnGiftCard[currentIndex].GiftCardOperation == GIFT_CARD_VOID_RELOAD){
						UifRegTender.GiftCardOperation = GIFT_CARD_RELOAD;
					}
					error = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);

					if(error != ITM_SUCCESS)
						return error;
				}
			}

			if(WorkGCF->TrnGiftCard[currentIndex].usUniqueID == pItemSales->usUniqueID)
			{	// if the unique IDs match then remove the gift card entry from the array
				// of gift card and then run through the remaining gift cards and shift them
				// up the array of gift cards one index.
				memset(&WorkGCF->TrnGiftCard[currentIndex], 0x00, sizeof(ITEMSALESGIFTCARD));
				for( ; currentIndex < NUM_SEAT_CARD; currentIndex++)
				{	// if the the next gift card has no information then remove the current gift
					// card and break out of the loop.
					if(WorkGCF->TrnGiftCard[currentIndex + 1].aszNumber[0] == 0)
					{
						memset(&WorkGCF->TrnGiftCard[currentIndex], 0x00, sizeof(ITEMSALESGIFTCARD));
						break;
					}
					WorkGCF->TrnGiftCard[currentIndex] = WorkGCF->TrnGiftCard[currentIndex + 1];
					memset(&WorkGCF->TrnGiftCard[currentIndex + 1], 0x00, sizeof(ITEMSALESGIFTCARD));
				}
				break;
			}
		}
	}
	return ITM_SUCCESS;
}

SHORT  ItemDisplayOepChooseDialog (CONST TCHAR *pLeadThruMnemonic, TCHAR **pMnemonicsList, SHORT sReasonCount)
{
	UIFDIADATA        UI;
	MLDPOPORDER_VARY  *pDisp;

	if (pLeadThruMnemonic == 0 || pMnemonicsList == 0 || sReasonCount < 1) {
		return 0;
	}

	pDisp = (MLDPOPORDER_VARY *) alloca (sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (48));
	NHPOS_ASSERT(pDisp);
	if (! pDisp) return 0;   // return doing nothing if alloca() fails.

	memset (pDisp, 0, sizeof(MLDPOPORDER_VARY) + sizeof(ORDERPMT) * (48));
	pDisp->uchMajorClass = CLASS_POPUP;
	pDisp->uchMinorClass = CLASS_POPORDERPMT;

	_tcsncpy(pDisp->aszLeadThrough, pLeadThruMnemonic, PARA_LEADTHRU_LEN);

	// Now we popup the OEP window with the list of coupons.
	// Then we let the operator pick.  If the operator cancels
	// out, then we pop down the OEP window and return.
	// Other wise we will then process what ever the operator selected.
	// We specify standard window size for an OEP window but the size
	//  in the Layout will override this size so we need to know what
	//  the actual size from the layout is.
	{
		int      i = 0;
		int      nScrollPage = 0, nDisplayedItems = 0;
		USHORT   usItemLoop;
		VOSRECT  VosRect = {0};

		// Create our popup window with standard sizes.
		// we must use BlFwIfSetOEPGroups() to set the group before calling
		// the function MldCreatePopUpRectWindowOnly() in order to prevent
		// OEP window close issues the first time the OEP window is used.
		BlFwIfSetOEPGroups(BL_OEPGROUP_STR_REASONCODE, 0);  // general purpose selection OEP window #202 for reason code
		VosRect.usRow = 0;    /* start row */
		VosRect.usCol = 0;   /* start colum */
		VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length MLD_NOR_POP_ROW_LEN */
		VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */
		nDisplayedItems = MldCreatePopUpRectWindowOnly(&VosRect);

		if (nDisplayedItems < 1)
		{
			MldDeletePopUp();
			ItemOtherClear();                   /* clear descriptor  */
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			return (0);                /* exit ...          */
		}

		// Now we begin to loop through the various pages of coupons to be displayed
		// refreshing the popup window with each screenfull.  If there are more items
		// to be displayed that will fit in this window, then we need to provide a
		// continue button so the operator can go to the next page.  The MLD system
		// will automatically provide a back button for us.
		while (1)
		{
			int j;
			for (j = i = 0, usItemLoop = nScrollPage * nDisplayedItems;
					usItemLoop < nScrollPage * nDisplayedItems + nDisplayedItems;
					usItemLoop++, i++)
			{
				if (usItemLoop >= sReasonCount)
					break;
				if (pMnemonicsList[usItemLoop][0]) {
					_tcscpy (pDisp->OrderPmt[j].aszPluName, pMnemonicsList[usItemLoop]);
					pDisp->OrderPmt[j].uchOrder = usItemLoop+1;
					j++;
				}
			}

			pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_DONE;
			pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_MORE;
			if (usItemLoop < sReasonCount) {
				pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_MORE;
			}

			pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_BACK;
			if (nScrollPage > 0) {
				pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_BACK;
			}

			// Fill in the popup window and then let the operator make a choice
			MldUpdatePopUp(pDisp);
			if (UifDiaOEP(&UI, pDisp) != UIF_SUCCESS)
			{    /* abort by user     */
				UI.ulData = UIF_CANCEL;
				break;                /* exit ...          */
			}

			// If the operator choses the Continue button to go to the
			// next page, FSC_SCROLL_DOWN, then we pull the next set of coupons
			// from the pDisp array filled in above.
			// If the operator choses the Back button to go to the previous
			// page, FSC_SCROLL_UP, then we will back up to the previous set
			// of coupons from the pDisp array filled in above.
			if (UI.auchFsc[0] == FSC_SCROLL_DOWN)
			{
				if (pDisp->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
					nScrollPage++;
				}
				if (nScrollPage < 1) {
					nScrollPage = 0;
				}
			}
			else if (UI.auchFsc[0] == FSC_SCROLL_UP)
			{
				nScrollPage--;
				if (nScrollPage < 1)
					nScrollPage = 0;
			}
			else if (UI.auchFsc[0] == FSC_CLEAR || UI.auchFsc[0] == FSC_ERROR) {
				UI.ulData = 0;
				break;
			}
			else
			{
				break;
			}
		}

		MldDeletePopUp();
		ItemOtherClear();                   /* clear descriptor  */
		BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
	}

	return (SHORT)UI.ulData;
}
/**** End of File ****/
