/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Normal tender key                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRNTndT.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTender() : prints tender operation
*               PrtTender_TH() : prints tender operation (thermal)
*               PrtTender_EJ() : prints tender operation (electric journal)
*               PrtTender_SP() : prints tender operation (slip)
*
*               PrtDflTender() : displays tender operation
*               PrtDflTenderForm() : displays tender operation
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflTender()                                   
* Oct-27-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Oct-27-93 : 02.00.02 : K.You      : del. PrtDflTender()                                   
* Apr-08-94 : 00.00.04 : Yoshiko.J  : add EPT feature in GPUS
* Apr-10-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtTender())
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflTender()
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDflTenderForm()
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <pmg.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include <prt.h>

extern VOID PrtSoftCHK(UCHAR uchMinorClass);

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
VOID PrtTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
	//SR 131 cwunn 7/16/2003 Tender Digit Masking 
	PARATENDERKEYINFO TendKeyInfo;
	USHORT	usMDCBase, usOffset;
	UCHAR   uchMinorClassTemp = pItem->uchMinorClass;
	
	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		uchMinorClassTemp = ITEMTENDER_CHARGETIPS(pItem).uchMinorTenderClass;
		if (uchMinorClassTemp < CLASS_TENDER1) uchMinorClassTemp = CLASS_TENDER1;
    } else if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
		uchMinorClassTemp = CLASS_TENDER2;    // transform CLASS_TEND_FSCHANGE and use that tender key's MDC settings
	}

	if (uchMinorClassTemp <= CLASS_TENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_TENDER1;
	} else if (uchMinorClassTemp <= CLASS_TENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_TENDER12;
	} else {
		NHPOS_ASSERT_TEXT(0, "PrtTender(): Out of Range Tender.");
		return;
	}

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = uchMinorClassTemp;
	ParaRead(&TendKeyInfo);

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID || 
		TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY)
	{
		// if this is a gift card or FreedomPay then use gift card masking	
		if(!CliParaMDCCheckField (MDC_GIFT_MASK, MDC_PARAM_BIT_C))
		{
			int i;
			int iMaskedChars = 0;			//number of characters to mask
			int iShownChars = 0;			//number of characerst MDC indicates should not be masked
			PARAMDC MaskMDC = {0};

			MaskMDC.uchMajorClass = CLASS_PARAMDC;
			MaskMDC.usAddress = MDC_GIFT_MASK;
			ParaRead(&MaskMDC);			

			// MDC_GIFT_MASK is an even numbered MDC so use the Even masks for checking bits
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			}else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			}else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}
			//iMaskedChars represents the number of characters to mask with X's.
			//This number should be the total number of characters minus the number that should be shown.
			//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
			iMaskedChars = _tcslen(pItem->aszNumber) - iShownChars;
			iMaskedChars = (iMaskedChars > 0) ? iMaskedChars : _tcslen(pItem->aszNumber);

			for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
				pItem->aszNumber[i] = _T('X');
			}
		}
	} else {
		PARAMDC TenderMDC = {0};		//Used to read MDC masking settings and Tender Key settings
		UCHAR	localMDCBitA, localMDCMask; 

		/*SR 131 cwunn 7/15/2003
			Since odd tender keys use even MDC pages and even tender keys use odd MDC
			pages to indicate their charge posting/EPT settings, the following code
			determines which type of MDC page is used.
			localMDCBitA is always the bottom radio button selection on	an MDC page,
			odd mdc bits use ODD_MDC_BIT3/ODD_MDC_MASK and even use EVEN_MDC_BIT3/EVEN_MDC_MASK
			For the MDC pages in question MDC bit A indicates if the
			current tender key causes EPT (on) or Charge Posting (off).
		*/
		TenderMDC.uchMajorClass = CLASS_PARAMDC;
		TenderMDC.usAddress = usMDCBase + (uchMinorClassTemp - usOffset);
		ParaRead(&TenderMDC);

		// Odd MDC address uses the Odd MDC masks
		localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
		localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

		if (pItem->uchMinorClass >= CLASS_TENDER12 && pItem->uchMinorClass <= CLASS_TENDER20)
		{
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA &&
				CliParaMDCCheckField (MDC_EPT_MASK3_ADR, MDC_PARAM_BIT_D))
			{
				int i;
				int iMaskedChars = 0;			//number of characters to mask
				int iShownChars = 0;			//number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK3_ADR;
				ParaRead( &MaskMDC );

				// MDC_EPT_MASK3_ADR is an odd numbered MDC address so use the Odd masks for checking bits
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == ODD_MDC_BIT2){
					//show 2 rightmost digits
					iShownChars = 2;
				}
				if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == ODD_MDC_BIT3){
					//show 4 rightmost digits
					iShownChars = 4;
				}
				if((MaskMDC.uchMDCData & (ODD_MDC_BIT3 | ODD_MDC_BIT2)) == (ODD_MDC_BIT3 | ODD_MDC_BIT2)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber) - iShownChars;
				iMaskedChars = (iMaskedChars > 0) ? iMaskedChars : _tcslen(pItem->aszNumber);

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		} else if(pItem->uchMinorClass >= CLASS_TENDER2 && pItem->uchMinorClass <= CLASS_TENDER6)
		{
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA &&
				CliParaMDCCheckField (MDC_EPT_MASK1_ADR, MDC_PARAM_BIT_B))
			{
				//tender status indicates to print account number Mask only if on Tenders 2-6
				int i;
				int iMaskedChars = 0;			//number of characters to mask
				int iShownChars = 0;			//number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

				// MDC_EPT_MASK2_ADR is an even numbered MDC address so use the Even masks for checking bits
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
					//show 2 rightmost digits
					iShownChars = 2;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
					//show 4 rightmost digits
					iShownChars = 4;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber) - iShownChars;
				iMaskedChars = (iMaskedChars > 0) ? iMaskedChars : _tcslen(pItem->aszNumber);

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			} 
		} else if(pItem->uchMinorClass >= CLASS_TENDER7 && pItem->uchMinorClass <= CLASS_TENDER11)
		{
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA &&
				CliParaMDCCheckField (MDC_EPT_MASK1_ADR, MDC_PARAM_BIT_A))
			{
				int i;
				int iMaskedChars = 0;			//number of characters to mask
				int iShownChars = 0;			//number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

				// MDC_EPT_MASK2_ADR is an even numbered MDC address so use the Even masks for checking bits
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT0){
					//show 2 rightmost digits
					iShownChars = 2;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT1){
					//show 4 rightmost digits
					iShownChars = 4;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber) - iShownChars;
				iMaskedChars = (iMaskedChars > 0) ? iMaskedChars : _tcslen(pItem->aszNumber);

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		}
	}
    

	/* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if (pItem->fsPrintStatus & PRT_VALIDATION) {         /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
			//US Customs modified to use the Begin and End small validation settings of print spooler
            if(PRT_SUCCESS == PrtSPVLInit())
			{
				PmgBeginSmallValidation( PMG_PRT_SLIP );
				PrtSPVLHead(pTran);    
				PrtTender_SP(pTran, pItem, 1);          /* SlipValidation E-067 */
				PrtSPVLTrail(pTran);
				PmgEndSmallValidation( PMG_PRT_SLIP );
			}
        }
		RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
        return ;
    }

    if ( !(pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING ))) {
   		/* not execute slip validation at duplicated receipt, 09/11/01 */
	    if (pItem->auchTendStatus[1] & ODD_MDC_BIT3) {
    	    if ((pItem->fsPrintStatus & PRT_SLIP) && (pItem->uchCPLineNo != 0)) {                /* Saratoga */
            	if ((usPrtSlipPageLine == 0) || ((fsPrtCompul & PRT_SLIP) == 0)) {  /* not listing slip */
            		/* usPrtSlipPageLine == 0) { */
					/* slip validation print for EPT response message */
	            	usPrtSlipPageLine = 0;	/* reset for slip release */
					if(PRT_SUCCESS == PrtSPVLInit())
					{
						PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
    					PrtSPVLHead(pTran);    
        				PrtTender_SP(pTran, pItem, 0);
            			PrtSPVLTrail(pTran);
						PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
						/* if (fsPrtPrintPort & PRT_RECEIPT) {         / thermal print /
   							PrtTender_TH(pTran, pItem);
        				}
	        			if (fsPrtPrintPort & PRT_JOURNAL) {         / electric journal /
		        			PrtTender_EJ(pItem);
    					} */
					}
					RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	    			return;	/* fixed return position at EPT tender, 09/18/01 */
				}
	        }
    	    /* return; */
    	}
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
        PrtTender_SP(pTran, pItem, 0);              /* normal slip          */
    }
    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtTender_TH(pTran, pItem);
    }
    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
#ifdef WEWANTTOFULLYHIDEEJ
		BOOL	fPrintAcctNum = FALSE;
		
		//SR 742 if the tender pressed and has an account number
		//it may or may not have to be masked. 
		//we see if we are using EPT, Credit Authorization, or CP
		//if we are not, then we want to print the entire account number
		//in the Electronic Journal JHHJ
		if(usAddress % 2)
		{
			if((!CliParaMDCCheck(usAddress, ODD_MDC_BIT0)) &&
				(!CliParaMDCCheck(usAddress, ODD_MDC_BIT1))&&
				(!CliParaMDCCheck(usAddress, ODD_MDC_BIT2))&&
				(!CliParaMDCCheck(usAddress, ODD_MDC_BIT3)))
			{
				fPrintAcctNum = TRUE;

			}
		}else
		{
			if((!CliParaMDCCheck(usAddress, EVEN_MDC_BIT0)) &&
				(!CliParaMDCCheck(usAddress, EVEN_MDC_BIT1))&&
				(!CliParaMDCCheck(usAddress, EVEN_MDC_BIT2))&&
				(!CliParaMDCCheck(usAddress, EVEN_MDC_BIT3)))
			{
				fPrintAcctNum = TRUE;
			}
		}
		//We are using EPT,Credit, or CP, then we 
		//want to mask the digits. JHHJ
		if(!fPrintAcctNum)
		{
			//SR 643 Account# and Date always mask on EJ

			iMaskedChars = _tcslen(pItem->aszNumber);

			if (pItem->aszNumber[0] != _T('\0')) {
				for(i=0; i < iMaskedChars; i++){
					pItem->aszNumber[i] = _T('X');
				}
			}

			if (pItem->auchExpiraDate[0] != _T('\0')) {		
				for(i=0; i < NUM_EXPIRA; i++) {
					pItem->auchExpiraDate[i] = _T('X');
				}
			}
		}
#endif
        PrtTender_EJ(pItem);
    }

	if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_BALANCE) {
		PrtRectCompul();
	}

	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
}

/*
*===========================================================================
** Format  : VOID  PrtTender_TH(TRANINFORMATION *pTran, ITEMTENDER   *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (thermal)
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
VOID PrtTender_TH(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
	PARAMDC TenderMDC;  //Used to read Tender Key settings
    USHORT  i;
	USHORT	usMDCBase, usOffset;
	UCHAR	localMDCBitA, localMDCMask;   	//used in reading MDC settings of Tender Keys
	UCHAR   uchMinorClassTemp = pItem->uchMinorClass;

    PrtTHHead(pTran);                                   /* print header if necessary */

    PrtTHOffTend(pItem->fbModifier);                    /* Cp off line */
	if (pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		PrtTHVoid(pItem->fbModifier, pItem->usReasonCode);                       /* void line */
	}

    PrtTHCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID);

	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		uchMinorClassTemp = ITEMTENDER_CHARGETIPS(pItem).uchMinorTenderClass;
		if (uchMinorClassTemp < CLASS_TENDER1) uchMinorClassTemp = CLASS_TENDER1;
    } else if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
		uchMinorClassTemp = CLASS_TENDER2;    // transform CLASS_TEND_FSCHANGE and use that tender key's MDC settings
	}

	if (uchMinorClassTemp <= CLASS_TENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_TENDER1;
	} else if (uchMinorClassTemp <= CLASS_TENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_TENDER12;
	} else {
		NHPOS_ASSERT_TEXT(0, "PrtTender_TH(): Out of Range Tender.");
		return;
	}

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = usMDCBase + (uchMinorClassTemp - usOffset);
	ParaRead(&TenderMDC);
	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

	//always print offline symbol
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
	}
	else 
	{
		if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
			PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
			PrtTHNumber(pItem->aszNumber);
		}
	}
	//SSTOLTZ
//	if( _tcscmp(pItem->aszMnem, _T("\0")) == 0){
	// change made for Amtrak receipts to show the TRANSACTION TYPE
	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		PrtTHAmtTwoMnem((RflChkTendAdr(pItem) | STD_FORMATMASK_NO_AMOUNT), 0, pItem->lTenderAmount, pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]);
	} else if ((TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP)) != 0) {
		if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
			if ((pItem->fbModifier & RETURNS_ORIGINAL) != 0) {
				PrtTHAmtTwoMnem(RflChkTendAdr(pItem), 0, pItem->lTenderAmount, pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]);
			} else {
				USHORT  usReturnType = TRN_DSI_TRANSSALE;
				USHORT  usAdr1 = TRN_DSI_TRANSTYPE;
				if (pTran->TranCurQual.fsCurStatus & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {
					// For an Amtrak return if the tender amount is positive, non-zero then use the
					// SALE mnemonic and not the RETURN mnemonic.
					if (pTran->TranModeQual.usReturnType & 0x0100) {    // check to see if RETURNS_MODIFIER_1 has been used in this transaction
						usReturnType = TRN_TRETURN1_ADR;
					} else if (pTran->TranModeQual.usReturnType & 0x0200) {   // check to see if RETURNS_MODIFIER_2 has been used in this transaction
						if (pItem->lTenderAmount <= 0)
							usReturnType = TRN_TRETURN2_ADR;
					} else if (pTran->TranModeQual.usReturnType & 0x0400) {   // check to see if RETURNS_MODIFIER_3 has been used in this transaction
						if (pItem->lTenderAmount <= 0)
							usReturnType = TRN_TRETURN3_ADR;
					}
				}
				if ((pTran->ulCustomerSettingsFlags & SYSCONFIG_CUSTOMER_ENABLE_AMTRAK) == 0) {
					usAdr1 = RflChkTendAdr(pItem);
					usReturnType = 0;
				}
				PrtTHAmtTwoMnem(usAdr1, usReturnType, pItem->lTenderAmount, 0);
			}
		}
	} else {
		PrtTHAmtTwoMnem(RflChkTendAdr(pItem), 0, pItem->lTenderAmount, pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]);
	}

    if (pItem->lFoodStamp) {                            /* FS */
        PrtTHZeroAmtMnem(TRN_FSCHNG_ADR, pItem->lFoodStamp);
    }

    if (pItem->lFSChange) {                             /* FS credit */
        PrtTHZeroAmtMnem(TRN_FSCRD_ADR, pItem->lFSChange);
    }

    if (pItem->lGratuity && (pTran->TranCurQual.fsCurStatus2 & PRT_PREAUTH_RECEIPT) == 0) {   /* charge tips associated with this tender */
        PrtTHZeroAmtMnem((TRN_CHRGTIP_ADR | STD_FORMATMASK_INDENT_4), pItem->lGratuity);
    }

	PrtTHZeroAmtMnem(TRN_REGCHG_ADR, pItem->lChange);   /* change */

	// if this is an Employee Payroll Deduction balance inquiry receipt or an EPT receipt then
	// we need to print the response text. we also need to print a receipt if this is an
	// electronic payment tender which was Declined (ITM_REJECT_DECLINED).
	if ((TrnInformation.TranCurQual.flPrintStatus & CURQUAL_EPAY_BALANCE) ||
		((TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT) == 0 &&
		(TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP)) != 0) ||
		(pItem->fbModifier & DECLINED_EPT_MODIF) != 0) {

		if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
			// we only want to print the response text for Electronic Payment for a Return if this is the
			// not the tender from the original.  If this was from the original transaction data then
			// we will not print the response text.

			if (pItem->uchPrintSelect != 0) {
				TCHAR   *puchStart = (TCHAR *)pItem->aszCPMsgText;
				for (i = 0; i < pItem->uchCPLineNo; i++) {
					TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};

					_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
					if (_tcsstr (aszPrint, _T("STORED")) != 0 && (TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_CUST_DUP | CURQUAL_MERCH_DUP)) != 0)
						continue;
					tcharTrimRight (aszPrint + 1);    // trim trailing spaces but if only spaces then keep the first one.
					PrtTHCPRspMsgText(aszPrint);
				}
				// If this tender is supposed to be printed with the Tip and Total lines because it is a Preauth then
				// print the tip and total lines after the EPT response text
				if ((pItem->fbReduceStatus & REDUCE_ITEM) == 0 && (pItem->fbModifier & RETURNS_ORIGINAL) == 0) {
					PARATENDERKEYINFO TendKeyInfo;

					TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
					TendKeyInfo.uchAddress = pItem->uchMinorClass;
					ParaRead(&TendKeyInfo);

					if(TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTH && (TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0 && (fsPrtPrintPort & PRT_RECEIPT)) {     /* thermal print */
						PrtTHTipTotal(PMG_PRT_RECEIPT);   // print tip and total lines since this is a TENDERKEY_TRANCODE_PREAUTH
					}
				}
			} else {
				USHORT  usTextOut = 0;
				for (i = 0; i < NUM_CPRSPCO; i++) {
					TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};

					_tcsncpy(aszPrint, pItem->aszCPMsgText[i], 39);
					usTextOut = (usTextOut || (pItem->aszCPMsgText[i][0] != 0));
					if (_tcsstr (aszPrint, _T("STORED")) != 0 && (TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_CUST_DUP | CURQUAL_MERCH_DUP)) != 0)
						continue;
					tcharTrimRight (aszPrint + 1);    // trim trailing spaces but if only spaces then keep the first one.
					PrtTHCPRspMsgText(aszPrint);      /* for charge posting */
				}
				// If this tender is supposed to be printed with the Tip and Total lines because it is a Preauth then
				// print the tip and total lines after the EPT response text
				if ((pItem->fbReduceStatus & REDUCE_ITEM) == 0 && (pItem->fbModifier & RETURNS_ORIGINAL) == 0) {
					PARATENDERKEYINFO TendKeyInfo;

					TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
					TendKeyInfo.uchAddress = pItem->uchMinorClass;
					ParaRead(&TendKeyInfo);

					if(TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTH && (TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0 && (fsPrtPrintPort & PRT_RECEIPT)) {     /* thermal print */
						PrtTHTipTotal(PMG_PRT_RECEIPT);   // print tip and total lines since this is a TENDERKEY_TRANCODE_PREAUTH
					}
				}
				if (usTextOut && (TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0 && (pItem->fbModifier & RETURNS_ORIGINAL) == 0) {
					PrtSoftCHK(SOFTCHK_EPT1_ADR);
				}
			}
		}
	}
}

/*
*===========================================================================
** Format  : VOID  PrtTender_EJ(ITEMTENDER   *pItem);      
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (electric journal)
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
VOID PrtTender_EJ(ITEMTENDER  *pItem)
{
	PARAMDC TenderMDC;  //Used to read Tender Key settings
    USHORT  i;
	USHORT	usMDCBase, usOffset;
	UCHAR	localMDCBitA, localMDCMask;   	//used in reading MDC settings of Tender Keys
	UCHAR   uchMinorClassTemp = pItem->uchMinorClass;

    PrtEJOffTend(pItem->fbModifier);                  /* void line */
	if (pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		PrtEJVoid(pItem->fbModifier, pItem->usReasonCode);                     /* void line */
	}

    PrtEJCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID);         /* for charge posting */

	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		uchMinorClassTemp = ITEMTENDER_CHARGETIPS(pItem).uchMinorTenderClass;
		if (uchMinorClassTemp < CLASS_TENDER1) uchMinorClassTemp = CLASS_TENDER1;
    } else if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
		uchMinorClassTemp = CLASS_TENDER2;    // transform CLASS_TEND_FSCHANGE and use that tender key's MDC settings
	}

	if (uchMinorClassTemp <= CLASS_TENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_TENDER1;
	} else if (uchMinorClassTemp <= CLASS_TENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_TENDER12;
	} else {
		NHPOS_ASSERT_TEXT(0, "PrtTender_EJ(): Out of Range Tender.");
		return;
	}

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = usMDCBase + (uchMinorClassTemp - usOffset);
	ParaRead(&TenderMDC);
	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

	//always print offline symbol
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		PrtEJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
		//if tender status indicates EPT
		if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
			PrtEJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
			PrtEJNumber(pItem->aszNumber);
		}
	}

    PrtEJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */
	
	//SSTOLTZ
	PrtEJAmtAndMnemonic(pItem);

    if (pItem->lFoodStamp) {                            /* FS */
        PrtEJZeroAmtMnem(TRN_FSCHNG_ADR, pItem->lFoodStamp);
    }

    if (pItem->lFSChange) {                             /* FS credit */
        PrtEJZeroAmtMnem(TRN_FSCRD_ADR, pItem->lFSChange);
    }

    if (pItem->lGratuity) {                             /* charge tips associated with this tender */
        PrtEJZeroAmtMnem((TRN_CHRGTIP_ADR | STD_FORMATMASK_INDENT_4), pItem->lGratuity);
    }

    PrtEJZeroAmtMnem(TRN_REGCHG_ADR, pItem->lChange);   /* change */

	if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		// we only want to print the response text for Electronic Payment for a Return if this is the
		// not the tender from the original.  If this was from the original transaction data then
		// we will not print the response text.
		if (pItem->uchPrintSelect != 0) {
			TCHAR   *puchStart = pItem->aszCPMsgText[0];
			for (i = 0; i < pItem->uchCPLineNo; i++) {
				TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};

				_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
				PrtEJCPRspMsgText(aszPrint);                    /* Saratoga */
			}

		} else {
			for (i = 0; i < NUM_CPRSPCO; i++) {
				TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};
				_tcsncpy(aszPrint, pItem->aszCPMsgText[i], STD_CPRSPTEXT_LEN);
				PrtEJCPRspMsgText (aszPrint);      /* for charge posting */
			}
		}
	}
}

/*
*===========================================================================
** Format  : VOID  PrtTender_SP(TRANINFORMATION *pTran, ITEMTENDER *pItem,
*                               UCHAR uchType);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMTENDER       *pItem     -Item Data address
*            UCHAR            uchType    - 0: normal slip, 1: slip validation
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (slip)
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
VOID PrtTender_SP(TRANINFORMATION *pTran, ITEMTENDER *pItem, UCHAR uchType) 
{
	PARAMDC TenderMDC;  //Used to read Tender Key settings
	TCHAR   aszSPPrintBuff[8 + NUM_CPRSPTEXT][PRT_SPCOLUMN + 1] = {0};
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   
	USHORT	usMDCBase, usOffset;
	UCHAR	localMDCBitA, localMDCMask;   	//used in reading MDC settings of Tender Keys
	UCHAR   uchMinorClassTemp = pItem->uchMinorClass;

	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		uchMinorClassTemp = ITEMTENDER_CHARGETIPS(pItem).uchMinorTenderClass;
		if (uchMinorClassTemp < CLASS_TENDER1) uchMinorClassTemp = CLASS_TENDER1;
    } else if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
		uchMinorClassTemp = CLASS_TENDER2;    // transform CLASS_TEND_FSCHANGE and use that tender key's MDC settings
	}

	if (uchMinorClassTemp <= CLASS_TENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_TENDER1;
	} else if (uchMinorClassTemp <= CLASS_TENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_TENDER12;
	} else {
		NHPOS_ASSERT_TEXT(0, "PrtTender_EJ(): Out of Range Tender.");
		return;
	}

    /* -- set off line tender -- */
    usSlipLine += PrtSPCPOffTend(aszSPPrintBuff[0], pItem->fbModifier); 

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = usMDCBase + (uchMinorClassTemp - usOffset);
	ParaRead(&TenderMDC);
	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

	//always print offline symbol
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );     /* EPT offline & exp.date   */
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
		//if tender status indicates EPT
		if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
			//then check if MDC indicates print expiration date on EPT Tender,
			if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
				//then print the expiration date
				usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );  /* EPT offline & exp.date   */
			}
		}
		else {	//if tender status indicates CP
			if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit 3, ept is not CP
				//then check if MDC indicates print expiration date on CP Tender
				if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
					usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );   /* EPT offline & exp.date   */
				}
			}
			else { //tender status indicates non-EPT or CP tender, expiration date
				usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );    /* EPT offline & exp.date   */
			}
		}
	}

	//always print void lines
	if (pItem->fbModifier & VOID_MODIFIER){
		usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
		//if tender status indicates EPT
		if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
			usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
		}
	}

    /* -- set room number and guest id -- */
    if ((pItem->aszRoomNo[0] != '\0') && (pItem->aszGuestID[0] != '\0')) {
        usSlipLine += PrtSPCPRoomCharge(aszSPPrintBuff[usSlipLine], pItem->aszRoomNo, pItem->aszGuestID);
    }
    /* -- set tender mnemonic and amount -- */
    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], RflChkTendAdr(pItem),  pItem->lTenderAmount);
    }

    if (pItem->lFoodStamp) {                            /* FS */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_FSCHNG_ADR, pItem->lFoodStamp);
    }

    if (pItem->lFSChange) {                             /* FS credit */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_FSCRD_ADR, pItem->lFSChange);
    }

    if (pItem->lGratuity) {                             /* FS credit */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_CHRGTIP_ADR, pItem->lGratuity);
    }

    /* -- set change mnemonic and amount -- */
    if (pItem->lChange) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_REGCHG_ADR, pItem->lChange);
    }

	if ((TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT) == 0) {
		if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
			// we only want to print the response text for Electronic Payment for a Return if this is the
			// not the tender from the original.  If this was from the original transaction data then
			// we will not print the response text.
			/* -- set response message text -- */
			if (uchType == 0) {                                 /* E-067 corr. 4/25 */
				if (pItem->uchPrintSelect != 0) {
					for (i = 0; i < pItem->uchCPLineNo && usSlipLine < 46; i++) {
						TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};
						_tcsncpy(aszPrint, &pItem->aszCPMsgText[0][0] + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
						usSlipLine += PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], aszPrint);
					}

				} else {
					for (i = 0; i < NUM_CPRSPCO && usSlipLine < 46; i++) {
						usSlipLine += PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], &pItem->aszCPMsgText[i][0]);
					}
				}
			}
		}
	}

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}


/*
*===========================================================================
** Format  : VOID  PrtDflTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
VOID PrtDflTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
	TCHAR  aszDflBuff[PRT_DFL_LINE_MAX + 5][PRT_DFL_LINE + 1] = { 0 };   /* display data save area, must be no larger than KDS_LINE_DATA_LEN TCHARs */
	USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflOffTend(aszDflBuff[usLineNo], pItem->fbModifier); /* Cp off line */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflCPRoomCharge(aszDflBuff[usLineNo], pItem->aszRoomNo, pItem->aszGuestID);  /* for charge posting */

    /* EPT offline symbol & exp.date */ 
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], RflChkTendAdr(pItem), pItem->lTenderAmount);
    }

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCHNG_ADR, pItem->lFoodStamp);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCRD_ADR, pItem->lFSChange);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CHRGTIP_ADR, pItem->lGratuity);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

	if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		// we only want to print the response text for Electronic Payment for a Return if this is the
		// not the tender from the original.  If this was from the original transaction data then
		// we will not print the response text.
		if (pItem->uchPrintSelect != 0) {
			TCHAR   *puchStart = pItem->aszCPMsgText[0];

			for (i = 0; i < pItem->uchCPLineNo && usLineNo < PRT_DFL_LINE_MAX; i++) {
				TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = { 0 };

				_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
				usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], aszPrint);         /* for charge posting */
			}
		} else {
			for (i = 0; i < NUM_CPRSPCO && usLineNo < PRT_DFL_LINE_MAX; i++) {
				usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], pItem->aszCPMsgText[i]);         /* for charge posting */
			}
		}
	}

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TENDER); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }    
    }

    /* -- send display data -- */
    PrtDflSend();

}

/*
*===========================================================================
** Format  : VOID  PrtDflTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
USHORT PrtDflTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer)
{
	TCHAR  aszDflBuff[PRT_DFL_LINE_MAX + 5][PRT_DFL_LINE + 1] = { 0 };   /* display data save area, must be no larger than KDS_LINE_DATA_LEN TCHARs */
    USHORT  usLineNo = 0;                       /* number of lines to be displayed */
    USHORT  usOffset = 0, i;                       

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflOffTend(aszDflBuff[usLineNo], pItem->fbModifier); /* Cp off line */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflCPRoomCharge(aszDflBuff[usLineNo], pItem->aszRoomNo, pItem->aszGuestID);  /* for charge posting */

    /* EPT offline symbol & exp.date */ 
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], RflChkTendAdr(pItem), pItem->lTenderAmount);
    }

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCHNG_ADR, pItem->lFoodStamp);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCRD_ADR, pItem->lFSChange);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CHRGTIP_ADR, pItem->lGratuity);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

	if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		// we only want to print the response text for Electronic Payment for a Return if this is the
		// not the tender from the original.  If this was from the original transaction data then
		// we will not print the response text.
		if (pItem->uchPrintSelect != 0) {
			TCHAR   *puchStart = pItem->aszCPMsgText[0];

			for (i = 0; i < pItem->uchCPLineNo && usLineNo < PRT_DFL_LINE_MAX; i++) {
				TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = { 0 };

				_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
				usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], aszPrint);         /* for charge posting */
			}
		} else {
			for (i = 0; i < NUM_CPRSPCO && usLineNo < PRT_DFL_LINE_MAX; i++) {
				usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], pItem->aszCPMsgText[i]);         /* for charge posting */
			}
		}
	}

	// set a carriage return on the end of each print line.
    for (i = 0; i < usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    
	// limit to size KDS_LINE_DATA_LEN characters.
	_tcsncpy(puchBuffer, aszDflBuff[0], ((usLineNo < PRT_DFL_LINE_MAX) ? usLineNo : PRT_DFL_LINE_MAX) * (PRT_DFL_LINE + 1));
    return usLineNo;

}

#if POSSIBLE_DEAD_CODE
	// only place this was used was in mldntnd.c function MldPrechkTender()
	// where it was giving problems and commented out.
/*
*===========================================================================
** Format  : SHORT CheckGiftMnem(ITEMTENDER *pItem)    
*
*   Input  : 
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
SHORT CheckGiftMnem(ITEMTENDER *pItem){
	PARALEADTHRU		Lead;
	SHORT GiftUsed;
	GiftUsed=FALSE;

	Lead.uchMajorClass = CLASS_PARALEADTHRU;
	Lead.uchAddress = LDT_RELOAD;
	CliParaRead(&Lead);
	if(_tcscmp(pItem->aszTendMnem,Lead.aszMnemonics) == 0 ){
		GiftUsed=TRUE;
	}
	Lead.uchAddress = LDT_ISSUE;
	CliParaRead(&Lead);
	if(_tcscmp(pItem->aszTendMnem,Lead.aszMnemonics) == 0 ){
		GiftUsed=TRUE;
	}
	Lead.uchAddress = LDT_VOID_ISSUE;
	CliParaRead(&Lead);
	if(_tcscmp(pItem->aszTendMnem,Lead.aszMnemonics) == 0 ){
		GiftUsed=TRUE;
	}
		Lead.uchAddress = LDT_VOID_RELOAD;
	CliParaRead(&Lead);
	if(_tcscmp(pItem->aszTendMnem,Lead.aszMnemonics) == 0 ){
		GiftUsed=TRUE;
	}
		Lead.uchAddress = LDT_GIFTCARD_BALANCE;
	CliParaRead(&Lead);
	if(_tcscmp(pItem->aszTendMnem,Lead.aszMnemonics) == 0 ){
		GiftUsed=TRUE;
	}

	if(GiftUsed == TRUE)
		return TRUE;
	else
		return FALSE;

}
#endif
/***** End Of Source *****/