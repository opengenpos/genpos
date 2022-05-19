/*
		Copyright, 2002-2015
					Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
			James Hall				Russell Herrington
			Ernest Smith			Chris Wun
			Cody Reneman			Chris Jaynes
			Ailan Chu				Richard Chambers
			Chris Hattrich			Chris Small
*===========================================================================
* Title       : ISP Server, ISP Unique Function Source File                        
* Category    : ISP Server, 2170 US Hospitality Model
* Program Name: ISPPASS.C                                            
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
* Abstruct: The provided function names are as follows:  
*
*           IspRecvPass()           Entry point of Password Function
*
*           IspRecvPassLogOn()      Receive Logon password    
*           IspSendPassResponse()   Send    Password function response
*           IspRecvPassChange()     Receive Change password
*           IspRecvPassLockKB()     Receive Lock Key Board
*           IspRecvPassUnlockKB()   Receive Unlock Key Board 
*           IspRecvPassLogOff()     Receive Log Off
*           IspRecvPassCheckTtl()   Receive Check totals 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-23-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-23-92:00.00.02:T.Kojima   : Add to call "RmtInitFileTbl()"  and 
*          :        :           : "RmtCloseAllFile()" in "IspRecvPassLogOn()"                                   
* Nov-10-92:00.00.02:H.Yamaguchi: Add to call "IspRecvPassCheckTtl()"  
* Jun-03-93:00.00.03:H.Yamaguchi: Modified "IspRecvPassLockKB"  
* Aug-23-93:00.00.03:H.Yamaguchi: Add "CLI_FCISPCHKTERM"  function
* Jun-01-95:02.05.04:M.Suzuki   : Add Coupon  function
* Aug-15-98:03.03.00:M.Ozawa    : Add GP R2.0 functions (Date/Time, Sysconf, Log)
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
*
** NCR2171 **
* Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
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
#include	<stdio.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <rfl.h>
#include    <uie.h>
#include    <fsc.h>
#include    <log.h>
#include    <nb.h>
#include    <uic.h>
#include    <paraequ.h>
#include    <para.h>
#include	<pifmain.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <csstbstb.h>
#include    <appllog.h>
#include    "rmt.h"         /* remote file sysytem  */
#include    <pmg.h> 
#include    "BlFWif.h"


/*
*===========================================================================
** Synopsis:    VOID    IspRecvPass(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function is a entry of all pasword function.
*===========================================================================
*/
VOID    IspRecvPass(VOID)
{
    CLIREQISP      *pReqMsgH = (CLIREQISP *)IspRcvBuf.auchData;
    SHORT          sError;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case CLI_FCISPLOGON :        /* ISP Log on */
        IspRecvPassLogOn(pReqMsgH);     
        break;

    case CLI_FCISPCHANGEPASS :   /* ISP Change password */
        IspRecvPassChange(pReqMsgH);
        break;

    case CLI_FCISPLOCKKEYBD :    /* ISP Lock key board */
        IspRecvPassLockKB();
        break;

    case CLI_FCISPUNLOCKKEYBD :  /* ISP Unlock key board */
        IspRecvPassUnlockKB();
        break;

    case CLI_FCISPLOGOFF :       /* ISP Log off */
        IspRecvPassLogOff();
        break;

    case CLI_FCISPCHKTOTALS :   /* Check totals */
        IspRecvPassCheckTtl(pReqMsgH->fsCheckTotal);
        break;

    case CLI_FCISPCHKTERM :   /* Check terminal */
        sError = IspAmISatellite(); /* Get current terminal condition */
        if ( sError == ISP_SUCCESS ) {
            if (ISP_IAM_BMASTER) {  /* if I am B-Master, then next */
                sError = ISP_WORK_AS_BMASTER;
            } else {
                sError = ISP_WORK_AS_SATELLITE;
            }
        } else {
            sError = ISP_WORK_AS_MASTER;
        }
        IspSendPassResponse(sError);            /* Send ?? response */
        break;

    /********************** Add R3.1 ****************************************/
    case CLI_FCISPBROADCAST :        /* ISP Broadcast */
        IspRecvPassBroadcast(pReqMsgH);     
        break;

    case CLI_FCISPSATELLITE :        /* ISP Read No. of Satellite */
        IspRecvPassSatellite();     
        break;
    /********************** End Add R3.1 ************************************/

    case CLI_FCISPTODREAD:          /* Read Date/Time (V3.3)*/
		{
			CLIREQISPTOD   *pReqTod = (CLIREQISPTOD *)IspRcvBuf.auchData;
			IspRecvDateTimeRead(pReqTod);
		}
        break;

    case CLI_FCISPTODWRITE:         /* Write Date/Time (V3.3)*/
		{
			CLIREQISPTOD   *pReqTod = (CLIREQISPTOD *)IspRcvBuf.auchData;
			IspRecvDateTimeWrite(pReqTod);
		}
        break;

    case CLI_FCISPSYSCONF:          /* Read System Parameter (V3.3)*/
        IspRecvSysConfig(pReqMsgH);
        break;

    case CLI_FCISPRESETLOG:         /* ISP Print Reset Log, V3.3 */
		{
			CLIREQISPLOG   *pReqLog = (CLIREQISPLOG *)IspRcvBuf.auchData;
			IspRecvResetLog(pReqLog);
		}
        break;

    case CLI_FCISPRESETLOG2:         /* ISP Print Reset Log, V3.3 */
		{
			CLIREQISPLOG2  *pReqLog2 = (CLIREQISPLOG2 *)IspRcvBuf.auchData;
			IspRecvResetLog2(pReqLog2);
		}
        break;

    case CLI_FCISPRESETLOG3:         /* ISP Print Reset Log, V3.3 */
		{
			CLIREQISPLOG3  *pReqLog3 = (CLIREQISPLOG3 *)IspRcvBuf.auchData;
			IspRecvResetLog3(pReqLog3);
		}
        break;

	case CLI_FCISPCHKMODE:			/* ISP Send Current Mode on Terminal */
		IspSendModeResponse();
		break;

    default:                                    /* not used */
        IspSendError(ISP_ERR_INVALID_MESSAGE);  /* Error response */
        break;
    }
}

struct {
	CLIREQISP  LastLogOn;
	UCHAR      auchFaddr[PIF_LEN_IP];
} IspRecvLogOnVersionInfo;

ISPLOGONVERSION  IspRecvLogonVersion (ISPLOGONVERSION *pVersion)
{
	ISPLOGONVERSION  myVersion;

	if (pVersion == 0) pVersion = &myVersion;

	pVersion->uchMajorVer = IspRecvLogOnVersionInfo.LastLogOn.uchNbFlag;
	pVersion->uchMinorVer = ((IspRecvLogOnVersionInfo.LastLogOn.fsCheckTotal & 0xf000) >> 12);
	pVersion->uchIssueVer = ((IspRecvLogOnVersionInfo.LastLogOn.fsCheckTotal & 0x0f00) >> 8);
	pVersion->uchBuildVer = (IspRecvLogOnVersionInfo.LastLogOn.fsCheckTotal & 0x00ff);

	return *pVersion;
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvPassLogOn(CLIREQISP  *pReqMsgH);
*     Input:    pReqMsgH  - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Execute Log on password function.
*===========================================================================
*/
VOID    IspRecvPassLogOn(CLIREQISP  *pReqMsgH)
{
    SHORT       sError;
	TCHAR       auchPassWord[CLI_PASSWORD+1] = {0};
   
    CliParaAllRead(CLASS_PARAPCIF, (UCHAR*)auchPassWord, CLI_PASSWORD * sizeof(TCHAR), 0, &sError);

    sError = _tcsncmp(pReqMsgH->auchPassWord, auchPassWord, CLI_PASSWORD);  /* Check Password */
    if ( 0 == sError ) {             /* if password is OK ! */
        IspChangeStatus(ISP_ST_NORMAL);        /* Change state to normal */
        CliDispDescriptor(CLI_MODULE_ISP, CLI_DESC_START_BLINK) ; /* Blink " COMM " */

        RmtCloseAllFile();          /* close all files              */
        RmtInitFileTbl();           /* initialize remote file table */
        
        sError = IspAmISatellite(); /* Get current terminal condition */

        if ( ISP_SUCCESS == sError ) {   /* if I am Master, then checks next */
            fsIspCleanUP &= ~ISP_WORKS_ASMASTER;      /* Reset condition  */
        } else {
            fsIspCleanUP |= ISP_WORKS_ASMASTER;       /* Set to work as Master  */
        }

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_REMOTE_LOGIN);                 /* Write log          */
		{
			char xBuff[128];
			sprintf (xBuff, "IspRecvPassLogOn() Login from remote %d.%d.%d.%d",
					IspRcvBuf.auchFaddr[0], IspRcvBuf.auchFaddr[1], IspRcvBuf.auchFaddr[2], IspRcvBuf.auchFaddr[3]);
			NHPOS_NONASSERT_TEXT(xBuff);
		}

		IspNetSynchSeqNo();                    // Synchronize sequence number with remote application 
        sError = ISP_SUCCESS;                  /* Set OK status */
    }  else {
		char xBuff[128];

        sError = ISP_ERR_NOTMATCH_PASSWORD;    /* Does not match password  */
		sprintf (xBuff, "IspRecvPassLogOn() Login from remote %d.%d.%d.%d failed - Password mismatch.",
				IspRcvBuf.auchFaddr[0], IspRcvBuf.auchFaddr[1], IspRcvBuf.auchFaddr[2], IspRcvBuf.auchFaddr[3]);
		NHPOS_NONASSERT_TEXT(xBuff);
    }

	// With GenPOS Rel 2.2.1.141 we are introducing the ability for GenPOS to use an Ack/Nak protocol
	// when communicating with a remote application.  We have seen some field issues from VCS, a large
	// enterprise style customer with multiple sites installed within an enterprise style LAN.
	// Due to the nature of the equipment and provisioning used, we are seeing network communication errors
	// in which some messages from GenPOS to the remote application are being dropped.
	IspNetConfig.fchStatus &= ~ISP_NET_USING_ACKNAK;   /* clear using Ack/Nak indicator in case earlier version of PCIF */
	IspRecvLogOnVersionInfo.LastLogOn = *pReqMsgH;
	memcpy(IspRecvLogOnVersionInfo.auchFaddr, IspRcvBuf.auchFaddr, sizeof(IspRecvLogOnVersionInfo.auchFaddr));

	if (sError == ISP_SUCCESS && pReqMsgH->uchNbFlag != 0) {
		// beginning with GenPOS Rel 2.2.1.141 we have inserted into the PCIF Third Party toolkit a modification
		// to the network layer so that if a device requests an Ack then an Ack will be sent when a message
		// is received.  We have also implemented a version identification beginning with GenPOS Rel 2.2.1.141
		// in the Logon message.  The PCIF will now put a version number into fields of the Logon request message
		// which previously had been zeroed out. See function IspVersionInfoGet() in PCIF DLL file R20_PCSTBISP.C
		IspNetConfig.fchStatus |= ISP_NET_USING_ACKNAK;   /* if using Ack/Nak then use Ack/Nak when sending */
	}
    IspSendPassResponse(sError);               /* Send ?? response */

    IspResetLog.uchStatus = 0;                 /* FVT#5 */
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendPassResponse(SHORT sReturn);
*     Input:    sReturn  - Return code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Send logon password response to ISP.
*===========================================================================
*/
VOID    IspSendPassResponse(SHORT sReturn)
{
    CLIREQISP      *pReqMsgH = (CLIREQISP *)IspRcvBuf.auchData;
	CLIRESISP      ResMsgH = {0};
   
    ResMsgH.usFunCode   = pReqMsgH->usFunCode;
    ResMsgH.sResCode    = STUB_SUCCESS;
    ResMsgH.usSeqNo     = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sReturn     = sReturn;
    
    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESISP), NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendModeResponse(VOID);
*     Input:    sReturn  - Return code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Send mode response to ISP.
*               The mode indicates if the terminal is in Supervisor Mode,
*               Register Mode, Program Mode, etc.
*
*               See the defines
*                   UIE_POSITION2        lock mode
*                   UIE_POSITION3        register mode
*                   UIE_POSITION4        supervisor mode
*                   UIE_POSITION5        program mode
*===========================================================================
*/
VOID	IspSendModeResponse(VOID)
{
    CLIREQISP   *pReqMsgH = (CLIREQISP *)IspRcvBuf.auchData;
	CLIRESISP   ResMsgH = {0};
   
    ResMsgH.usFunCode   = pReqMsgH->usFunCode;
    ResMsgH.sResCode    = STUB_SUCCESS;
    ResMsgH.usSeqNo     = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sReturn     = UieReadMode();
    
    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESISP), NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendSystemStatusResponse(VOID);
*     Input:    sReturn  - Return code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Send system status as reported by IspComReadStatus().
*===========================================================================
*/
VOID	IspSendSystemStatusResponse(VOID)
{
    CLIREQSYSTEMSTATUS      *pReqMsgH = (CLIREQSYSTEMSTATUS *)IspRcvBuf.auchData;
	CLIRESSYSTEMSTATUS      ResMsgH = {0};
   
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.fsComStatus   = IspComReadStatus();
    
    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESSYSTEMSTATUS), NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvPassChange(CLIREQISP   *pReqMsgH);
*     Input:    pReqMsgH  - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Change password function and generate logs that the password
*               was changed in ASSRTLOG, PIFLOG, and Electronic Journal.
*===========================================================================
*/
VOID    IspRecvPassChange(CLIREQISP   *pReqMsgH)
{
    USHORT      usDataLen;
   
    CliParaAllWrite(CLASS_PARAPCIF, (UCHAR*)pReqMsgH->auchPassWord, CLI_PASSWORD*sizeof(WCHAR), 0, &usDataLen);

	NHPOS_NONASSERT_NOTE("==NOTE", "==NOTE: IspRecvPassChange() Password changed.");

    PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_PASSWORD_CHANGE);                 /* Write log          */

    memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
    _tcscat(IspResetLog.aszMnemonic, _T("PSWD CHNG"));
    IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGWITHTRAILER, 2);

    IspSendPassResponse(ISP_SUCCESS);            /* Send OK response */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvPassLockKB(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Lock Terminal function.
*===========================================================================
*/
VOID    IspRecvPassLockKB(VOID)
{
    SHORT      sError = ISP_SUCCESS;
	SHORT      sRetValue = ISP_SUCCESS;
	USHORT     fsIspCleanUPSave = fsIspCleanUP;
	SHORT      hsIspKeyBoardHandSave = hsIspKeyBoardHand;
   
    if ( hsIspKeyBoardHand == 0 ) {            
		// check and set the keyboard and transaction lock.  this lock is used
		// to ensure that Cashier can not Sign-in if there is a remote application
		// that has locked the keyboard for actions that must not be done in
		// parallel to Cashier actions.
        sRetValue = sError = UicCheckAndSet();                    /* Request keyboard lock */
        if ( 0 < sError )  {
            hsIspKeyBoardHand = sError ;                /* Saves Terminal Handle  */
            fsIspCleanUP = (ISP_LOCKED_KEYBOARD | ISP_REQUEST_CLEANUP);     /* Set cleanup flag   */
            PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_LOCK_KEYBOARD);      /* Write log          */
            IspSetNbStopFunc();                       /* Set Stop all function flag */
			PifSleep(500);                            // Give Notice Board time to broadcast the status change
			sRetValue = ISP_SUCCESS;
        }    
    }

	{
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: IspRecvPassLockKB() sError %d  hsIspKeyBoardHand %d  %d  fsIspCleanUP 0x%x", sError, hsIspKeyBoardHandSave, hsIspKeyBoardHand, fsIspCleanUPSave);
		NHPOS_ASSERT_TEXT((0 <= sError), xBuff);
	}

    IspSendPassResponse(sRetValue);                  /* Send response   */

}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvPassUnlockKB(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Unlock Terminal function.
*===========================================================================
*/
VOID    IspRecvPassUnlockKB(VOID)
{
	const char *pErrorFmt = "==WARNING: IspRecvPassUnlockKB() sError %d  hsIspKeyBoardHand %d  %d  fsIspCleanUP 0x%x";
    SHORT      sError = ISP_SUCCESS;
	USHORT     fsIspCleanUPSave = fsIspCleanUP;
	SHORT      hsIspKeyBoardHandSave = hsIspKeyBoardHand;
	char       xBuff[128];

    if ( hsIspKeyBoardHand != 0 ) {
        sError = IspCleanUpLockKB();              /* Unlock KeyBoard */
		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_UNLOCK_KEYBOARD);      /* Write log          */
        if ( sError != UIC_SUCCESS)  {
            IspSendPassResponse(sError);          /* Send error response */
			sprintf (xBuff, pErrorFmt, sError, hsIspKeyBoardHandSave, hsIspKeyBoardHand, fsIspCleanUPSave);
			NHPOS_ASSERT_TEXT((sError == UIC_SUCCESS), xBuff);
            IspAbort(FAULT_INVALID_HANDLE);       /* Detects invalid handle */
        }    
		PifSleep(500);                            // Give Notice Board time to broadcast the status change
    }

	{
		sprintf (xBuff, pErrorFmt, sError, hsIspKeyBoardHandSave, hsIspKeyBoardHand, fsIspCleanUPSave);
		NHPOS_ASSERT_TEXT((0 <= sError), xBuff);
	}

    IspSendPassResponse(sError);                  /* Send OK response */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvPassLogOff(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Log off function.
*===========================================================================
*/
VOID    IspRecvPassLogOff(VOID)
{
	IspNetConfig.fchStatus &= ~ISP_NET_USING_ACKNAK;   /* clear using Ack/Nak indicator with logout request from remote application */
	{
		CLIREQISP  LastLogOn = {0};
		IspRecvLogOnVersionInfo.LastLogOn = LastLogOn;  // clear the version information
	}
	IspSendPassResponse(ISP_SUCCESS);    /* Send OK response */
    IspCleanUpSystem();                  /* Cleanup keyboard/PLU sub index */ 
    IspCleanUpLockFun();                 /* Cleanup Locked function */
    IspChangeStatus(ISP_ST_PASSWORD);    /* Change state to PASSWORD */

    /* --- V3.3 FVT#5 --- */
    if (IspResetLog.uchStatus & ISP_RESETLOG_LOGOFF) {
        IspRecvResetLogInternalNote(UIF_UCHRESETLOG_TRAILERONLY, 1);             /* Trailer Print */
        IspResetLog.uchStatus = 0;
    }

    PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_REMOTE_LOGOFF);                 /* Write log          */
	{
		char xBuff[128];
		sprintf (xBuff, "IspRecvPassLogOff() Log off remote %d.%d.%d.%d",
				IspRcvBuf.auchFaddr[0], IspRcvBuf.auchFaddr[1], IspRcvBuf.auchFaddr[2], IspRcvBuf.auchFaddr[3]);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

    /* --- V3.3 FVT#5 --- */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvPassCheckTtl(SHORT fsCheckTotal);
*     Input:    fsCheckTotal 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Check the total buckets as specified in the bit map of
*               the fsCheckTotal argument to determine which totals are in
*               a reset state and so can not be processed until the Saved
*               totals have been read.
*===========================================================================
*/
VOID    IspRecvPassCheckTtl(SHORT fsCheckTotal)
{
    SHORT   sRet;
    SHORT   sError;

    sRet = ISP_TTL_RESET_FLAG & fsCheckTotal;

    sError = IspAmISatellite(); /* Get current terminal condition */
    if (ISP_SUCCESS == sError ) {
        IspSendPassResponse(0);     /* Send OK Response */
        return;
    }

    if ( ISP_TTL_DEPARTMENT & fsCheckTotal ) {
        sError = IspCheckDegTtl(FLEX_DEPT_ADR);   /* Check totals */
        if ( sError == ISP_SUCCESS ) {
            sRet &=~ISP_TTL_DEPARTMENT ;
        }
    }

    if ( ISP_TTL_PLU & fsCheckTotal ) {
        sError = IspCheckDegTtl(FLEX_PLU_ADR);  /* Check totals */
        if ( sError == ISP_SUCCESS ) {
            sRet &=~ISP_TTL_PLU ;
        }
    }

    if ( ISP_TTL_CASHIER & fsCheckTotal ) {
        sError = IspCheckDegTtl(FLEX_CAS_ADR);  /* Check totals */
        if ( sError == ISP_SUCCESS ) {
            sRet &=~ISP_TTL_CASHIER ;
        }
    }

    if ( ISP_TTL_ETK & fsCheckTotal ) {
        sError = IspCheckDegTtl(FLEX_ETK_ADR);  /* Check totals */
        if ( sError == ISP_SUCCESS ) {
            sRet &=~ISP_TTL_ETK ;
        }
    }

    /********************** Add R3.0 ****************************************/
    if ( ISP_TTL_CPN & fsCheckTotal ) {
        sError = IspCheckDegTtl(FLEX_CPN_ADR);  /* Check totals */
        if ( sError == ISP_SUCCESS ) {
            sRet &=~ISP_TTL_CPN ;
        }
    }
    /********************** End Add R3.0 ************************************/

    IspSendPassResponse(sRet);     /* Send Response */

	{
		char  xBuff[128];
		sprintf (xBuff, "==NOTE: IspRecvPassCheckTtl() fsCheckTotal 0x%x sRet 0x%x", fsCheckTotal, sRet);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	}
}

/********************** End Add R3.1 ************************************/
/*
*===========================================================================
** Synopsis:    VOID     IspRecvPassBroadcast(CLIREQISP  *pReqMsgH);
*     Input:    pReqMsgH  - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Execute Parameter Broadcast function.
*===========================================================================
*/
VOID    IspRecvPassBroadcast(CLIREQISP  *pReqMsgH)
{
    SHORT       sError;
   
    sError = IspAmISatellite(); /* Get current terminal condition */
    if (ISP_SUCCESS == sError ) {
        IspSendPassResponse(ISP_SUCCESS);          /* Send response */
        return;
    }

    if (pReqMsgH->uchNbFlag & NB_REQALLPARA) {
        if (ISP_IAM_MASTER) {
            SstChangeInqBcasInf(0xfffe);    /* MT Mask */
        }
        if (ISP_IAM_BMASTER) {
            SstChangeInqBcasInf(0xfffd);    /* BM Mask */
        }
    }                                                 

    PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_PARAM_BROADCAST);                 /* Write log          */
    PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), pReqMsgH->uchNbFlag);        /* Write log          */

	{
		char xBuff[128];

		sprintf (xBuff, "NbWriteMessage(NB_MESOFFSET0  0x%x)", pReqMsgH->uchNbFlag);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
	// Make sure that we limit the Notice Board message change to the permissible parameter broadcast
	// options to prevent problems.
    NbWriteMessage(NB_MESOFFSET0, (pReqMsgH->uchNbFlag & (NB_REQPLU | NB_REQSUPER | NB_REQALLPARA | NB_REQLAYOUT)));
	PifSleep(1500);   // Give message time to be broadcast to cluster before responding back to requestor
    IspSendPassResponse(ISP_SUCCESS);          /* Send response */
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvPassSatellite(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Execute Read No. of Satellite function.
*===========================================================================
*/
VOID    IspRecvPassSatellite(VOID)
{
    UCHAR   uchTermNo = RflGetNoTermsInCluster();
    
    /* return no. of satellite from MDC, saratoga */
    IspSendPassSatResponse(ISP_SUCCESS, uchTermNo);
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendPassSatResponse(SHORT sReturn, UCHAR uchNoOfSat);
*     Input:    sReturn  - Return code
*               uchNoOfSat - No. of Sattellite
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Send read no. of satellite response to ISP.
*===========================================================================
*/
VOID    IspSendPassSatResponse(SHORT sReturn, UCHAR uchNoOfSat)
{
    CLIREQISP      *pReqMsgH = (CLIREQISP *)IspRcvBuf.auchData;
	CLIRESISP      ResMsgH = {0};
   
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sReturn       = sReturn;
    ResMsgH.uchNoOfSat    = uchNoOfSat;
    
    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESISP), NULL, 0);
}

/********************** End Add R3.1 ************************************/

/*
*===========================================================================
** Synopsis:    VOID     IspRecvDateTimeRead(CLIREQISPTOD  *pReqMsgH);
*     Input:    pReqMsgH - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Read Date/Time, V3.3
*===========================================================================
*/
VOID    IspRecvDateTimeRead(CLIREQISPTOD  *pReqMsgH)
{
    DATE_TIME       DateTime;
	CLIRESISPTOD    ResMsgH = {0};

    ResMsgH.usFunCode  = pReqMsgH->usFunCode;           /* Function code */
    ResMsgH.usSeqNo    = pReqMsgH->usSeqNo;             /* Sequence number */
    ResMsgH.sResCode   = STUB_SUCCESS;                  /* Response code */
    ResMsgH.sReturn    = ISP_SUCCESS;                   /* Return Valu from each module */

    PifGetDateTime(&DateTime);
	SerConvertDateTime (&DateTime, &ResMsgH.DateTime);

    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESISPTOD), (UCHAR)NULL, 0);    /* Send response */
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvDateTimeWrite(CLIDATETIME  *pReqMsgH);
*     Input:    pReqMsgH - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Set Date/Time, V3.3
*===========================================================================
*/
VOID    IspRecvDateTimeWrite(CLIREQISPTOD  *pReqMsgH)
{
	CONST TCHAR  aszParaDT[] = _T("DATETIME");
    DATE_TIME    DateTime;

    CstConvertDateTime(&(pReqMsgH->DateTime), &DateTime);

    PifSetDateTime(&DateTime);

    IspSendPassResponse(ISP_SUCCESS);                    /* Send response */

    /* --- FVT #5,  Jan/25/99 --- */
    memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
    _tcscpy (IspResetLog.aszMnemonic, aszParaDT);
    IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGWITHTRAILER, 2);
	PifLog (MODULE_ISP_LOG, LOG_EVENT_ISP_SET_DATE_TIME);
    /* --- FVT #5,  Jan/25/99 --- */
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvSysConfig(SYSCONFIG  *pReqMsgH);
*     Input:    pReqMsgH - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Set System Parameter, V3.3
*===========================================================================
*/
VOID    IspRecvSysConfig(CLIREQISP  *pReqMsgH)
{
    SYSCONFIG CONST        *pSysConf = PifSysConfig();    /* Get System Parameter */
    SHORT                  i;
    USHORT                 fbStatus;                   /* printer status of PmgGetPrtStatus() (R2.0 GCA)*/
	CLIRESISPSYSPARA       ResMsgH = {0};
	CONST TCHAR            aszParaCfg[] = _T("CONFIG");
    TCHAR                  auchDllName[CLI_LEN_DEVICE_NAME];

    ResMsgH.usFunCode = pReqMsgH->usFunCode;            /* Function code */
    ResMsgH.usSeqNo    = pReqMsgH->usSeqNo;             /* Sequence number */
    ResMsgH.sResCode   = STUB_SUCCESS;                  /* Response code */
    ResMsgH.sReturn    = ISP_SUCCESS;                   /* Return Valu from each module */

    /* --- Set Data --- */
    ResMsgH.SysPara.uchTerminal = pSysConf->auchLaddr[3];      /* Set Terminal No. */

    for( i = 0; i < PIF_LEN_PORT; i++ )         /* Set Peripheral No. */
    {
        ResMsgH.SysPara.auchComPort[i] = pSysConf->auchComPort[i];
    }

    for( i = 0; i < PIF_LEN_PORT; i++ )         /* Set Peripheral No. */
    {
        PifGetActiveDevice(pSysConf->auchComPort[i], i, &ResMsgH.SysPara.auchComDeviceName[i][0], &auchDllName[0]);
    }

    for( i = 0; i < PIF_LEN_PORT; i++ )         /* Set RS232 MODE */
    {
        ResMsgH.SysPara.ausComBaud[i]        = pSysConf->ausComBaud[i];
        ResMsgH.SysPara.auchComByteFormat[i] = pSysConf->auchComByteFormat[i];
    }

    for( i = 0; i < PIF_LEN_OPTION; i++ )       /* Set Option Parameter */
    {
        ResMsgH.SysPara.ausOption[i] = pSysConf->ausOption[i];
    }

    for( i = 0; i < PIF_LEN_RMT_PORT; i++ )     /* Set Protocol Converter #1 */
    {
        ResMsgH.SysPara.auchRmtPort[0][i] = pSysConf->auchRmtPort[0][i];
    }

    for( i = 0; i < PIF_LEN_RMT_PORT; i++ )     /* Set Protocol Converter #2 */
    {
        ResMsgH.SysPara.auchRmtPort[1][i] = pSysConf->auchRmtPort[1][i];
    }

    /* Set Load Parameters */
    ResMsgH.SysPara.uchLoadDevice = pSysConf->pUniqueInfo->uchLoadDevice;
    ResMsgH.SysPara.usLoadBaud    = pSysConf->pUniqueInfo->ausLoadBaud[0];
    ResMsgH.SysPara.uchLoadCom    = pSysConf->pUniqueInfo->auchLoadCom[0];

    /* Set System ID */
    PifGetSystemVersion(&ResMsgH.SysPara.SystemId[0]);

    /* Set Application ID */
    PifGetVersion(&ResMsgH.SysPara.ApplicationId[0]);

    /* Set ROM Size */
    ResMsgH.SysPara.uchROM1Size = pSysConf->uchROM1Size;
    ResMsgH.SysPara.uchROM2Size = pSysConf->uchROM2Size;

    /* Set SRAM Size */
    ResMsgH.SysPara.uchSRAM1Size = pSysConf->uchSRAM1Size;
    ResMsgH.SysPara.uchSRAM2Size = pSysConf->uchSRAM2Size;
    ResMsgH.SysPara.uchSRAM3Size = pSysConf->uchSRAM3Size;
    ResMsgH.SysPara.uchSRAM4Size = pSysConf->uchSRAM4Size;
    ResMsgH.SysPara.uchSRAM5Size = pSysConf->uchSRAM5Size;

    ResMsgH.SysPara.uchCom = pSysConf->uchCom;              /* Set RS232 Board Type */
    ResMsgH.SysPara.uchNet = pSysConf->uchNet;              /* Set IHC Board Type */
    ResMsgH.SysPara.uchOperType = pSysConf->uchOperType;    /* Set Operator Display Type */
    ResMsgH.SysPara.uchCustType = pSysConf->uchCustType;    /* Set Customer Display Type */
    ResMsgH.SysPara.uchKeyType = pSysConf->uchKeyType;      /* Set Keyboard Type */
    ResMsgH.SysPara.uchWaiterType = pSysConf->uchWaiterType;    /* Set Server Lock Type */
    ResMsgH.SysPara.uchIntegMSRType = pSysConf->uchIntegMSRType;    /* Set Integ. MSR Type */

    /* Set Drawer Type */
    ResMsgH.SysPara.uchDrawer1 = pSysConf->uchDrawer1;
    ResMsgH.SysPara.uchDrawer2 = pSysConf->uchDrawer2;

    /* Set Printer Error status (R2.0 GCA)*/
    ResMsgH.SysPara.usErrorStatusS  = PmgGetStatusOnly(PMG_PRT_SLIP, &fbStatus);    /* slip printer      */

    /* --- Return Disk Size,    Saratoga --- */
    ResMsgH.SysPara.ulFreeBytesAvailableToCaller = pSysConf->ulFreeBytesAvailableToCaller;
    ResMsgH.SysPara.ulTotalNumberOfBytes         = pSysConf->ulTotalNumberOfBytes;
    ResMsgH.SysPara.ulTotalNumberOfFreeBytes     = pSysConf->ulTotalNumberOfFreeBytes;

    /* --- Send Response --- */
    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESISPSYSPARA), NULL, 0);

    /* --- FVT #5,  Jan/25/99 --- */
    memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
    _tcscpy(IspResetLog.aszMnemonic, aszParaCfg);
    IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGWITHTRAILER, 2);
    /* --- FVT #5,  Jan/25/99 --- */
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvResetLog(CLIREQISPLOG *pReqLog)
*     Input:    pReqMsgH  - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:     Print Reset Log on Journal. V3.3
*===========================================================================
*/
VOID    IspRecvResetLog(CLIREQISPLOG *pReq)
{
    FSCTBL      Ring;
    DATE_TIME   WorkDate;
    UCHAR       uchDummy = 0;

    /* V3.3 */
    PifGetDateTime(&WorkDate);
    IspResetLog.uchYear = (UCHAR)(WorkDate.usYear%100);
    IspResetLog.uchMonth= (UCHAR)WorkDate.usMonth;
    IspResetLog.uchDay  = (UCHAR)WorkDate.usMDay;
    IspResetLog.uchHour = (UCHAR)WorkDate.usHour;
    IspResetLog.uchMin  = (UCHAR)WorkDate.usMin;

    uchResetLog = pReq->uchAction;

    memset(&IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));

    Ring.uchMajor = FSC_RESET_LOG;
    Ring.uchMinor = pReq->uchAction;
    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */

    /* --- Send Response --- */
    IspSendPassResponse(ISP_SUCCESS);                   /* Send response */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvResetLog2(CLIREQISPLOG2 *pReqLog)
*     Input:    pReqMsgH  - Point a received buffer address.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:     Print Reset Log on Journal. V3.3
*===========================================================================
*/
VOID    IspRecvResetLog2(CLIREQISPLOG2 *pReq)
{
    FSCTBL      Ring;
    DATE_TIME   WorkDate;
    UCHAR       uchDummy = 0;

    PifGetDateTime(&WorkDate);
    IspResetLog.uchYear = (UCHAR)(WorkDate.usYear%100);
    IspResetLog.uchMonth= (UCHAR)WorkDate.usMonth;
    IspResetLog.uchDay  = (UCHAR)WorkDate.usMDay;
    IspResetLog.uchHour = (UCHAR)WorkDate.usHour;
    IspResetLog.uchMin  = (UCHAR)WorkDate.usMin;

    uchResetLog = UIF_UCHRESETLOG_NOACTION;
    _tcsncpy(IspResetLog.aszMnemonic, pReq->aszMnemonic, 24);

    Ring.uchMajor = FSC_RESET_LOG;
    Ring.uchMinor = 0xFF;
    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */

    /* --- Send Response --- */
    IspSendPassResponse(ISP_SUCCESS);                   /* Send response */
}

VOID    IspRecvResetLog3(CLIREQISPLOG3 *pReq)
{
    FSCTBL      Ring;
    DATE_TIME   WorkDate;
    UCHAR       uchDummy = 0;

    PifGetDateTime(&WorkDate);
    IspResetLog.uchYear = (UCHAR)(WorkDate.usYear%100);
    IspResetLog.uchMonth= (UCHAR)WorkDate.usMonth;
    IspResetLog.uchDay  = (UCHAR)WorkDate.usMDay;
    IspResetLog.uchHour = (UCHAR)WorkDate.usHour;
    IspResetLog.uchMin  = (UCHAR)WorkDate.usMin;

    uchResetLog = (pReq->usFlagsAndAction & 0xff);
    _tcsncpy(IspResetLog.aszMnemonic, pReq->aszMnemonic, 24);

    Ring.uchMajor = FSC_RESET_LOG;
    Ring.uchMinor = 0xFF;
    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */

    /* --- Send Response --- */
    IspSendPassResponse(ISP_SUCCESS);                   /* Send response */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRecvResetLog3(USHORT usFunc)
*     Input:    usFunc      - Function Code from PC
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:     Print PC IF Log on Journal.             V3.3 (FVT #5)
*===========================================================================
*/
VOID    IspRecvResetLogInternalNote(USHORT usFunc, ULONG ulNumber)
{
    FSCTBL      Ring;
    DATE_TIME   WorkDate;
    UCHAR       uchDummy = 0;

    PifGetDateTime(&WorkDate);
    IspResetLog.uchYear  = (UCHAR)(WorkDate.usYear % 100);
    IspResetLog.uchMonth = (UCHAR)WorkDate.usMonth;
    IspResetLog.uchDay   = (UCHAR)WorkDate.usMDay;
    IspResetLog.uchHour  = (UCHAR)WorkDate.usHour;
    IspResetLog.uchMin   = (UCHAR)WorkDate.usMin;
    IspResetLog.ulNumber = ulNumber;
	uchResetLog = usFunc;

    Ring.uchMajor = FSC_RESET_LOG;
    Ring.uchMinor = 0xFE;
    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */

    IspResetLog.uchStatus |= ISP_RESETLOG_LOGOFF;
}

/*===== END OF SOURCE =====*/