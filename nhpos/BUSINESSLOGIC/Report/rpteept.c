#if defined(POSSIBLE_DEAD_CODE)

/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation                   **
**     @  *=*=*=*=*=*=*=*=*  0          Georgia Southern                  **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : EEPT Store and Forward Report Module                         
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTEEPT.C                           
* --------------------------------------------------------------------------
* Abstruct: 
*
* --------------------------------------------------------------------------
*  Update Histories
*    Date  : Ver.Rev. :   Name      : Description
* 05/21/15 : 02.02.01 : R.Chambers  : added logging of calls to RptForwardStoreForward()
*/
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <cscas.h>
#include <csstbcas.h>
#include <cswai.h>
#include <csstbwai.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <report.h>
#include <mld.h>
#include <mldsup.h>
#include <plu.h>
#include <csstbfcc.h>
#include <..\Mld\INCLUDE\mldlocal.h>

#include <mldmenu.h>

#include <csstbept.h>
#include "rptcom.h"
#include <uifsup.h>
#include <uifpgequ.h>
#include "..\Uifsup\uifsupex.h"
#include <pmg.h>
#include "..\Print\IncludeTherm\prtcom.h"
#include "EEPT.H"
#include "EEptTrans.h"
#include "cpm.h"

 static const TCHAR FARCONST auchMldRptSupRegFin1[] = _T("%s");  
 static const TCHAR FARCONST auchMldRptSupRegFin2[] = _T("%s\t%d"); 
 static const TCHAR FARCONST auchMldRptSupRegSF3[] = _T("%s\t%s"); 
 static const TCHAR FARCONST auchMldRptSupRegFin4[] = _T("%s\t%8.8Mu");

 CONST TCHAR FARCONST  ReportAmount[] = _T("%s\t$%3l$");
 CONST TCHAR FARCONST  ReportAmount2[] = _T("%s\t$%3.2l$");
 CONST TCHAR FARCONST  ReportAmount3[] = _T("%s\t$%3.3l$");

CONST TCHAR FARCONST auchDateTime3[] =_T("%2u:%02u%s %02u/%02u/%04u");
CONST TCHAR FARCONST auchDateTime4[] =_T("%2u:%02u %02u/%02u/%04u");

static 	TCHAR  *aszMnemonicFailed = _T("Error Text:");
static 	TCHAR  *aszMnemonicTransaction =  _T("Transaction #:");
static 	TCHAR  *aszMnemonicCluster = _T("Store #:");
static 	TCHAR  *aszMnemonicRegister = _T("Register #:");
static 	TCHAR  *aszMnemonicCashier = _T("Cashier #:");

static  TCHAR  *aszMnemonicFailedError = _T("Failed - Error");
static  TCHAR  *aszMnemonicFailedDeclined = _T("Failed - Decline");

static VOID PrtStoreDetailedReport(VOID* pItem);
static VOID PrtStoreSummaryReport(VOID* pItem);
static VOID PrtStoreFailedReport(VOID* pItem);

typedef struct  {
	USHORT    usTranCode;
	TCHAR    *tcsLabel;
} RptStoreTranType;

static RptStoreTranType myTranTypeList[] = {
	{TENDERKEY_TRANCODE_SALE, _T("SALE")},
     {TENDERKEY_TRANCODE_RETURN, _T("RETURN")},
     {TENDERKEY_TRANCODE_VOID_SALE, _T("VOID_SALE")},
     {TENDERKEY_TRANCODE_VOID_RETURN, _T("VOID_RETURN")},
     {TENDERKEY_TRANCODE_ADJUST, _T("ADJUST")},
     {TENDERKEY_TRANCODE_VOUCHER, _T("VOUCHER")},
     {TENDERKEY_TRANCODE_CASHSALE, _T("CASHSALE")},
     {TENDERKEY_TRANCODE_CASHRETURN, _T("CASHRETURN")},
     {TENDERKEY_TRANCODE_FOODSTAMPSALE, _T("FOODSTAMPSALE")},
     {TENDERKEY_TRANCODE_FOODSTAMPRETURN, _T("FOODSTAMPRETURN")},
     {TENDERKEY_TRANCODE_DRIVERLICENSE_MANUAL, _T("")},
     {TENDERKEY_TRANCODE_DRIVERLICENSE_STRIPE, _T("")},
     {TENDERKEY_TRANCODE_MICR_READER, _T("MICR_READER")},
     {TENDERKEY_TRANCODE_MICR_MANUAL, _T("MICR_MANUAL")},
     {TENDERKEY_TRANCODE_PREAUTH, _T("PREAUTH")},
     {TENDERKEY_TRANCODE_PREAUTHCAPTURE, _T("FINALIZED")},
     {TENDERKEY_TRANCODE_ISSUE, _T("ISSUE")},
     {TENDERKEY_TRANCODE_RELOAD, _T("RELOAD")},
     {TENDERKEY_TRANCODE_VOID_RELOAD, _T("VOID_RELOAD")},
     {TENDERKEY_TRANCODE_BALANCE, _T("BALANCE")},
     {TENDERKEY_TRANCODE_NONSFSALE, _T("NONSFSALE")},
     {TENDERKEY_TRANCODE_VOID_ISSUE, _T("VOID_ISSUE")},
     {TENDERKEY_TRANCODE_EPD_PAYMENT, _T("EPD_PAYMENT")},
     {TENDERKEY_TRANCODE_EPD_VOIDPAYMENT, _T("EPD_VOIDPAYMENT")},
     {TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4, _T("EPD_LOOKUPNAMESS4")},
     {TENDERKEY_TRANCODE_EPD_LOOKUPEPDACCTNO, _T("EPD_LOOKUPEPDACCTNO")},
     {TENDERKEY_TRANCODE_BATCH_ITEMDETAIL, _T("BATCH_ITEMDETAIL")},
     {TENDERKEY_TRANCODE_BATCH_SUMMARY, _T("BATCH_SUMMARY")},
	 {0, 0}
};

static USHORT RptStoreNumberFromBlock (UCHAR  *auchStoreNo)
{
	USHORT  usStoreNumber = 0;

	usStoreNumber  = (auchStoreNo[0] & 0x000f); usStoreNumber *= 10;
	usStoreNumber += (auchStoreNo[1] & 0x000f); usStoreNumber *= 10;
	usStoreNumber += (auchStoreNo[2] & 0x000f); usStoreNumber *= 10;
	usStoreNumber += (auchStoreNo[3] & 0x000f);

	return usStoreNumber;
}

static TCHAR *RptStoreTranTypeLookup (USHORT usTranCode)
{
	int i;

	for (i = 0; myTranTypeList[i].tcsLabel; i++) {
		if (myTranTypeList[i].usTranCode == usTranCode) {
			return myTranTypeList[i].tcsLabel;
		}
	}
	return _T("");
}

SHORT RptStoreForwardRead(LONG lUieAc102_SubMenu, UCHAR uchMinorClass, SHORT uchType)
{
	EEPTF_HEADER  StoreForwardData;
	SHORT         sRet;
    UCHAR         uchSpecMnemo, uchRptType, uchACNo, uchRptName;
    //SHORT       sReturn;
   // DATE_TIME   DateTime;
    TTLREGFIN     TtlData;
	SHORT         sError;

	memset(&StoreForwardData, 0x00, sizeof(StoreForwardData));

	RptPrtStatusSet(uchMinorClass); 

	if((sRet = CliStoreForwardRead(lUieAc102_SubMenu, &StoreForwardData)) != SUCCESS){
		// error
	}
	if((StoreForwardData.ulTransStored == 0) && (uchType == RPT_DETAILED)){
		return LDT_FILE_EMPTY;
	}
	else if((uchType == RPT_FAILED)){
		if (CliParaMDCCheckField(MDC_SF_EXEC_RULES_01, MDC_PARAM_BIT_D)) {
			// this indicates that we are ignoring declines, ITMSTOREFORWARD_FLAG_IGNORE_DECLINES set
			// we will check whether ulErrorTrans indicates any Declined or not.
			if (StoreForwardData.ulTransStored == 0 && StoreForwardData.ulFailedTrans == 0 && StoreForwardData.ulErrorTrans == 0 && StoreForwardData.ulHardErrorTrans == 0) {
				return LDT_FILE_EMPTY;
			}
		} else if (StoreForwardData.ulTransStored == 0) {
			return LDT_FILE_EMPTY;
		}
	}

	memset(&TtlData, 0x00, sizeof(TtlData));
	StoreForwardData.uchMajorClass = CLASS_RPTSTOREFORWARD;
	StoreForwardData.uchMinorClass = (UCHAR) uchType;
	StoreForwardData.lUieAc102_SubMenu |= (lUieAc102_SubMenu & EEPT_SUBMENU_NONSTATUSMASK);
	
	uchRptName = RPT_FINANC_ADR;            /* Set financial address */
    uchACNo = AC_STORE_FORWARD;           /* Set financial A/C No. */

	uchSpecMnemo = SPC_DAIRST_ADR;                      /* Set Daily Special Mnemonics */
    uchRptType = RPT_READ_ADR;

	MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Increment Consecutive Counter */
	MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    0,										/* Report Name Address */
                    0,										/* Special Mnemonics Address */
                    RPT_READ_ADR,                           /* Report Name Address */
                    0,                                      /* Not Print */
                    AC_STORE_FORWARD,                       /* A/C Number */
                    RPT_TYPE_IND,                           /* Reset Type */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */

	if ((sError = RptStrFrwdEdit(&StoreForwardData, uchType)) != SUCCESS) {
        RptPrtError(sError);
    }

	//UieExit(aACEnter);

	MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);

}

#if defined(RptForwardStoreForward)
SHORT RptForwardStoreForward_Special(LONG lUieAc102_SubMenu);
SHORT RptForwardStoreForward_Debug (LONG lUieAc102_SubMenu, char *aszPath, int nLineNo)
{
	char xBuff[128];
	int  iLen = strlen(aszPath);

	if (iLen > 20) iLen -= 20; else iLen = 0;

	sprintf (xBuff, "==STATE: RptForwardStoreForward_Debug %d 0x%x  %s  %d", lUieAc102_SubMenu, lUieAc102_SubMenu, aszPath  + iLen, nLineNo);
	NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	return RptForwardStoreForward_Special (lUieAc102_SubMenu);
}

SHORT RptForwardStoreForward_Special(LONG lUieAc102_SubMenu)
#else
SHORT RptForwardStoreForward(LONG lUieAc102_SubMenu)
#endif
{
	SHORT sRet;

	if((sRet = CliForwardStoreForward(lUieAc102_SubMenu)) != SUCCESS){
		// error
		char  xBuff[128];
		sprintf (xBuff, "==ERROR: CliForwardStoreForward() return %d.", sRet);
		NHPOS_ASSERT_TEXT((sRet == SUCCESS), xBuff);
	}
	return 0;
}

SHORT RptToggleStoreForward(SHORT onOff)
{
	SHORT sRet;
	if((sRet = CliToggleStoreForward(onOff)) != SUCCESS){
		// error
	}
	return 0;
}

SHORT RptStrFrwdEdit(VOID *StoreForwardData, SHORT uchType){
	
	EEPTF_HEADER* StoreForward= (EEPTF_HEADER*) StoreForwardData;;
	
	if (RptCheckReportOnMld()) {
        uchRptMldAbortStatus = (UCHAR)MldDispItem(StoreForward, uchType); /* display on LCD          */ 
		return 0;
    }
    PrtPrintItem(NULL, StoreForward);
	return 0;
}


VOID MldStoreForwardDisp( VOID* pItem, USHORT usStatus){

	if(usStatus == RPT_SUMMARY)
		MldSummaryDisp( pItem, usStatus);
	else if(usStatus == RPT_DETAILED)
		MldDetailedDisp( pItem, usStatus);
	else if(usStatus == RPT_FAILED)
		MldFailedDisp(pItem, usStatus);

}

static void MldPutLineToDisplay (TCHAR  *pszString, USHORT usRow)
{
	MldScroll1Buff.uchCurItem += (UCHAR)usRow;

    while (usRow--) {
        MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
        pszString += (MLD_SUPER_MODE_CLM_LEN+1);
    }
}

VOID MldSummaryDisp( VOID* pItem, USHORT usStatus){
	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
	TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
    USHORT usRow=0;
	PARAMDC		ParaMDC;

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin1, _T("Store And Forward Report:"));       
	MldPutLineToDisplay (aszString, usRow);

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, _T("Transactions Stored:"), StoreForwardData->ulTransStored);
	MldPutLineToDisplay (aszString, usRow);

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, _T("Transactions Approved:"), StoreForwardData->ulApproved);
	MldPutLineToDisplay (aszString, usRow);

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, _T("Transactions Declined:"), StoreForwardData->ulFailedTrans);
	MldPutLineToDisplay (aszString, usRow);

	//total stored
	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, _T("Transactions Error:"), StoreForwardData->ulErrorTrans);       
	MldPutLineToDisplay (aszString, usRow);

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, _T("Transactions Hard Error:"), StoreForwardData->ulHardErrorTrans);
	MldPutLineToDisplay (aszString, usRow);

	ParaMDC.uchMajorClass = CLASS_PARAMDC;
	ParaMDC.usAddress = MDC_SYSTEM3_ADR;
	ParaMDCRead(&ParaMDC);
	if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
	{
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount2, _T("Total Amount Stored:"), StoreForwardData->lTotalPending);       
		MldPutLineToDisplay (aszString, usRow);
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount2, _T("Total Void Stored:"), StoreForwardData->lTotalPendingVoid);       
		MldPutLineToDisplay (aszString, usRow);      
	}
	else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
	{
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount3, _T("Total Amount Stored:"), StoreForwardData->lTotalPending);       
		MldPutLineToDisplay (aszString, usRow);
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount3, _T("Total Void Stored:"), StoreForwardData->lTotalPendingVoid);       
		MldPutLineToDisplay (aszString, usRow);      
	}
	else{
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount, _T("Total Amount Stored:"), StoreForwardData->lTotalPending);       
		MldPutLineToDisplay (aszString, usRow);
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount, _T("Total Void Stored:"), StoreForwardData->lTotalPendingVoid);       
		MldPutLineToDisplay (aszString, usRow);    
	}

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin1, _T("First Transaction Stored:"));       
	MldPutLineToDisplay (aszString, usRow);

	/* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
        /* check date and print line */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
            /* TIME DD/MM/YY   */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
                      StoreForwardData->fromTime.usHour,           
					  StoreForwardData->fromTime.usMin,
					  StoreForwardData->fromTime.usMDay,
					  StoreForwardData->fromTime.usMonth,
					  StoreForwardData->fromTime.usYear);           
        } else {                                                 /* MM/DD/YY */
            /* TIME  MM/DD/YY  */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
                      StoreForwardData->fromTime.usHour,           
					  StoreForwardData->fromTime.usMin,
					  StoreForwardData->fromTime.usMonth,
					  StoreForwardData->fromTime.usMDay,
					  StoreForwardData->fromTime.usYear); 
        } 
	} else {
		/* check if Hour is '0' */
		if (StoreForwardData->fromTime.usHour == 0) {   /* in case of "0:00 AM" */
			usHour = 12;
		} else {
			usHour = StoreForwardData->fromTime.usHour;
		}
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
			//TIME(AM/PM) DD/MM/YY 
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
						  (usHour > 12)? usHour - 12 : usHour,
						  StoreForwardData->fromTime.usMin,
						  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
						  StoreForwardData->fromTime.usMDay,
						  StoreForwardData->fromTime.usMonth,
						  StoreForwardData->fromTime.usYear); 
		} else {     
			 //TIME(AM/PM) MM/DD/YY 
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
						  (usHour > 12)? usHour - 12 : usHour,
						  StoreForwardData->fromTime.usMin,
						  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
						  StoreForwardData->fromTime.usMonth,
						  StoreForwardData->fromTime.usMDay,
						  StoreForwardData->fromTime.usYear);       
		}
	}

	MldPutLineToDisplay (aszString, usRow);

	usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin1, _T("Last Transaction Stored:"));       
	MldPutLineToDisplay (aszString, usRow);

	/* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
        /* check date and print line */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
            /* TIME DD/MM/YY   */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
                      StoreForwardData->toTime.usHour,           
					  StoreForwardData->toTime.usMin,
					  StoreForwardData->toTime.usMDay,
					  StoreForwardData->toTime.usMonth,
					  StoreForwardData->toTime.usYear);           
        } else {                                                 /* MM/DD/YY */
            /* TIME  MM/DD/YY  */
            usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
                      StoreForwardData->toTime.usHour,           
					  StoreForwardData->toTime.usMin,
					  StoreForwardData->toTime.usMonth,
					  StoreForwardData->toTime.usMDay,
					  StoreForwardData->toTime.usYear); 
        } 
	} else {
		/* check if Hour is '0' */
		if (StoreForwardData->fromTime.usHour == 0) {   /* in case of "0:00 AM" */
			usHour = 12;
		} else {
			usHour = StoreForwardData->fromTime.usHour;
		}
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
			//TIME(AM/PM) DD/MM/YY 
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
						  (usHour > 12)? usHour - 12 : usHour,
						  StoreForwardData->fromTime.usMin,
						  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
						  StoreForwardData->toTime.usMDay,
						  StoreForwardData->toTime.usMonth,
						  StoreForwardData->toTime.usYear); 
		} else {     
			 //TIME(AM/PM) MM/DD/YY 
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
						  (usHour > 12)? usHour - 12 : usHour,
						  StoreForwardData->fromTime.usMin,
						  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
						  StoreForwardData->toTime.usMonth,
						  StoreForwardData->toTime.usMDay,
						  StoreForwardData->toTime.usYear);       
		}
	}      

	MldPutLineToDisplay (aszString, usRow);

	if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
        //return(RPT_ABORTED);
    } 
    if(RptPauseCheck() == RPT_ABORTED){
        // return(RPT_ABORTED);
    }

    RptFeed(RPT_DEFALTFEED);  
}

VOID MldDetailedDisp( VOID* pItem, USHORT usStatus){

	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
	TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
    USHORT usRow=0;
	PARATERMINALINFO TermInfo;
	EEPT_BLOCK_HEADER blockHeader;
	LONG lBlockSize = 0;
	TCHAR AcctNo[22];
	TCHAR processedMessage [48];
	PARAMDC		ParaMDC;

	memset(processedMessage, 0, sizeof(processedMessage));
	memset(&AcctNo, 0, sizeof(AcctNo));
	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);
	
	memset (&blockHeader, 0, sizeof(EEPT_BLOCK_HEADER));        // initialize for the iteration through the file.
	while((lBlockSize = CliGetNextBlockIterate(StoreForwardData->lUieAc102_SubMenu, &blockHeader)) > 0) {
		USHORT  usStoreNumber = TermInfo.TerminalInfo.usStoreNumber;

		memset(&AcctNo, 0, sizeof(AcctNo));
		memset(processedMessage, 0, sizeof(processedMessage));
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicTransaction, blockHeader.usConsNo);

		MldScroll1Buff.uchCurItem += (UCHAR)usRow;
		pszString = &aszString[0];
		while (usRow--) {
			/* display format data */
			MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
			pszString += (MLD_SUPER_MODE_CLM_LEN+1);
		}

		if (blockHeader.auchStoreNo) {
			usStoreNumber = RptStoreNumberFromBlock (blockHeader.auchStoreNo);
		}

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicCluster, usStoreNumber);
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicRegister, TermInfo.TerminalInfo.usRegisterNumber);       
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin4, aszMnemonicCashier, blockHeader.ulCashierID);       
		MldPutLineToDisplay (aszString, usRow);


		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, _T("Auth Type:"), RptStoreTranTypeLookup(blockHeader.usTranCode));       
		MldPutLineToDisplay (aszString, usRow);

		ParaMDC.uchMajorClass = CLASS_PARAMDC;
		ParaMDC.usAddress = MDC_SYSTEM3_ADR;
		ParaMDCRead(&ParaMDC);
		{
			int      iLoop;
			TCHAR    *tcsTransactionAmount = _T("Transaction Amount:");
			LONG     lValue = blockHeader.lChargeTtl;

			for (iLoop = 0; iLoop < 2; iLoop++) {
				// loop twice, once for transaction amount and once for tips amount if there are tips
				if (blockHeader.usEeptreqdata_Flag != 0) {  // is this a void transaction
					lValue *= -1;
				}
				if (lValue) {
					if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
					{
						// no decimal point in currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount2, tcsTransactionAmount, lValue);       
					}
					else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
					{
						// 2 decimal point currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount3, tcsTransactionAmount, lValue);    
					}
					else{
						// 3 decimal point currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount, tcsTransactionAmount, lValue);    
					}
					MldPutLineToDisplay (aszString, usRow);
				}
				if (blockHeader.lChargeTip) {
					tcsTransactionAmount = _T("Charge Tips Amount:");
					lValue = blockHeader.lChargeTip;
				} else {
					break;
				}
			}
		}

		MaskAcctNo(blockHeader.AcctNo, AcctNo);
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, _T("Account Number:"), AcctNo ); //     
		MldPutLineToDisplay (aszString, usRow);

		if(blockHeader.sProcessed == PROCESSED_ERROR){
			memcpy(processedMessage, _T("Failed - Error"), sizeof(_T("Failed - Error")));
		}
		else if(blockHeader.sProcessed == PROCESSED_DECLINED){
			memcpy(processedMessage, _T("Failed - Decline"), sizeof(_T("Failed - Decline")));
		}
		else if(blockHeader.sProcessed == PROCESSED)
		{
			memcpy(processedMessage, _T("Approved"), sizeof(_T("Approved")));
		}
		else
			memcpy(processedMessage, _T("Stored"), sizeof(_T("Stored")));

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, _T("Processed:"), processedMessage);       
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin1, _T("Transaction Time:"));       
		MldPutLineToDisplay (aszString, usRow);

		/* check time */
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
			/* check date and print line */
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				/* TIME DD/MM/YY   */
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usYear);           
			} else {                                                 /* MM/DD/YY */
				/* TIME  MM/DD/YY  */
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usYear); 
			} 
		} else {
			/* check if Hour is '0' */
			if (blockHeader.DateTime.usHour == 0) {   /* in case of "0:00 AM" */
				usHour = 12;
			} else {
				usHour = blockHeader.DateTime.usHour;
			}
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				//TIME(AM/PM) DD/MM/YY 
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usYear); 
			} else {     
				 //TIME(AM/PM) MM/DD/YY 
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usYear);       
			}
		}           

		MldPutLineToDisplay (aszString, usRow);

		if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
			//return(RPT_ABORTED);
		} 
		if(RptPauseCheck() == RPT_ABORTED){
			// return(RPT_ABORTED);
		}
		RptFeed(RPT_DEFALTFEED);       
	}
}

VOID MldFailedDisp( VOID* pItem, USHORT usStatus){
	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
	TCHAR  aszString[2 * (MLD_SUPER_MODE_CLM_LEN+1)];           /* buffer for formatted data */
    TCHAR  *pszString;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
    USHORT usRow=0;
	SHORT	i;
	PARATERMINALINFO TermInfo;
	EEPT_BLOCK_HEADER blockHeader;
	LONG lBlockSize = 0;
	TCHAR AcctNo[22];
	TCHAR tErrorMsg[20];
	USHORT          usRet;
    PARALEADTHRU  Lead;
	PARAMDC		ParaMDC;

	i = 0;
    memset(&Lead, 0, sizeof(Lead));
	memset(&AcctNo, 0, sizeof(AcctNo));
	memset(&tErrorMsg, 0, sizeof(tErrorMsg));
	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

	ParaTerminalInfoParaRead (&TermInfo);
	
	memset (&blockHeader, 0, sizeof(EEPT_BLOCK_HEADER));        // initialize for the iteration through the file.
	while((lBlockSize = CliGetNextBlockIterateAll(StoreForwardData->lUieAc102_SubMenu, &blockHeader)) > 0){
		USHORT  usStoreNumber = TermInfo.TerminalInfo.usStoreNumber;

		if( (blockHeader.sProcessed != PROCESSED_DECLINED) && (blockHeader.sProcessed != PROCESSED_ERROR)){
			continue;
		}

		memset(&AcctNo, 0, sizeof(AcctNo));
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicTransaction, blockHeader.usConsNo);
		MldPutLineToDisplay (aszString, usRow);

		if (blockHeader.auchStoreNo) {
			usStoreNumber = RptStoreNumberFromBlock (blockHeader.auchStoreNo);
		}

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicCluster, usStoreNumber);
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin2, aszMnemonicRegister, TermInfo.TerminalInfo.usRegisterNumber);       
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin4, aszMnemonicCashier, blockHeader.ulCashierID);       
		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, _T("Auth Type:"), RptStoreTranTypeLookup(blockHeader.usTranCode));       
		MldPutLineToDisplay (aszString, usRow);

		ParaMDC.uchMajorClass = CLASS_PARAMDC;
		ParaMDC.usAddress = MDC_SYSTEM3_ADR;
		ParaMDCRead(&ParaMDC);
		{
			int      iLoop;
			TCHAR    *tcsTransactionAmount = _T("Transaction Amount:");
			LONG     lValue = blockHeader.lChargeTtl;

			for (iLoop = 0; iLoop < 2; iLoop++) {
				// loop twice, once for transaction amount and once for tips amount if there are tips
				if (blockHeader.usEeptreqdata_Flag != 0) {  // is this a void transaction
					lValue *= -1;
				}
				if (lValue) {
					if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
					{
						// no decimal point in currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount2, tcsTransactionAmount, lValue);       
					}
					else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
					{
						// 2 decimal point currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount3, tcsTransactionAmount, lValue);    
					}
					else{
						// 3 decimal point currency
						usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, ReportAmount, tcsTransactionAmount, lValue);    
					}
					MldPutLineToDisplay (aszString, usRow);
				}
				if (blockHeader.lChargeTip) {
					tcsTransactionAmount = _T("Charge Tips Amount:");
					lValue = blockHeader.lChargeTip;
				} else {
					break;
				}
			}
		}

		MaskAcctNo(blockHeader.AcctNo, AcctNo);
		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, _T("Account Number:"), AcctNo );


		MldScroll1Buff.uchCurItem += (UCHAR)usRow;
		pszString = &aszString[0];
		while (usRow--) {
			MldString(MLD_SCROLL_1, MLD_SCROLL_APPEND, pszString, MLD_SUPER_MODE_CLM_LEN);
			pszString += (MLD_SUPER_MODE_CLM_LEN+1);
		}

		tcharTrimRight(blockHeader.TextResp);
		if(blockHeader.sProcessed == PROCESSED_ERROR){
			if(blockHeader.TextResp[0] == 0){
				usRet = CpmConvertError( blockHeader.sErrorCode);
				Lead.uchMajorClass = CLASS_PARALEADTHRU;
				Lead.uchAddress    = usRet;
				CliParaRead(&Lead);

				for(i = 0; i < 20; i++){
					tErrorMsg[i] = (TCHAR) Lead.aszMnemonics[i];
				}
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, aszMnemonicFailed, tErrorMsg); 
			}
			else
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, aszMnemonicFailed, blockHeader.TextResp);     
		}
		else if(blockHeader.sProcessed == PROCESSED_DECLINED){
			usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegSF3, aszMnemonicFailed, blockHeader.TextResp);  
		}

		MldPutLineToDisplay (aszString, usRow);

		usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchMldRptSupRegFin1, _T("Transaction Time:"));       
		MldPutLineToDisplay (aszString, usRow);

		/* check time */
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
			/* check date and print line */
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				/* TIME DD/MM/YY   */
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usYear);           
			} else {                                                 /* MM/DD/YY */
				/* TIME  MM/DD/YY  */
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usYear); 
			} 
		} else {
			/* check if Hour is '0' */
			if (blockHeader.DateTime.usHour == 0) {   /* in case of "0:00 AM" */
				usHour = 12;
			} else {
				usHour = blockHeader.DateTime.usHour;
			}
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				//TIME(AM/PM) DD/MM/YY 
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usYear); 
			} else {     
				 //TIME(AM/PM) MM/DD/YY 
				usRow = MldSPrintf(aszString, MLD_SUPER_MODE_CLM_LEN, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usYear);       
			}
		}           

		MldPutLineToDisplay (aszString, usRow);

		if (UieReadAbortKey() == UIE_ENABLE) {                              /* if Abort ?          */
			//return(RPT_ABORTED);
		} 
		if(RptPauseCheck() == RPT_ABORTED){
			// return(RPT_ABORTED);
		}
		RptFeed(RPT_DEFALTFEED);       
	}
}

VOID PrtStoreForwardrReport( VOID* pItem){
	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
	if(StoreForwardData->uchMinorClass == RPT_SUMMARY)
		PrtStoreSummaryReport(StoreForwardData);
	else if(StoreForwardData->uchMinorClass == RPT_DETAILED)
		PrtStoreDetailedReport(StoreForwardData);
	else if(StoreForwardData->uchMinorClass == RPT_FAILED)
		PrtStoreFailedReport(StoreForwardData);
}

// Print the AC102 Report from Supervisor Mode.
// report print keys off of uchMajorClass == CLASS_RPTSTOREFORWARD
// also keyword AC_STORE_FORWARD for searches
static VOID PrtStoreSummaryReport( VOID* pItem){
	TCHAR         *pReportTitle =               _T("Store Forward Summary Report");
	TCHAR         *pReportRecovered =           _T("  ** RECOVERED FILE **");
	TCHAR         *pTransactionsStore =         _T("Trans Stored:");
	TCHAR         *pTransactionsApproved =      _T("Trans Approved:");
	TCHAR         *pTransactionsDeclined =      _T("Trans Declined:");
	TCHAR         *pTransactionsError =         _T("Trans Errors:");
	TCHAR         *pTransactionsHardError =     _T("Trans Hard Errors:");
	TCHAR         *pTransactionTotal =          _T("Total Stored:");
	TCHAR         *pTransactionTotalVoid =      _T("Total Void:");
	TCHAR         *pFirstTransactionStoreDate = _T("Time of First:");
	TCHAR         *pLastTransactionStoreDate =  _T("Time of Last:");
	EEPTF_HEADER  *StoreForwardData = (EEPTF_HEADER*) pItem;
	USHORT         usHour;
    USHORT         usRow=0;
	PARAMDC		   ParaMDC;

	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pReportTitle);
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pReportTitle);
	if (StoreForwardData->lUieAc102_SubMenu & EEPT_SUBMENU_RECOVEREDFILE) {
		// Amtrak has seen problems with Stored File becoming invalid so we will indicate if this
		// report is from a recovered stored transaction file or not.
		PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pReportRecovered);
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pReportRecovered);
	}
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, _T(" "));      // empty line after the title

	// Print out the summary statistics from the Store and Forward file header.
	// These statistics are compiled in function EEptFile::ForwardEEptFileAdvancePointer() as
	// part of the processing of stored transactions by function CDsiClient::ForwardStoredTransactions().
	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin2, pTransactionsStore,    StoreForwardData->ulTransStored);
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, pTransactionsStore,    StoreForwardData->ulTransStored);

	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin2, pTransactionsApproved, StoreForwardData->ulApproved);
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, pTransactionsApproved, StoreForwardData->ulApproved);

	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin2, pTransactionsDeclined, StoreForwardData->ulFailedTrans);
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, pTransactionsDeclined, StoreForwardData->ulFailedTrans);

	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin2, pTransactionsError,    StoreForwardData->ulErrorTrans); 
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, pTransactionsError,    StoreForwardData->ulErrorTrans); 

	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin2, pTransactionsHardError,    StoreForwardData->ulHardErrorTrans); 
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, pTransactionsHardError,    StoreForwardData->ulHardErrorTrans); 

	// Determine the number of decimal places to display in the totals
	ParaMDC.uchMajorClass = CLASS_PARAMDC;
	ParaMDC.usAddress = MDC_SYSTEM3_ADR;
	ParaMDCRead(&ParaMDC);
	if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
	{
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount2, pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount2, pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount2, pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount2, pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
	}
	else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
	{
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount3, pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount3, pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount3, pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount3, pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
	}
	else{
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount,  pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount,  pTransactionTotal,   StoreForwardData->lTotalPending);
		PrtPrintf(PMG_PRT_JOURNAL, ReportAmount,  pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
		PrtPrintf(PMG_PRT_RECEIPT, ReportAmount,  pTransactionTotalVoid,   StoreForwardData->lTotalPendingVoid);
	}
	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pFirstTransactionStoreDate);   
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pFirstTransactionStoreDate);     

	/* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
        /* check date and print line */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
            /* TIME DD/MM/YY   */
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime4,
				StoreForwardData->fromTime.usHour,           
				StoreForwardData->fromTime.usMin,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
				StoreForwardData->fromTime.usHour,           
				StoreForwardData->fromTime.usMin,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usYear);           
        } else {                                                 /* MM/DD/YY */
            /* TIME  MM/DD/YY  */
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime4,
				StoreForwardData->fromTime.usHour,           
				StoreForwardData->fromTime.usMin,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
				StoreForwardData->fromTime.usHour,           
				StoreForwardData->fromTime.usMin,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usYear); 
        } 
	} else {
		/* check if Hour is '0' */
		if (StoreForwardData->fromTime.usHour == 0) {   /* in case of "0:00 AM" */
			usHour = 12;
		} else {
			usHour = StoreForwardData->fromTime.usHour;
		}
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
			//TIME(AM/PM) DD/MM/YY 
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->fromTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->fromTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usYear); 
		} else {     
			 //TIME(AM/PM) MM/DD/YY 
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->fromTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->fromTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->fromTime.usMonth,
				StoreForwardData->fromTime.usMDay,
				StoreForwardData->fromTime.usYear);       
		}
	}      

	if (StoreForwardData->toTime.usHour == 0) {   /* in case of "0:00 AM" */
        usHour = 12;
    } else {
        usHour = StoreForwardData->toTime.usHour;
    }
	PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pLastTransactionStoreDate); 
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pLastTransactionStoreDate); 

	/* check time */
    if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
        /* check date and print line */
        if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
            /* TIME DD/MM/YY   */
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime4,
				StoreForwardData->toTime.usHour,           
				StoreForwardData->toTime.usMin,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usYear);           

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
				StoreForwardData->toTime.usHour,           
				StoreForwardData->toTime.usMin,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usYear);        
        } else {                                                 /* MM/DD/YY */
            /* TIME  MM/DD/YY  */
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime4,        /* format */
				StoreForwardData->toTime.usHour,           
				StoreForwardData->toTime.usMin,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,        /* format */
				StoreForwardData->toTime.usHour,           
				StoreForwardData->toTime.usMin,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usYear); 
        } 
	} else {
		/* check if Hour is '0' */
		if (StoreForwardData->toTime.usHour == 0) {   /* in case of "0:00 AM" */
			usHour = 12;
		} else {
			usHour = StoreForwardData->toTime.usHour;
		}
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
			//TIME(AM/PM) DD/MM/YY 
			PrtPrintf(PMG_PRT_JOURNAL, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->toTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->toTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usYear); 
		} else {     
			 //TIME(AM/PM) MM/DD/YY 
			PrtPrintf(PMG_PRT_JOURNAL,
				auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->toTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usYear);

			PrtPrintf(PMG_PRT_RECEIPT,
				auchDateTime3,       
				(usHour > 12)? usHour - 12 : usHour,
				StoreForwardData->toTime.usMin,
				(usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
				StoreForwardData->toTime.usMonth,
				StoreForwardData->toTime.usMDay,
				StoreForwardData->toTime.usYear);        
		}
	}  
}

// Print the AC102 Report from Supervisor Mode.
// report print keys off of uchMajorClass == CLASS_RPTSTOREFORWARD
// also keyword AC_STORE_FORWARD for searches
static VOID PrtStoreDetailedReport( VOID* pItem){
	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
	USHORT usLineCount = 0;
    USHORT usRow=0;
	PARATERMINALINFO TermInfo;
	EEPT_BLOCK_HEADER blockHeader;
	LONG lBlockSize = 0;
	TCHAR AcctNo[22];
	TCHAR processedMessage [48];
	PARAMDC		ParaMDC;
	TCHAR       *pReportTitle =               _T("Store Forward Detail Report");
	TCHAR       *pReportRecovered =           _T("  ** RECOVERED FILE **");
	
	memset(processedMessage, 0, sizeof(processedMessage));
	memset(&AcctNo, 0, sizeof(AcctNo));
	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

	ParaTerminalInfoParaRead (&TermInfo);
	
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pReportTitle);  usLineCount++;
	if (StoreForwardData->lUieAc102_SubMenu & EEPT_SUBMENU_RECOVEREDFILE) {
		// Amtrak has seen problems with Stored File becoming invalid so we will indicate if this
		// report is from a recovered stored transaction file or not.
		PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pReportRecovered);
	}
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, _T(" "));    usLineCount++;    // empty line after the title

	memset (&blockHeader, 0, sizeof(EEPT_BLOCK_HEADER));        // initialize for the iteration through the file.
	while((lBlockSize = CliGetNextBlockIterate(StoreForwardData->lUieAc102_SubMenu, &blockHeader)) > 0){
		USHORT  usStoreNumber = TermInfo.TerminalInfo.usStoreNumber;

		memset(processedMessage, 0, sizeof(processedMessage));
		memset(&AcctNo, 0, sizeof(AcctNo));
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicTransaction, blockHeader.usConsNo);  usLineCount++;
		if (blockHeader.auchStoreNo) {
			usStoreNumber = RptStoreNumberFromBlock (blockHeader.auchStoreNo);
		}
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicCluster, usStoreNumber);    usLineCount++;
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicRegister, TermInfo.TerminalInfo.usRegisterNumber);    usLineCount++;
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin4, aszMnemonicCashier, blockHeader.ulCashierID);   usLineCount++;

		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, _T("Auth Type:"), RptStoreTranTypeLookup(blockHeader.usTranCode));     usLineCount++;

		{
			USHORT  usReturnType = TRN_DSI_TRANSSALE;
			TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN * 2 + 5];  /* PARA_... defined in "paraequ.h" */
			TCHAR  aszTranMnem2[PARA_TRANSMNEMO_LEN + 1];

			if (blockHeader.usReturnType & 0x0f00) {
				if (blockHeader.usReturnType & 0x100)    // check to see if RETURNS_MODIFIER_1 has been used in this transaction
					usReturnType = TRN_TRETURN1_ADR;
				else if (blockHeader.usReturnType & 0x200)    // check to see if RETURNS_MODIFIER_2 has been used in this transaction
					usReturnType = TRN_TRETURN2_ADR;
				else if (blockHeader.usReturnType & 0x400)    // check to see if RETURNS_MODIFIER_3 has been used in this transaction
					usReturnType = TRN_TRETURN3_ADR;
			}
			RflGetTranMnem(aszTranMnem, TRN_DSI_TRANSTYPE);
			RflGetTranMnem(aszTranMnem2, usReturnType);
			_tcscat (aszTranMnem, aszTranMnem2);
			PrtPrintf(PMG_PRT_RECEIPT, aszTranMnem);     usLineCount++;
		}

		ParaMDC.uchMajorClass = CLASS_PARAMDC;
		ParaMDC.usAddress = MDC_SYSTEM3_ADR;
		ParaMDCRead(&ParaMDC);
		{
			int      iLoop;
			TCHAR    *tcsTransactionAmount = _T("Transaction Amount:");
			LONG     lValue = blockHeader.lChargeTtl;

			for (iLoop = 0; iLoop < 2; iLoop++) {
				// loop twice, once for transaction amount and once for tips amount if there are tips
				if (blockHeader.usEeptreqdata_Flag != 0) {  // is this a void transaction
					lValue *= -1;
				}
				if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
				{
					// no decimal point in currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount2, tcsTransactionAmount, lValue);     usLineCount++;
				}
				else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
				{
					// 2 decimal point currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount3, tcsTransactionAmount, lValue);     usLineCount++;
				}
				else{
					// 3 decimal point currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount, tcsTransactionAmount, lValue);     usLineCount++;
				}
				tcsTransactionAmount = _T("Charge Tips Amount:");
				lValue = blockHeader.lChargeTip;
			}
		}

		MaskAcctNo(blockHeader.AcctNo, AcctNo);
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, _T("Account Number:"), AcctNo);     usLineCount++;

		if(blockHeader.sProcessed == PROCESSED_ERROR){
			_tcscpy (processedMessage, _T("Failed - Error"));
		}
		else if(blockHeader.sProcessed == PROCESSED_HARD_ERROR){
			_tcscpy (processedMessage, _T("Failed - Hard Error"));
		}
		else if(blockHeader.sProcessed == PROCESSED_DECLINED){
			_tcscpy (processedMessage, _T("Failed - Decline"));
		}
		else if(blockHeader.sProcessed == PROCESSED)
		{
			_tcscpy (processedMessage, _T("Approved"));
		}
		else {
			_tcscpy (processedMessage, _T("Stored"));
		}

		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, _T("Processed:"), processedMessage);    usLineCount++;

		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, _T("Transaction Time:"));       usLineCount++;

		/* check time */
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
			/* check date and print line */
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				/* TIME DD/MM/YY   */
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usYear);           
				  usLineCount++;
			} else {                                                 /* MM/DD/YY */
				/* TIME  MM/DD/YY  */
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
						  blockHeader.DateTime.usHour,           
						  blockHeader.DateTime.usMin,
						  blockHeader.DateTime.usMonth,
						  blockHeader.DateTime.usMDay,
						  blockHeader.DateTime.usYear); 
				  usLineCount++;
			} 
		} else {
			/* check if Hour is '0' */
			if (blockHeader.DateTime.usHour == 0) {   /* in case of "0:00 AM" */
				usHour = 12;
			} else {
				usHour = blockHeader.DateTime.usHour;
			}
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				//TIME(AM/PM) DD/MM/YY 
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usYear); 
				  usLineCount++;
			} else {     
				 //TIME(AM/PM) MM/DD/YY 
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
							  (usHour > 12)? usHour - 12 : usHour,
							  blockHeader.DateTime.usMin,
							  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
							  blockHeader.DateTime.usMonth,
							  blockHeader.DateTime.usMDay,
							  blockHeader.DateTime.usYear);
				  usLineCount++;
			}
		}      
    	RptFeedPrintNormal(RPT_DEFALTFEED, &usLineCount); 
	}
}

VOID PrtStoreFailedReport( VOID* pItem){
	EEPTF_HEADER* StoreForwardData = (EEPTF_HEADER*) pItem;
    TCHAR  aszPrtNull[1] = {_T('\0')};
    USHORT usHour;
    USHORT usRow=0;
	USHORT usLineCount = 0;
	SHORT	i;
	PARATERMINALINFO TermInfo;
	EEPT_BLOCK_HEADER blockHeader;
	LONG lBlockSize = 0;
	TCHAR AcctNo[22];
	TCHAR tErrorMsg[20];
	USHORT          usRet;
    PARALEADTHRU  Lead;
	PARAMDC		ParaMDC;
	TCHAR       *pReportTitle =               _T("Store Forward Declined Report");
	TCHAR       *pReportRecovered =           _T("  ** RECOVERED FILE **");

	i = 0;
    memset(&Lead, 0, sizeof(Lead));
	memset(&AcctNo, 0, sizeof(AcctNo));
	memset(&tErrorMsg, 0, sizeof(tErrorMsg));
	TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);
	
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, pReportTitle);   usLineCount++;
	if (StoreForwardData->lUieAc102_SubMenu & EEPT_SUBMENU_RECOVEREDFILE) {
		// Amtrak has seen problems with Stored File becoming invalid so we will indicate if this
		// report is from a recovered stored transaction file or not.
		PrtPrintf(PMG_PRT_JOURNAL, auchMldRptSupRegFin1, pReportRecovered);
	}
	PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, _T(" "));    usLineCount++;   // empty line after the title

	memset (&blockHeader, 0, sizeof(EEPT_BLOCK_HEADER));        // initialize for the iteration through the file.
	while((lBlockSize = CliGetNextBlockIterateAll(StoreForwardData->lUieAc102_SubMenu, &blockHeader)) > 0){
		USHORT  usStoreNumber = TermInfo.TerminalInfo.usStoreNumber;

		if( (blockHeader.sProcessed != PROCESSED_DECLINED) && (blockHeader.sProcessed != PROCESSED_ERROR) && (blockHeader.sProcessed != PROCESSED_HARD_ERROR)){
			continue;
		}

		memset(&AcctNo, 0, sizeof(AcctNo));
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicTransaction, blockHeader.usConsNo);   usLineCount++;
		if (blockHeader.auchStoreNo) {
			usStoreNumber = RptStoreNumberFromBlock (blockHeader.auchStoreNo);
		}
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicCluster, usStoreNumber);   usLineCount++;
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin2, aszMnemonicRegister, TermInfo.TerminalInfo.usRegisterNumber);   usLineCount++;
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin4, aszMnemonicCashier, blockHeader.ulCashierID);   usLineCount++;
		
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, _T("Auth Type:"), RptStoreTranTypeLookup(blockHeader.usTranCode));      usLineCount++;

		{
			USHORT  usReturnType = TRN_DSI_TRANSSALE;
			TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN * 2 + 5];  /* PARA_... defined in "paraequ.h" */
			TCHAR  aszTranMnem2[PARA_TRANSMNEMO_LEN + 1];

			if (blockHeader.usReturnType & 0x0f00) {
				if (blockHeader.usReturnType & 0x100)    // check to see if RETURNS_MODIFIER_1 has been used in this transaction
					usReturnType = TRN_TRETURN1_ADR;
				else if (blockHeader.usReturnType & 0x200)    // check to see if RETURNS_MODIFIER_2 has been used in this transaction
					usReturnType = TRN_TRETURN2_ADR;
				else if (blockHeader.usReturnType & 0x400)    // check to see if RETURNS_MODIFIER_3 has been used in this transaction
					usReturnType = TRN_TRETURN3_ADR;
			}
			RflGetTranMnem(aszTranMnem, TRN_DSI_TRANSTYPE);
			RflGetTranMnem(aszTranMnem2, usReturnType);
			_tcscat (aszTranMnem, aszTranMnem2);
			PrtPrintf(PMG_PRT_RECEIPT, aszTranMnem);        usLineCount++;
		}

		ParaMDC.uchMajorClass = CLASS_PARAMDC;
		ParaMDC.usAddress = MDC_SYSTEM3_ADR;
		ParaMDCRead(&ParaMDC);
		
		{
			int      iLoop;
			TCHAR    *tcsTransactionAmount = _T("Transaction Amount:");
			LONG     lValue = blockHeader.lChargeTtl;

			for (iLoop = 0; iLoop < 2; iLoop++) {
				// loop twice, once for transaction amount and once for tips amount if there are tips
				if (blockHeader.usEeptreqdata_Flag != 0) {  // is this a void transaction
					lValue *= -1;
				}

				if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
				{
					// no decimal point in currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount2, tcsTransactionAmount, lValue);        usLineCount++;
				}
				else if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
				{
					// 2 decimal point currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount3, tcsTransactionAmount, lValue);     usLineCount++;
				}
				else{
					// 3 decimal point currency
					PrtPrintf(PMG_PRT_RECEIPT, ReportAmount, tcsTransactionAmount, lValue);     usLineCount++;
				}
				tcsTransactionAmount = _T("Charge Tips Amount:");
				lValue = blockHeader.lChargeTip;
			}
		}
		
		MaskAcctNo(blockHeader.AcctNo, AcctNo);
		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, _T("Account Number:"), AcctNo);    usLineCount++;

		tcharTrimRight(blockHeader.TextResp);
		if(blockHeader.sProcessed == PROCESSED_ERROR || blockHeader.sProcessed == PROCESSED_HARD_ERROR){
			//display the Error message
			if(blockHeader.TextResp[0] == 0){
				usRet = CpmConvertError( blockHeader.sErrorCode);
				Lead.uchMajorClass = CLASS_PARALEADTHRU;
				Lead.uchAddress    = usRet;
				CliParaRead(&Lead);

				for(i = 0; i < 20; i++){
					tErrorMsg[i] = (TCHAR) Lead.aszMnemonics[i];
				}
				PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, aszMnemonicFailed, tErrorMsg);    usLineCount++;
			}
			else
    			PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, aszMnemonicFailed, blockHeader.TextResp);     usLineCount++;
		}
		else if(blockHeader.sProcessed == PROCESSED_DECLINED){
			PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegSF3, aszMnemonicFailed, blockHeader.TextResp);     usLineCount++;
		}

		PrtPrintf(PMG_PRT_RECEIPT, auchMldRptSupRegFin1, _T("Transaction Time:"));       usLineCount++;

		if (blockHeader.DateTime.usHour == 0) {   /* in case of "0:00 AM" */
			usHour = 12;
		} else {
			usHour = blockHeader.DateTime.usHour;
		}
		
		/* check time */
		if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT3)) {        /* in case of military hour */
			/* check date and print line */
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				/* TIME DD/MM/YY   */
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,
					  blockHeader.DateTime.usHour,           
					  blockHeader.DateTime.usMin,
					  blockHeader.DateTime.usMDay,
					  blockHeader.DateTime.usMonth,
					  blockHeader.DateTime.usYear);
				  usLineCount++;
			} else {                                                 /* MM/DD/YY */
				/* TIME  MM/DD/YY  */
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime4,        /* format */
					  blockHeader.DateTime.usHour,           
					  blockHeader.DateTime.usMin,
					  blockHeader.DateTime.usMonth,
					  blockHeader.DateTime.usMDay,
					  blockHeader.DateTime.usYear);
				  usLineCount++;
			} 
		} else {
			/* check if Hour is '0' */
			if (blockHeader.DateTime.usHour == 0) {   /* in case of "0:00 AM" */
				usHour = 12;
			} else {
				usHour = blockHeader.DateTime.usHour;
			}
			if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {    /* DD/MM/YY */
				//TIME(AM/PM) DD/MM/YY 
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
					  (usHour > 12)? usHour - 12 : usHour,
					  blockHeader.DateTime.usMin,
					  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
					  blockHeader.DateTime.usMDay,
					  blockHeader.DateTime.usMonth,
					  blockHeader.DateTime.usYear);
				  usLineCount++;
			} else {     
				 //TIME(AM/PM) MM/DD/YY 
				  PrtPrintf(PMG_PRT_RECEIPT, auchDateTime3,       
					  (usHour > 12)? usHour - 12 : usHour,
					  blockHeader.DateTime.usMin,
					  (usHour >= 12)? STD_TIME_PM_SYMBOL : STD_TIME_AM_SYMBOL,
					  blockHeader.DateTime.usMonth,
					  blockHeader.DateTime.usMDay,
					  blockHeader.DateTime.usYear);
				  usLineCount++;
			}
		}       
    	RptFeedPrintNormal(RPT_DEFALTFEED, &usLineCount); 
	}
}
VOID MaskAcctNo(UCHAR* AcctNo, TCHAR* AcctNo2){
	SHORT x,i;
	x = 0;
	i = 0; 
	while( i < 22){
		AcctNo2[i] = AcctNo[i];
		i++;
	}
/*	x-=4;
	for(i=0;i<x;i++){
		AcctNo2[i] =  _T('X');
	}

	AcctNo2[i+1] = AcctNo[i+1];
	AcctNo2[i+2] = AcctNo[i+2];
	AcctNo2[i+3] = AcctNo[i+3];
*/
}
#endif
