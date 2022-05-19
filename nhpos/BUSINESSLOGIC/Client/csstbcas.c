/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Source File of Cashier Function                   
* Category    : Client/Server STUB, US Hospitality Mode
* Program Name: CSSTBCAS.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               CliCasSignIn()          Cashier Sign-In
*               CliCasSignOut()         Cashier Sign-Out
*               CliCasPreSignOut()      Cashier Pre Sign-Out
*
*               SerCasClose()           Cashier Close
*               SerCasAssign()          Cashier Assign
*               SerCasDelete()          Cashier Delete
*               SerCasSecretClr()       Cashier secret code clear
*               SerCasAllRead()         Cashier all read
*               SerCasAllIdRead()       Cashier all ID read
*               SerCasIndRead()         Cashier Individual Read
*               SerCasIndLock()         Cashier Individual Lock
*               SerCasIndUnLock()       Cashier Individual UnLock
*               SerCasAllLock()         Cashier all Lock
*               SerCasAllUnLock()       Cashier all unclock
*               SerCasIndTermLock()     Individual Terminal Lock R3.1
*               SerCasIndTermUnLock()   Individual Terminal UnLock R3.1
*               SerCasAllTermLock()     All Terminal Lock R3.1
*               SerCasAllTermUnLock()   All Terminal unclock R3.1
*               SerCasChkSignIn()       Cashier Check sign-in cashier exit
*
*               CstCasSignIn()          Sub function of Cashier Sign-in
*
*                  CstInitCasMsgH()     Initialize Request/Response Msg Hed
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jul-27-93:H.Yamaguchi: Adds CstInitCasMsgH() for reduction 
* Dec-06-95:M.Ozawa    : Adds SerCasIndTermLock()/SerCasIndTermUnLock()/
*                        SerCasAllTermLock()/SerCasAllTermUnLock()
* Aug-03-98:M.Ozawa    : Enhanced to R3.3 Cashier feature
* Dec-02-99:hrkato     : Saratoga
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
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <cscas.h>
#include    <csttl.h>
#include    <csstbstb.h>
#include    <csstbcas.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"
#include	"display.h"
#include    "csop.h"
#include    "BlFWif.h"
#include    <rfl.h>

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;

static CASIF *CstAssignRescashierToCasif (CASIF *pCasif, CLIRESCASHIER  *pResMsgH)
{
    memcpy(pCasif->fbCashierStatus, pResMsgH->fbStatus, CLI_CASHIERSTATUS);
    _tcsncpy(pCasif->auchCashierName, pResMsgH->auchCasName, CLI_CASHIERNAME);
    pCasif->usGstCheckStartNo = pResMsgH->usStartGCN;
    pCasif->usGstCheckEndNo   = pResMsgH->usEndGCN;
    pCasif->uchChgTipRate     = pResMsgH->uchChgTipRate;
    pCasif->uchTeamNo         = pResMsgH->uchTeamNo;
    pCasif->uchTerminal       = pResMsgH->uchTerminal;
	pCasif->usSupervisorID    = pResMsgH->usSupervisorID;
	pCasif->usCtrlStrKickoff  = pResMsgH->usCtrlStrKickoff;
	pCasif->usStartupWindow   = pResMsgH->usStartupWindow;
	pCasif->ulGroupAssociations = pResMsgH->ulGroupAssociations;
	pCasif->ulCashierSecret   = pResMsgH->ulSecretNo;
	return pCasif;
}

static CLIRESCASHIER *CstAssignCasifToRescashier (CLIRESCASHIER  *pResMsgH, CASIF *pCasif)
{
	pResMsgH->ulCashierNo = pCasif->ulCashierNo;
    pResMsgH->ulSecretNo  = pCasif->ulCashierSecret;
    pResMsgH->usStartGCN  = pCasif->usGstCheckStartNo;
    pResMsgH->usEndGCN    = pCasif->usGstCheckEndNo;
    pResMsgH->uchChgTipRate = pCasif->uchChgTipRate;
    pResMsgH->uchTeamNo   = pCasif->uchTeamNo;
    pResMsgH->uchTerminal = pCasif->uchTerminal;
	pResMsgH->usSupervisorID = pCasif->usSupervisorID;
	pResMsgH->usCtrlStrKickoff = pCasif->usCtrlStrKickoff;
	pResMsgH->usStartupWindow = pCasif->usStartupWindow;
	pResMsgH->ulGroupAssociations = pCasif->ulGroupAssociations;
    memcpy(pResMsgH->fbStatus, pCasif->fbCashierStatus, CLI_CASHIERSTATUS);
    _tcsncpy(pResMsgH->auchCasName, pCasif->auchCashierName, CLI_CASHIERNAME);
	return pResMsgH;
}


/*
*===========================================================================
** Synopsis:    SHORT   CliCasSignIn(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-in
*
** Return:      CAS_PERFORM:        sign-in OK
*               CAS_LOCK:           she is still locked
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_SECRET_UNMATCH: wrong her secret code      
*
** Description: This function supports Cashier Sign-In function.
*===========================================================================
*/
SHORT   CliCasSignIn(CASIF *pCasif)
{
	int			    unlockStatus;	/* License status - TLDJR */
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sRetCode = CAS_PERFORM;
    SHORT           sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;
	SHORT			sMoved=0;

#ifdef DEMO_VERSION
	if(pCasif->ulCashierNo != 1 && pCasif->ulCashierNo !=2){
		return CAS_NOT_IN_FILE;
	}
#endif

	if (OpSignInStatus () != 0) {
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: CAS_LOCK Sign-in failed OpSignInStatus().");
		return (CAS_LOCK);
	}

	// check to see if this GenPOS terminal is licensed.
	// licensing is done on a terminal by terminal basis.
	unlockStatus = BlFwIfLicStatus();
	switch(unlockStatus)
	{
		case UNREGISTERED_COPY: return CAS_NOT_REGISTERED;
		case LICENSE_MISSING:   return CAS_NOT_REGISTERED;
		case DEMO_MODE: break;
	}

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;
    ReqMsgH.ulSecretNo = pCasif->ulCashierSecret;
    ReqMsgH.ulOption    = pCasif->ulCashierOption;  /* V3.3 */
    ReqMsgH.usTerminalNo = pCasif->usUniqueAddress = (USHORT)CliReadUAddr();/* Set unique address */

    memset(&CliMsg, 0, sizeof(CLICOMIF));
	CliMsg.usFunCode    = CLI_FCCASSIGNIN | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasSignIn(pCasif);
        sMoved = 1;
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

    {
		SHORT          sCliRetCode = CliMsg.sRetCode;
		CLIRESCASHIER  ResMsgHSave = ResMsgH;

		// If the Master terminal allowed the operator sign-in to work
		// then we try to update the Backup Master terminal by letting
		// it know of the sign-in attempt.  However, there are some
		// circumstances when a successful sign-in from the Master
		// will not be followed by a successful sign-in from the Backup.
		// The more common is if Master and Backup Master are out of synch
		// and the Backup Master thinks someone is signed-in when they aren't.
		// Another is after a parameter broadcast has caused the operator file
		// on the Backup Master to be resized and all operators deleted and
		// the Master and Backup Master have not yet been synchronized with
		// an AC42.

		if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
			CASIF   CasifBackup = *pCasif;
            SHORT   sBRetCode;

            sBRetCode = CasSignIn(&CasifBackup);
			sErrorBM = STUB_SUCCESS;
            if (STUB_M_DOWN == sErrorM) {
				// Communication with Master failed so use what we got from the
				// Backup Master.
                CliMsg.sRetCode = sBRetCode;
				*pCasif = CasifBackup;
                sMoved = 1;
            }
		} else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
			// If there was an error talking to the Backup Master Terminal
			// then we will return the status of the Master Terminal request.
			if (CAS_PERFORM != CliMsg.sRetCode && CAS_REQUEST_SECRET_CODE != CliMsg.sRetCode) {
				CliMsg.sRetCode = sCliRetCode;
				ResMsgH = ResMsgHSave;
			}
		}
    }

    if (sMoved == 0) {
		CstAssignRescashierToCasif (pCasif, &ResMsgH);
    }

	sRetCode = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && STUB_DISCOVERY != sErrorBM)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliCasSignIn() sRetCode %d  sErrorM %d  sErrorBM %d", sRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCasSignOut(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-out
*
** Return:      CAS_PERFORM:    sign-out OK
*               CAS_NOT_SIGNIN: she is not sign-in
*
** Description: This function supports Cashier Sign-Out function.
*===========================================================================
*/
SHORT   CliCasSignOut(CASIF *pCasif)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sError = 0;            
	SHORT			sErrorM = 0, sErrorBM = 0;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;
    ReqMsgH.usTerminalNo = pCasif->usUniqueAddress = (USHORT)CliReadUAddr();    /* Set unique address */
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASSIGNOUT | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasSignOut(pCasif);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	{
		SHORT  sCliRetCode = CliMsg.sRetCode;

		if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
			CliMsg.sRetCode = CasSignOut(pCasif);
			sErrorBM = STUB_SUCCESS;
			if (STUB_SUCCESS == sErrorM) {
				CliMsg.sRetCode = sCliRetCode;
			}
		} else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
			// If there was an error talking to the Backup Master Terminal
			// then we will return the status of the Master Terminal request.
			CliMsg.sRetCode = sCliRetCode;
		}
	}

    sError = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliCasSignOut() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCasPreSignOut(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to pre sign-out
*
** Return:      CAS_PERFORM:    sign-out OK
*
** Description:
*   This function supports Cashier Pre-Sign-Out.
*   This function must be executed before sign-out function thru the Total
*   module (CliTtlUpdateFile()). This function informs cashier module that 
*   sign-out operation performed on the terminal. Cashier Module will be 
*   disable another sign-in function untill receiving the sign-out function 
*   from the total module.
*===========================================================================
*/
SHORT   CliCasPreSignOut(CASIF *pCasif)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sError = 0;            
	SHORT			sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;
    ReqMsgH.usTerminalNo = pCasif->usUniqueAddress = (USHORT)CliReadUAddr();    /* Set unique address */
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASPRESIGNOUT | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasPreSignOut(pCasif);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sError) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sError = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	{
		SHORT  sCliRetCode = CliMsg.sRetCode;

		if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
			CliMsg.sRetCode = CasPreSignOut(pCasif);
			sErrorBM = STUB_SUCCESS;

			if (STUB_SUCCESS == sErrorM) {
				CliMsg.sRetCode = sCliRetCode;
			}
		} else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
			// If there was an error talking to the Backup Master Terminal
			// then we will return the status of the Master Terminal request.
			CliMsg.sRetCode = sCliRetCode;
		}
	}

	sError = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliCasPreSignOut() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CstCasPreSignOut(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-in
*
** Return:      CAS_PERFORM:        sign-in OK
*               CAS_LOCK:           she is still locked
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_SECRET_UNMATCH: wrong her secret code      
*
** Description: This function supports Cashier Pre Sign-Out sub function, R3.3.
*===========================================================================
*/
SHORT   CstCasPreSignOut(CASIF *pCasif)
{
    SHORT   sRetry, sRetryCo;
	SHORT	sError;
    USHORT  usPrevious;

    sRetryCo = sRetry = CstReadMDCPara(CLI_MDC_RETRY);
    do {
        sError = CasPreSignOut(pCasif);

        if (CAS_DURING_SIGNIN == sError) {
            
            /* allow power switch at error display */
            usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */

            if (0 > --sRetryCo) {
                CstDisplayError(STUB_IAM_BUSY, CliSndBuf.auchFaddr[CLI_POS_UA]);
                sRetryCo = sRetry;
            } else {
                CstSleep();
            }
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
        }
    } while (CAS_DURING_SIGNIN == sError);

    return sError;
}

/*
*===========================================================================
** Synopsis:    USHORT  SerCasClose(USHORT usCashierNo);
*
*     InPut:    usCashierNo - Cashier No. want to close
*
** Return:      CAS_PERFORM:    closed
*               CAS_NO_SIGNIN:  she is not sign-in      
*
** Description: This function supports Cashier Close function.
*===========================================================================
*/
SHORT  SerCasClose(ULONG ulCashierNo)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDCLOSE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = CasClose(ulCashierNo);
    } else {
        sRetCode = CAS_NOT_MASTER;
    }
    if (CAS_PERFORM == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerCasAssign(CASIF *pCasif);
*
*     InPut:    pCasif   - Cashier information should be assigned
*
** Return:      CAS_PERFORM:    assigned
*               CAS_CASHIER_FULL:   exceed number of cashier assigned     
*
** Description: This function supports Cashier Assign function.
*===========================================================================
*/
SHORT   SerCasAssign(CASIF *pCasif)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sError, sErrorM, sErrorBM;

    PifRequestSem(husCliExeNet);

    /* V3.3 */
    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;
    ReqMsgH.ulSecretNo  = pCasif->ulCashierSecret;
    ReqMsgH.usStartGCN  = pCasif->usGstCheckStartNo;
    ReqMsgH.usEndGCN    = pCasif->usGstCheckEndNo;
    ReqMsgH.uchChgTipRate = pCasif->uchChgTipRate;
    ReqMsgH.uchTeamNo   = pCasif->uchTeamNo;
    ReqMsgH.uchTerminal = pCasif->uchTerminal;
	ReqMsgH.usSupervisorID = pCasif->usSupervisorID;
	ReqMsgH.usCtrlStrKickoff = pCasif->usCtrlStrKickoff;
	ReqMsgH.usStartupWindow = pCasif->usStartupWindow;
	ReqMsgH.ulGroupAssociations = pCasif->ulGroupAssociations;
    memcpy(ReqMsgH.fbStatus, pCasif->fbCashierStatus, CLI_CASHIERSTATUS);
    _tcsncpy(ReqMsgH.auchCasName, pCasif->auchCashierName, CLI_CASHIERNAME);
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASASSIGN | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    sError = CAS_NOT_ALLOWED;
    sErrorM = CstSendMaster();
    if (STUB_SELF == sErrorM) {
        sError = CasAssign(pCasif);
		sErrorM = STUB_SUCCESS;
    } else if (STUB_SUCCESS == sErrorM) {
		sError = CliMsg.sRetCode;
	}
	
	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
	}

    if (sErrorM == STUB_M_DOWN || CAS_PERFORM == sError) {
        sErrorBM = CstSendBMaster();                  /* Send to B Master */
		if (STUB_SELF == sErrorBM) {
			sError = CasAssign(pCasif);
			sErrorBM = STUB_SUCCESS;
		} else if (STUB_SUCCESS == sErrorBM) {
			sError = CliMsg.sRetCode;
		}
    }

	/*====================== TEST START ========================*/
	if (PifSysConfig()->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS) {  /* SerCasAssign() Debug Option, Generate Cashier records */
		if ((CAS_PERFORM == sError) &&
			(pCasif->auchCashierName[0] == _T('A')) &&
			(pCasif->auchCashierName[1] == _T('0')) &&
			(pCasif->auchCashierName[2] == _T('A')) &&
			(pCasif->auchCashierName[3] == _T('1')))
		{
			ULONG   ulCashierNo = 1;
			CASIF   Casif = *pCasif;
			SHORT   sErrorTemp = sError;

			NHPOS_NONASSERT_NOTE("==TEST ENABLE", "==TEST ENABLE: Generate FLEX_CAS_MAX Cashier records.");
			for (ulCashierNo = 1; (CAS_PERFORM == sErrorTemp) && (ulCashierNo <= FLEX_CAS_MAX); ulCashierNo++) {
				Casif.ulCashierNo = ulCashierNo;
				Casif.auchCashierName[0] = _T('C');
				Casif.auchCashierName[1] = _T('A');
				Casif.auchCashierName[2] = _T('S');
				Casif.auchCashierName[3] = (TCHAR)(ulCashierNo / 100) + _T('0');
				Casif.auchCashierName[4] = (TCHAR)((ulCashierNo % 100) / 10) + _T('0');
				Casif.auchCashierName[5] = (TCHAR)(ulCashierNo % 10) + _T('0');
				sErrorTemp = CasAssign(&Casif);
			}
		}
	}
	/*========================= TEST END =============================*/

    PifReleaseSem(husCliExeNet);

	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: SerCasAssign() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerCasDelete(USHORT usCashierNo);
*
*     InPut:    ulCashierNo - Cashier No. want to delete
*
** Return:      CAS_PERFORM:    deleted
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_DURING_SIGNIN:  she is still sign-in      
*
** Description: This function supports Cashier Delete function.
*===========================================================================
*/
SHORT   SerCasDelete(ULONG ulCashierNo)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASDELETE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = CasDelete(ulCashierNo);
        if (CAS_PERFORM == sRetCode) {
			CLITTLCASWAI    TtlCas = {0};

            TtlCas.uchMajorClass = CLASS_TTLCASHIER;
            TtlCas.ulCashierNo = ulCashierNo;
            TtlCas.uchMinorClass = CLASS_TTLSAVDAY;
            TtlTotalDelete(&TtlCas);
            TtlCas.uchMinorClass = CLASS_TTLSAVPTD;
            TtlTotalDelete(&TtlCas);
        }
    } else {
        sRetCode = CAS_NOT_MASTER;
    }
    if (CAS_PERFORM == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT  SerCasSecretClr(USHORT usCashierNo);
*
*     InPut:    ulCashierNo - Cashier No. want to clear her secret
*
** Return:      CAS_PERFORM:        cleared.
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_DURING_SIGNIN:  she is still sign-in      
*
** Description: This function supports Cashier Secret No clear function.
*===========================================================================
*/
SHORT  SerCasSecretClr(ULONG ulCashierNo)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sRetCode, sError;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCLRSCODE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == CstSendMaster()) {
        sRetCode = CasSecretClr(ulCashierNo);
    } else {
		sRetCode = CliMsg.sRetCode;
    }
    if (CAS_PERFORM == sRetCode) {
        sError = CstSendBMaster();                  /* Send to B Master */
    }

	PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT  SerCasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer);
*     InPut:    usRcvBufferSize - buffer size can be read
*    OutPut:    aulRcvBuffer	- Cashier Id can be read
*
** Return:      number of cashier can be read      
*
** Description: This function supports Cashier All Id Read function.
*===========================================================================
*/
SHORT  SerCasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasAllIdRead(usRcvBufferSize, aulRcvBuffer);
    } else
	{
		return CliCasAllIdRead(usRcvBufferSize, aulRcvBuffer);
	}

    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerCasIndRead(CASIF *pCasif);
*     InOut:    Casif   - Cashier information should be read
*
** Return:      CAS_PERFORM:    read OK
*               CAS_NOT_IN_FILE:    she is not assigned      
*
** Description: This function supports Cashier Individual Read function.
*===========================================================================
*/
SHORT   SerCasIndRead(CASIF *pCasif)
{
	SHORT   sRetStatus = CAS_NOT_MASTER;

    if (STUB_SELF == SstReadAsMaster()) {
        sRetStatus = CasIndRead(pCasif);
    } else
	{
		sRetStatus = CliCasIndRead(pCasif);
	}
    return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT  SerCasIndLock(USHORT usCashierNo);
*     InPut:    ulCashierNo - Cashier No. want to lock
*
** Return:      CAS_PERFORM:    locked
*               CAS_DURING_SIGNIN:  she is still sign-in          
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_OTHER_LOCK:     another cashier is still locked
*
** Description: This function supports Cashier Individual Lock function.
*===========================================================================
*/
SHORT   SerCasIndLock(ULONG ulCashierNo)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasIndLock(ulCashierNo);
    }
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT  SerCasIndUnLock(USHORT usCashierNo);
*     InPut:    ulCashierNo - Cashier No. want to unlock
*
** Return:      CAS_PERFORM:    unlocked
*               CAS_DIFF_NO:    she is not locked      
*
** Description: This function supports Cashier Individual UnLock function.
*===========================================================================
*/
SHORT   SerCasIndUnLock(ULONG ulCashierNo)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasIndUnLock(ulCashierNo);
    } 
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerCasAllLock(VOID);
*
** Return:      CAS_PEFORM:     all lock OK
*               CAS_ALREADY_LOCK:   already locked
*               CAS_DURING_SIGNIN:  exist sign-in cashier      
*
** Description: This function supports Cashier All Lock function.
*===========================================================================
*/
SHORT   SerCasAllLock(VOID)
{
	SHORT  sRetStatus = CAS_NOT_MASTER;
    if (STUB_SELF == SstReadAsMaster()) {
        sRetStatus = CasAllLock();
    } 
    return sRetStatus;
}

SHORT   SerCasAllLockForceOutCheck(VOID)
{
	SHORT  sRetStatus = CAS_NOT_MASTER;
    if (STUB_SELF == SstReadAsMaster()) {
        sRetStatus = Cas_SignInCheckAgainstForcedOut();
    } 
    return sRetStatus;
}


/*
*===========================================================================
** Synopsis:    VOID    SerCasAllUnLock(VOID);
*
** Return:      none.      
*
** Description: This function supports Cashier All UnLock function.
*===========================================================================
*/
VOID    SerCasAllUnLock(VOID)
{
    CasAllUnLock();
}
    
/*
*===========================================================================
** Synopsis:    SHORT  SerCasIndTermLock(USHORT usTerminalNo);
*     InPut:    usTerminalNo - Terminal No. want to lock
*
** Return:      CAS_PERFORM:    locked
*               CAS_DURING_SIGNIN:  she is still sign-in          
*               CAS_OTHER_LOCK:     another cashier is still locked
*
** Description: This function supports Individual Terminal Lock function. R3.1
*===========================================================================
*/
SHORT   SerCasIndTermLock(USHORT usTerminalNo)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasIndTermLock(usTerminalNo);
    }
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT  SerCasIndTermUnLock(USHORT usTerminalNo);
*     InPut:    usTerminalNo - Terminal No. want to unlock
*
** Return:      CAS_PERFORM:    unlocked
*               CAS_DIFF_NO:    she is not locked      
*
** Description: This function supports Individual Terminal UnLock function. R3.1
*===========================================================================
*/
SHORT   SerCasIndTermUnLock(USHORT usTerminalNo)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasIndTermUnLock(usTerminalNo);
    } 
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerCasAllTermLock(VOID);
*
** Return:      CAS_PEFORM:     all lock OK
*               CAS_ALREADY_LOCK:   already locked
*               CAS_DURING_SIGNIN:  exist sign-in cashier      
*
** Description: This function supports All Terminal Lock function. R3.1
*===========================================================================
*/
SHORT   SerCasAllTermLock(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasAllTermLock();
    } 
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    VOID    SerCasAllTermUnLock(VOID);
*
** Return:      none.      
*
** Description: This function supports All Terminal UnLock function. R3.1
*===========================================================================
*/
VOID    SerCasAllTermUnLock(VOID)
{
    CasAllTermUnLock();
}

/*
*===========================================================================
** Synopsis:    VOID    SerCasDelayedBalance(UCHAR uchTermNo)
*
** Return:      none.      
*
** Description: This function turns delayed balance to on for the terminal
*===========================================================================
*/
SHORT    SerCasDelayedBalance(UCHAR uchTermNo, ULONG ulState)
{
	USHORT     usTermNo;

	usTermNo = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

    if(STUB_SELF == SstReadAsMaster())
	{
		CasDelayedBalance(uchTermNo, ulState);

		if(usTermNo == uchTermNo)
		{
			/*The following PM_CNTRL was not being used, so we use this one
			to display the D-B message on the display*/
			flDispRegDescrControl |= PM_CNTRL;
			flDispRegKeepControl |= PM_CNTRL;

			//We set the class so that we can send the dummy data,
			//and trigger the discriptor to show D-B to inform the user that
			//the terminal is in delay balance mode
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
		}

		return SUCCESS;

	}else
	{
		return CliCasDelayedBalance(uchTermNo, ulState);
	}
	return CAS_NOT_MASTER;
}
    
/*
*===========================================================================
** Synopsis:    SHORT   SerCasChkSignIn(USHORT *ausRcvBuffer);
*    OutPut:    aulRcvBuffer   - store cashier number by terminal
*
** Return:      CAS_PERFORM:    not exist sign-in cashier
*               CAS_DURING_SIGNIN:  exist sign-in cashier         
*
** Description: This function supports to check that sign-in cashier exist or not.
*===========================================================================
*/
SHORT   SerCasChkSignIn(ULONG *aulRcvBuffer)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return CasChkSignIn(aulRcvBuffer);
    }
    return CAS_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCasOpenPickupLoan(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-in
*
/** Return:      CAS_PERFORM:        sign-in OK
*               CAS_LOCK:           she is still locked
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_ALREADY_OPENED: Other opened
*
** Description:
*   This function supports Cashier Sign-In for pickup/loan function.
*===========================================================================
*/
SHORT   CliCasOpenPickupLoan(CASIF *pCasif)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sError;            
    SHORT           sCliRetCode = 0, sErrorM, sErrorBM = 0;
	SHORT			sMoved = 0;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo  = pCasif->ulCashierNo;
    ReqMsgH.usTerminalNo = pCasif->usUniqueAddress = (USHORT)CliReadUAddr();/* Set unique address */

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASOPENPICLOA | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);
    CliMsg.sRetCode     = CAS_PERFORM;      /* added 3/4 */

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasOpenPickupLoan(pCasif);
        sMoved = 1;
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
		SHORT    sBRetCode = 0;            
        SHORT    sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);
        do {                                        /* Add loop, Dec/02/2000 */
            sBRetCode = CasOpenPickupLoan(pCasif);
            if (CAS_PERFORM != sBRetCode) {
                CstSleep();
            } else {
                break;
            }
        } while (0 <= --sRetryCo);
        if (STUB_M_DOWN == sErrorM) {
            CliMsg.sRetCode = sBRetCode;
            sMoved = 1;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

    if (sMoved == 0) {
        _tcsncpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME);
        memcpy(pCasif->fbCashierStatus, ResMsgH.fbStatus, sizeof(pCasif->fbCashierStatus));
        pCasif->ulCashierSecret = ResMsgH.ulSecretNo;
    }
    sError = CliMsg.sRetCode;    

    PifReleaseSem(husCliExeNet);
    return sError;
}
/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY SerCasAllIdRead(USHORT usRcvBufferSize,
*                                                USHORT FAR *auchRcvBuffer);
*     InPut:    usRcvBufferSize - buffer size can be read
*    OutPut:    aulRcvBuffer	- Cashier Id can be read
*
** Return:      number of cashier can be read
*
** Description:
*   This function supports Cashier All Id Read function.
*===========================================================================
*/
SHORT  CliCasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASALLIDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);
    CliMsg.pauchResData = (UCHAR *)aulRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}
/*
*===========================================================================
** Synopsis:    SHORT   CliCasClosePickupLoan(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-out
*
** Return:      CAS_PERFORM:        sign-in OK
*               CAS_LOCK:           she is still locked
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_ALREADY_OPENED: Other opened
*
** Description:
*   This function supports Cashier Sign-In for pickup/loan function.
*===========================================================================
*/
SHORT   CliCasClosePickupLoan(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sCliRetCode = 0, sErrorM = 0, sErrorBM = 0;
	SHORT			sMoved = 0;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo  = pCasif->ulCashierNo;
    ReqMsgH.usTerminalNo = pCasif->usUniqueAddress = (USHORT)CliReadUAddr();/* Set unique address */

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCLOSEPICLOA | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasClosePickupLoan(pCasif);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
        CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
		SHORT   sBRetCode = 0;
		SHORT   sRetryCo;       /* Add retry Counter, Dec/02/2000 */

        sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);
        do {                                        /* Add loop, Dec/02/2000 */
            sBRetCode = CasClosePickupLoan(pCasif);
            if (CAS_PERFORM != sBRetCode) {
                CstSleep();
            } else {
                break;
            }
        } while (0 <= --sRetryCo);
        if (STUB_M_DOWN == sErrorM) {
            CliMsg.sRetCode = sBRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		// We depend on the CstSendBMasterFH () function to handle the
		// error condition indication to the operator and the caller
		// to display an error dialog if the Master Terminal is down.
		CliMsg.sRetCode = sCliRetCode;
	}

	sCliRetCode = CliMsg.sRetCode;    

    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliCasClosePickupLoan() sCliRetCode %d  sErrorM %d  sErrorBM %d", sCliRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sCliRetCode;
}
/*
*===========================================================================
** Synopsis:    SHORT   CliCasClosePickupLoan(CASIF *pCasif);
*
*     InOut:    *pCasif  - cashier information want to sign-out
*
** Return:      CAS_PERFORM:        sign-in OK
*               CAS_LOCK:           she is still locked
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_ALREADY_OPENED: Other opened
*
** Description:
*   This function supports Cashier Sign-In for pickup/loan function.
*===========================================================================
*/
SHORT   CliCasDelayedBalance(UCHAR uchTermNo, ULONG ulState)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT           sError;
    SHORT           sCliRetCode = 0, sErrorM, sErrorBM = 0;
	SHORT			sMoved = 0;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchTerminal = uchTermNo;
	ReqMsgH.ulOption = ulState;
   
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASDELAYBALON | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        CliMsg.sRetCode = CasDelayedBalance(uchTermNo, ulState);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
		SHORT   sBRetCode = 0;            
        SHORT   sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);
        do {                                        /* Add loop, Dec/02/2000 */
            sBRetCode = CasDelayedBalance(uchTermNo, ulState);
            if (CAS_PERFORM != sBRetCode) {
                CstSleep();
            } else {
                break;
            }
        } while (0 <= --sRetryCo);
        if (STUB_M_DOWN == sErrorM) {
            CliMsg.sRetCode = sBRetCode;
        }
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		// We depend on the CstSendBMasterFH () function to handle the
		// error condition indication to the operator and the caller
		// to display an error dialog if the Master Terminal is down.
		if (0 != (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			if (CAS_PERFORM != CliMsg.sRetCode) {
				CliMsg.sRetCode = sCliRetCode;
			}
		}
		else {
			// If there is not a Backup Master then return the
			// status of the Master Terminal regardless.
			CliMsg.sRetCode = sCliRetCode;
		}
	}

	sError = CliMsg.sRetCode;    

    PifReleaseSem(husCliExeNet);
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasIndRead(CASIF FAR *pCasif);
*     InOut:    Casif   - Cashier information should be read
*
** Return:      CAS_PERFORM:    read OK
*               CAS_NOT_IN_FILE:    she is not assigned
*
** Description:
*   This function supports Cashier Individual Read function.
*===========================================================================
*/
SHORT  CliCasIndRead(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sError;
    SHORT           sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;
	SHORT			sMove = 1;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    sErrorM = CstSendMaster();

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		sMove = -1;
	}

	// If the Master Terminal is down then lets
	// try the Backup Master Terminal if we have
	// a Backup Master provisioned.
	if (sErrorM == STUB_M_DOWN) {
		CliMsg.sRetCode = CAS_NOT_ALLOWED;
		if (0 != (CliNetConfig.fchStatus & CLI_NET_BACKUP)) {
			sErrorBM = CstSendBMaster();
			sMove = 1;
			if (sErrorBM == STUB_SELF) {
				// if we are a Backup Master that has a failed Master
				CliMsg.sRetCode = CasIndRead(pCasif);
				sMove = 0;
			}
		}
	}

    if (sMove > 0) {
        pCasif->ulCashierNo			= ResMsgH.ulCashierNo;
        pCasif->ulCashierSecret		= ResMsgH.ulSecretNo;
		pCasif->usGstCheckStartNo	= ResMsgH.usStartGCN;
		pCasif->usGstCheckEndNo		= ResMsgH.usEndGCN;
		pCasif->uchChgTipRate		= ResMsgH.uchChgTipRate;
		pCasif->uchTeamNo			= ResMsgH.uchTeamNo;
		pCasif->uchTerminal			= ResMsgH.uchTerminal;
		pCasif->usSupervisorID		= ResMsgH.usSupervisorID;	//new in version 2.2.0
		pCasif->usCtrlStrKickoff	= ResMsgH.usCtrlStrKickoff;	//new in version 2.2.0
		pCasif->usStartupWindow		= ResMsgH.usStartupWindow;	//new in version 2.2.0
		pCasif->ulGroupAssociations = ResMsgH.ulGroupAssociations;	//new in version 2.2.0
	    memcpy(pCasif->fbCashierStatus, ResMsgH.fbStatus, CLI_CASHIERSTATUS);
	    _tcsncpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME);
    }

	sError = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliCasIndRead() sError %d  sErrorM %d  sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;
}
/*===== END OF SOURCE =====*/