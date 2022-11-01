/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server PC STUB, Source File of Cashier Function
* Category    : Client/Server PC STUB, US Hospitality Mode
* Program Name: PCSTBCAS.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
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
*               SerCasIndTermUnlock()   Individual Terminal Unlock R3.1
*               SerCasAllTermLock()     All Terminal Lock R3.1
*               SerCasAllTermUnLock()   All Termianl Unlock R3.1
*               SerCasChkSignIn()       Cashier Check sign-in cashier exit
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Rel   :   Name      :     Description
* May-07-92 :       : H.Nakashima : initial
* May-07-96 :       : T.Nakahata  : R3.1 Initial
* Sep-07-98 :       : O.Nakada    : Add V3.3
* Feb-22-15 : 2.2.1 : R.Chambers  : initialize Req/Res buffers, fix memcpy() length cashier mnemonic
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
#include    <memory.h>

#include    <r20_pc2172.h>
#include    <r20_pif.h>
#include    "csstubin.h"

#include "mypifdefs.h"

/*
*===========================================================================
** Synopsis:    USHORT	STUBENTRY SerCasClose(USHORT usCashierNo);
*
*     InPut:    usCashierNo - Cashier No. want to close
*
** Return:      CAS_PERFORM:    closed
*               CAS_NO_SIGNIN:  she is not sign-in
*
** Description:
*   This function supports Cashier Close function.
*===========================================================================
*/
SHORT  STUBENTRY SerCasClose(ULONG ulCashierNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDCLOSE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasAssign(CASIF FAR *pCasif);
*
*     InPut:    pCasif   - Cashier information should be assigned
*
** Return:      CAS_PERFORM:    assigned
*               CAS_CASHIER_FULL:   exceed number of cashier assigned
*
** Description:
*   This function supports Cashier Assign function.
*===========================================================================
*/
SHORT   STUBENTRY SerCasAssign(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo   = pCasif->ulCashierNo;
    ReqMsgH.ulSecretNo   = pCasif->ulCashierSecret;
    ReqMsgH.usStartGCN    = pCasif->usGstCheckStartNo;
    ReqMsgH.usEndGCN      = pCasif->usGstCheckEndNo;
    ReqMsgH.uchChgTipRate = pCasif->uchChgTipRate;
    ReqMsgH.uchTeamNo     = pCasif->uchTeamNo;
    ReqMsgH.uchTerminal   = pCasif->uchTerminal;
	ReqMsgH.usSupervisorID = pCasif->usSupervisorID;
	ReqMsgH.usCtrlStrKickoff = 	pCasif->usCtrlStrKickoff;
	ReqMsgH.usStartupWindow  = 	pCasif->usStartupWindow;
	ReqMsgH.ulGroupAssociations = pCasif->ulGroupAssociations;
    memcpy(ReqMsgH.fbStatus, pCasif->fbCashierStatus, CLI_CASHIERSTATUS);
    memcpy(ReqMsgH.auchCasName, pCasif->auchCashierName, CLI_CASHIERNAME * sizeof(WCHAR));//Operator name Length * 2 bytes

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasDelete(USHORT usCashierNo);
*
*     InPut:    usCashierNo - Cashier No. want to delete
*
** Return:      CAS_PERFORM:    deleted
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_DURING_SIGNIN:  she is still sign-in
*
** Description:
*   This function supports Cashier Delete function.
*===========================================================================
*/
SHORT 	STUBENTRY SerCasDelete(ULONG ulCashierNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY SerCasSecretClr(USHORT usCashierNo);
*
*     InPut:    usCashierNo - Cashier No. want to clear her secret
*
** Return:      CAS_PERFORM:        cleared.
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_DURING_SIGNIN:  she is still sign-in
*
** Description:
*   This function supports Cashier Secret No clear function.
*===========================================================================
*/
SHORT  STUBENTRY SerCasSecretClr(ULONG ulCashierNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCLRSCODE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY SerCasAllIdRead(USHORT usRcvBufferSize,
*                                                USHORT FAR *auchRcvBuffer);
*     InPut:    usRcvBufferSize - buffer size can be read
*    OutPut:    auchRcvBuffer   - Cashier Id can be read
*
** Return:      number of cashier can be read
*
** Description:
*   This function supports Cashier All Id Read function.
*===========================================================================
*/
SHORT  STUBENTRY SerCasAllIdRead(USHORT usRcvBufferSize,
                                ULONG  *auchRcvBuffer)
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
    CliMsg.pauchResData = (UCHAR *)auchRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
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
SHORT   STUBENTRY SerCasIndRead(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (CAS_PERFORM == CliMsg.sRetCode) {
        pCasif->ulCashierNo       = ResMsgH.ulCashierNo;
        pCasif->ulCashierSecret  = ResMsgH.ulSecretNo;
		pCasif->usGstCheckStartNo = ResMsgH.usStartGCN;
		pCasif->usGstCheckEndNo   = ResMsgH.usEndGCN;
		pCasif->uchChgTipRate     = ResMsgH.uchChgTipRate;
		pCasif->uchTeamNo         = ResMsgH.uchTeamNo;
		pCasif->uchTerminal       = ResMsgH.uchTerminal;
	    memcpy(pCasif->fbCashierStatus, ResMsgH.fbStatus, CLI_CASHIERSTATUS);
        memcpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME * sizeof(WCHAR));
		pCasif->usSupervisorID = ResMsgH.usSupervisorID;
		pCasif->usCtrlStrKickoff = ResMsgH.usCtrlStrKickoff;
		pCasif->usStartupWindow = ResMsgH.usStartupWindow;
		pCasif->ulGroupAssociations = ResMsgH.ulGroupAssociations;
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY SerCasIndLock(USHORT usCashierNo);
*     InPut:    usCashierNo - Cashier No. want to lock
*
** Return:      CAS_PERFORM:    locked
*               CAS_DURING_SIGNIN:  she is still sign-in
*               CAS_NOT_IN_FILE:    she is not assigned
*               CAS_OTHER_LOCK:     another cashier is still locked
*
** Description:
*   This function supports Cashier Individual Lock function.
*===========================================================================
*/
SHORT 	STUBENTRY SerCasIndLock(ULONG ulCashierNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY SerCasIndUnLock(USHORT usCashierNo);
*     InPut:    usCashierNo - Cashier No. want to unlock
*
** Return:      CAS_PERFORM:    unlocked
*               CAS_DIFF_NO:    she is not locked
*
** Description:
*   This function supports Cashier Individual UnLock function.
*===========================================================================
*/
SHORT   STUBENTRY SerCasIndUnLock(ULONG ulCashierNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasAllLock(VOID);
*
** Return:      CAS_PEFORM:     all lock OK
*               CAS_ALREADY_LOCK:   already locked
*               CAS_DURING_SIGNIN:  exist sign-in cashier
*
** Description:
*   This function supports Cashier All Lock function.
*===========================================================================
*/
SHORT   STUBENTRY SerCasAllLock(VOID)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASALLLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    VOID    STUBENTRY SerCasAllUnLock(VOID);
*
** Return:      none.
*
** Description:
*   This function supports Cashier All UnLock function.
*===========================================================================
*/
VOID    STUBENTRY SerCasAllUnLock(VOID)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASALLUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();

    PifReleaseSem(husCliExeNet);
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasIndTermLock(USHORT usTerminalNo)
*     InPut:    usTerminalNo - Terminal No. want to lock
*
** Return:      CAS_PERFORM:        locked
*               CAS_DURING_SIGNIN:  she is still sign-in
*               CAS_OTHER_LOCK:     another cashier is still locked
*
** Description: This function supports Individual Terminal Lock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerCasIndTermLock(USHORT usTerminalNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.usTerminalNo = usTerminalNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDTMLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasIndTermUnLock(USHORT usTerminalNo)
*     InPut:    usTerminalNo - Terminal No. want to unlock
*
** Return:      CAS_PERFORM:    unlocked
*               CAS_DIFF_NO:    she is not locked
*
** Description: This function supports Individual Terminal UnLock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerCasIndTermUnLock(USHORT usTerminalNo)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.usTerminalNo = usTerminalNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDTMUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasAllTermLock(VOID)
*
** Return:      CAS_PERFORM:    all lock OK
*               CAS_ALREADY_LOCK:   already locked
*               CAS_DURING_SIGNIN:  exist sign-in cashier
*
** Description: This function supports All Terminal Lock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerCasAllTermLock(VOID)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASALLTMLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasAllTermUnLock(VOID)
*
** Return:      none.
*
** Description: This function supports All Terminal UnLock Function R3.1
*===========================================================================
*/
VOID    STUBENTRY SerCasAllTermUnLock(VOID)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASALLTMUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();

    PifReleaseSem(husCliExeNet);
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasChkSignIn(USHORT FAR *ausRcvBuffer);
*    OutPut:    auchRcvBuffer   - store cashier number by terminal
*
** Return:      CAS_PERFORM:    not exist sign-in cashier
*               CAS_DURING_SIGNIN:  exist sign-in cashier
*
** Description:
*   This function supports to check that sign-in cashier exist or not.
*===========================================================================
*/
SHORT   STUBENTRY SerCasChkSignIn(ULONG  *ausRcvBuffer)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCHKSIGNIN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);
    CliMsg.pauchResData = (UCHAR *)ausRcvBuffer;
    CliMsg.usResLen     = sizeof(*ausRcvBuffer) * CLI_ALLTRANSNO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasOpenPickupLoan(CASIF FAR *pCasif);
*
*     InPut:    pCasif   - Cashier information should be assigned
*
** Return:      CAS_PERFORM:       assigned
*               CAS_NOT_IN_FILE:   not cashier assigned
*               CASALREADY_OPENED: already opened by other user
*
** Description:
*   This function supports to open cashier at Pickup/Loan function.
*===========================================================================
*/
SHORT   STUBENTRY SerCasOpenPickupLoan(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASOPENPICLOA;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (CAS_PERFORM == CliMsg.sRetCode) {
        pCasif->ulCashierNo      = ResMsgH.ulCashierNo;
		// copy the entire cashier status flag array rather than just
		// the first two entries of that array.
		// not sure why only first two bytes were being copied but there
		// is more status information available.
		// Richard Chambers, Apr-24-2020
//		pCasif->fbCashierStatus[0] = ResMsgH.fbStatus[0];
//		pCasif->fbCashierStatus[1] = ResMsgH.fbStatus[1];
		memcpy(pCasif->fbCashierStatus, ResMsgH.fbStatus, CLI_CASHIERSTATUS);
        pCasif->ulCashierSecret = ResMsgH.ulSecretNo;
        memcpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME * sizeof(WCHAR));
        CliNetConfig.usCliTranNo = 1;
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerCasClosePickupLoan(CASIF FAR *pCasif);
*
*     InPut:    pCasif   - Cashier information should be assigned
*
** Return:      CAS_PERFORM:       assigned
*               CAS_NOT_IN_FILE:   not cashier assigned
*
** Description:
*   This function supports to close Cashier at pickup/Loan function.
*===========================================================================
*/
SHORT   STUBENTRY SerCasClosePickupLoan(CASIF *pCasif)
{
    CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNo = pCasif->ulCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCLOSEPICLOA;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

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
SHORT STUBENTRY SerCasDelayedBalance(UCHAR uchTermNo)
{
	CLIREQCASHIER   ReqMsgH = {0};
    CLIRESCASHIER   ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchTerminal = uchTermNo;
   
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASDELAYBALON + CLI_FCWBACKUP + CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);
 
	CstSendMaster();
	
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}
/*===== END OF SOURCE =====*/
