/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : SERVER module, Header file for internal
*   Category           : SERVER module, US Hospitality Model
*   Program Name       : SERVMIN.H
*  ------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories    :
*
*   Date           :NAME           :Description
*   May-07-92      :H.NAKASHIMA    :Initial
*   Jun-04-93      :H.Yamaguchi    : Modified Enhance
*   Nov-21-95      :T.Nakahata     : R3.1 Initial (E/J Cluster Reset)
*
** NCR2172 **
*
*   Oct-05-99 : 01.00.00 : M.Teraki    : Added #pragma(...)
*   Aug-04-00            : M.Teraki    : change num. of kps timer (1 -> No.of KP)
*
** GenPOS **
*
*   Apr-02-15 : 02.02.01 : R.Chambers  : moved Operator Message function prototypes to cscas.h
*
*** OpenGenPOS ***
* Nov-17-24:02.04.01:R.Chambers : SER_MAX_TMPSIZE defined as SERISP_MAX_TMPBUF in ecr.h
* Nov-17-24:02.04.01:R.Chambers : SERNETCONFIG replaced by new PIFNETCONFIG in pif.h
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

/*
------------------------------------------
    Definition of Return Value
------------------------------------------
*/
#define     SERV_SUCCESS        0           /* normal success */
#define     SERV_BUSY           -3          /* busy error */
#define     SERV_TIME_OUT       -7          /* time out detected */
#define     SERV_ILLEGAL        -8          /* illegal parameter */

#define     SERV_MULTI_SEND     -10         /* multi sending */
#define     SERV_MULTI_RECV     -11         /* multi receiving */
#define     SERV_POWER_DOWN     -14         /* power down my terminal */
#define     SERV_TIMER_EXPIRED  -15         /* timer expired */
#define     SERV_NOT_MASTER     -20         /* not master */

/*
------------------------------------------
    Server Status (usSerStatus)
------------------------------------------
*/
#define     SER_STNORMAL        0x0000      /* normal state */
#define     SER_STINQUIRY       0x0100      /* inquiry state */
#define     SER_STMULSND        0x0200      /* multi sending state */
#define     SER_STMULRCV        0x0400      /* multi receiving state */
#define     SER_STBACKUP        0x0800      /* back up state */

/*
------------------------------------------
    MAX Definition
------------------------------------------
*/
#define     SER_MAX_TIMEOUT     0               /* time out co. inquiry */
#define     SER_MAX_TIMER       30              /* timer value, multi sending */
#define     SER_SLEEP_TIMER     3000            /* sleep timer value (ms) */
#define     SER_INQUIRY_TIMER   1000            /* delay (ms) if inquiry error */

#define     SER_MAX_KP          8               /* maximum kp # */ /* V1.0.0.4 */

#if defined(POSSIBLE_DEAD_CODE)
/*
------------------------------------------
    NET Status  (SerNetConfig.fchStatus)
	used with SerNetConfig.fchStatus to indicate
	network layer status
------------------------------------------
*/
#define     SER_NET_OPEN        PIFNET_NET_OPEN          /* NET opened */
#define     SER_NET_SEND        PIFNET_NET_SEND          /* NET sending */
#define     SER_NET_RECEIVE     PIFNET_NET_RECEIVE       /* NET receiving */
#define     SER_NET_BACKUP      PIFNET_NET_BACKUP        /* NET back up system */

//------------------------------------------------
//     SAT Status Definition
//     used with SerNetConfig.fchSatStatus to indicate
//     Satellite Terminal status.
//-------------------------------------------------
#define     SER_SAT_DISCONNECTED    PIFNET_SAT_DISCONNECTED    // Terminal is a Disconnected Satellite, see also PIF_CLUSTER_DISCONNECTED_SAT
#define     SER_SAT_UPTODATE        PIFNET_SAT_UPTODATE        // Terminal is up to date with parameters.
#define     SER_SAT_JOINED          PIFNET_SAT_JOINED          // Terminal has been joined to a cluster.  see also PIF_CLUSTER_JOINED_SAT
#endif

/*
------------------------------------------
    Communication Status Definition
------------------------------------------
*/
#define     SER_COMSTS_M_UPDATE     BLI_STS_M_UPDATE    /* master U/D or O/D */
#define     SER_COMSTS_BM_UPDATE    BLI_STS_BM_UPDATE   /* B. master U/D or O/D */
#define     SER_COMSTS_M_OFFLINE    BLI_STS_M_OFFLINE   /* master off-line */
#define     SER_COMSTS_BM_OFFLINE   BLI_STS_BM_OFFLINE  /* B. master off-line */
#define     SER_COMSTS_STOP         BLI_STS_STOP        /* STOP requet */
#define     SER_COMSTS_INQUIRY      BLI_STS_INQUIRY     /* inquiry status */
#define     SER_COMSTS_INQTIMEOUT   BLI_STS_INQTIMEOUT  /* inquiry TimeOut */
#define     SER_COMSTS_BM_ENABLED   BLI_STS_BACKUP      /* B. master is provisioned */

/*------------------------------------------------
    Terminal Configuration Definition  MACRO
-------------------------------------------------*/
#define     SER_IAM_MASTER      (SerIamMaster() == SERV_SUCCESS)
#define     SER_IAM_BMASTER     (SerIamBMaster() == SERV_SUCCESS)
#define     SER_IAM_SATELLITE   (SerIamSatellite() == SERV_SUCCESS)


/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    PROTOTYPE
===========================================================================
*/
/*
------------------------------------------
    SERVMAIN.C  (Main Functions)
------------------------------------------
*/
/*VOID THREADENTRY SerStartUp(VOID);*/
VOID    SerInitialize(VOID);    
VOID    SerRevRequest(VOID);
VOID    SerExeSpeReq(VOID);
VOID    SerRevMesHand(VOID);
VOID    SerErrResHand(VOID);

/*
------------------------------------------
    SERVESR.C  (Exec. Special Request)
------------------------------------------
*/
VOID    SerESRTimeOutBackUp(VOID);
VOID    SerESRTimeOutMulti(VOID);
VOID    SerESRTimeOutKps(VOID);
VOID    SerESRTimeOutBcas(VOID);
VOID    SerESRChgBMStat(VOID);
VOID    SerESRChgMTStat(VOID);
VOID    SerESRBackUpFinal(VOID);
VOID    SerESRNonBKSys(VOID);
VOID    SerESRAC85onMT(VOID);
VOID    SerESRAC85onBM(VOID);
VOID    SerESRBackUp(VOID);
VOID    SerESRAC42onMT(VOID);
VOID    SerESRAC42onBM(VOID);
VOID    SerESRBMDown(VOID);
VOID    SerESRTimeOutSps(VOID);

/*
------------------------------------------
    SERVRMH.C  (Resp. Message Handling)
------------------------------------------
*/
SHORT   SerRMHCheckData(VOID);
VOID    SerRMHInquiry(VOID);
VOID    SerRMHNormal(VOID);
VOID    SerRMHMulSnd(VOID);
VOID    SerRMHMulRcv(VOID);
VOID    SerRMHBackUp(VOID);
VOID    SerRecvNormal(CLIREQCOM *pReqMsgH);
SHORT   SerRecvMultiple(CLIREQCOM *pReqMsgH);
SHORT   SerRecvNextFrame(CLIREQCOM *pReqMsgH);
SHORT   SerSendMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen);
SHORT   SerSendNextFrame(CLIREQCOM *pReqMsgH);
VOID    SerSendBakMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen);
SHORT   SerSendBakNextFrame(CLIREQCOM *pReqMsgH);
USHORT  SerChekFunCode(USHORT usFunCode);
SHORT   SerChekResponse(VOID);

/*
------------------------------------------
    SERVERH.C  (Error Resp. Handling)
------------------------------------------
*/
VOID    SerERHTimeOut(VOID);
VOID    SerERHPowerFail(VOID);
VOID    SerERHOther(VOID);

/*
------------------------------------------
    SERVINQ.C  (Inquiry Functions)
------------------------------------------
*/
VOID    SerRecvInq(VOID);
VOID    SerRecvInquiry(VOID);
VOID    SerRecvInqChgDate(VOID);
VOID    SerRecvInqReqInq(VOID);

VOID    SerSendInquiry(VOID);
VOID    SerSendInqResp(VOID);
VOID    SerSendInqDate(VOID);
VOID    SerSendInqDateResp(VOID);
VOID    SerSendInqReqInq(VOID);

SHORT   SerChekInqDate(VOID);
SHORT   SerChekInqStatus(VOID);

VOID    SerConvertInqDate(DATE_TIME *pSource, CLIINQDATE *pDestine);

/*
------------------------------------------
    SERVCAS.C  (Cashier Functions)
------------------------------------------
*/
VOID    SerRecvCas(VOID);

/*
------------------------------------------
    SERVWAI.C  (Waiter Functions)
------------------------------------------
*/
VOID    SerRecvWai(VOID);

/*
------------------------------------------
    SERVGCF.C  (GCF Functions)
------------------------------------------
*/
VOID    SerRecvGCF(VOID);
VOID    SerChekGCFUnLock(VOID);

/*
------------------------------------------
    SERVOPR.C  (Op. Para Functions)
------------------------------------------
*/
VOID    SerRecvOpr(VOID);
VOID    SerRecvOprPlu(VOID);
VOID    SerRecvOprPara(VOID);

/*
------------------------------------------
    SERVTTL.C  (Total Functions)
------------------------------------------
*/
VOID    SerRecvTtl(VOID);
SHORT   SerChekTtlUpdate(CLIREQTOTAL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerChekTtlReset(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerChekTtlResetCas(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerChekTtlResetWai(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerChekTranNo(CLIREQRESETTL *pReqMsgH);
/* ===== New Functions (Release 3.1) ===== */
SHORT   SerChekTtlResetIndFin(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerChekTtlDelete(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH);
SHORT   SerRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH);
/*
------------------------------------------
    SERVKPS.C  (KPS Functions)
------------------------------------------
*/
VOID    SerRecvKps(VOID);
VOID    SerChekKpsUnLock(VOID);

/*
------------------------------------------
    SERVBAK.C  (Back Up Functions)
------------------------------------------
*/
VOID    SerRecvBak(VOID);

/*
------------------------------------------
    SERVCOM.C   (Communication Functions)
------------------------------------------
*/
SHORT   SerIamMaster(VOID);
SHORT   SerIamBMaster(VOID);
SHORT   SerIamSatellite(VOID);
SHORT   SerSendResponse(CLIRESCOM *pResMsgH, USHORT usResMsgHLen, UCHAR *puchReqData, USHORT usReqLen);
SHORT   SerSendRequest( UCHAR uchServer, CLIREQCOM *pReqMsgH, USHORT usReqMsgHLen, UCHAR *puchReqData, USHORT usReqLen);
SHORT   SerSendResponseToIpAddress(UCHAR *auchFaddr, CLIRESCOM *pResMsgH, USHORT usResMsgHLen, UCHAR *puchReqData, USHORT usReqLen);
SHORT   SerSendError(SHORT sError);
SHORT   SerResponseTargetTerminal (VOID);

VOID    SerChangeStatus(USHORT usStatus);
USHORT  SerComReadStatus(VOID);

SHORT   SerNetOpen(VOID);
VOID    SerNetClose(VOID);
SHORT   SerNetSend(USHORT usSendLen);
SHORT   SerNetReceive(VOID);

VOID    SerSendNtbStart(VOID);

VOID    SerTimerStart(VOID);
VOID    SerTimerStop(VOID);
SHORT   SerTimerRead(UCHAR address);

VOID    SerKpsTimerStart(UCHAR);
VOID    SerKpsTimerStop(UCHAR);
SHORT   SerKpsTimerRead(UCHAR);

VOID    SerBcasTimerStart(VOID);
VOID    SerBcasTimerStop(VOID);
SHORT   SerBcasTimerRead(VOID);

VOID    Ser_Timer_Start(PIFTIMER *pTim, UCHAR uchAddr);
SHORT   Ser_Timer_Read(PIFTIMER *pTim, ULONG ulMax);

VOID    SerSpsTimerStart(UCHAR uchAddr);
UCHAR   SerSpsTimerTermNo(VOID);
VOID    SerSpsTimerStop(VOID);
SHORT   SerSpsTimerRead(VOID);

SHORT    SerSendResponseFH(CLIRESCOM *pResMsgH,
                          USHORT usResMsgHLen,
                          ULONG  ulOffsetPoint,
                          USHORT usReqLen);

/*********************************************************
    Adds enhanced HP US Version

    Shared Printer Interface (Satellite/Master/BMaster)
    Display on fly
    Employee time keeping    

*********************************************************/

/*
------------------------------------------
    SERVSHR.C  (Shared printer Functions)
------------------------------------------
*/
VOID    SerRecvSps(VOID);
VOID    SerClearShrTermLock(VOID);

/*
------------------------------------------
    SERVDFL.C  (Display on fly Functions)
------------------------------------------
*/
VOID    SerRecvDfl(VOID);

/*
--------------------------------------------------
    SERVETK.C  (Employee time keeping Functions)
--------------------------------------------------
*/
VOID    SerRecvEtk(VOID);


/*
------------------------------------------
    SERVCPM.C  (Charge posting Functions)
------------------------------------------
*/
VOID    SerRecvCpm(VOID);
VOID    SstCpmResetReqNo(USHORT usUaddr);

/*
--------------------------------------------------
    SERVEJ.C  (Electoric Journal Functions, R3.1)
--------------------------------------------------
*/
VOID    SerRecvEJ(VOID);
VOID	SerRmtFileServer(VOID); //ESMITH LAYOUT
SHORT	SerOpJoinCluster(VOID);    //Disconnected Satellite, Join command
SHORT	SerOpUnjoinCluster(VOID);  //Disconnected Satellite, Unjoin command
SHORT   SerOpCheckOperatorMessageFlags (UCHAR *ucOperationalFlags);
SHORT	SerOpTransferTotals(VOID); //Disconnected Satellite, Transfer Totals command
SHORT	SerOpUpdateTotalsMsg(TTLMESSAGEUNION *pTtlMessage); //message to update totals, TtlTUMUpdateTotalsMsg()


/*
===========================================================================
    MEMORY
===========================================================================
*/

extern XGRAM           SerSndBuf;              /* Send Buffer */
extern XGRAM           SerRcvBuf;              /* Receive Buffer */
extern PIFNETCONFIG    SerNetConfig;           /* NET work configration */
extern USHORT          usSerStatus;            /* SERVER Status */
extern SHORT           sSerExeError;           /* Error Code of ESR */
extern USHORT          fsSerShrStatus;         /* Shred printer status  */
extern UCHAR           auchSerTmpBuf[SERISP_MAX_TMPBUF];  /* Data Save Buffer */
extern SERISPPREVIOUS  SerResp;                /* Response Structure */
extern PIFTIMER        SerTimer;               /* Timer Keep */
extern PIFTIMER        SerKpsTimer[SER_MAX_KP];/* Timer Control for KPS */
extern PIFTIMER        SerBcasTimer;           /* Timer Control for B-cast */
extern PIFTIMER        SerSpsTimer;            /* Timer Control for SPS */
extern PifSemHandle    husSerIFSerCli;         /* Semapho, SERVER - STUB */
extern PifSemHandle    usSerCliSem;            /* STUB Semapho */
extern PifSemHandle    husSerCreFileSem;       /* Samapho, create temp. file - used temp. file */
extern PifFileHandle   hsSerTmpFile;           /* save area for file handle of temporary buffer file */


/*===== END OF DEFINITION =====*/        
