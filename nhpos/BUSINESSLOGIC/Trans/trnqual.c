/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         
:   Category           :                                                 
:   Program Name       : TRNQUAL.C                                       
:*------------------------------------------------------------------------
:   Abstract         
:       TrnQual()               ;   transaction qualifier 
:
:*------------------------------------------------------------------------
:   Update Histories                                                      
:       Date    :   Version/Revision    :   Name    :   Description      
:       92/5/14     00.00.01                hkato              
:       94/3/22     00.00.04                Kyou      add flex GC file(mod. TrnQualClsCashier
:                                                     TrnQualClsWaiter)
:       95/5/02     03.00.00              TNakahata   add flex drive thru
:                                                    (mod. TrnQualTrans)
:       95/5/12     03.00.00              TNakahata   add unique trans #
:                                                    (add  TrnQualTransCash )
:                                                    (mod. TrnQualTransRO )
:       95/7/19     03.00.04              TNakahata    FVT comment
:                                                    (add TrnQualModCustName)
:       95/7/26     03.00.04              TNakahata   save # of Person to
:                                                   GCQual at Store/Recall
:       95/12/1     03.01.00                hkato     R3.1
:
:       99/08/13    03.05.00               K.Iwata    R3.5
:                                                     ( merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL)
:       99/12/02    01.00.00                hrkato  Saratoga
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <stdio.h>

#include    <ecr.h>
#include	"pif.h"
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    "paraequ.h"
#include    <para.h>
#include    <prt.h>
#include	<rfl.h>
#include	"cscas.h"


/*==========================================================================
*    Synopsis:  VOID    TrnQual( VOID *TrnQual )    
*
*    Input:     VOID    *TrnQual
*    Output:    none
*    InOut:     none
*               
*    Return:    none
*
*    Description:   transaction qualifier method            R3.1
==========================================================================*/

VOID    TrnQual(VOID *TrnQual)
{
    switch(ITEMSTORAGENONPTR(TrnQual)->uchMajorClass) {
    case CLASS_ITEMOPEOPEN:
        TrnQualOpn(TrnQual);
        break;

    case CLASS_ITEMTRANSOPEN:
        TrnQualTrans(TrnQual);
        break;

    case CLASS_ITEMTOTAL:
        TrnQualTotal(TrnQual);
        break;

    case CLASS_ITEMMODIFIER:
        TrnQualModifier(TrnQual);
        break;

    case CLASS_ITEMOPECLOSE:
        TrnQualCls(TrnQual);
        break;

    case CLASS_ITEMSALES:
    case CLASS_ITEMDISC:
    case CLASS_ITEMCOUPON:
        TrnQualItems(TrnQual);
        break;

    case CLASS_ITEMOTHER:               /* Saratoga */
        TrnQualOther(TrnQual);
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "==ERROR: TrnQual() unknown uchMajorClass");
        break;
    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnQualOpnCashier( ITEMOPEOPEN  *ItemOpeOpen )
*
*    Input:     ITEMOPEOPEN *ItemOpeOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by cashier open, V3.3
==========================================================================*/

static VOID    TrnQualOpnCashier( ITEMOPEOPEN *ItemOpeOpen )
{
	TrnInformation.TranModeQual.sCasSignInKey = ItemOpeOpen->uchMinorClass;             /* Sign-in key used for the Sign-in action */
    TrnInformation.TranModeQual.ulCashierID = ItemOpeOpen->ulID;               /* cashier ID */
    TrnInformation.TranModeQual.ulCashierSecret = ItemOpeOpen->ulSecret;     /* cashier secret code */
    TrnInformation.TranModeQual.fbCashierStatus = ItemOpeOpen->fbCashierStatus;       /* cashier status */
    _tcsncpy( &TrnInformation.TranModeQual.aszCashierMnem[0], &ItemOpeOpen->aszMnemonic[0], NUM_OPERATOR );

    memcpy( &TrnInformation.TranModeQual.auchCasStatus[CAS_CASHIERSTATUS_0], &ItemOpeOpen->auchStatus[0], sizeof(ItemOpeOpen->auchStatus)); /* saratoga */
    TrnInformation.TranModeQual.usFromTo[0] = ItemOpeOpen->usFromTo[0];                  /* GCF number from */
    TrnInformation.TranModeQual.usFromTo[1] = ItemOpeOpen->usFromTo[1];                  /* GCF number to */
    TrnInformation.TranModeQual.uchCasChgTipRate = ItemOpeOpen->uchChgTipRate;           /* charge tips rate */
    TrnInformation.TranModeQual.uchCasTeamNo = ItemOpeOpen->uchTeamNo;                   /* team no */
    TrnInformation.TranModeQual.usSupervisorID = ItemOpeOpen->usSupervisorID;            /* supervisor id associated with this waiter */
    TrnInformation.TranModeQual.usCtrlStrKickoff = ItemOpeOpen->usCtrlStrKickoff;        /* startup control string to run with Sign-in */
    TrnInformation.TranModeQual.usStartupWindow = ItemOpeOpen->usStartupWindow;          /* startup window to display with Sign-in */
	TrnInformation.TranModeQual.ulGroupAssociations = ItemOpeOpen->ulGroupAssociations;

    /* save cashier interrupt status, R3.3 */
    TrnInformation.TranModeQual.fsModeStatus &= ~MODEQUAL_CASINT;
    switch( ItemOpeOpen->uchMinorClass ) {
    case CLASS_CASINTIN :                                       /* cashier A sign-in */
    case CLASS_CASINTB_IN :                                     /* cashier B sign-in */
        TrnInformation.TranModeQual.fsModeStatus |= MODEQUAL_CASINT;
		break;
    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnQualOpnWaiter( ITEMOPEOPEN *ItemOpeOpen )
*
*    Input:     ITEMOPEOPEN *ItemOpeOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by waiter open, V3.3
==========================================================================*/
static VOID    TrnQualOpnWaiter( ITEMOPEOPEN *ItemOpeOpen )
{
	// We need to have a waiter/bartender/surrogate specific area of data because this is
	// used for bartender or surrogate Sign-in which is done with an active Cashier Sign-in state.
    TrnInformation.TranModeQual.ulWaiterID = ItemOpeOpen->ulID;            /* waiter ID */
    TrnInformation.TranModeQual.fbWaiterStatus = ItemOpeOpen->fbCashierStatus;    /* waiter status */
    TrnInformation.TranModeQual.fbCashierStatus |= (ItemOpeOpen->fbCashierStatus & MODEQUAL_WAITER_LOCK);    /* waiter status */
    _tcsncpy( &TrnInformation.TranModeQual.aszWaiterMnem[0], &ItemOpeOpen->aszMnemonic[0], NUM_OPERATOR );

    memcpy( &TrnInformation.TranModeQual.auchWaiStatus[0], &ItemOpeOpen->auchStatus[0], sizeof(ItemOpeOpen->auchStatus)); /* saratoga */
    TrnInformation.TranModeQual.uchWaiChgTipRate = ItemOpeOpen->uchChgTipRate;   /* charge tips rate */
	TrnInformation.TranModeQual.ulWaiGroupAssociations = ItemOpeOpen->ulGroupAssociations;
    TrnInformation.TranModeQual.usWaiCtrlStrKickoff = ItemOpeOpen->usCtrlStrKickoff;        /* startup control string to run with Sign-in */
    TrnInformation.TranModeQual.usWaiStartupWindow = ItemOpeOpen->usStartupWindow;          /* startup window to display with Sign-in */

	/*
	* The following memory resident data items are for Cashier/Operator Sign-in only.
	*
    * TrnInformation.TranModeQual.usFromTo[0] = ItemOpeOpen->usFromTo[0];                  / GCF number from /
    * TrnInformation.TranModeQual.usFromTo[1] = ItemOpeOpen->usFromTo[1];                  / GCF number to /
    * TrnInformation.TranModeQual.uchCasTeamNo = ItemOpeOpen->uchTeamNo;                   / team no /
	* TrnInformation.TranModeQual.sCasSignInKey = ItemOpeOpen->uchMinorClass;              / Sign-in key used for the Sign-in action /
    * TrnInformation.TranModeQual.usSupervisorID = ItemOpeOpen->usSupervisorID;            / supervisor id associated with this waiter /
    * TrnInformation.TranModeQual.usCtrlStrKickoff = ItemOpeOpen->usCtrlStrKickoff;        / startup control string to run with Sign-in /
    * TrnInformation.TranModeQual.usStartupWindow = ItemOpeOpen->usStartupWindow;          / startup window to display with Sign-in /
	* TrnInformation.TranModeQual.ulGroupAssociations = ItemOpeOpen->ulGroupAssociations;
	*/

	TrnInformation.TranModeQual.fsModeStatus &= ~ MODEQUAL_BARTENDER;
    if (ItemOpeOpen->uchMinorClass == CLASS_BARTENDERIN) {
        TrnInformation.TranModeQual.fsModeStatus |= MODEQUAL_BARTENDER;
    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnQualOpn( ITEMOPEOPEN *ItemOpeOpen )    
*
*    Input:     ITEMOPEOPEN *ItemOpeOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by operator open
==========================================================================*/

VOID    TrnQualOpn( ITEMOPEOPEN *ItemOpeOpen )
{                                                               
    switch( ItemOpeOpen->uchMinorClass ) {

    case CLASS_CASHIERIN :                                      /* cashier sign-in */
    case CLASS_B_IN :                                           /* cashier B sign-in */
    case CLASS_CASINTIN :                                       /* cashier sign-in, R3.3 */
    case CLASS_CASINTB_IN :                                     /* cashier B sign-in, R3.3 */
        TrnQualOpnCashier( ItemOpeOpen );                            
        break;

    case CLASS_WAITERIN :                                       /* waiter sign-in */
    case CLASS_WAITERLOCKIN :                                   /* waiter lock sign-in */
    case CLASS_BARTENDERIN :                                    /* bartender sign-in,   R3.1 */
        TrnQualOpnWaiter( ItemOpeOpen );
        break;

    default:
        break;
    }
}

/*==========================================================================
*    Synopsis:  VOID    TrnQualTrans( ITEMTRANSOPEN *ItemTransOpen )    
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by transaction open             R3.1
==========================================================================*/

VOID    TrnQualTrans(ITEMTRANSOPEN *ItemTransOpen)
{
    if (ItemTransOpen->uchSeatNo != 0) {
        TrnInformation.TranGCFQual.fsGCFStatus2 |= GCFQUAL_USESEAT;
    }
    TrnInformation.TranCurQual.uchSeat = ItemTransOpen->uchSeatNo;

    switch(ItemTransOpen->uchMinorClass) {
    case CLASS_NEWCHECK:                                    /* newcheck operation */
    case CLASS_CASINTOPEN:                                  /* cashier interrupt open, R3.3 */
        TrnQualTransNC(ItemTransOpen);
        break;

	case CLASS_OPEN_TRANSBATCH:
    case CLASS_REORDER:                                     /* reorder */
    case CLASS_MANUALPB:                                    /* manual PB */
    case CLASS_ADDCHECK:                                    /* addcheck */
    case CLASS_CASINTRECALL:                                /* cashier interrupt recall, R3.3 */
        TrnQualTransRO(ItemTransOpen); 
        break;


    case CLASS_STORE_RECALL:                                /* store/recall trans R3.0 */
        TrnQualTransSR(ItemTransOpen);
        break;

    case CLASS_CASHIER:                                     /* cashier transaction R3.0 */
        TrnQualTransCash(ItemTransOpen);
        break;

    case CLASS_STORE_RECALL_DELIV:
        break;

    case CLASS_OPEN_LOAN:                                   /* loan transaction    2172 */
    case CLASS_OPEN_PICK:                                   /* pick-up transaction 2172 */
        TrnQualLoanPick(ItemTransOpen);
        break;

    default:
        break;
    }
}


/*==========================================================================
*    Synopsis:  VOID    TrnQualTransNC( ITEMTRANSOPEN *ItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by newcheck                     R3.1
*
*                   NOTE: this is Guest Check specific data that is stored in
*                         the TranGCFQual memory area. The TranGCFQual memory
*                         area is stored in the Guest Check File along with the
*                         transaction data.
*
*                         There is some duplication between the TranGCFQual
*                         and TranModeQual memory resident data in order to
*                         save state when the Guest Check is saved.
*
*                         See functions TrnSaveGC() and TrnGetGC().
==========================================================================*/

VOID    TrnQualTransNC( ITEMTRANSOPEN *ItemTransOpen )
{
    TrnInformation.TranGCFQual.ulCashierID = ItemTransOpen->ulCashierID;
//    TrnInformation.TranGCFQual.usWaiterID = ItemTransOpen->usWaiterID;
    TrnInformation.TranGCFQual.usGuestNo = ItemTransOpen->usGuestNo;
    TrnInformation.TranGCFQual.uchCdv = ItemTransOpen->uchCdv;
	if (ItemTransOpen->usTableNo) {
		// if a table number is specified then we will save it otherwise do not
		// modify the TranGCFQual.usTableNo since in some cases it is also used
		// to contain a value generated by ItemCommonUniqueTransNo() however for
		// Table Service there may be entry of a table number.  See uses of
		// usTableNo with CLASS_NEWCHECK, CLASS_TABLENO
		TrnInformation.TranGCFQual.usTableNo = ItemTransOpen->usTableNo;
	}
    TrnInformation.TranGCFQual.usNoPerson = ItemTransOpen->usNoPerson;
    TrnInformation.TranGCFQual.usNoPersonMax = ItemTransOpen->usNoPerson;
    TrnInformation.TranGCFQual.uchSlipLine = ItemTransOpen->uchLineNo;
//    TrnInformation.TranGCFQual.auchCheckOpen[0] = ItemTransOpen->auchCheckOpen[0];
//    TrnInformation.TranGCFQual.auchCheckOpen[1] = ItemTransOpen->auchCheckOpen[1];
//    TrnInformation.TranGCFQual.auchCheckOpen[2] = ItemTransOpen->auchCheckOpen[2];
    TrnInformation.TranGCFQual.uchTeamNo = ItemTransOpen->uchTeamNo;    /* V3.3 */
//    memcpy (TrnInformation.TranGCFQual.auchCheckHashKey, ItemTransOpen->auchCheckHashKey, 6);
}

/*
*===========================================================================
** Synopsis:    VOID TrnGetTransactionInvoiceNum(TCHAR *tchRefNum)
*     Input:    
*     Output:   reference number string of 
*     INOut:    nothing
*
** Return:      
*
** Description: Provides the Invoice Number for this transaction
*===========================================================================
*/
UCHAR *TrnGetTransactionInvoiceNum(UCHAR *auchRefNum)
{
	strncpy (auchRefNum, TrnInformation.invno.auchInvoiceNo, NUM_INVOICE_NUM);

	return auchRefNum;
}

void TrnSaveTransactionInvoiceNum(ITEMTENDER *pTenderToSave)
{
	if (pTenderToSave) {
		TrnInformation.tenderLastTender = *pTenderToSave;
		// we need to store the current consecutive number some place so we are using usReasonCode.
		TrnInformation.tenderLastTender.usReasonCode = TrnInformation.TranGCFQual.usConsNo;
	}
	TrnInformation.invnoLastTender = TrnInformation.invno;   // save the current invoice number in case needed for a retry.
}

// if there is a valid tender saved and invoice number then retrieve them.
// if an address to receive the tender information is not specified, is zero, then just return status.
// return 0 == nothing, 1 == something but no tender, 2 == something but no tender match, 3 == something and a match and changed.
SHORT TrnGetTransactionInvoiceNumTenderIfValid(ITEMTENDER *pTender)
{
	SHORT  sRet = 0;  // assume there is nothing to return

	if (TrnInformation.tenderLastTender.uchMajorClass && TrnInformation.invnoLastTender.auchInvoiceNo[0]) {
		sRet = 1;    // indicate that there is something to return but it may not be the right thing
		if (pTender) {
			if (pTender->uchMajorClass != 0) {
				// since an ITEMTENDER object is provided for comparison, lets see if the provided
				// ITEMTENDER object is the same as what was last stored.
				int iTest = 0;

				sRet = 2;  // indicate something to return but the tender information does not match

				// We want to determine if this ITEMTENDER object is similar to the saved  ITEMTENDER object
				// so we will perform a couple of tests. These tests will pass if the provided ITEMTENDER object
				// is probably a repeated attempt to do the same tender as the saved ITEMTENDER object.
				iTest  = pTender->uchMajorClass == TrnInformation.tenderLastTender.uchMajorClass;
				iTest &= pTender->uchMinorClass == TrnInformation.tenderLastTender.uchMinorClass;
				iTest &= pTender->usCheckTenderId == TrnInformation.tenderLastTender.usCheckTenderId;
				// we store the current consecutive number in usReasonCode in function TrnSaveTransactionInvoiceNum() above.
				iTest &= TrnInformation.tenderLastTender.usReasonCode == TrnInformation.TranGCFQual.usConsNo;
				if (!iTest)
					return sRet;
			}
			sRet = 3;  // indicate something to return and the tender information did match or no tender info provided.
		}
	}
	return sRet;
}

// clear the storage area for saving the last tender and invoice number. this is used to retry an Electronic tender
// if there is a problem such as a time out for the last tender request. we indicate that there is nothing to
// retry by clearing this data area.
VOID TrnClearSavedTransactionInvoiceNum(VOID)
{
	memset(&TrnInformation.tenderLastTender, 0, sizeof(TrnInformation.tenderLastTender));
	memset(&TrnInformation.invnoLastTender, 0, sizeof(TrnInformation.invnoLastTender));
}

UCHAR *TrnGenerateTransactionInvoiceNum(UCHAR *pauchRefNum)
{
	int               i;
	UCHAR             *auchRefNum = TrnInformation.invno.auchInvoiceNo;
	PARATERMINALINFO  TermInfo;
	DATE_TIME         DateTime;

	PifGetDateTime(&DateTime);

	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PifSysConfig()->usTerminalPosition;
 
	ParaTerminalInfoParaRead (&TermInfo);

	//Store No. 0-9999
	i = sprintf (auchRefNum, "%4.4d", (TermInfo.TerminalInfo.usStoreNumber % 10000));

	//Regiser No. 0-99
	i += sprintf (auchRefNum + i, "%2.2d", (TermInfo.TerminalInfo.usRegisterNumber % 100));

	// Date time stamp
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usMonth);
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usMDay);
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usHour);
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usMin);
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usSec);

	if (pauchRefNum)
		strncpy (pauchRefNum, TrnInformation.invno.auchInvoiceNo, NUM_INVOICE_NUM);

	return pauchRefNum;
}

UCHAR *TrnGenerateTransactionInvoiceNumFromTender(UCHAR *pauchRefNum, ITEMTENDER *pItemTender)
{
	int               i;
	UCHAR             *auchRefNum = TrnInformation.invno.auchInvoiceNo;
	PARATERMINALINFO  TermInfo;
	DATE_TIME DateTime;

	PifGetDateTime(&DateTime);

	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PifSysConfig()->usTerminalPosition;
 
	ParaTerminalInfoParaRead (&TermInfo);

	//Store No. 0-9999
	i = sprintf (auchRefNum, "%4.4d", (TermInfo.TerminalInfo.usStoreNumber % 10000));

	//Regiser No. 0-99
	i += sprintf (auchRefNum + i, "%2.2d", (TermInfo.TerminalInfo.usRegisterNumber % 100));

	// Date time stamp
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usMonth);
	i += sprintf (auchRefNum + i, "%2.2u", DateTime.usMDay);
	i += sprintf (auchRefNum + i, "%2.2u", (pItemTender->uchTenderHour % 100));
	i += sprintf (auchRefNum + i, "%2.2u", (pItemTender->uchTenderMinute % 100));
	i += sprintf (auchRefNum + i, "%2.2u", (pItemTender->uchTenderSecond % 100));

	if (pauchRefNum)
		strncpy (pauchRefNum, TrnInformation.invno.auchInvoiceNo, NUM_INVOICE_NUM);

	return pauchRefNum;
}

VOID TrnGetTransactionCardHolder(TCHAR *tchCardHolder)
{
 	_tcsncpy (tchCardHolder, TrnInformation.tchCardHolder, NUM_CPRSPTEXT);
	tchCardHolder[STD_CPRSPTEXT_LEN] = 0;
}

VOID TrnPutTransactionCardHolder(TCHAR *tchCardHolder)
{
	memset (TrnInformation.tchCardHolder, 0, sizeof(TrnInformation.tchCardHolder));
	if (tchCardHolder) {
		TCHAR tcsCardHolderTemp[STD_CPRSPTEXT_LEN + 5] = {0};
		TCHAR *pFirstOffset;

		_tcsncpy (tcsCardHolderTemp, tchCardHolder, STD_CPRSPTEXT_LEN);
		pFirstOffset = _tcsstr (tcsCardHolderTemp, _T("/"));    // ITM_MSR_NAME_LASTFIRST
		if (pFirstOffset) {
			*pFirstOffset = 0;
			pFirstOffset++;
			_tcscpy (TrnInformation.tchCardHolder, pFirstOffset);
			_tcscat (TrnInformation.tchCardHolder, _T(" "));
			_tcscat (TrnInformation.tchCardHolder, tcsCardHolderTemp);
		} else {
			_tcsncpy (TrnInformation.tchCardHolder, tchCardHolder, STD_CPRSPTEXT_LEN);
		}
	}
}


/*==========================================================================
*    Synopsis:  VOID    TrnQualTransRO( ITEMTRANSOPEN *ItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by reorder, addcheck
==========================================================================*/

VOID    TrnQualTransRO( ITEMTRANSOPEN *ItemTransOpen )
{
    if ( ItemTransOpen->uchLineNo != 0 ) {
        TrnInformation.TranGCFQual.uchSlipLine = ItemTransOpen->uchLineNo;
    }

    /* --- if current system uses unique transaction no., store its no.
        to GCF qualifier --- */
//    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
//        TrnInformation.TranGCFQual.usTableNo = ItemTransOpen->usTableNo;
//    }
}



/*==========================================================================
*    Synopsis:  VOID    TrnQualTransSR( ITEMTRANSOPEN *pItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *pItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by store/recall transaction.    R3.1
*
*                   NOTE: this is Guest Check specific data that is stored in
*                         the TranGCFQual memory area. The TranGCFQual memory
*                         area is stored in the Guest Check File along with the
*                         transaction data.
*
*                         There is some duplication between the TranGCFQual
*                         and TranModeQual memory resident data in order to
*                         save state when the Guest Check is saved.
*
*                         See functions TrnSaveGC() and TrnGetGC().
==========================================================================*/

VOID    TrnQualTransSR( ITEMTRANSOPEN *pItemTransOpen )
{
    /* --- save guest check qualifier to order no. --- */
    TrnInformation.TranGCFQual.usGuestNo = pItemTransOpen->usGuestNo;

    /* --- save no. of person to qualifier --- */
    TrnInformation.TranGCFQual.usNoPerson = pItemTransOpen->usNoPerson;

    /* --- cashier ID to qualifier --- */
    TrnInformation.TranGCFQual.ulCashierID = pItemTransOpen->ulCashierID;

    /* --- if current system uses unique transaction no., store its no.
        to GCF qualifier --- */
//    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
//        TrnInformation.TranGCFQual.usTableNo = pItemTransOpen->usTableNo;
//    }

//    TrnInformation.TranGCFQual.auchCheckOpen[0] = pItemTransOpen->auchCheckOpen[0];
//   TrnInformation.TranGCFQual.auchCheckOpen[1] = pItemTransOpen->auchCheckOpen[1];
//    TrnInformation.TranGCFQual.auchCheckOpen[2] = pItemTransOpen->auchCheckOpen[2];
//    memcpy (TrnInformation.TranGCFQual.auchCheckHashKey, pItemTransOpen->auchCheckHashKey, 6);
}


/*==========================================================================
*    Synopsis:  VOID TrnQualTransCash( ITEMTRANSOPEN *pItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *pItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by cashier transaction.
==========================================================================*/

VOID TrnQualTransCash( ITEMTRANSOPEN *pItemTransOpen )
{
    /* --- if current system uses unique transaction no., store its no.
        to GCF qualifier --- */
//    if ( TrnInformation.TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
//        TrnInformation.TranGCFQual.usTableNo = pItemTransOpen->usTableNo;
//    }

//    TrnInformation.TranGCFQual.auchCheckOpen[0] = pItemTransOpen->auchCheckOpen[0];
//    TrnInformation.TranGCFQual.auchCheckOpen[1] = pItemTransOpen->auchCheckOpen[1];
//    TrnInformation.TranGCFQual.auchCheckOpen[2] = pItemTransOpen->auchCheckOpen[2];
//    memcpy (TrnInformation.TranGCFQual.auchCheckHashKey, pItemTransOpen->auchCheckHashKey, 6);
}

/*
*==========================================================================
*    Synopsis:  VOID    TrnQualLoanPick(ITEMTRANSOPEN *pItem)
*
*    Input:     ITEMTRANSOPEN   *pItem
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   Save Operator ID,                           Saratoga
*==========================================================================
*/
VOID    TrnQualLoanPick(ITEMTRANSOPEN *pItem)
{
    TrnInformation.TranModeQual.ulCashierID = pItem->ulCashierID;
}

/*==========================================================================
*    Synopsis:  VOID    TrnQualTotal( ITEMTRANSOPEN *ItemTransOpen )
*
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by total key                    R3.1
==========================================================================*/

VOID    TrnQualTotal(ITEMTOTAL *ItemTotal)
{
    if (ItemTotal->auchTotalStatus[0] != CLASS_TOTAL1                        /* check if subtotal key */
        && (ITM_TTL_FLAG_TOTAL_TYPE(ItemTotal) != PRT_TRAY1)                 /* check if tray total */
        && (ITM_TTL_FLAG_TOTAL_TYPE(ItemTotal) != PRT_TRAY2)                 /* check if tray total with stub */
        && (ItemTotal->uchMinorClass != CLASS_TOTAL2_FS)                     /* check if Food Stamp Whole Total,  Saratoga */
        && (ItemTotal->uchMinorClass != CLASS_TOTAL9)) {                     /* check if Food Stamp Total,        Saratoga */

        if (ItemTotal->uchMinorClass == CLASS_SPLIT) {
            TrnInformation.TranCurQual.uchSplit = CLASS_SPLIT;
            TrnInformation.TranCurQual.uchSeat = 0;
        } else {
			TranCurQualTotalStatusPut (ItemTotal->auchTotalStatus);
        }
    }
}

/*==========================================================================
*    Synopsis:  VOID    TrnQualModSeat(ITEMMODIFIER *pItemModifier)
*
*    Input:     ITEMMODIFIER    *pItemModifier
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by seat#.                       R3.1
==========================================================================*/

VOID    TrnQualModSeat(ITEMMODIFIER *pItemModifier)
{
    TrnInformation.TranCurQual.uchSeat = pItemModifier->uchLineNo;
}

ULONG TrnQualModifierReturnData (USHORT *pusReturnType, USHORT *pusReasonCode)
{
	if (pusReturnType)
		*pusReturnType = (TrnInformation.TranModeQual.usReturnType & 0x00ff);  // return the type, masking off indicators in most significant byte

	if (pusReasonCode)
		*pusReasonCode = TrnInformation.TranModeQual.usReasonCode;

	// Replicate return information in GCFQUAL since that is what is saved in the Guest Check File
	TrnInformation.TranGCFQual.usReturnType = (TrnInformation.TranModeQual.usReturnType & 0x00ff);  // return the type, masking off indicators in most significant byte
	TrnInformation.TranGCFQual.usReasonCode = TrnInformation.TranModeQual.usReasonCode;
	return TrnInformation.TranCurQual.fsCurStatus;
}

ULONG TrnQualModifierReturnTypeTest (VOID)
{
	ULONG  fbModifier = 0;

	if (TrnInformation.TranModeQual.usReturnType & 0x100)    // check to see if RETURNS_MODIFIER_1 has been used in this transaction
		fbModifier |= RETURNS_MODIFIER_1;
	if (TrnInformation.TranModeQual.usReturnType & 0x200)    // check to see if RETURNS_MODIFIER_2 has been used in this transaction
		fbModifier |= RETURNS_MODIFIER_2;
	if (TrnInformation.TranModeQual.usReturnType & 0x400)    // check to see if RETURNS_MODIFIER_3 has been used in this transaction
		fbModifier |= RETURNS_MODIFIER_3;

	// Replicate return information in GCFQUAL since that is what is saved in the Guest Check File
	TrnInformation.TranGCFQual.usReturnType = (TrnInformation.TranModeQual.usReturnType & 0x00ff);  // return the type, masking off indicators in most significant byte
	TrnInformation.TranGCFQual.usReasonCode = TrnInformation.TranModeQual.usReasonCode;
	return fbModifier;
}

ULONG TrnQualModifierReturnTypeSet (ULONG fbModifier)
{
	ULONG  ulReturnType = TrnQualModifierReturnTypeTest ();

	if (fbModifier & RETURNS_MODIFIER_1)
		TrnInformation.TranModeQual.usReturnType |= 0x100;    // indicate that RETURNS_MODIFIER_1 has been used in this transaction
	if (fbModifier & RETURNS_MODIFIER_2)
		TrnInformation.TranModeQual.usReturnType |= 0x200;    // indicate that RETURNS_MODIFIER_2 has been used in this transaction
	if (fbModifier & RETURNS_MODIFIER_3)
		TrnInformation.TranModeQual.usReturnType |= 0x400;    // indicate that RETURNS_MODIFIER_3 has been used in this transaction

	// Replicate return information in GCFQUAL since that is what is saved in the Guest Check File
	TrnInformation.TranGCFQual.usReturnType = (TrnInformation.TranModeQual.usReturnType & 0x00ff);  // return the type, masking off indicators in most significant byte
	TrnInformation.TranGCFQual.usReasonCode = TrnInformation.TranModeQual.usReasonCode;
	return ulReturnType;
}


/*==========================================================================
*    Synopsis:  VOID    TrnQualModifier(ITEMMODIFIER *ItemModifier)
*
*    Input:     ITEMMODIFIER    *ItemModifier
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by modifier                     R3.1
*                   based on the minor class we will modify the memory resident
*                   transaction qualifier data for the current transaction to
*                   indicate to BusinessLogic what the operator has modified
*                   about the transaction such as number of persons, whether the
*                   transaction is a Transaction Void, etc.
==========================================================================*/

VOID    TrnQualModifier(ITEMMODIFIER *pItemModifier)
{                                                               
    switch( pItemModifier->uchMinorClass ) {
    case CLASS_PRETURN :
		if ((TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_TRETURN | CURQUAL_PRETURN)) != 0) {
			TrnInformation.TranModeQual.usReturnType = 0;
			TrnInformation.TranModeQual.usReasonCode = 0;
		} else {
			TrnInformation.TranModeQual.usReturnType &= 0xff00;
			TrnInformation.TranModeQual.usReturnType |= (pItemModifier->usTableNo & 0x00ff);   // set the default return type
			TrnInformation.TranModeQual.usReasonCode = pItemModifier->usNoPerson;              // set the default return code
		}
		TrnInformation.TranCurQual.fsCurStatus ^= (CURQUAL_TRETURN | CURQUAL_PRETURN);   /* CLASS_PRETURN, preselect return */
        break;

    case CLASS_PVOID :
		TrnInformation.TranCurQual.fsCurStatus ^= CURQUAL_PVOID;          /* CLASS_PVOID, preselect void */
        break;

    case CLASS_NORECEIPT :                              /* receipt/no receipt */
        ;                                               // nothing to set for this condition
        break;

    case CLASS_TABLENO :                                /* table No */
		if ( pItemModifier->usTableNo != 0) {
			if ( TrnInformation.TranGCFQual.usTableNo != pItemModifier->usTableNo ) {
				TrnInformation.TranCurQual.fsCurStatus |= CURQUAL_TABLECHANGE;
			}
			TrnInformation.TranGCFQual.usTableNo = pItemModifier->usTableNo;
		}
        break;

    case CLASS_NOPERSON :                               /* No of person */
		if ( pItemModifier->usNoPerson != 0 ) {
			if ( TrnInformation.TranGCFQual.usNoPerson != pItemModifier->usNoPerson ) {
				TrnInformation.TranCurQual.fsCurStatus |= CURQUAL_PERSONCHANGE;
			}
			TrnInformation.TranGCFQual.usNoPerson = pItemModifier->usNoPerson;
		}

		if ( !( TrnInformation.TranCurQual.fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN) ) ) {
			if ( TrnInformation.TranGCFQual.usNoPerson >= TrnInformation.TranGCFQual.usNoPersonMax ) {
				TrnInformation.TranGCFQual.usNoPersonMax = TrnInformation.TranGCFQual.usNoPerson;
			} 
		}
        break;

    case CLASS_LINENO :                                 /* line No */
		if ( pItemModifier->uchLineNo != 0 ) {
			TrnInformation.TranGCFQual.uchSlipLine = pItemModifier->uchLineNo;
		}
        break;

    case CLASS_ALPHATENT:
    case CLASS_ALPHANAME:
		if ( pItemModifier->aszName != '\0' ) {
			_tcsncpy( TrnInformation.TranGCFQual.aszName, pItemModifier->aszName, NUM_NAME );
		}
        break;

    case CLASS_SEATNO:
        TrnQualModSeat(pItemModifier);                   /* Seat# Transaction,  R3.1 */
        break;

    case CLASS_SEAT_SINGLE:
        TrnInformation.fsTransStatus |= TRN_STATUS_SEAT_TRANS;
        TrnQualModSeat(pItemModifier);
        break;

    case CLASS_SEAT_MULTI:
        TrnInformation.fsTransStatus |= TRN_STATUS_MULTI_TRANS;
        TrnQualModSeat(pItemModifier);
        break;

    case CLASS_SEAT_TYPE2:
        TrnInformation.fsTransStatus |= TRN_STATUS_TYPE2_TRANS;
        TrnQualModSeat(pItemModifier);
        break;

    case CLASS_WICTRN:                                  /* Saratoga */
        TrnInformation.TranCurQual.fsCurStatus ^= CURQUAL_WIC;
        break;

    default:
        break;
    }
}



/*==========================================================================
*   Synopsis:   VOID    TrnQualItems(VOID *pItem)
*
*   Input:     
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    Update Seat# Queue.                        R3.1
*                   For those guest check items that are on a seat basis, we need
*                   to update the seat number queue with changes if this transaction
*                   is using seats.
==========================================================================*/

VOID    TrnQualItems(VOID *pItem)
{
    ITEMGENERICHEADER   *pGeneric = pItem;

    switch (pGeneric->uchMajorClass) {
    case CLASS_ITEMSALES:
        if (pGeneric->uchSeatNo != 0) {
            TrnInformation.TranGCFQual.fsGCFStatus2 |= GCFQUAL_USESEAT;
            TrnSplInsertUseSeatQueue(pGeneric->uchSeatNo);
        }
        break;

    case CLASS_ITEMDISC:
        if (pGeneric->uchSeatNo >= 1 && pGeneric->uchSeatNo <= NUM_SEAT) {   //SR206
            TrnInformation.TranGCFQual.fsGCFStatus2 |= GCFQUAL_USESEAT;
            TrnSplInsertUseSeatQueue(pGeneric->uchSeatNo);
        }
        break;

    case CLASS_ITEMCOUPON:
        if (pGeneric->uchSeatNo != 0) {
            TrnInformation.TranGCFQual.fsGCFStatus2 |= GCFQUAL_USESEAT;
            TrnSplInsertUseSeatQueue(pGeneric->uchSeatNo);
        }
        break;

    default:
        break;
    }
}

/*
*==========================================================================
*   Synopsis:   VOID    TrnQualOther(ITEMOTHER *pItem)
*
*   Input:     
*   Output:     none
*   InOut:      none
*
*   Return:     none
*
*   Description:    Save Account# for EPT ASK(Pre-Inquirly)     Saratoga
*==========================================================================
*/
VOID    TrnQualOther(ITEMOTHER *pItem)
{
	UCHAR uchSeat = TrnInformation.TranCurQual.uchSeat;

	// The account number information is stored as encrypted data.
	// Since we are transforming from TCHAR to UCHAR the account number, we
	// must decrypt the string, transform, then encrypt the string again.
	// If we were to transform without the decrypt/encrypt, the encrypted
	// string would become corrupted because the transformation removes
	// most significant byte of the two byte TCHAR.
	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
    _RflMemcpyTchar2Uchar(TrnInformation.TranGCFQual.aszNumber[uchSeat], pItem->aszNumber, NUM_NUMBER);
	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
	RflEncryptByteString (TrnInformation.TranGCFQual.aszNumber[uchSeat], sizeof(TrnInformation.TranGCFQual.aszNumber[uchSeat]));

	RflDecryptByteString ((UCHAR *)&(pItem->auchMSRData[0]), sizeof(pItem->auchMSRData));
    _RflMemcpyTchar2Uchar(TrnInformation.TranGCFQual.auchMSRData[uchSeat], pItem->auchMSRData, NUM_MSRDATA);
	RflEncryptByteString ((UCHAR *)&(pItem->auchMSRData[0]), sizeof(pItem->auchMSRData));
	RflEncryptByteString (TrnInformation.TranGCFQual.auchMSRData[uchSeat], sizeof(TrnInformation.TranGCFQual.auchMSRData[uchSeat]));
}



/*==========================================================================
*    Synopsis:  VOID    TrnQualClsCashier( VOID )
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   clear existing data by cashier close.
*                   We have to be careful to clear only temporary data.
*                   Other data such as file handles need to remain.
==========================================================================*/

VOID    TrnQualClsCashier()
{
	TrnInformation.fsTransStatus = 0;
    TrnInitialize( TRANI_MODEQUAL | TRANI_CURQUAL | TRANI_GCFQUAL | TRANI_ITEMIZERS);

    TrnInformation.usTranItemStoVli = 0;
    TrnInformation.usTranConsStoVli = 0;

    TrnSplitA.usTranConsStoVli = 0;
    TrnSplitB.usTranConsStoVli = 0;
}                                                                           



/*==========================================================================
*    Synopsis:  VOID    TrnQualClsCasInt( VOID )
*
*    Input:     none
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by cashier interrupt close      R3.3
==========================================================================*/

static VOID    TrnQualClsCasInt()
{
    /* initialize except mode qualifier */
    TrnInitialize( TRANI_CURQUAL | TRANI_GCFQUAL | TRANI_ITEMIZERS);

    TrnInformation.usTranItemStoVli = 0;
    TrnInformation.usTranConsStoVli = 0;

    TrnSplitA.usTranConsStoVli = 0;
    TrnSplitB.usTranConsStoVli = 0;

}                                                                           



/*==========================================================================
*    Synopsis:  VOID    TrnQualClsWaiter( ITEMOPECLOSE *ItemOpeClose )    
*
*    Input:     ITEMOPECLOSE    *ItemOpeClose
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by waiter, V3.3
==========================================================================*/

static VOID    TrnQualClsWaiter()
{                                                                                                                                                   
    if ( TrnInformation.TranModeQual.ulCashierID == 0 ) {                  /* cashier not open */
        TrnQualClsCashier();
    } else {
		// We need to have a waiter/bartender/surrogate specific area of data because this is
		// used for bartender or surrogate Sign-in which is done with an active Cashier Sign-in state.
        TrnInformation.TranModeQual.ulWaiterID = 0;                        /* waiter ID */
        TrnInformation.TranModeQual.fbWaiterStatus = 0;                    /* waiter status */
		TrnInformation.TranModeQual.fbCashierStatus &= ~ MODEQUAL_WAITER_LOCK;   /* clear the waiter lock bit in case it was set R3.1 */
        memset( &TrnInformation.TranModeQual.aszWaiterMnem[0], 0, sizeof(TrnInformation.TranModeQual.aszWaiterMnem));                                                /* waiter mnemonic */
        memset( &TrnInformation.TranModeQual.auchWaiStatus[0], 0, sizeof(TrnInformation.TranModeQual.auchWaiStatus));
        TrnInformation.TranModeQual.uchWaiChgTipRate = 0;                  /* charge tips rate */
		TrnInformation.TranModeQual.ulWaiGroupAssociations = 0;
		TrnInformation.TranModeQual.usWaiCtrlStrKickoff = 0;               /* startup control string to run with Sign-in */
		TrnInformation.TranModeQual.usWaiStartupWindow = 0;                /* startup window to display with Sign-in */

        TrnInformation.TranModeQual.fsModeStatus &= ~ MODEQUAL_BARTENDER;   /* clear the bartender surrogate bit in case it was set R3.1 */

        TrnInformation.TranCurQual.fsCurStatus &= ~(CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN);          /* reset p-void and transaction return status */
    }
}


/*==========================================================================
*    Synopsis:  VOID    TrnQualCls( ITEMMODIFIER *ItemModifier )    
*
*    Input:     ITEMMODIFIER    *ItemModifier
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by operator close               R3.1
==========================================================================*/

VOID    TrnQualCls( ITEMOPECLOSE *ItemOpeClose )
{
    switch( ItemOpeClose->uchMinorClass ) {

    case CLASS_CASHIEROUT :                                     /* cashier sign-out */
    case CLASS_B_OUT :                                          /* cashier B sign-out */
        TrnQualClsCashier();                           
        break;

    /* cashier interrupt close, R3.3 */
    case CLASS_CASINTOUT :                                      /* cashier sign-out */
    case CLASS_CASINTB_OUT :                                    /* cashier B sign-out */
        TrnQualClsCasInt();                            
        break;

    case CLASS_WAITEROUT :                                      /* waiter sign-out */
    case CLASS_WAITERLOCKOUT :                                  /* waiter lock sign-out */
    case CLASS_MODELOCK :                                       /* mode lock-out */
    case CLASS_BARTENDEROUT:                                    /* bartender sign-out, R3.1 */
    case CLASS_CASHIERMISCOUT:                                  /* sign-out by misc. operation, V3.3 */
        TrnQualClsWaiter();
        break;

    default:
        break;

    }
}

ITEMDISC *TrnGetCurChargeTip (ITEMDISC **TranChargeTips)
{
	if (TranChargeTips && *TranChargeTips) {
		*TranChargeTips =  &TrnInformation.TranChargeTips;
	}
	return &TrnInformation.TranChargeTips;
}

ITEMDISC *TrnClearCurChargeTip (ITEMDISC **TranChargeTips)
{
	if (TranChargeTips && *TranChargeTips) {
		*TranChargeTips =  &TrnInformation.TranChargeTips;
	}
	memset (&TrnInformation.TranChargeTips, 0, sizeof(TrnInformation.TranChargeTips));
	return &TrnInformation.TranChargeTips;
}

/* ===== End of File ( TRNQUAL.C ) ===== */