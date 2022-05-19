/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTEND1.C (without amount tender)
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
:  Abstract:
:   ItemTendFast()          ;   w/o amount tender
:
:  ---------------------------------------------------------------------
:  Update Histories
:    Date  : Ver.Rev. :   Name      : Description
: 2/14/95  : 03.00.00 :   hkato     : R3.0
:11/10/95  : 03.01.00 :   hkato     : R3.1
:  8/6/98  : 03.03.00 :   hrkato    : V3.3
: 8/17/99  : 03.05.00 :   K.Iwata   : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: 12/6/999 : 01.00.00 :   hrkato    : Saratoga
: 04/22/15 : 02.02.01 : R.Chambers  : cleanup source, localize variables
: 07/15/15 : 02.02.01 : R.Chambers  : VCS Print on Demand receipt printing fixes
: 07/28/17 : 02.02.02  : R.Chambers : removed unneeded include files, localized variables
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "display.h"

#include    "paraequ.h"
#include    "para.h"
#include    "mld.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "csttl.h"
#include    "rfl.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "cpm.h"
#include    "itmlocal.h"
#include    "eeptl.h"
#include    "eept.h"
#include	"fsc.h"
#include "..\SA\UIE\uieerror.h"
#include "uireg.h"
#include "..\Uifreg\uiregloc.h"
#include    <trans.h>
#include	<prt.h>
#include "pifmain.h"
#include	"EEptTrans.h"
#include	"appllog.h"

#include "ej.h"
#include	"csstbej.h"
#include    "..\Print\IncludeTherm\prtrin.h"
#include "pmg.h"
#include    "..\Print\IncludeTherm\prrcolm_.h"
#include "prt.h"
#include "maint.h"
#include "..\Maint\maintram.h"
#include "csstbcas.h"
#include <csstbfcc.h>
#include <csstbept.h>
#include <bl.h>
#include <report.h>
#include <csgcs.h>
#include "ConnEngineObjectIf.h"

static VOID    ItemTendAffectTransFromTo(ITEMAFFECTION *pAffect);  /* V3.3 ICI */

VOID ItemEptMsgTextCopyReplace (TCHAR *pDest, TCHAR *pSource, int iMaxDestLen)
{
	TCHAR  *pP20String = _T("#P20.");
	int    iLen = 0, iLoop;
	TCHAR  *pDigit = pSource;

	*pDest = 0;
	iLoop = 0;
	if (_tcsncmp (pSource, pP20String, (iLen = _tcslen(pP20String))) == 0) {
		// this is a P20 transaction mnemonic.  next three digits indicates which one.
		// format will be #P20.nnn where nn is the mnemonic number
        USHORT  usP20Address = 0;

		pDigit = pSource + iLen;

		if (*pDigit >= _T('0') && *pDigit <= _T('9')) {
			PARATRANSMNEMO  ParaTransMnemoLD;

			usP20Address = usP20Address * 10 + (*pDigit++ - _T('0'));
			iLen++;
			if (*pDigit >= _T('0') && *pDigit <= _T('9')) {
				usP20Address = usP20Address * 10 + (*pDigit++ - _T('0'));
				iLen++;
				if (*pDigit >= _T('0') && *pDigit <= _T('9')) {
					usP20Address = usP20Address * 10 + (*pDigit++ - _T('0'));
					iLen++;
				}
			}

			ParaTransMnemoLD.uchMajorClass =  CLASS_PARATRANSMNEMO;
			ParaTransMnemoLD.uchAddress = usP20Address;
			CliParaRead( &ParaTransMnemoLD );
			_tcsncpy( pDest, ParaTransMnemoLD.aszMnemonics, NUM_TRANSMNEM);
			iLoop = _tcslen (pDest);
			iMaxDestLen -= iLoop;

			if (usP20Address == TRN_DSI_TERMINAL ) {
				// special P20 address that indicates to put in the terminal id
				CHAR  aszTerminalId[NUM_CPRSPTEXT];

				if (ParaTendReadTrainTripIniFile ("TERMID", aszTerminalId, NUM_CPRSPTEXT) >= 0) {
					for (iLen = 0; iLoop < iMaxDestLen && aszTerminalId[iLen]; iLoop++, iLen++) {
						pDest[iLoop] = aszTerminalId[iLen];
					}
					iLen = iMaxDestLen;  // indicate there is nothing else to copy from pSource
				} else {
					iLoop = 0;           // empty line with no terminal id
					pDest[0] = 0;        // empty line with no terminal id
					iLen = iMaxDestLen;  // indicate there is nothing else to copy from pSource
				}
			}
		}
	} else {
		iLen = 0;
	}

	for (; iLoop < iMaxDestLen && iLen < iMaxDestLen; iLoop++, iLen++) {
		pDest[iLoop] = pSource[iLen];
	}
	for ( ; iLoop < iMaxDestLen; iLoop++) {
		pDest[iLoop] = _T(' ');
	}
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTendFast(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    w/o amount tender
==========================================================================*/

SHORT   ItemTendFast(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) != 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);
    }

    /*----- During Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2 | TRN_SPL_SPLIT)) {
        return(ItemTendFastSpl(UifRegTender, ItemTender));
    } else {
        return(ItemTendFastNor(UifRegTender, ItemTender));
    }
}

/*
*===========================================================================
** Synopsis:    VOID ItemTendGiftDupRec()
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      
*
** Description: Checks whether or not to print a duplicate receipt on a gift
*				card tender.
*===========================================================================
*/
VOID	ItemTendGiftDupRec(UIFREGTENDER *UifRegTender)
{

	// Check to make sure that the gift card type is tender even though
	// we may have done this same check before the function ItemTendGiftDupRec
	// was called.
	if((UifRegTender->GiftCardOperation == GIFT_CARD_TENDER) && CliParaMDCCheck(MDC_ENHEPT_ADR,ODD_MDC_BIT2))		// print dup. receipt if MDC 361
	{
		ITEMOTHER   WorkOther = {0};

		//Receipt Printed on Previously
		if (SetReceiptPrint(SET_RECEIPT_SECOND_COPY) < 0) {
			return ;
		}

		WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
        WorkOther.uchMinorClass = CLASS_CP_EPT_FULLDUP;     /* 5/30/96 */
        WorkOther.fsPrintStatus = PRT_RECEIPT;          /* only receipt     */
        TrnItem( &WorkOther );                      /* print trans.     */
	}
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFastNor(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    w/o amount tender of normal.
==========================================================================*/
VOID  ItemTendSetPrintStatus (CONST ITEMTENDER *pItemTender)
{
	ULONG			ulMinTotal;
	PARAMISCPARA	ParaMiscPara;
	PARATENDERKEYINFO TendKeyInfo;
    TRANCURQUAL   *pCurQualRcvBuff = TrnGetCurQualPtr();

	NHPOS_ASSERT(pItemTender->uchMinorClass != 0);

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItemTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if (pItemTender->uchMinorClass >= CLASS_FOREIGN1 && pItemTender->uchMinorClass <= CLASS_FOREIGN1) {
		// This is actually a Foreign Currency Cash Tender so we treat it as cash
		SetReceiptPrint(0);
		TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_CHARGE;
	} else if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID) {
		SetReceiptPrint(SET_RECEIPT_TRAN_PREPAID);
	} else if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHARGE || TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK) {
		// This is actually a Cash or a Check Tender
		SetReceiptPrint(0);
	} else {
		SetReceiptPrint(SET_RECEIPT_TRAN_EPT);
	}


	// Credit Card Floor amount check	***PDINU
	// If this is a credit type of transaction then we will do the following:
	//   Read the credit floor parameter from AC128
	//   If the floor limit check is enabled then perform a floor limit check
	//   If not then do the normal transaction close and print procedure.
	memset (&ParaMiscPara, 0, sizeof(ParaMiscPara));
	ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
	ParaMiscPara.uchAddress = MISC_CREDITFLOOR_ADR;	// Address for where the floor amount is stored
	ParaMiscParaRead(&ParaMiscPara);
	ulMinTotal = ParaMiscPara.ulMiscPara;
	ParaMiscPara.uchAddress = MISC_CREDITFLOORSTATUS_ADR;	// Address for where the status bits are stored
	ParaMiscParaRead(&ParaMiscPara);

	pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;     // clear the indicator and set it back below if still necessary
	pCurQualRcvBuff->fbNoPrint &= ~(CURQUAL_NO_SECONDCOPY);     // assume we are doing two receipts for now.
	pCurQualRcvBuff->fbNoPrint &= ~(CURQUAL_NO_R);              // assume we want at least one receipt for now.
	pCurQualRcvBuff->fbNoPrint &= ~(CURQUAL_NO_EPT_RESPONSE);   // assume we want ept response for now.
	SetReceiptPrint(SET_RECEIPT_FIRST_COPY);

	if ((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) && (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTH)) {
		// if this is a Preauth then we need to have the total, tip, and signature lines regardless.
		return;
	}

	// disable printing of a second copy for TENDERKEY_TRANTYPE_ and override MDC 100 Bit B setting
	// MDC 100 Bit B (MDC_CPPARA2_ADR, EVEN_MDC_BIT2) allows/disallows printing of items on the duplicate copy
	// the printing logic for not printing duplicate receipt basically just prints the second one only.
//	pCurQualRcvBuff->fsCurStatus &= ~CURQUAL_PRINT_EPT_LOGO;
	if ((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) || (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD))
	{
		if(TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
			pCurQualRcvBuff->fsCurStatus &= ~CURQUAL_CUST_DUP;
			pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);              // disable printing of a second copy if pre-auth capture tender.
			pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;   // EPT logo is not needed for the Pre-auth capture tender
		} else if (ParaMiscPara.ulMiscPara & A128_CC_ENABLE) {
			if ((RflLLabs(pItemTender->lTenderAmount) > (LONG)ulMinTotal))
			{
				pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;  // above AC128 threshold so we want the EPT logo
				if (ItemTenderCheckTenderMdc(pItemTender->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_D)) {
					if ((pCurQualRcvBuff->fsCurStatus & CURQUAL_MERCH_DUP) != 0) {
						SetReceiptPrint(SET_RECEIPT_SECOND_COPY);        // first copy was Merchant and not printing it so change to Customer if allowed
					} else {
						pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
					}
				}
			} else
			{
				ITEMMODLOCAL  *pModLocalDataRcvBuff = ItemModGetLocalPtr();
				TRANMODEQUAL  *pModeQualRcvBuff = TrnGetModeQualPtr();

				if ((pCurQualRcvBuff->fsCurStatus & CURQUAL_MERCH_DUP) != 0) {
					// First receipt to print is the Merchant copy.  See if we are allowed to print it.
					// If not then switch to the Customer copy and see if we are allowed to print that.
					// If not then no receipt printing.
					if ((ParaMiscPara.ulMiscPara & A128_CC_MERC) != 0)	//Do not print merchant receipt   ***PDINU
					{
//						pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						SetReceiptPrint(SET_RECEIPT_SECOND_COPY);        // first copy was Merchant and not printing it so change to Customer if allowed
						if ((ParaMiscPara.ulMiscPara & A128_CC_CUST) != 0)	 //Do not print customer receipt?   ***PDINU
						{
							pCurQualRcvBuff->fsCurStatus &= ~CURQUAL_CUST_DUP;      // just incase customer is turn on, turn it off
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						} else {
							// If we are printing a customer copy check to see if we should print card authorization text
							if ((ParaMiscPara.ulMiscPara & A128_CC_CUST_AUTH) != 0)	//Do not print customer authorization receipt   ***PDINU
							{
								pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_EPT_RESPONSE);             // disable printing of a second copy.
							}
						}
					} else {
						if ((ParaMiscPara.ulMiscPara & A128_CC_CUST) != 0)	//Do not print customer receipt   ***PDINU
						{
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						}
						// If we are printing a merchant copy check to see if we should print card authorization text
						if ((ParaMiscPara.ulMiscPara & A128_CC_MERC_AUTH) != 0)	//Do not print merchant receipt authorization text
						{
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_EPT_RESPONSE);             // disable printing of a second copy.
						}
					}
				} else if ((pCurQualRcvBuff->fsCurStatus & CURQUAL_CUST_DUP) != 0) {
					// First receipt to print is the Customer copy.  See if we are allowed to print it.
					// If not then switch to the Merchant copy and see if we are allowed to print that.
					// If not then no receipt printing.
					if ((ParaMiscPara.ulMiscPara & A128_CC_CUST) != 0)	 //Do not print customer receipt   ***PDINU
					{
//						pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						SetReceiptPrint(SET_RECEIPT_SECOND_COPY);        // first copy was Customer and not printing it so change to Merchant if allowed
						if ((ParaMiscPara.ulMiscPara & A128_CC_MERC) != 0)	 //Do not print merchant receipt?   ***PDINU
						{
							pCurQualRcvBuff->fsCurStatus &= ~CURQUAL_MERCH_DUP;     // just incase merchant is turn on, turn it off
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						} else {
							// If we are printing a merchant copy check to see if we should print card authorization text
							if ((ParaMiscPara.ulMiscPara & A128_CC_MERC_AUTH) != 0)	//Do not print merchant authorization receipt   ***PDINU
							{
								pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_EPT_RESPONSE);             // disable printing of a second copy.
							}
						}
					} else {
						if ((ParaMiscPara.ulMiscPara & A128_CC_MERC) != 0)	//Do not print merchant receipt   ***PDINU
						{
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);             // disable printing of a second copy.
						}
						// If we are printing a customer copy check to see if we should print card authorization text
						if ((ParaMiscPara.ulMiscPara & A128_CC_CUST_AUTH) != 0)	//Do not print merchant receipt   ***PDINU
						{
							pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_EPT_RESPONSE);             // disable printing of a second copy.
						}
					}
				} else {
					// if not printing either type of receipt then just indicate not dup
					// there appear to be issues with the use of flags so we are just using a new one
					// I do not like this and we really should straighten this mess out.  Richard Chambers, May 19, 2011
					pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);              // disable printing of a second copy.
				}

				if ((ParaMiscPara.ulMiscPara & A128_CC_MERC) != 0 && (ParaMiscPara.ulMiscPara & A128_CC_CUST) != 0)	{
					pCurQualRcvBuff->fsCurStatus &= ~(CURQUAL_CUST_DUP | CURQUAL_MERCH_DUP);   // just incase turn off both types of receipts
					pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_R);                              // do not print a receipt.
					pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);              // disable printing of a second copy.
				}

				if (CliParaMDCCheckField (MDC_RCT_PRT_ADR, MDC_PARAM_BIT_C) == 0) {
					pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;
				}

				// if the Receipt modifier key has been pressed then look to see how we should
				// interpret the indicator, whether as normal or inverted depending on MDC 57.
				// Electronic Payment receipts use their own logic for print or not.
				if ( pModeQualRcvBuff->fsModeStatus & MODEQUAL_NORECEIPT_DEF ) {
					pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_R;       /* MDC says otherwise */
				}
				if ( pModLocalDataRcvBuff->fbModifierStatus & MOD_NORECEIPT ) {
					pCurQualRcvBuff->fbNoPrint ^= CURQUAL_NO_R;       /* override by Receipt key press so invert */
				}

				if ((pCurQualRcvBuff->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN | CURQUAL_DUPLICATE_COPY)) != 0) {
					pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;  // if a transaction return or transaction void, print anyway.
				}
			}
		}
	} else {
		ITEMMODLOCAL  *pModLocalDataRcvBuff = ItemModGetLocalPtr();
		TRANMODEQUAL  *pModeQualRcvBuff = TrnGetModeQualPtr();

		// if the Receipt modifier key has been pressed then look to see how we should
		// interpret the indicator, whether as normal or inverted depending on MDC 57.
		// Electronic Payment receipts use their own logic for print or not.
		pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;       /* assume we print receipt unless MDC says otherwise */
		if ( pModeQualRcvBuff->fsModeStatus & MODEQUAL_NORECEIPT_DEF ) {
			pCurQualRcvBuff->fbNoPrint |= CURQUAL_NO_R;       /* MDC says otherwise */
		}
		if ( pModLocalDataRcvBuff->fbModifierStatus & MOD_NORECEIPT ) {
			pCurQualRcvBuff->fbNoPrint ^= CURQUAL_NO_R;       /* override by Receipt key press so invert */
		}

		if ((pCurQualRcvBuff->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN | CURQUAL_DUPLICATE_COPY)) != 0) {
			pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;  // if a transaction return or transaction void, print anyway.
		}
	}

	if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK || TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHARGE) {
		// disable printing of a second copy for TENDERKEY_TRANTYPE_CHECK or for TENDERKEY_TRANTYPE_CHARGE
		// and override MDC 100 Bit B setting.  (TENDERKEY_TRANTYPE_CHARGE is a Cash tender).
		// MDC 100 Bit B (MDC_CPPARA2_ADR, EVEN_MDC_BIT2) allows/disallows printing of items on the duplicate copy
		// the printing logic for not printing duplicate receipt basically just prints the second one only.
		pCurQualRcvBuff->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);
	}
	if (ItemTenderCheckTenderMdc(pItemTender->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_B)) {
		pCurQualRcvBuff->fsCurStatus2 |= PRT_DUPLICATE_COPY;
		if (pCurQualRcvBuff->fsCurStatus2 & (PRT_DEMAND_COPY | PRT_GIFT_RECEIPT | PRT_POST_RECEIPT)) {
			pCurQualRcvBuff->fsCurStatus2 &= ~PRT_DUPLICATE_COPY;
		}
		if (pCurQualRcvBuff->fsCurStatus2 & PRT_DUPLICATE_COPY) {
			pCurQualRcvBuff->fbNoPrint &= ~CURQUAL_NO_R;    /* tender MDC says print a receipt anyway */
		}
	}
}

SHORT   ItemTendFastNor(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    ULONG           fsCurStatus;
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();
	PARATENDERKEYINFO TendKeyInfo;

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {/* FS Tender */
        ItemTendSendFS();                                       /* FS total */
    } else {
		ITEMTENDER      DummyTender = {0};
        if ( TranCurQualPtr->auchTotalStatus[0] == 0 ) {            // if not total key pressed then
			ITEMTOTAL       ItemTotal = {0};
            ItemTendAffectTax(UifRegTender);                        /* tax */
            ItemPreviousItem(&DummyTender, 0 );                     /* Saratoga */
            ItemTendTrayTotal();                                    /* tray total */
            ItemTendTrayTax();                                      /* whole tax at tray transaction */
            ItemTendVAT(UifRegTender);
            ItemTendCheckTotal(&ItemTotal, UifRegTender);
            /* add current total with multiple check payment amount  07/01/96 */
            MldDispSubTotal(MLD_TOTAL_1, ItemTotal.lMI + ItemTransLocalPtr->lWorkMI);    /* R3.0 */
			/* if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) { */
				/* for check endorsement calculation */
				ItemTenderLocal.ItemTotal = ItemTotal;
			/* } */
        } else {                                                    /* exist check total */
            ItemTendAffectTax(UifRegTender);                        /* tax */
            ItemTendTrayTotal();                                    /* tray total */
            ItemTendTrayTax();                                      /* whole tax at tray transaction */
            ItemTendVAT(UifRegTender);
            /*--- Display Total Key, R3.0 ---*/
            if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {
				ITEMCOMMONLOCAL *WorkCommon = ItemCommonGetLocalPointer();

				MldScrollWrite(&WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
                ItemSendKds(&WorkCommon->ItemSales, 0);             /* send to enhanced kds, 2172 */
            }
            ItemPreviousItem(&DummyTender, 0 );                     /* Saratoga */
        }
    }

    ItemTendMulti();                                            /* print data for multi check */
  
	ItemTendSetPrintStatus (ItemTender);

	ItemTendFastTender( UifRegTender, ItemTender );             /* tender */

	DisplayGiftCard();

    if ( CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);             /* drawer open      */
    }

    ItemTendAffection();                                        /* affection        */

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

    //TrnGetCurQual(&WorkCur);
    fsCurStatus = pWorkCur->fsCurStatus;                             /* Saratoga */
    /* if (!(fsStatus & CURQUAL_PVOID)) { */
        ItemTendPrintFSChange();
    /* } */

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	{
		BOOL  bPrintEptLogo = FALSE;

        /* exist EPT tend ? */
		bPrintEptLogo = ((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT) && CliParaMDCCheckField(MDC_CPPARA2_ADR, MDC_PARAM_BIT_C));
		bPrintEptLogo = bPrintEptLogo && ((pWorkCur->fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0);
		bPrintEptLogo = bPrintEptLogo || ((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID) && CliParaMDCCheckField(MDC_ENHEPT_ADR, MDC_PARAM_BIT_A));
		bPrintEptLogo = bPrintEptLogo || (ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_CPPARTIAL));
		bPrintEptLogo = bPrintEptLogo && !((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) != 0) || ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) != 0);
		bPrintEptLogo = bPrintEptLogo && !((ItemTenderLocal.fbTenderStatus[1] & (TENDER_FS_PARTIAL | TENDER_SPLIT_TENDER)) != 0);
		bPrintEptLogo = bPrintEptLogo && (TendKeyInfo.TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_CHARGE);

		if (bPrintEptLogo) {   /* EPT logo */
			ItemTrailer( TYPE_EPT );                                /* EPT trailer      */
		} else {
			SetReceiptPrint(0);
			ItemTrailer( TYPE_STORAGE );                            /* normal trailer   */
		}
	}

	ItemTendClose();
    ItemTendDelGCF();                                           /* delete GCF */

	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID)		// if tender type is a gift card see if we need to print a duplicate receipt
	{		
		ItemTendGiftDupRec(UifRegTender);
	} else if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CREDIT && TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
		;  // if this is a Preauth Capture then no duplicate receipt needed.
	} else {
		ItemTendDupRec();										    /* Print the customer receipt */
	}

    if (!(fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN))) {
        ItemTendFSChit();                                       /* generate F.S chit */
    }
    ItemTendSaved(ItemTender);                                  /* set saved MI */
    ItemTendInit();                                            /* initialize transaction information */
    ItemWICRelOnFin(fsCurStatus);
    return(ItemFinalize());                                     /* tender finalize */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendFastCheck( UIFREGTENDER *UifRegTender,
*                   ITEMTENDER  *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender,  ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    check w/o amount tender
==========================================================================*/

SHORT   ItemTendFastCheck( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
	// Check to see if we are requiring amount entry for this tender key.  If so then prohibit fast tender
	// since fast tender does it for the amount of transaction without requiring amount entry.
	if ( ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_1, MDC_PARAM_BIT_D )
		&& ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_1, MDC_PARAM_BIT_C ) == 0 ) {

		if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {   /* preselect void */
//			if ( ItemTender->lRound >= 0L ) {                              /* audaction */
				if ( UifRegTender->uchMinorClass == CLASS_UITENDER1 ) {
					return( ITM_SUCCESS );
				}
//			}
		} else {
			if ( ItemTender->lRound <= 0L ) {                           /* audaction */
				if ( UifRegTender->uchMinorClass == CLASS_UITENDER1 ) { /* cash tender*/
					return( ITM_SUCCESS );
				}
			}
		}
		return( LDT_PROHBT_ADR );
	}

    if ( ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL ) {
        if ( ItemModLocalPtr->fsTaxable & ~MOD_CANADAEXEMPT ) {
            return( LDT_SEQERR_ADR );                               /* sequence error */
        }
    }
                                                                    /* R3.0 */
    if ( CliParaMDCCheck( MDC_PSN_ADR, EVEN_MDC_BIT1 )              /* No person at total key */
        && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER
        && ItemModLocalPtr->usCurrentPerson == 0 ) {

        return( LDT_NOOFPRSN_ADR );                                 /* Enter No. of Person */
    }

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendSPVInt( ITEMTENDER *ItemTender )
*
*   Input:      ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    supervisor intervention
==========================================================================*/

SHORT   ItemTendSPVInt( ITEMTENDER *pItemTender )
{
    DCURRENCY   lMI = pItemTender->lRound;    /* whole sales amount */

    if( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {   /* preselect void */
        lMI *= -1;
    }

	if((lMI == 0L) && (TranCurQualPtr->fsCurStatus2 & CURQUAL_NOTALLOW_0_TEND) == 0)
	{
		return(ITM_SUCCESS);
	}

	if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
		// This is  a return transaction so do not check the negative total MDC
        return( ITM_SUCCESS );
	}

    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT2 ) == 0 || lMI > 0L ) {
        return( ITM_SUCCESS );
    }

    return( ItemSPVInt(LDT_SUPINTR_ADR) );
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendAffectTax(UIFREGTENDER *UifRegTender)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax data                           V3.3
*==========================================================================
*/
VOID    ItemTendAffectTax(UIFREGTENDER *pUifRegTender)
{
	ITEMAFFECTION   ItemAffection = {0};

    if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0
        && ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) == 0
        && ItemCommonTaxSystem() != ITM_TAX_INTL)
	{
		USCANTAX        *pTax = &ItemAffection.USCanVAT.USCanTax;
		TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
		DCURRENCY       lTax = 0L, lWholeTax = 0L;
		DCURRENCY       lWholePrtTax = 0L;              /* 5/10/96 */
		ITEMCOMMONTAX   WorkTax = {0};
		SHORT           i;
		UCHAR           auchTotalStatus[NUM_TOTAL_STATUS];

        ItemTendGetTotal(&auchTotalStatus[0], pUifRegTender->uchMinorClass);

        /*----- generate affection tax -----*/
        ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;              /* major class */
        ItemAffection.uchMinorClass = CLASS_TAXAFFECT;

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax( &auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT );
#pragma message("Remove this check once we determine TH ISSUE JHHJ \z added to cause compiler warning.")
			if(WorkTax.alTaxable[1] > 100000)
			{
				// Tim Horton's of Canada was having a problem with a tax calculation error
				// which would suddenly happen and then not be seen for a while.
				// This was back in Rel 2.1.0 around 2007 or so.  They are no longer a customer.
				//   Richard Chambers, Apr-09-2015
				NHPOS_ASSERT(!"ItemTendAffectTax TAX ERROR");
			}
            ItemCanTaxCopy1(&ItemAffection, &WorkTax);
            if ( auchTotalStatus[1] != 0 ) {    // is there any Canadian tax that needs to be affected?
                TrnItem( &ItemAffection );      /* transaction module i/F */
            }

        } else if (ItemCommonTaxSystem() == ITM_TAX_US)
		{               /* V3.3 */
			//SR 279 JHHJ
			//This if statement now allows the user to decide whether or not to recalculate
			//tax when two checks are added together.

			if((TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK))
			 // we are in multiple check mode.
			{
				if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1)
					|| 	((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK))//one cent tax
				{
					ItemCurTax( &auchTotalStatus[1], &WorkTax );
					ItemUSTaxCopy1(&ItemAffection, &WorkTax);
				}
				//       ITM_TTL_FLAG_TAX_1             ITM_TTL_FLAG_TAX_2              ITM_TTL_FLAG_TAX_3
				//       CURQUAL_TOTAL_FLAG_TAX_1       CURQUAL_TOTAL_FLAG_TAX_2        CURQUAL_TOTAL_FLAG_TAX_3
				if ( (auchTotalStatus[1] & 0x01) || (auchTotalStatus[1] & 0x10) || (auchTotalStatus[2] & 0x01) ) {     // is there any US tax that needs to be affected?
					TrnItem( &ItemAffection );                              /* transaction module i/F */
				}
			}else
			{
				ItemCurTax( &auchTotalStatus[1], &WorkTax );
				ItemUSTaxCopy1(&ItemAffection, &WorkTax);
				//       ITM_TTL_FLAG_TAX_1             ITM_TTL_FLAG_TAX_2              ITM_TTL_FLAG_TAX_3
				//       CURQUAL_TOTAL_FLAG_TAX_1       CURQUAL_TOTAL_FLAG_TAX_2        CURQUAL_TOTAL_FLAG_TAX_3
				if ( (auchTotalStatus[1] & 0x01) || (auchTotalStatus[1] & 0x10) || (auchTotalStatus[2] & 0x01) ) {     // is there any US tax that needs to be affected?
					TrnItem( &ItemAffection );                              /* transaction module i/F */
				}
			}
        }

        /* send to enhanced kds, 2172 */
        ItemSendKds( &ItemAffection, 0);

        /*--- Display Tax, R3.1 ---*/
        if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER
            || (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {

            /*----- Make Subtotal,   R3.1 96/4/3 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                ItemAffection.lAmount = WorkTI->lMI;
            }

            ItemAffection.uchMinorClass = CLASS_TAXPRINT;

            if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
                /*----- adjust tax print format 4/25/96 -----*/
                ItemCanTaxCopy2(&ItemAffection);
            }
            MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&ItemAffection);
        }

        lTax = pTax->lTaxAmount[0] + pTax->lTaxAmount[1] + pTax->lTaxAmount[2] + pTax->lTaxAmount[3];
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
            WorkTI->lHourly += lTax;
        }

        /*----- generate print tax -----*/
        ItemAffection.uchMinorClass = CLASS_TAXPRINT;
        ItemAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if (ItemCommonTaxSystem() == ITM_TAX_US                  /* V3.3 */
            && TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK                /* postcheck, store/recall */
            && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK ) {

            for( i = 0; i < 3; i++ ) {                                  /* update affect taxable */
                pTax->lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
                pTax->lTaxAmount[i] += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
                lWholeTax += pTax->lTaxAmount[i];
                pTax->lTaxExempt[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxExempt[i];
                lWholePrtTax += WorkTI->Itemizers.TranUsItemizers.lPrintTax[i];
            }

            if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
                for( i = 0; i != 3; i++ ) {                             /* update affect taxable */
                    pTax->lTaxable[i] -= WorkTI->Itemizers.TranUsItemizers.lPrintTaxable[i];
                    pTax->lTaxAmount[i] -= WorkTI->Itemizers.TranUsItemizers.lPrintTax[i];
                    pTax->lTaxExempt[i] -= WorkTI->Itemizers.TranUsItemizers.lPrintTaxExempt[i];
                }
            }
        }

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            if ( TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK              /* postcheck, store/recall */
            && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK ) {
                if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {
                    pTax->lTaxExempt[0] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
                } else {
                    pTax->lTaxable[0] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[0];
                }
                if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
                    for ( i = 0; i < 3; i++ ) {
                        pTax->lTaxExempt[i + 1] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i + 1];
                    }
                } else {
                    for ( i = 0; i < 3; i++ ) {                                 /* save Canadian tax itemizers */
                        pTax->lTaxable[i+1] += WorkTI->Itemizers.TranCanadaItemizers.lAffTaxable[i+1];
                    }
                }

                for ( i = 0; i < 4; i++ ) {                                     /* save Canadian tax itemizers */
                    pTax->lTaxAmount[i] += WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i];
                    lWholeTax += pTax->lTaxAmount[i];
                    lWholePrtTax += WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i];
                }

                if ( ( TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) {
                    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT ) {
                        pTax->lTaxExempt[0] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
                    } else {
                        pTax->lTaxable[0] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[0];
                    }
                    if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {
                        for ( i = 0; i < 3; i++ ) {
                            pTax->lTaxExempt[i + 1] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i + 1];
                        }
                    } else {
                        for ( i = 0; i < 3; i++ ) {                            /* save Canadian tax itemizers */
                            pTax->lTaxable[i+1] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTaxable[i + 1];
                        }
                    }

                    for( i = 0; i < 4; i++ ) {                              /* update affect taxable */
                        pTax->lTaxAmount[i] -= WorkTI->Itemizers.TranCanadaItemizers.lPrtTax[i];
                    }
                }
            }
            ItemCanTaxCopy2(&ItemAffection);
        }

        /*----- Make Subtotal,   R3.1 4/25/96 -----*/
        if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
            ItemAffection.lAmount = WorkTI->lMI - lWholeTax;

            /*----- check postcheck, store/recall or not 96/4/25 -----*/
            if ( TranCurQualPtr->flPrintStatus & CURQUAL_POSTCHECK
            && ( TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_ADDCHECK ) {
                if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) { /* 5/6/96 */
                    ItemAffection.lAmount += (lWholePrtTax +lTax);  /* 5/10/96 */
                } else {
                    ItemAffection.lAmount += lTax;          /* soft check */
                }
            }

            /*----- check split system or not 96/4/4 -----*/
            if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
                if (!CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
                    if (!CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)) {
                        ItemAffection.lAmount = WorkTI->lMI;
                    }
                }
            }
        }

        TrnItem( &ItemAffection );                                      /* transaction module i/F */

        /*--- Display Tax, R3.1 ---*/
        if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
            || (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
            MldScrollWrite(&ItemAffection, MLD_NEW_ITEMIZE);
            MldScrollFileWrite(&ItemAffection);
        }
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendTrayTotal( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate various tray total data
==========================================================================*/

VOID    ItemTendTrayTotal( VOID )
{
    if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {                         /* tray total transaction */
		ITEMTOTALLOCAL  *pWorkTotal = ItemTotalGetLocalPointer();

        if ( ! ( ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE )              /* itemize state */
            && ! (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {/* partial tender */

            ItemPromotion( PRT_RECEIPT, TYPE_TRAY );
            ItemHeader( TYPE_STORAGE ) ;
        } else {
            pWorkTotal->usNoPerson++;
        }

		if ( (ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)                      /* itemize state */
			&& ! (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)) {    /* partial tender */

			UIFREGTOTAL     UifRegTotal = {0};
			ITEMTOTAL       ItemTotal = {0};
			ITEMCOMMONTAX   WorkTax = {0};
			ITEMCOMMONVAT   WorkVAT = {0};
			ITEMAFFECTION   ItemAffectTax = {0}, ItemPrintTax = {0};

			UifRegTotal.uchMinorClass = pWorkTotal->uchSaveMinor;             /* for tax calculation */

			ItemTotal.uchMajorClass = pWorkTotal->uchSaveMajor;
			ItemTotal.uchMinorClass = pWorkTotal->uchSaveMinor;
			ItemTotal.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                       /* print journal */

			ItemTendTTTax(&UifRegTotal, &ItemTotal, &WorkTax, &WorkVAT);    /* calculate tax */
			ItemTotalTTAmount( &ItemTotal );
			ItemTotalTTTaxSave(&WorkTax, &ItemAffectTax, &ItemPrintTax, &WorkVAT);

			if (ItemCommonTaxSystem() == ITM_TAX_INTL) {                    /* V3.3 */
				ItemTendVATCommon(&ItemAffectTax, ITM_NOT_SPLIT, ITM_DISPLAY);
			} else {
				TrnItem( &ItemAffectTax );                                      /* transaction module i/F */
				TrnItem( &ItemPrintTax );                                       /* transaction module i/F */

				/* send to enhanced kds, 2172 */
				ItemSendKds(&ItemAffectTax, 0);

				MldScrollWrite(&ItemPrintTax, MLD_NEW_ITEMIZE);
				MldScrollFileWrite(&ItemPrintTax);
			}

			TrnItem( &ItemTotal );                                          /* transaction module i/F */

			/* send to enhanced kds, 2172 */
			ItemSendKds(&ItemTotal, 0);

			/*--- Display Total Key, R3.0 ---*/
			MldScrollWrite(&ItemTotal, MLD_NEW_ITEMIZE);
			MldScrollFileWrite(&ItemTotal);
			MldDispSubTotal(MLD_TOTAL_1, ItemTotal.lMI);

			ItemTotalTTSave(&ItemTotal, &WorkTax, &WorkVAT);
		}
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendTTTax( UIFREGTOTAL *UifRegTotal,
*                           *ITEMTOTAL *ItemTotal, ITEMCOMMONTAX *WorkTax )
*
*   Input:      UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal,
*               ITEMCOMMONTAX   *WorkTax )
*   Output:     ITEMTOTAL   *ItemTotal, ITEMCOMMONTAX   *WorkTax
*   InOut:      none
*
*   Return:
*
*   Description:    calculate tax
=========================================================================*/

SHORT   ItemTendTTTax(UIFREGTOTAL *UifRegTotal, ITEMTOTAL *ItemTotal,
            ITEMCOMMONTAX *WorkTax, ITEMCOMMONVAT *WorkVAT)
{
    SHORT           sStatus;
    DCURRENCY       lTax;
    PARATTLKEYTYP   WorkType;

    WorkType.uchMajorClass = CLASS_PARATTLKEYTYP;
	//Necessary because we now have more than just 6 available totals to be used. JHHJ
	if(UifRegTotal->uchMinorClass <= CLASS_UITOTAL8)
	{
		WorkType.uchAddress = ( UCHAR )( TLTY_NO3_ADR + UifRegTotal->uchMinorClass - CLASS_UITOTAL3 );
	} else
	{
		WorkType.uchAddress = ( UCHAR )( UifRegTotal->uchMinorClass - CLASS_UITOTAL3);
	}
    CliParaRead( &WorkType );                                           /* get target total key */
    ItemTotal->auchTotalStatus[0] = WorkType.uchTypeIdx;                /* total key type */
	ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, UifRegTotal->uchMinorClass);

    if (ItemCommonTaxSystem() == ITM_TAX_US) {                          /* V3.3 */
        if ( ( sStatus = ItemCurTax( &ItemTotal->auchTotalStatus[1], WorkTax ) ) != ITM_SUCCESS ) {
            return( sStatus );
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        ItemCanTax( &ItemTotal->auchTotalStatus[1], WorkTax, ITM_NOT_SPLIT );
		if(WorkTax->alTaxable[1] > 100000)
		{
			NHPOS_ASSERT(!"ItemTendTTTax TAX ERROR");
		}
    } else {
        ItemCurVAT( &ItemTotal->auchTotalStatus[1], WorkVAT, ITM_NOT_SPLIT);
    }

    if (ItemCommonTaxSystem() == ITM_TAX_INTL) {
        lTax = WorkVAT->lPayment;
    } else {
        lTax = WorkTax->alTax[0] + WorkTax->alTax[1] + WorkTax->alTax[2] + WorkTax->alTax[3];
    }

    ItemTotal->lMI += lTax;                                         /* MI */
    ItemTotal->lTax += lTax;                                        /* all tax */

    return( ITM_SUCCESS );
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendTrayTax( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate whole tax data at tray transaction
==========================================================================*/

VOID    ItemTendTrayTax( VOID )
{
    SHORT   i;
    CONST ITEMTOTALLOCAL  *WorkTotal = ItemTotalGetLocalPointer();

    if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) == 0 && TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {   /* tray ttl transaction */
		ITEMAFFECTION   Affect = {0};

        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

			WorkTI->lHourly += WorkTotal->lTrayTax[0] + WorkTotal->lTrayTax[1]    /* V3.3 */
                + WorkTotal->lTrayTax[2] + WorkTotal->lTrayTax[3] + WorkTotal->lTrayVatService;
        }

        Affect.uchMajorClass = CLASS_ITEMAFFECTION;
        Affect.uchMinorClass = CLASS_TAXPRINT;
        Affect.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {                  /* V3.3 */
            if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) ) {   /* GST/PST print option */
                Affect.USCanVAT.USCanTax.lTaxable[0] = WorkTotal->lTrayTaxable[0];
                Affect.USCanVAT.USCanTax.lTaxable[1] = WorkTotal->lTrayTaxable[1]
                        + WorkTotal->lTrayTaxable[2] + WorkTotal->lTrayTaxable[3];
                Affect.USCanVAT.USCanTax.lTaxable[4] = WorkTotal->lTrayTaxable[4];

                Affect.USCanVAT.USCanTax.lTaxAmount[0] = WorkTotal->lTrayTax[0];
                Affect.USCanVAT.USCanTax.lTaxAmount[1] = WorkTotal->lTrayTax[1] + WorkTotal->lTrayTax[2] + WorkTotal->lTrayTax[3];

            } else if ( CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT1 ) == 0
                && CliParaMDCCheck( MDC_CANTAX_ADR, EVEN_MDC_BIT2 ) == 0 ) {
                Affect.USCanVAT.USCanTax.lTaxable[0] = WorkTotal->lTrayTaxable[0]
                    + WorkTotal->lTrayTaxable[1] + WorkTotal->lTrayTaxable[2] + WorkTotal->lTrayTaxable[3];

                Affect.USCanVAT.USCanTax.lTaxAmount[0] = WorkTotal->lTrayTax[0]
                    + WorkTotal->lTrayTax[1] + WorkTotal->lTrayTax[2] + WorkTotal->lTrayTax[3];

            } else {
                for( i = 0; i < 4; i++ ) {                              /* individual print */
                    Affect.USCanVAT.USCanTax.lTaxable[i] = WorkTotal->lTrayTaxable[i];
                    Affect.USCanVAT.USCanTax.lTaxAmount[i] = WorkTotal->lTrayTax[i];
                }
            }
            Affect.USCanVAT.USCanTax.lTaxable[4] = WorkTotal->lTrayTaxable[4];          /* non-taxable */

            for( i = 0; i < 4; i++ ) {                                  /* tax exempt amount */
                Affect.USCanVAT.USCanTax.lTaxExempt[i] = WorkTotal->lTrayTaxExempt[i];
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            for( i = 0; i < 3; i++ ) {
                Affect.USCanVAT.USCanTax.lTaxable[i] += WorkTotal->lTrayTaxable[i];
                Affect.USCanVAT.USCanTax.lTaxAmount[i] += WorkTotal->lTrayTax[i];
                Affect.USCanVAT.USCanTax.lTaxExempt[i] = WorkTotal->lTrayTaxExempt[i];
            }
        } else {
            memset(&Affect, 0, sizeof(ITEMAFFECTION));
			for (i = 0; i < NUM_VAT; i++) {
				Affect.USCanVAT.ItemVAT[i] = WorkTotal->TrayItemVAT[i];
			}
            Affect.lAmount = WorkTotal->lTrayVatService;
            ItemTendVATCommon(&Affect, ITM_NOT_SPLIT, ITM_DISPLAY);
            return;
        }

        TrnItem(&Affect);

        /*--- Display Tax, R3.0 ---*/
        Affect.uchMinorClass = CLASS_TAXPRINT;
        MldScrollWrite(&Affect, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&Affect);
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendCheckTotal(ITEMTOTAL *ItemTotal, UIFREGTENDER *UifTender)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate check total data,     R3.0,    V3.3
==========================================================================*/

VOID    ItemTendCheckTotal(ITEMTOTAL *ItemTotal, UIFREGTENDER *UifTender)
{
    UIFREGTOTAL     UifRegTotal = {0};
	ITEMCOMMONTAX   WorkTax = {0};
    ITEMCOMMONVAT   WorkVAT = {0};
    PARATEND        ParaTend;
    DCURRENCY       lAmount;

    ParaTend.uchMajorClass = CLASS_PARATEND;
    ParaTend.uchAddress = UifTender->uchMinorClass;
    CliParaRead(&ParaTend);

	//We now allow Tenders that arent assigned a total
	//to have the minor class assigned to 0, so that we
	//dont follow Total 2's control code regarding Printing

    UifRegTotal.uchMajorClass = CLASS_UIFREGTOTAL;
    if (ParaTend.uchTend == 2 /*|| ParaTend.uchTend == 0*/) {
        UifRegTotal.uchMinorClass = CLASS_UITOTAL2;
    } else if(ParaTend.uchTend != 0){
        UifRegTotal.uchMinorClass = ParaTend.uchTend;
    } else {
		UifRegTotal.uchMinorClass = CLASS_UITOTAL1;
	}

    memset( ItemTotal, 0, sizeof( ITEMTOTAL ) );

    if ( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) {                     /* tray total transaction */
        ItemTendCalAmount(&ItemTotal->lMI, UifTender);

        /* --- Check Total Key Type --- */
        ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
		//We now allow Tenders that arent assigned a total
		//to have the minor class assigned to 0, so that we
		//dont follow Total 2's control code regarding Printing
        if (ParaTend.uchTend == 2 /*|| ParaTend.uchTend == 0*/) {
            ItemTotal->uchMinorClass = CLASS_UITOTAL2;
        } else {
            ItemTotal->uchMinorClass = ParaTend.uchTend;
        }
        ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        TrnItem(ItemTotal);
    } else {
        /*----- Split Check,   R3.1 -----*/
        if (TrnSplCheckSplit()  & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2 | TRN_SPL_SPLIT)) {
            ItemTotalSplTax(&UifRegTotal, ItemTotal, &WorkTax, &WorkVAT);

            /* --- Netherland rounding, 2172 --- */
            if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
                if ( RflRound( &lAmount, ItemTotal->lMI, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                    ItemTotal->lMI = lAmount;
                }
            }

            /* --- Check Total Key Type --- */
            ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
			//We now allow Tenders that arent assigned a total
			//to have the minor class assigned to 0, so that we
			//dont follow Total 2's control code regarding Printing
            if (ParaTend.uchTend == 2 /*|| ParaTend.uchTend == 0*/) {
                ItemTotal->uchMinorClass = CLASS_UITOTAL2;
            } else {
                ItemTotal->uchMinorClass = ParaTend.uchTend;
            }
            ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            TrnItem(ItemTotal);
        } else {
            /* --- Check Total Key Type --- */
            if (ParaTend.uchTend <= 2 || ItemTotalType(&UifRegTotal) != ITM_TYPE_NONFINALIZE)
			{
				if((TranCurQualPtr->fsCurStatus & CURQUAL_MULTICHECK)) //SR 279
				{
					if(!CliParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT1))//one cent tax
					{
						ItemTotalCTTax(&UifRegTotal, ItemTotal, &WorkTax, &WorkVAT);
					} else {
						// we skipped the tax, but we still need the status of the Total to be used,
						// so we read the information here
						ItemTotal->auchTotalStatus[0] = UifRegTotal.uchMinorClass;
						ItemTotalAuchTotalStatus (ItemTotal->auchTotalStatus, CLASS_TOTAL2);
					}
				} else {
					// all other kinds of checks
					ItemTotalCTTax(&UifRegTotal, ItemTotal, &WorkTax, &WorkVAT);
				}

                ItemTotalCTAmount(ItemTotal);

                ItemTotal->uchMajorClass = CLASS_ITEMTOTAL;
                ItemTotal->uchMinorClass = CLASS_TOTAL2;
                ItemTotal->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
                TrnItem(ItemTotal);
            } else {
				ITEMCOMMONLOCAL  *WorkCommon = ItemCommonGetLocalPointer();
				ITEMTENDER       DummyTender = {0};
                ItemTotalNFTotal(&UifRegTotal);
                memcpy(ItemTotal, &WorkCommon->ItemSales, sizeof(ITEMTOTAL));
                ItemPreviousItem(&DummyTender, 0);              /* Saratoga */
            }
        }
    }

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTotal, 0);

    /*--- Display Total Key, R3.0 ---*/
    MldScrollWrite(ItemTotal, MLD_NEW_ITEMIZE);
    MldScrollFileWrite(ItemTotal);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendVAT(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate check total data,     R3.0,    V3.3
*==========================================================================
*/
VOID    ItemTendVAT(UIFREGTENDER *pUifRegTender)
{
    if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)
        && !(TranCurQualPtr->fsCurStatus & CURQUAL_TRAY)
        && ItemCommonTaxSystem() == ITM_TAX_INTL)
	{
		ITEMAFFECTION   Affect = {0};
		ITEMCOMMONVAT   WorkVAT = {0};
		UCHAR           auchTotalStatus[NUM_TOTAL_STATUS];

        ItemTendGetTotal(&auchTotalStatus[0], pUifRegTender->uchMinorClass);
        ItemCurVAT(&auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);

        memcpy(&Affect.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
        Affect.lAmount = WorkVAT.lService;

        ItemTendVATCommon(&Affect, ITM_NOT_SPLIT, ITM_DISPLAY);

        /* affect exclude vat/service to hourly */
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();

            WorkTI->lHourly += WorkVAT.lPayment;
        }

        ItemTenderLocal.lPayment = WorkVAT.lPayment; /* V3.4 */
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendVATCommon(ITEMAFFECTION pAffect, SHORT sType)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*   Return:
*
*   Description:    Generate VAT Data,                          V3.3
*==========================================================================
*/
VOID    ItemTendVATCommon(ITEMAFFECTION *pAffect, SHORT sType, SHORT sDispType)
{
	PARAMDC         WorkMDC = {0};
    DCURRENCY       lService = pAffect->lAmount;

	// retrieve MDC address pair 259/260 (MDC_VAT1_ADR and MDC_VAT2_ADR)
    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_VAT1_ADR;
    CliParaRead(&WorkMDC);

    if (WorkMDC.uchMDCData & EVEN_MDC_BIT3) {    // check MDC_VAT2_ADR, EVEN_MDC_BIT3 or MDC_PARAM_BIT_A
		// Print subtotal before VAT service line at Reg. Mode if not a tray total.
        if (( TranCurQualPtr->fsCurStatus & CURQUAL_TRAY ) == 0) {
			TRANITEMIZERS   *WorkTI;

            /*----- generate Subotal print/display data -----*/
            if (sType == ITM_SPLIT) {
                WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
            } else {
				WorkTI = TrnGetTIPtr();
            }

            pAffect->uchMajorClass = CLASS_ITEMAFFECTION;
            pAffect->uchMinorClass = CLASS_VAT_SUBTOTAL;
            /* use cancel total for split sub total */
            pAffect->lAmount = (pAffect->lCancel ? pAffect->lCancel : WorkTI->lMI);
            pAffect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            if (sType == ITM_SPLIT) {
                TrnItemSpl(pAffect, TRN_TYPE_SPLITA);
            } else {
                TrnItem(pAffect);
            }
            if (sDispType == ITM_DISPLAY) {
                if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
                    MldScrollWrite2(MLD_DRIVE_2, pAffect, MLD_NEW_ITEMIZE);
                } else {
                    MldScrollWrite(pAffect, MLD_NEW_ITEMIZE);
                }
                MldScrollFileWrite(pAffect);
            }
        }
    }
    pAffect->lAmount = lService;

    if (!(WorkMDC.uchMDCData & (ODD_MDC_BIT0 | ODD_MDC_BIT1)) && lService != 0L) {
        /*----- generate VATable service data -----*/
        pAffect->uchMajorClass = CLASS_ITEMAFFECTION;
        pAffect->uchMinorClass = CLASS_SERVICE_VAT;
        pAffect->lAmount = lService;
        pAffect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        pAffect->fbModifier = 0;
        if (WorkMDC.uchMDCData & ODD_MDC_BIT3) {
            pAffect->fbModifier |= EXCLUDE_VAT_SER;
        }
        if (sType == ITM_SPLIT) {
            TrnItemSpl(pAffect, TRN_TYPE_SPLITA);
        } else {
            TrnItem(pAffect);
        }

        /* send to enhanced kds, 2172 */
        ItemSendKds(pAffect, 0);

        if (sDispType == ITM_DISPLAY) {
            if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
                MldScrollWrite2(MLD_DRIVE_2, pAffect, MLD_NEW_ITEMIZE);
            } else {
                MldScrollWrite(pAffect, MLD_NEW_ITEMIZE);
            }
            MldScrollFileWrite(pAffect);
        }
    }

    /*----- generate VAT data -----*/
    pAffect->uchMajorClass = CLASS_ITEMAFFECTION;
    pAffect->uchMinorClass = CLASS_VATAFFECT;
    pAffect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
    pAffect->fbModifier = 0;
    if (WorkMDC.uchMDCData & ODD_MDC_BIT2) {
        pAffect->fbModifier |= EXCLUDE_VAT_SER;
    }
    if (sType == ITM_SPLIT) {
        TrnItemSpl(pAffect, TRN_TYPE_SPLITA);
    } else {
        TrnItem(pAffect);
    }

    /* send to enhanced kds, 2172 */
    ItemSendKds(pAffect, 0);

    if (sDispType == ITM_DISPLAY) {
        if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
            MldScrollWrite2(MLD_DRIVE_2, pAffect, MLD_NEW_ITEMIZE);
        } else {
            MldScrollWrite(pAffect, MLD_NEW_ITEMIZE);
        }
        MldScrollFileWrite(pAffect);
    }

    if (WorkMDC.uchMDCData & (ODD_MDC_BIT0 | ODD_MDC_BIT1) && lService != 0L) {
        /*----- genearte non-VATable service data -----*/
        pAffect->uchMajorClass = CLASS_ITEMAFFECTION;
        pAffect->uchMinorClass = CLASS_SERVICE_NON;
        pAffect->lAmount = lService;
        pAffect->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
        pAffect->fbModifier = 0;
        if (WorkMDC.uchMDCData & ODD_MDC_BIT3) {
            pAffect->fbModifier |= EXCLUDE_VAT_SER;
        }
        if (sType == ITM_SPLIT) {
            TrnItemSpl(pAffect, TRN_TYPE_SPLITA);
        } else {
            TrnItem(pAffect);
        }

        /* send to enhanced kds, 2172 */
        ItemSendKds(pAffect, 0);

        if (sDispType == ITM_DISPLAY) {
            if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
                MldScrollWrite2(MLD_DRIVE_2, pAffect, MLD_NEW_ITEMIZE);
            } else {
                MldScrollWrite(pAffect, MLD_NEW_ITEMIZE);
            }
            MldScrollFileWrite(pAffect);
        }
    }
}


/*==========================================================================
**  Synopsis:   VOID    ItemTendFastTender( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tender data
==========================================================================*/

VOID    ItemTendFastTender( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT       sValPrintFlag = 0;
    DCURRENCY   lMI;

    ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
	ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */

    if (ItemTender->aszNumber[0] == 0) {
        memcpy(ItemTender->aszNumber, UifRegTender->aszNumber, sizeof(ItemTender->aszNumber));
    }
    if (UifRegTender->aszNumber[0] != 0) {
        memcpy(ItemTenderLocal.aszNumber, UifRegTender->aszNumber, sizeof(ItemTenderLocal.aszNumber));
    }
    if (ItemTender->auchExpiraDate[0] != 0) {
        memcpy(ItemTenderLocal.auchExpiraDate, ItemTender->auchExpiraDate, sizeof(ItemTenderLocal.auchExpiraDate));
    }

	ItemTenderMdcToStatus (UifRegTender->uchMinorClass, ItemTender->auchTendStatus);

    lMI = ItemTender->lRound;
    if ( TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) {    /* preselect void */
        lMI *= -1L;
    }

    /* --- Dec/6/2000 --- */
    if ( ItemTender->auchTendStatus[0] & ITEMTENDER_STATUS_0_VALIDATION ) {   /* validation print required */
        ItemTender->fsPrintStatus = PRT_VALIDATION;                           /* print validation */
        TrnThrough( ItemTender );
        sValPrintFlag = 1;
    }

    /* --- DTREE#2: Issue#3 Check Endorsement,  Dec/18/2000, 21RFC05402 --- */
    ItemCommonCheckEndorse(ItemTender->uchMinorClass, ItemTender->lTenderAmount, ItemTendWholeTransAmount(), &sValPrintFlag);

    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT3 ) && lMI <= 0L ) {
        /* --- Dec/6/2000 --- */
        if (sValPrintFlag == 1) {
	        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    			if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    			} else {
		            UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
		        }
	        }
        }
        ItemTendAudVal(ItemTendWholeTransAmount());            /* audaction validation */
        sValPrintFlag = 1;	/* 06/22/01 */
    }
    if (CliParaMDCCheck(MDC_PSELVD_ADR, EVEN_MDC_BIT2) && (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK)) {
        /* --- Dec/6/2000 --- */
        if (sValPrintFlag == 1) {
	        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    			if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
			        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
    			} else {
		            UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
		        }
	        }
        }
        ItemTendAudVal(ItemTendWholeTransAmount());            /* audaction validation */
        sValPrintFlag = 1;	/* 06/22/01 */
    }

    ItemTender->fsPrintStatus = PRT_SLIP | PRT_JOURNAL;                     /* print slip/journal */

    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) {    /* Saratoga */
        ItemTendOverDisp(UifRegTender, ItemTender);
    } else {
        ItemTendFastDisp(UifRegTender, ItemTender);
    }

    if (ItemTendCheckEPT(UifRegTender) == ITM_EPT_TENDER) {
   		ItemTendCPPrintRsp(ItemTender);                             /* Saratoga */
		/* execute slip validation for partial ept tender, 09/12/01 */
	    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) &&
		    (TranCurQualPtr->uchSeat == 0) &&
	         !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2)) {
      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
    	} else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
      		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
		} else {
	        if ((ItemTender->fsPrintStatus & PRT_SLIP) && (ItemTender->uchCPLineNo != 0)) {   /* Saratoga */
				USHORT  fsPrintStatus;                  /* 51:print status */

	    	    if (sValPrintFlag == 1) {
		    	    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
    					if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
				    	    UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
	    				} else {
			            	UieErrorMsg(LDT_PAPERCHG_ADR, UIE_WITHOUT);        /* Paper Change */
				        }
		    	    }
	        	}
			    /* if (TranCurQualPtr->uchSeat) { */
			    	TrnPrtSetSlipPageLine(0);
			    /* } */
		       	fsPrintStatus = ItemTender->fsPrintStatus;
       			ItemTender->fsPrintStatus = PRT_SLIP;             			/* print slip */
   		    	TrnThrough( ItemTender );
	      		ItemTender->fsPrintStatus = fsPrintStatus;
    	  		ItemTender->fsPrintStatus &= ~PRT_SLIP;             		/* avoid duplicated slip validation */
	    	    sValPrintFlag = 1;	/* 06/22/01 */

      			/* release slip to prevent the optional listing slip */
		        if ( !CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation, 03/23/01 */
	    			TrnSlipRelease();
		   		}
     		}
     	}
    }

    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    	if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */
            if (sValPrintFlag == 1) {
		        UieErrorMsg(LDT_VSLPREMV_ADR, UIE_WITHOUT);        /* Paper Change */
			}
		}
	}

	/* request paper at previous splited gcf */
    if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) {
	    if (TranCurQualPtr->uchSeat == 0) {
		    if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) { /* validation slip print */
    			if ( CliParaMDCCheck(MDC_SLIPVAL_ADR, EVEN_MDC_BIT0) ) { /* both validation and slip print */

					if (ItemTenderLocal.fbSlibPageStatue) {
						ITEMOTHER TrnItemOther = {0};
		   			    TrnPrtSetSlipPageLine(0);
					    if (ItemTenderLocal.usSlipPageLine == 0) {
							ItemTenderLocal.usSlipPageLine++;
						}
	    				TrnItemOther.uchMajorClass = CLASS_ITEMOTHER;   /* set class code */
					    TrnItemOther.uchMinorClass = CLASS_INSIDEFEED;
					    TrnItemOther.fsPrintStatus = PRT_SLIP; /* set slip bit on other data */
        				TrnItemOther.lAmount = ItemTenderLocal.usSlipPageLine;
		   			    TrnThrough( &TrnItemOther );
		   			}
				}
			}
    	}
	}

	// print split receipts.  needs to be here so that we have the EPT reponse text
	ItemTendSplitReceipts (UifRegTender, ItemTender);          //If doing split tender, need multiple receipts

    /* send to enhanced kds, 2172 */
    ItemSendKds(ItemTender, 0);

    /*----- Split Check,   R3.1 -----*/
    if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {
        TrnItemSpl(ItemTender, TRN_TYPE_SPLITA);
        ItemTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
        TrnItem(ItemTender);
        MldScrollWrite2(MLD_DRIVE_2, ItemTender, MLD_NEW_ITEMIZE);
    } else {
        TrnItem(ItemTender);
        MldScrollWrite(ItemTender, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(ItemTender);
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendFastDisp( UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    display tender amount
==========================================================================*/

VOID    ItemTendFastDisp( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
	REGDISPMSG      DispMsg1 = {0}, DispMsg2 = {0};
	DCURRENCY		tmpAmount = ItemTender->lTenderAmount;

	if(UifRegTender->GiftCardOperation != 0)
		ItemTender->lTenderAmount = UifRegTender->lTenderAmount;

    DispMsg1.uchMajorClass = ItemTender->uchMajorClass;
    DispMsg1.uchMinorClass = ItemTender->uchMinorClass;
    DispMsg1.lAmount = ItemTender->lTenderAmount;                   /* tender amount */

    flDispRegDescrControl |= TENDER_CNTRL;                          /* descriptor */

    /* --- Use EPT Response Text,   Saratoga --- */
    RflGetTranMnem (DispMsg1.aszMnemonic, RflChkTendAdr (ItemTender));

    if (ItemTender->aszTendMnem[0] != 0) {
//		Use transaction mnemonic for display regardless of tender struct mnemonic
//		previous to change here, the transaction mnemonic was always over writing
//		the DispMsg1.aszMnemonic text.  Richard Chambers, Aug-10-2017
//        _tcsncpy(DispMsg1.aszMnemonic, ItemTender->aszTendMnem, NUM_TRANSMNEM);
    } else {
        _tcsncpy(ItemTender->aszTendMnem, DispMsg1.aszMnemonic, NUM_TRANSMNEM );
    }

    DispMsg1.fbSaveControl = DISP_SAVE_TYPE_2;                              /* save for redisplay (not recovery) */

    if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
        if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT                /* GST exempt */
            || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {          /* PST exempt */
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
       if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
            flDispRegDescrControl |= TAXEXMPT_CNTRL;                /* tax exempt */
        }
    }

    ItemTendDispIn( &DispMsg2 );                                    /* generate display mnemonic data */
    flDispRegDescrControl &= ~TVOID_CNTRL;
    flDispRegKeepControl &= ~( TVOID_CNTRL | PAYMENT_CNTRL | RECALL_CNTRL | TAXEXMPT_CNTRL | TENDER_CNTRL );

    if ( CliParaMDCCheck( MDC_RECEIPT_ADR, ODD_MDC_BIT0 ) ) {       /* receipt control */
        flDispRegKeepControl &= ~RECEIPT_CNTRL;
        flDispRegDescrControl &= ~RECEIPT_CNTRL;
    }

    if ( CliParaMDCCheck( MDC_MENU_ADR, ODD_MDC_BIT0) != 0 ) {
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();

        uchDispCurrMenuPage = pWorkSales->uchMenuShift;               /* set default menu no. */
        if (pWorkSales->uchAdjKeyShift == 0) {                       /* not use adj. shift key */
            pWorkSales->uchAdjCurShift = pWorkSales->uchAdjDefShift; /* set default adjective, 2172 */
        }
    }

    DispWriteSpecial( &DispMsg1, &DispMsg2 );                       /* display check total */

    flDispRegDescrControl &= ~( TENDER_CNTRL | TAXEXMPT_CNTRL );
	ItemTender->lTenderAmount = tmpAmount;
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendAffection( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate hourly, close check data             R3.1
==========================================================================*/

VOID    ItemTendAffection( VOID )
{
    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
	ITEMAFFECTION   ItemAffection = {0};
    SHORT           i;

    ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;              /* major class */
    ItemAffection.uchMinorClass = CLASS_HOURLY;                     /* minor class */
    ItemAffection.lAmount = WorkTI->lHourly;                        /* hourly total */
    ItemAffection.sItemCounter = WorkTI->sItemCounter;              /* item counter */
    ItemAffection.sNoPerson = ItemTendNoPerson();
	ItemAffection.uchOffset = TranGCFQualPtr->uchHourlyOffset;      /* hourly offset */
    /* V3.3 */
	for (i = 0; i < STD_NO_BONUS_TTL; i++) {
        ItemAffection.lBonus[i] = WorkTI->lHourlyBonus[i];          /* hourly bonus total */
    }
    TrnItem( &ItemAffection );                                      /* transaction module i/F */

    /* cashier interrupt, at no recall case, R3.3 */
    if ((TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) &&
        ((TranCurQualPtr->fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER)) {

        ;   /* not affect to GCF Close */
    } else if ( WorkGCF->usGuestNo != 0 && (WorkGCF->ulCashierID != 0 )) { /* GCF operation, V3.3 */
        memset( &ItemAffection, 0, sizeof( ITEMAFFECTION ) );       /* initialize */
        ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;          /* major class */
        /* V3.3 */
        ItemAffection.uchMinorClass = CLASS_CASHIERCHECK;       /* minor class */
        ItemAffection.ulId = WorkGCF->ulCashierID;              /* cashier ID */
        ItemAffection.usGuestNo =  WorkGCF->usGuestNo;              /* GCF No */
        TrnItem( &ItemAffection );                                  /* transaction module i/F */
    }

    /*----- Affect Service Time,    R3.1 -----*/
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {
        if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING)) {
            ItemTendCalServTime();
        }
    }

    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) != CURQUAL_STORE_RECALL) { /*--- ### 99/8/17 ADD K.Iwata R3.5 ---*/
        /*----- affect to guest check ownder, V3.3 ICI ---- */
        ItemTendAffectTransFromTo(&ItemAffection);
    }

}


/*==========================================================================
**  Synopsis:   VOID    ItemTendAffectTransFromTo( ITEMAFFECTION *pAffect )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    affect to transfer from/to of guest check owner, V3.3 ICI
*
*               See as well function ItemMiscCheckTransAffect()
*               which does a similar affectation operation.
*               Waiter functionality is a legacy from NCR 2170 Hospitality
*               so this affectation may not be actually affecting the totals.
==========================================================================*/

static VOID    ItemTendAffectTransFromTo( ITEMAFFECTION *pAffect )
{
    /**** Affect to Check Transfer From/To, V3.3 ICI ***/
    if (TranGCFQualPtr->ulCashierID) {
        if (TranModeQualPtr->ulCashierID != TranGCFQualPtr->ulCashierID) {

            memset(pAffect, 0, sizeof(ITEMAFFECTION));

            /* set transfer from of guest check owner*/
            pAffect->uchMajorClass = CLASS_ITEMAFFECTION ;
            pAffect->uchMinorClass = CLASS_OLDWAITER;
            pAffect->lAmount = TranGCFQualPtr->lCurBalance;
            pAffect->lAmount *= -1L;
            pAffect->ulId = TranGCFQualPtr->ulCashierID;
            pAffect->sNoPerson = TranGCFQualPtr->usNoPerson * -1;
            pAffect->sCheckOpen = -1;

            /* check P-void GCF or Normal one */
            if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_PVOID) {
                pAffect->sNoPerson = TranGCFQualPtr->usNoPerson;
                pAffect->sCheckOpen = 1;
            }
            TrnItem(pAffect);

            /* set transfer to of cashier */
            pAffect->uchMinorClass = CLASS_NEWWAITER;
            pAffect->lAmount *= -1L;
            pAffect->ulId = TranModeQualPtr->ulCashierID;
            pAffect->sNoPerson *= -1;
            pAffect->sCheckOpen *= -1;
            TrnItem(pAffect);
        }
    }
}

VOID    ItemTendCloseNoTotals( USHORT usFlags )
{
    TRANCURQUAL     *pWorkCur = TrnGetCurQualPtr();        /* get current qualifier */

    if ( pWorkCur->flPrintStatus & CURQUAL_POSTCHECK ) {
        pWorkCur->uchPrintStorage = PRT_CONSOLSTORAGE;                                /* postheck, store/recall */
    } else if ( ( pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_PRE_UNBUFFER ) {
        pWorkCur->uchPrintStorage = PRT_NOSTORAGE;                          /* unbuffering */
    } else {
        pWorkCur->uchPrintStorage = PRT_ITEMSTORAGE;                        /* precheck */
	}

    if (CliParaMDCCheck(MDC_DEPT2_ADR, EVEN_MDC_BIT3)) {            /* Not Print Void Item, R3.1 */
        pWorkCur->uchKitchenStorage = PRT_KIT_NET_STORAGE;
    } else {
        pWorkCur->uchKitchenStorage = PRT_ITEMSTORAGE;                              /* for kitchen print */
    }

    if ( ( CliParaMDCCheck( MDC_PSELVD_ADR, EVEN_MDC_BIT1 ) == 0          /* kitchen send at pvoid */
        && ( pWorkCur->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) )  /* preselect void */
        || ( pWorkCur->fsCurStatus & CURQUAL_TRAINING ) )
	{           /* training */
        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;                        /* for kitchen print */
    }
#if 1
	if ((usFlags & 0x01) != 0 || (pWorkCur->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) != 0) {
		pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;  // if a transaction return or transaction void, print anyway.
	}
#else

	if ( TranModeQualPtr->fsModeStatus & MODEQUAL_NORECEIPT_DEF ) {
		pWorkCur->fbNoPrint |= CURQUAL_NO_R;       /* MDC says otherwise */
	}
	if ( ItemModLocalPtr->fbModifierStatus & MOD_NORECEIPT ) {
		pWorkCur->fbNoPrint ^= CURQUAL_NO_R;       /* override by Receipt key press so invert */
	}
	if ((usFlags & 0x01) != 0 || (pWorkCur->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) != 0) {
		pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;  // if a transaction return or transaction void, print anyway.
	}
	if ((pWorkCur->fsCurStatus & (CURQUAL_CUST_DUP | CURQUAL_MERCH_DUP | CURQUAL_DUPLICATE_COPY)) != 0) {
		pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;  // if an electronic payment transaction or duplicate copy receipt, print anyway.
	}
#endif
    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK
        && !(!CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2))       /* R3.1 */
        && !(pWorkCur->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING)
        && (pWorkCur->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER
        && (TranModeQualPtr->fsModeStatus & MODEQUAL_CASINT) == 0)
	{        /* R3.3 */
        pWorkCur->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);
		pWorkCur->fsCurStatus &= ~CURQUAL_MERCH_DUP;
    }

    if ((pWorkCur->flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_STORE_RECALL  /* 5/8/96 */
        && !CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3)
        && !(pWorkCur->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING)
        && (pWorkCur->fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER)
	{
        pWorkCur->fbNoPrint |= (CURQUAL_NO_SECONDCOPY);
		pWorkCur->fsCurStatus &= ~CURQUAL_MERCH_DUP;
    }

    if ( pWorkCur->auchTotalStatus[3] & CURQUAL_TOTAL_COMPULSRY_SLIP ) {    /* compulsory slip */
       pWorkCur->fbCompPrint = PRT_SLIP;
    } else {                                                        /* optional slip */
        pWorkCur->fbCompPrint &= ~PRT_SLIP;
    }

	{
		int bPrintPartial = 0;

		bPrintPartial = ((ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) != 0);
		bPrintPartial |= ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SPLIT_TENDER) != 0);
		bPrintPartial |= ((ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL) != 0);
		bPrintPartial |= ((ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2) != 0);

		if (bPrintPartial) {
			pWorkCur->fsCurStatus2 |= PRT_POST_RECEIPT;
		}
	}

	{
		ITEMSALESLOCAL  *pWorkSales = ItemSalesGetLocalPointer();
		ITEMTRANSCLOSE  ItemTransClose = {0};

		ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;            /* major class */
		if (pWorkSales->fbSalesStatus & SALES_PREAUTH_BATCH) {
			ItemTransClose.uchMinorClass = CLASS_CLS_PREAUTH_RECEIPT;
		} else {
			ItemTransClose.uchMinorClass = CLASS_CLSTENDER1;
		}
		TrnClose( &ItemTransClose );                                    /* transaction module i/F */
	}
}

// Mark the current guest check as paid and then close it out.
// This is used by EPD Balance Inquiry and anything else which
// needs to create a dummy guest check to do some kind of processing.
VOID    ItemTendMarkAndCloseGuestCheck( USHORT usFlags )
{
	SHORT   sStatus;
	TRANGCFQUAL      *WorkGCF = TrnGetGCFQualPtr();     /* get GCF qualifier */

	WorkGCF->fsGCFStatus2 |= GCFQUAL_PAYMENT_TRANS;
	if (WorkGCF->usGuestNo) {
		// ensure we have a non-zero guest check number which may happen with a Table Service system.
		// The Employee Payroll Deduction was done for Vetrans Canteen Service (VCS) and they use a
		// Store Recall type of system but we want to do something reasonable for any type of system.
		while ((sStatus = TrnSaveGC(0, WorkGCF->usGuestNo)) !=  TRN_SUCCESS) {
			USHORT  usStatus = GusConvertError(sStatus);
			UieErrorMsg(usStatus, UIE_WITHOUT);
		}
	}
}
/*==========================================================================
**  Synopsis:   VOID    ItemTendClose( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate transaction close data             R3.1
==========================================================================*/

VOID    ItemTendClose( VOID )
{
    SHORT           sStatus;
    USHORT          usStatus;

	ItemTendCloseNoTotals (0x00);

    while ( ( sStatus = TrnSendTotal() ) !=  TRN_SUCCESS ) {
        usStatus = TtlConvertError( sStatus );
        UieErrorMsg( usStatus, UIE_WITHOUT );
    }
	{
		USHORT  usReasonCode, usReturnType;
		ULONG   ulfsCurStatus = 0;

		if ((ulfsCurStatus = TrnQualModifierReturnData (&usReturnType, &usReasonCode)) & (CURQUAL_PRETURN | CURQUAL_TRETURN)) {
			if ((ulfsCurStatus & CURQUAL_PRETURN) != 0) {
				if (usReturnType == 1 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_D)) {
					ItemTendCollectSignatureOnly (FSC_VOID, usReturnType);
				}
				else if (usReturnType == 2 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_C)) {
					ItemTendCollectSignatureOnly (FSC_VOID, usReturnType);
				}
				else if (usReturnType == 3 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_B)) {
					ItemTendCollectSignatureOnly (FSC_VOID, usReturnType);
				}
			} else {
				ULONG  fbModifier = TrnQualModifierReturnTypeTest ();

				if ((fbModifier & RETURNS_MODIFIER_1) != 0 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_D)) {
					ItemTendCollectSignatureOnly (FSC_ASK, 1);
				} else if ((fbModifier & RETURNS_MODIFIER_2) != 0 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_C)) {
					ItemTendCollectSignatureOnly (FSC_ASK, 2);
				} else if ((fbModifier & RETURNS_MODIFIER_3) != 0 && CliParaMDCCheckField(MDC_SIGCAP_USE_RULES, MDC_PARAM_BIT_B)) {
					ItemTendCollectSignatureOnly (FSC_ASK, 3);
				}
			}
		}
	}
}

/*==========================================================================
**  Synopsis:   VOID   ItemTendDrawer( UCHAR uchMinor )
*
*   Input:      UCHAR uchMinor
*   Output:     none
*   InOut:      none
*   Return:
*
*   Description: drawer open
==========================================================================*/
VOID  ItemTendDrawer(UCHAR uchMinor)
{
    if ( ItemTenderCheckTenderMdc(uchMinor, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_C ) == 0 ) {   // open drawer at tender?
        ItemDrawer();                                               /* drawer open */

        if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 ) {  /* drawer compulsory */
            UieDrawerComp( UIE_ENABLE );
		} else {
			UieDrawerComp(UIE_DISABLE);				/* set non compulsory drawer status */
		}
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendCoin( ITEMTENDER    *ItemTender )
*
*   Input:      ITEMTENDER  *ItemTender
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    coin dispenser control
==========================================================================*/
VOID    ItemTendCoin( ITEMTENDER    *ItemTender )
{
    if ( ItemTender->lChange > 0L ) {
        UieCoinSendData(ItemTender->lChange);
    }
}

/*==========================================================================
**  Synopsis:   BOOL    ItemTendIsDup()
*
*   Input:      none  
*   Output:     BOOL	:False - No Duplicate
						:True  - Duplicate Receipt
*   InOut:      none
*
*   Return:
*
*   Description:    are we printing duplicate receipt
==========================================================================*/
BOOL ItemTendIsDup( VOID )
{
    if (((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC)))
        && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

        (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC)))
        && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||
            (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF)) 
        && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2))) ||
	
		(ItemTenderLocal.fbTenderStatus[1] & TENDER_CP_DUP_REC)) {
			return TRUE;
	}

	return FALSE;
}
/*==========================================================================
**  Synopsis:   VOID    ItemTendSplitRec (ITEMTENDER *pItemTender, USHORT fbCompPrint)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    print duplicate receipt for CP,EPT
*                   This function is used with a Split Tender which is different
*                   from the Table Service operation of Split Guest Check.
*                   A Split Tender or Partial Tender is when the tendering of
*                   a guest check is done by using more than one partial tender
*                   amount to pay for a guest check.
*                   A Split Guest Check operation is when a Table Service Check
*                   with the items assigned to various seats is pulled up for
*                   payment and each seat is payed by a particular tender.  The
*                   Split Guest Check allows for each individual seat to be split
*                   from the main guest check and then tendered separately.
==========================================================================*/
VOID    ItemTendSplitRec (ITEMTENDER *pItemTender, USHORT fbCompPrint)
{
	PARATENDERKEYINFO TendKeyInfo;
    TRANCURQUAL WorkCur, WorkCur2;

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItemTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

    TrnGetCurQual( &WorkCur );                       /* get current qualifier */

	WorkCur2 = WorkCur;           // save a copy of the current status to put back after changes

	if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTH) {
		WorkCur.fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;    // ensure that no logo is turned of for preauth
	}

	WorkCur.uchPrintStorage = PRT_SUMMARY_RECEIPT;     /* only print total/tender */
	// do not print split tender receipts to Electronic Journal.
    WorkCur.fbCompPrint = (UCHAR) (fbCompPrint & ~PRT_JOURNAL);
	switch (RflGetSystemType()) {
		case FLEX_PRECHK_UNBUFFER:
			// since Pre-Guest Check Unbuffered prints each line as it is entered we need to ensure that
			// if the Electronic Journal is updated with a partial electronic payment tender.
			WorkCur.fbNoPrint |= ((pItemTender->fsPrintStatus & PRT_JOURNAL) ^ PRT_JOURNAL);
			break;
		default:
			WorkCur.fbNoPrint |= PRT_JOURNAL;
			break;
	}
    WorkCur.fsCurStatus2 &= ~PRT_GIFT_RECEIPT;  /* ensure that gift receipt is off to get EPT response if there */
	if (WorkCur.fsCurStatus2 & PRT_DUPLICATE_COPY) {
		WorkCur.fsCurStatus |= CURQUAL_DUPLICATE_COPY;
	}
    WorkCur.uchKitchenStorage = PRT_NOSTORAGE;  /* Q-004 corr. 5/17 */
    TrnPutCurQual( &WorkCur );                  /* kitchen not print*/

	// Using MDC 236 Bit D to indicate if the items of a guest check should
	// be printed with a split tender or partial tender summary receipt or not.
	if (CliParaMDCCheckField(MDC_SPLIT_GCF_ADR, MDC_PARAM_BIT_D)) {
		TrnOtherSummaryReceipt(pItemTender, 1);
	} else {
		TrnOtherSummaryReceipt(pItemTender, 0);
	}

    TrnPutCurQual( &WorkCur2 );                  /* kitchen not print*/
}

VOID    ItemTendDupRec( VOID )
{
    TRANCURQUAL *pWorkCur = TrnGetCurQualPtr();

	if (pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY)
		return;

	// We are now doing printing of receipts with split tender so if this is a
	// split tender of some kind then we do not need a duplicate receipt.
	// This restriction applies to standard split or partial tender as well as for Food Stamp partial tender.
	// Amtrak Train Delay Voucher uses Food Stamp tender so we need to include it for Amtrak split tender.
	if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL)           // ItemTendDupRec() no dupe if this is partial tender
		return;
	if (ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL)      // ItemTendDupRec() no dupe if this is partial tender
		return;
	if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL)        // ItemTendDupRec() no dupe if this is food stamp partial tender
		return;
	if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_PARTIAL2)       // ItemTendDupRec() no dupe if this is food stamp partial tender
		return ;

	if (pWorkCur->fsCurStatus2 & PRT_DUPLICATE_COPY) {
		ITEMOTHER   WorkOther = {0};

        WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
        WorkOther.uchMinorClass = CLASS_CP_EPT_FULLDUP;     /* 5/30/96 */
        WorkOther.fsPrintStatus = PRT_RECEIPT;          /* only receipt     */

        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;    /* Q-004 corr. 5/17 */

		//Receipt Printed on Previously
		if (SetReceiptPrint(SET_RECEIPT_SECOND_COPY) < 0) {
			return ;
		}

        TrnItem( &WorkOther );                      /* print trans.     */
	} else if ((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC | TENDER_CPPARTIAL))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC | TENDER_CPPARTIAL)))
        && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

        (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC | TENDER_CPPARTIAL))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC | TENDER_CPPARTIAL)))
        && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||
            (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF)) /* S-004 corr.  */
        && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2))) {

        if (CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT2) == 0 || (pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY)) {
			ITEMOTHER   WorkOther = {0};
                                                        /* Full Print option*/
            WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
            WorkOther.uchMinorClass = CLASS_CP_EPT_FULLDUP;     /* 5/30/96 */
            WorkOther.fsPrintStatus = PRT_RECEIPT;          /* only receipt     */

            pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;    /* Q-004 corr. 5/17 */

			//Receipt Printed on Previously
			if (SetReceiptPrint(SET_RECEIPT_SECOND_COPY) < 0) {
				return ;
			}

            TrnItem( &WorkOther );                      /* print trans.     */
        } else {
			ITEMOTHER   WorkOther = {0};

            WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
            WorkOther.uchMinorClass = CLASS_CP_EPT_DUPLI;
            WorkOther.fsPrintStatus = PRT_RECEIPT;          /* only receipt     */

            pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;    /* Q-004 corr. 5/17 */

			//Receipt Printed on Previously
			if (SetReceiptPrint(SET_RECEIPT_SECOND_COPY) < 0) {
				return ;
			}

            TrnItem( &WorkOther );                      /* print trans.     */
        }
        /*--- 5/27/96 Display Other, R3.0 ---
        MldScrollWrite(&WorkOther, MLD_NEW_ITEMIZE);
        MldScrollFileWrite(&WorkOther); */

    } else if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_CP_DUP_REC) && (CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0))) {
		ITEMOTHER   WorkOther = {0};

        WorkOther.uchMajorClass = CLASS_ITEMOTHER;  /* set major class  */
        WorkOther.uchMinorClass = CLASS_CP_EPT_FULLDUP;     /* 5/30/96 */
        WorkOther.fsPrintStatus = PRT_RECEIPT;          /* only receipt     */

        pWorkCur->uchKitchenStorage = PRT_NOSTORAGE;    /* Q-004 corr. 5/17 */

		//Receipt Printed on Previously
		if (SetReceiptPrint(SET_RECEIPT_SECOND_COPY) < 0) {
			return ;
		}

        TrnItem( &WorkOther );                      /* print trans.     */
    }
}

/*==========================================================================
**  Synopsis:  VOID  SetReceiptPrint()
*
*   Input:      
*   Output:     
*   InOut:      none
*
*   Return:
*
*   Description:    Sets the receipt print order for Merchant and Customer copy for both an
*                   EPT (credit or debit) or a Gift Card (pre-paid).
*                   The behavior of this functionality depends on a number of MDC settings.
*                     - MDC 100 Bit D whether two credit/debit receipts may be printed or only one
*                     - MDC 361 Bit B whether two Gift Card receipts may be printed or only one
*                     - MDC 475 Bit D Merchant/Customer receipt print order for credit/debit and Gift Card
==========================================================================*/

SHORT  SetReceiptPrint(USHORT usCopyType)
{
	static USHORT  usCopyTypeSave = 0;
	TRANCURQUAL    *pWorkCur = TrnGetCurQualPtr();

	if (usCopyType == 0) {
		usCopyTypeSave = 0;
	}

	if ((usCopyType & 0xf0) != 0) {
		usCopyTypeSave = (usCopyType & 0xf0);
		// if we are just setting the transaction type then we will return otherwise drop on through
		if ((usCopyType & 0x0f) == 0)
			return 0;
	}

	usCopyType &= 0x0f;

	if (usCopyType == 0) {
		pWorkCur->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;         // normal trailer, ensure no ept logo printed with partial tender summary.
		pWorkCur->fsCurStatus &= ~CURQUAL_PRINT_EPT_LOGO;
	}
	pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP | CURQUAL_DUPLICATE_COPY);   // clear the bits before setting them in SetReceiptPrint()

	if (usCopyType && usCopyType == (usCopyTypeSave & 0x0f)) {
		// we have already done this one so just return.
		pWorkCur->fbNoPrint |= CURQUAL_NO_R;               // ensure a receipt will not be printed.
		return (-1);
	}

	if ((pWorkCur->fsCurStatus2 & PRT_DUPLICATE_COPY) != 0) {
		if (usCopyType == SET_RECEIPT_SECOND_COPY) {
			// if this is the second copy then put the duplicate transaction mnemonic on receipt.
			pWorkCur->fbNoPrint &= ~CURQUAL_NO_R;               // ensure a receipt will be printed.
			pWorkCur->fsCurStatus |= CURQUAL_DUPLICATE_COPY;
		} else if (pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) {
			// if this is the first copy and there will be no second copy then
			// put the duplicate transaction mnemonic on receipt.
			pWorkCur->fsCurStatus |= CURQUAL_DUPLICATE_COPY;
		}
	}

	if ((ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT) != 0 || (usCopyTypeSave & SET_RECEIPT_TRAN_EPT) != 0) {
		if ((usCopyTypeSave & SET_RECEIPT_TRAN_EPT) != 0) {
			if(!CliParaMDCCheckField (MDC_RCT_PRT_ADR, MDC_PARAM_BIT_D)) {
				//Print the Merchant copy first.
				if (usCopyType == SET_RECEIPT_FIRST_COPY) {
					pWorkCur->fsCurStatus |= CURQUAL_MERCH_DUP; 
				} else if ((pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) == 0 && CliParaMDCCheckField(MDC_CPPARA2_ADR, MDC_PARAM_BIT_D)) {
					pWorkCur->fsCurStatus |= CURQUAL_CUST_DUP;
				} else {
					pWorkCur->fbNoPrint |= CURQUAL_NO_R;               // ensure a receipt will not be printed.
					pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);   // clear the bits before setting them in SetReceiptPrint()
				}
			}
			else {
				//Print the Customer copy first.
				if (usCopyType == SET_RECEIPT_FIRST_COPY)
					pWorkCur->fsCurStatus |= CURQUAL_CUST_DUP; 
				else if ((pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) == 0 && CliParaMDCCheckField (MDC_CPPARA2_ADR, MDC_PARAM_BIT_D))
					pWorkCur->fsCurStatus |= CURQUAL_MERCH_DUP;
				else {
					pWorkCur->fbNoPrint |= CURQUAL_NO_R;               // ensure a receipt will not be printed.
					pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);   // clear the bits before setting them in SetReceiptPrint()
				}
			}
			if (pWorkCur->fsCurStatus & (CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP)) {
				pWorkCur->fsCurStatus |= CURQUAL_PRINT_EPT_LOGO; 
				pWorkCur->fbNoPrint &= ~CURQUAL_NO_EPT_LOGO_SIG;         // ensure ept logo printed.
			} else {
				pWorkCur->fsCurStatus &= ~CURQUAL_PRINT_EPT_LOGO; 
				pWorkCur->fbNoPrint |= CURQUAL_NO_EPT_LOGO_SIG;         // ensure no ept logo printed.
			}
		}

		if ((usCopyTypeSave & SET_RECEIPT_TRAN_PREPAID) != 0) {
			//checking to see if only the Gift Cards need to be printed twice
			if (!CliParaMDCCheckField (MDC_RCT_PRT_ADR, MDC_PARAM_BIT_D)) {
				//Print the Merchant copy first.
				if (usCopyType == SET_RECEIPT_FIRST_COPY)
					pWorkCur->fsCurStatus |= CURQUAL_MERCH_DUP; 
				else if ((pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) == 0 && CliParaMDCCheckField (MDC_ENHEPT_ADR, MDC_PARAM_BIT_B))
					pWorkCur->fsCurStatus |= CURQUAL_CUST_DUP;
				else {
					pWorkCur->fbNoPrint |= CURQUAL_NO_R;               // ensure a receipt will not be printed.
					pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);   // clear the bits before setting them in SetReceiptPrint()
				}
			}
			else {
				//Print the Customer copy first.
				if (usCopyType == SET_RECEIPT_FIRST_COPY)
					pWorkCur->fsCurStatus |= CURQUAL_CUST_DUP; 
				else if ((pWorkCur->fbNoPrint & CURQUAL_NO_SECONDCOPY) == 0 && CliParaMDCCheckField (MDC_ENHEPT_ADR, MDC_PARAM_BIT_B))
					pWorkCur->fsCurStatus |= CURQUAL_MERCH_DUP;
				else {
					pWorkCur->fbNoPrint |= CURQUAL_NO_R;               // ensure a receipt will not be printed.
					pWorkCur->fsCurStatus &= ~(CURQUAL_MERCH_DUP | CURQUAL_CUST_DUP);   // clear the bits before setting them in SetReceiptPrint()
				}
			}
		}
	}
	usCopyTypeSave = ((usCopyTypeSave & 0xf0) | (usCopyType & 0x0f));

	return 0;
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTendFastSpl(UIFREGTENDER *UifRegTender,
*                           ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER    *UifRegTender, ITEMTENDER   *ItemTender
*   Output:     ITEMTENDER  *ItemTender
*   InOut:      none
*
*   Return:
*
*   Description:    w/o amount tender for split tender.        R3.1
==========================================================================*/

SHORT   ItemTendFastSpl(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT       sFlag = 0;
    SHORT       sStatus;            /* 6/6/96 */

    do {   // one time only loop to allow breaks on status checks
        if (TranCurQualPtr->uchSeat != 0 && TranCurQualPtr->auchTotalStatus[0] == 0) {
			/*----- Split Key -> Seat# -> Tender -----*/
			ITEMTOTAL   ItemTotal = {0};
			UCHAR       auchTotalStatus[NUM_TOTAL_STATUS];

            ItemTendGetTotal(&auchTotalStatus[0], UifRegTender->uchMinorClass);
            ItemTendAffectTaxSpl(auchTotalStatus);
            ItemTendSplitCheckTotal(&ItemTotal, UifRegTender);
            TrnItemSpl(&ItemTotal, TRN_TYPE_SPLITA);

            /* send to enhanced kds, 2172 */
            ItemSendKds(&ItemTotal, 0);

            MldScrollWrite2(MLD_DRIVE_2, &ItemTotal, MLD_NEW_ITEMIZE);
            MldDispSubTotal(MLD_TOTAL_2, ItemTotal.lMI);
            TrnThroughDisp(&ItemTotal); /* display on KDS */
			/* if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) { */
				/* for check endorsement calculation */
				ItemTenderLocal.ItemTotal = ItemTotal;
			/* } */
            break;
        }

		if (TranCurQualPtr->uchSeat != 0 && TranCurQualPtr->auchTotalStatus[0] != 0) {
			/*----- Split Key -> Seat# -> Total Key -> Tender -----*/
			ITEMCOMMONLOCAL  *WorkCommon = ItemCommonGetLocalPointer();
			ITEMTENDER       DummyTender = {0};

            ItemTendAffectTaxSpl(TranCurQualPtr->auchTotalStatus);
            if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
                /* send to enhanced kds, 2172 */
                ItemSendKds(&WorkCommon->ItemSales, 0);

                MldScrollWrite2(MLD_DRIVE_2, &WorkCommon->ItemSales, MLD_NEW_ITEMIZE);
                TrnThroughDisp(&WorkCommon->ItemSales); /* display on KDS */
				/* if (ItemTenderLocal.ItemTotal.uchMajorClass == 0) { */
					/* for check endorsement calculation */
					memcpy(&ItemTenderLocal.ItemTotal, &WorkCommon->ItemSales, sizeof(ITEMTOTAL));
				/* } */
            }

			if (WorkCommon->ItemSales.uchMajorClass == CLASS_ITEMTENDER) {
				/* ---- to affect previous partial tender 06/28/96 ---- */
				ITEMTENDER  *pPrevTender = (VOID *)&(WorkCommon->ItemSales);
				UCHAR       fbStorageSave;

                fbStorageSave = pPrevTender->fbStorageStatus;
                pPrevTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
                pPrevTender->fbStorageStatus |= NOT_SEND_KDS;
                TrnItem(pPrevTender);
                pPrevTender->fbStorageStatus = fbStorageSave;

                /* send to enhanced kds, 2172 */
                ItemSendKds(pPrevTender, 0);

            }
            ItemPreviousItemSpl(&DummyTender, TRN_TYPE_SPLITA); /* Saratoga */
            break;
        }

        if (TranCurQualPtr->auchTotalStatus[0] == 0) {
			/*----- Split Key -> Total Key -> Tender -----*/
			return(LDT_SEQERR_ADR);
        }

        if ((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC))
            || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC)))
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

            (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC))
            || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC)))
            && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||
                (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF))
            && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2))) {

            TrnStoGetConsToPostR();         /* 6/5/96 */
        }
        /*----- Check Status 6/6/96 -----*/
        if ((sStatus = ItemTendSplitTender(UifRegTender)) != ITM_SUCCESS) {
            return(sStatus);
        }
        sFlag = 1;
        break;
    } while (0);   // end of one time only loop to allow breaks from status checks

    if (sFlag == 0) {
        ItemTendFastTender(UifRegTender, ItemTender);
        ItemTendDispSpl();
    } else {
        ItemTendFastTenderSplit(UifRegTender, ItemTender);
        ItemTendAffection();
    }

    if (CliParaMDCCheck(MDC_DRAWER2_ADR, EVEN_MDC_BIT0) == 0) {
       ItemTendDrawer(UifRegTender->uchMinorClass);
    }

    ItemTendEuroTender(ItemTender);                            /* Euro trailer, V3.4 */

    if (sFlag == 0) {
        if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT          /* 5/28/96 exist EPT tend ? */
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {   /* 5/28/96 EPT logo */
            ItemTrailer( TYPE_SPLIT_EPT );                          /* 5/28/96 EPT trailer      */
        } else {
            ItemTrailer(TYPE_SPLITA);                               /* 5/28/96 */
        }
        ItemTendCloseSpl();

        ItemTenderLocal.lTenderizer = 0L;
        ItemTenderLocal.lFCMI = 0L;
        ItemTenderLocal.fbTenderStatus[1] &= ~TENDER_SEAT_PARTIAL;
        return(UIF_SEAT_TRANS);

    } else {
        if (ItemTenderLocal.fbTenderStatus[0] & TENDER_EPT          /* 5/28/96 exist EPT tend ? */
            && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT1)) {   /* 5/28/96 EPT logo */
            ItemTrailer( TYPE_EPT );                                /* 5/28/96 EPT trailer      */
        } else {
            ItemTrailer( TYPE_STORAGE );                            /* 5/28/96 normal trailer   */
        }
        /* 5/28/96 ItemTrailer(TYPE_STORAGE); */
        ItemTendClose();
        ItemTendSplitValidation(UifRegTender, ItemTender);
        /* 6/4/96 ItemTendDupRec();                                 5/28/96 EPT duplicate    */
        ItemTendDupRecSpl(ItemTender, UifRegTender);
        ItemTendDelGCF();
        ItemTendSaved(ItemTender);
        ItemTendInit();
        return(ItemFinalize());
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendAffectTaxSpl(UCHAR *puchTotal)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate tax data for split file,          R3.1
==========================================================================*/

VOID    ItemTendAffectTaxSpl(CONST UCHAR *puchTotal)
{
    if ((ItemTenderLocal.fbTenderStatus[1] & TENDER_SEAT_PARTIAL) == 0) {
		ITEMAFFECTION   ItemAffection = {0};
		ITEMCOMMONTAX   WorkTax = {0};
		ITEMCOMMONVAT   WorkVAT = {0};

        /*----- generate affection tax -----*/
        ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;
        ItemAffection.uchMinorClass = CLASS_TAXAFFECT;

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(puchTotal + 1, &WorkTax, ITM_SPLIT);
            ItemCanTaxCopy1(&ItemAffection, &WorkTax);
            if (*(puchTotal + 1) != 0) {
                ItemAffection.fbStorageStatus = NOT_CONSOLI_STORAGE;
                TrnItem(&ItemAffection);

                /* send to enhanced kds, 2172 */
                ItemSendKds(&ItemAffection, 0);
            }

            /*----- adjust tax print format 4/17/96 -----*/
            ItemCanTaxCopy2(&ItemAffection);
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            ItemCurTaxSpl(puchTotal + 1, &WorkTax);
            ItemUSTaxCopy1(&ItemAffection, &WorkTax);
			//       ITM_TTL_FLAG_TAX_1      ITM_TTL_FLAG_TAX_2          ITM_TTL_FLAG_TAX_3
            if (*(puchTotal + 1) & 0x01 || *(puchTotal + 1) & 0x10 || *(puchTotal + 2) & 0x01) {
                ItemAffection.fbStorageStatus = NOT_CONSOLI_STORAGE;
                TrnItem(&ItemAffection);

                /* send to enhanced kds, 2172 */
                ItemSendKds(&ItemAffection, 0);
            }
        } else {
            ItemCurVAT(puchTotal + 1, &WorkVAT, ITM_SPLIT);
			//       ITM_TTL_FLAG_TAX_1      ITM_TTL_FLAG_TAX_2          ITM_TTL_FLAG_TAX_3
            if (*(puchTotal + 1) & 0x01 || *(puchTotal + 1) & 0x02 || *(puchTotal + 2) & 0x04) {
                memset(&ItemAffection, 0, sizeof(ITEMAFFECTION));
                memcpy(&ItemAffection.USCanVAT.ItemVAT[0], &WorkVAT.ItemVAT[0], sizeof(ITEMVAT) * NUM_VAT);
                ItemAffection.lAmount = WorkVAT.lService;

                ItemTendVATCommon(&ItemAffection, ITM_SPLIT, ITM_DISPLAY);
            }
            ItemTenderLocal.lPayment = WorkVAT.lPayment; /* V3.4 */
        }

        if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
			TRANITEMIZERS   *WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
            /*----- Make Subtotal,   R3.1 5/6/96 -----*/
            if (CliParaMDCCheck(MDC_TIPPO2_ADR, EVEN_MDC_BIT2)) {
                ItemAffection.lAmount = WorkTI->lMI;
            }

            /*--- Display Tax, R3.0 ---*/
            ItemAffection.uchMinorClass = CLASS_TAXPRINT;
            ItemAffection.fsPrintStatus = PRT_SLIP | PRT_JOURNAL;
            ItemAffection.fbStorageStatus = NOT_ITEM_STORAGE;
            TrnItemSpl(&ItemAffection, TRN_TYPE_SPLITA);
            MldScrollWrite2(MLD_DRIVE_2, &ItemAffection, MLD_NEW_ITEMIZE);
            TrnThroughDisp(&ItemAffection); /* display on KDS */
        }
        /*----- Add Tax to Hourly in TransInf -----*/
        if (CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT0) == 0) {
			TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
            if (ItemCommonTaxSystem() != ITM_TAX_INTL) {
				DCURRENCY   lTax;
                lTax = ItemAffection.USCanVAT.USCanTax.lTaxAmount[0]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[1]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[2]
                    + ItemAffection.USCanVAT.USCanTax.lTaxAmount[3];
                WorkTI->lHourly += lTax;
            } else {
                WorkTI->lHourly += WorkVAT.lPayment;
            }
        }
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendDispSpl(VOID)
*
*   Input:
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Display Base Transaction on LCD Left.
==========================================================================*/

VOID    ItemTendDispSpl(VOID)
{
	MLDTRANSDATA        WorkMLD = {0};

    MldSetMode(MLD_DRIVE_THROUGH_1SCROLL);

    WorkMLD.usVliSize = TrnInformationPtr->usTranConsStoVli;
    WorkMLD.sFileHandle = TrnInformationPtr->hsTranConsStorage;
    WorkMLD.sIndexHandle = TrnInformationPtr->hsTranConsIndex;
    MldPutTransToScroll(&WorkMLD);

    MldDispSubTotal(MLD_TOTAL_1, TranItemizersPtr->lMI);
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendCloseSpl(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate transaction close data             R3.1
==========================================================================*/

VOID    ItemTendCloseSpl(VOID)
{
	ITEMTRANSCLOSE  ItemTransClose = {0};

    ItemTransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;

    if ((((ItemTenderLocal.fbTenderStatus[0] & (TENDER_EPT | TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_EPT | TENDER_RC | TENDER_AC)))
        && CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT0)) ||

        (((ItemTenderLocal.fbTenderStatus[0] & (TENDER_RC | TENDER_AC))
        || (ItemTenderLocal.fbTenderStatusSave & (TENDER_RC | TENDER_AC)))
        && ((ItemTenderLocal.fbModifier & OFFEPTTEND_MODIF) ||
            (ItemTenderLocal.fbModifier & OFFCPTEND_MODIF)) /* S-004 corr.  */
        && CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT2))) {

        if (CliParaMDCCheck(MDC_CPPARA2_ADR, EVEN_MDC_BIT2) == 0) {
            ItemTransClose.uchMinorClass = CLASS_CLSSEATTRANSDUP1;    /* Full Print option*/
        } else {
            ItemTransClose.uchMinorClass = CLASS_CLSSEATTRANSDUP2;
        }
    } else {
        ItemTransClose.uchMinorClass = CLASS_CLSSEATTRANS;
    }
    TrnClose(&ItemTransClose);

    /* send to enhanced kds, 2172 */
    ItemSendKds(&ItemTransClose, 0);

    /*----- Reset ItemTenderLocal Area -----*/
    ItemTenderLocal.fbTenderStatus[0] &= ~(TENDER_EPT | TENDER_RC | TENDER_AC);
    ItemTenderLocal.fbTenderStatusSave &= ~(TENDER_EPT | TENDER_RC | TENDER_AC);
    ItemTenderLocal.fbModifier &= ~(OFFEPTTEND_MODIF | OFFCPTEND_MODIF);
    memset(&ItemTenderLocal.aszNumber[0], 0, sizeof(ItemTenderLocal.aszNumber));
    memset(&ItemTenderLocal.aszRoomNo[0], 0, sizeof(ItemTenderLocal.aszRoomNo));
    memset(&ItemTenderLocal.aszGuestID[0], 0, sizeof(ItemTenderLocal.aszGuestID));
    memset(&ItemTenderLocal.aszCPMsgText[0][0], 0, sizeof(ItemTenderLocal.aszCPMsgText));
    memset(&ItemTenderLocal.auchExpiraDate[0], 0, sizeof(ItemTenderLocal.auchExpiraDate));
    memset(&ItemTenderLocal.auchApproval[0], 0, sizeof(ItemTenderLocal.auchApproval));
}



/*==========================================================================
**  Synopsis:   SHORT    ItemTendGiftCard(UIFREGTENDER *pUifRegTender)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*	
*   Description:    generate Gift Card Issue/Reload information            SSTOLTZ
==========================================================================*/
SHORT ItemTendGiftCard (UIFREGTENDER *pUifRegTender)
{
	TRANGCFQUAL    *pWorkGCF = TrnGetGCFQualPtr();
	USHORT          usNo=1;
	USHORT          index=0;
	SHORT           error;//,sRetStatus
	SHORT			sRetStatus ;

#if 0
	// check not implemented for now as there are scenarios in which one employee may
	// decide to put money on another employee's EPD account.
	{
		// check to see if any of these are a card reload.  if any are a card reload
		// then perform a check on the allowable tenders.  If it is a reload from a
		// tender transaction type that is not allowed then error.
		// an example for VCS is to do a card reload from an Employee Payroll Deduction.
		PARATENDERKEYINFO TendKeyInfoTender = {0};

		TendKeyInfoTender.uchAddress = pUifRegTender->uchMinorClass;
		ParaTendInfoRead(&TendKeyInfoTender);
		for (index = 0; index <= NUM_SEAT_CARD; index++) {
			if(pWorkGCF->TrnGiftCard[index].GiftCard != IS_GIFT_CARD_IS_NOT) {
				if (pWorkGCF->TrnGiftCard[index].aszNumber[0] == 0 && pWorkGCF->TrnGiftCard[index].aszTrack2[0] == 0)
				{
					continue;
				}
				if(pWorkGCF->TrnGiftCard[index].Used == GIFT_CARD_USED){
					continue;
				}
				if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_RELOAD){
					if (pWorkGCF->TrnGiftCard[index].auchPluStatus & PLU_EPAYMENT) {
						if (TendKeyInfoTender.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD) {
							return LDT_WRONG_ACTYPE_ADR;
						}
					}
				}
			}
		}
	}
#endif

	sRetStatus=UIF_SUCCESS;

//************
/*
	Gift Card Info and Functions that Gift Card Use
	TtlWriteFile-totals info to write
    PrtTender_TH IS WHERE IT CHECK TO SEE IF THE MNEM IS A GIFT CARD
	MldNumber where displays the numbers for PLUs
    prttender_ej()
	         _th()
	CheckGiftMnem()
	PrtPrintTran
	CheckGiftCardPrt
	ItemVoidGiftCardCancel VOIDS THE GIFT CARDS IF CANCEL IS PRESSED.
*/	
//************

	for (index = 0; index <= NUM_SEAT_CARD; index++) {
		if(pWorkGCF->TrnGiftCard[index].GiftCard != IS_GIFT_CARD_IS_NOT) {
			SHORT           giftType=0;
			ITEMTENDER      ItemTender = {0};
			UIFREGTENDER	UifRegTender = {0};
			USHORT			usLeadAdrs = 0;

			if (pWorkGCF->TrnGiftCard[index].aszNumber[0] == 0 && pWorkGCF->TrnGiftCard[index].aszTrack2[0] == 0)
			{
				continue;
			}
			
			if(pWorkGCF->TrnGiftCard[index].Used == GIFT_CARD_USED){
				continue;
			}

			//Gift Card or FreedomPay
			UifRegTender.GiftCard = pWorkGCF->TrnGiftCard[index].GiftCard;
			ItemTender.uchMajorClass = CLASS_ITEMTENDER;

			//sets up each gift card to the correct type
			if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_ISSUE){
				UifRegTender.GiftCardOperation = GIFT_CARD_ISSUE;	
				TrnGetTransactionInvoiceNum (ItemTender.refno.auchReferncNo);
				giftType = GIFT_CARD_ISSUE;
				usLeadAdrs = LDT_ISSUE;
			}
			else if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID){
				UifRegTender.GiftCardOperation = GIFT_CARD_VOID;
				ItemTender.refno = pWorkGCF->TrnGiftCard[index].refno;
				ItemTender.authcode = pWorkGCF->TrnGiftCard[index].authcode;
				giftType = GIFT_CARD_VOID;
				usLeadAdrs = LDT_VOID_ISSUE;
			}
			else if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_RELOAD){
				UifRegTender.GiftCardOperation = GIFT_CARD_RELOAD;
				TrnGetTransactionInvoiceNum (ItemTender.refno.auchReferncNo);
				giftType = GIFT_CARD_RELOAD;
				usLeadAdrs = LDT_RELOAD;
			}
			else if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID_RELOAD){
				UifRegTender.GiftCardOperation = GIFT_CARD_VOID_RELOAD;
				ItemTender.refno =  pWorkGCF->TrnGiftCard[index].refno;
				ItemTender.authcode = pWorkGCF->TrnGiftCard[index].authcode;
				giftType = GIFT_CARD_VOID_RELOAD;
				usLeadAdrs = LDT_VOID_RELOAD;
			}

			RflGetLead (ItemTender.aszTendMnem, usLeadAdrs);
			
			ItemTender.fsPrintStatus = 0;
			ItemTender.fbStorageStatus = 0;

			ItemTender.lTenderAmount = pWorkGCF->TrnGiftCard[index].aszPrice;
			ItemTender.lRound = pWorkGCF->TrnGiftCard[index].aszPrice;
		
			// check for Track2 Data.
			if(pWorkGCF->TrnGiftCard[index].aszTrack2[0] != 0)
			{
				memcpy (ItemTender.auchMSRData, pWorkGCF->TrnGiftCard[index].aszTrack2, sizeof(ItemTender.auchMSRData));
			}else {
				// if no Track2 Data exists we will copy and send the gift card number instead.
				memcpy (ItemTender.aszNumber, pWorkGCF->TrnGiftCard[index].aszNumber, sizeof(ItemTender.aszNumber));
			}

			error = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);

			if (error == ITM_SUCCESS) {
				pWorkGCF->TrnGiftCard[index].Used = GIFT_CARD_USED;
				pWorkGCF->TrnGiftCard[index].ItemTender = ItemTender;
				pWorkGCF->TrnGiftCard[index].usCommStatus = GIFT_COMM_SUCCESS;
			}
			else {
				pWorkGCF->TrnGiftCard[index].usCommStatus = GIFT_COMM_FAILURE;
				sRetStatus = error;
			}

			{
				REGDISPMSG		Disp = {0};

				Disp.uchMajorClass = CLASS_UIFREGOTHER; 
				Disp.uchMinorClass = CLASS_UICLEAR;
				DispWrite(&Disp);
			}

			memset (&ItemTender, 0, sizeof(ItemTender));  // clear the memory area per PABP recommendations
			if (error != ITM_SUCCESS) {
				break;
			}
		}
	}

	return (sRetStatus);
}

/*==========================================================================
**  Synopsis:   SHORT    VOID DisplayGiftCard()
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    generate Gift Card Issue/Reload information            SSTOLTZ
==========================================================================*/
VOID DisplayGiftCard(){
	TRANGCFQUAL    *pWorkGCF = TrnGetGCFQualPtr();
	USHORT          index=0;

	for (index = 0; index <= NUM_SEAT_CARD; index++){
		if(pWorkGCF->TrnGiftCard[index].Used == GIFT_CARD_USED)
			MldScrollWrite(&pWorkGCF->TrnGiftCard[index].ItemTender, MLD_NEW_ITEMIZE);
	}
}


/****** end of file ******/