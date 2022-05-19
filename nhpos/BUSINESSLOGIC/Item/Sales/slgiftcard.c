/*
*===========================================================================
* Title       : Gift Card Sales Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: SLGIFTCARD.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*				
*               
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
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
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "fsc.h"
#include "paraequ.h"
#include "para.h"
#include "regstrct.h"
#include "item.h"
#include "display.h"
#include "uireg.h"
//SR 143 cwunn @/For & HALO Override
#include "transact.h"
#include "trans.h"
#include "..\item\include\itmlocal.h"
#include "rfl.h"
//SR 143 @/FOR Cancel Mechanism
#include	"..\Sa\UIE\uieseq.h" 
#include <mld.h>
#include <csstbpar.h>
#include    "..\Print\IncludeTherm\prrcolm_.h"
#include "..\..\UifReg\uiregloc.h"
#include "pifmain.h"
#include "BlFWif.h"
#include <csstbgcf.h>
#include <ConnEngineObjectIf.h>


/*
*===========================================================================
** Synopsis:    SHORT   ItemSalesReferenceEntry(ITEMTENDER *pItem)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Get Reference Number
*===========================================================================
*/
static SHORT   ItemLinePromptEntry (TCHAR *tcsPrompt, TCHAR *tcsBuffer, int iBufLen)
{
	SHORT           sReturn;
    UIFDIADATA      Dia = {0};

	if (iBufLen < 1) {
		NHPOS_ASSERT(iBufLen > 0);
        return(UIF_CANCEL);
	}

	if (tcsPrompt && tcsPrompt[0]) {
		REGDISPMSG      Disp = {0};

		Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
		Disp.uchMinorClass = CLASS_REGDISP_CPAN;
		_tcsncpy(Disp.aszMnemonic, tcsPrompt, PARA_LEADTHRU_LEN);

		flDispRegDescrControl |= COMPLSRY_CNTRL;
		Disp.fbSaveControl = DISP_SAVE_TYPE_4;
		DispWrite(&Disp);
		flDispRegDescrControl &= ~COMPLSRY_CNTRL;
	}

	//Gets Ref. No.
    sReturn = UifDiaCP6(&Dia, (UCHAR)iBufLen, (UIFREG_ALFANUM_INPUT | UIFREG_FLAG_MACROPAUSE));

	tcsBuffer[0] = 0;
   if (sReturn == UIF_SUCCESS) {
        if (Dia.auchFsc[0] == FSC_NUM_TYPE || Dia.auchFsc[0] == FSC_PRICE_ENTER) {
			_tcsncpy(tcsBuffer, Dia.aszMnemonics, iBufLen);
			tcsBuffer[iBufLen-1] = 0;
		}
		else if (Dia.auchFsc[0] == FSC_SCANNER || Dia.auchFsc[0] == FSC_P1 || Dia.auchFsc[0] == FSC_P5) {
			_tcsncpy(tcsBuffer, Dia.aszMnemonics, iBufLen);
			tcsBuffer[iBufLen-1] = 0;
		}
        return(UIF_SUCCESS);
	} else if (sReturn == UIFREG_ABORT) {
		REGDISPMSG      Disp = {0};

		Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
		Disp.uchMinorClass = CLASS_REGDISP_CPAN;
        Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
        DispWrite(&Disp);

        Disp.uchMajorClass = CLASS_UIFREGOTHER;
        Disp.uchMinorClass = CLASS_UICLEAR;
        Disp.fbSaveControl = DISP_SAVE_TYPE_4;
        DispWrite(&Disp);

        return(UIF_CANCEL);
    }
   return(UIF_SUCCESS);
}

SHORT   ItemLineLeadthruPromptEntry (USHORT usLeadthruNumber, TCHAR *tcsBuffer, int iBufLen)
{
	WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1] = {0};

	RflGetLead (aszMnemonics, usLeadthruNumber);
	return ItemLinePromptEntry (aszMnemonics, tcsBuffer, iBufLen);
}


SHORT   ItemLinePausePromptEntry (USHORT usPauseNumber, TCHAR *tcsBuffer, int iBufLen)
{
	WCHAR  aszMnemonics[PARA_LEADTHRU_LEN + 1] = {0};

	RflGetPauseMsg (aszMnemonics, usPauseNumber);
	return ItemLinePromptEntry (aszMnemonics, tcsBuffer, iBufLen);
}

                                                         
/*
*===========================================================================
** Synopsis:    VOID GetGiftCardAccount(TRANGCFQUAL    *pWorkGCF)
*     Input:    TRANGCFQUAL    *pWorkGCF
*     Output:   nothing
*     INOut:    TRANGCFQUAL    *pWorkGCF
*
** Return:      
*
** Description: Gets GiftCard Account
*===========================================================================
*/
SHORT GetGiftCardAccount(SHORT sGiftCard, ITEMTENDER *pItem, UCHAR* pauchFsc){
	SHORT	      sPinPadRet = LDT_WRONG_ACTYPE_ADR;
	USHORT        index = 0;
	SHORT         sLen = 26;
	UIFDIADATA    Dia = {0};
	REGDISPMSG    Disp = { 0 };

	//displays "Enter Number"
    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, LDT_NUM_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	if (sGiftCard == IS_GIFT_CARD_FREEDOMPAY) {
		Dia.ulOptions = UIF_PINPAD_OPTION_READ_CONTACTLESS;
	}
	else {
		Dia.ulOptions = UIF_PINPAD_OPTION_READ_STRIPE;
	}
	Dia.ulOptions |= UIF_RULES_NO_CHECK_NUMBER;   // do not check account number against PARAMTDR validation rules

//	sPinPadRet = ItemTendReadPadMsr (&Dia, pItem, sLen, pauchFsc);

	Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
	DispWrite(&Disp);

	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	if(sGiftCard == IS_GIFT_CARD_FREEDOMPAY) {
		if(sPinPadRet == UIF_SUCCESS || sPinPadRet == UIF_VIRTUALKEYBOARD) {
			sPinPadRet = ITM_SUCCESS;
			if (*pauchFsc != FSC_VIRTUALKEYEVENT)
				sPinPadRet = LDT_WRONG_ACTYPE_ADR;
		}
		else
			sPinPadRet = UIF_CANCEL;
	}
	else {
		if (sPinPadRet == UIF_SUCCESS || sPinPadRet == UIF_PINPADSUCCESS) {
			sPinPadRet = ITM_SUCCESS;
		}
		else if (sPinPadRet == UIF_CANCEL)
			sPinPadRet = UIF_CANCEL;
		else
			sPinPadRet = LDT_WRONG_ACTYPE_ADR;
	}
	
	return sPinPadRet;
}
/*
*===========================================================================
** Synopsis:    VOID ItemSalesGetBalance(UIFREGTENDER *UifRegTender)
*     Input:    UIFREGTENDER *UifRegTender
*     Output:   nothing
*     INOut:    UIFREGTENDER *UifRegTender
*
** Return:      
*
** Description: Gets GiftCard Balance
*===========================================================================
*/
SHORT ItemSalesGetBalance(UIFREGTENDER *pUifRegTender,ITEMTENDER *pItemTender)
{
	USHORT  usNo = 1;
	SHORT   sPinPadRet;
	UCHAR	auchFsc;
	
	pUifRegTender->GiftCardOperation = GIFT_CARD_BALANCE;
	
	//Get Account No.
	sPinPadRet = GetGiftCardAccount (pUifRegTender->GiftCard, pItemTender, &auchFsc);
	if(sPinPadRet == ITM_SUCCESS)
	{
		TrnGetTransactionInvoiceNum (pItemTender->refno.auchReferncNo);
		pItemTender->lTenderAmount=00;
 
		//Process Balance
		sPinPadRet = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, pUifRegTender, pItemTender, usNo);
	}

	return sPinPadRet;
}

/*
*===========================================================================
** Synopsis:    VOID ItemSalesBalance()
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Gift Card Balance
*===========================================================================
*/
SHORT ItemSalesBalance(SHORT GiftCard){

	UIFREGTENDER  UifRegTender = {0};
	ITEMTENDER    ItemTender = {0}, ItemTender2 = {0};
	SHORT         sReturnType;
	TRANGCFQUAL   * const pWorkGCF = TrnGetGCFQualPtr();

	sReturnType = ItemCommonSetupTransEnviron ();
	if(sReturnType != ITM_SUCCESS)
		return (sReturnType);

	{
		TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* force to print receipt no matter MODEQUAL_NORECEIPT_DEF and MOD_NORECEIPT setting */
		pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;
	}

	ItemTender.uchMajorClass = CLASS_ITEMTENDER;
	ItemTender.uchMinorClass=3;
	UifRegTender.uchMinorClass=3;
	RflGetLead (ItemTender.aszTendMnem,LDT_GIFTCARD_BALANCE);

	ItemTendGetEPTMDC(UifRegTender.uchMinorClass, &ItemTender.auchTendStatus[0]);

	UifRegTender.GiftCard = GiftCard;

	sReturnType = ItemSalesGetBalance(&UifRegTender, &ItemTender);

	/* inhibit gift receipt printing */
    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );           /* inhibit post receipt, R3.0 */
	if (sReturnType == UIF_CANCEL) {
		flUifRegStatus &= ~UIFREG_MODIFIER;         /* set modifier status */
		ItemOtherClear();                           /* E-068 */       
	}
	else
	{
		ItemTender.fsPrintStatus = PRT_JOURNAL | PRT_GIFT_BALANCE;

		// We want to decrypt and then encrypt the card number before and after
		// we mask the number b/c when the number is sent through TrnItem it must
		// be encrypted for it to be handled by other functions correctly.
		RflDecryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));

		// Mask Number to be printed.
		GiftBalMaskAcct(&ItemTender);

		RflEncryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));
		
		//copy tender info into WorkGCF so it will be used to print later
		pWorkGCF->TrnGiftCard[0].ItemTender = ItemTender;

		//Display Balance
        MldScrollWrite(&ItemTender, MLD_NEW_ITEMIZE);
		
		// Prepare and save balance information for printing.
		GiftBalPrtPrep(&ItemTender);

		{
			REGDISPMSG   Disp = {0}, RegDispBack = {0};

			Disp.fbSaveControl = DISP_SAVE_TYPE_2;       /*save new display settings in memory resident save area */
			Disp.uchMajorClass = CLASS_UIFREGOTHER;
			Disp.uchMinorClass = CLASS_UICLEAR;
			ItemTendDispIn(&RegDispBack);
			DispWriteSpecial( &Disp, &RegDispBack);
		}

		if (sReturnType != ITM_SUCCESS) {
			UieErrorMsg (sReturnType, UIE_WITHOUT);
		}

		ItemTendCloseNoTotals(0x01);
		ItemTendMarkAndCloseGuestCheck(0x01);

		//Next are a series of funtion calls to finalize and close this "transaction"
		// so that a normal operations may continue once the balance has been executed.
		UifRegCheckDrawerLimit(1);						//SR 155 Cash Drawer Limit
		UifRegWorkClear();

		flUifRegStatus &= ~UIFREG_BUFFERFULL;       /* reset buffer full status */
		flUifRegStatus |= UIFREG_FINALIZE;          /* set finalize status */

		ItemFinalize();
		ItemTendInit();
        ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );
		sReturnType = ITM_SUCCESS;
	}

    return (sReturnType);
}

SHORT ItemReturnsLoadGuestCheck (KEYMSG *pData)    // handle FSC_ASK, ITM_ASK_RETURNS
{
	SHORT            sRetStatus = LDT_SEQERR_ADR;

	if (TranGCFQualPtr->fsGCFStatus & (GCFQUAL_TRETURN | GCFQUAL_PVOID))  // status check in ItemReturnsLoadGuestCheck()
		// guest check load not allowed if already in Transaction Return or Transaction Void
		return LDT_SEQERR_ADR;

	/*--- Check  if First Itemize ---*/
	if ( ! ItemSalesLocalSaleStarted() ) {
		ULONG    ulStatus[2] = { 0 };    // cash drawer status, up to 2 drawers in system

		BlFwIfDrawerStatus(ulStatus);

		if (CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_D)) {
			ItemTendCPCommDisp ();

			// ensure that the unique transaction number is properly set
			if (_tcslen(pData->SEL.INPUT.DEV.SCA.auchStr) > 23) {
				pData->SEL.INPUT.DEV.SCA.auchStr[21] = _T('0');
				pData->SEL.INPUT.DEV.SCA.auchStr[22] = _T('0');
			} else {
				return LDT_KEYOVER_ADR;
			}
		}


		if (CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_D)) {
			ItemTendCPCommDisp ();

			sRetStatus = CliGusLoadFromConnectionEngine (pData->SEL.INPUT.DEV.SCA.auchStr);
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
		} else {
			GCNUM    usNewGuestNo = 0;
			GCNUM    usGCNumber = 0;

		   /*----- Get Order# in case of Transaction Open R3.1 -----*/
			sRetStatus = ItemCommonGetSRAutoNo(&usNewGuestNo);
			if (sRetStatus != ITM_SUCCESS) {
				return(sRetStatus);
			}

			usGCNumber = (GCNUM)pData->SEL.INPUT.lData;
			sRetStatus = CliGusLoadPaidTransaction (usGCNumber, usNewGuestNo);
			if (sRetStatus != ITM_SUCCESS) {
				TRANGCFQUAL     * const pWorkGCF = TrnGetGCFQualPtr();
				ITEMCOMMONLOCAL * const pItemCommonLocal = ItemCommonGetLocalPointer();

				ItemCommonResetStatus(COMMON_PROHIBIT_SIGNOUT);

				pWorkGCF->fsGCFStatus &= ~GCFQUAL_TRETURN;

				pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO);  // Clear the Connsecutive number flag to indicate it needs to be generated

				/* finalize: do not clear TRANI_MODEQUAL since it contains the Operator Id
				 *           clearing the Operator Id results in Ask #3 not working as well as improper Cashier Totals.
				 */
				TrnInitialize(TRANI_CURQUAL | TRANI_ITEM); 
				ItemInit();
			}
		}
		if (sRetStatus == ITM_SUCCESS) {
			ITEMSALESLOCAL  * const pWorkSales = ItemSalesGetLocalPointer();
			pWorkSales->fbSalesStatus |= SALES_ITEMIZE;
			UieModeChange(UIE_DISABLE);
		}
	}

	return sRetStatus;
}

/*
*===========================================================================
** Synopsis: SHORT ItemCheckNoPurchase (UIFREGTENDER *pUifRegTender)
*
*    Input: UIFREGTENDER *pUifRegTender indicating tender key and amount
*
*   Output: None
*
*    InOut: None
*
** Return:   UIF_SUCESS      : Return when tender completes successfully
*            LDT_SEQERR_ADR : Return when key sequence error detected. 
*            sReturnStatus : Return when ItemTender () returns error. 
*
** Description: Interface to the GenPOS Rel 2.2 VCS Check No Purchase functionality
*               which uses the DataCap Bad Check Server to validate an entered
*               check against the list of Bad Check accounts stored in the
*               database.
*
*               This function implements the Ask #8, Ask #9, and Ask #10 tender
*               action of FSC_ASK with extended codes of 8, 9, or 10 which is
*               is then transformed into a tender key 8, 9, or 10.
*
*               There is also an older version of Check No Purchase in which
*               a #/Type is used to provide the account number and then a
*               tender key is pressed to document the Check No Purchase amount.
*/
SHORT ItemCheckNoPurchase(UIFREGTENDER *pUifRegTender)
{
	SHORT         sRetStatus;

	// Check No Purchase functionality we will check to see if we should allow it or not.
	// Check No Purchase is only allowed for Tender keys 8, 9, and 10 so if this is one of
	// those tender keys and if the tender type is check and if it is a Bad Check Server
	// transaction type then we will just indicate that Check No Purchase is allowed.
	if (CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0)) {
		PARATENDERKEYINFO	TendKeyInfo = { 0 };

		switch(pUifRegTender->uchMinorClass)
		{//only allow Check Tender button to be pressed next. SR 18 JHHJ
		case CLASS_UITENDER8:												//If the tender pressed is 8,	
			if(!ParaMDCCheck(MDC_TEND84_ADR, EVEN_MDC_BIT2)){	//check to see if it is assigned to Check no Purchase
				NHPOS_ASSERT_TEXT(ParaMDCCheck(MDC_TEND84_ADR, EVEN_MDC_BIT2), "==PROVISION: MDC 216 Bit A not allow Check No Purchase Tender 8.");
				return(LDT_BLOCKEDBYMDC_ADR);				//return Sequence error  SR 18 JHHJ 8-21-03
			}
			break;
		case CLASS_UITENDER9:													//If the tender pressed is 9
			if(!ParaMDCCheck(MDC_TEND94_ADR, EVEN_MDC_BIT2)){	//check to see if it is assigned to Check No Purchase
				NHPOS_ASSERT_TEXT(ParaMDCCheck(MDC_TEND94_ADR, EVEN_MDC_BIT2), "==PROVISION: MDC 218 Bit A not allow Check No Purchase Tender 9.");
				return(LDT_BLOCKEDBYMDC_ADR);				//return Sequence error SR 18 JHHJ 8-21-03
			}
			break;
		case CLASS_UITENDER10:											//If the tender pressed is 10	
			if(!ParaMDCCheck(MDC_TEND104_ADR, EVEN_MDC_BIT2)){	//check to see if it is assigned to Check No Purchase
				NHPOS_ASSERT_TEXT(ParaMDCCheck(MDC_TEND104_ADR, EVEN_MDC_BIT2), "==PROVISION: MDC 220 Bit A not allow Check No Purchase Tender 10.");
				return(LDT_BLOCKEDBYMDC_ADR);				//return Sequence error SR 18 JHHJ 8-21-03
			}
			break;
		default:
			return(LDT_SEQERR_ADR);
		}

		TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
		TendKeyInfo.uchAddress = pUifRegTender->uchMinorClass;
		CliParaRead(&TendKeyInfo);

		if (TendKeyInfo.TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_CHECK) {
			NHPOS_ASSERT_TEXT((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK), "Check No Purchase Invalid Tender type");
			return(LDT_EQUIPPROVERROR_ADR);
		}
	} else {
		NHPOS_ASSERT_TEXT(CliParaMDCCheck(MDC_CHK_NO_PURCH, ODD_MDC_BIT0), "==PROVISION: MDC 379 Bit D Check No Purchase not allowed.");
		return(LDT_SEQERR_ADR);
	}

	{
 		DCURRENCY       lTenderAmount = pUifRegTender->lTenderAmount;
		PARATRANSHALO   WorkHALO = { 0 };

		if (lTenderAmount == 0) {
			return( LDT_KEYOVER_ADR );                                  /* Key Entered Over Limitation */
		}

		WorkHALO.uchMajorClass = CLASS_PARATRANSHALO;
		WorkHALO.uchAddress = 0;

		if ( pUifRegTender->uchMajorClass != CLASS_UIFREGPAMTTENDER) { /* 2172 */
			if (pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN1 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN2) {
				WorkHALO.uchAddress = (UCHAR)(HALO_FOREIGN1_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN1);
			} else if (pUifRegTender->uchMinorClass >= CLASS_UIFOREIGN3 && pUifRegTender->uchMinorClass <= CLASS_UIFOREIGN8) {
				WorkHALO.uchAddress = (UCHAR)(HALO_FOREIGN3_ADR + pUifRegTender->uchMinorClass - CLASS_UIFOREIGN3);
			} else {
				if (pUifRegTender->uchMinorClass >= CLASS_UITENDER1 && pUifRegTender->uchMinorClass <= CLASS_UITENDER8) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND1_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER1 );
				} else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER9 && pUifRegTender->uchMinorClass <= CLASS_UITENDER11) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND9_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER9 );
				} else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER12 && pUifRegTender->uchMinorClass <= CLASS_UITENDER20) {
					WorkHALO.uchAddress = ( UCHAR )( HALO_TEND12_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER12 );
				}
			}
			CliParaRead( &WorkHALO );                                       /* get HALO */
			if ( RflHALO( lTenderAmount, WorkHALO.uchHALO ) != RFL_SUCCESS ) {
				return( LDT_KEYOVER_ADR );                                  /* Key Entered Over Limitation */
			}
		}

		if ( pUifRegTender->fbModifier & VOID_MODIFIER ) {               /* void tender */
			WorkHALO.uchAddress = HALO_VOID_ADR;                        /* Void HALO */
			CliParaRead( &WorkHALO );                                   /* get HALO */

			if ( RflHALO( lTenderAmount, WorkHALO.uchHALO ) != RFL_SUCCESS ) {
				return( LDT_KEYOVER_ADR );                              /* Key Entered Over Limitation */
			}
		}
	}

	if ((sRetStatus = CheckAllowFunction()) != SUCCESS) {
		return sRetStatus;
	}

	{
		ITEMCOMMONLOCAL *pItemCommonLocal = 0;

		ItemCommonGetLocal(&pItemCommonLocal);
		pItemCommonLocal->fbCommonStatus &= ~(COMMON_CONSNO | COMMON_VOID_EC);  // Clear the Connsecutive number flag to indicate it needs to be generated
	}

	sRetStatus = ItemCommonSetupTransEnviron ();
	if (sRetStatus != ITM_SUCCESS)
		return (sRetStatus);

	if (ITM_SUCCESS == sRetStatus) {
		ULONG         ulStatus[2] = { 0 };    // cash drawer status, up to 2 drawers in system

		BlFwIfDrawerStatus(ulStatus);

		uchIsNoCheckOn = 1;     // ItemCheckNoPurchase() indicate to do Check No Purchase, MDC_CHK_NO_PURCH, in the tender functionality

		sRetStatus = ItemTenderEntry(pUifRegTender);    /* Tender Modele */

		uchIsNoCheckOn = 0;      // ItemCheckNoPurchase() having done the tender, now turn it off.
		fsPrtCompul = 0;
		fsPrtNoPrtMask = 0;

		UifRegCheckDrawerLimit(1);						//SR 155 Cash Drawer Limit
		UifRegWorkClear();

		switch (sRetStatus) {
		case UIF_CANCEL:                                /* cancel */
			UifRegData.regtender.fbModifier &= ~OFFCPTEND_MODIF;  /* set off line modifier status */
			flUifRegStatus &= ~UIFREG_MODIFIER;         /* set modifier status */
			ItemOtherClear();                           /* E-068 */       
			break;

		case UIF_SUCCESS:                               /* success */
			flUifRegStatus &= ~UIFREG_BUFFERFULL;       /* reset buffer full status */
			break;

		case LDT_TAKETL_ADR:                            /* buffer full */
			flUifRegStatus |= UIFREG_BUFFERFULL;        /* set buffer full status */
			break;

		case UIF_FINALIZE:                              /* finalize */
			flUifRegStatus |= UIFREG_FINALIZE;          /* set finalize status */
			UifRegClearDisplay(0);						/* clear work area */
			BlFwIfDefaultUserScreen();					/* call function to bring up default window(s) */
			sRetStatus = UIF_SUCCESS;
			break;

		case UIF_CAS_SIGNOUT:                           /* cashier sign out */
			/* set cashier sign out and finalize status */
			flUifRegStatus |= (UIFREG_CASHIERSIGNOUT | UIFREG_FINALIZE);
			sRetStatus = UIF_SUCCESS;
			break;

		case UIF_WAI_SIGNOUT:                           /* waiter sign out */
			/* set waiter sign out and finalize status */
			flUifRegStatus |= (UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE);
			sRetStatus = UIF_SUCCESS;
			break;

		case UIF_SEAT_TRANS:                            /* End of Seat Trans,  R3.1 */
			flUifRegStatus |= UIFREG_BASETRANS;         /* Goto Base Trans Seq */
			flUifRegStatus &= ~(UIFREG_SEATTRANS | UIFREG_TYPE2_TRANS);
			sRetStatus = UIF_SUCCESS;
			break;

		default:
			NHPOS_ASSERT_TEXT((sRetStatus == UIF_SUCCESS), "ItemCheckNoPurchase(): Unknown return type from ItemTender().");
			break;
		}
	}

	/* inhibit gift receipt printing */
    ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );           /* inhibit post receipt, R3.0 */

    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);

	if (sRetStatus != UIF_CANCEL) {
		//Next are a series of funtion calls to finalize and close this "transaction"
		// so that a normal operations may continue once the balance has been executed.
		ItemFinalize();
		ItemTendInit();
	}

	return (sRetStatus);
}

/*
 *  Perform an Employee Payroll Deduction lookup and display the account number
 *  information.
 *
 *  We display the information in the leadthru area so as to inform the Cashier
 *  of the account number allowing the Cashier to type in the number.
**/
SHORT ItemAccountLookupDisplayOnly (SHORT GiftCard, SHORT sLookupType, TCHAR *aszNumber)
{
	UIFREGTENDER  UifRegTender = {0};
	ITEMTENDER    ItemTender = {0};
	SHORT         sReturnType;
	//USHORT returnDisplay;

	if (GiftCard != IS_GIFT_CARD_EPAYMENT)
		return LDT_PROHBT_ADR;

	UifRegTender.GiftCard = GiftCard;
	UifRegTender.GiftCardOperation = sLookupType;

	ItemTender.uchMajorClass = CLASS_ITEMTENDER;
	UifRegTender.uchMinorClass = ItemTender.uchMinorClass = 3;

	ItemTendGetEPTMDC(UifRegTender.uchMinorClass, &ItemTender.auchTendStatus[0]);

	// collect the information that we will need to generate a DataCap XML message.
	// the tender type for a lookup is TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4:
	// however we have two ways of doing an EPD lookup.
	//  - Last Name and last four digits of Social Security Number
	//  - Last Name, First Name, and last four digits of SSN
	// if First Name, Last Name, and SSN is specified then we will use AcctLookupByFullNameSS4
	// however if only Last Name and SSN then we will use AcctLookupByNameSS4
	{
		USHORT  usNo = 1;

		TrnGetTransactionInvoiceNum (ItemTender.refno.auchReferncNo);
		ItemTender.lTenderAmount = 0;
 
		sReturnType = ItemLinePromptEntry (_T("Last Name"), ITEMTENDER_EPDNAMES(&ItemTender).aszLastName, sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszLastName)/sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszLastName[0]));
		if (sReturnType == UIF_SUCCESS) {
			sReturnType = ItemLinePromptEntry (_T("First Name"), ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName, sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName)/sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName[0]));
			if (sReturnType == UIF_SUCCESS) {
				sReturnType = ItemLinePromptEntry (_T("Last 4 Social"), ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo, sizeof(ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo)/sizeof(ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo[0]));
				if (sReturnType == UIF_SUCCESS) {
					//Process Balance
					sReturnType = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);
				}
			}
		}
	}

	if (sReturnType == ITM_SUCCESS) {
		TCHAR aszDisplayBuffer[128];

		// We want to decrypt and then encrypt the card number before and after
		// we mask the number b/c when the number is sent through TrnItem it must
		// be encrypted for it to be handled by other functions correctly.
		RflDecryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));

		_tcsncpy (aszDisplayBuffer, ItemTender.aszCPMsgText[0], 40);
		_tcsncpy (aszDisplayBuffer+40, _T("\r\n"), 2);    // Add carriage return and line feed to start beginning of next line
		_tcsncpy (aszDisplayBuffer+42, ItemTender.aszCPMsgText[1], 40);
		_tcsncpy (aszDisplayBuffer+82, _T("\r\n"), 2);    // Add carriage return and line feed to start beginning of next line
		_tcscpy (aszDisplayBuffer+84, _T("Number: "));
		_tcscat (aszDisplayBuffer+84, ItemTender.aszNumber);

		if (aszNumber)
			_tcsncpy (aszNumber, ItemTender.aszNumber, 20);

		// Mask Number to be printed.
		GiftBalMaskAcct(&ItemTender);

		RflEncryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));

//		UieErrorMsgWithText(0xF001, (UIE_WITHOUT | UIE_ABORT), aszDisplayBuffer);
		{
			int iRet;

			iRet = UieMessageBox(aszDisplayBuffer, _T("Account Lookup"), MB_OKCANCEL | MB_ICONWARNING | MB_SYSTEMMODAL);
			if (iRet != IDOK)
				sReturnType = UIF_CANCEL;
		}
	}
	else if (sReturnType != UIF_CANCEL) {
		UieErrorMsg (sReturnType, UIE_WITHOUT);
	}

    return (sReturnType);
}

SHORT ItemAccountLookup(SHORT GiftCard, SHORT sLookupType)
{
	UIFREGTENDER  UifRegTender = {0};
	ITEMTENDER    ItemTender = {0};
	SHORT         sReturnType;
	//USHORT returnDisplay;
	TRANGCFQUAL   * const pWorkGCF = TrnGetGCFQualPtr();

	UifRegTender.GiftCard = GiftCard;
	UifRegTender.GiftCardOperation = sLookupType;

	ItemTender.uchMajorClass = CLASS_ITEMTENDER;
	UifRegTender.uchMinorClass = ItemTender.uchMinorClass = 3;

	ItemTendGetEPTMDC(UifRegTender.uchMinorClass, &ItemTender.auchTendStatus[0]);

	// collect the information that we will need to generate a DataCap XML message.
	// the tender type for a lookup is TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4:
	// however we have two ways of doing an EPD lookup.
	//  - Last Name and last four digits of Social Security Number
	//  - Last Name, First Name, and last four digits of SSN
	// if First Name, Last Name, and SSN is specified then we will use AcctLookupByFullNameSS4
	// however if only Last Name and SSN then we will use AcctLookupByNameSS4
	{
		USHORT  usNo = 1;

		TrnGetTransactionInvoiceNum (ItemTender.refno.auchReferncNo);
		ItemTender.lTenderAmount = 0;
 
		sReturnType = ItemLinePromptEntry (_T("Last Name"), ITEMTENDER_EPDNAMES(&ItemTender).aszLastName, sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszLastName)/sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszLastName[0]));
		if (sReturnType == UIF_SUCCESS) {
			sReturnType = ItemLinePromptEntry (_T("First Name"), ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName, sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName)/sizeof(ITEMTENDER_EPDNAMES(&ItemTender).aszFirstName[0]));
			if (sReturnType == UIF_SUCCESS) {
				sReturnType = ItemLinePromptEntry (_T("Last 4 Social"), ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo, sizeof(ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo)/sizeof(ITEMTENDER_EPDLAST4SSNO(&ItemTender).aszLast4SSNo[0]));
				if (sReturnType == UIF_SUCCESS) {
					//Process Balance
					sReturnType = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);
				}
			}
		}
	}

	if (sReturnType == ITM_SUCCESS)
	{
		ItemTender.fsPrintStatus = PRT_JOURNAL | PRT_GIFT_BALANCE;

		if (GiftCard == IS_GIFT_CARD_EPAYMENT) {
		}
		else {
			// We want to decrypt and then encrypt the card number before and after
			// we mask the number b/c when the number is sent through TrnItem it must
			// be encrypted for it to be handled by other functions correctly.
			RflDecryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));

			// Mask Number to be printed.
			GiftBalMaskAcct(&ItemTender);

			RflEncryptByteString ((UCHAR *)&(ItemTender.aszNumber[0]), sizeof(ItemTender.aszNumber));
		}
		
		//copy tender info into WorkGCF so it will be used to print later
		pWorkGCF->TrnGiftCard[0].ItemTender = ItemTender;

		//Display Balance
        MldScrollWrite(&ItemTender, MLD_NEW_ITEMIZE);
		
		// Prepare and save balance information for printing.
		GiftBalPrtPrep(&ItemTender);

	}
	else if (sReturnType != UIF_CANCEL) {
		UieErrorMsg (sReturnType, UIE_WITHOUT);
	}

	{
		REGDISPMSG   Disp = {0}, RegDispBack = {0};

		Disp.fbSaveControl = DISP_SAVE_TYPE_2;       /*save new display settings in memory resident save area */
		Disp.uchMajorClass = CLASS_UIFREGOTHER;
		Disp.uchMinorClass = CLASS_UICLEAR;
		ItemTendDispIn(&RegDispBack);
		DispWriteSpecial( &Disp, &RegDispBack);
	}

	if (sReturnType != UIF_CANCEL) {
		ItemTendCloseNoTotals(0x01);
		ItemTendMarkAndCloseGuestCheck(0x01);

		//Next are a series of funtion calls to finalize and close this "transaction"
		// so that a normal operations may continue once the balance has been executed.
		UifRegCheckDrawerLimit(1);						//SR 155 Cash Drawer Limit
		UifRegWorkClear();

		flUifRegStatus &= ~UIFREG_BUFFERFULL;       /* reset buffer full status */
		flUifRegStatus |= UIFREG_FINALIZE;          /* set finalize status */

		ItemFinalize();
		ItemTendInit();
        ItemMiscResetStatus( MISC_POSTRECPT | MISC_PARKING | MISC_GIFTRECEIPT | MISC_PRINTDEMAND );
		sReturnType = ITM_SUCCESS;
	}

	if (sReturnType == ITM_SUCCESS)
		UifRegClearDisplay(FSC_GIFT_CARD);			/* clear work area */
	else
		UifRegClearDisplay(0);			/* clear work area */

    return (sReturnType);
}

/*
*===========================================================================
** Synopsis:   VOID  GiftBalPrtPrep(ITEMTENDER *ItemTender)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Prepares for printing the gift card balance.
*===========================================================================
*/
VOID  GiftBalPrtPrep(ITEMTENDER *ItemTender)
{
	TRANCURQUAL	 * const pWorkCur = TrnGetCurQualPtr();
	ITEMPRINT	 ItemPrint = {0};

	pWorkCur->flPrintStatus |= CURQUAL_EPAY_BALANCE;  // indicate this is an account balance receipt so do not print some standard receipt areas

	// Prepare and store data to be printed.
	ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
	ItemPrint.uchMinorClass = CLASS_BALANCE;
	ItemPrint.fsPrintStatus = PRT_JOURNAL | PRT_RECEIPT;
	memcpy(&ItemPrint.aszCPText, &ItemTender->aszCPMsgText, sizeof(ItemTender->aszCPMsgText));
	ItemPrint.uchCPLineNo = ItemTender->uchCPLineNo;
	ItemPrint.ulStoreregNo = pWorkCur->ulStoreregNo;
	ItemPrint.usConsNo = pWorkCur->usConsNo;

	TrnStorage(&ItemPrint);

	memset(&ItemPrint, 0, sizeof(ItemPrint));
	ItemPrint.uchMajorClass = CLASS_ITEMPRINT;
	ItemPrint.uchMinorClass = CLASS_TRAILER1;
	ItemPrint.fsPrintStatus = PRT_JOURNAL | PRT_RECEIPT;
	ItemPrint.ulStoreregNo = pWorkCur->ulStoreregNo;
	ItemPrint.usConsNo = pWorkCur->usConsNo;
	TrnStorage(&ItemPrint);
}

/*
*===========================================================================
** Synopsis:   VOID  GiftBalMaskAcct(ITEMTENDER *ItemTender)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Pre Auth. masking of gift card account number. This
*				function will also be called when displaying the gift card
*				number for a balance inquiry.
*===========================================================================
*/
static VOID CommonGiftBalMaskAcct (TCHAR *aszNumber, USHORT usMaxChars)
{
	int				iShownChars=0, iMaskedChars=0, i;

	if(!ParaMDCCheck(MDC_GIFT_MASK, EVEN_MDC_BIT1))
	{
		UCHAR  uchMDCData = RflGetMdcData(MDC_GIFT_MASK);

		iShownChars = 0;
		if((uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
			//show 2 rightmost digits
			iShownChars = 2;
		}
		if((uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
			//show 4 rightmost digits
			iShownChars = 4;
		}
		if((uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
			//show 6 rightmost digits
			iShownChars = 6;
		}

		// signed integer arithmetic to ensure if value is negative we don't do loop
		aszNumber[usMaxChars-1] = 0;  // ensure string is zero terminated.
		iMaskedChars = (int)_tcslen(aszNumber) - iShownChars;

		for(i=0; i < iMaskedChars; i++) {
			aszNumber[i] = _T('X');
		}
	}
}

VOID	GiftBalMaskAcct(ITEMTENDER *pItemTender)
{
	CommonGiftBalMaskAcct (pItemTender->aszNumber, sizeof(pItemTender->aszNumber)/sizeof(pItemTender->aszNumber[0]));
}


/*
*===========================================================================
** Synopsis:   VOID  GiftPreAuthMaskAcct(ITEMSALES *pItemSales)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Pre. Authorization masking of gift card account number. This
*				function will be called when displaying the gift card
*				number for pre authorization of a gift card.
*===========================================================================
*/
VOID	GiftPreAuthMaskAcct(ITEMSALES *pItemSales)
{
	CommonGiftBalMaskAcct (pItemSales->aszNumber[0], sizeof(pItemSales->aszNumber[0])/sizeof(pItemSales->aszNumber[0][0]));
}
