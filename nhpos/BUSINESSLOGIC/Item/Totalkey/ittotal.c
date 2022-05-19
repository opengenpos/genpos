/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-2000      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TOTAL KEY MODULE
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTOTAL.C (main)
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
  ---------------------------------------------------------------------
:  Abstract:
:   ItemTotal()             ;   total key module main
:
:  ---------------------------------------------------------------------
:  Update Histories
:   Date    :   Version/Revision    :   Name    :   Description
:
:   92/5/18     00.00.01                hkato
:   95/7/18     03.00.01                hkato
:   95/11/13    03.01.00                hkato
:   99/08/13    03.05.00                M.teraki: Merge STORE_RECALL_MODEL
:                                                       /GUEST_CHECK_MODEL
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
#include    "uie.h"
#include    "pif.h"
#include    "rfl.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "fdt.h"
#include    "itmlocal.h"
#include	<plu.h>
#include	"csstbfcc.h"
#include	"CSSTBEPT.H"
#include	"csstbttl.h"
#include	"csttl.h"


ITEMTOTALLOCAL  ItemTotalLocal;

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalGetLocal( ITEMTOTALLOCAL *pData )
**  Synopsis:   SHORT   ItemTotalPutLocal( ITEMTOTALLOCAL *pData )
*
*   Input:      ITEMTOTALLOCAL  *pData
*   Output:     ITEMTOTALLOCAL  *pData
*   InOut:      none
*
*   Return:
*
*   Description:    get total local data
*   Description:    put total local data
==========================================================================*/
CONST volatile ITEMTOTALLOCAL * CONST ItemTotalLocalPtr = &ItemTotalLocal;

ITEMTOTALLOCAL *ItemTotalGetLocalPointer (VOID)
{
	return &ItemTotalLocal;
}

VOID    ItemTotalGetLocal( ITEMTOTALLOCAL *pData )
{
    *pData = ItemTotalLocal;
}

VOID    ItemTotalPutLocal( ITEMTOTALLOCAL *pData )
{
	ItemTotalLocal = *pData;
}

// The following is to work around a field issue seen on some Amtrak trains in which
// the initialization sequence which beings with the Start Log In screen in which
// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
// the Inventory app for other train trip startup activities does not complete properly.
// The result is that the LSA is unable to complete the Start Log In screen task and
// is stuck with it partially complete. A control string is used to automate this task
// so as part of the sanity check as to whether to allow this workaround, check that a
// control string is running.
//
// This change is used to check if the Loan has already been made and if so, just report success.
//      Richard Chambers, Aug-17-2018, for Amtrak only
SHORT ItemTotalCashierCheckLoanTotalZero (ULONG  ulEmployeeId)
{
	/* Check Daily,PTD Current Total File */
	TTLCASHIER    CastTtl = {0};
	SHORT         sError;

	CastTtl.uchMajorClass = CLASS_TTLCASHIER;
	CastTtl.uchMinorClass = CLASS_TTLCURDAY;
	CastTtl.ulCashierNumber = ulEmployeeId;
	if ((sError = SerTtlTotalRead(&CastTtl)) == TTL_SUCCESS) {
		if (CastTtl.Loan.lAmount != 0) {
			sError = TTL_NOTZERO;    // if non-zero loan amount then return TTL_NOTZERO even if Total Read succeeded.
		}
		// drop through with the sError value of TTL_SUCCESS.
	} else {
		// if there was an error then return sucess. We are just interested in zero or non-zero
		// indicating if loan not yet made or already made.
		sError = TTL_SUCCESS;        // total for this Employe Id does not exist so basically it's zero!
	}

	return sError;
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTotalAuchTotalStatus (UCHAR auchTotalStatus[NUM_TOTAL_STATUS], CONST UCHAR uchMinorClass)
*
*   Input:      UCHAR uchMinorClass, UCHAR auchTotalStatus[NUM_TOTAL_STATUS]
*   Output:     UCHAR auchTotalStatus[NUM_TOTAL_STATUS]
*   InOut:      none
*
*   Return:
*
*   Description:    Get the provisioned status data modifying the behavior of a total key.
*                   Specify the total key number, 1 - 20, and the address of the total status
*                   buffer for the total key.  The total status buffer is a UCHAR array
*                   that has NUM_TOTAL_STATUS array elements.  See struct ITEMTOTAL.
*                   The actual size of each block of the memory resident data is MAX_TTLKEY_DATA.
*                   The provisioning is done through P61 action of Program Mode.
*
*                   This function expects that auchTotalStatus is the same type and size of
*                   the struct ITEMTOTAL member auchTotalStatus[NUM_TOTAL_STATUS].
*
*                   auchTotalStatus[0] is used for total key type and is skipped since
*                   that data is not part of the total key provisioning from the
*                   Para address of CLASS_PARATTLKEYCTL but rather from CLASS_PARATTLKEYTYP.
*                   There appear to be various transformation for total key type based
*                   on the FSC used so we are not going to deal with that here.
==========================================================================*/
SHORT  ItemTotalAuchTotalStatus (UCHAR auchTotalStatus[NUM_TOTAL_STATUS], CONST UCHAR uchMinorClass)
{
	SHORT  sWork = 0;
	USHORT usOffset = uchMinorClass;

	if (uchMinorClass < CLASS_TOTAL1 || uchMinorClass > CLASS_TOTAL20) {
		// protect against minor class out of range
		usOffset = CLASS_TOTAL1;
	}
	usOffset = (usOffset - CLASS_TOTAL1) * MAX_TTLKEY_DATA;
	// skip auchTotalStatus[0] which is not part of the total key provisioning
	// accessed through this lookup.  See function ItemTotalTypeIndex().
	CliParaAllRead( CLASS_PARATTLKEYCTL, auchTotalStatus + 1, MAX_TTLKEY_DATA, usOffset, &sWork );
	return 0;
}


UCHAR  ItemTotalTypeIndex (UCHAR uchMinorClass)
{
	PARATTLKEYTYP   WorkType = {0};

	// Total Key Type information is stored in Para.ParaTtlKeyTyp
	// Currently for 20 total keys, there are only 17 addresses because
	// Totals 1,2,9 have a "specified type" already.  For these just return the Total Key number.
	// SubTotal(1)    Check Total(2)       Food Stamp Total (9)
	//
	// See ITM_TTL_GET_TTLTYPE() for calculation and PRT_NOFINALIZE for list of defined constants.
	// To get the Total key address from the relative total key we use the following table
	// ParaTtlKeyTyp Offset::			0	1	2	3	4	5		6	7	8	9	10	11	12	13	14	15	16
	// ParaTtlKeyTyp Address::			1	2	3	4	5	6		7	8	9	10	11	12	13	14	15	16  17
	// Relative Total Key::		1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20
	if (uchMinorClass > CLASS_TOTAL20) {
		uchMinorClass = CLASS_TOTAL1;   // protect against minor class out of range, make it just a Subtotal
	}
	if(uchMinorClass < CLASS_TOTAL3 || uchMinorClass == CLASS_TOTAL9) {
		// if out of range then just use the total key number.
		WorkType.uchTypeIdx = uchMinorClass;  // total key with no ITM_TTL_FLAG_TOTAL_TYPE()
	} else {
		WorkType.uchMajorClass = CLASS_PARATTLKEYTYP;
		if(uchMinorClass <= CLASS_TOTAL8) {
			WorkType.uchAddress = ( UCHAR )( TLTY_NO3_ADR + uchMinorClass - CLASS_TOTAL3 );
		} else if(uchMinorClass >= CLASS_TOTAL10) {
			WorkType.uchAddress = ( UCHAR )( TLTY_NO10_ADR + uchMinorClass - CLASS_TOTAL10);
		}
		CliParaRead( &WorkType );                  /* get target total key */
	}

    return WorkType.uchTypeIdx ;               /* total key type */
}

static SHORT ItemTotalGiftCard (VOID)
{
	TRANGCFQUAL    * const pWorkGCF = TrnGetGCFQualPtr();
	USHORT          usNo=1;
	USHORT          index=0;
	SHORT           error;
	SHORT			sRetStatus = UIF_SUCCESS;

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
		if(pWorkGCF->TrnGiftCard[index].GiftCard == IS_GIFT_CARD_FREEDOMPAY) {
			UIFREGTENDER	UifRegTender = {0};
			ITEMTENDER      ItemTender = {0};
			USHORT          usLdtAddress = 0;
			SHORT           giftType=0;
		
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
				usLdtAddress = LDT_ISSUE;
			}
			else if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID){
				usLdtAddress = LDT_VOID_ISSUE;
			}
			else if(pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_RELOAD){
				usLdtAddress = LDT_RELOAD;
			}
			else if (pWorkGCF->TrnGiftCard[index].GiftCardOperation == GIFT_CARD_VOID_RELOAD){
				usLdtAddress = LDT_VOID_RELOAD;
			}

			RflGetLead (ItemTender.aszTendMnem, usLdtAddress);
			ItemTender.fsPrintStatus = 0;
			ItemTender.fbStorageStatus = 0;

			// We want to check to see if this is a valid account so what we are going
			// to do is to do a balance inquiry and find out if the inquiry is declined
			// or not.  When we actually do the tender, we can then process the requested
			// operation.
			UifRegTender.GiftCardOperation = GIFT_CARD_BALANCE;
			ItemTender.lTenderAmount = 0;
			ItemTender.lRound = 0;

			// check for Track2 Data.
			if(pWorkGCF->TrnGiftCard[index].aszTrack2[0] != 0)
			{
				memcpy (ItemTender.auchMSRData, pWorkGCF->TrnGiftCard[index].aszTrack2, sizeof(ItemTender.auchMSRData));
				RflEncryptByteString ((UCHAR *)ItemTender.auchMSRData, sizeof(ItemTender.auchMSRData));
			}else {
				// if no Track2 Data exists we will copy and send the gift card number instead.
				memcpy (ItemTender.aszNumber, pWorkGCF->TrnGiftCard[index].aszNumber, sizeof(ItemTender.aszNumber));
			}

			error = ItemTendEPTCommGiftCard (EEPT_FC2_EPT, &UifRegTender, &ItemTender, usNo);

			if (error == ITM_SUCCESS) {
				pWorkGCF->TrnGiftCard[index].Used = GIFT_CARD_TESTED;
			}
			else {
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
**   Synopsis:  SHORT   ItemTotal( UIFREGTOTAL *UifRegTotal )
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    total key module main
==========================================================================*/

SHORT   ItemTotal(UIFREGTOTAL *UifRegTotal)
{
	PARATTLKEYORDDEC	OrderDec = {0};
    SHORT           sTotalKeyType, sRetStatus;
	UCHAR           auchTotalStatus[NUM_TOTAL_STATUS];

	if ( UifRegTotal->uchMinorClass == CLASS_UITOTAL2_FS ) {
		//  special check total for Food Stamp after Food Stamp Tender
		// There is a check in function UifTender() which processes a tender key
		// press after processing the tender key press using function ItemTenderEntry()
		// so that if ItemTenderEntry() returns a value of UIF_FS2NORMAL indicating
		// Food Stamp tender to partial payment of transaction with non-food stamp items
		// then a Total Key press with a uchMinorClass of CLASS_UITOTAL2_FS was created
		// and ItemTotal() called.
		//
		// However the processing done for that resulted in an incorrect total being calculated
		// for the Total Key with inconsistent results being printed out on the receipt and in
		// the Electronic Journal since that calculated total amount was incorrect. However the
		// actual total amount as used for calculating change was correct. The incorrect calculation
		// seemed to affect only that generated Total Key press.
		//
		// We are now short circuiting that Total Key generation, doing some of the things done
		// by function ItemTotalCFS(), which was eventually called but not doing the actual Total
		// key press generation.
		//
		//    Richard Chambers, Mar-18-2018
		ITEMTENDERLOCAL *WorkTend = ItemTenderGetLocalPointer();

		if ((WorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) == 0) {
			memset(&WorkTend->ItemFSTotal, 0, sizeof(ITEMTOTAL));
		}
		WorkTend->fbTenderStatus[1] &= ~TENDER_FS_TOTAL;
		return UIF_SUCCESS;
	}

	//Read color data for the item, and send it to the KDS system
	OrderDec.uchMajorClass = CLASS_PARATTLKEYORDERDEC;
	OrderDec.uchAddress = UifRegTotal->uchMinorClass;

	sRetStatus = ItemTotalGiftCard();		/* Generates Gift Card Information   SSTOLTZ*/
	if(sRetStatus != UIF_SUCCESS)
		return sRetStatus;

	//send the request to read the information
	CliParaRead(&OrderDec);
	if(OrderDec.uchOrderDec)
	{
		UIFREGSALES		dummyData = {0};

		sRetStatus = ItemSalesOrderDeclaration(dummyData, OrderDec.uchOrderDec, CLASS_ORDER_DEC_MID);
	}
    if ((sRetStatus = ItemCommonCheckComp()) != ITM_SUCCESS) {
        return(sRetStatus);
    }

	// check to see if this is a Return transaction and if so then
	// at least one item has been marked as a return with a reason code.
	{
		if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
			// this is a return so lets see if any items are being returned.
			// if nothing marked as a return then prohibit.
			if ((ItemSalesLocalPtr->fbSalesStatus & SALES_TRETURN_ITEM) == 0) {
				return(LDT_PROHBT_ADR);           /* return compulsory error */
			}
			// set the indicator so that we can
			// inform other parts of the tender functionality of this fact.
			UifRegTotal->fbModifier |= RETURNS_TRANINDIC;
		}
	}

	/* --- Food Stamp Tender Key Sequence,  Saratoga --- */
	// we now allow a total if doing a partial tender to allow for Amtrak multiple charge tips.
//	{
//		ITEMTENDERLOCAL WorkTend;
//		ItemTenderGetLocal(&WorkTend);
//		if (WorkTend.fbTenderStatus[0] & TENDER_PARTIAL) {
//			return(LDT_SEQERR_ADR);
//		}
//	}
#if 0
	// This source code eliminated as part of changes for JIRA AMT-2853.
	// There are a number of places where there was a check on the flags for
	// a CURQUAL_STORE_RECALL type system along with processing a TENDER_PARTIAL in which the
	// total calculation depended on the system type and if we were doing a split or partial tender.
	// We started with changes in functions TrnItemTotal() and ItemTotalCTAmount() in order
	// to reduce the complexity of the source code and dependencies on flags as much as possible
	// as part of making source code changes for AMT-2853.
	//    Richard Chambers for Amtrak Rel 2.2.1 project June 25, 2013
	// we now allow a total if doing a partial tender by just pretending that we have done
	// something by returning a success.
	{
		ITEMTENDERLOCAL WorkTend;
		ItemTenderGetLocal(&WorkTend);
		if (WorkTend.fbTenderStatus[0] & TENDER_PARTIAL) {
			return(UIF_SUCCESS);
		}
	}
#endif

    if (TranCurQualPtr->fsCurStatus & CURQUAL_WIC) {                /* Saratoga */
        if (ItemModLocalPtr->fsTaxable & (MOD_USEXEMPT | MOD_USTAXMOD)) {
			NHPOS_NONASSERT_NOTE("==OPERATION", "==OPERATION: fsCurStatus & CURQUAL_WIC && fsTaxable & (MOD_USEXEMPT | MOD_USTAXMOD)");
            return(LDT_SEQERR_ADR);
        }
    }

	//we get the total control information for the current total
	//so that we can figure out whether this total key
	//is allowed to evaluate auto coupons. JHHJ
	ItemTotalAuchTotalStatus (auchTotalStatus, UifRegTotal->uchMinorClass);

	// get the total key type for use below and our first check on user entered data.
    sTotalKeyType = ItemTotalType(UifRegTotal);
    if (sTotalKeyType != ITM_TYPE_SERVICE_TOTAL && UifRegTotal->fbInputStatus & INPUT_0_ONLY) {
		// if this is not a Service Total and a value of zero was actually entered by the
		// user then sequence error.
        return(LDT_SEQERR_ADR);
    }

	//if MDC 444 is set to evaluate auto coupons AND
	//the total key is allowed to evaluate auto coupons (ITM_TTL_FLAG_ALLOW_AUTO_COUPON) JHHJ
	if ((CliParaMDCCheck(MDC_AUTOCOUP_ADR, ODD_MDC_BIT0)) && TTL_STAT_FLAG_ALLOW_AUTO_COUPON(auchTotalStatus)) {
  		USHORT    acStatus;

		// use the type of the total key which will influence coupon processing if done.
		// if this is a food stamp total then we do not want to display coupons that do not
		// affect food stamp itemizer. see function ItemAutoCouponFillPopUpList().
		acStatus = ItemAutoCouponCheck(sTotalKeyType);
	}

	{
		SHORT    sRetStatus;

		switch (sTotalKeyType) {
		case    ITM_TYPE_SUBTOTAL:                                      /* subtotal */
			sRetStatus = ItemTotalST(UifRegTotal);
			break;

		case    ITM_TYPE_CHECK_TOTAL:                                   /* check total */
			sRetStatus = ItemTotalCT(UifRegTotal);
			break;

		case    ITM_TYPE_FS_TOTAL:                                      /* food stamp total */
			if (UifRegTotal->fbModifier & RETURNS_TRANINDIC) {
				if (TranItemizersPtr->lMI < 0)
					sRetStatus = LDT_PROHBT_ADR;        // negative total amount but are doing a return so Prohibit.
				else
					sRetStatus = ItemTotalFS(UifRegTotal);
			} else {
				sRetStatus = ItemTotalFS(UifRegTotal);
			}
			break;

		case    ITM_TYPE_CHECK_FS_TOTAL:
			if (UifRegTotal->fbModifier & RETURNS_TRANINDIC) {
				if (TranItemizersPtr->lMI < 0)
					sRetStatus = LDT_PROHBT_ADR;        // negative total amount but are doing a return so Prohibit.
				else
					sRetStatus = ItemTotalCFS(UifRegTotal);                          /* total#2 for FS */
			} else {
				sRetStatus = ItemTotalCFS(UifRegTotal);                          /* total#2 for FS */
			}
			break;

		case    ITM_TYPE_SERVICE_TOTAL:                                 /* service total */
			sRetStatus = ItemTotalSE( UifRegTotal );
			break;

		case    ITM_TYPE_FINALIZE_TOTAL:                                /* finalize total */
			sRetStatus = ItemTotalFT( UifRegTotal );
			break;

		case    ITM_TYPE_NONFINALIZE:                                   /* non-finalize total */
			sRetStatus = ItemTotalNF( UifRegTotal );
			break;

		case    ITM_TYPE_TRAY_TOTAL:
			sRetStatus = ItemTotalTT( UifRegTotal );                       /* tray total */
			break;

		case   ITM_TYPE_SPLIT_KEY:
			sRetStatus = ItemTotalSP(UifRegTotal);                           /* Split Key, R3.1 */
			break;

		case   ITM_TYPE_CASHIER_INTRPT:
			sRetStatus = ItemTotalCI(UifRegTotal);                           /* Cashier Interrupt, R3.3 */
			break;

		default :
			sRetStatus = (LDT_PROHBT_ADR);                                   /* prohibit */
			break;
		}

		{
			// check to see if we are dealing with a partial tender scenario.
			// this change is part of changes for Amtrak to allow multiple charge tips.
			if (sRetStatus == UIF_SUCCESS && ItemTenderLocalPtr->fbTenderStatus[0] & TENDER_PARTIAL) {
				REGDISPMSG      RegDispMsgD = {0};

				/* set and display mnemonic E/C,amount */
				RflGetLead (RegDispMsgD.aszMnemonic, LDT_BD_ADR);

				RegDispMsgD.uchMajorClass = CLASS_UIFREGTENDER;
				RegDispMsgD.uchMinorClass = CLASS_UITENDER1;
				RegDispMsgD.lAmount = TranGCFQualPtr->lCurBalance + ItemCommonLocalPtr->lChargeTip;    /* set amount */ 
				RegDispMsgD.fbSaveControl = 5;           /* set type5, DISP_SAVE_TYPE_5 */
				flDispRegDescrControl |= TENDER_CNTRL;   /* set descriptor*/
				DispWrite( &RegDispMsgD );
			}
		}

		return sRetStatus;
	}
}



/*==========================================================================
**   Synopsis:  SHORT   ItemTotalType( UIFREGTOTAL *UifRegTotal )
*
*   Input:      UIFREGTOTAL *UifRegTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    get total key type
==========================================================================*/
SHORT   ItemTotalType( UIFREGTOTAL *UifRegTotal )
{
    UCHAR   uchType1, uchType2;
    PARATTLKEYTYP   Work;

    if ( UifRegTotal->uchMinorClass == CLASS_UITOTAL1 ) {       /* subtotal */
        return( ITM_TYPE_SUBTOTAL );
    }
    if ( UifRegTotal->uchMinorClass == CLASS_UITOTAL2 ) {       /* check total */
        return( ITM_TYPE_CHECK_TOTAL );
    }
    if ( UifRegTotal->uchMinorClass == CLASS_UITOTAL9 ) {       /* food stamp total */
        return( ITM_TYPE_FS_TOTAL );
    }
    if ( UifRegTotal->uchMinorClass == CLASS_UITOTAL2_FS ) {    /* check total for FS */
        return( ITM_TYPE_CHECK_FS_TOTAL );
    }

    if (UifRegTotal->uchMinorClass == CLASS_UISPLIT) {          /* Split Key #1, R3.1 */
        return(ITM_TYPE_SPLIT_KEY);
    }

    if (UifRegTotal->uchMinorClass == CLASS_UICASINT) {          /* cashier interrupt, R3.3 */
        return(ITM_TYPE_CASHIER_INTRPT);
    }

    if (UifRegTotal->uchMinorClass == CLASS_UIKEYOUT) {          /* server key out, V3.3 */
        return( ITM_TYPE_SERVICE_TOTAL );
    }

    Work.uchMajorClass = CLASS_PARATTLKEYTYP;

	// Total Key Type information is stored in Para.ParaTtlKeyTyp
	// Currently for 20 total keys, there are only 17 addresses because
	// Totals 1,2,9 have a "specified type" already
	// SubTotal(1)    Check Total(2)       Food Stamp Total (9)
	// To get the Total key address from the relative total key we use the following table
	// ParaTtlKeyTyp Address::	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16
	// Relative Total Key::		3	4	5	6	7	8	10	11	12	13	14	15	16	17	18	19	20
	if(UifRegTotal->uchMinorClass <= CLASS_UITOTAL8)
	{
		Work.uchAddress = ( UCHAR )( TLTY_NO3_ADR + UifRegTotal->uchMinorClass - CLASS_UITOTAL3 );
	} else
	{
		Work.uchAddress = ( UCHAR )( UifRegTotal->uchMinorClass - CLASS_UITOTAL3);
	}
    CliParaRead( &Work );                                       /* get target total key */

    uchType1 = ( UCHAR )( Work.uchTypeIdx / CHECK_TOTAL_TYPE );               /* total key type, ITM_TTL_FLAG_TOTAL_TYPE() */
    uchType2 = ( UCHAR )( Work.uchTypeIdx % CHECK_TOTAL_TYPE );               /* total index, ITM_TTL_FLAG_TOTAL_KEY() */

    if ( uchType1 == PRT_FINALIZE1 || uchType1 == PRT_FINALIZE2 ) {           /* finalize total */
        if ( uchType2 > CLASS_TOTAL20 ) {
            return( ITM_TYPE_ERROR );
        }

		if( uchType2 <= CLASS_TOTAL11)
		{
			/* check charge posting, EPT tender */
			if (uchType2 % 2) {         /* odd tender case */
				if (CliParaMDCCheck(( USHORT)(MDC_CPTEND1_ADR + (uchType2 - 1)), EVEN_MDC_BIT0 | EVEN_MDC_BIT1 | EVEN_MDC_BIT2 | EVEN_MDC_BIT3 )) {
					return( ITM_TYPE_ERROR );
				}
			} else {
				if (CliParaMDCCheck(( USHORT)(MDC_CPTEND1_ADR + (uchType2 - 1)), ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3 )) {
					return( ITM_TYPE_ERROR );
				}
			}
		} else
		{
			/* check charge posting, EPT tender */
			if (uchType2 % 2) {         /* odd tender case */
				if (CliParaMDCCheck(( USHORT)(MDC_CPTEND12_ADR + (uchType2 - 12)), EVEN_MDC_BIT0 | EVEN_MDC_BIT1 | EVEN_MDC_BIT2 | EVEN_MDC_BIT3 )) {
					return( ITM_TYPE_ERROR );
				}
			} else {
				if (CliParaMDCCheck(( USHORT)(MDC_CPTEND1_ADR + (uchType2 - 12)), ODD_MDC_BIT0 | ODD_MDC_BIT1 | ODD_MDC_BIT2 | ODD_MDC_BIT3 )) {
					return( ITM_TYPE_ERROR );
				}
			}
		}
    } else {
        if ( uchType2 != 0 ) {                                  /* illegal parameter */
            return( ITM_TYPE_ERROR );
        }
    }

    switch( uchType1 ) {                                        /* total key type */
    case    PRT_NOFINALIZE   :
        return( ITM_TYPE_NONFINALIZE );                         /* non-finalize total */

    case    PRT_SERVICE1   :
    case    PRT_SERVICE2   :
        return( ITM_TYPE_SERVICE_TOTAL );                       /* service total */

    case    PRT_FINALIZE1   :
    case    PRT_FINALIZE2   :
        return( ITM_TYPE_FINALIZE_TOTAL );                      /* finalize total */

    case    PRT_TRAY1   :
    case    PRT_TRAY2   :
        return( ITM_TYPE_TRAY_TOTAL );                          /* tray total */

    default:
        return( ITM_TYPE_ERROR );
    }
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalInit( VOID )
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    initialize transaction information         R3.1
==========================================================================*/

VOID    ItemTotalInit( VOID )
{
	USHORT  usSystemType = RflGetSystemType();

    if (usSystemType == FLEX_STORE_RECALL) {
		FDTPARA     WorkFDT = { 0 };

		FDTParameter(&WorkFDT);
        if (WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2
            || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE
            || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY) {
            MldDisplayInit(MLD_DRIVE_1, 0);
        }
    }

    TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEMIZERS | TRANI_ITEM | TRANI_CONSOLI);
    if (usSystemType == FLEX_POST_CHK
        && !CliParaMDCCheck(MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2)
        && !CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {
        TrnInitializeSpl(TRANI_GCFQUAL_SPLA | TRANI_ITEMIZERS_SPLA | TRANI_CONSOLI_SPLA);
        TrnInitializeSpl(TRANI_GCFQUAL_SPLB | TRANI_ITEMIZERS_SPLB | TRANI_CONSOLI_SPLB);
    }
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSPVInt( ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    supervisor intervention on negative or zero sales, MDC 19 Bit B
==========================================================================*/

SHORT   ItemTotalSPVInt( ITEMTOTAL *ItemTotal )
{
    DCURRENCY    lMI;

	if (TranGCFQualPtr->fsGCFStatus & GCFQUAL_TRETURN) {
		// This is  a return transaction so do not check the negative total MDC
        return( ITM_SUCCESS );
	}

    lMI = ItemTotal->lMI;                                             /* tender amount */
    if( TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK ) {      /* preselect void */
        lMI *= -1;
    }

    if ( CliParaMDCCheck( MDC_SIGNOUT_ADR, ODD_MDC_BIT2 ) == 0 || lMI > 0L ) {
        return( ITM_SUCCESS );
    }

    return( ItemSPVInt(LDT_SUPINTR_ADR) );
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalPrev(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    send previous "item" to transaction module
==========================================================================*/

SHORT   ItemTotalPrev( VOID )
{
    if ( ItemCommonLocalPtr->ItemSales.uchMajorClass == CLASS_ITEMTOTAL ) {
        return( ITM_SUCCESS );
    }

    return(ItemPreviousItem(NULL, 0));       /* ItemTotalPrev():  Send previous item since not CLASS_ITEMTOTAL */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalPrevSpl(VOID)
*
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    send previous "item" to transaction module
==========================================================================*/
SHORT   ItemTotalPrevSpl(VOID)
{
    ITEMCOMMONLOCAL     * const Work = ItemCommonGetLocalPointer();
	ITEMTOTAL           DummyTotal = {0};

    if (Work->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
        return(ITM_SUCCESS);
    }

    /* automatic charge tips, V3.3 */
/*
    if ( Work->ItemSales.uchMajorClass == CLASS_ITEMDISC ) {
        if (( Work->ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP ) ||
            ( Work->ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP2 ) ||
            ( Work->ItemSales.uchMinorClass == CLASS_AUTOCHARGETIP3 )) {

            return( ITM_SUCCESS );
        }
    }
*/
    /* ---- to affect previous partial tender 06/28/96 ---- */
    if (Work->ItemSales.uchMajorClass == CLASS_ITEMTENDER) {
		ITEMTENDER  *pPrevTender = (ITEMTENDER *)&(Work->ItemSales);
		UCHAR       fbStorageSave;
        fbStorageSave = pPrevTender->fbStorageStatus;
        pPrevTender->fbStorageStatus = NOT_CONSOLI_STORAGE;
        pPrevTender->fbStorageStatus |= NOT_SEND_KDS;
        TrnItem(pPrevTender);
        pPrevTender->fbStorageStatus = fbStorageSave;
    }

	return(ItemPreviousItemSpl(&DummyTotal , 0));   /* Saratoga */
}

/*==========================================================================
**  Synopsis:   VOID    ItemTotalStub( ITEMTOTAL *ItemTotal )
*
*   Input:      ITEMTOTAL   *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    print stub receipt
==========================================================================*/

VOID    ItemTotalStub( ITEMTOTAL *ItemTotal )
{
	ITEMTOTAL   WorkTotal = {0};
    ITEMPRINT   WorkPrint = {0};

    WorkTotal.uchMajorClass = ItemTotal->uchMajorClass;
    WorkTotal.uchMinorClass = ( UCHAR )( ItemTotal->uchMinorClass + CLASS_STUB_OFFSET );   /* stub */

    WorkTotal.fsPrintStatus = PRT_RECEIPT;                          /* receipt */

    WorkTotal.lMI = ItemTotal->lService;

    memcpy( &WorkTotal.auchTotalStatus[0], &ItemTotal->auchTotalStatus[0], sizeof( WorkTotal.auchTotalStatus ) );

    /* -- restore print flags -- */
    WorkPrint.uchMajorClass = CLASS_ITEMPRINT;
    WorkPrint.uchMinorClass = CLASS_RESTART;
    TrnThrough( &WorkPrint );

    ItemHeader( TYPE_STB_TRAIL );                               /* header print */
    TrnThrough( &WorkTotal );                                   /* stub print */
    ItemTrailer( TYPE_STB_TRAIL );                              /* trailer print */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalSysType(ITEMTOTAL *ItemTotal)
*
*   Input:      ITEMTOTAL *ItemTotal
*   Output:     none
*   InOut:      none
*
*   Return:
*
*   Description:    Determine the total key print type by looking at various
*                   system parameters. The decision is based on:
*                    - System Type (Post Guest Check, Store Recall, etc.)
*                    - MDC 12 Bit A setting to "Issue Soft Check Print by Total Key." or not.
*                    - whether Print Buffered is set for the individual total key
*
*                    See also function ItemTotalSEClose() which uses this to determine
*                    the kind of print storage.
==========================================================================*/

SHORT   ItemTotalSysType(ITEMTOTAL *ItemTotal)
{
    if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_BUFFER) {
        return(0);
    } else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) {
        return(1);
    } else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK) {
		// System type is Post Guest Check so look at MDC 12 Bit A to determine whether we
		// are to "Issue Soft Check Print by Total Key." or not.
        if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {         /* Postcheck */
            if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {          /* if total key is Buffering Print */
                return(2);
            } else {                                                /* Not Print */
                return(3);
            }
        } else {                                                    /* Softcheck */
            if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {          /* if total key is Buffering Print */
                return(4);
            } else {                                                /* Not Print */
                return(5);
            }
        }
    } else if ((TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL) {                                                        /* Store/Recall */
		// System type is Store Recall so look at MDC 12 Bit A to determine whether we
		// are to "Issue Soft Check Print by Total Key." or not.
        if (CliParaMDCCheck(MDC_GCNO_ADR, EVEN_MDC_BIT3)) {         /* Not Print */
            return(6);
        } else {
            if (ITM_TTL_FLAG_PRINT_BUFFERING(ItemTotal)) {          /* if total key is Buffering Print */
                return(7);                                          /* Print */
            } else {
                return(8);                                          /* Not Print */
            }
        }
    }
	NHPOS_ASSERT_TEXT(0, "**ERROR: Incorrect System Type in (TranCurQualPtr->flPrintStatus & CURQUAL_GC_SYS_MASK).");
    return(10);                                          /* bogus return value */
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTotalAutoChargeTips(VOID)
*
*   Input:      None
*   Output:
*   InOut:      nothing
*
*   Return:
*
*   Description:    Create automatic charge tips data, not display, V3.3
==========================================================================*/
SHORT   ItemTotalAutoChargeTips(VOID)
{
	UIFREGDISC  UifRegDisc = {0};

    UifRegDisc.uchMajorClass = CLASS_UIFREGDISC;
    UifRegDisc.uchMinorClass = CLASS_UIAUTOCHARGETIP;
    return (ItemDiscount(&UifRegDisc));     /* Discount Modele */

}

/*==========================================================================
**  Synopsis:   VOID   ItemTotalEuroTender(VOID)
*
*   Input:      None
*   Output:
*   InOut:      nothing
*
*   Return:
*
*   Description:    generate Euro trailer amount format, V3.4
==========================================================================*/
VOID   ItemTotalEuroTender(VOID)
{
	ITEMTENDER ItemTender = {0};

    ItemTender.uchMajorClass = CLASS_ITEMTENDER;
    ItemTendEuroTender(&ItemTender);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTotalCalTax(ITEMAFFECTION *ItemAffection, SHORT sType)
*
*   Input:      ITEMAFFECTION   *ItemAffection  - Affection data
*               SHORT           sType           - transaction type as to wheter Operator or Store/Recall
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:
*
*   Description:    calculate US tax (with / F.S tax exempt)
*
*                   The value of sType should be ITM_CALTAX_TRANS_OPER
*                   or ITM_CALTAX_TRANS_SR indicating whether this is an
*                   operator transaction or a Store/Recall transaction.
*==========================================================================
*/
VOID    ItemTotalCalTax(ITEMAFFECTION *pAffect, SHORT sType)
{
    SHORT           i;
    CONST TRANITEMIZERS   *WorkTI;

    /*----- Split Check,   R3.1 -----*/
	if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {  // TrnSplCheckSplit()
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }
    /* TrnGetTI(&WorkTI); */
    if(sType == 0) {                /* Operator Transaction */
        for (i = 0; i < 3; i++) {
            pAffect->USCanVAT.USCanTax.lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
        }
    } else {                        /* Store/Recall Transaction */
        for (i = 0; i < 3; i++) {
            pAffect->USCanVAT.USCanTax.lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
            pAffect->USCanVAT.USCanTax.lTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
        }
    }

    /* --- FS partial tender performed --- */
    if (ItemTenderLocalPtr->fbTenderStatus[1] & TENDER_FS_PARTIAL) {
        if (CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT2)) {
            /* --- FS tendered amount --- */
            if (CliParaMDCCheck(MDC_FS1_ADR, ODD_MDC_BIT3)) {
                ItemTotalTaxTend(pAffect, sType);
            } else {
				/* --- taxable SI, Maryland --- */
                ItemTotalTaxMary(pAffect, sType);
            }
        } else {
			/* --- proportional --- */
            ItemTotalTaxProp(pAffect, sType);
        }
    } else {
		/* non-food stamp transaction */
        ItemTotalTaxNor(pAffect);
    }

    if(sType == 1) {                /* Store/Recall Transaction */
        for (i = 0; i < 3; i++) {
            pAffect->USCanVAT.USCanTax.lTaxable[i] -= WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
            pAffect->USCanVAT.USCanTax.lTaxAmount[i] -= WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
        }
    }
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTotalTaxNor(ITEMAFFECTION *pAffect)
*
*   Input:      ITEMAFFECTION   *ItemAffection
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:
*
*   Description:    calculate US tax (non-food stamp transaction)
*                   TAR191447 The amount that prints on the check endorsement is incorrect.
==========================================================================
*/
VOID    ItemTotalTaxNor(ITEMAFFECTION *pAffect)
{
    SHORT           sStatus;
    CONST ITEMTENDERLOCAL *WorkTend = ItemTenderGetLocalPointer();

    if ((!WorkTend->ItemFSTotal.uchMinorClass && !WorkTend->ItemTotal.uchMinorClass) ||	/* FS toal key */
    	(WorkTend->ItemFSTotal.uchMinorClass) ||	/* FS tender */
        (WorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) || /* FS tender */
	    (!WorkTend->ItemFSTotal.uchMinorClass && WorkTend->ItemTotal.uchMinorClass && WorkTend->ItemTotal.auchTotalStatus[1] & 0x01)) {	/* non FS tender */

		PARATAXRATETBL  WorkRate;

		WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
	    WorkRate.uchAddress    = TXRT_NO1_ADR;
    	CliParaRead(&WorkRate);
	    if (WorkRate.ulTaxRate != 0L) {
    	    RflRateCalc3(&pAffect->USCanVAT.USCanTax.lTaxAmount[0], pAffect->USCanVAT.USCanTax.lTaxable[0], WorkRate.ulTaxRate, RND_TAX1_ADR);
	    } else {
    	    sStatus = RflTax1(&pAffect->USCanVAT.USCanTax.lTaxAmount[0], pAffect->USCanVAT.USCanTax.lTaxable[0], RFL_USTAX_1);
	        if (sStatus != RFL_SUCCESS) {
    	        pAffect->USCanVAT.USCanTax.lTaxAmount[0] = 0L;
        	}
	    }

	}

    if ((!WorkTend->ItemFSTotal.uchMinorClass && !WorkTend->ItemTotal.uchMinorClass) ||	/* FS toal key */
    	(WorkTend->ItemFSTotal.uchMinorClass) ||	/* FS tender */
        (WorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) || /* FS tender */
	    (!WorkTend->ItemFSTotal.uchMinorClass && WorkTend->ItemTotal.uchMinorClass && WorkTend->ItemTotal.auchTotalStatus[1] & 0x10)) {	/* non FS tender */

		PARATAXRATETBL  WorkRate;

		WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
	    WorkRate.uchAddress = TXRT_NO2_ADR;
    	CliParaRead(&WorkRate);
	    if (WorkRate.ulTaxRate != 0L) {
    	    RflRateCalc3(&pAffect->USCanVAT.USCanTax.lTaxAmount[1], pAffect->USCanVAT.USCanTax.lTaxable[1], WorkRate.ulTaxRate, RND_TAX1_ADR);
	    } else {
    	    sStatus = RflTax1(&pAffect->USCanVAT.USCanTax.lTaxAmount[1], pAffect->USCanVAT.USCanTax.lTaxable[1], RFL_USTAX_2);
	        if (sStatus != RFL_SUCCESS) {
    	        pAffect->USCanVAT.USCanTax.lTaxAmount[1] = 0L;
        	}
	    }

	}

    if ((!WorkTend->ItemFSTotal.uchMinorClass && !WorkTend->ItemTotal.uchMinorClass) ||	/* FS toal key */
    	(WorkTend->ItemFSTotal.uchMinorClass) ||	/* FS tender */
        (WorkTend->fbTenderStatus[1] & TENDER_FS_PARTIAL) || /* FS tender */
	    (!WorkTend->ItemFSTotal.uchMinorClass && WorkTend->ItemTotal.uchMinorClass && WorkTend->ItemTotal.auchTotalStatus[2] & 0x01)) {	/* non FS tender */

		PARATAXRATETBL  WorkRate;

		WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
	    WorkRate.uchAddress = TXRT_NO3_ADR;
    	CliParaRead(&WorkRate);
	    if (WorkRate.ulTaxRate != 0L) {
    	    RflRateCalc3(&pAffect->USCanVAT.USCanTax.lTaxAmount[2], pAffect->USCanVAT.USCanTax.lTaxable[2], WorkRate.ulTaxRate, RND_TAX1_ADR);
	    } else {
    	    sStatus = RflTax1(&pAffect->USCanVAT.USCanTax.lTaxAmount[2], pAffect->USCanVAT.USCanTax.lTaxable[2], RFL_USTAX_3);
	        if (sStatus != RFL_SUCCESS) {
    	        pAffect->USCanVAT.USCanTax.lTaxAmount[2] = 0L;
        	}
	    }
	}
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTotalTaxTend(ITEMAFFECTION *pAffect, SHORT sType)
*
*   Input:      ITEMAFFECTION   *ItemAffection
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:
*
*   Description:    Function of U.S Tax
*                   F.S tax exempt calculation (F.S tendered amount)
*==========================================================================
*/
VOID    ItemTotalTaxTend(ITEMAFFECTION *pAffect, SHORT sType)
{
    SHORT           i, sSign = 1;
	DCURRENCY       alTaxable[STD_TAX_ITEMIZERS_MAX] = {0};
    CONST ITEMTENDERLOCAL *WorkTend = ItemTenderGetLocalPointer();
    CONST TRANITEMIZERS   *WorkTI;

    /*----- Split Check,   R3.1 -----*/
	if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {  // TrnSplCheckSplit()
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    if(sType == 0) {                /* Operator Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
        }
    } else {                        /* Store/Recall Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
        }
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        sSign = -1;
    }

    /*----- FoodStamp Tax Exempt, V6 -----*/
    for (i = 0; i < 3; i++) {
        if (alTaxable[i] * sSign >= 0L) {
            if (WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i] * sSign >= WorkTend->lFSTenderizer * sSign) {
                pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - WorkTend->lFSTenderizer;
                pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTend->lFSTenderizer;
            } else {
                pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
                pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
            }
        }
    }

    ItemTotalTaxNor(pAffect);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTotalTaxMary(ITEMAFFECTION *pAffect, SHORT sType)
*
*   Input:      ITEMAFFECTION   *ItemAffection
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:
*
*   Description:    Function of U.S Tax
*                   F.S tax exempt calculation(Maryland method, taxable SI)
*==========================================================================
*/
VOID    ItemTotalTaxMary(ITEMAFFECTION *pAffect, SHORT sType)
{
    SHORT           i, sSign = 1;
	DCURRENCY       alTaxable[STD_TAX_ITEMIZERS_MAX] = {0};
    CONST TRANITEMIZERS   *WorkTI;

    /*----- Split Check,   R3.1 -----*/
	if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {  // TrnSplCheckSplit()
        WorkTI = TrnGetTISplPointer(TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

    if(sType == 0) {                /* Operator Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
        }
    } else {                        /* Store/Recall Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
        }
    }

    if (TranCurQualPtr->fsCurStatus & CURQUAL_PRESELECT_MASK) {
        sSign = -1;
    }

    /*----- FoodStamp Tax Exempt, V6 -----*/
    for (i = 0; i < 3; i++) {
        if (alTaxable[i] * sSign >= 0L
            && WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i] * sSign >= 0L) {
            pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
            pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
        }
    }

    ItemTotalTaxNor(pAffect);
}

/*
*==========================================================================
**   Synopsis:  VOID    ItemTotalTaxProp(ITEMAFFECTION *pAffect, SHORT sType)
*
*   Input:      ITEMAFFECTION   *ItemAffection
*   Output:     ITEMAFFECTION   *ItemAffection
*   InOut:      none
*
*   Return:
*
*   Description:    F.S tax exempt calculation (proportional)
*==========================================================================
*/
VOID    ItemTotalTaxProp(ITEMAFFECTION *pAffect, SHORT sType)
{
    SHORT           i, sSign = 1;
	DCURRENCY       alTaxable[STD_TAX_ITEMIZERS_MAX] = {0};
    ITEMTENDERLOCAL * const pWorkTend = ItemTenderGetLocalPointer();
    CONST TRANITEMIZERS   *WorkTI;

    /*----- Split Check,   R3.1 -----*/
	if (TrnSplCheckSplit() & (TRN_SPL_SEAT | TRN_SPL_MULTI | TRN_SPL_TYPE2)) {  // TrnSplCheckSplit()
        WorkTI = TrnGetTISplPointer (TRN_TYPE_SPLITA);
    } else {
        WorkTI = TrnGetTIPtr();
    }

	if(sType == 0) {                /* Operator Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
        }
    } else {                        /* Store/Recall Transaction */
        for (i = 0; i < 3; i++) {
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lNonAffectTaxable[i];
            alTaxable[i] += WorkTI->Itemizers.TranUsItemizers.lAffectTaxable[i];
        }
    }

    if (TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) {
        sSign = -1;
    }

    if (pWorkTend->lFSTenderizer * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodWholeTax * sSign
        || WorkTI->Itemizers.TranUsItemizers.lFoodWholeTax == 0L) {

        /*----- FoodStamp Tax Exempt, V6 -----*/
        for (i = 0; i < 3; i++) {
            if (WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i] * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
                pAffect->USCanVAT.USCanTax.lTaxable[i] -= WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTI->Itemizers.TranUsItemizers.lFoodStampable;

            } else {
                pAffect->USCanVAT.USCanTax.lTaxable[i] -= WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
                pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
            }
        }
    } else {
        for (i = 0; i < 3; i++) {
            if (WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i] * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodWholeTax * sSign) {
                if (pWorkTend->lFSTenderizer * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
                    pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                    pAffect->USCanVAT.USCanTax.lFSExempt[i] = WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                } else {
                    pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - pWorkTend->lFSTenderizer;
                    pAffect->USCanVAT.USCanTax.lFSExempt[i] = pWorkTend->lFSTenderizer;
                }
            } else {
				D13DIGITS  huWork;
                if (pWorkTend->lFSTenderizer * sSign > WorkTI->Itemizers.TranUsItemizers.lFoodStampable * sSign) {
                    /* TAR 111547 */
                    huWork = WorkTI->Itemizers.TranUsItemizers.lFoodStampable;
                    huWork *= WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
                    huWork /= WorkTI->Itemizers.TranUsItemizers.lFoodWholeTax;
                    pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - huWork;
                    pAffect->USCanVAT.USCanTax.lFSExempt[i] = huWork;
                } else {
                    /* TAR 111547 */
                    huWork = pWorkTend->lFSTenderizer;
                    huWork *= WorkTI->Itemizers.TranUsItemizers.lFoodTaxable[i];
                    huWork /= WorkTI->Itemizers.TranUsItemizers.lFoodWholeTax;
                    pAffect->USCanVAT.USCanTax.lTaxable[i] = alTaxable[i] - huWork;
                    pAffect->USCanVAT.USCanTax.lFSExempt[i] = huWork;
                }
            }
        }
    }

    ItemTotalTaxNor(pAffect);
}

/****** end of source ******/