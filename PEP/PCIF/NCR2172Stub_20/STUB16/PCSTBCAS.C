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
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Large Model
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp
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
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial
* May-07-96:T.Nakahata : R3.1 Initial
* Sep-07-98:O.Nakada   : Add V3.3
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <pc2172.h>

#include    <pif.h>
#include    <memory.h>
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
SHORT  STUBENTRY SerCasClose(USHORT usCashierNo)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = usCashierNo;

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
SHORT   STUBENTRY SerCasAssign(CASIF FAR *pCasif)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo   = pCasif->usCashierNo;
    ReqMsgH.uchSecretNo   = pCasif->uchCashierSecret;
    ReqMsgH.usStartGCN    = pCasif->usGstCheckStartNo;
    ReqMsgH.usEndGCN      = pCasif->usGstCheckEndNo;
    ReqMsgH.uchChgTipRate = pCasif->uchChgTipRate;
    ReqMsgH.uchTeamNo     = pCasif->uchTeamNo;
    ReqMsgH.uchTerminal   = pCasif->uchTerminal;
    memcpy(ReqMsgH.fbStatus, pCasif->fbCashierStatus, CLI_CASHIERSTATUS);
    memcpy(ReqMsgH.auchCasName, pCasif->auchCashierName, CLI_CASHIERNAME * 2); //Length of Operator Name * 2 bytes

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
SHORT 	STUBENTRY SerCasDelete(USHORT usCashierNo)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = usCashierNo;

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
SHORT  STUBENTRY SerCasSecretClr(USHORT usCashierNo)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = usCashierNo;

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
                                USHORT FAR *auchRcvBuffer)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
SHORT   STUBENTRY SerCasIndRead(CASIF FAR *pCasif)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = pCasif->usCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (CAS_PERFORM == CliMsg.sRetCode) {
        pCasif->usCashierNo       = ResMsgH.usCashierNo;
        pCasif->uchCashierSecret  = ResMsgH.uchSecretNo;
		pCasif->usGstCheckStartNo = ResMsgH.usStartGCN;
		pCasif->usGstCheckEndNo   = ResMsgH.usEndGCN;
		pCasif->uchChgTipRate     = ResMsgH.uchChgTipRate;
		pCasif->uchTeamNo         = ResMsgH.uchTeamNo;
		pCasif->uchTerminal       = ResMsgH.uchTerminal;
	    memcpy(pCasif->fbCashierStatus, ResMsgH.fbStatus, CLI_CASHIERSTATUS);
        memcpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME);
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
SHORT 	STUBENTRY SerCasIndLock(USHORT usCashierNo)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = usCashierNo;

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASCHKSIGNIN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);
    CliMsg.pauchResData = (UCHAR *)ausRcvBuffer;
    CliMsg.usResLen     = 2 * CLI_ALLTRANSNO;

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
SHORT   STUBENTRY SerCasOpenPickupLoan(CASIF FAR *pCasif)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	  SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = pCasif->usCashierNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASOPENPICLOA;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    CstSendMaster();
	  sRetCode = CliMsg.sRetCode;

    if (CAS_PERFORM == CliMsg.sRetCode) {
        pCasif->usCashierNo      = ResMsgH.usCashierNo;
//        pCasif->fbCashierStatus  = ResMsgH.fbStatus;
		pCasif->fbCashierStatus[0] = ResMsgH.fbStatus[0];
		pCasif->fbCashierStatus[1] = ResMsgH.fbStatus[1];
        pCasif->uchCashierSecret = ResMsgH.uchSecretNo;
        memcpy(pCasif->auchCashierName, ResMsgH.auchCasName, CLI_CASHIERNAME);
        usCliTranNo = 1;
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
SHORT   STUBENTRY SerCasClosePickupLoan(CASIF FAR *pCasif)
{
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
	  SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.usCashierNo = pCasif->usCashierNo;

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
    CLIREQCASHIER   ReqMsgH;
    CLIRESCASHIER   ResMsgH;
    SHORT           sError, sBRetCode;
	SHORT			sMoved = 0;
    SHORT           sRetryCo;       /* Add retry Counter, Dec/02/2000 */

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQCASHIER));
    memset(&ResMsgH, 0, sizeof(CLIRESCASHIER));

    ReqMsgH.uchTerminal = uchTermNo;
   
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCCASDELAYBALON + CLI_FCWBACKUP + CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASHIER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASHIER);

    if (STUB_SELF == (sError = CstSendMaster())) {
        CliMsg.sRetCode = CasDelayedBalance(uchTermNo);
    }
    if (CAS_PERFORM == CliMsg.sRetCode) {
        if (STUB_SELF == CstSendBMaster()) {
            sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);
            do {                                        /* Add loop, Dec/02/2000 */
                sBRetCode = CasDelayedBalance(uchTermNo);
                if (CAS_PERFORM != sBRetCode) {
                    CstSleep();
                } else {
                    break;
                }
            } while (0 <= --sRetryCo);
            if (STUB_M_DOWN == sError) {
                CliMsg.sRetCode = sBRetCode;
            } else if (sBRetCode < 0) {
                CstBackUpError();
            }
        }
    }
    sError = CliMsg.sRetCode;    

    PifReleaseSem(husCliExeNet);
    return sError;
}
/*===== END OF SOURCE =====*/
