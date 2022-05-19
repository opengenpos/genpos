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
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Multiline Display  Normal tender key
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDNTND.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldTender() : displays tender operation
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-10-95 : 03.00.00 : M.Ozawa    : Initial
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
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <mld.h>
#include <item.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : USHORT  MldSpecialMnemAmt(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount
*                               USHORT usDispType);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmount         -Amount
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
static USHORT  MldSpecialMnemAmt(TCHAR *pszWork, USHORT usTranAdr, ITEMTENDER *pItem, USHORT usDispType)
{
	CONST TCHAR  *aszMldMnemAmt = _T("%-s \t%l$");
	CONST TCHAR  *aszMldMnemNoAmt = _T("%-s");
	CONST TCHAR  *aszMldMnemFormat = 0;
    USHORT  usRow;
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN * 2 + 5] = {0};  /* PARA_... defined in "paraequ.h" */

	if( _tcscmp(pItem->aszTendMnem, _T("\0")) == 0)//
		/* -- get transaction mnemonics -- */
		if ((usTranAdr & STD_FORMATMASK_INDENT_4) != 0) {
			/* -- get transaction  mnemonics -- */
			aszTranMnem [0] = aszTranMnem [1] = aszTranMnem [2] = aszTranMnem [3] = _T(' ');
			RflGetTranMnem(aszTranMnem + 4, usTranAdr);
		} else {
			RflGetTranMnem(aszTranMnem, usTranAdr);
		}
	else {
		_tcscpy(aszTranMnem,pItem->aszTendMnem);
	}

	if (pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL][0]) {
		_tcscat (aszTranMnem, _T(" ("));
		_tcsncat (aszTranMnem, pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], PARA_TRANSMNEMO_LEN - 6);
		_tcscat (aszTranMnem, _T(")"));
	}

	if (pItem->uchMinorClass == CLASS_TEND_TIPS_RETURN) {
		aszMldMnemFormat = aszMldMnemNoAmt;
	} else {
		aszMldMnemFormat = aszMldMnemAmt;
	}

    if (usDispType == MLD_PRECHECK_SYSTEM) {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemFormat, aszTranMnem, pItem->lTenderAmount);
    } else {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemFormat, aszTranMnem, pItem->lTenderAmount);
    }

    return(usRow);
}

/*
*===========================================================================
** Format  : VOID  MldTender(ITEMTENDER *pItem, USHORT usType):
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays tender operation
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
static SHORT MldPrechkTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG   alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1];
	PARATENDERKEYINFO TendKeyInfo;
	PARAMDC TenderMDC;  //Used to read MDC masking settings and Tender Key settings
	USHORT	usMDCBase, usOffset;
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
	//The following UCHARS are used in reading MDC settings of Tender Keys
	UCHAR	localMDCBitA, localMDCMask;
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
		NHPOS_ASSERT_TEXT(0, "MldPrechkTender(): Out of Range Tender.");
		return (MLD_ERROR);
	}

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = usMDCBase + (uchMinorClassTemp - usOffset);
	ParaRead(&TenderMDC);
	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = uchMinorClassTemp;
	ParaRead(&TendKeyInfo);

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	if ((pItem->fsPrintStatus & PRT_GIFT_BALANCE) != 0) {
		usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_PRECHECK_SYSTEM);
	}

	//chech for a gift card and use gift card masking 
	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID || TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY){
		GiftBalMaskAcct(pItem);
	} else {
		//SR 131 cwunn 7/17/2003 Determine masking settings
		if (pItem->uchMinorClass >= CLASS_TENDER12 && pItem->uchMinorClass <= CLASS_TENDER20)
		{
			if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) && ParaMDCCheck(MDC_EPT_MASK3_ADR, ODD_MDC_BIT0))
			{
				//SR 131 cwunn 7/16/2003 Tender Digit Masking
				int i;
				int iMaskedChars = 0;  //number of characters to mask
				int iShownChars = 0;   //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK3_ADR;
				ParaRead( &MaskMDC );

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
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		} else if((pItem->uchMinorClass >= CLASS_TENDER2) && (pItem->uchMinorClass <= CLASS_TENDER6)) {
			//SR 131 cwunn 7/17/2003 Indicate how many account digits to show
			//tender status indicates EPT & MDC masks EPTs, Mask only if on Tenders 2-6
			if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) && ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2))
			{
				//SR 131 cwunn 7/16/2003 Tender Digit Masking
				int i;
				int iMaskedChars = 0;  //number of characters to mask
				int iShownChars = 0;   //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
					//show 2 rightmost digits
					iShownChars = 2;
				} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
					//show 4 rightmost digits
					iShownChars = 4;
				} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber) - iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		} else if((pItem->uchMinorClass >= CLASS_TENDER7) && (pItem->uchMinorClass <= CLASS_TENDER11)) {
			if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) && ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3))
			{
				//tender status indicates CP (not EPT) and MDC masks CP Mask only if on Tenders 7-11
				//SR 131 cwunn 7/16/2003 Tender Digit Masking
				int i;
				int iMaskedChars = 0;  //number of characters to mask
				int iShownChars = 0;   //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

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
				iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		}
	}

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_PRECHECK_SYSTEM);

    //SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
	//if tender status indicates EPT
	if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
		usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_PRECHECK_SYSTEM);
	}

	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));

	//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		//always print offline symbol, EPT offline & exp.date
		usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_PRECHECK_SYSTEM);
	}
	else {	//if tender status indicates EPT
		if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
			//then check if MDC indicates print expiration date on EPT Tender,
			if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
				//then print the expiration date, EPT offline & exp.date 
				usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_PRECHECK_SYSTEM);
			}
		}
		else {	//if tender status indicates CP
			if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit A, ept is not CP
				//then check if MDC indicates print expiration date on CP Tender, EPT offline & exp.date
				if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
					usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_PRECHECK_SYSTEM);
				}
			}
			else { //tender status indicates non-EPT or CP tender, expiration date, EPT offline & exp.date
				usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_PRECHECK_SYSTEM);
			}
		}
	}

    /* -- set room number and guest id -- */
    if ((pItem->aszRoomNo[0] != 0xff) && (pItem->aszRoomNo[0] != '\0') && (pItem->aszGuestID[0] != '\0')) {
        usScrollLine += MldCPRoomCharge(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszRoomNo, pItem->aszGuestID, MLD_PRECHECK_SYSTEM);
    }

    if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
        if (pItem->lFSChange || pItem->lFoodStamp) {
			LONG   lFS = pItem->lFoodStamp / 100L;    // divide by 100 in order to truncate the amount to US dollars. Food Stamp is US specific.
            usScrollLine += MldFSMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_FSCHNG_ADR, lFS, pItem->lFSChange, MLD_PRECHECK_SYSTEM);
        }
        if (pItem->lChange) {
            usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_PRECHECK_SYSTEM);
        }

    } else {
        /* -- set tender mnemonic and amount -- */
//RJC --------------------------------------------
//RJC   following lines commented out as they are causing problems with the cash tender
//RJC   mnemonic.  For some reason, the function CheckGiftMnem() is indicating that a
//RJC   cash tender with a number such as $10 or $20 is a gift card and it is clearing
//RJC   the mnemonic.
//RJC		GiftRSP = CheckGiftMnem(pItem);
//RJC		if (GiftRSP == TRUE){
//RJC			usScrollLine += MldGiftMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszMnem,	pItem->lTenderAmount, MLD_PRECHECK_SYSTEM);
//RJC		}
//RJC		else{
			usScrollLine += MldSpecialMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTendAdr(pItem),  pItem, MLD_PRECHECK_SYSTEM);
//RJC		}

        /* -- set change mnemonic and amount -- */
        if (pItem->lChange) {
            usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_PRECHECK_SYSTEM);
        }
    }

	if (pItem->lGratuity) {
		usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], (TRN_CHRGTIP_ADR | STD_FORMATMASK_INDENT_4), pItem->lGratuity, MLD_PRECHECK_SYSTEM);
	}

    /* -- set response message text, Saratoga -- */
	if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0 && pItem->uchMinorClass != CLASS_TEND_TIPS_RETURN) {
		int i;
		for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
			usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
		}
	}

    if (!(CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) ) {
        memset(&alAttrib[0][0], 0x00, sizeof(alAttrib));
    }

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : VOID  MldTender(ITEMTENDER *pItem, USHORT usType):
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays tender operation
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on a few MDC settings.  In
*		order to comply with SR 131, which complies with pending Visa and
*		Mastecard merchant agreements, such account numbers can be masked
*		by configuring MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================
*/
static SHORT MldDriveTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll)
{
	TCHAR  aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_DR3COLUMN + 1] = { 0 }; /* print data save area */
    LONG   alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1];
	PARAMDC TenderMDC;  //Used to read MDC masking settings and Tender Key settings
	PARATENDERKEYINFO TendKeyInfo;
	USHORT	usMDCBase, usOffset;
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
	UCHAR	localMDCBitA, localMDCMask; //The following UCHARS are used in reading MDC settings of Tender Keys


	// Determine the correct Address to look in for the MDC
	// For Tenders 1-11, the address ranges from 180 to 190
	// For Tender 12-20., the address ranges from 403 to 411
	if (pItem->uchMinorClass <= CLASS_UITENDER11) {
		usMDCBase = MDC_CPTEND1_ADR;
		usOffset = CLASS_UITENDER1;
	} else if (pItem->uchMinorClass <= CLASS_UITENDER20) {
		usMDCBase = MDC_CPTEND12_ADR;
		usOffset = CLASS_UITENDER12;
	} else {
		NHPOS_ASSERT_TEXT(0, "MldDriveTender(): Out of Range Tender.");
		return 0;
	}

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = usMDCBase + (pItem->uchMinorClass - usOffset);
	ParaRead(&TenderMDC);

	localMDCBitA = (TenderMDC.usAddress % 2) ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = (TenderMDC.usAddress % 2) ? MDC_ODD_MASK : MDC_EVEN_MASK;

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItem->uchMinorClass;
	ParaRead(&TendKeyInfo);

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	//chech for a gift card and use gift card masking 
	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID || 
		TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY)
	{
		if(!ParaMDCCheck(MDC_GIFT_MASK, EVEN_MDC_BIT1))
		{
			int i;
			int iMaskedChars = 0;  //number of characters to mask
			int iShownChars = 0;   //number of characerst MDC indicates should not be masked
			PARAMDC MaskMDC;  //Used to read MDC masking settings and Tender Key settings

			MaskMDC.uchMajorClass = CLASS_PARAMDC;
			MaskMDC.usAddress = MDC_GIFT_MASK;
			ParaRead(&MaskMDC);			

			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			} else if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}
			//iMaskedChars represents the number of characters to mask with X's.
			//This number should be the total number of characters minus the number that should be shown.
			//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
			iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
			iMaskedChars = iMaskedChars ? iMaskedChars : 0;

			for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
				pItem->aszNumber[i] = _T('X');
			}
		}
	} else {
		if (pItem->uchMinorClass >= CLASS_TENDER12 && pItem->uchMinorClass <= CLASS_TENDER20)
		{
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA && ParaMDCCheck(MDC_EPT_MASK3_ADR, ODD_MDC_BIT0))
			{
				int i;
				int iMaskedChars = 0;   //number of characters to mask
				int iShownChars = 0;    //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				//Tenders 12-20 use MDC 443 for digit masking
				//Tenders less than 12 use MDC 378 for digit masking
				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK3_ADR;
				ParaRead( &MaskMDC );

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
				iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		} else if(pItem->uchMinorClass >= CLASS_TENDER2 && pItem->uchMinorClass <= CLASS_TENDER6)
		{
			//SR 131 cwunn 7/17/2003 Indicate how many account digits to show
			if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) && ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2))
			{
				int i;
				int iMaskedChars = 0;   //number of characters to mask
				int iShownChars = 0;    //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				//SR 131 cwunn 7/17/2003 Determine masking settings
				//Tenders 12-20 use MDC 443 for digit masking
				//Tenders less than 12 use MDC 378 for digit masking
				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

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
				iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		} else if(pItem->uchMinorClass >= CLASS_TENDER7 && pItem->uchMinorClass <= CLASS_TENDER11)
		{
			if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA && ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3))
			{
				int i;
				int iMaskedChars = 0;   //number of characters to mask
				int iShownChars = 0;    //number of characerst MDC indicates should not be masked
				PARAMDC MaskMDC = {0};  //Used to read MDC masking settings and Tender Key settings

				//Tenders 12-20 use MDC 443 for digit masking
				//Tenders less than 12 use MDC 378 for digit masking
				MaskMDC.uchMajorClass = CLASS_PARAMDC;
				MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
				ParaRead( &MaskMDC );

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
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for (i = 0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = _T('X');
				}
			}
		}
	}
    /* -- initialize the buffer -- */
    memset(aszScrollBuff[0], '\0', sizeof(aszScrollBuff));

    if ((pItem->fbModifier & VOID_MODIFIER) && (MldCheckReverseVideo())) {
        /* --- reverse video if void operation ---- */
        memset(&alAttrib[0][0], MLD_ATTRIB_REV_VIDEO, sizeof(alAttrib));
    } else {
        memset(&alAttrib[0][0], MLD_ATTRIB_STD_VIDEO, sizeof(alAttrib));
    }

    /* -- set void mnemonic and number -- */
    usScrollLine += MldVoidOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->fbModifier, MLD_DRIVE_THROUGH_3SCROLL);

    //SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		//always print offline symbol, EPT offline & exp.date
		usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_PRECHECK_SYSTEM);
	}
	else {	//if tender status indicates EPT
		if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
			usScrollLine += MldNumber(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszNumber, MLD_DRIVE_THROUGH_3SCROLL);
		}
	}

	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));

	//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
	//if tender status indicates EPT
	if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
		//then check if MDC indicates print expiration date on EPT Tender,
		if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
			//then print the expiration date, EPT offline & exp.date
			usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_DRIVE_THROUGH_3SCROLL);
		}
	}
	else {	//if tender status indicates CP
		if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit A, ept is not CP
			//then check if MDC indicates print expiration date on CP Tender, EPT offline & exp.date
			if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
				usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_DRIVE_THROUGH_3SCROLL);
			}
		}
		else { //tender status indicates non-EPT or CP tender, expiration date, EPT offline & exp.date
			usScrollLine += MldOffline(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->auchExpiraDate, pItem->auchApproval, MLD_DRIVE_THROUGH_3SCROLL);
		}
	}

    /* -- set room number and guest id -- */
    if ((pItem->aszRoomNo[0] != 0xff) && (pItem->aszRoomNo[0] != '\0') && (pItem->aszGuestID[0] != '\0')) {
        usScrollLine += MldCPRoomCharge(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszRoomNo, pItem->aszGuestID, MLD_DRIVE_THROUGH_3SCROLL);
    }

/*********
    usScrollLine += MldFolioPost(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszFolioNumber, pItem->aszPostTransNo, MLD_DRIVE_THROUGH_3SCROLL);
**********/

    if (pItem->uchMinorClass == CLASS_TEND_FSCHANGE) {
        if (pItem->lFSChange || pItem->lFoodStamp) {
			LONG   lFS = pItem->lFoodStamp / 100L;    // divide by 100 in order to truncate the amount to US dollars. Food Stamp is US specific.
            usScrollLine += MldFSMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_FSCHNG_ADR, lFS, pItem->lFSChange, MLD_DRIVE_THROUGH_3SCROLL);
        }
        if (pItem->lChange) {
            usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_DRIVE_THROUGH_3SCROLL);
        }
    } else {
        /* -- set tender mnemonic and amount -- */
        usScrollLine += MldSpecialMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], RflChkTendAdr(pItem),  pItem, MLD_DRIVE_THROUGH_3SCROLL);

        /* -- set change mnemonic and amount -- */
        if (pItem->lChange) {
            usScrollLine += MldMnemAmt(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], TRN_REGCHG_ADR, pItem->lChange, MLD_DRIVE_THROUGH_3SCROLL);
        }
    }

    /* -- set response message text, Saratoga -- */
	if ((pItem->fbModifier & RETURNS_ORIGINAL) == 0) {
		int i;
		for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
		   usScrollLine += MldCPDriveRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_DRIVE_THROUGH_3SCROLL);
		}
	}

    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_DR3COLUMN, &alAttrib[0][0]));
}


/*
*===========================================================================
** Format  : VOID  MldTender(ITEMTENDER *pItem, USHORT usType):
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays tender operation
*===========================================================================
*/
SHORT MldTender(ITEMTENDER *pItem, USHORT usType, USHORT usScroll)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        return (MldDriveTender(pItem, usType, usScroll));
    } else {
        return (MldPrechkTender(pItem, usType, usScroll));
    }
}
/***** End Of Source *****/
