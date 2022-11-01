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
* Title       : Client/Server PC STUB, Waiter Function Source File                       
* Category    : Client/Server PC STUB, US Hospitalty Model                                                 
* Program Name: PCSTBWAI.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               SerWaiClose()           Waiter Close
*               SerWaiAssign()          Waiter Assign
*               SerWaiDelete()          Waiter Delete
*               SerWaiSecretClr()       Waiter secret code clear
*               SerWaiAllIdRead()       Waiter all ID read
*               SerWaiIndRead()         Waiter Individual Read
*               SerWaiIndLock()         Waiter Individual Lock
*               SerWaiIndUnLock()       Waiter Individual UnLock
*               SerWaiAllLock()         Waiter all Lock
*               SerWaiAllUnLock()       Waiter all unclock
*               SerWaiIndTermLock()     Individual Terminal Lock R3.1
*               SerWaiIndTermUnlock()   Individual Terminal Unlock R3.1
*               SerWaiAllTermLock()     All Terminal Lock R3.1
*               SerWaiAllTermUnLock()   All Termianl Unlock R3.1
*               SerWaiChkSignIn()       Waiter Check sign-in waiter exit
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* May-07-96:T.Nakahata : R3.1 Initial
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

#include    <pc2170.h>

#include    <pc2172.h>

#include    <pif.h>
#include    <memory.h>
#include    "csstubin.h"
/*
*===========================================================================
** Synopsis:    USHORT  STUBENTRY SerWaiClose(USHORT usWaiterNo);
*     InPut:    usWaiterNo  - Waiter No. want to close
*
** Return:      WAI_PERFORM:    closed
*               WAI_NO_SIGNIN:  he is not sign-in      
*
** Description:
*   This function supports Waiter Close function.
*===========================================================================
*/
SHORT  STUBENTRY SerWaiClose(USHORT usWaiterNo)
{
    CLIREQWAITER    ReqMsgH;
    CLIRESWAITER    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = usWaiterNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDCLOSE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiAssign(WAIIF FAR *pWaiif);
*     InPut:    pWaiif   - Waiter information should be assigned
*
** Return:      WAI_PERFORM:    assigned
*               WAI_WAITER_FULL:    exceed number of waiter assigned     
*
** Description:
*   This function supports Waiter Assign function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiAssign(WAIIF FAR *pWaiif)
{
    CLIREQWAITER    ReqMsgH;
    CLIRESWAITER    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo  = pWaiif->usWaiterNo;
    ReqMsgH.uchSecretNo = pWaiif->uchWaiterSecret;
    ReqMsgH.fbStatus    = pWaiif->fbWaiterStatus;
    ReqMsgH.usStartGCN  = pWaiif->usGstCheckStartNo;
    ReqMsgH.usEndGCN    = pWaiif->usGstCheckEndNo;  
    memcpy(ReqMsgH.auchWaiName, pWaiif->auchWaiterName, CLI_WAITERNAME);
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiDelete(USHORT usWaiterNo);
*     InPut:    usWaiterNo  - Waiter No. want to delete
*
** Return:      WAI_PERFORM:    deleted
*               WAI_NOT_IN_FILE:    he is not assigned
*               WAI_DURING_SIGNIN:  he is still sign-in      
*
** Description:
*   This function supports Waiter Delete function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiDelete(USHORT usWaiterNo)
{
    CLIREQWAITER    ReqMsgH;
    CLIRESWAITER    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = usWaiterNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    USHORT  STUBENTRY SerWaiSecretClr(USHORT usWaiterNo);
*     InPut:    usWaiterNo  - Waiter No. want to clear her secret
*
** Return:      WAI_PERFORM:    cleared.
*               WAI_NOT_IN_FILE:    he is not assigned
*               WAI_DURING_SIGNIN:  he is still sign-in      
*
** Description:
*   This function supports Waiter Secret No clear function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiSecretClr(USHORT usWaiterNo)
{
    CLIREQWAITER    ReqMsgH;
    CLIRESWAITER    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = usWaiterNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAICLRSCODE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    USHORT  STUBENTRY SerWaiAllIdRead(USHORT usRcvBufferSize,
*                                       USHORT FAR *auchRcvBuffer);
*     InPut:    usRcvBufferSize - buffer size can be read
*    OutPut:    auchRcvBuffer   - Waiter Id can be read
*
** Return:      number of waiter can be read      
*
** Description:
*   This function supports Waiter All Id Read function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiAllIdRead(USHORT usRcvBufferSize,
                                  USHORT FAR *auchRcvBuffer)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIALLIDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);
    CliMsg.pauchResData = (UCHAR *)auchRcvBuffer;
    CliMsg.usResLen     = usRcvBufferSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiIndRead(WAIIF FAR *pWaiif);
*     InOut:    pWaiif   - Waiter information should be read
*
** Return:      WAI_PERFORM:    read OK
*               WAI_NOT_IN_FILE:    he is not assigned      
*
** Description:
*   This function supports Waiter Individual Read function.
*===========================================================================
*/      
SHORT 	STUBENTRY SerWaiIndRead(WAIIF FAR *pWaiif)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = pWaiif->usWaiterNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (WAI_PERFORM == CliMsg.sRetCode) {
        pWaiif->usWaiterNo      = ResMsgH.usWaiterNo;
        pWaiif->fbWaiterStatus  = ResMsgH.fbStatus;
        pWaiif->uchWaiterSecret = ResMsgH.uchSecretNo;
        pWaiif->usGstCheckStartNo = ResMsgH.usStartGCN;
        pWaiif->usGstCheckEndNo   = ResMsgH.usEndGCN;
        memcpy(pWaiif->auchWaiterName, ResMsgH.auchWaiName, CLI_WAITERNAME);
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    USHORT  STUBENTRY SerWaiIndLock(USHORT usWaiterNo);
*     InPut:    usWaiterNo  - Waiter No. want to lock
*
** Return:      WAI_PERFORM:    locked
*               WAI_DURING_SIGNIN:  he is still sign-in	      
*               WAI_NOT_IN_FILE:    he is not assigned
*               WAI_OTHER_LOCK:     another waiter is still locked
*
** Description:
*   This function supports Waiter Individual Lock function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiIndLock(USHORT usWaiterNo)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = usWaiterNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    USHORT  STUBENTRY SerWaiIndUnLock(USHORT usWaiterNo);
*     InPut:    usWaiterNo  - Waiter No. want to unlock
*
** Return:      WAI_PERFORM:    unlocked
*               WAI_DIFF_NO:    he is not locked      
*
** Description:
*   This function supports Waiter Individual UnLock function.
*===========================================================================
*/
SHORT   STUBENTRY SerWaiIndUnLock(USHORT usWaiterNo)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usWaiterNo = usWaiterNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiAllLock(VOID);
*
** Return:      WAI_PEFORM:     all lock OK
*               WAI_ALREADY_LOCK:   already locked
*               WAI_DURING_SIGNIN:  exist sign-in waiter      
*
** Description:
*   This function supports Waiter All Lock function.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiAllLock(VOID)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIALLLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    VOID    STUBENTRY SerWaiAllUnLock(VOID);
*
** Return:      none.      
*
** Description:
*   This function supports Waiter All UnLock function.
*===========================================================================
*/
VOID 	STUBENTRY SerWaiAllUnLock(VOID)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIALLUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();

    PifReleaseSem(husCliExeNet);
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiIndTermLock(USHORT usTerminalNo)
*     InPut:    usTerminalNo - Terminal No. want to lock
*
** Return:      WAI_PERFORM:        locked
*               WAI_DURING_SIGNIN:  she is still sign-in
*               WAI_OTHER_LOCK:     another waiter is still locked
*
** Description: This function supports Individual Terminal Lock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerWaiIndTermLock(USHORT usTerminalNo)
{
    CLIREQWAITER    ReqMsgH;
    CLIRESWAITER    ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usTerminalNo = usTerminalNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDTMLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiIndTermUnLock(USHORT usTerminalNo)
*     InPut:    usTerminalNo - Terminal No. want to unlock
*
** Return:      WAI_PERFORM:    unlocked
*               WAI_DIFF_NO:    she is not locked
*
** Description: This function supports Individual Terminal UnLock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerWaiIndTermUnLock(USHORT usTerminalNo)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    ReqMsgH.usTerminalNo = usTerminalNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIINDTMUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiAllTermLock(VOID)
*
** Return:      WAI_PERFORM:    all lock OK
*               WAI_ALREADY_LOCK:   already locked
*               WAI_DURING_SIGNIN:  exist sign-in waiter
*
** Description: This function supports All Terminal Lock Function R3.1
*===========================================================================
*/
SHORT   STUBENTRY SerWaiAllTermLock(VOID)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIALLTMLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiAllTermUnLock(VOID)
*
** Return:      none.
*
** Description: This function supports All Terminal UnLock Function R3.1
*===========================================================================
*/
VOID    STUBENTRY SerWaiAllTermUnLock(VOID)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAIALLTMUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);

    CstSendMaster();

    PifReleaseSem(husCliExeNet);
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerWaiChkSignIn(USHORT FAR *ausRcvBuffer);
*
*    OutPut:    ausRcvBuffer   - store waiter number by terminal
*
** Return:      WAI_PERFORM:    not exist sign-in waiter
*               WAI_DURING_SIGNIN:  exist sign-in waiter    
*
** Description:
*   This function supports to check that sign-in waiter exist or not.
*===========================================================================
*/
SHORT 	STUBENTRY SerWaiChkSignIn(USHORT FAR *ausRcvBuffer)
{
    CLIREQWAITER   ReqMsgH;
    CLIRESWAITER   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQWAITER));
    memset(&ResMsgH, 0, sizeof(CLIRESWAITER));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCWAICHKSIGNIN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQWAITER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESWAITER);
    CliMsg.pauchResData = (UCHAR *)ausRcvBuffer;
    CliMsg.usResLen     = 2 * CLI_ALLTRANSNO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*===== END OF SOURCE =====*/
