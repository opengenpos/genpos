/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Common Header                        
* Category    : ISP Server , NCR 2170 US Hospitality Model         
* Program Name: ISPCOM.H                                                      
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Jun-01-95:02.05.04:M.Suzuki   : R3.0                                   
* Jun-15-95:03.00.00:hkato      : R3.0
* Nov-09-95:03.01.00:T.Nakahata : R3.1 Initial
*                                   Increase No. of Employees (200 => 250)
*                                   Increase Employee No. ( 4 => 9digits)
*                                   Add Employee Name (Max. 16 Char.)
*
*** NCR2172 ***
*
* Oct-05-99:01.00.00:M.Teraki   : Initial (for Win32)
* Dec-07-99:01.00.00:hrkato     : Saratoga
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

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

#define ISP_MAX_TMPSIZE    CONSOLIMAXSIZE+256  /* Temporary buffer size     */

/*
------------------------------------------------
    Terminal configlation
-------------------------------------------------
*/
#define ISP_WITH_BACKUP_MASTER       1       /* With Backup Master System */

/*
------------------------------------------------
    Define Network port number 
------------------------------------------------
*/

#define ISP_NET_RECEIVE_TIME     60000       /*   60  seconds  */
#define ISP_SLEEP_ABORT          10000       /*   10  seconds  */
#define ISP_MAX_RECEIVE_TIME       180L      /*  180  seconds  */ 

#define ISP_SLEEP_WAIT            5000       /*    5  seconds  */
#define ISP_ELAPSED_TIME             5L      /*    5  seconds  */
/*
------------------------------------------------
    Locked Flag (fsIspLockedFC)
------------------------------------------------
*/
#define ISP_LOCK_ALLCASHIER         0x0001  /* Locked all Cashier  function */
#define ISP_LOCK_ALLWAITER          0x0002  /* Locked all Waiter   function */
#define ISP_LOCK_ALLGCF             0x0004  /* Locked all GCF      function */
#define ISP_LOCK_ALLOPEPARA         0x0008  /* Locked all Ope para function */
#define ISP_LOCK_MASSMEMORY         0x0010  /* Locked all MMM      function,    Saratoga */
#define ISP_LOCK_ALLETK             0x0020  /* Locked all ETK      function */

#define ISP_LOCK_INDCASHIER         0x0100  /* Locked Ind Cashier  function */
#define ISP_LOCK_INDWAITER          0x0200  /* Locked Ind Waiter   function */
#define ISP_LOCK_INDGCF             0x0400  /* Locked Ind GCF      function */
#define ISP_LOCK_PICKUPCASHIER      0x1000  /* Sign in pickup Cashier,  Saratoga */
#define ISP_LOCK_PLUTTL             0x2000  /* Locked plu total read function,    Saratoga */

/*
------------------------------------------------
    System Flag (fsIspCleanUP) 
------------------------------------------------
*/
#define ISP_REQUEST_CLEANUP         0x0001  /* Request clean-up function */
#define ISP_WORKS_ASMASTER          0x0002  /* Works as Master           */
#define ISP_NB_STOPPED              0x1000  /* Exec stop NB              */
#define ISP_CREATE_FLEXMEM          0x2000  /* Create flex memory        */
#define ISP_CREATE_SUBINDEX_PLU     0x4000  /* Create sub index for PLU  */
#define ISP_LOCKED_KEYBOARD         0x8000  /* Locked terminal for ISP   */

/*
------------------------------------------------
    Define state control (usIspState)
------------------------------------------------
*/
#define ISP_RSTLOWSTATE             0x0ff00  /* Reset lower 8 bit state control    */
#define ISP_ST_NORMAL               0x0000   /* Normal state               */
#define ISP_ST_MULTI_SEND           0x0100   /* Multi  sending state       */
#define ISP_ST_MULTI_RECV           0x0200   /* Multi  receive state       */
#define ISP_ST_PASSWORD             0x0400   /* Exchange password state    */

/*
------------------------------------------
    NET Status  (IspNetConfig.fchStatus)
------------------------------------------
*/
#define ISP_NET_OPEN                0x01     /* NET opened */
#define ISP_NET_SEND                0x02     /* NET sending */
#define ISP_NET_RECEIVE             0x04     /* NET receiving */
#define ISP_NET_BACKUP              0x10     /* NET back up system */
#define ISP_NET_PROHIBIT_SEND       0x20     /* NET prohibit sending */
#define ISP_NET_USING_ACKNAK        0x40     /* we are using PIF_NET_SET_STIME to implement Ack/Nak for ISP thread */

/*
------------------------------------------
    Communication Status Definition
------------------------------------------
*/
#define ISP_COMSTS_M_UPDATE         BLI_STS_M_UPDATE     /* master U/D or O/D    */
#define ISP_COMSTS_BM_UPDATE        BLI_STS_BM_UPDATE    /* B. master U/D or O/D */
#define ISP_COMSTS_M_OFFLINE        BLI_STS_M_OFFLINE    /* master off-line      */
#define ISP_COMSTS_BM_OFFLINE       BLI_STS_BM_OFFLINE   /* B. master off-line   */
#define ISP_COMSTS_STOP             BLI_STS_STOP         /* STOP requet          */
#define ISP_COMSTS_INQUIRY          BLI_STS_INQUIRY      /* inquiry status       */
#define ISP_COMSTS_INQTIMEOUT       BLI_STS_INQTIMEOUT   /* inquiry timeout      */
#define ISP_COMSTS_AC85             0x4000    /* Execute AC 85        */
#define ISP_COMSTS_AC42             0x8000    /* Execute AC 42        */

/*
------------------------------------------------
    fsCheckTotal
-   Bit masks used with function IspRecvPassCheckTtl(SHORT fsCheckTotal)
-   to determine which total types to check with SerTtlIssuedCheck()
-   if the total has been reset or not (TTL_NOTRESET).
-   See also function IspCheckDegTtl().
------------------------------------------------
*/
#define ISP_TTL_DEPARTMENT          0x0001  /* Check or Error for  Cashier  */
#define ISP_TTL_PLU                 0x0002  /* Check or Error for  PLU      */
#define ISP_TTL_WAITER              0x0004  /* Check or Error for  WAITER   */
#define ISP_TTL_CASHIER             0x0008  /* Check or Error for  CASHIER  */
#define ISP_TTL_PLU_MAINT           0x0010  /* Check or Error for  PLU Maint */
#define ISP_TTL_ETK                 0x0020  /* Check or Error for  ETK      */
#define ISP_TTL_CPN                 0x0040  /* Check or Error for  CPN R3.0 */
#define ISP_TTL_RESET_FLAG          0x006f  /* Reset flag    Chg R3.0       */

/*
------------------------------------------------
    Terminal Configuration Definition  MACRO
------------------------------------------------
*/
#define ISP_IAM_MASTER      (IspIamMaster()  == ISP_SUCCESS)
#define ISP_IAM_BMASTER     (IspIamBMaster() == ISP_SUCCESS)
#define ISP_IAM_SATELLITE   (IspIamSatellite() == ISP_SUCCESS)

/*
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*--------------------------------------
    Response Message Keep
---------------------------------------*/
typedef struct {
    SHORT       sError;                      /* Receive Error Code   */
    USHORT      usRcvdLen;                   /* Receive Data Length  */
    USHORT      usTimeOutCo;                 /* Time Out Counter     */
    UCHAR       uchPrevUA;                   /* Previous Unique Address  */    
    USHORT      usPrevMsgHLen;               /* Previous Response Mes. Len */
    USHORT      usPrevDataOff;               /* Previous Data Offset */
    CLIREQDATA  *pSavBuff;                   /* Saved data pointer   */
} ISPPREVIOUS;

/*--------------------------------------
    NET work configuration structure
---------------------------------------*/
typedef struct {
    UCHAR       auchFaddr[PIF_LEN_IP];       /* IP Address */
    USHORT      usHandle;                    /* NET work handle */
    UCHAR       fchStatus;                   /* NET work status */
	USHORT      fchSatStatus;              /* Satellite Status */
} ISPNETCONFIG;

/*--------------------------------------
    TIMER control structure
---------------------------------------*/
typedef struct {
    UCHAR       uchHour;
    UCHAR       uchMin;
    UCHAR       uchSec;
} ISPTIMER;

/*--------------------------------------
    TTL Department total structure 
---------------------------------------*/
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usDeptNo;           /* Department Number,   Saratoga */
} ISPTTLDEPT;   

typedef struct {
	UCHAR   uchMajorVer;
	UCHAR   uchMinorVer;
	UCHAR   uchIssueVer;
	USHORT  uchBuildVer;
} ISPLOGONVERSION;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/
extern USHORT        fsIspLockedFC;          /* Locked all function flag */
extern USHORT        fsIspCleanUP;           /* System control flag      */

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern XGRAM         IspSndBuf;               /* Sends    buffer */
extern XGRAM         IspRcvBuf;               /* Receives buffer */
extern UCHAR         auchIspTmpBuf[ISP_MAX_TMPSIZE];  /* Temporary buffer size */

extern USHORT        usIspState;              /* ISP state control        */
extern ULONG         ulIspLockedCasNO;        /* Locked a cashier #       */
extern USHORT        usIspLockedWaiNO;        /* Locked a waiter  #       */
extern USHORT        usIspLockedGCFNO;        /* Locked a GCF     #       */
extern USHORT        husIspOpHand;            /* Saves Op Handle         */

extern SHORT         hsIspKeyBoardHand;       /* Saves Terminal Handle   */

extern ISPNETCONFIG  IspNetConfig;            /* NET work configration   */
extern ISPPREVIOUS   IspResp;                 /* Response Structure      */
extern ISPTIMER      IspTimer;                /* Timer Keep              */
extern SHORT         sIspExeError;            /* Error Code of ESR       */

extern USHORT        usIspTest;               /* ISP Test counter        */
extern SHORT         sIspNetError;            /* Save Net work error     */       
extern USHORT        usIspPickupFunCode;      /* Saves Pick or Loan Fun.    Saratoga */
extern ULONG         ulIspPickupCasNO;        /* Pickuped a cashier #,      Saratoga */
extern USHORT        husIspMasHand;           /* Saves Mass memory Handle,  Saratoga */
extern USHORT        usIspTransNoIHav;        /* Saves Transaction No,      Saratoga */
extern LONG          lIspTotal;               /* Total for pick,            Saratoga */
extern SHORT         sIspCounter;             /* Counter for pick,          Saratoga */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

/*---------------------------------------------------------------------
    ISPUIF.C  (User I/F Functions)
---------------------------------------------------------------------*/
SHORT   IspNetChkBoard(VOID);               /* Check Comm. Board    */

/*---------------------------------------------------------------------
    ISPMAIN.C  (Main Functions)
----------------------------------------------------------------------*/
/* VOID    THREADENTRY IspMain(VOID);          / Starts ISP Task          */
VOID    IspInitialize(VOID);                /* Initialize               */
SHORT   IspNetChkBoard(VOID);               /* Check Comm. Board        */

SHORT   IspRevRequest(VOID);                /* Receive request          */
VOID    IspExeSpeReq(VOID);                 /* Exec. special request    */
VOID    IspRevMesHand(VOID);                /* Response message handling */
VOID    IspErrResHand(VOID);                /* Error handling           */

/*----------------------------------------------------------------------
    ISPESR.C  (Exec. Special Request)
----------------------------------------------------------------------*/
VOID    IspESRCheckTerminalStatus(VOID);    /* Check terminal status */
#if 0
#pragma message("  ====++++====   IspCleanUpEJ debug is ENABLED     ====++++====\z")
SHORT    IspCleanUpEJ_Debug(char *aszFilePath, int nLineNo);                 /* Clean-up E/J status, V3.3 */
#define IspCleanUpEJ()  IspCleanUpEJ_Debug(__FILE__,__LINE__)
#else
SHORT    IspCleanUpEJ(VOID);                 /* Clean-up E/J status, V3.3 */
#endif

/*----------------------------------------------------------------------
    ISPRMH.C  (Resp. Message Handling)
----------------------------------------------------------------------*/
VOID    IspRMHPassWord(VOID);               /* Receive during password state */
VOID    IspRMHNormal(VOID);                 /* Receive during normal state */
VOID    IspRMHMulSnd(VOID);                 /* Receive during multi. send state */
VOID    IspRMHMulRcv(VOID);                 /* Receive during multi. receive state */
VOID    IspRecvNormal(CLIREQCOM *pReqMsgH); /* Call each function */
SHORT   IspRecvMultiple(CLIREQCOM *pReqMsgH); /* Check multiple receiving */
SHORT   IspRecvNextFrame(CLIREQCOM *pReqMsgH); /* Received next frame */
VOID    IspSendMultiple(CLIRESCOM *pResMsgH, USHORT usResMsgHLen);  /* Send multiple response */
SHORT   IspSendNextFrame(CLIREQCOM *pReqMsgH); /* Send next frame */
USHORT  IspCheckFunCode(USHORT usFunCode);     /* Check function code */
SHORT   IspCheckResponse(VOID);                /* Check response condition */
SHORT   IspCheckNetRcvData(VOID);              /* Check received data length */
VOID    IspRecvSatCasWai(USHORT usFun);        /* Execute as satellite function */

/*-------------------------------------------------------------------------
    ISPERH.C  (Error Resp. Handling)
-------------------------------------------------------------------------*/
VOID    IspERHTimeOut(VOID);                /* Time out error handling */    
VOID    IspERHPowerFail(VOID);              /* Power fail error handling */
VOID    IspERHOther(VOID);                  /* Other error handling */

/*-------------------------------------------------------------------------
    ISPCAS.C  ( 0x0100 Cashier Functions)
-------------------------------------------------------------------------*/
VOID    IspRecvCas(VOID);                   /* Cashier function handling */

/*-------------------------------------------------------------------------
    ISPWAI.C  ( 0x0200 Waiter Functions )
-------------------------------------------------------------------------*/
//VOID    IspRecvWai(VOID);                   /* Waiter function handling */
//SHORT   IspCheckTtlWai(ULONG ulWaiterNo);  /* Check waiter  total issued or not */

/*-----------------------------------------------------------------------
    ISPGCF.C  ( 0x0300 GCF Functions)
-----------------------------------------------------------------------*/
VOID    IspRecvGCF(VOID);                   /* GCF function handling */
VOID    IspCheckGCFUnLock(VOID);            /* if Response = GCF read & lock, then unlock */
VOID    IspCheckGCFNo(USHORT usGCFNo);      /* If same GCF, then reset locked flag */

/*------------------------------------------------------------------------
    ISPTTL.C  ( 0x0400 Total Functions)
------------------------------------------------------------------------*/
VOID    IspRecvTtl(VOID);                         /* Total function handling */
SHORT   IspRecvTtlReset(CLIREQRESETTL *pReqMsgH); /* Reset total function */
SHORT   IspRecvTtlResetCas(CLIREQRESETTL *pReqMsgH); /* Reset cashier total func. */
SHORT   IspRecvTtlResetWai(CLIREQRESETTL *pReqMsgH); /* Reset waiter total func. */
SHORT   IspRecvTtlRead(CLIREQRESETTL *pReqMsgH);     /* Read total function */
SHORT   IspRecvTtlReadPlu(CLIREQRESETTL *pReqMsgH);  /* Read total function, R3.0 */
SHORT   IspRecvTtlGetLen(UCHAR uchMajorClass);       /* Get total structure length */
/* ===== New Functions (Release 3.1) ===== */
SHORT   IspRecvTtlResetIndFin(CLIREQRESETTL *pReqMsgH); /* Reset individual financial, R3.1 */

SHORT   IspMakeClosePickFormat(VOID);               /* Saratoga */

SHORT   IspRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH); /* saratoga */

VOID    IspLoanPickFCAmount(DCURRENCY *lNative, DCURRENCY lAmount, UCHAR uchTotalID); /* Saratoga */
SHORT   IspRecvTtlDelete(CLIREQRESETTL *pReqMsgH);

/*----------------------------------------------------------------------
    ISPOPR.C  ( 0x0500 Op. Para Functions)
----------------------------------------------------------------------*/
VOID    IspRecvOpr(VOID);                       /* Ope para handling   */
SHORT   IspRecvCheckPara(UCHAR uchMajorClass);  /* Check parameter     */
SHORT   IspAfterProcess(UCHAR uchMajorClass);   /* Exec. after process */
VOID    IspRecvMasEnterCM(VOID);                /* Saratoga */
VOID    IspRecvMasMntCom(VOID);                 /* Saratoga */
SHORT   IspLabelAnalysis(VOID *pPlu);           /* saratoga */

/*-----------------------------------------------------------------------
    ISPOPRSB.C  ( 0x0500 Op. Para Functions)
-----------------------------------------------------------------------*/
SHORT   IspCheckTtlPLU(TCHAR *auchPluNo, UCHAR  uchAjectNo);    /* Check PLU total issued or not,  Saratoga */
#if 1
SHORT   IspTtlErrorConvert_Debug(SHORT sError, char *aszPath, int nLineNo);
#define IspTtlErrorConvert(sError) IspTtlErrorConvert_Debug(sError,__FILE__,__LINE__)
#else
SHORT   IspTtlErrorConvert(SHORT sError);    /* Convert error code        */
#endif
SHORT   IspCheckTtlDept(USHORT usDeptNo);    /* Check Dept total issued or not, Saratoga */
SHORT   IspCheckTtlGCF(VOID);                /* Check GCF  total issued or not */
SHORT   IspCheckTtlCas(ULONG ulCashierNo);   /* Check cashier total issued or not */
VOID    IspSavFlexMem(VOID);                 /* Save flex memmory data      */
SHORT   IspCreateFlexMem(VOID);              /* Create flex memory          */
SHORT   IspCreateFiles(VOID);                /* Create files                */ 
SHORT   IspCheckFlexFileSize(UCHAR uchAddr); /* Check flexible file size    */
SHORT   IspCheckDegTtl(UCHAR uchAdd);        /* Check all total s */
SHORT   IspCheckTtlCpn(USHORT usCpnNo);      /* Check Coupon total issued or not  Add R3.0 */


/*-----------------------------------------------------------------------
    ISPPASS.C  ( 0x0700 Password Functions)
-----------------------------------------------------------------------*/
VOID    IspRecvPass(VOID);                      /* Special function handling */
VOID    IspRecvPassLogOn(CLIREQISP  *pReqMsgH); /* Log on password func.     */
VOID    IspSendPassResponse(SHORT sReturn);     /* Send special func. response */
VOID    IspRecvPassChange(CLIREQISP  *pReqMsgH);  /* Password change func.   */
VOID    IspRecvPassLockKB(VOID);                /* Lock keyboard function    */ 
VOID    IspRecvPassUnlockKB(VOID);              /* Unlock keyboard function  */
VOID    IspRecvPassLogOff(VOID);                /* Log off function          */
VOID    IspRecvPassCheckTtl(SHORT fsCheckTotal);  /* Check total */
VOID    IspRecvPassBroadcast(CLIREQISP  *pReqMsgH); /* Parameter Broadcast func.     */
VOID    IspRecvPassSatellite(VOID);             /* Read No. of Satellite func.     */
VOID    IspSendPassSatResponse(SHORT sReturn, UCHAR uchNoOfSat); /* Send read no. of satellite response */
VOID    IspRecvDateTimeRead(CLIREQISPTOD  *pReqMsgH);   /* V3.3 */
VOID    IspRecvDateTimeWrite(CLIREQISPTOD  *pReqMsgH);  /* V3.3 */
VOID    IspRecvSysConfig(CLIREQISP  *pReqMsgH);         /* V3.3 */
VOID    IspRecvResetLog(CLIREQISPLOG *pReqMsgH);        /* V3.3 */
VOID    IspRecvResetLog2(CLIREQISPLOG2 *pReqMsgH);      /* V3.3 */
VOID    IspRecvResetLog3(CLIREQISPLOG3 *pReqMsgH);      /* V3.3 */
VOID    IspRecvResetLogInternalNote(USHORT usFunc, ULONG ulNumber);                /* V3.3 FVT#5 */
VOID	IspSendModeResponse(VOID);						/* Mode of Current Terminal */
VOID	IspSendSystemStatusResponse(VOID);              // provide system status are reported by IspComReadStatus().

ISPLOGONVERSION  IspRecvLogonVersion (ISPLOGONVERSION *pVersion);

/*-------------------------------------------------------------------------
    ISPCOM.C   (Communication Functions)
-------------------------------------------------------------------------*/
SHORT   IspIamMaster(VOID);                  /* Check am I Master    */
SHORT   IspIamBMaster(VOID);                 /* Check am I B-Master  */
SHORT   IspIamSatellite(VOID);               /* Check am I Satellite */
VOID    IspSendResponse(CLIRESCOM *pResMsgH, ULONG ulResMsgHLen, UCHAR  *puchReqData, ULONG ulReqLen); /* Send response */
VOID    IspSendError(SHORT sError);          /* Send error response */

VOID    IspChangeStatus(USHORT usStatus);    /* Change state control */
USHORT  IspComReadStatus(VOID);              /* Get terminal status */

SHORT   IspNetOpen(VOID);                    /* Open net work */
VOID    IspNetClose(VOID);                   /* Close net work */
VOID	IspNetSynchSeqNo (VOID);
VOID    IspNetSend(ULONG ulSendLen);        /* Send net work  */
VOID    IspNetReceive(VOID);                 /* Receive net work */

VOID    IspTimerStart(VOID);                 /* Start timer */
VOID    IspTimerStop(VOID);                  /* Stop timer */
LONG    IspTimerRead(VOID);                  /* Check elapsed time */
SHORT   IspAmISatellite(VOID);               /* Check to work as Satellite */

VOID    IspWriteResetLog(VOID);                 /* write pcif reset log, V3.3 */
VOID    IspWriteResetLogAction (UCHAR uchAction, UCHAR uchReset, WCHAR *aszMnemonic);
BOOL    IspRecvTtlCheckPluNo(TCHAR *puchPluNo); /* Check PLU No,    Saratoga */

/*-------------------------------------------------------------------------
    ISPABORT.C   (Abort/Log Functions)
-------------------------------------------------------------------------*/
VOID    IspAbort(USHORT usFaultCode);       /* Go to system fault */

/*-----------------------------------------------------------------------
    ISPCLEAN.C   (CleanUp Functions)
-----------------------------------------------------------------------*/
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// various IspClean functions are called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to these functions
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   IspCleanUp debug is ENABLED     ====++++====\z")
VOID    IspCleanUpSystem_Debug(char *aszFilePath, int nLineNo);             /* Cleanup system condition */     
VOID    IspCleanUpLockFun_Debug(char *aszFilePath, int nLineNo);            /* Cleanup locked function */
SHORT   IspCleanUpUpdatePLU_Debug(char *aszFilePath, int nLineNo);          /* Cleanup PLU sub-index */

SHORT   IspCleanUpLockKB_Debug(char *aszFilePath, int nLineNo);             /* Unlock keyboard */
SHORT   IspCleanUpLockCas_Debug(USHORT fsFlag, char *aszFilePath, int nLineNo);   /* Unlock cashier func. */
SHORT   IspCleanUpLockGCF_Debug(USHORT fsFlag, char *aszFilePath, int nLineNo);   /* Unlock GCF    func. */
VOID    IspCleanUpLockOp_Debug(char *aszFilePath, int nLineNo);             /* Unlock ope para func. */
VOID    IspSetNbStopFunc_Debug(char *aszFilePath, int nLineNo);             /* Set all stop func.  */
VOID    IspResetNbStopFunc_Debug(char *aszFilePath, int nLineNo);           /* Reset all stop func. */
VOID    IspCleanUpLockETK_Debug(char *aszFilePath, int nLineNo);            /* Unlock etk func. */
SHORT   IspCleanUpPickupCas_Debug(ULONG ulCashierNo, char *aszFilePath, int nLineNo);/* Saratoga */
SHORT   IspCleanUpLockMas_Debug(char *aszFilePath, int nLineNo);            /* Saratoga */
SHORT   IspCleanUpLockPLUTtl_Debug(char *aszFilePath, int nLineNo);         /* saratoga */

#define IspCleanUpSystem()    IspCleanUpSystem_Debug(__FILE__,__LINE__)
#define IspCleanUpLockFun()   IspCleanUpLockFun_Debug(__FILE__,__LINE__)
#define IspCleanUpUpdatePLU() IspCleanUpUpdatePLU_Debug(__FILE__,__LINE__)

#define IspCleanUpLockKB()        IspCleanUpLockKB_Debug(__FILE__,__LINE__)
#define IspCleanUpLockCas(fsFlag) IspCleanUpLockCas_Debug(fsFlag,__FILE__,__LINE__)
#define IspCleanUpLockGCF(fsFlag) IspCleanUpLockGCF_Debug(fsFlag,__FILE__,__LINE__)
#define IspCleanUpLockOp()    IspCleanUpLockOp_Debug(__FILE__,__LINE__)
#define IspSetNbStopFunc()    IspSetNbStopFunc_Debug(__FILE__,__LINE__)
#define IspResetNbStopFunc()  IspResetNbStopFunc_Debug(__FILE__,__LINE__)
#define IspCleanUpLockETK()   IspCleanUpLockETK_Debug(__FILE__,__LINE__)
#define IspCleanUpPickupCas(ulCashierNo) IspCleanUpPickupCas_Debug(ulCashierNo,__FILE__,__LINE__)
#define IspCleanUpLockMas()   IspCleanUpLockMas_Debug(__FILE__,__LINE__)
#define IspCleanUpLockPLUTtl() IspCleanUpLockPLUTtl_Debug(__FILE__, __LINE__)
#else
VOID    IspCleanUpSystem(VOID);             /* Cleanup system condition */     
VOID    IspCleanUpLockFun(VOID);            /* Cleanup locked function */
SHORT   IspCleanUpUpdatePLU(VOID);          /* Cleanup PLU sub-index */

SHORT   IspCleanUpLockKB(VOID);             /* Unlock keyboard */
SHORT   IspCleanUpLockCas(USHORT fsFlag);   /* Unlock cashier func. */
SHORT   IspCleanUpLockGCF(USHORT fsFlag);   /* Unlock GCF    func. */
VOID    IspCleanUpLockOp(VOID);             /* Unlock ope para func. */
VOID    IspSetNbStopFunc(VOID);             /* Set all stop func.  */
VOID    IspResetNbStopFunc(VOID);           /* Reset all stop func. */
VOID    IspCleanUpLockETK(VOID);            /* Unlock etk func. */
SHORT   IspCleanUpPickupCas(ULONG ulCashierNo);/* Saratoga */
SHORT   IspCleanUpLockMas(VOID);            /* Saratoga */
SHORT   IspCleanUpLockPLUTtl(VOID);         /* saratoga */
#endif

/*-----------------------------------------------------------------------
    ISPEJ.C  ( 0x0900 EJ  Functions)
-----------------------------------------------------------------------*/
VOID    IspRecvEj(VOID);                    /* EJ function handling */

/*-----------------------------------------------------------------------
    ISPETK.C  ( 0x0a00 ETK  Functions)
-----------------------------------------------------------------------*/
VOID    IspRecvEtk(VOID);                   /* Etk function handling */

/*-----------------------------------------------------------------------
    ISPCPM.C  ( 0x0b00 CPM  Functions)
-----------------------------------------------------------------------*/
VOID    IspRecvCpm(VOID);                   /* CPM function handling */


/*===== END OF DEFINITION =====*/