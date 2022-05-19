/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

-----------------------------------------------------------------------*\
:   Title          : TENDER MODULE
:   Category       : TENDER MODULE, US Hospitality Application
:   Program Name   : ITTEND5.C (charge posting function)
:  ---------------------------------------------------------------------
:  Abstract:
:   ItemTendCP()                    ; charge posting tender main routine
:       ItemTendRC()                ; room charge tender main routine
:       ItemTendAC()                ; account charge/credit autho. tender main routine
:       ItemTendEPT()               ; EPT tender main routine
:       ItemTendCPEntry1()          ; entry room #/guest id
:       ItemTendCPEntry2()          ; entry guest line #
:       ItemTendCPEntry3()          ; entry account number
:       ItemTendCPEdit()            ; edit charge posting request data
:       ItemTendCPCanDisp()         ; display total amount after canceled charge posting tender
:       ItemTendCPConvData()        ; convert 2170 data to CP format data
:       ItemTendCPConvErr()         ; convert CPM error to 2170 error
:       ItemTendCPPrintLN()         ; print charge posting guest line #
:       ItemTendCPPrintErr()        ; print charge posting and EPT error code to journal
:       ItemTendCPCommDisp()        ; display communication mnemonics
:       ItemTendCPTrain()           ; trainning mode for room/acct. charge
:       ItemTendEPTTrain()          ; trainning mode for EPT
:       ItemTendCPECTend()          ; E/C handling for charge posting
:       ItemTendEPTComm()           : Perform EPT communications with card vendor
:       ItemTendCPTendStub()        ; print off line tender stub
:       ItemExecuteOffTend()        ; check MDC & execute offline tender
:       ItemTendEditRspMsg()        ; edit response msg. from CP
:
:  ---------------------------------------------------------------------
:  Update Histories
:   Date   : Ver.Rev.  : Name      : Description
: Nov-19-93: 00.00.01  : K.You     : initial
: Mar-07-95: 02.05.03  : M.Suzuki  : change return status to ITM_PARTTEND
:          :           :           : for partial tender.(mod. ItemTendCPComm)
: Aug-31-95: 03.00.06  : M.Suzuki  : bug fixed E100. (mod. ItemTendCPCanDisp)
: Nov-14-95: 03.01.00  : hkato     : R3.1
: Aug-25-98: 03.03.00  : hrkato    : V3.3 (Split System & Charge Posting)
: Aug-11-99: 03.05.00  : K.Iwata   : R3.5 (remove WAITER_MODEL)
: Dec-06-99: 01.00.00  : hrkato    : Saratoga
: Jul-28-17: 02.02.02  : R.Chambers : removed unneeded include files, localized variables
-------------------------------------------------------------------------*/
#include	<windows.h>
#include	<tchar.h>
#include    <string.h>
#include    <stdlib.h>
#include	<stdio.h>
#include	<malloc.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "fsc.h"
#include    "paraequ.h"
#include    "para.h"
#include    "display.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "appllog.h"
#include    "item.h"
#include    "csstbpar.h"
#include    "rfl.h"
#include    "eept.h"
#include    "eeptl.h"
#include    "prevept.h"
#include    "csstbstb.h"
#include    "csstbfcc.h"
#include    "BlfWIf.h"
#include    "cpm.h"
#include    "csstbcpm.h"
#include    "csstbept.h"
#include    "csstbgcf.h"

#include    "itmlocal.h"
#include	"EEptTrans.h"
#include	"pifmain.h"
#include    "mld.h"
#include    "maint.h"
#include    "trans.H"
#include    "bl.H"
#include    "ConnEngineObjectIf.h"

static SHORT   hsItemCPHandle1 = -1;                    /* Temporary File Handle#1 */
static SHORT   hsItemCPHandle2 = -1;                    /* Temporary File Handle#2 */

static TCHAR   aszItemCPTmpFile1[] = _T("REGTMP1");    /* Tmp#1 File Name */
static TCHAR   aszItemCPTmpFile2[] = _T("REGTMP2");    /* Tmp#2 File Name */

// Declarations for functions defined below.
static VOID    ItemTendCPSaveRsp(UCHAR uchFCode, EEPTRSPDATA *CPRcv, SHORT sType);

/*
*===========================================================================
** Synopsis:    USHORT     CpmConvertError(SHORT sError);
*     Input:    sError
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   CpmConvertError(SHORT sError)
{
	USHORT  usLeadthruNo;

	switch (sError) {
	case CPM_RET_SUCCESS:                       /* Success              */
		usLeadthruNo = 0;
		break;

	case CPM_RET_LENGTH:                        /* EEPT_RET_LENGTH receive length error */
		usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
		break;

	case  CPM_RET_OFFLINE:                      /* EEPT_RET_OFFLINE CPM & PMS is offline */
		usLeadthruNo = LDT_CPM_OFFLINE_ADR;     /* 81 * CPM offline     */
		break;

	case  CPM_RET_TIMEOUT:                      /* EEPT_RET_TIMEOUT time out error       */
		usLeadthruNo = LDT_REQST_TIMED_OUT;     /* 81 * CPM offline     */
		break;

	case  CPM_RET_FAIL:                         /* EEPT_RET_FAIL other communication  */
		usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
		break;

	case  CPM_RET_STOP:                         /* EEPT_RET_STOP CPM is stop process  */
		usLeadthruNo = LDT_CPM_STOP_FUNC_ADR;   /*  80 * CPM Stop       */
		break;

	case  CPM_RET_BUSY:                         /* EEPT_RET_BUSY CPM is busy          */
		usLeadthruNo = LDT_CPM_BUSY_ADR;        /* 79 * CPM busy        */
		break;

	case  CPM_RET_PORT_NONE:                    /* EEPT_RET_PORT_NONE PMS is not provide   */
		usLeadthruNo = LDT_EQUIPPROVERROR_ADR;  /* Equipment not provisioned properly */
		break;

	case  CPM_RET_NOT_MASTER:                   /* EEPT_RET_NOT_MASTER I'm not master (O/S) */
		usLeadthruNo = LDT_PROHBT_ADR;          /* 10 * Prohibit Operation */
		break;

	case  CPM_RET_BADPROV:                      // EEPT_RET_BADPROV
		usLeadthruNo = LDT_EQUIPPROVERROR_ADR;  /* Equipment not provisioned properly */
		break;

	case  CPM_RET_CANCELLED:                    // EEPT_RET_CANCELLED
		usLeadthruNo = LDT_CANCEL_ADR;
		break;

	case  CPM_RET_SERVERCOMMS:                  // EEPT_RET_SERVERCOMMS
		usLeadthruNo = LDT_EQUIPPROVERROR_ADR;
		break;

	case  CPM_RET_INVALIDDATA:                  // EEPT_RET_INVALIDDATA
		usLeadthruNo = LDT_WRONG_ACNO_ADR;
		break;

	case  CPM_DUR_INQUIRY:
	case  CPM_M_DOWN_ERROR:
		usLeadthruNo = LDT_LNKDWN_ADR;
		break;

	case  CPM_BM_DOWN_ERROR:
		usLeadthruNo = LDT_SYSBSY_ADR;
		break;

	case  CPM_TIME_OUT_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  CPM_UNMATCH_TRNO:
		usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
		break;

	case  CPM_BUSY_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  CPM_RET_SERVICE_ERROR:                  // EEPT_RET_SERVICE_ERROR an error such as encryption error in DSI Client
		usLeadthruNo = LDT_ALTERNATE_TENDER_RQD;
		break;

	default:
		//SR368
		// sError is a negative value as are EEPTDLL_BASE_ERROR (lowest in range) and EEPTDLL_MAX_ERROR (max of range)
		// See the error code table of EptErrorCodeTable[] which contains the sError codes beginning with -100 
		if (sError <= EEPTDLL_BASE_ERROR && sError >= EEPTDLL_MAX_ERROR)
			usLeadthruNo = EEPTDLL_LDTOFFSET + (sError * -1);
		else
			usLeadthruNo = LDT_CPM_UNKNOWNRESPONSE; /*  Unknown error  */
		break;
	}

	if (sError != STUB_SUCCESS) {
		if (sError <= EEPTDLL_BASE_ALT_TENDER && sError >= EEPTDLL_MAX_ALT_TENDER) {
			// if in the range for 
			usLeadthruNo = LDT_ALTERNATE_TENDER_RQD;
		}
		PifLog(MODULE_STB_LOG, LOG_EVENT_STB_CONVERTERROR_CPM);
		PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(sError * (-1)));
		PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), usLeadthruNo);
	}

	return (usLeadthruNo);
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendCPPrintAccRej(VOID *CPRcvData, SHORT sType)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Print Response Text in Accept/Reject.
*==========================================================================
*/
VOID    ItemTendCPPrintAccRej(EEPTRSPDATA  *CPRcv, SHORT sType)
{
    UCHAR           aszCPMErrorDef[3];
	UCHAR			uchPrintLine, uchPrintSelect;
    SHORT           sPrintLineMax;
    SHORT           sError;
	ITEMPRINT       Print = {0};

    if (CPRcv->auchMsgRecpt == '0') {
        return;
    }

    /* --- Print Error Message in Text --- */
    Print.uchMajorClass = CLASS_ITEMPRINT;
    Print.uchMinorClass = CLASS_RSPTEXT;

    Print.fsPrintStatus = PRT_JOURNAL;
    if (CPRcv->auchMsgRecpt == '1' || CPRcv->auchMsgRecpt == '2') {
        Print.fsPrintStatus |= PRT_RECEIPT;
    }
    if (CPRcv->auchMsgSlip == '1') {
        Print.fsPrintStatus |= PRT_SLIP;
    }
    if (CPRcv->auchMsgRecpt == '2') {
        ItemTenderLocal.fbTenderStatus[1] |= TENDER_CP_DUP_REC;
    }
    if (CPRcv->auchPrintSelect == '0') {
        uchPrintSelect = NUM_REC_1LINE;
        sPrintLineMax  = NUM_MAX_LINE + 6;
     } else {
        uchPrintSelect = NUM_SLIP_1LINE;
        sPrintLineMax  = NUM_MAX_LINE;
    }
    aszCPMErrorDef[0] = CPRcv->auchPrintLineNo[0];
    aszCPMErrorDef[1] = CPRcv->auchPrintLineNo[1];
    aszCPMErrorDef[2] = '\0';
    sError = atoi(aszCPMErrorDef);

    /* --- Check No. of Print Line --- */
    if (sError > sPrintLineMax) {
        uchPrintLine = (UCHAR)sPrintLineMax;
    } else {
        uchPrintLine = (UCHAR)sError;
    }

    Print.uchCPLineNo   = (UCHAR)sPrintLineMax;
    Print.uchPrintSelect= uchPrintSelect;

    _tcsncpy(&Print.aszCPText[0][0], &CPRcv->auchMsgText[0], NUM_CPRSPTEXT);

	/* execute slip validation for partial ept tender, 09/12/01 */
    if ((Print.fsPrintStatus & PRT_SLIP) &&
   		(Print.uchCPLineNo != 0)) {                /* Saratoga */
        Print.fsPrintStatus |= PRT_SLIP;
        TrnThrough(&Print);
	    Print.fsPrintStatus = PRT_JOURNAL;
	    if (CPRcv->auchMsgRecpt == '1' || CPRcv->auchMsgRecpt == '2') {
    	    Print.fsPrintStatus |= PRT_RECEIPT;
	    }
	}
    if (sType == 0) {
        TrnItem(&Print);
    } else {
        TrnThrough(&Print);
    }
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendCPRejectAskDisp(USHORT usLead)
*
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Display Error Message from Response Text for Ask
*==========================================================================
*/
static VOID    ItemTendCPRejectAskDisp(USHORT usLead)
{
	REGDISPMSG      Disp = {0};

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CLEAR;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    RflGetLead (Disp.aszMnemonic, usLead);

    for (;;) {
        PifBeep(UIE_ERR_TONE);
        flDispRegDescrControl |= COMPLSRY_CNTRL;
        flDispRegKeepControl |= COMPLSRY_CNTRL;
        DispWrite(&Disp);
        flDispRegDescrControl &= ~COMPLSRY_CNTRL;
        flDispRegKeepControl &= ~COMPLSRY_CNTRL;

        if (UifDiaClear() == UIF_SUCCESS) {
            ItemOtherClear();
            break;
        }
    }
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCPRejectAsk(EEPTRSPDATA *CPRcvData)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Display Error Message from Response Text for ASK
*========================================================================== 
*/
SHORT   ItemTendCPRejectAsk(EEPTRSPDATA *CPRcv, ITEMTENDER *pItem)
{
    switch (CPRcv->auchMsgInstruction) {
    case    EEPT_RES_INST_PRINT:
        if (!(ItemSalesLocalPtr->fbSalesStatus & SALES_ITEMIZE)) {
            ItemCommonOpenAsk(CPRcv);
        }
        ItemTendCPPrintAccRej(CPRcv, 0);
        ItemTendCPRejectAskDisp(LDT_EPT_REJECT_ADR);
        return(ITM_REJECT_ASK);

    case    EEPT_RES_INST_DISPMSG:
        return(ItemTendCPConvErr(CPRcv->auchRspMsgNo));

    case    EEPT_RES_INST_DISPTEXT:
        ItemTendCPDispRspText(CPRcv, NULL);
        return(UIF_CANCEL);

    default:
        return(LDT_EPT_REJECT_ADR);
    }
}


/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCPReject(UCHAR uchFCode, VOID *CPRcvData)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Display Error Message from Response Text
*==========================================================================
*/
SHORT   ItemTendCPReject(UCHAR uchFCode, EEPTRSPDATA *pCPRcv, ITEMTENDER *pItem)
{
    if (uchFCode == EEPT_FC2_ASK) {
        return(ItemTendCPRejectAsk(pCPRcv, pItem));
	}
	
	/* --- Print Reject Message in Text --- */
    if (pCPRcv->auchMsgInstruction == EEPT_RES_INST_PRINT) {
        ItemTendCPPrintAccRej(pCPRcv, 0);
    }

    /* --- Display Error Message with Response Data --- */
    switch (pCPRcv->auchMsgInstruction) {
    case    EEPT_RES_INST_DISPMSG:
        return(ItemTendCPConvErr(pCPRcv->auchRspMsgNo));

    case    EEPT_RES_INST_PRINT:
    case    EEPT_RES_INST_DISPTEXT:
        ItemTendCPDispRspText(pCPRcv, NULL);
		if (pCPRcv->auchEptStatus == EEPT_FC2_REJECTED) {
			return(ITM_REJECT_DECLINED);
		}
		else {
			return(UIF_CANCEL);
		}

    default:
        return(LDT_EPT_REJECT_ADR);
    }
}



/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendEPTPartial(UCHAR uchFCode, ITEMTENDER *pItem,
*                               EEPTRSPDATA *CPRcvData, UIFREGTENDER *pUifTender)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Partial Amount Response from PC
*==========================================================================
*/
static SHORT   ItemTendEPTPartial(UCHAR uchFCode,
                          ITEMTENDER *pItem, EEPTRSPDATA *CPRcv, UIFREGTENDER *pUifTender)
{
    SHORT           sStatus = ITM_PARTTEND;
    DCURRENCY       lAmount;

	pItem->uchMajorClass = CLASS_ITEMTENDER;
    pItem->uchMinorClass = pUifTender->uchMinorClass;

    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_DISPTEXT) {
		// The auchMsgText member of the EEPTRSPDATA is a TCHAR to support multi-lingual
        _tcsncpy(pItem->aszTendMnem, CPRcv->auchMsgText, PARA_LEADTHRU_LEN);
    }

    /* --- Calculate Tender Amount, Balance Due, Change --- */
    if (RflLLabs(pItem->lRound) >= RflLLabs(pItem->lTenderAmount)) {
        pItem->lBalanceDue = pItem->lRound - pItem->lTenderAmount;

        /* Netherlands rounding,    Saratoga */
        if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            pUifTender->uchMajorClass != CLASS_UIFREGPAMTTENDER &&
            pUifTender->uchMinorClass == CLASS_UITENDER1) {
            if (RflRound(&lAmount, pItem->lBalanceDue, RND_TOTAL1_ADR) == RFL_SUCCESS) {
                pItem->lBalanceDue = lAmount;
            }
        }
        pItem->lChange = 0L;
        sStatus = ITM_PARTTEND;

    } else {
        pItem->lChange = pItem->lTenderAmount - pItem->lRound;

        /* Netherlands rounding,    Saratoga */
        if (CliParaMDCCheck(MDC_ROUND1_ADR, ODD_MDC_BIT3) &&
            pUifTender->uchMajorClass != CLASS_UIFREGPAMTTENDER &&
            pUifTender->uchMinorClass == CLASS_UITENDER1) {
            if ( RflRound( &lAmount, pItem->lChange, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                pItem->lChange = lAmount;
            }
        }
        pItem->lBalanceDue = 0L;
        sStatus = ITM_OVERTEND;
    }

    /* --- Set Folio#, Post# from Response Text --- */
     _RflStrncpyUchar2Tchar(pItem->aszFolioNumber, CPRcv->auchFolioNumber, sizeof(CPRcv->auchFolioNumber));
     _RflStrncpyUchar2Tchar(pItem->aszPostTransNo, CPRcv->auchPostTransNo, sizeof(CPRcv->auchPostTransNo));

    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_DISPTEXT) {
		// The auchMsgText member of the EEPTRSPDATA is a TCHAR to support multi-lingual
        _tcsncpy(pItem->aszTendMnem, CPRcv->auchMsgText, PARA_LEADTHRU_LEN);
    }

    /* --- Save Response Text into Tmp#2 File --- */
    ItemTendCPSaveRsp(uchFCode, CPRcv, ITM_FILE_APPEND);

    /* --- Check Buffer is Full Used --- */
    if (ItemTenderLocal.uchBuffOff == 3) {
        pItem->uchBuffOff = 0;
    } else {
        ItemTenderLocal.uchBuffOff++;
        pItem->uchBuffOff = ItemTenderLocal.uchBuffOff;
    }

    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_PRINT) {
        pItem->uchTenderOff = 1;
    }

    return(sStatus);
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendCPSaveRsp(UCHAR uchFCode, VOID *CPRcvData, SHORT sType)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*
*   Description:    Save Response Text into Tmp File#2
*==========================================================================
*/
static VOID    ItemTendCPSaveRsp(UCHAR uchFCode, EEPTRSPDATA *CPRcv, SHORT sType)
{
    SHORT               sError;
    SHORT               sPrintLineMax;
    USHORT              usEventRcv;
	TRN_CPRSPHEADER     Header = {0};
    UCHAR               aszCPMErrorDef[3];
    UCHAR               uchOffset;

    if (CPRcv->auchMsgRecpt == '0') {
        return;
    }

    Header.usPrintStatus |= PRT_JOURNAL;
    if (CPRcv->auchMsgRecpt == '1' || CPRcv->auchMsgRecpt == '2') {
        Header.usPrintStatus |= PRT_RECEIPT;
    }
    if (CPRcv->auchMsgRecpt == '2') {
        ItemTenderLocal.fbTenderStatus[1] |= TENDER_CP_DUP_REC;
    }
    if (CPRcv->auchMsgSlip == '1') {
        Header.usPrintStatus |= PRT_SLIP;
    }
    if (CPRcv->auchPrintSelect == '0') {
        Header.uchPrintSelect = NUM_REC_1LINE;
        sPrintLineMax         = NUM_MAX_LINE + 6;
    } else {
        Header.uchPrintSelect = NUM_SLIP_1LINE;
        sPrintLineMax         = NUM_MAX_LINE;
    }

    aszCPMErrorDef[0] = CPRcv->auchPrintLineNo[0];
    aszCPMErrorDef[1] = CPRcv->auchPrintLineNo[1];
    aszCPMErrorDef[2] = '\0';
    sError = atoi(aszCPMErrorDef);

    /* --- Check No. of Print Line --- */
    if (sError > sPrintLineMax) {
        Header.uchPrintLine = (UCHAR)sPrintLineMax;
    } else {
        Header.uchPrintLine = (UCHAR)sError;
    }
    /* --- Not Print on R/J/S --- */
    if (CPRcv->auchMsgInstruction != EEPT_RES_INST_PRINT) {
        Header.uchPrintLine = 0;
    }

    {
		TRN_CPHEADER   FileHeader = {0};

        /* --- Get Write Position --- */
		if (sType != ITM_FILE_OVERRIDE) {
			// override will go to the beginning of the circular file and be
			// the first item so we don't need the file header. Otherwise we do need it.
			ItemTendCPReadFileHeader (&FileHeader, hsItemCPHandle2);
		}

        uchOffset  = (UCHAR)(FileHeader.uchWriteCnt % RSP_SAVE_MAX);

        /* --- Save Response Data --- */
        /* Save Response Header */
        ItemTendCPWriteResponseHeader (uchOffset, &Header, hsItemCPHandle2);

        /* Save Print Text Data */
        ItemTendCPWriteResponseMessage (uchOffset, CPRcv->auchMsgText, hsItemCPHandle2);

        /* Save File Header     R2.0GCA */
        if (FileHeader.uchWriteCnt < RSP_SAVE_MAX) {
            FileHeader.usVli += sizeof(TRN_CPRSPHEADER) + sizeof(CPRcv->auchMsgText);
        } else {
            switch (uchFCode) {
            case EEPT_FC2_EPT:
                usEventRcv = LOG_EVENT_EPT_RCV;
                break;

            case EEPT_FC2_ASK:
                usEventRcv = LOG_EVENT_CP_ASK_RCV;
                break;

            default:
                usEventRcv = 0;
            }
            ItemTendCPPrintErr(usEventRcv, LDT_FLFUL_ADR);  /* Print error "E-02" to Jounal */
        }
        FileHeader.uchWriteCnt += 1;
        ItemTendCPWriteFileHeader (&FileHeader, hsItemCPHandle2);
    }
}

/*==========================================================================
**  Synopsis:   SHORT ItemTendCP( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
*
*   Input:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     ITM_SUCCESS     : successful
*                               : other error
*
*   Description: charge posting main routine
==========================================================================*/

SHORT ItemTendCP( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
    SHORT   sStatus = ITM_SUCCESS;

	ItemTenderLocal.fbTenderStatus[0] &= ~(TENDER_AC | TENDER_RC | TENDER_EPT);     /* reset other tender status   */

    /* foreign tender ? Q-001 corr '94 5/17 */
    if ( (UifRegTender->uchMinorClass >= CLASS_UIFOREIGN1) && (UifRegTender->uchMinorClass <= CLASS_UIFOREIGN8) ) {
        if (ItemModLocal.fbModifierStatus & MOD_OFFLINE) {
            ItemModLocal.fbModifierStatus &=~ MOD_OFFLINE;
            return(LDT_SEQERR_ADR);
        }
        return(sStatus);
    }

	if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_A)) {  // perform EPT tender
		/* EPT case, Electronic Payment using a vendor network for VISA, etc. to a bank using DataCap control or external device */
		ItemTender->fbModifier |= EPTTEND_SET_MODIF;
		sStatus = ItemTendEPT(UifRegTender, ItemTender);
	} else {
		// in house charge posting type of a system and not an external third party vendor payment network
#if 1
		{
			SHORT           sGusRetValue = 0;

			if ((TranCurQualPtr->fsCurStatus & CURQUAL_PRETURN) == 0 && (TranCurQualPtr->fsCurStatus & CURQUAL_TRETURN) && ItemTender->lRound < 0) {
				ITEMTENDER  xItemTend;

				// this is a return or a void transaction and the transaction total is less than zero
				// so we need to check the rules to determine if this tender is allowed.
				ItemTender->uchMajorClass = CLASS_ITEMTENDER;                   /* major class */
				ItemTender->uchMinorClass = UifRegTender->uchMinorClass;        /* minor class */
				xItemTend = *(ItemTender);
				if ((UifRegTender->uchMinorClass >= CLASS_UIFOREIGN1) && (UifRegTender->uchMinorClass <= CLASS_UIFOREIGN8)) {
					xItemTend.uchMinorClass = UifRegTender->uchMinorClass - CLASS_UIFOREIGN1 + CLASS_FOREIGN1;        /* minor class */
				}
				else if ((UifRegTender->uchMinorClass >= CLASS_UIFOREIGNTOTAL1) && (UifRegTender->uchMinorClass <= CLASS_UIFOREIGNTOTAL8)) {
					xItemTend.uchMinorClass = UifRegTender->uchMinorClass - CLASS_UIFOREIGNTOTAL1 + CLASS_FOREIGN1;        /* minor class */
				}
				if (0 > (sGusRetValue = CliGusLoadTenderTableLookupAndCheckRules(&xItemTend))) {
					return CliGusLoadTenderTableConvertError(sGusRetValue);
				}
			}
		}
#endif
		// now perform the kind of tender action requested.
		if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_D)) {         // perform room charge tender
																												 /* room charge case */
			sStatus = LDT_ALTERNATE_TENDER_RQD;
		}
		else if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_C)) {  // perform account charge tender
																											   /* account charge case */
			sStatus = LDT_ALTERNATE_TENDER_RQD;
		}
		else if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_B)) {  // perform credit auth tender
																											   /* credit autho. case */
			sStatus = LDT_ALTERNATE_TENDER_RQD;
		}
		else {
			if (ItemModLocal.fbModifierStatus & MOD_OFFLINE) {
				ItemModLocal.fbModifierStatus &= ~MOD_OFFLINE;
				return(LDT_SEQERR_ADR);
			}
		}
	}

    if (ItemModLocal.fbModifierStatus & MOD_OFFLINE) {  /* E-003 corr. 4/18 */
        ItemModLocal.fbModifierStatus &=~ MOD_OFFLINE;
    }

    if (sStatus != ITM_SUCCESS) {
		if(sStatus != LDT_SF_TRANS_LIMIT){
			if(sStatus != LDT_STORED_LIMIT){
				if (sStatus != ITM_PARTTEND) {
					ItemTenderLocal.fbTenderStatus[0] = ItemTenderLocal.fbTenderStatusSave;
				}
			}
		}
    }
    return(sStatus);
}

/*==========================================================================
**  Synopsis:   SHORT ItemTendRC( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_PROHBT_ADR  : prohibit error
*               UIF_CANCEL      : canceled by user
*                               : other error
*
*   Description: room charge tender main routine
==========================================================================*/

SHORT ItemTendRC( UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender )
{
	NHPOS_ASSERT_TEXT(!LDT_ERR_ADR, "** ERROR: ItemTendRC() non-functional. CPM module removed");
	return(LDT_PROHBT_ADR);
}


/*
*==========================================================================
**  Synopsis:   SHORT ItemTendAC(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender,
*                                 ITEMTENDER *ItemTender)
*   Input:      UCHAR        uchFuncCode
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_SEQERR_ADR  : sequence error
*               LDT_ERR_ADR     : Error
*
*   Description: account charge/account autho. tender main routine
*==========================================================================
*/
SHORT   ItemTendAC(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	NHPOS_ASSERT_TEXT(!LDT_ERR_ADR, "** ERROR: ItemTendAC() non-functional. CPM module removed");
	return(LDT_PROHBT_ADR);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendEPT(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*
*   Input:      UIFREGTENDER *pUifTender
*               ITEMTENDER   *pItem
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_PROHBT_ADR  : prohibit error
*               UIF_CANCEL      :
*
*   Description: EPT tender main routine                        Saratoga
*==========================================================================
*/
SHORT   ItemTendEPT(UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
	SHORT  sRetStatus = ItemTendEPTEx(pUifTender, pItem);

	return sRetStatus;
}


/*
*==========================================================================
**  Synopsis:   SHORT ItemTendEPTEx(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
*
*   Input:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_PROHBT_ADR  : prohibit error
*               UIF_CANCEL      :
*
*   Description: EPT tender main routine                R3.1    Saratoga
*==========================================================================
*/
static SHORT ItemTenderPerformRequiredMdc(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	SHORT           sStatus, sAcct = 0, sPinStatus = 0;
	TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
	BOOL		    bGetExpDate = TRUE;
	UCHAR           uchSeat;

	if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_B)){   // compulsory account number entry
		sAcct = 1;
	}

	/* --- Check Pre-Inquiry of Account# --- */

	/* Information required for Pre-Authorization */
	uchSeat = TranCurQualPtr->uchSeat;
	//----------------------------------------------------------------------------------
	// Asserts for invariant conditions needed for successful use of this function
	NHPOS_ASSERT_ARRAYSIZE(WorkGCF->aszNumber[uchSeat], NUM_NUMBER);
	//----------------------------------------------------------------------------------
	if (WorkGCF->auchExpiraDate[uchSeat][0] != '\0') {
		_RflStrncpyUchar2Tchar(ItemTender->auchExpiraDate, WorkGCF->auchExpiraDate[uchSeat], NUM_EXPIRA);
	}

	if (WorkGCF->refno[uchSeat].auchReferncNo[0] != '\0') {
		ItemTender->refno = WorkGCF->refno[uchSeat];
	}
	else {
		TrnGetTransactionInvoiceNum(ItemTender->refno.auchReferncNo);
		TrnGetTransactionInvoiceNum(ItemTender->invno.auchInvoiceNo);
	}

	if (WorkGCF->authcode[uchSeat].auchAuthCode[0] != '\0') {
		ItemTender->authcode = WorkGCF->authcode[uchSeat];
	}

	if (WorkGCF->auchAcqRefData[uchSeat][0] != '\0') {
		strncpy(ItemTender->tchAcqRefData, WorkGCF->auchAcqRefData[uchSeat], NUM_ACQREFDATA);
	}
	if (WorkGCF->srno[uchSeat].auchRecordNumber[0] != '\0') {
		ItemTender->srno = WorkGCF->srno[uchSeat];
	}

	/* End Information required for Pre-Authorization */
	if (WorkGCF->aszNumber[uchSeat][0] != '\0') {
		RflDecryptByteString(WorkGCF->aszNumber[uchSeat], sizeof(WorkGCF->aszNumber[uchSeat]));
		_RflStrncpyUchar2Tchar(ItemTender->aszNumber, WorkGCF->aszNumber[uchSeat], NUM_NUMBER);
		RflEncryptByteString(WorkGCF->aszNumber[uchSeat], sizeof(WorkGCF->aszNumber[uchSeat]));
		RflEncryptByteString((UCHAR *)&(ItemTender->aszNumber), sizeof(ItemTender->aszNumber));
	}
	else if (sAcct != 0) {
		NHPOS_ASSERT_TEXT(sAcct != 0, "**ERROR: Compulsory account number not allowed due to PCI-DSS");
		return LDT_ALTERNATE_TENDER_RQD;

#if defined(POSSIBLE_DEAD_CODE)
		sStatus = ItemTendDiaAcctChg(ItemTender, NUM_NUMBER);
		if (sStatus == UIF_PINPADSUCCESS || sStatus == UIF_VIRTUALKEYBOARD) {
			//if ItemTendDiaAcctChg has a successful pinpad read
			//then the date will not need to be entered
			bGetExpDate = FALSE;
			if (sStatus == UIF_VIRTUALKEYBOARD) {
				UifRegTender->GiftCard = IS_GIFT_CARD_FREEDOMPAY;
			}
		}
		else if (sStatus == UIF_CANCEL) {
			return(UIF_CANCEL);
		}
		else if (sStatus == EPT_SUCCESS) {
			// if in ItemTendDiaAcctChg the card swipe is cancelled
			// for a credit transaction that is set to have a compulsory account number,
			// ItemTendDiaAcctChg will prompt a user for an account number and
			// return EPT_SUCCESS to state that a success has accoured in getting
			// the account number from the terminal user. This function should not return
			// it should prompt for an expiration date.
			if (ItemTender->uchPaymentType == NEW_FSC_ENTRY) {
				UifRegTender->uchMajorClass = ItemTender->uchMajorClass;
				UifRegTender->uchMinorClass = ItemTender->uchMinorClass;
				// We need to ensure that the tender amount is positive since this may
				// also be a void transaction which would have negative tender amount.
				if (ItemTender->lTenderAmount >= 0)
					UifRegTender->lTenderAmount = ItemTender->lTenderAmount;
				return(UIF_NEW_FSC);
			}
		}
		else if (sStatus == EEPT_RET_PINNEEDED){
			/*
			this condition happens when we are doing a manual EBT transaction
			we have already got the card number and next we need the Exp Date and
			then we need the Pin number.
			*/
			sPinStatus = sStatus;
		}
		else {
			return(sStatus);
		}
#endif
	}

	if ((TranCurQualPtr->fsCurStatus2 & CURQUAL_REQUIRE_NON_GIFT) && (UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY)) {
		// do not allow use of FreedomPay if a FreedomPay has been funded
		// in this transaction.
		return(LDT_PROHBT_ADR);
	}

	/* request expiration date by MDC, 09/03/01 */
	if (ItemTender->auchExpiraDate[0] == '\0' && bGetExpDate) {
		/* check compulsory account # entry */
		if (ItemTenderCheckTenderMdc(ItemTender->uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_A)) {           /* Request ExpDate? */
			if ((sStatus = ItemTendCompEntry(ItemTender->auchExpiraDate, UIFREG_MAX_DIGIT4)) != ITM_SUCCESS) {
				return(UIF_CANCEL);
			}
			_tcsncpy(ItemTenderLocal.auchExpiraDate, ItemTender->auchExpiraDate, NUM_EXPIRA);
		}
	}

	if (ItemTenderCheckTenderMdc(ItemTender->uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_A)) {           /* Request ExpDate? */
		// Tender key is provisioned to request an expiration date.
		// Check the expiration date against the current date.
		USHORT       usMonth, usYear;
		DATE_TIME    DT;                         /* date & time */

		PifGetDateTime(&DT);
		usMonth = (ItemTender->auchExpiraDate[0] & 0x0f) * 10 + (ItemTender->auchExpiraDate[1] & 0x0f);
		usYear = (ItemTender->auchExpiraDate[2] & 0x0f) * 10 + (ItemTender->auchExpiraDate[3] & 0x0f) + 2000;
		if (usYear < DT.usYear) {
			return EEPTDLL_ERR_EXPIRE_DATE;
		}
		else if (usYear == DT.usYear && usMonth < DT.usMonth) {
			return EEPTDLL_ERR_EXPIRE_DATE;
		}
	}

	return UIF_SUCCESS;
}

SHORT   ItemTendEPTEx(UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
    SHORT           sStatus;
	TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
	BOOL		    bGetExpDate = TRUE;
	PARATENDERKEYINFO	TendKeyInfo;

	//----------------------------------------------------------------------------------
	// Asserts for invariant conditions needed for successful use of this function

		// Asserts for array bounds are large enough
		NHPOS_ASSERT_ARRAYSIZE (ItemTender->aszNumber, NUM_NUMBER);

		// Asserts for array data types are the same for memcpy, memset, _tcsncpy, _tcsnset, etc
		NHPOS_ASSERT (sizeof(ItemTenderLocal.auchExpiraDate[0]) == sizeof(ItemTender->auchExpiraDate[0]));
	//-----------------------------------------------------------------------------------

	//ItemTendDiaAcctChg needs the MinorClass of the Tender
	//so it can find out what kind of tender this transaction is
	//to know what account info to be getting
	ItemTender->uchMajorClass = CLASS_ITEMTENDER;
	ItemTender->uchMinorClass = UifRegTender->uchMinorClass;

	// Get the tender key transaction type and code so that we can make some decisions
	// based on the tender key attributes.
	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	// If this is an electronic payment transaction and the amount of the transaction is zero then Prohibit
	// We allow this to continue if doing Check No Purchase.
	if (!uchIsNoCheckOn && ItemTender->lRound == 0)
		return LDT_CP_NOALLOW_ADR;

	//Check Store and Forward Transaction Limits for everything except for Preauth Capture.
	// The exception of Preauth Capture is for Amtrak which has NetePay running on the terminal
	// so a Preauth Capture will always work unless NetePay is down.
	UifRegTender->uchSpecialProcessingFlags = 0;
	if(CliParaMDCCheckField(MDC_STR_FWD_ADR, MDC_PARAM_BIT_D) && TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_PREAUTHCAPTURE){
			return LDT_ALTERNATE_TENDER_RQD;
	}

	{
		ITEMCOMMONLOCAL *pItemCommonLocal = 0;

		ItemCommonGetLocal(&pItemCommonLocal);
		ItemTender->lGratuity = pItemCommonLocal->lChargeTip;  // find out how much tips have been charged if any.
	}


    /* Prompt for charge tips according to MDC Bit 444A
	   Cancel out of EPT transaction if operator forgot to
	   enter
	*/
	if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
		return(UIF_CANCEL);
	}

    /* pre-calculate total amount and display */
    ItemTenderLocal.fbTenderStatusSave = ItemTenderLocal.fbTenderStatus[0];
    ItemTenderLocal.fbTenderStatus[0] |= TENDER_EPT;       /* set EPT status   */
    ItemTenderLocal.fbTenderStatus[0] &= ~(TENDER_AC | TENDER_RC);     /* reset other tender status   */
    if ((ItemModLocal.fbModifierStatus & MOD_OFFLINE) ||/* E-003 corr. 4/18 */
        (UifRegTender->fbModifier & OFFCPTEND_MODIF)) { /* check modifier   */
        ItemTender->fbModifier &=~ OFFCPTEND_MODIF;     /* E-001 corr. 4/18 */
        ItemTender->fbModifier |= OFFEPTTEND_MODIF;     /* E-001 corr. 4/18 */
        ItemTenderLocal.fbModifier |= OFFEPTTEND_MODIF; /* E-069 corr. 4/28 */
        sStatus = ItemExecuteOffTend(UifRegTender, ItemTender);
        return(sStatus);
    }

	// The purpose of the following code is to provide a way for performing a Manual Card Entry action.
	// The reason for this is to allow the provisioning of a tender key so that it can be used to request
	// manual entry of the card account number, expiration date, CVV number, and the billing address zip code
	// when there is a problem processing either a card swipe or a card insertion in the case of EMV.
	switch (TendKeyInfo.TenderKeyInfo.usTranType) {
		case TENDERKEY_TRANTYPE_CREDIT:
		case TENDERKEY_TRANTYPE_DEBIT:
			if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_B) &&   // compulsory account number entry
				ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_B)) {   // perform Credit Authorization Operation
					_tcsncpy(ItemTender->aszNumber, L"Prompt7890123456", 20);
				RflEncryptByteString((UCHAR *)&(ItemTender->aszNumber), sizeof(ItemTender->aszNumber));
			}
			break;
		default:
			if ((sStatus = ItemTenderPerformRequiredMdc(UifRegTender, ItemTender)) != UIF_SUCCESS) {
				return sStatus;
			}
			break;
	}

	// allow us to over ride the tender key if this is an EPT with auto detection.
	// auto detection allows us to determine the tender key type from the account number.
	UifRegTender->uchMinorClass = ItemTender->uchMinorClass;

	//Reference Number
	//ItemTendReferenceAuthEntry will return UIF_CANCEL if cancel is pressed
	//return UIF_SUCCESS if successfull or return UIFREG_CLEAR meaning skip/not used
	sStatus = ItemTendReferenceAuthEntry(ItemTender, NUM_REFERENCE, LDT_REFERENCENUM_ADR);
	if (sStatus == UIF_CANCEL) {
		return(UIF_CANCEL);
	}
	else if (sStatus == UIF_SUCCESS) {
		ItemTenderLocal.refno = ItemTender->refno;
	}

	//Authorization Code
	sStatus = ItemTendReferenceAuthEntry(ItemTender, NUM_AUTHCODE, LDT_AUTHCODE_ADR);
	if (sStatus == UIF_CANCEL) {
		return(UIF_CANCEL);
	}
	else if (sStatus == UIF_SUCCESS) {
		ItemTenderLocal.authcode = ItemTender->authcode;
	}

	//Routing Number
	sStatus = ItemTendCheckAcctRoutSeqEntry(ItemTender, NUM_ROUTING_NUM, LDT_CHKROUTINGNUM_ADR);
	if (sStatus == UIF_CANCEL){
		return(UIF_CANCEL);
	}
	else if (sStatus == UIF_SUCCESS){
		_tcsncpy(ItemTenderLocal.tchRoutingNumber, ItemTender->tchRoutingNumber, NUM_ROUTING_NUM);
	}

#if 0
	// Do not ask for check sequence number for VCS Pilot
	//Check Sequence Number
	sStatus = ItemTendCheckAcctRoutSeqEntry(ItemTender, NUM_SEQUENCE_NUM, LDT_CHKSEQUENCENUM_ADR);
	if (sStatus == UIF_CANCEL) {
		return(UIF_CANCEL);
	}
	else if (sStatus == UIF_SUCCESS) {
		_tcsncpy(ItemTenderLocal.tchCheckSeqNo, ItemTender->tchCheckSeqNo, NUM_SEQUENCE_NUM);
	}
#endif

	/* check minus amount case to see if we should request supervisor intervention */
    if (ItemTender->lTenderAmount < 0L) {
		if (!(ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_4, MDC_PARAM_BIT_C))) {  /* negative tender requires supervisor int. */
            if (ItemSPVInt(LDT_SUPINTR_ADR) == UIF_CANCEL) {    /* cancel SI    */
                ItemTenderLocal.fbTenderStatus[0] &= ~TENDER_EPT;  /* reset EPT status   */
                ItemTendCPCanDisp(UifRegTender);                /* recovery display */
                return(UIF_CANCEL);
            }
        }
    }

#if 1
	if ((TranCurQualPtr->fsCurStatus & (CURQUAL_PRESELECT_MASK | CURQUAL_TRETURN)) && ItemTender->lRound < 0) {
		// this is a return or a void transaction and the transaction total is less than zero
		// so we need to check the rules to determine if this tender is allowed.
		SHORT   sGusRetValue;
		if (0 > (sGusRetValue = CliGusLoadTenderTableLookupAndCheckRules (ItemTender))) {
			return CliGusLoadTenderTableConvertError (sGusRetValue);
		}
		if (sGusRetValue & PARATENDRETURNGLOBAL_ALLOWDECLINED) {
			ItemTender->fbModifier |= DECLINED_EPT_MODIF;    // indicate that is is a declined EPT, EEPT_FC2_REJECTED
		}
		if (sGusRetValue & PARATENDRETURNGLOBAL_ALLOWSTORED) {
			UifRegTender->uchSpecialProcessingFlags |= EEPTREQDATA_SPECIALFLAGS_FORCESTORE;
		}
	}
#endif

	ItemTenderMdcToStatus(UifRegTender->uchMinorClass, &ItemTender->auchTendStatus[0]);

	sStatus = ItemTendEPTComm(EEPT_FC2_EPT, UifRegTender, ItemTender, TranModeQualPtr->ulCashierID);

	// if we are doing manual card entry prompting then we need to clear the aszNumber area so as to
	// prevent the aszNumber character string appearing on the printed receipt and Electronic Journal.
	switch (TendKeyInfo.TenderKeyInfo.usTranType) {
		case TENDERKEY_TRANTYPE_CREDIT:
		case TENDERKEY_TRANTYPE_DEBIT:
			if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_2, MDC_PARAM_BIT_B) &&   // compulsory account number entry
				ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_3, MDC_PARAM_BIT_B)) {   // perform Credit Authorization Operation
				memset(ItemTender->aszNumber, 0, sizeof(ItemTender->aszNumber));
			}
			break;
	}
	{
		PARAMISCPARA	ParaMiscPara;
		ULONG			ulMinTotal;
		ULONG           fsCurStatus = CURQUAL_MERCH_DUP;    // assume we will request the signature capture

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

		if ((ParaMiscPara.ulMiscPara & A128_CC_ENABLE) && (RflLLabs(ItemTender->lTenderAmount) <= (LONG)ulMinTotal))
		{
			if ((ParaMiscPara.ulMiscPara & A128_CC_MERC) != 0)	//Do not print merchant receipt   ***PDINU
			{
				fsCurStatus &= ~CURQUAL_MERCH_DUP;              // indicate do not do signature capture.
			}
		}

		if (ItemTenderCheckTenderMdc(UifRegTender->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_D)) {
			fsCurStatus |= CURQUAL_MERCH_DUP;              // indicate require signature signature capture.
		}

		if (sStatus == 0 && (fsCurStatus & CURQUAL_MERCH_DUP))
			ItemTendDiaSignatureCapture(ItemTender, NUM_NUMBER);
	}
    return sStatus;
}


/*
*==========================================================================
**  Synopsis:   SHORT ItemTendCPComm( UCHAR uchFuncCode,
*                                     UIFREGTENDER *UifRegTender,
*                                     ITEMTENDER *ItemTender )
*
*   Input:      UCHAR        uchFuncCode
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_SEQERR_ADR  : sequence error
*               LDT_ERR_ADR     : error
*
*   Description: communicate CPM
*==========================================================================
*/
SHORT   ItemTendCPComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender)
{
	NHPOS_ASSERT_TEXT(!LDT_ERR_ADR, "** ERROR: ItemTendCPComm() non-functional. CPM module removed");
	return(LDT_ERR_ADR);
}


/*==========================================================================
**  Synopsis:   VOID ItemTendCPCanDisp(UIFREGTENDER *UifTender)
*
*   Input:      nothing
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     nothing
*
*   Description: display total amount after canceled charge posting tender
==========================================================================*/

VOID ItemTendCPCanDisp(UIFREGTENDER *pUifTender)
{
    DCURRENCY           lAmount = 0;

    /* check partial or check total */
    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {
		REGDISPMSG  RegDispMsgD = { 0 };
		DCURRENCY   lSubTendAmount = 0;
		SHORT       sSplitStatus = TrnSplCheckSplit();

        /*  set class code for display  */
        RegDispMsgD.uchMajorClass = CLASS_UIFREGTENDER;
        RegDispMsgD.uchMinorClass = CLASS_UITENDER1;

        /* get and set mnemonics */
        RflGetLead ( RegDispMsgD.aszMnemonic, LDT_BD_ADR );  /* get mnemonic, balance due */

        /*----- Split Check,   R3.1 -----*/
        if (sSplitStatus == TRN_SPL_SEAT
            || sSplitStatus == TRN_SPL_MULTI
            || sSplitStatus == TRN_SPL_TYPE2) {
            ItemTendCalAmountSpl(&lSubTendAmount, pUifTender);
        } else if (sSplitStatus == TRN_SPL_SPLIT) {
            ItemTendCalAmountSplit(&lSubTendAmount, pUifTender);
        } else {
            ItemTendCalAmount(&lSubTendAmount, pUifTender);
        }

        lSubTendAmount += ItemTransLocalPtr->lWorkMI;              /* R3.0 */

        /* --- Netherland rounding, 2172 --- */
        if ( ! CliParaMDCCheck( MDC_ROUND1_ADR, ODD_MDC_BIT3 ) ) {
            if ( RflRound( &lAmount, lSubTendAmount, RND_TOTAL1_ADR ) == RFL_SUCCESS ) {
                lSubTendAmount;
            }
        }

        RegDispMsgD.lAmount = lSubTendAmount;
        RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_5;   /* save for recovery, DISP_SAVE_TYPE_5 */
        flDispRegDescrControl |= TENDER_CNTRL;   /* set descriptor*/

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT        /* GST exempt */
                || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {  /* PST exempt */
                flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
                flDispRegKeepControl |= TAXEXMPT_CNTRL;             /* tax exempt */
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
                flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
                flDispRegKeepControl |= TAXEXMPT_CNTRL;             /* tax exempt */
            }
        }
		DispWrite(&RegDispMsgD);
	} else {
		REGDISPMSG     RegDispMsgD = { 0 };

		RegDispMsgD.uchMajorClass = CLASS_UIFREGTOTAL;

        if (ItemTenderLocal.fbTenderStatus[1] & TENDER_FS_TOTAL) {
            RegDispMsgD.uchMinorClass = CLASS_UITOTAL9;
            RegDispMsgD.lAmount = ItemTenderLocal.ItemFSTotal.lMI;

            RflGetTranMnem (RegDispMsgD.aszMnemonic, TRN_TTL9_ADR);
            flDispRegDescrControl |= TOTAL_CNTRL;
        } else {
			ITEMCOMMONLOCAL     *pItemCommonL = ItemCommonGetLocalPointer();
			ITEMTOTAL           *pItemTotal = (ITEMTOTAL *) &pItemCommonL->ItemSales;

            RegDispMsgD.uchMinorClass = CLASS_UITOTAL2;

            RegDispMsgD.lAmount = pItemTotal->lMI + ItemTransLocalPtr->lWorkMI;  /* R3.0 */
            if (pItemCommonL->ItemSales.uchMajorClass == CLASS_ITEMTOTAL) {
                USHORT  usAddress = (TRN_TTL1_ADR + pItemCommonL->ItemSales.uchMinorClass  - 1);
                RflGetTranMnem( RegDispMsgD.aszMnemonic, usAddress);
                flDispRegDescrControl |= TOTAL_CNTRL;                   /* set descriptor */
            } else {
                flDispRegDescrControl &= ~TOTAL_CNTRL;                  /* reset descriptor */
                flDispRegKeepControl &= ~TOTAL_CNTRL;
            }
        }
        RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_1;                   /* set type1 */

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_GSTEXEMPT        /* GST exempt */
                || TranGCFQualPtr->fsGCFStatus & GCFQUAL_PSTEXEMPT ) {  /* PST exempt */
                flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
                flDispRegKeepControl |= TAXEXMPT_CNTRL;             /* tax exempt */
            }
        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {
            if ( TranGCFQualPtr->fsGCFStatus & GCFQUAL_USEXEMPT ) {
                flDispRegDescrControl |= TAXEXMPT_CNTRL;            /* tax exempt */
                flDispRegKeepControl |= TAXEXMPT_CNTRL;             /* tax exempt */
            }
        }
		DispWrite(&RegDispMsgD);
	}

    if (ItemTenderLocal.fbTenderStatus[0] & TENDER_PARTIAL) {
        flDispRegDescrControl &= ~TOTAL_CNTRL;                          /* reset descriptor */
    } else {
        flDispRegDescrControl &= ~(TOTAL_CNTRL | TAXEXMPT_CNTRL);       /* reset descriptor */
    }
}

/*==========================================================================
**  Synopsis:   VOID ItemTendCPCommDisp( VOID )
*
*   Input:      nothing
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     nothing
*
*   Description: display communication mnemonics
==========================================================================*/

VOID ItemTendCPCommDisp( VOID )
{
	REGDISPMSG      RegDispMsgD = {0};

    RegDispMsgD.uchMajorClass = CLASS_UIFREGOTHER;
    RegDispMsgD.uchMinorClass = CLASS_UILOWMNEMO;

    /* get and set mnemonics */
    RflGetLead (RegDispMsgD.aszMnemonic, LDT_COMUNI_ADR);
    RegDispMsgD.fbSaveControl = DISP_SAVE_TYPE_4;          /* display previous data at redisplay */
    DispWrite(&RegDispMsgD);
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendCPTrain( ITEMTENDER *ItemTender,
*                           UCHAR uchFuncCode, UIFREGTENDER *UifTender)
*
*   Input:      UCHAR       uchFuncCode
*   Output:     nothing
*   InOut:      ITEMTENDER  *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               UIF_CANCEL      : cancel by user
*
*   Description: trainning mode for room/acct. charge
==========================================================================*/

SHORT ItemTendCPTrain( ITEMTENDER *pItemTender, UCHAR uchFuncCode, UIFREGTENDER *pUifTender)
{
    TCHAR           asz123456[] = _T("123456");
    USHORT          usRemain;

    PifSleep(3000);         /* sleep 3 seconds */

    /* set folio. and post number */
    _tcsncpy(pItemTender->aszFolioNumber, asz123456, 6);
    _tcsncpy(pItemTender->aszPostTransNo, asz123456, 6);

    /* distinguish trainning handling */
    usRemain = ( USHORT)(pItemTender->lTenderAmount % 3L);

    if ((usRemain == 1) && (uchFuncCode == CPM_FC2_CREDITA)) {
        usRemain = 2;
    }

    if (usRemain == 0) {
        /* set room/acct. charge states */
        ItemTenderLocal.fbTenderStatus[0] |= (TENDER_RC | TENDER_AC);
        ItemTenderLocal.fbTenderStatus[0] &= ~TENDER_EPT;
        return(ITM_SUCCESS);

    } else if (usRemain == 1) {
        /* wait supervisor intervention */
        if (ItemSPVInt(LDT_OVERINT_ADR) == UIF_CANCEL) {   /* cancel SI */
            ItemTendCPCanDisp(pUifTender);        /* recover display */
            return(UIF_CANCEL);
        }

        /* display communication lead through */
        ItemTendCPCommDisp();
        PifSleep(3000);     /* sleep 3 seconds */

        /* set room charge states */
        ItemTenderLocal.fbTenderStatus[0] |= (TENDER_RC | TENDER_AC);
        ItemTenderLocal.fbTenderStatus[0] &= ~TENDER_EPT;
        return(ITM_SUCCESS);
    }else {
        return(LDT_CP_NOALLOW_ADR);     /* response charge not allow */
    }
}


/*==========================================================================
**  Synopsis:   VOID ItemTendCPConvData( UCHAR *pDesBuff,
*                                        size_t usDesSize,
*                                        const UCHAR FARCONST *pDataFmt,
*                                        LONG lSrcData )
*
*   Input:      UCHAR           *pDesBuff
*               SIZE_T          usDesSize
*               UCHAR FARCONST  *pDataFmt
*               LONG            lSrcData
*
*   Output:     nothing
*   InOut:      UCHAR           *pDesBuff
*
*   Return:     nothing
*
*   Description: convert 2170 data to CP format data
==========================================================================*/

VOID ItemTendCPConvData( UCHAR *pDesBuff,
                         size_t usDesSize,
                         const TCHAR  *pDataFmt,
                         DCURRENCY lSrcData)
{
	TCHAR   auchTmpBuff[33] = { 0 };

#if !defined(UNICODE)
	// Lets check to make sure that we are compiled with UNICODE turned on
	NHPOS_ASSERT(0);
#endif+

    RflSPrintf(auchTmpBuff, TCHARSIZEOF(auchTmpBuff) - 1, pDataFmt, lSrcData);

	NHPOS_ASSERT(usDesSize <= 0xFFFF);//MAXWORD
    _RflStrncpyTchar2Uchar (pDesBuff, auchTmpBuff, (USHORT)usDesSize);
}


/*==========================================================================
**  Synopsis:   SHORT ItemTendCPConvErr( UCHAR *pCPMErrorDef )
*
*   Input:      UCHAR           *pCPMErrorDef
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     2170 lead through error address
*
*   Description: convert CPM error to 2170 error
==========================================================================*/

SHORT ItemTendCPConvErr( UCHAR *pCPMErrorDef )
{
    USHORT  usLeadthruNo = LDT_ERR_ADR;
    SHORT   sCPMErrorDef;
    UCHAR   aszCPMErrorDef[3];

    /* convert ascii data to int. data */
	aszCPMErrorDef[0] = *pCPMErrorDef;
	aszCPMErrorDef[1] = *(pCPMErrorDef + 1);
	aszCPMErrorDef[2] = '\0';
    sCPMErrorDef = atoi(aszCPMErrorDef);

    switch (sCPMErrorDef) {         /* global error     */
    case CPM_PROHBT_ADR0:
    case CPM_PROHBT_ADR1:
    case CPM_PROHBT_ADR2:
        usLeadthruNo = LDT_PROHBT_ADR;

    case CPM_CP_NOALLOW_ADR:        /* charge not allow */
        usLeadthruNo = LDT_CP_NOALLOW_ADR;

    case CPM_GC_OUT_ADR:            /* guest checked out */
        usLeadthruNo = LDT_GC_OUT_ADR;

    case CPM_WRONG_RM_ADR:          /* wrong room number */
        usLeadthruNo = LDT_WRONG_RM_ADR;

    case CPM_WRONG_GID_ADR:         /* wrong guest ID */
        usLeadthruNo = LDT_WRONG_GID_ADR;

    case CPM_VOID_NOALLOW_ADR:      /* void not allowed */
        usLeadthruNo = LDT_VOID_NOALLOW_ADR;

    case CPM_WRONG_ACNO_ADR:        /* wrong account number */
        usLeadthruNo = LDT_WRONG_ACNO_ADR;

    case CPM_CP_ADVISE_ADR:         /* advise front desk */
        usLeadthruNo = LDT_CP_ADVISE_ADR;

    case CPM_WRONG_ACTYPE_ADR:      /* wrong account type */
        usLeadthruNo = LDT_WRONG_ACTYPE_ADR;

    case CPM_WRONG_SLD_ADR:         /* wrong SLD */
        usLeadthruNo = LDT_WRONG_SLD_ADR;

    case CPM_FLFUL_ADR:             /* charge file full */
        usLeadthruNo = LDT_FLFUL_ADR;

    case CPM_NTINFL_ADR:            /* guest number not found */
        usLeadthruNo = LDT_NTINFL_ADR;

    case CPM_WRONG_VOUCH_ADR:       /* wrong voucher number */
        usLeadthruNo = LDT_WRONG_VOUCH_ADR;

    default:
        usLeadthruNo = LDT_ERR_ADR;
    }

	if (sCPMErrorDef != STUB_SUCCESS) {
		PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CONVERTERROR_CPM);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), sCPMErrorDef);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), usLeadthruNo);
	}

	return usLeadthruNo;
}


/*==========================================================================
**  Synopsis:   VOID ItemTendCPPrintErr( USHORT usFuncCode, SHORT sErrorCode )
*
*   Input:      USHORT  usFuncCode
*               SHORT   sErrorCode
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     nothing
*
*   Description: print error code for charge posting snd EPT
==========================================================================*/

VOID ItemTendCPPrintErr( USHORT usFuncCode, SHORT sErrorCode )
{
	ITEMPRINT       ItemPrintData = {0};

    ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;
    ItemPrintData.uchMinorClass = CLASS_CP_ERROR;

    /* set function and error code */
    ItemPrintData.usFuncCode = usFuncCode;
    ItemPrintData.sErrorCode = sErrorCode;

    TrnThrough(&ItemPrintData);
}

/*==========================================================================
**  Synopsis:   SHORT ItemTendCPECTend( ITEMTENDER *ItemTender )
*
*   Input:      ITEMTENDER   *ItemTender
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_PROHBT_ADR  : prohibit error
*               UIF_CANCEL      : canceled by user
*                               : other error
*
*   Description: E/C handling for charge posting
==========================================================================*/

SHORT ItemTendCPECTend( ITEMTENDER *pItemTender )
{
	NHPOS_ASSERT_TEXT(!LDT_ERR_ADR, "** ERROR: ItemTendCPECTend() non-functional. CPM module removed");
	return LDT_ERR_ADR;
}

/*==========================================================================
**  Synopsis:   VOID    ItemTendCPECor( ITEMTENDER *ItemTender )
*
*   Input:      ITEMTENDER   *ItemTender
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     noting
*
*   Description: This function executes E/C process for Charge Posting.
============================================================================
*/
VOID    ItemTendCPECor( ITEMTENDER *pItemTender )
{
    UCHAR           fbNoPrint;
    UCHAR           fbCompPrint;
    TRANCURQUAL     * const pWorkCur = TrnGetCurQualPtr();

    fbNoPrint = pWorkCur->fbNoPrint;
    fbCompPrint = pWorkCur->fbCompPrint;
	pWorkCur->fbNoPrint |= (PRT_RECEIPT | PRT_SLIP);
	pWorkCur->fbCompPrint &= ~(PRT_RECEIPT | PRT_SLIP);

    ItemPrintStart(TYPE_THROUGH);               /* start print service      */
    ItemHeader(TYPE_THROUGH);

    pItemTender->uchMajorClass = CLASS_ITEMTENDEREC;
    pItemTender->fsPrintStatus = PRT_JOURNAL;
    TrnThrough(pItemTender);
    pItemTender->uchMajorClass = CLASS_ITEMTENDER;

	pWorkCur->fbNoPrint = fbNoPrint;
	pWorkCur->fbCompPrint = fbCompPrint;
    return;
}

/*==========================================================================
**  Synopsis:   SHORT ItemExecuteOffTend(UIFREGTENDER *UifRegTender,
*                                        ITEMTENDER *ItemTender);
*
*   Return:     ITM_SUCCESS : offline tender process success
*               other       : error
*
*   Description: This function check HALO & SPV, sets OFFTEND_MODIF bit and
*                approval Code (space).
===========================================================================
*/
SHORT   ItemExecuteOffTend(UIFREGTENDER *pUifRegTender, ITEMTENDER *pItemTender)
{
	PARATRANSHALO   WorkHALO = { 0 };

    /* get HALO parameter */
    WorkHALO.uchMajorClass = CLASS_PARATRANSHALO;
	WorkHALO.uchAddress = 0;
    if (pUifRegTender->uchMinorClass >= CLASS_UITENDER1 && pUifRegTender->uchMinorClass <= CLASS_UITENDER8) {
		WorkHALO.uchAddress = ( UCHAR )( HALO_TEND1_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER1 );
    } else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER9 && pUifRegTender->uchMinorClass <= CLASS_UITENDER11) {
		WorkHALO.uchAddress = ( UCHAR )( HALO_TEND9_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER9 );
    } else if (pUifRegTender->uchMinorClass >= CLASS_UITENDER12 && pUifRegTender->uchMinorClass <= CLASS_UITENDER20) {
		WorkHALO.uchAddress = ( UCHAR )( HALO_TEND12_ADR + pUifRegTender->uchMinorClass - CLASS_UITENDER12 );
    }
    CliParaRead(&WorkHALO);

    /* check off line tender limit amount */
    if ((ItemModLocal.fbModifierStatus & MOD_OFFLINE) || (pUifRegTender->fbModifier & OFFCPTEND_MODIF)) { /* check modifier   */
        if (RflHALO(pItemTender->lTenderAmount, WorkHALO.uchHALO) != RFL_SUCCESS) {
            /* reset each status */
            ItemTenderLocal.fbTenderStatus[0] &= ~(TENDER_RC | TENDER_AC);
            pItemTender->fbModifier &= ~(OFFCPAUTTEND_MODIF | OFFCPTEND_MODIF | OFFEPTTEND_MODIF);
            return(LDT_PROHBT_ADR);
        }
    }

    /* check request SuperVisor ? */
    if (CliParaMDCCheck(MDC_CPPARA1_ADR, ODD_MDC_BIT3)) {
        if (ItemSPVInt(LDT_SUPINTR_ADR) == UIF_CANCEL) {    /* cancel SI    */
            ItemTendCPCanDisp(pUifRegTender);                /* recovery display */

            /* reset each status */
            ItemTenderLocal.fbTenderStatus[0] &= ~(TENDER_RC | TENDER_AC);
            pItemTender->fbModifier &= ~(OFFCPAUTTEND_MODIF | OFFCPTEND_MODIF | OFFEPTTEND_MODIF);  // Supervisor Intervention failed, cancel Offline Tender
            return(UIF_CANCEL);
        }
    }

    /* success process  */
    return(ITM_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT ItemTendCollectAcct( ITEMTENDER    *pItem,
*                                          SHORT		 sLen,
*                                          UCHAR         *pauchFsc)
*
*   Input:      ITEMTENDER     *pItem
*               SHORT          sLen
*				UCHAR		   *pauchFsc
*   Output:     nothing
*   InOut:      ITEMTENDER      *pItem
*				UCHAR			*pauchFsc
*
*   Return:     ITM_SUCCESS     : successful
*
*   Description: Collect Account Information From MSR or Keyboard
==========================================================================*/
SHORT ItemTendReadPadMsr (UIFDIADATA  *pDia, ITEMTENDER *pItem, SHORT sLen, UCHAR* pauchFsc)
{
	*pauchFsc = FSC_CANCEL;
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	return LDT_EQUIPPROVERROR_ADR;
}

SHORT ItemTendCollectAcct(ITEMTENDER *pItem, SHORT sLen, UCHAR* pauchFsc)
{
	*pauchFsc = FSC_CANCEL;
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	return LDT_EQUIPPROVERROR_ADR;
}

/*==========================================================================
**  Synopsis:   SHORT   ItemTendDiaAcctChg(ITEMTENDER *pItem, SHORT sLen)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:
*   Description:    Account# Entry                              Saratoga
*==========================================================================
*/
SHORT   ItemTendDiaAcctChg(ITEMTENDER *pItem, SHORT sLen)
{
	UCHAR			auchFsc;
	REGDISPMSG      Disp = {0};

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
		// if Rel 2.1 and earlier behavior requested then we display the total amount when
		// asking for account # entry. This was a Rel 2.1 behavior some customers liked.
		Disp.lAmount = pItem->lRound;
	}
    RflGetLead (Disp.aszMnemonic, LDT_NUM_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	return ItemTendCollectAcct(pItem,sLen,&auchFsc);
}

SHORT ItemTendCollectSignatureOnly(USHORT usFsc, USHORT usMinorClass)
{
	REGDISPMSG		   Disp = {0};

	BlFwIfReadSignatureCapture(NULL);

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, LDT_CUST_NAME_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;
	{
		ITEMTENDER    ItemTender = {0};
		UIFDIADATA    Dia = {0};
		SHORT		  sPinPadRet;
		UCHAR           auchFsc;

		Dia.ulOptions = 0;                      // indicate the UIF_PINPAD_OPTION_READ_xxxx options
		Dia.ulOptions |= UIF_PINPAD_OPTION_READ_SIGNATURE;
		sPinPadRet = ItemTendReadPadMsr (&Dia, &ItemTender,  NUM_NUMBER, &auchFsc);
	}

	BlFwIfReadSignatureCaptureRead (0, usFsc, usMinorClass);

	Disp.fbSaveControl = DISP_SAVE_TYPE_0;
	DispWrite(&Disp);

	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	BlFwIfReadSignatureClear ();

	return 0;
}

SHORT ItemTendCollectSignature(ITEMTENDER *pItem, SHORT sLen, UCHAR* pauchFsc)
{
	SHORT			   sPinPadRet;
	REGDISPMSG		   Disp = {0};
	UIFDIADATA         Dia = {0};
	PARATENDERKEYINFO  TendKeyInfo;

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItem->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	Dia.lAmount = pItem->lTenderAmount;     // indicate the amount of the tender for display purposes if PINPad
	Dia.ulOptions = 0;                      // indicate the UIF_PINPAD_OPTION_READ_xxxx options
	if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_D) == 0) {
		switch (TendKeyInfo.TenderKeyInfo.usTranType) {
			case TENDERKEY_TRANTYPE_CREDIT:
				Dia.ulOptions |= UIF_PINPAD_OPTION_READ_SIGNATURE;
				break;

			default:
				break;
		}
	} else {
		Dia.ulOptions |= UIF_PINPAD_OPTION_READ_SIGNATURE;
	}

	sPinPadRet = ItemTendReadPadMsr (&Dia, pItem, sLen, pauchFsc);

	BlFwIfReadSignatureCaptureRead (0, FSC_TENDER, pItem->uchMinorClass);

	Disp.fbSaveControl = DISP_SAVE_TYPE_0;
	DispWrite(&Disp);

	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

	BlFwIfReadSignatureClear ();

	return sPinPadRet;
}

SHORT   ItemTendDiaSignatureCapture(ITEMTENDER *pItem, SHORT sLen)
{
	SHORT           sResult;
	UCHAR			auchFsc;
	REGDISPMSG      Disp = {0};
	PARATENDERKEYINFO  TendKeyInfo;

	if ( ! UieCheckMsrDeviceType (IMSR_SIG_CAPTURE))
		return 0;

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItem->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if (ItemTenderCheckTenderMdc(pItem->uchMinorClass, MDC_TENDER_PAGE_6, MDC_PARAM_BIT_D) == 0) {
		// Reqquire signature capture is not set for this tender key so check the
		// type of transaction to determine if signature capture should be done or not.
		return 0;
	}

	BlFwIfReadSignatureCapture(NULL);

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, LDT_CUST_NAME_ADR);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	sResult = ItemTendCollectSignature(pItem,sLen,&auchFsc);
	BlFwIfReadSignatureClear ();
	return sResult;
}


/*==========================================================================
**  Synopsis:   SHORT   ItemTendReferenceAuthEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:		UIF_SUCCESS - Successful
*				UIF_CANCEL - Abort, Cancel pressed, Error
*				UIFREG_CLEAR - skip tender Code does not require Reference Number
*
*   Description:  Used for both Reference# Entry and Authorization Code entry
*                 when dealing with Electronic Payment transactions requring
*                 the Cashier to enter a Reference Number and/or an Authorization Code.
*                 This is used with Credit Sale Void tender key transaction code and
*                 transaction type.
*
*                 Mercury Payments seems to use a two character card type prefix to
*                 its Authorization Code such as VI for VISA or MC for MasterCard.
*==========================================================================
*/
SHORT   ItemTendReferenceAuthEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress)
{
    SHORT           sReturn;
    UIFDIADATA      Dia = {0};
	REGDISPMSG      Disp = {0};
	PARATENDERKEYINFO TendKeyInfo;

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, uchLeadAddress);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

    /* --- Reference # Entry --- */
	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItem->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if((TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_DEBIT) ||
		(TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_VOID_SALE &&
		 TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_VOID_RETURN &&
		 TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_ADJUST) ){

		Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
		DispWrite(&Disp);

		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

		return(UIFREG_CLEAR);
	}

	// some types of Electronic Payment servers will use a two character prefix
	// with the AuthCode such as VI for VISA or MC for MasterCard which requires
	// the use of Alphanumeric keyboard and UIFREG_ALFANUM_INPUT as an argument to UifDiaCP6()
    sReturn = UifDiaCP6(&Dia, (UCHAR)sLen, UIFREG_NUMERIC_INPUT);
    if (sReturn == UIF_SUCCESS) {
		// for a numeric keyboard we will look for #/Type key as the Enter key however
		// for an alphanumeric keyboard we will look for the P1 key as the Enter Key.
        if (Dia.auchFsc[0] == FSC_NUM_TYPE || Dia.auchFsc[0] == FSC_P1) {
			if(uchLeadAddress == LDT_AUTHCODE_ADR){
				_RflMemcpyTchar2Uchar(pItem->authcode.auchAuthCode, Dia.aszMnemonics, sLen);
			}else{
				_RflMemcpyTchar2Uchar(pItem->refno.auchReferncNo, Dia.aszMnemonics, sLen);
			}
        } else if (Dia.auchFsc[0] == FSC_SCANNER) {
			if(uchLeadAddress == LDT_AUTHCODE_ADR){
				_RflMemcpyTchar2Uchar(pItem->authcode.auchAuthCode, Dia.aszMnemonics, sLen);
			}else{
				_RflMemcpyTchar2Uchar(pItem->refno.auchReferncNo, Dia.aszMnemonics, sLen);
			}
        }
        return(UIF_SUCCESS);
    } else if (sReturn == UIFREG_ABORT) {
        Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
        DispWrite(&Disp);

		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);
        return(UIF_CANCEL);
    }

    return(UIF_SUCCESS);
}

/*==========================================================================
**  Synopsis:   SHORT   SHORT ItemTendCheckAcctRoutSeqEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:		UIF_SUCCESS - Successful
*				UIF_CANCEL - Abort, Cancel pressed, Error
*				UIFREG_CLEAR - skip tender Code does not require Reference Number
*
*   Description:    Routing/Sequence Number Entry
*==========================================================================
*/
SHORT	ItemTendCheckAcctRoutSeqEntry(ITEMTENDER *pItem, SHORT sLen, UCHAR uchLeadAddress)
{
    SHORT           sReturn;
    UIFDIADATA      Dia = {0};
	REGDISPMSG      Disp = {0};
	PARATENDERKEYINFO TendKeyInfo = { 0 };

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CPAN;
    RflGetLead (Disp.aszMnemonic, uchLeadAddress);

    flDispRegDescrControl |= COMPLSRY_CNTRL;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    DispWrite(&Disp);
    flDispRegDescrControl &= ~COMPLSRY_CNTRL;

    /* --- Routing/Sequence # Entry --- */
	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pItem->uchMinorClass;
	CliParaRead(&TendKeyInfo);

	if (TendKeyInfo.TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_MICR_MANUAL){

		Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
		DispWrite(&Disp);

		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

		return(UIFREG_CLEAR);
	}

	// some types of Electronic Payment servers will use a two character prefix
	// with the AuthCode such as VI for VISA or MC for MasterCard which requires
	// the use of Alphanumeric keyboard and UIFREG_ALFANUM_INPUT as an argument to UifDiaCP6()
    sReturn = UifDiaCP6(&Dia, (UCHAR)sLen, UIFREG_NUMERIC_INPUT);
    if (sReturn == UIF_SUCCESS) {
		// for a numeric keyboard we will look for #/Type key as the Enter key however
		// for an alphanumeric keyboard we will look for the P1 key as the Enter Key.
        if (Dia.auchFsc[0] == FSC_NUM_TYPE || Dia.auchFsc[0] == FSC_P1) {
			if(uchLeadAddress == LDT_CHKROUTINGNUM_ADR){
				_tcsncpy(pItem->tchRoutingNumber, Dia.aszMnemonics, sLen);
			}else{
				_tcsncpy(pItem->tchCheckSeqNo, Dia.aszMnemonics, sLen);
			}
        } else if (Dia.auchFsc[0] == FSC_SCANNER) {
			if(uchLeadAddress == LDT_CHKROUTINGNUM_ADR){
				_tcsncpy(pItem->tchRoutingNumber, Dia.aszMnemonics, sLen);
			}else{
				_tcsncpy(pItem->tchCheckSeqNo, Dia.aszMnemonics, sLen);
			}
        }
        return(UIF_SUCCESS);
    } else if (sReturn == UIFREG_ABORT) {
        Disp.fbSaveControl = DISP_SAVE_ECHO_ONLY;
        DispWrite(&Disp);

		RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

        return(UIF_CANCEL);
    }

    return(UIF_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCheckLabel12(UCHAR *aszNumber)
*
*   Input:      UCHAR *aszNumber
*   Output:     nothing
*   InOut:      nothing
*   Return:     UIF_SUCCESS     : successful
*               UIF_CANCEL      : error
*   Description:    Check Number as Label Type 12
*==========================================================================
*/
SHORT   ItemTendCheckLabel12(TCHAR *aszNumber)
{
	TCHAR   achLabel[3] = { 0 };
	PARAMDC WorkMDC = { 0 };

    WorkMDC.uchMajorClass = CLASS_PARAMDC;

    /* --- EAN-13 Digits Label by Scanner --- */
    if (aszNumber[0] == _T('A')) {
        achLabel[0] = aszNumber[1];
        achLabel[1] = aszNumber[2];

    /* --- UPC-A 12 Digits Label by Scanner --- */
    } else if (aszNumber[1] == _T('A')) {
        achLabel[0] = _T('0');
        achLabel[1] = aszNumber[2];

    /* --- EAN-13/UPC-A Label by PLU# --- */
    } else if (aszNumber[13] != 0) {
        achLabel[0] = aszNumber[1];
        achLabel[1] = aszNumber[2];

    /* --- UPC-A W/O CD Label by PLU# --- */
    } else if (aszNumber[13] == 0) {
        achLabel[0] = aszNumber[1];
        achLabel[1] = aszNumber[2];

    } else {
        return(LDT_PROHBT_ADR);
    }
    achLabel[2] = _T('\0');

    switch (_ttoi(achLabel)) {
    case 02:
        WorkMDC.usAddress = MDC_PLULB02H_ADR;
        break;

    case 04:
        WorkMDC.usAddress = MDC_PLULB04H_ADR;
        break;

    case 20:
        WorkMDC.usAddress = MDC_PLULB20H_ADR;
        break;

    case 21:
        WorkMDC.usAddress = MDC_PLULB21H_ADR;
        break;

    case 22:
        WorkMDC.usAddress = MDC_PLULB22H_ADR;
        break;

    case 23:
        WorkMDC.usAddress = MDC_PLULB23H_ADR;
        break;

    case 24:
        WorkMDC.usAddress = MDC_PLULB24H_ADR;
        break;

    case 25:
        WorkMDC.usAddress = MDC_PLULB25H_ADR;
        break;

    case 26:
        WorkMDC.usAddress = MDC_PLULB26H_ADR;
        break;

    case 27:
        WorkMDC.usAddress = MDC_PLULB27H_ADR;
        break;

    case 28:
        WorkMDC.usAddress = MDC_PLULB28H_ADR;
        break;

    case 29:
        WorkMDC.usAddress = MDC_PLULB29H_ADR;
        break;

    default:
        return(LDT_PROHBT_ADR);
    }

    /* Check Label type */
    CliParaRead(&WorkMDC);
    if (WorkMDC.uchMDCData != (EVEN_MDC_BIT1 | EVEN_MDC_BIT2 | EVEN_MDC_BIT3)) {/* 1110 as Label Type12 */
        return(LDT_PROHBT_ADR);
    }

    if (aszNumber[13] != ItemCommonCDV(&aszNumber[1], 12)) {
        return(LDT_PROHBT_ADR);
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT ItemTendEPTComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender,
*                                     ITEMTENDER *ItemTender, USHORT usCashierID)
*   Input:      UCHAR        uchFuncCode
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_SEQERR_ADR  : sequence error
*               LDT_ERR_ADR     : error
*
*   Description: communicate EPT                                Saratoga
*==========================================================================
*/
static SHORT   ItemTendEPTCommAcceptProcessing (USHORT usAutoStoreEvent, EEPTREQDATA *pCPSend, EEPTRSPDATA *pCPRcv, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, PARATENDERKEYINFO *pTendKeyInfo)
{
    USHORT    usMsgIndex;

	/*  After we have completely processed the transaction
		we will no longer need to store account information
		inside of the Guest Check File.
	*/
	{
		// get the record number from the batch to have in the case of batch settlement
		// requirements for Amtrak
		int i;
		for(i = 0; i < NUM_POSTTRAN; i++)
			ItemTender->aszPostTransNo[i] = pCPRcv->auchPostTransNo[i];
		ItemTender->aszPostTransNo[NUM_POSTTRAN - 1] = 0;
	}

//			if(UifRegTender->GiftCard == IS_GIFT_CARD_GENERIC || UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY || UifRegTender->GiftCard == IS_GIFT_CARD_EPAYMENT)
	{
		int j = 0, i = 0;

		// this is static for the moment. we may need to add some code
		// that will determine what uchPrintSelect should be set to
		// instead of just NUM_SLIP_1LINE
		ItemTender->uchPrintSelect = NUM_SLIP_1LINE;
		usMsgIndex = 0;
		for(i = 0; i < NUM_CPRSPCO_CARDLABEL;i++)
		{
			// if data exists before we copy to the next array in aszCPMsgText[i]
			// we will want to increment the no. of lines to be printed.
			if(pCPRcv->auchMsgText[usMsgIndex])
				ItemTender->uchCPLineNo++;

			for(j = 0; j < NUM_CPRSPTEXT; j++)
			{
				ItemTender->aszCPMsgText[i][j] = pCPRcv->auchMsgText[usMsgIndex++];
			}
		}

		for(i = 0; i < NUM_AUTHCODE; i++)
			ItemTender->authcode.auchAuthCode[i] = pCPRcv->auchAuthCode[i];
		for(i = 0; i < NUM_REFERENCE; i++)
			ItemTender->refno.auchReferncNo[i] = pCPRcv->auchRefNo[i];
		for(i = 0; i < NUM_INVOICE_NUM; i++)
			ItemTender->invno.auchInvoiceNo[i] = pCPRcv->auchInvoiceNo[i];

		ItemTender->srno = pCPRcv->srno;

		i = _tcslen(pCPSend->aszCardLabel);
		if (i > 4) {
			_tcsncpy (ItemTender->ITEMTENDER_CARDTYPE, pCPSend->aszCardLabel, i - 4);
		} else {
			ItemTender->ITEMTENDER_CARDTYPE[0] = 0;
		}
	}

	{
		ITEMDISC  *pItemDisc = TrnGetCurChargeTip(0);
		ITEMCOMMONLOCAL *pItemCommonLocal = ItemCommonGetLocalPointer();

		if (pItemCommonLocal->ReturnsTenderChargeTips.uchMinorClass) {
			ITEMTENDER_CHARGETIPS(ItemTender) = pItemCommonLocal->ReturnsTenderChargeTips;
		} else if (pItemDisc->uchMinorClass) {
			ITEMTENDER_CHARGETIPS(ItemTender).uchSignature = 0xff;
			ITEMTENDER_CHARGETIPS(ItemTender).uchMinorClass = pItemDisc->uchMinorClass;
			ITEMTENDER_CHARGETIPS(ItemTender).uchMinorTenderClass = ItemTender->uchMinorClass;
			ITEMTENDER_CHARGETIPS(ItemTender).fbReduceStatus = pItemDisc->fbReduceStatus;
			ITEMTENDER_CHARGETIPS(ItemTender).lGratuity = pItemDisc->lAmount;
			ITEMTENDER_CHARGETIPS(ItemTender).fbModifier = pItemDisc->fbDiscModifier;
			ITEMTENDER_CHARGETIPS(ItemTender).fsPrintStatus = pItemDisc->fsPrintStatus;
		} else {
			ITEMTENDER_CHARGETIPS(ItemTender).uchSignature = 0xff;
			ITEMTENDER_CHARGETIPS(ItemTender).uchMinorTenderClass = ItemTender->uchMinorClass;
			ITEMTENDER_CHARGETIPS(ItemTender).lGratuity = ItemTender->lGratuity;
		}
//				ItemTender->lGratuity = ITEMTENDER_CHARGETIPS(ItemTender).lGratuity;
		TrnClearCurChargeTip(0);
		memset (&(pItemCommonLocal->ReturnsTenderChargeTips), 0, sizeof(pItemCommonLocal->ReturnsTenderChargeTips));
		memset (pItemCommonLocal->aszCardLabelReturnsTenderChargeTips, 0, sizeof(pItemCommonLocal->aszCardLabelReturnsTenderChargeTips));
	}

	{
		TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
		UCHAR           uchSeat = TranCurQualPtr->uchSeat;

		NHPOS_ASSERT_TEXT((uchSeat <= NUM_SEAT_CARD), "==ERROR: seat number specified higher than struct size.");
		if (uchSeat > NUM_SEAT_CARD)  // there are places in code where value of seat may be 99 so lets guard for that
			uchSeat = 0;

		memset(WorkGCF->aszNumber[uchSeat], '\0', sizeof(WorkGCF->aszNumber[uchSeat]));
		memset(WorkGCF->auchExpiraDate[uchSeat], '\0', sizeof(WorkGCF->auchExpiraDate[uchSeat]));
		memset(WorkGCF->auchAcqRefData[uchSeat], '\0', sizeof(WorkGCF->auchAcqRefData[uchSeat]));
		memset(WorkGCF->auchMSRData[uchSeat], '\0', sizeof(WorkGCF->auchMSRData[uchSeat]));
		memset (WorkGCF->refno + uchSeat, '\0', sizeof(WorkGCF->refno[uchSeat]));
		memset (WorkGCF->authcode + uchSeat, '\0', sizeof(WorkGCF->authcode[uchSeat]));
		memset(WorkGCF->srno + uchSeat, '\0', sizeof(WorkGCF->srno[uchSeat]));

	}
		
	if(UifRegTender->GiftCardOperation == GIFT_CARD_BALANCE){
		UCHAR temp[12] = { 0 };
		int x = 0, i = 0;

		for( x = 0; x < 10; x++){
			if(pCPRcv->auchBalance[x] != _T('.'))
			{	temp[i] = pCPRcv->auchBalance[x];
				i++;
			}
		}
		temp[i] = 0;
		UifRegTender->lTenderAmount = atol(temp);
		ItemTender->lTenderAmount = atol(temp);
	}

	if (UifRegTender->GiftCardOperation == GIFT_CARD_LOOKUPNAMESS4 && pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD) {
		int x = 0;

		memset (ItemTender->aszNumber, 0, sizeof(ItemTender->aszNumber));
		for (x = 0; x < 20; x++) {
			ItemTender->aszNumber[x] = pCPRcv->auchBankRespCode[x];
		}
		RflEncryptByteString ((UCHAR *)&(ItemTender->aszNumber[0]), sizeof(ItemTender->aszNumber));
	}

	return 0;
}

static SHORT   ItemTendEPTCommDoComms (UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, PARATENDERKEYINFO *pTendKeyInfo, ULONG ulCashierID)
{
    UCHAR           uchTendStatus;
    SHORT           sStatus, sResponse = 0;
    USHORT          usRet, usEventSend, usEventRcv, usMsgIndex, usAutoStoreEvent = 0;
	EEPTREQDATA     CPSend = { 0 };
	EEPTRSPDATA     CPRcv = { 0 };
	SHORT           sFPresponse = 0;
	WCHAR           refnumMercury[] = _T("Void Refno not found");
	WCHAR           refnumDCap[] = _T("INVALID REQUEST:");

    /* edit send data */
    ItemTender->uchCPLineNo = 0;

	if (uchFuncCode == EEPT_FC2_EPT) {
        usEventSend = LOG_EVENT_EPT_SND;
        usEventRcv = LOG_EVENT_EPT_RCV;
        uchTendStatus = TENDER_EPT;
    } else if (uchFuncCode == EEPT_FC2_ASK) {
		usEventSend = LOG_EVENT_CP_ASK_SND;
		usEventRcv = LOG_EVENT_CP_ASK_RCV;
	}
	else {
		NHPOS_ASSERT_TEXT((uchFuncCode == EEPT_FC2_EPT), "**ERROR: ItemTendEPTCommDoComms() invalid uchFuncCode.");
		return LDT_PROHBT_ADR;
	}

	{
		ITEMDISC  *pItemDisc = TrnGetCurChargeTip(0);
		ITEMCOMMONLOCAL *pItemCommonLocal = 0;

		ItemCommonGetLocal(&pItemCommonLocal);

		if (pItemCommonLocal->ReturnsTenderChargeTips.uchMinorClass) {
			ITEMTENDER_CHARGETIPS(ItemTender) = pItemCommonLocal->ReturnsTenderChargeTips;
		} else if (pItemDisc->uchMinorClass) {
			ITEMTENDER_CHARGETIPS(ItemTender).uchSignature = 0xff;
			ITEMTENDER_CHARGETIPS(ItemTender).uchMinorClass = pItemDisc->uchMinorClass;
			ITEMTENDER_CHARGETIPS(ItemTender).uchMinorTenderClass = ItemTender->uchMinorClass;
			ITEMTENDER_CHARGETIPS(ItemTender).fbReduceStatus = pItemDisc->fbReduceStatus;
			ITEMTENDER_CHARGETIPS(ItemTender).lGratuity = pItemDisc->lAmount;
			ITEMTENDER_CHARGETIPS(ItemTender).fbModifier = pItemDisc->fbDiscModifier;
			ITEMTENDER_CHARGETIPS(ItemTender).fsPrintStatus = pItemDisc->fsPrintStatus;
		} else {
			ITEMTENDER_CHARGETIPS(ItemTender).lGratuity = ItemTender->lGratuity;
		}
	}

    ItemTendEPTEdit(CPM_NORMALTYP, uchFuncCode, &CPSend, UifRegTender, ItemTender, pTendKeyInfo, 0L, 0xff, 0L, ulCashierID);

	//Do not allow Returns on Positive Amounts
	//in order to do a return NHPOS must have voided
	//creating a negative amount

	if((pTendKeyInfo->TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_RETURN && CPSend.auchChargeTtl[0] != '-') ||
		(pTendKeyInfo->TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_VOID_RETURN && CPSend.auchChargeTtl[0] == '-') ||
		(pTendKeyInfo->TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_FOODSTAMPRETURN && CPSend.auchChargeTtl[0] != '-') ||
		(pTendKeyInfo->TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_CASHRETURN)){
		//Disallow Cash Return, this only corresponds to FS
		//No server is available, but can be implemented later
		NHPOS_NONASSERT_NOTE("==OPERATION", "==OPERATION: TranCode for return, etc. requires negative amount.")
		return LDT_PROHBT_ADR;
	}

	// for Employee Payroll Deduction and E-Meal for VCS we want to eliminate card holder and card label
	if (pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD || pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EMEAL) {
		ItemTender->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER][0] = 0;
		ItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL][0] = 0;
	}

	/* display communication lead through */
    ItemTendCPCommDisp();

    /* check training mode, EPT Tender/ASK */
	{
		if (TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) {
			sStatus = ItemTendCPTrain(ItemTender, uchFuncCode, UifRegTender);
			return(sStatus);
		}
	}

    do {
		/* initialize receive buffer */
		memset(&CPRcv, '\0', sizeof(EEPTRSPDATA));
		sStatus = XEEptBuildXMLAndExecuteTrans(&CPSend, &CPRcv);

		if (sStatus != EEPT_RET_SUCCESS) {
			RflMemRand (&CPSend, sizeof(CPSend));
			if(sStatus == UIF_CANCEL){
				return sStatus;
			}
			/* print error message to journal */
			usRet = CpmConvertError(sStatus);
			ItemTendCPPrintErr(usEventSend, usRet);
			ItemTendCPCanDisp(UifRegTender);
			return(usRet);
		}

		/* print error message to journal */
		usAutoStoreEvent = 0;    // clear the auto store event indicator to be set if there was one.
        if (sStatus <= 0) {
			LONG  lError = RflConvertCharFieldToLongCurrency(CPRcv.auchErrorNumber, 6);
            if (!sStatus) {             /* receive length 0 */
                sStatus = CPM_RET_LENGTH;
            }
            usRet = CpmConvertError(sStatus);
			if (usRet == LDT_ERR_ADR) {
				UieSetEptErrorText(EEPTDLL_USE_ERROR_TEXT, CPRcv.auchErrorText);
			}
			else if (sStatus == EEPTDLL_MULTIUSE_3328) {
				UieSetEptErrorText(LDT_EPT_ERROR_MULTIUSE, CPRcv.auchErrorText);
			}
            ItemTendCPPrintErr(usEventRcv, usRet);
            ItemTendCPCanDisp(UifRegTender);
			{
				char  xBuff[128];
				sprintf(xBuff, "ItemTendEPTCommDoComms(): sStatus = %d, usRet = %d, lError = %d", sStatus, usRet, lError);
				NHPOS_ASSERT_TEXT((sStatus == EPT_SUCCESS), xBuff);
			}
			RflMemRand (&CPRcv, sizeof(CPSend));
			switch (lError) {
				case 1007:    // EEPTINTERFACE_TIMEOUT_RESPONSE - Timeout on Response
				case 2004:    // EEPTINTERFACE_DISCONNECTING_SOCKET - Disconnecting Socket
//			Testing with a simple web server indicates that an error 3007 may indicate the request was never sent.
//			It appears the dsiEMVUS control attempts a connection to the Datacap server before it tries to do any other
//			processing so a 3007 indicates a problem with initial set so no need to do a retry.
//			Also a 4003 error code with Mercury Payments dsiEMVUS local DSI Server if LAN cable pulled during the
//			transaction processing should be ignored since it seems that the request never made it out to the processor.
//				case 3007:    // EEPTINTERFACE_CONNECT_LOST - Connection lost
//				case 4003:    // Global String Error
				case 3010:    // EEPTINTERFACE_TIMEOUT_SERVER_RESPONSE - Timeout waiting on Server response
					usRet = LDT_REQST_TIMED_OUT;
					break;
			}
            return(usRet);
		} else if (sStatus > 0x4000) {
			usAutoStoreEvent = 1;    // EEPT_RET_AUTO_STORED, we have had an auto store event
		}

        /* --- Check Function/Response Status --- */
        if ((sStatus = ItemTendCPFuncRsp1(uchFuncCode, &CPRcv, sResponse)) != ITM_SUCCESS) {
            return(sStatus);
        }

		if(pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY)
		{
			if(pTendKeyInfo->TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_SALE && (CPRcv.auchEptStatus == EEPT_FC2_ACCEPTED))
				if(ItemTender->lTenderAmount != ItemTender->lRound)
					CPRcv.auchEptStatus = EEPT_FC2_PARTTEND;
		}
		else{
			if(CPRcv.auchEptStatus == EEPT_FC2_ACCEPTED || CPRcv.auchEptStatus == EEPT_FC2_STORED){
				LONG       newBalance = 0, checkBalance = 0;

				newBalance = RflConvertCharFieldToLongCurrency(CPRcv.auchAuthorize, 10);

				//The is the Tender Amount - the Authorized Amount
				checkBalance = ItemTender->lTenderAmount - newBalance;
				if(ItemTender->lTenderAmount < 0)
				{
					ItemTender->lTenderAmount = (newBalance * -1L);
				}
				else{
					ItemTender->lTenderAmount = newBalance;
				}
				// If the Tender and the Authorized Amount are not the same then its a Partial Tender
				// If this is an Employee Payroll Deduction or an E-Meal (VCS) then we allow partial payments as well.
				// We will also allow partial payments with a Debit card.
				if(checkBalance > 0L)
				{
					CPRcv.auchEptStatus = EEPT_FC2_PARTTEND;
				}
			}
		}

        /* check response data */
		switch (CPRcv.auchEptStatus) {
        case EEPT_FC2_STORED:
			ItemTender->fbModifier |= STORED_EPT_MODIF;    // indicate that is is a Stored EPT, EEPT_FC2_STORED
        case EEPT_FC2_ACCEPTED:

			if ((ItemTender->fbModifier & STORED_EPT_MODIF) == 0) {
				ItemTender->fbModifier |= APPROVED_EPT_MODIF;    // indicate that is is an approved EPT, EEPT_FC2_ACCEPTED
			}

			ItemTendEPTCommAcceptProcessing (usAutoStoreEvent, &CPSend, &CPRcv, UifRegTender, ItemTender, pTendKeyInfo);

            if (uchFuncCode == EEPT_FC2_ASK) {
				SHORT  sRet = ItemTendEEPTAcceptReport(&CPRcv, UifRegTender, ItemTender);
				RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
				RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
            } else {
				SHORT  sRet = ItemTendEEPTAccept(uchFuncCode, ItemTender, &CPRcv);
				RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
				RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
				return sRet;
            }

            /* set acct. charge states */
            ItemTenderLocal.fbTenderStatus[0] |= uchTendStatus;
			return(ITM_SUCCESS);

        case EEPT_FC2_DECLINED:
        case EEPT_FC2_REJECTED:
			ItemTender->fbModifier |= DECLINED_EPT_MODIF;    // indicate that is is a declined EPT, EEPT_FC2_REJECTED
//			if(UifRegTender->GiftCard == IS_GIFT_CARD_GENERIC || UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY)
			{
				int j = 0, i = 0;

				// this is static for the moment. we may need to add some code
				// that will determine what uchPrintSelect should be set to
				// instead of just NUM_SLIP_1LINE
				ItemTender->uchPrintSelect = NUM_SLIP_1LINE;
				usMsgIndex = 0;
				for(i = 0; i < NUM_CPRSPCO_CARDLABEL;i++)
				{
					// if data exists before we copy to the next array in aszCPMsgText[i]
					// we will want to increment the no. of lines to be printed.
					if(CPRcv.auchMsgText[usMsgIndex])
						ItemTender->uchCPLineNo++;

					for(j = 0; j < NUM_CPRSPTEXT;j++)
					{
						ItemTender->aszCPMsgText[i][j] = (TCHAR)CPRcv.auchMsgText[usMsgIndex++];
					}
				}

				for(i=0; i < NUM_AUTHCODE;i++)
					ItemTender->authcode.auchAuthCode[i] = CPRcv.auchAuthCode[i];
				for(i=0; i < NUM_REFERENCE;i++)
					ItemTender->refno.auchReferncNo[i] = CPRcv.auchRefNo[i];

			}

			//there is no return number for a refno error.
			//so we are comparing to the error text.
			if(wcscmp(CPRcv.auchErrorText, refnumDCap) == 0){
				memset(&ItemTender->refno, '\0', sizeof(ItemTender->refno));
			}
			else if(wcscmp(CPRcv.auchErrorText, refnumMercury) == 0){
				memset(&ItemTender->refno, '\0', sizeof(ItemTender->refno));
			}

            sStatus = ItemTendCPReject(uchFuncCode, &CPRcv, ItemTender);
			RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
			RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
			return sStatus;

        case EEPT_FC2_OVERRIDE:
            if ((sStatus = ItemTendCPOverride(uchFuncCode, &CPSend, &CPRcv, UifRegTender, ItemTender, usEventSend)) != ITM_SUCCESS) {
				RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
				RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
                return(sStatus);
            }
            sResponse = EEPT_FC2_OVERRIDE;
			// drop out of the switch() in order to retry the EPT attempt.
            break;

        case EEPT_FC2_PARTTEND:
			PifBeep(UIE_ALERT_TONE);     // alert the operator that there is a Balance Due and a Partial Tender.
            if ((sStatus = ItemTendPartTender(UifRegTender, ItemTender)) != ITM_SUCCESS) {
				RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
				RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
                return(sStatus);
            }
			// This is a partial tender so lets make sure that the various data structures and data files are updated
			// with this Electronic Payment so that if this is a Preauth then we will record the correct data for
			// a Preauth Capture.
			ItemTendEPTCommAcceptProcessing (usAutoStoreEvent, &CPSend, &CPRcv, UifRegTender, ItemTender, pTendKeyInfo);
            sStatus = ItemTendEPTPartial(uchFuncCode, ItemTender, &CPRcv, UifRegTender);
			RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
			RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
			return sStatus;

        case EEPT_FC2_ERROR:
            sStatus = ItemTendCPReject(uchFuncCode, &CPRcv, ItemTender);
			RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
			RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
			return sStatus;

        case EEPT_FC2_INVALID:
			RflMemRand(&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
			RflMemRand(&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
			return(LDT_EPT_REJECT_ADR);

        default:
			PifLog (MODULE_EEPT, CPRcv.auchEptStatus);
			PifLog (MODULE_EEPT, (USHORT)__LINE__);
			RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
			RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
            return(LDT_ERR_ADR);
        }

		// in some cases we will want to retry the electronic payment attempt
		// for instance if a status of EEPT_FC2_OVERRIDE is returned by EPT.
		// we will loop back to do so.
	} while (1);

	RflMemRand (&CPRcv, sizeof(CPSend));     // clear the memory for PCI compliance.
	RflMemRand (&CPSend, sizeof(CPSend));    // clear the memory for PCI compliance.
    return(ITM_SUCCESS);
}


/*
*==========================================================================
**  Synopsis:   SHORT ItemTendEPTCommGiftCard(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender,
*                                     ITEMTENDER *ItemTender, USHORT usCashierID)
*   Input:      UCHAR        uchFuncCode
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_SEQERR_ADR  : sequence error
*               LDT_ERR_ADR     : error
*
*   Description:  We are reusing the EPT interface to do several of the Gift Card (Prepaid) operations
*				  other than a tender.  Since much of the information required is similar to a tender,
*				  we will just go ahead and use the tender data struct.
*==========================================================================
*/
SHORT   ItemTendEPTCommGiftCard (UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, ULONG ulCashierID)
{
	CONST SYSCONFIG *pSysConfig = PifSysConfig();
	int     iLoop;
	int     iEptDll;
	SHORT   sRetVal;
	USHORT  addr;
	PARATENDERKEYINFO TendKeyInfo;

	//searches through the tender numbers to find which one is a prepaid and uses that
	//tender number for minor class.
	for(addr = 1; addr <= STD_TENDER_MAX; addr++){
		memset(&TendKeyInfo, 0, sizeof(TendKeyInfo));
		TendKeyInfo.uchAddress = (UCHAR) addr;
		ParaTendInfoRead(&TendKeyInfo);

		if(UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY || UifRegTender->GiftCard == IS_GIFT_CARD_GENERIC) {
			if(TendKeyInfo.TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_PREPAID ){
				continue;
			}
		} else  if(UifRegTender->GiftCard == IS_GIFT_CARD_EPAYMENT) {
			if(TendKeyInfo.TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_EPD ){
				continue;
			}
		}

		// check to see if this tender key is assigned to a Prepaid transaction type.
		// if it is then we will determine if we should use this tender key information by
		// looking to see if the tender request is a FreedomPay request or just a standard
		// gift card request.
		// If the tender key is assigned to the FreedomPay action code and the tender request is a
		// FreedomPay tender, then we will use this tender key information to process
		// the tender request.
		// Otherwise, we will look for the first tender key that is setup to
		// process a prepaid (gift card) tender and use that.
		if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID ){
			iEptDll = TendKeyInfo.TenderKeyInfo.auchEptTend;
			if (iEptDll >= '0' && iEptDll <= '9') {
				iEptDll -= '0';  // change to binary value for index into pSysConfig->EPT_ENH_Info[]
			}
			else {
				iEptDll = 0;     // default is zero or the first pSysConfig->EPT_ENH_Info[] entry
			}

			if(UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY) {
				if (pSysConfig->EPT_ENH_Info[iEptDll].ulDevice == DEVICE_EPT_EX_DLL1) {
					ItemTender->uchMinorClass = (UCHAR) addr; 
					UifRegTender->uchMinorClass =(UCHAR) addr;
					break;
				}
			}
			else {
				ItemTender->uchMinorClass = (UCHAR) addr; 
				UifRegTender->uchMinorClass =(UCHAR) addr;
				break;
			}
		} else if (TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD) {
				ItemTender->uchMinorClass = (UCHAR) addr; 
				UifRegTender->uchMinorClass =(UCHAR) addr;
				break;
		}
	}

	// If we did not find a matching tender key for this tender key request,
	// then we will indicate a provisioning error so that someone can
	// make the necessary database changes.
	if (addr > STD_TENDER_MAX) {
		return LDT_EQUIPPROVERROR_ADR;
	}

	if(UifRegTender->GiftCardOperation == GIFT_CARD_VOID){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_VOID_ISSUE;
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_BALANCE){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_BALANCE;
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_RELOAD){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_RELOAD;  // for TENDERKEY_TRANTYPE_EPD reload is same as Payment
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_VOID_RELOAD){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_VOID_RELOAD;	// for TENDERKEY_TRANTYPE_EPD void reload is same as VoidPayment
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_ISSUE){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_ISSUE;
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_VOID){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_VOID_ISSUE;
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_LOOKUPNAMESS4){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4;	
	}
	else if(UifRegTender->GiftCardOperation == GIFT_CARD_LOOKUPEPDACCTNO){
		TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_EPD_LOOKUPEPDACCTNO;
	}
	else {
		return LDT_ERR_ADR;
	}

	if(UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY) {
		TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_FREEDOMPAY;
	}
	else if(UifRegTender->GiftCard == IS_GIFT_CARD_EPAYMENT) {
		BOOL  bCorrectTranCode;
		TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_EPD;
		bCorrectTranCode = (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_RELOAD);
		bCorrectTranCode = (bCorrectTranCode || (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_VOID_RELOAD));
		bCorrectTranCode = (bCorrectTranCode || (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_BALANCE));
		bCorrectTranCode = (bCorrectTranCode || (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4));
		if (! bCorrectTranCode) {
			return LDT_ERR_ADR;
		}
	}

	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
		// Not sure why this was done. Mercury Payments requires a Merchant Id field
		// and Steve Dudek of Crown Royal is depending on multiple people sharing the
		// same terminal yet having different Merchant accounts with Mercury Payment and
		// others.  Same change required several places.  Richard Chambers, Jul-21-2015
		// For gift card, we will just override the merchant id field since it is
		// not used with gift cards.
		for (iLoop = 0; iLoop < ARRAYSIZEOF(TendKeyInfo.TenderKeyInfo.tcMerchantID); iLoop++){
			TendKeyInfo.TenderKeyInfo.tcMerchantID[iLoop] = _T('0');
		}
	}

	sRetVal = ItemTendEPTCommDoComms(uchFuncCode, UifRegTender, ItemTender, &TendKeyInfo, ulCashierID);
	return sRetVal;
}

/*
*==========================================================================
**  Synopsis:   SHORT ItemTendEPTComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender,
*                                     ITEMTENDER *ItemTender, USHORT usCashierID)
*   Input:      UCHAR        uchFuncCode
*   Output:     nothing
*   InOut:      UIFREGTENDER *UifRegTender
*               ITEMTENDER   *ItemTender
*
*   Return:     ITM_SUCCESS     : successful
*               LDT_SEQERR_ADR  : sequence error
*               LDT_ERR_ADR     : error
*
*   Description: communicate EPT                                Saratoga
*==========================================================================
*/
SHORT   ItemTendEPTComm(UCHAR uchFuncCode, UIFREGTENDER *UifRegTender, ITEMTENDER *ItemTender, ULONG ulCashierID)
{
	PARATENDERKEYINFO TendKeyInfo = {0};
	SHORT returnType;
	//SHORT sFPresponse = 0;

	if (uchFuncCode == EEPT_FC2_ACTION_EPT) {
		// this is a EMV Parameter Download request so just set the transaction type and code.
		// we then need to turn the function code into the standard electronic payment type of function code.
		TendKeyInfo.TenderKeyInfo.usTranType = UifRegTender->GiftCard;            // Transaction Type (Credit, Debit, EBT, etc)
		TendKeyInfo.TenderKeyInfo.usTranCode = UifRegTender->GiftCardOperation;   // Transaction Code (Sale, Return, Void, etc)
		uchFuncCode = EEPT_FC2_EPT;   // ItemTendEPTComm() - inform ItemTendEPTCommDoComms() this is standard EPT.
	} else {
		// this is a standard electronic payment tender so use the tender key provisioning.
		TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
		TendKeyInfo.uchAddress = UifRegTender->uchMinorClass;	
		CliParaRead(&TendKeyInfo);
	}

	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID)
	{
		int iLoop;

		// Override the transaction code if it is a sale code and make it
		// a nonsufficient funds sale so that if the gift card has insufficient
		// funds to cover the tender, we can do a partial tender.
		if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_SALE) {
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_NONSFSALE;
		}

		UifRegTender->GiftCardOperation = GIFT_CARD_TENDER;

		if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
			// Not sure why this was done. Mercury Payments requires a Merchant Id field
			// and Steve Dudek of Crown Royal is depending on multiple people sharing the
			// same terminal yet having different Merchant accounts with Mercury Payment and
			// others.  Same change required several places.  Richard Chambers, Jul-21-2015
			// For gift card, we will just override the merchant id field since it is
			// not used with gift cards.
			for (iLoop = 0; iLoop < ARRAYSIZEOF(TendKeyInfo.TenderKeyInfo.tcMerchantID); iLoop++){
				TendKeyInfo.TenderKeyInfo.tcMerchantID[iLoop] = _T('0');
			}
		}

		if (UifRegTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY) 
		{
			// If this transaction is for some kind of a gift card analogue then we will
			// need to replace the standard tender destination with the appropriate
			// enhanced EPT information.
			CONST SYSCONFIG *pSysConfig = PifSysConfig();
			int iEptDll = TendKeyInfo.TenderKeyInfo.auchEptTend;

			if (iEptDll >= '0' && iEptDll <= '9') {
				iEptDll -= '0';  // change to binary value for index into pSysConfig->EPT_ENH_Info[]
			}
			else {
				iEptDll = 0;     // default is zero or the first pSysConfig->EPT_ENH_Info[] entry
			}
			if (pSysConfig->EPT_ENH_Info[iEptDll].ulDevice == DEVICE_EPT_EX_DLL1) {
				TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_SALE;
				TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_FREEDOMPAY;
			}
			else
				return LDT_EQUIPPROVERROR_ADR;
		}
	}

	returnType = ItemTendEPTCommDoComms(uchFuncCode, UifRegTender, ItemTender, &TendKeyInfo, ulCashierID);
	return returnType;
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendEPTEdit(UCHAR uchCPFlag, UCHAR uchFuncCode,
*                           EEPTREQDATA *ItemReqData, UIFREGTENDER *pUifTender,
*                           ITEMTENDER *ItemTender,LONG lChargeTips,
*                           UCHAR uchSeat, LONG lTax, USHORT usCashierID)
*
*   Input:      UCHAR           uchCPFlag
*               UCHAR           uchFuncCode
*               CPMRCREQDATA    *ItemReqData
*               ITEMTENDER      *ItemTender
*               UCHAR           uchTenderClass
*               LONG            lChargeTips
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     nothing
*
*   Description: edit room charge request data                  Saratoga
==========================================================================
*/
VOID    ItemTendEPTEdit(UCHAR uchCPFlag, UCHAR uchFuncCode, EEPTREQDATA *CPSend,
            UIFREGTENDER *pUifTender, ITEMTENDER *ItemTender, PARATENDERKEYINFO *pTendKeyInfo,
            LONG lChargeTips, UCHAR uchSeat, LONG lTax, ULONG ulCashierID)
{
    static  const TCHAR FARCONST    fmt01ld[] = _T("%01ld");
    static  const TCHAR FARCONST    fmt02ld[] = _T("%02ld");
    static  const TCHAR FARCONST    fmt02lx[] = _T("%02lx");
    static  const TCHAR FARCONST    fmt03ld[] = _T("%03ld");
    static  const TCHAR FARCONST    fmt04ld[] = _T("%04ld");
    static  const TCHAR FARCONST    fmt05ld[] = _T("%05ld");
    static  const TCHAR FARCONST    fmt06ld[] = _T("%06ld");
    static  const TCHAR FARCONST    fmt08ld[] = _T("%08ld");
    static  const TCHAR FARCONST    fmt010ld[] = _T("%010ld");

    TRANGCFQUAL     *WorkGCF = TrnGetGCFQualPtr();
    TRANITEMIZERS   *WorkTI = TrnGetTIPtr();
    DCURRENCY       lAmount, lDiscItemizers1, lDiscItemizers2,lTenderAmount;
    DCURRENCY       lChargeTipsforEdit;
	BOOL            bDataCapEpdBcServer;
    CHAR            chStrLen;
    UCHAR           i;
	UCHAR			asz2172[] = "2172", uchTmp;
    PARASTOREGNO    StoreReg;
    PARAHOTELID     Hotel;
    ITEMCOMMONTAX   WorkTax;
    ITEMCOMMONVAT   WorkVAT;
	PARAMDC			ParaMDC;
	TCHAR			versionNumber[40];

	// check if type for asz2172 array is probably same as CPSend->auchSystemNo
	NHPOS_ASSERT(sizeof(asz2172[0]) == sizeof(CPSend->auchSystemNo[0]));
	NHPOS_ASSERT(sizeof(asz2172) >= sizeof(CPSend->auchSystemNo));

    /* initialize buffer */
    memset(CPSend, ' ', sizeof(EEPTREQDATA));

    CPSend->uchSTX = EEPT_STX;                          /* STX Character, Start of Transmission used for old RS-232 protocol */

	bDataCapEpdBcServer = ((pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EPD) || (pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_EMEAL) || (pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_CHECK) || (pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_BATCH));

    ItemTendCPConvData(CPSend->auchDesTerminal, UCHARSIZEOF(CPSend->auchDesTerminal), fmt02ld, CliCpmGetAsMasUaddr());

    ItemTendCPConvData(CPSend->auchTenderID, UCHARSIZEOF(CPSend->auchTenderID), fmt02ld, ItemTender->uchMinorClass);  /* Tender ID */

    CPSend->auchFuncCode = uchFuncCode;                                    /* Function Code */

    memcpy (CPSend->aszCardHolder, ItemTender->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER], sizeof(CPSend->aszCardHolder));
    memcpy (CPSend->aszCardLabel, ItemTender->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], sizeof(CPSend->aszCardLabel));

	memcpy(CPSend->auchSystemNo, asz2172, sizeof(CPSend->auchSystemNo));   /* System # */
    StoreReg.uchMajorClass = CLASS_PARASTOREGNO;
    StoreReg.uchAddress = SREG_NO_ADR;
    CliParaRead(&StoreReg);
    ItemTendCPConvData(CPSend->auchStoreNo, UCHARSIZEOF(CPSend->auchStoreNo), fmt04ld, StoreReg.usStoreNo);

	// generate the pseudo ref number for Stored credt tender transactions for Amtrak
	{
		int    iLoop;

		if (TrnGetTransactionInvoiceNumTenderIfValid(ItemTender) != 3) {
			TrnGenerateTransactionInvoiceNumFromTender(0, ItemTender);
		}
		// for those cases where the user must enter a reference number do not overwrite the user entered text.
		if(pTendKeyInfo->TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_PREAUTHCAPTURE &&
			pTendKeyInfo->TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_VOID_SALE &&
			pTendKeyInfo->TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_VOID_RETURN) {
			TrnGetTransactionInvoiceNum (ItemTender->refno.auchReferncNo);
		}
		TrnGetTransactionInvoiceNum (ItemTender->invno.auchInvoiceNo);
		for (iLoop = 0 ; iLoop < NUM_REFERENCE; iLoop++) {
			CPSend->aszStoredRefNum[iLoop] = ItemTender->refno.auchReferncNo[iLoop];
		}
	}

	CPSend->auchSrcTerminal[0] = ' ';
	CPSend->auchSrcTerminal[1] = ' ';

	if (_ttoi(pTendKeyInfo->TenderKeyInfo.tcMerchantID) == 0) {
		// If the value is zero in the tender key provisioning then enforce proper number
		// of zeros in the merchange id field so that DSI Client will get its own value.
		for (i = 0; i < ARRAYSIZEOF(pTendKeyInfo->TenderKeyInfo.tcMerchantID); i++){
			pTendKeyInfo->TenderKeyInfo.tcMerchantID[i] = _T('0');
		}
	}

	if (bDataCapEpdBcServer) {   // EPD, E-Meal, and Bad Check Server will provide the Terminal Number and Store Number
		ItemTendCPConvData(CPSend->auchSrcTerminal, UCHARSIZEOF(CPSend->auchSrcTerminal), fmt02ld, CliReadUAddr());
		ItemTendCPConvData(CPSend->auchMerchantID, UCHARSIZEOF(CPSend->auchMerchantID), fmt08ld, StoreReg.usStoreNo);
	} else {
		for(i = 0; i < ARRAYSIZEOF(CPSend->auchMerchantID); i++) {
			CPSend->auchMerchantID[i] = (UCHAR)pTendKeyInfo->TenderKeyInfo.tcMerchantID[i];
		}
	}
	{
		UCHAR uchTempTerminalNo[2];
		ItemTendCPConvData(uchTempTerminalNo, UCHARSIZEOF(uchTempTerminalNo), fmt02lx, CliReadUAddr());
		CPSend->auchTerminalNo = uchTempTerminalNo[1];                   /* Terminal # */
	}

	//IP Address from the P98 Tender Key information
	ItemTendCPConvData(&CPSend->auchIpAddress[0][0], 3, fmt03ld, pTendKeyInfo->TenderKeyInfo.uchIPAddress[0]);
	ItemTendCPConvData(&CPSend->auchIpAddress[1][0], 3, fmt03ld, pTendKeyInfo->TenderKeyInfo.uchIPAddress[1]);
	ItemTendCPConvData(&CPSend->auchIpAddress[2][0], 3, fmt03ld, pTendKeyInfo->TenderKeyInfo.uchIPAddress[2]);
	ItemTendCPConvData(&CPSend->auchIpAddress[3][0], 3, fmt03ld, pTendKeyInfo->TenderKeyInfo.uchIPAddress[3]);

	//Ip Port from the P98 Tender Key information
	ItemTendCPConvData(CPSend->auchIpPort, UCHARSIZEOF(CPSend->auchIpPort), fmt05ld, pTendKeyInfo->TenderKeyInfo.usIPPortNo);

    Hotel.uchMajorClass = CLASS_PARAHOTELID;
    Hotel.uchAddress = HOTEL_ID_ADR;
    CliParaRead(&Hotel);
    if (Hotel.aszMnemonics[0] != _T('\0')) {
		NHPOS_ASSERT(Hotel.aszMnemonics[0] <= 0xFF);//MAXBYTE
        CPSend->auchHotelID[0] = (UCHAR)Hotel.aszMnemonics[0]; /* Hotel ID */
    }
    if (Hotel.aszMnemonics[1] != _T('\0')) {
		NHPOS_ASSERT(Hotel.aszMnemonics[1] <= 0xFF);//MAXBYTE
        CPSend->auchHotelID[1] = (UCHAR)Hotel.aszMnemonics[1];
    }

    if (uchSeat == 0 || uchSeat == 99 || TrnSplCheckSplit() == TRN_SPL_SEAT) {
        uchTmp = 0;
    } else if (uchSeat >= 1 && uchSeat <= NUM_SEAT) {
        uchTmp = uchSeat;
    } else {
        uchTmp = 99;
    }
    ItemTendCPConvData(CPSend->auchGuest, UCHARSIZEOF(CPSend->auchGuest), fmt06ld, uchTmp);
    ItemTendCPConvData(CPSend->auchGuest, UCHARSIZEOF(CPSend->auchGuest) - 2, fmt04ld, WorkGCF->usGuestNo);
	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_USCBP)) {
		// change for US Customs and Border Protection whose NETePay server software depends on
		// the operator id containing the following information in the following composition.
		// this is a holder over from the way US Customs was done for their version of NHPOS Rel 2.0.5.
		// it appears that DataCap wrote the US Customs NETePay server to pull the terminal id
		// from the first two digits of the operator id.
		memset (CPSend->auchCashierNo, 0, sizeof(CPSend->auchCashierNo));
		ItemTendCPConvData(CPSend->auchCashierNo, 2, fmt02ld, CliReadUAddr());
		ItemTendCPConvData(CPSend->auchCashierNo+2, 4, fmt04ld, (StoreReg.usStoreNo % 10000));
		ItemTendCPConvData(CPSend->auchCashierNo+6, 3, fmt03ld, (ulCashierID % 1000));
	} else {
		CPSend->auchCashierNo[0] = CPSend->auchCashierNo[1] = '0';
		ItemTendCPConvData(CPSend->auchCashierNo+2, 8, fmt08ld, ulCashierID);
	}

	// The foilo number may contain information for GIFT_CARD_LOOKUPNAMESS4 transaction code
	memset (CPSend->auchDriversLicence, 0, sizeof(CPSend->auchDriversLicence));
	for (i = 0; ItemTender->aszFolioNumber[i] && i < sizeof(CPSend->auchDriversLicence)/sizeof(CPSend->auchDriversLicence[0]); i++) {
		CPSend->auchDriversLicence[i] = ItemTender->aszFolioNumber[i];
	}

	// If this is a gift card type of tender then we will do an
	// override so that 
	if(pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID)
	{	
		uchCPFlag = EEPT_OVERTYP;
	}

	CPSend->uchType = pTendKeyInfo->TenderKeyInfo.usTranType;

	//Transaction Type
	ItemTendCPConvData(CPSend->auchTranType, UCHARSIZEOF(CPSend->auchTranType), fmt02ld, pTendKeyInfo->TenderKeyInfo.usTranType);

	//Transaction Code
	ItemTendCPConvData(CPSend->auchTranCode, UCHARSIZEOF(CPSend->auchTranCode), fmt02ld, pTendKeyInfo->TenderKeyInfo.usTranCode);

	ParaMDC.uchMajorClass = CLASS_PARAMDC;
	ParaMDC.usAddress = MDC_SYSTEM3_ADR;
	ParaMDCRead(&ParaMDC);

	if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)//2 decimal
	{
		CPSend->auchDecimal = '2';
	}
	else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)//3 decimal
	{
		CPSend->auchDecimal = '3';
	}
	else//no decimal
	{
		CPSend->auchDecimal = '0';
	}

	//Reference Number data
	if(*(ItemTender->refno.auchReferncNo) != '\0'){
		for(i = 0; i < UCHARSIZEOF(CPSend->auchRefNo); i++){
			CPSend->auchRefNo[i] = ItemTender->refno.auchReferncNo[i];
		}
	}
	if(*(ItemTender->invno.auchInvoiceNo) != '\0'){
		for(i = 0; i < NUM_INVOICE_NUM; i++){
			CPSend->auchInvoiceNo[i] = ItemTender->invno.auchInvoiceNo[i];
		}
	}
	//Authorization Code data
	if(*(ItemTender->authcode.auchAuthCode) != '\0'){
		for(i = 0; i < UCHARSIZEOF(CPSend->auchAuthCode); i++){
			CPSend->auchAuthCode[i] = ItemTender->authcode.auchAuthCode[i];
		}
	}

	//Routing Number
	if(*(ItemTender->tchRoutingNumber) != '\0'){
		for(i = 0; i < UCHARSIZEOF(CPSend->auchRoutingNumber); i++){
			NHPOS_ASSERT(ItemTender->tchRoutingNumber[i] <= 0xFF);//MAXBYTE
			CPSend->auchRoutingNumber[i] = (UCHAR)ItemTender->tchRoutingNumber[i];
		}
	}
	//Check Sequence Number
	if(*(ItemTender->tchCheckSeqNo) != '\0'){
		for(i = 0; i < UCHARSIZEOF(CPSend->auchCheckSeqNo); i++){
			NHPOS_ASSERT(ItemTender->tchCheckSeqNo[i] <= 0xFF);//MAXBYTE
			CPSend->auchCheckSeqNo[i] = (UCHAR)ItemTender->tchCheckSeqNo[i];
		}
	}
	//Acquirer Reference Number
	if(*(ItemTender->tchAcqRefData) != '\0'){
		for(i = 0; i < UCHARSIZEOF(CPSend->auchAcqRefData); i++){
			CPSend->auchAcqRefData[i] = ItemTender->tchAcqRefData[i];
		}
	}
	if(ItemTender->srno.auchRecordNumber[0] != '\0'){
		CPSend->srno = ItemTender->srno;
	}

    ItemTendCPConvData(CPSend->auchTransNo, UCHARSIZEOF(CPSend->auchTransNo), fmt04ld, MaintCurrentSpcCo(SPCO_CONSEC_ADR));

    CPSend->auchCPFlag = uchCPFlag;                     /* Charge Post Flag */

    Hotel.uchAddress = HOTEL_SLD_ADR;
    CliParaRead(&Hotel);
    memset(CPSend->auchSLDNumber, '0', sizeof(CPSend->auchSLDNumber));
    chStrLen = (CHAR)_tcslen(Hotel.aszMnemonics);        /* SLD # */
    for (i = UCHARSIZEOF(CPSend->auchSLDNumber) - 1; 1 <= chStrLen; i--, chStrLen--) {
		NHPOS_ASSERT(Hotel.aszMnemonics[chStrLen - 1] <= 0xFF);//MAXBYTE
        CPSend->auchSLDNumber[i] = (UCHAR)Hotel.aszMnemonics[chStrLen - 1];
    }

    if (*(ItemTender->auchMSRData) != '\0') {           /* MSR Data */
		RflDecryptByteString ((UCHAR *)&(ItemTender->auchMSRData[0]), sizeof(ItemTender->auchMSRData));
		for(i = 0; i < UCHARSIZEOF(CPSend->auchMSRData); i++){
			NHPOS_ASSERT(ItemTender->auchMSRData[i] <= 0xFF);//MAXBYTE
			CPSend->auchMSRData[i] = (UCHAR)ItemTender->auchMSRData[i];
		}
		RflEncryptByteString ((UCHAR *)&(ItemTender->auchMSRData[0]), sizeof(ItemTender->auchMSRData));
    }
    else if (*(ItemTender->aszNumber) != '\0') {           /* MSR Data */
		RflDecryptByteString ((UCHAR *)&(ItemTender->aszNumber[0]), sizeof(ItemTender->aszNumber));
		for(i = 0; i < NUM_NUMBER_EPT; i++){
			CPSend->auchAcctNo[i] = (UCHAR)ItemTender->aszNumber[i];
		}
		RflEncryptByteString ((UCHAR *)&(ItemTender->aszNumber[0]), sizeof(ItemTender->aszNumber));
		for (i = 0; i < NUM_NUMBER_EPT; i++) {		/* convert NULL of Account No. data to space */
			if (CPSend->auchAcctNo[i] == '\0') {
				CPSend->auchAcctNo[i] = ' ';
			}
		}
    }
    for (i = 0; i < NUM_MSRDATA; i++) {		/* convert NULL of MSR data to space */
		if (CPSend->auchMSRData[i] == '\0') {
			CPSend->auchMSRData[i] = ' ';
		}
	}
                                                        /* Expiration Date */
    /* set expiration date, 03/01/02 */
    if (*(ItemTender->auchExpiraDate) != '\0') {
		for(i = 0; i < UCHARSIZEOF(CPSend->auchExpDate); i++){
			NHPOS_ASSERT(ItemTender->auchExpiraDate[i] <= 0xFF);//MAXBYTE
			CPSend->auchExpDate[i] = (UCHAR)ItemTender->auchExpiraDate[i];
		}
    }

    if (ItemTender->uchCPLineNo) {
        ItemTendCPConvData(CPSend->auchGuestLineNo, UCHARSIZEOF(CPSend->auchGuestLineNo), fmt02ld, ItemTender->uchCPLineNo);
    }

    lAmount = 0L;
    lDiscItemizers1 = 0L;
    lDiscItemizers2 = 0L;
    lChargeTipsforEdit = 0L;

    if (!(ItemTenderLocal.fbTenderStatus[0] & TENDER_EC) && !(ItemTenderLocal.fbTenderStatus[0] & TENDER_CPPARTIAL)) {
		ITEMCOMMONLOCAL *CommonLocalData = ItemCommonGetLocalPointer();
		UCHAR  auchTotalStatus[NUM_TOTAL_STATUS];

        if (lChargeTips == 0L) {
            if (CommonLocalData->lChargeTip) {          /* charge tip exist */
                lChargeTipsforEdit = CommonLocalData->lChargeTip;
            }
        } else {
            lChargeTipsforEdit = lChargeTips;
        }
        ItemTendGetTotal(&auchTotalStatus[0], ItemTender->uchMinorClass);
        memset(&WorkTax, 0, sizeof(ITEMCOMMONTAX));
        memset(&WorkVAT, 0, sizeof(ITEMCOMMONVAT));

        if (ItemCommonTaxSystem() == ITM_TAX_CANADA) {
            ItemCanTax(&auchTotalStatus[1], &WorkTax, ITM_NOT_SPLIT);
            for(i = 0; i < 4; i++) {
                lAmount += WorkTax.alTax[i];
                lAmount += WorkTI->Itemizers.TranCanadaItemizers.lAffTax[i];
            }

        } else if (ItemCommonTaxSystem() == ITM_TAX_US) {

            if (ItemTender->uchMinorClass == CLASS_UITENDER11 &&
                CliParaMDCCheck(MDC_FS2_ADR, EVEN_MDC_BIT1)) {
                lAmount = 0L;

            } else {
#if 1
			    ItemTendAddTaxSum(&lAmount);
#else
                ItemCurTax(&auchTotalStatus[1], &WorkTax);
                for (i = 0; i < 3; i++) {
                    lAmount += WorkTax.alTax[i];
                    lAmount += WorkTI->Itemizers.TranUsItemizers.lTaxItemizer[i];
                }
#endif
            }

        } else {
            ItemCurVAT(&auchTotalStatus[1], &WorkVAT, ITM_NOT_SPLIT);
            lAmount += WorkVAT.lPayment;
        }

        lDiscItemizers1 = WorkTI->lDiscountable[0];
        lDiscItemizers2 = WorkTI->lDiscountable[1];
    }

	if (pTendKeyInfo->TenderKeyInfo.usTranType != TENDERKEY_TRANTYPE_CREDIT) {
		// If this is a FreedomPay transaction then we will just provide the tender total.
		// With other types of EPT such as Mercury and DataCap there is a field for the
		// gratuity.  FreedomPay has a simpler protocol and we only provide the tender amount.
		lChargeTipsforEdit = 0;
	} else if (pTendKeyInfo->TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_SALE && pTendKeyInfo->TenderKeyInfo.usTranCode != TENDERKEY_TRANCODE_PREAUTHCAPTURE) {
		// if this is some kind of a credit card return or void sale, then no separate charge tips
		lChargeTipsforEdit = 0;
	}

	CPSend->lTenderAmount = ItemTender->lTenderAmount;
	CPSend->lGratuity = lChargeTipsforEdit;          // unmodified ItemTender->lGratuity for reporting
	CPSend->ReturnsTenderChargeTips = ITEMTENDER_CHARGETIPS(ItemTender);

	if (lChargeTipsforEdit < 0) {
		lChargeTipsforEdit = 0;    // modify ItemTender->lGratuity amount if negative tip as for a return
	}

	ItemTendCPConvData(CPSend->auchChargeTip, UCHARSIZEOF(CPSend->auchChargeTip), fmt010ld, lChargeTipsforEdit);
	lTenderAmount = ItemTender->lTenderAmount - lChargeTipsforEdit;
	ItemTender->lGratuity = lChargeTipsforEdit;

	{
		DCURRENCY  lCashBackAmount = 0;

		if (pTendKeyInfo->TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_DEBIT) {
			// Implement Debit Cash Back business rules
			lCashBackAmount = ItemTender->lTenderAmount - ItemTender->lRound;
			if (lCashBackAmount < 0)
				lCashBackAmount = 0;
			lTenderAmount = lTenderAmount - lCashBackAmount;
		}
		ItemTendCPConvData(CPSend->auchDebitCashBack, UCHARSIZEOF(CPSend->auchDebitCashBack), fmt010ld, lCashBackAmount);

	}

    ItemTendCPConvData(CPSend->auchChargeTtl, UCHARSIZEOF(CPSend->auchChargeTtl), fmt010ld, lTenderAmount);

    if (uchSeat <= NUM_SEAT) { //SR206
        lAmount = lTax;
    }
    ItemTendCPConvData(CPSend->auchTtlTax, UCHARSIZEOF(CPSend->auchTtlTax), fmt010ld, lAmount);

	//Acount # is placed in the CPSend Structure right justified after zero filling
	//auchAcctNo the leading zeros may need to be removed by the function recieving
	//the CPSend Structure, the leading zeros have been left for now so any interfaces
	//that already depend on the zero filled right justified number
                                                        /* Account # */
	if (ItemTender->aszNumber[0] != 0) {
		memset(CPSend->auchAcctNo, '0', sizeof(CPSend->auchAcctNo));
		RflDecryptByteString((UCHAR *)&(ItemTender->aszNumber[0]), sizeof(ItemTender->aszNumber));
		if (ItemTender->aszNumber[0] == 'P' && ItemTender->aszNumber[1] == 'r') {
			CPSend->auchAcctNo[0] = 'P'; CPSend->auchAcctNo[1] = 'r';
		}
		else {
			chStrLen = (CHAR)_tcslen(ItemTender->aszNumber);
			for (i = UCHARSIZEOF(CPSend->auchAcctNo) - 1; 1 <= chStrLen; i--, chStrLen--) {
				CPSend->auchAcctNo[i] = (UCHAR)ItemTender->aszNumber[chStrLen - 1];
			}
		}
		RflEncryptByteString ((UCHAR *)&(ItemTender->aszNumber[0]), sizeof(ItemTender->aszNumber));
    }

    Hotel.uchAddress = ItemTender->uchMinorClass + (UCHAR)(HOTEL_TEND1_ADR - CLASS_UITENDER1);
    CliParaRead(&Hotel);
    CPSend->auchActionCode[0] = '0';
    CPSend->auchActionCode[1] = '0';
	NHPOS_ASSERT(Hotel.aszMnemonics[0] <= 0xFF);//MAXBYTE
    CPSend->auchActionCode[2] = (UCHAR)Hotel.aszMnemonics[0];  /* Action Code for EPT */

    ItemTendCPConvData(CPSend->auchDiscItemTtl1, UCHARSIZEOF(CPSend->auchDiscItemTtl1), fmt010ld, lDiscItemizers1);

    ItemTendCPConvData(CPSend->auchDiscItemTtl2, UCHARSIZEOF(CPSend->auchDiscItemTtl2), fmt010ld, lDiscItemizers2);

    /* --- Bonus Total #1-8 --- */
    ItemTendCPConvData(CPSend->auchBonus1, UCHARSIZEOF(CPSend->auchBonus1), fmt010ld, WorkTI->lHourlyBonus[0]);
    ItemTendCPConvData(CPSend->auchBonus2, UCHARSIZEOF(CPSend->auchBonus2), fmt010ld, WorkTI->lHourlyBonus[1]);
    ItemTendCPConvData(CPSend->auchBonus3, UCHARSIZEOF(CPSend->auchBonus3), fmt010ld, WorkTI->lHourlyBonus[2]);
    ItemTendCPConvData(CPSend->auchBonus4, UCHARSIZEOF(CPSend->auchBonus4), fmt010ld, WorkTI->lHourlyBonus[3]);
    ItemTendCPConvData(CPSend->auchBonus5, UCHARSIZEOF(CPSend->auchBonus5), fmt010ld, WorkTI->lHourlyBonus[4]);
    ItemTendCPConvData(CPSend->auchBonus6, UCHARSIZEOF(CPSend->auchBonus6), fmt010ld, WorkTI->lHourlyBonus[5]);
    ItemTendCPConvData(CPSend->auchBonus7, UCHARSIZEOF(CPSend->auchBonus7), fmt010ld, WorkTI->lHourlyBonus[6]);
    ItemTendCPConvData(CPSend->auchBonus8, UCHARSIZEOF(CPSend->auchBonus8), fmt010ld, WorkTI->lHourlyBonus[7]);

    if (ItemCommonTaxSystem() == ITM_TAX_US) {
	    ItemTendCPConvData(CPSend->auchFoodStamp, UCHARSIZEOF(CPSend->auchFoodStamp), fmt010ld, WorkTI->Itemizers.TranUsItemizers.lFoodStampable);
    }

    CPSend->uchETX = EEPT_ETX;                          /* ETX Character */
	
	//Version Number to be put into the XML message
	memset(versionNumber, 0x00, sizeof(versionNumber));
	PifGetVersion(versionNumber);
	memcpy(CPSend->auchVersionNo, versionNumber, sizeof(CPSend->auchVersionNo));
	CPSend->auchPaymentType = ItemTender->uchPaymentType;

	//Transaction tender unique id to identify specific tender within the transaction
	CPSend->usCheckTenderId = ItemTender->usCheckTenderId;
	CPSend->usReturnType = TrnQualModifierReturnTypeTest();

	memcpy (CPSend->uchUniqueIdentifier, WorkGCF->uchUniqueIdentifier, sizeof(CPSend->uchUniqueIdentifier));
	memcpy (CPSend->uchUniqueIdentifierReturn, WorkGCF->uchUniqueIdentifierReturn, sizeof(CPSend->uchUniqueIdentifierReturn));

	CPSend->uchSpecialProcessingFlags = 0;
	CPSend->uchSpecialProcessingFlags |= pUifTender->uchSpecialProcessingFlags;
	{
		// determine what other, customer specific flags need to be set for DSI Client functionality.
		const SYSCONFIG    *pSysConfig = PifSysConfig();
		ITEMSALESLOCAL     *pWorkSales = ItemSalesGetLocalPointer();

		if (pSysConfig->ulCustomerSettingsFlags & SYSCONFIG_CUSTOMER_ENABLE_AMTRAK)
			CPSend->uchSpecialProcessingFlags |= EEPTREQDATA_CUSTOMERFLAGS_ENABLE_AMTRAK;

		if (pSysConfig->ulCustomerSettingsFlags & SYSCONFIG_CUSTOMER_ENABLE_MERCURYPAYTEST)
			// NOTE: per conversation with Paul Stewart, Vantiv/WorldPay/Mercury this is no longer necessary for testing.
			//       Richard Chambers, May-23-2018
			CPSend->uchSpecialProcessingFlags |= EEPTREQDATA_SPECIALFLAGS_MERCURYPAYTEST;

		if (pSysConfig->ulCustomerSettingsFlags & SYSCONFIG_CUSTOMER_ENABLE_NO_PARTIAL_AUTH)
			CPSend->uchSpecialProcessingFlags |= EEPTREQDATA_SPECIALFLAGS_NO_PARTIAL_AUTH;

		if ((pWorkSales->fbSalesStatus & SALES_PREAUTHCAP_DECLN) != 0) {
			CPSend->uchSpecialProcessingFlags |= EEPTREQDATA_SPECIALFLAGS_AUTODECLINE;
		}

	}

    /* reset EC status */
    ItemTenderLocal.fbTenderStatus[0] &= ~TENDER_EC;
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCPFuncRsp1(UCHAR uchFCode,
*                           VOID *CPRcvData, SHORT sResponse)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Check Relation of Function and Response Status
*==========================================================================
*/
SHORT   ItemTendCPFuncRsp1(UCHAR uchFCode, EEPTRSPDATA *CPRcv, SHORT sResponse)
{

    /* --- Current Waiting Status --- */
    if (sResponse == EEPT_FC2_OVERRIDE) {
        if (CPRcv->auchEptStatus == EEPT_FC2_ACCEPTED ||
			CPRcv->auchEptStatus == EEPT_FC2_REJECTED ||
			CPRcv->auchEptStatus == EEPT_FC2_STORED) {
            return(ITM_SUCCESS);
        } else {
            return(LDT_ERR_ADR);
        }
    }

    /* --- Current Waiting Status --- */
    if (sResponse == EEPT_FC2_NEEDGLINE) {
        if (CPRcv->auchEptStatus == EEPT_FC2_ACCEPTED ||
			CPRcv->auchEptStatus == EEPT_FC2_STORED ||
            CPRcv->auchEptStatus == EEPT_FC2_REJECTED ||
            CPRcv->auchEptStatus == EEPT_FC2_OVERRIDE) {
            return(ITM_SUCCESS);
        } else {
            return(LDT_ERR_ADR);
        }
    }

    switch (uchFCode) {
    case    EEPT_FC2_EPT:
        if (CPRcv->auchEptStatus == EEPT_FC2_ACCEPTED ||
            CPRcv->auchEptStatus == EEPT_FC2_STORED ||
            CPRcv->auchEptStatus == EEPT_FC2_REJECTED ||
            CPRcv->auchEptStatus == EEPT_FC2_OVERRIDE ||
            CPRcv->auchEptStatus == EEPT_FC2_NEEDGLINE ||
            CPRcv->auchEptStatus == EEPT_FC2_DECLINED ||
            CPRcv->auchEptStatus == EEPT_FC2_ERROR ||
            CPRcv->auchEptStatus == EEPT_FC2_PARTTEND) {
            return(ITM_SUCCESS);
        }
        break;

    case    EEPT_FC2_ASK:
        if (CPRcv->auchEptStatus == EEPT_FC2_ACCEPTED ||
            CPRcv->auchEptStatus == EEPT_FC2_STORED ||
            CPRcv->auchEptStatus == EEPT_FC2_REJECTED) {
            return(ITM_SUCCESS);
        }
        break;

    default:
        break;
    }

    return(LDT_ERR_ADR);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendEEPTAccept(UCHAR uchFCode,
*                                       ITEMTENDER *pItem, EEPTRSPDATA *CPRcvData)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Accept Response from PC
*==========================================================================
*/
SHORT   ItemTendEEPTAccept(UCHAR uchFCode, ITEMTENDER *pItem, EEPTRSPDATA *CPRcv)
{
    SHORT           sCnt = 0;

    /* --- Set Folio#, Post# from Response Text --- */
     _RflStrncpyUchar2Tchar(pItem->aszFolioNumber, CPRcv->auchFolioNumber, sizeof(CPRcv->auchFolioNumber));
     _RflStrncpyUchar2Tchar(pItem->aszPostTransNo, CPRcv->auchPostTransNo, sizeof(CPRcv->auchPostTransNo));

    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_DISPTEXT) {
        _tcsncpy(pItem->aszCPMsgText[0], CPRcv->auchMsgText, NUM_LEADTHRU);
    }

    /* --- Save Response Text into Tmp#2 File --- */
    ItemTendCPSaveRsp(uchFCode, CPRcv, ITM_FILE_APPEND);

    /* --- Check Buffer is Full Used --- */
    if (ItemTenderLocal.uchBuffOff == 3) {
        pItem->uchBuffOff = 0;
    } else {
        ItemTenderLocal.uchBuffOff++;
        pItem->uchBuffOff = ItemTenderLocal.uchBuffOff;
    }

    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_PRINT) {
        pItem->uchTenderOff = 1;
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendEEPTAcceptReport(EEPTRSPDATA *CPRcvData,
*                           UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Accept Response from PC (Ask Function)
*==========================================================================
*/
SHORT   ItemTendEEPTAcceptReport(EEPTRSPDATA *CPRcv, UIFREGTENDER *pUifTender, ITEMTENDER *pItem)
{
    SHORT           sCnt = 0, sStatus = 0;

    /* --- Set Folio#, Post# from Response Text (Not Used with EPT) --- */
     _RflStrncpyUchar2Tchar(pItem->aszFolioNumber, CPRcv->auchFolioNumber, sizeof(CPRcv->auchFolioNumber));
     _RflStrncpyUchar2Tchar(pItem->aszPostTransNo, CPRcv->auchPostTransNo, sizeof(CPRcv->auchPostTransNo));

    /* --- Pre-Inquiry --- */
	sStatus = ItemCommonSetupTransEnviron ();
	if(sStatus != ITM_SUCCESS)
		return (sStatus);

    /* --- Print Accept Message in Text --- */
    if (CPRcv->auchMsgInstruction == EEPT_RES_INST_PRINT) {
        ItemTendCPPrintAccRej(CPRcv, 0);
    }

    return(ITM_SUCCESS);
}


/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCPOverride(UCHAR uchFCode, VOID *CPSendData,
*                           VOID *CPRcvData, UIFREGTENDER *pUifTender,
*                           ITEMTENDER *pItem, USHORT usEventSend)
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Override Response from PC
*==========================================================================
*/
SHORT   ItemTendCPOverride(UCHAR uchFCode, VOID *CPSendData, VOID *CPRcvData,
            UIFREGTENDER *pUifTender, ITEMTENDER *pItem, USHORT usEventSend)
{
    EEPTREQDATA     *CPSend = (EEPTREQDATA *)CPSendData;
    EEPTRSPDATA     *CPRcv = (EEPTRSPDATA *)CPRcvData;
    SHORT           sStatus;
    USHORT          usRet;
	PARATENDERKEYINFO TendKeyInfo = { 0 };

    /* --- Supervisor Intervention --- */
	if(! CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT1)) {
		if (ItemSPVInt(LDT_OVERINT_ADR) == UIF_CANCEL) {
			return(UIF_CANCEL);
		}
	}

	/* --- Generate Override Send Data --- */
	memset(CPSend, ' ', sizeof(EEPTREQDATA));

	TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
	TendKeyInfo.uchAddress = pUifTender->uchMinorClass;	
	CliParaRead(&TendKeyInfo);

	if(TendKeyInfo.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID)
	{
		int iLoop;

		if(pUifTender->GiftCardOperation == GIFT_CARD_VOID){
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_VOID_ISSUE;
		}
		else if(pUifTender->GiftCardOperation == GIFT_CARD_BALANCE){
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_BALANCE;
		}
		else if(pUifTender->GiftCardOperation == GIFT_CARD_RELOAD){
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_RELOAD;
		}
		else if(pUifTender->GiftCardOperation == GIFT_CARD_VOID_RELOAD){
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_VOID_RELOAD;	
		}
		else if(pUifTender->GiftCardOperation == GIFT_CARD_ISSUE){
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_ISSUE;
		}

		// Override the transaction code if it is a sale code and make it
		// a nonsufficient funds sale so that if the gift card has insufficient
		// funds to cover the tender, we can do a partial tender.
		if (TendKeyInfo.TenderKeyInfo.usTranCode == TENDERKEY_TRANCODE_SALE) {
			TendKeyInfo.TenderKeyInfo.usTranCode = TENDERKEY_TRANCODE_NONSFSALE;
		}

		if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_REL_21_STYLE)) {
			// Not sure why this was done. Mercury Payments requires a Merchant Id field
			// and Steve Dudek of Crown Royal is depending on multiple people sharing the
			// same terminal yet having different Merchant accounts with Mercury Payment and
			// others.  Same change required several places.  Richard Chambers, Jul-21-2015
			// For gift card, we will just override the merchant id field since it is
			// not used with gift cards.
			for (iLoop = 0; iLoop < ARRAYSIZEOF(TendKeyInfo.TenderKeyInfo.tcMerchantID); iLoop++){
				TendKeyInfo.TenderKeyInfo.tcMerchantID[iLoop] = _T('0');
			}
		}

		if (pUifTender->GiftCard == IS_GIFT_CARD_FREEDOMPAY)
		{
			// If this transaction is for some kind of a gift card analogue then we will
			// need to replace the standard tender destination with the appropriate
			// enhanced EPT information.
			CONST SYSCONFIG *pSysConfig = PifSysConfig();
			int iEptDll = TendKeyInfo.TenderKeyInfo.uchTend;

			if (iEptDll >= '0' && iEptDll <= '9') {
				iEptDll -= '0';  // change to binary value for index into pSysConfig->EPT_ENH_Info[]
			}
			else {
				iEptDll = 0;     // default is zero or the first pSysConfig->EPT_ENH_Info[] entry
			}
			if (pSysConfig->EPT_ENH_Info[iEptDll].ulDevice == DEVICE_EPT_EX_DLL1) {
				TendKeyInfo.TenderKeyInfo.usTranType = TENDERKEY_TRANTYPE_FREEDOMPAY;
			}
			else
				return LDT_EQUIPPROVERROR_ADR;
		}
	}

	ItemTendEPTEdit(EEPT_OVERTYP, uchFCode, CPSend, pUifTender, pItem, &TendKeyInfo, 0L, 0xff, 0L, TranModeQualPtr->ulCashierID);

	/* --- Display Communication Message --- */
	ItemTendCPCommDisp();

	//Give time for previous response message to complete
	//Removal will cause charge post busy when supervisor
	//intervention is not on
	PifSleep(2000);

	/* --- Send Request Data to PMS/EPT, R2.0 GCA --- */
	sStatus = XEEptBuildXMLAndExecuteTrans(CPSend, CPRcv);

	if (sStatus != EEPT_RET_SUCCESS) {
		usRet = CpmConvertError(sStatus);
		ItemTendCPPrintErr(usEventSend, usRet);
		return(usRet);
	}

    return(EEPT_RET_SUCCESS);
}


/*
*==========================================================================
**  Synopsis:   VOID    ItemTendCPDispRspText(VOID *CPRcvData)
*
*   Input:
*   Output:
*   InOut:
*   Return:
*   Description:    Display Error Message from Response Text
*==========================================================================
*/
VOID    ItemTendCPDispRspText(EEPTRSPDATA *pCPRcv, TCHAR   *aszNumber)
{
	REGDISPMSG       Disp = {0};

    Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    Disp.uchMinorClass = CLASS_REGDISP_CLEAR;
    Disp.fbSaveControl = DISP_SAVE_TYPE_4;
    _tcsncpy(Disp.aszMnemonic, pCPRcv->auchMsgText, PARA_LEADTHRU_LEN);

    for (;;) {
        PifBeep(UIE_ERR_TONE);
        flDispRegDescrControl |= COMPLSRY_CNTRL;
        DispWrite(&Disp);
        flDispRegDescrControl &= ~COMPLSRY_CNTRL;

		if(PifSysConfig()->uchKeyType == KEYBOARD_TOUCH)
		{
			int   iRet;
			int   i;
			TCHAR errorTitle[25] = { 0 };
			TCHAR szMnemoTemp[512] = { 0 };         //Size is: UIE_LCD_COLUMN (40) + 1 + sizeof(errorTitle)
			CHARDATA  Data = { 0 };

			_tcscpy(errorTitle, _T("Message EPT/CP "));
			for (i = 0;
				i < sizeof(pCPRcv->auchMsgText) / sizeof(pCPRcv->auchMsgText[0]) - 1 && i < sizeof(szMnemoTemp) / sizeof(szMnemoTemp[0]) - 40;
				i += 40) {
				_tcscat (szMnemoTemp, STD_DIALOG_NEWLINE);
				_tcsncat(szMnemoTemp, pCPRcv->auchMsgText + i, 40);
			}

			iRet = UieMessageBox(szMnemoTemp, errorTitle, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);

			//RJC TEMP  The CHARDATA sent depends on the keyboard page
			UieClearRingBuf();                      /* clear ring buffer       */
			Data.uchFlags = 0;
			if (iRet == IDABORT) {
				Data.uchASCII = 0xFF;
				Data.uchScan = 0x29;     // FSC_AUTO_SIGN_OUT key press
			}
			else {
				Data.uchASCII = 0x43;    //  FSC_CLEAR key press
				Data.uchScan = 0x00;
			}
			//Put button info into ring buffer
			UiePutKeyData(2,  &Data, NULL);
		}

        if (UifDiaClear() == UIF_SUCCESS) {
            ItemOtherClear();
            break;
        }
    }
}

/*
*==========================================================================
**  Synopsis:   VOID    ItemTendCPPrintRsp(ItemTender *pItem)
*   Input:
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*   Description:    Print Response Text     (Change R2.0GCA)
*==========================================================================
*/
VOID    ItemTendCPPrintRsp(ITEMTENDER *pItem)
{
    UCHAR   uchOffset;
    TRN_CPHEADER    FileHeader;
    TRN_CPRSPHEADER RspHeader;

    /* --- Get Current Response Position --- */
	ItemTendCPReadFileHeader (&FileHeader, hsItemCPHandle2);

    if (FileHeader.uchWriteCnt == 0) {
        return;
    }
    uchOffset = (UCHAR)((FileHeader.uchWriteCnt - 1) % RSP_SAVE_MAX);

    /* --- Set Print Status --- */
    //RPH 11-10-3 SR# 201
	ItemTendCPReadResponseHeader (uchOffset, &RspHeader, hsItemCPHandle2);

	{
		TCHAR  aszCardHolder[NUM_CPRSPTEXT];
		TCHAR  aszCardLabel[NUM_CPRSPTEXT];

		memcpy (aszCardHolder, pItem->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER], sizeof(pItem->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER]));
		memcpy (aszCardLabel, pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], sizeof(pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]));
		/* --- Print Response --- */
		memset(pItem->aszCPMsgText, 0, sizeof(pItem->aszCPMsgText));
		pItem->fsPrintStatus = RspHeader.usPrintStatus;
		pItem->uchCPLineNo   = RspHeader.uchPrintLine;
		pItem->uchPrintSelect= RspHeader.uchPrintSelect;

		ItemTendCPReadResponseMessage (uchOffset, pItem->aszCPMsgText, hsItemCPHandle2);
		memcpy (pItem->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER], aszCardHolder, sizeof(pItem->aszCPMsgText[NUM_CPRSPCO_CARDHOLDER]));
		memcpy (pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL], aszCardLabel, sizeof(pItem->aszCPMsgText[NUM_CPRSPCO_CARDLABEL]));
	}
}

/*
==========================================================================
**  Synopsis:   VOID    ItemTendGetEPTMDC(UCHAR uchClass, UCHAR *auchStatus)
*   Input:      UIFREGTENDER *  UifRegTender
*   Output:     ITEMTENDER *    ItemTender
*   InOut:      none
*   Return:
*   Description:    get tender MDC from MinorClass of UifRegTender
==========================================================================
*/
VOID    ItemTendGetEPTMDC(UCHAR uchClass, UCHAR *auchStatus)
{
    auchStatus[1] = ItemTenderCheckTenderMdc (uchClass, MDC_TENDER_PAGE_3, MDC_PARAM_NIBBLE_B);
}

// -------------------    Charge Post Receipt temporary File functions start  -----------------
/*
*==========================================================================
**    Synopsis: SHORT   ItemTendCPExpandFile(SHORT hsHandle, USHORT usSize)
*       Input:  SHORT   hsHandle  - file handle
*               USHORT  usSize - size of file to expand
*      Output:  Nothing
**  Return   :
**  Description:    Expand specified file size
*==========================================================================
*/
static SHORT   ItemTendCPExpandFile(SHORT hsHandle, USHORT usSize)
{
    ULONG   ulActualSize;
    SHORT   sReturn;

    sReturn = PifSeekFile(hsHandle, (ULONG)usSize, &ulActualSize);

    if (sReturn < 0 || (ULONG)usSize != ulActualSize) {
        return(ITM_ERROR);
    }

    /* --- store file size to created file --- */
    sReturn = PifSeekFile(hsHandle, 0UL, &ulActualSize);

    if (sReturn < 0 || ulActualSize != 0UL) {
        return(ITM_ERROR);
    }

    /* --- Write File Size --- */
    PifWriteFile(hsHandle, &usSize, sizeof(USHORT));

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT   ItemTendCPCreateFile(VOID)
*       Input:
*      Output:  Nothing
*       InOut:  Nothing
**  Return:
**  Description:    Create CP Temporary Files.
*==========================================================================
*/
static SHORT  ItemTendCPCreateFileHelper (TCHAR  * aszItemCPTmpFile, USHORT usSize)
{
    SHORT  hsFile;
	SHORT  sRet = ITM_ERROR;

    if ((hsFile = PifOpenFile(aszItemCPTmpFile, auchTEMP_NEW_FILE_READ_WRITE)) >= 0) {   /* saratoga */
		/* --- allocate file size to store specified no. of items --- */
		sRet = ItemTendCPExpandFile(hsFile, usSize);
		PifCloseFile(hsFile);
	} else {
		char xBuff[128] = {0};
		sprintf (xBuff, "==ERROR: ItemTendCPCreateFileHelper() file %S %d", aszItemCPTmpFile, hsFile);
		NHPOS_ASSERT_TEXT(hsFile >= 0, xBuff);
	}

	return sRet;
}

SHORT   ItemTendCPCreateFile(VOID)
{
	SHORT  sRet1, sRet2;

    PifDeleteFile(aszItemCPTmpFile1);
    PifDeleteFile(aszItemCPTmpFile2);

	sRet1 = ItemTendCPCreateFileHelper (aszItemCPTmpFile1, ITM_FILE1_SIZE);  // ItemTendCPCreateFile()
	sRet2 = ItemTendCPCreateFileHelper (aszItemCPTmpFile2, ITM_FILE2_SIZE);  // ItemTendCPCreateFile()

	if (sRet1 < 0 || sRet2 < 0) {
        PifDeleteFile(aszItemCPTmpFile1);
        PifDeleteFile(aszItemCPTmpFile2);
		return(ITM_ERROR);
	} else {
		return(ITM_SUCCESS);
	}
}


/*
*==========================================================================
**    Synopsis: SHORT   ItemTendCPOpenFile(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :
**  Description:    Open files and save file handle.
*==========================================================================
*/
SHORT   ItemTendCPOpenFile(VOID)
{
    if ((hsItemCPHandle1 = PifOpenFile(aszItemCPTmpFile1, auchTEMP_OLD_FILE_READ_WRITE)) < 0) { /* saratoga */
        return(LDT_PROHBT_ADR);
    }

    if ((hsItemCPHandle2 = PifOpenFile(aszItemCPTmpFile2, auchTEMP_OLD_FILE_READ_WRITE)) < 0) { /* saratoga */
        PifCloseFile(hsItemCPHandle1);
        hsItemCPHandle1 = -1;
        return(LDT_PROHBT_ADR);
    }

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: SHORT  ItemTendCPReadFile(ULONG ulOffset, VOID *pData,
*                                   ULONG ulSize, SHORT hsHandle, ULONG *pulActualBytesRead)
*       Input:  ULONG  ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to read data buffer.
*               ULONG  ulSize - Number of bytes to be Read.
*               SHORT   hsFileHandle - File Handle.
				ULONG *pulActualBytesRead - Number of bytes Read

*      Output:  Nothing
*       InOut:  Nothing

**  Return   :  ITM_NO_READ_SIZE
				ITM_ERROR
				ITM_SUCCESS

**  Description:    Read data from item, consoli., pst rec. file.

  11-10-3 RPH Changes for PifReadFile

  Return type Changed USHORT->SHORT
  Size Read to Success/Error Size returned in pulActualBytesRead
*==========================================================================
*/
SHORT ItemTendCPReadFileHeader (TRN_CPHEADER *pFileHeader, SHORT hsItemCPHandle)
{
	ULONG  ulActualBytesRead;

	return ItemTendCPReadFile (0, pFileHeader, sizeof(TRN_CPHEADER), hsItemCPHandle, &ulActualBytesRead);
}

VOID ItemTendCPWriteFileHeader (TRN_CPHEADER *pFileHeader, SHORT hsItemCPHandle)
{
	ItemTendCPWriteFile (0, pFileHeader, sizeof(TRN_CPHEADER), hsItemCPHandle);
}

SHORT ItemTendCPReadResponseHeader (USHORT usPos, TRN_CPRSPHEADER *pResponseHeader, SHORT hsItemCPHandle)
{
	EEPTRSPDATA  *CPRcv = 0;
	ULONG  ulActualBytesRead;
	ULONG  ulFilePos = sizeof(TRN_CPHEADER) + usPos * (sizeof(TRN_CPRSPHEADER) + sizeof(CPRcv->auchMsgText));

	return ItemTendCPReadFile (ulFilePos, pResponseHeader, sizeof(TRN_CPRSPHEADER), hsItemCPHandle, &ulActualBytesRead);
}

VOID ItemTendCPWriteResponseHeader (USHORT usPos, TRN_CPRSPHEADER *pResponseHeader, SHORT hsItemCPHandle)
{
	EEPTRSPDATA  *CPRcv = 0;
	ULONG  ulFilePos = sizeof(TRN_CPHEADER) + usPos * (sizeof(TRN_CPRSPHEADER) + sizeof(CPRcv->auchMsgText));

	ItemTendCPWriteFile (ulFilePos, pResponseHeader, sizeof(TRN_CPRSPHEADER), hsItemCPHandle);
}

SHORT ItemTendCPReadResponseMessage (USHORT usPos, WCHAR *pResponseMessage, SHORT hsItemCPHandle)
{
	EEPTRSPDATA  *CPRcv = 0;
	ULONG  ulActualBytesRead;
	ULONG  ulFilePos = sizeof(TRN_CPHEADER) + usPos * (sizeof(TRN_CPRSPHEADER) + sizeof(CPRcv->auchMsgText)) + sizeof(TRN_CPRSPHEADER);

	return ItemTendCPReadFile (ulFilePos, pResponseMessage, sizeof(CPRcv->auchMsgText), hsItemCPHandle, &ulActualBytesRead);
}

VOID ItemTendCPWriteResponseMessage (USHORT usPos, WCHAR *pResponseMessage, SHORT hsItemCPHandle)
{
	EEPTRSPDATA  *CPRcv = 0;
	ULONG  ulFilePos = sizeof(TRN_CPHEADER) + usPos * (sizeof(TRN_CPRSPHEADER) + sizeof(CPRcv->auchMsgText)) + sizeof(TRN_CPRSPHEADER);

	ItemTendCPWriteFile (ulFilePos, pResponseMessage, sizeof(CPRcv->auchMsgText), hsItemCPHandle);
}

SHORT  ItemTendCPReadFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsHandle,
						  ULONG *pulActualBytesRead)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;

    if (!ulSize) {          /* check read size */
        return(ITM_NO_READ_SIZE);          /* if read nothing, return */
    }

//    sReturn = PifSeekFile(hsHandle, 0UL, &ulActPos);
//    sReturn = PifReadFile(hsHandle, &usFSize, sizeof(USHORT), pulActualBytesRead);
    sReturn = PifSeekFile(hsHandle, ulOffset, &ulActPos);

	PifReadFile(hsHandle, pData, ulSize, pulActualBytesRead);
	if(*pulActualBytesRead != ulSize){
		return(ITM_ERROR);
	}

    return(ITM_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    ItemTendCPWriteFile(USHORT usOffset,
*                           VOID *pData, USHORT usSize, SHORT hsHandle)
*
*       Input:  ULONG  ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to write data buffer.
*               ULONG  ulSize - Number of bytes to be written.
*               SHORT   hsHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:    Write data to item, consoli. post rec. file.
*==========================================================================
*/
VOID    ItemTendCPWriteFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsHandle)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;
//	ULONG	ulActualBytesRead;  //11-10-3 RPH

    if (!ulSize) {                  /* check writed size */
        return;                     /* if nothing, return */
    }

//    sReturn = PifSeekFile(hsHandle, 0UL, &ulActPos);

	//RPH 11-10-3 Changes for PifReadFile
//    sReturn = PifReadFile(hsHandle, &usFSize, sizeof(USHORT), &ulActualBytesRead);
    sReturn = PifSeekFile(hsHandle, ulOffset, &ulActPos);

    PifWriteFile(hsHandle, pData, ulSize);
}

/*
*==========================================================================
**    Synopsis: VOID    ItemTendCPClearFile(VOID)
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:    Clear
				The size of uchTmp, ITM_FILE1_BASE bytes, is based on the sizes used
				for the defines of ITM_FILE1_SIZE and ITM_FILE2_SIZE which
				are multiples of ITM_FILE1_BASE bytes.
*==========================================================================
*/
VOID    ItemTendCPClearFile(VOID)
{
	UCHAR   uchTmp[ITM_FILE1_BASE] = {0};

	BlFwIfReadSignatureLineDisplay();  // If we have an Ingenico i6550 then turn it into a customer display

	if (hsItemCPHandle1 >= 0) {
		ULONG   ulPos;
		/* --- Clear Reg Tmp#1 --- */
		for (ulPos = 0; ulPos < ITM_FILE1_SIZE; ulPos += sizeof(uchTmp)) {
			ItemTendCPWriteFile(ulPos, uchTmp, sizeof(uchTmp), hsItemCPHandle1);
		}
	} else {
		PifLog (MODULE_TENDER, LOG_ERROR_STB_TEMP_FILE_HANDLE_BAD);
		PifLog (MODULE_DATA_VALUE(MODULE_TENDER), 1);
		PifLog (MODULE_LINE_NO(MODULE_TENDER), (USHORT)__LINE__);
	}

	if (hsItemCPHandle2 >= 0) {
		ULONG   ulPos;
		/* --- Clear Reg Tmp#2 --- */
		for (ulPos = 0; ulPos < ITM_FILE2_SIZE; ulPos += sizeof(uchTmp)) {
			ItemTendCPWriteFile(ulPos, uchTmp, sizeof(uchTmp), hsItemCPHandle2);
		}
	} else {
		PifLog (MODULE_TENDER, LOG_ERROR_STB_TEMP_FILE_HANDLE_BAD);
		PifLog (MODULE_DATA_VALUE(MODULE_TENDER), 2);
		PifLog (MODULE_LINE_NO(MODULE_TENDER), (USHORT)__LINE__);
	}
}

/*
*==========================================================================
**    Synopsis: VOID    ItemTendCPCloseFile(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
**  Return   :  Nothing
**  Description:    Close files.
*==========================================================================
*/
VOID    ItemTendCPCloseFile(VOID)
{
    PifCloseFile(hsItemCPHandle1);
	hsItemCPHandle1 = -1;
    PifCloseFile(hsItemCPHandle2);
	hsItemCPHandle2 = -1;
}



/****** End of Source ******/