/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server PC STUB, ISP Function Source File
* Category    : Client/Server PC STUB, US Hospitality Model
* Program Name: PCSTBISP.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               IspLogOn()              Log On
*               IspChangePassWord()     Change Password
*               IspLockKeyBoard()       Lock Terminal K/B
*               IspUnLockKeyBoard()     Unlock Terminal K/B
*               IspLogOff()             Log Off
*               IspCheckTotals()        Check existing total
*               IspCheckAsMaster()      Check as mster
*               IspReqBroadcast()       Broadcast request         Add R3.1
*               IspNoOfSatellite()      Read Number of Satellite  Add R3.1
*               IspDateTimeRead()       Add V3.3
*               IspDateTimeWrite()      Add V3.3
*               IspSysConfig()          Add V3.3
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Mar-14-96:M.Suzuki   : Add R3.1                                   
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
** Synopsis:    SHORT  STUBENTRY IspLogOn(UCHAR FAR *puchIPAddr, 
*                                         UCHAR FAR *puchPassWord);
*
*     Input:    puchIPAddr      - IP Adress
*               puchPassWord    - pointer of Pass word
*
** Return:       0:  Log On Succcess
*               <0:  Log On Fail
*
** Description:
*   This function executes to try log on.
*===========================================================================
*/
SHORT   STUBENTRY IspLogOn(UCHAR FAR *puchIPAddr, UCHAR FAR *puchPassWord)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);

    if (CliNetConfig.fchStatus & CLI_NET_OPEN) {
        CstNetClose();
    }

    memcpy(&CliNetConfig.auchFaddr[0], puchIPAddr, PIF_LEN_IP);
    CstNetOpen();

    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memcpy(ReqMsgH.auchPassWord, puchPassWord, CLI_PASSWORD);
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPLOGON;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if (ISP_SUCCESS != CliMsg.sRetCode) {
        CstNetClose();                  /* close network for fail */
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}    


/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY IspChangePassWord(UCHAR FAR *puchPassWord);
*
*     Input:    puchPassWord    - pointer of Pass word
*
** Return:       0:  Changing Succcessful
*               <0:  Changing Fail
*
** Description:
*   This function executes to change the pass word
*===========================================================================
*/
SHORT   STUBENTRY IspChangePassWord(UCHAR FAR *puchPassWord)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memcpy(ReqMsgH.auchPassWord, puchPassWord, CLI_PASSWORD*sizeof(WCHAR));
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPCHANGEPASS;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspLockKeyBoard(VOID);
*
** Return:       0:  Lock Success
*               <0:  error
*
** Description:
*   This function executes to lock terminla key board
*===========================================================================
*/
SHORT   STUBENTRY IspLockKeyBoard(VOID)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPLOCKKEYBD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspUnLockKeyBoard(VOID);
*
** Return:       0:  Un Lock Succcess
*               <0:  Un Lock Fail
*
** Description:
*   This function executes to unlock key board.
*===========================================================================
*/
SHORT   STUBENTRY IspUnLockKeyBoard(VOID)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPUNLOCKKEYBD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspLogOff(VOID);
*
** Return:       0:  Log Off Succcess
*               <0:  Log Off Fail
*
** Description:
*   This function executes to try log off.
*===========================================================================
*/
SHORT   STUBENTRY IspLogOff(VOID)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPLOGOFF;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
    CstNetClose();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspCheckTotals(SHORT fsCheckTotal);
*
*     Input:    fsCheckTotal    - Bit for totals to be checked
*
** Return:      plus or 0   Success, existing total bit will be ON.
*               minus       fail
*
** Description:
*   This function executes to check existing total.
*===========================================================================
*/
SHORT   STUBENTRY IspCheckTotals(SHORT fsCheckTotal)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    ReqMsgH.fsCheckTotal = fsCheckTotal;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPCHKTOTALS;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspCheckAsMaster(VOID)
*
*     Input:    nothing
*
** Return:      ISP_WORK_AS_MASTER      Works as Master
*               ISP_WORK_AS_SATELLITE   Works as satellite
*               0 > Response            Failed
*
** Description:
*   This function executes to check existing total.
*===========================================================================
*/
SHORT   STUBENTRY IspCheckAsMaster(VOID)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPCHKTERM;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspCheckModeOnTerm(VOID)
*
*     Input:    nothing
*
** Return:      ISP_WORK_AS_MASTER      Works as Master
*               ISP_WORK_AS_SATELLITE   Works as satellite
*               0 > Response            Failed
*
** Description:
*   This function executes to check existing total.
*===========================================================================
*/
SHORT   STUBENTRY IspCheckModeOnTerm(VOID)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPCHKMODE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}


/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspReqBroadcast(UCAHR uchNbFlag);
*
*     Input:    uchNbFlag   - Bit for broadcast type
*
** Return:      plus or 0   Success, existing total bit will be ON.
*               minus       fail
*
** Description:
*   This function executes to broadcast request.
*===========================================================================
*/
SHORT   STUBENTRY IspReqBroadcast(UCHAR uchNbFlag)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    ReqMsgH.uchNbFlag = uchNbFlag;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPBROADCAST;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY IspNoOfSatellite(UCAHR FAR *puchNoOfSat);
*
*     Output:   puchNbFlag   - pointer ofNumber of satellite
*
** Return:      plus or 0   Success, existing total bit will be ON.
*               minus       fail
*
** Description:
*   This function executes to read number of satellite.
*===========================================================================
*/
SHORT   STUBENTRY IspNoOfSatellite(UCHAR FAR *puchNoOfSat)
{
    CLIREQISP   ReqMsgH;
    CLIRESISP   ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPSATELLITE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    *puchNoOfSat = ResMsgH.uchNoOfSat;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY IspDateTimeRead(CLIDATETIME FAR *pDateTime);
*     Input:    pDateTime   - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:       0:  Succcess
*               <0:  Fail
*
** Description: Read Date/Time
*===========================================================================
*/
SHORT   STUBENTRY IspDateTimeRead(CLIDATETIME FAR *pDateTime)
{
    CLIREQISPTOD   ReqMsgH;
    CLIRESISPTOD   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISPTOD));
    memset(&ResMsgH, 0, sizeof(CLIRESISPTOD));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPTODREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISPTOD);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISPTOD);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    memcpy(pDateTime, &ResMsgH.DateTime, sizeof(CLIDATETIME));

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY IspDateTimeWrite(CLIDATETIME FAR *pDateTime);
*     Input:    pDateTime - Point a write buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:       0:  Succcess
*               <0:  Fail
*
** Description: Set Date/Time
*===========================================================================
*/
SHORT   STUBENTRY IspDateTimeWrite(CLIDATETIME FAR *pDateTime)
{
    CLIREQISPTOD   ReqMsgH;
    CLIRESISPTOD   ResMsgH;
	SHORT		   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISPTOD));
    memset(&ResMsgH, 0, sizeof(CLIRESISPTOD));

    memcpy(&ReqMsgH.DateTime, pDateTime, sizeof(CLIDATETIME));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPTODWRITE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISPTOD);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISPTOD);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY IspSysConfig(SYSCONFIG FAR *pSysPara);
*     Input:    pSysPara - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:       0:  Succcess
*               <0:  Fail
*
** Description: Read System Parameter
*===========================================================================
*/
SHORT   STUBENTRY IspSysConfig(CLISYSPARA FAR *pSysPara)
{
    CLIREQISP           ReqMsgH;
    CLIRESISPSYSPARA    ResMsgH;
	SHORT				sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISP));
    memset(&ResMsgH, 0, sizeof(CLIRESISP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPSYSCONF;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISP);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESISPSYSPARA);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    memcpy(pSysPara, &ResMsgH.SysPara, sizeof(CLISYSPARA));

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY IspRecvResetLog(UCHAR   uchAction);
*     Input:    uchAction - Action code
*     Output:   nothing
*     InOut:    nothing
*
** Return:       0:  Succcess
*               <0:  Fail
*
** Description: ISP Print Reset Log, R2.0GCA
*===========================================================================
*/
SHORT   STUBENTRY IspRecvResetLog(UCHAR   uchAction)
{
    CLIREQISPLOG           ReqMsgH;
	SHORT				   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISPLOG));

    ReqMsgH.uchAction   = uchAction;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCISPRESETLOG;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQISPLOG);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT STUBENTRY IspRecvResetLog2(UCHAR FAR *pszMnemonic);
*     Input:    pszMnemonic     -   pointer of charactor string
*     Output:   nothing
*     InOut:    nothing
*
** Return:       0:  Succcess
*               <0:  Fail
*
** Description: ISP Print Reset Log by Mnemonic. V2.1
*===========================================================================
*/

SHORT   STUBENTRY   IspRecvResetLog2(UCHAR FAR *pszMnemonic)
{
    CLIREQISPLOG2   ReqMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQISPLOG2));

    memcpy(&ReqMsgH.aszMnemonic, pszMnemonic, CLI_ISPLOG_LEN);
    
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    =   CLI_FCISPRESETLOG2;
    CliMsg.pReqMsgH     =   (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen =   sizeof(CLIREQISPLOG2);
    
    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*===== END OF SOURCE =====*/
