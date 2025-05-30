/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB, Header file for internal
*   Category           : Client/Server STUB, US Hospitality Model
*   Program Name       : CSSTUBIN.H
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
*       2020  -> OpenGenPOS Rel 2.4.0 (Windows 10, Datacap removed) Open source
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2019 with Rel 2.4.0
* --------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories    :
*
*   Date           :NAME           :Description
*   May-07-92      :H.NAKASHIMA    :Initial
*   Jun-08-93      :H.Yamaguchi    : Adds Enhance function.
*   Nov-20-95      :T.Nakahata     : R3.1 Initial (E/J Cluster Reset)
*
*
*** NCR2172 ***
*
*   Oct-04-99      :M.Teraki       : R1.0 (prototype) Initial
*                                  : Add #pragma pack(...)
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
    Communication Status Definition
------------------------------------------
*/
#define     CLI_STS_M_UPDATE     BLI_STS_M_UPDATE        /* master U/D or O/D, BLI_STS_M_UPDATE */
#define     CLI_STS_BM_UPDATE    BLI_STS_BM_UPDATE       /* B. master U/D or O/D, BLI_STS_BM_UPDATE */
#define     CLI_STS_M_OFFLINE    BLI_STS_M_OFFLINE       /* master off-line, BLI_STS_M_OFFLINE */
#define     CLI_STS_BM_OFFLINE   BLI_STS_BM_OFFLINE      /* B. master off-line, BLI_STS_BM_OFFLINE */
#define     CLI_STS_STOP         BLI_STS_STOP            /* STOP request, BLI_STS_STOP */
#define     CLI_STS_INQUIRY      BLI_STS_INQUIRY         /* inquiry status, BLI_STS_INQUIRY */
#define     CLI_STS_INQTIMEOUT   BLI_STS_INQTIMEOUT      /* inquiry time out, BLI_STS_INQTIMEOUT */
#define     CLI_STS_BACKUP_FOUND BLI_STS_BACKUP          /* Notice Board indicates Backup messages received, BLI_STS_BACKUP */
#define     CLI_STS_M_REACHABLE  BLI_STS_M_REACHABLE     /* master off-line but is reachable, BLI_STS_M_REACHABLE */

#if defined(POSSIBLE_DEAD_CODE)
//------------------------------------------------
//     SAT Status Definition
//     used with CliNetConfig.fchSatStatus to indicate
//     Satellite Terminal status.
//-------------------------------------------------
#define     CLI_SAT_DISCONNECTED    PIFNET_SAT_DISCONNECTED     // Terminal is a Disconnected Satellite, see also PIF_CLUSTER_DISCONNECTED_SAT
#define     CLI_SAT_UPTODATE        PIFNET_SAT_UPTODATE         // Terminal is up to date with parameters.
#define     CLI_SAT_JOINED          PIFNET_SAT_JOINED           // Terminal has been joined to a cluster.  see also PIF_CLUSTER_JOINED_SAT

/*------------------------------------------------
    NET Status Definition
	used with CliNetConfig.fchStatus to indicate
	network layer status
-------------------------------------------------*/
#define     CLI_NET_OPEN        PIFNET_NET_OPEN        /* NET opened */
#define     CLI_NET_SEND        PIFNET_NET_SEND        /* NET sending */
#define     CLI_NET_RECEIVE     PIFNET_NET_RECEIVE     /* NET receiving */
#define     CLI_NET_BACKUP      PIFNET_NET_BACKUP      /* NET back up system */
//#define     CLI_NET_DISP20      0x20        /* NET display 2 x 20, used with old DISP_2X20 operator display for NCR 2170 */

/*------------------------------------------------
    ERROR CODE (Display)
-------------------------------------------------*/
#define     CLI_ERRORCODE_BUSY      10      /* Error Code, Master Busy */
#define     CLI_ERRORCODE_BACKUP    11      /* Error Code, Back Up */
#endif

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



/*--------------------------------------
    Client Communication Interface Structure

    This structure is used to define Client subsystem
    memory resident storage area for the data request
    that the Client subsystem is processing.

    This struct is not used in the actual message but rather
    to store the information needed the Client subsystem
    needs to create a message that is sent and to process
    the response message received.
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;                  /* Function Code */
    CLIREQCOM   *pReqMsgH;                  /* Request Message Pointer */
    USHORT      usReqMsgHLen;               /* Request Message Length */
    UCHAR       *pauchReqData;              /* Request Data or pointer to a PIF file handle containing the data */
    USHORT      usReqLen;                   /* Request Data Length or size of the sending file */
    SHORT       sError;                     /* SERVER ERROR CODE such as STUB_BUSY or STUB_SUCCESS */
    USHORT      usRetryCo;                  /* ERROR retry count.  see CstComErrHandle()  */
    SHORT       sRetCode;                   /* Function Return Code, value of pResp->sReturn put here by CstComRespHandle() */
    USHORT      usAuxLen;                   /* Received Data Length or for files the current offset into the file. */
    CLIRESCOM   *pResMsgH;                  /* Response Message Pointer */
    USHORT      usResMsgHLen;               /* Response Message Length */
    UCHAR       *pauchResData;              /* Response Data Bufer or pointer to a PIF file handle to receive the data */
    USHORT      usResLen;                   /* Response Dara Buffer Length or size of the receiving file */
} CLICOMIF;


/*  Define for other   */
#define  CLI_BAK_ETK_PARAM_FILE      0      /* PARAMETK */
#define  CLI_BAK_ETK_TOTAL_FILE      1      /* TOTALETK */

//#define  CLI_BAK_FILE_SIZE         450    //  same as OP_BACKUP_WORK_SIZE

/*  Struct  */

typedef struct {
    USHORT      usFile;
    LONG        lFilePosition;
    USHORT      usMaxRecord;
    USHORT      usDataLen;
    USHORT      usSeqNo;
}CLI_FILE_BACKUP;


/*  Define for other   */
// #define  CLI_BAK_MMM_SIZE       450    //  same as OP_BACKUP_WORK_SIZE

/*  Struct  */
typedef struct {
    USHORT      usFile;
    LONG        lFilePosition;
    USHORT      usMaxRecord;
    USHORT      usDataLen;
    USHORT      usSeqNo;
}CLI_MMM_BACKUP;

/*--- End of define for internal function for backup file  ----*/


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

/*-------------------------
    CSSTBCOM.C 
--------------------------*/
VOID    CstInitialize(VOID);
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function CstSendMasterFH () and CstSendBMasterFH ()
// is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to CstSendMasterFH () and
// CstSendBMasterFH () that provide the file and line number in the file
// from whence the the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CstSendMaster_Debug() is ENABLED    ====++++====\z")
#pragma message("  ====++++====   CstSendBMaster_Debug() is ENABLED   ====++++====")
#define CstSendMaster() CstSendMaster_Debug(__FILE__, __LINE__)
#define CstSendBMaster() CstSendBMaster_Debug(__FILE__, __LINE__)
SHORT   CstSendMaster_Debug(char *aszFilePath, int nLineNo);
SHORT   CstSendBMaster_Debug(char *aszFilePath, int nLineNo);
#else
SHORT   CstSendMaster(VOID);
SHORT   CstSendBMaster(VOID);
#endif
SHORT   CstSendMasterWithArgs (CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
SHORT   CstSendBMasterWithArgs (CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
SHORT   CstSendTerminal(USHORT usTerminalPosition);
SHORT   CstSendBroadcast(CLICOMIF *pCliMsg);
SHORT   SstUpdateAsMaster(VOID);
SHORT   SstUpdateBackUp(VOID);
SHORT   SstReadAsMaster(VOID);   // check terminal is Master or not
SHORT   SstReadFAsMaster(VOID);

VOID    CstComCheckError(CLICOMIF *pCliMsg, XGRAM *pCliRcvBuf);
VOID    CstComRespHandle(CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
USHORT  CstComReadStatus(VOID);

SHORT   CstDisplayError(SHORT sError, USHORT usTarget);
SHORT   CstBackUpError(VOID);
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CstMTOutOfDate_Debug() CstBMOutOfDate_Debug() is ENABLED     ====++++====\z")
#pragma message("  ====++++====   CstMTOffLine_Debug() CstBMOffLine_Debug() is ENABLED       ====++++====")
SHORT   CstBMOutOfDate_Debug(char *aszFilePath, int iLineNo);
#define CstBMOutOfDate() CstBMOutOfDate_Debug(__FILE__, __LINE__)
SHORT   CstBMOffLine_Debug(char *aszFilePath, int iLineNo);
#define CstBMOffLine() CstBMOffLine_Debug(__FILE__, __LINE__)
SHORT   CstMTOutOfDate_Debug(char *aszFilePath, int iLineNo);
#define CstMTOutOfDate() CstMTOutOfDate_Debug(__FILE__, __LINE__)
SHORT   CstMTOffLine_Debug(char *aszFilePath, int iLineNo);
#define CstMTOffLine() CstMTOffLine_Debug(__FILE__, __LINE__)
#else
SHORT   CstBMOutOfDate(VOID);
SHORT   CstBMOffLine(VOID);
SHORT   CstMTOutOfDate(VOID);
SHORT   CstMTOffLine(VOID);
#endif
USHORT  CstReadMDCPara(UCHAR address);

VOID    CstSleep(VOID);
VOID    CstNetOpen(VOID);
VOID    CstNetClose(VOID);
VOID    CstNetClear(VOID);
SHORT   CstNetSend(USHORT usSize, XGRAM *pCliSndBuf);
SHORT   CstNetReceive(USHORT usMDC, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);

SHORT   CstSendTarget(UCHAR uchUaddr, CLICOMIF *pCliMsg, XGRAM *pCliSndBuf, XGRAM *pCliRcvBuf);
VOID    CstCpmRecMessage(VOID);

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function CstSendMasterFH () and CstSendBMasterFH ()
// is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to CstSendMasterFH () and
// CstSendBMasterFH () that provide the file and line number in the file
// from whence the the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CstSendMasterFH_Debug() is ENABLED    ====++++====\z")
#pragma message("  ====++++====   CstSendBMasterFH_Debug() is ENABLED   ====++++====")
#define CstSendMasterFH(usType) CstSendMasterFH_Debug(usType,__FILE__, __LINE__)
#define CstSendBMasterFH(usType) CstSendBMasterFH_Debug(usType,__FILE__, __LINE__)
SHORT   CstSendMasterFH_Debug(USHORT usType, char *aszFilePath, int nLineNo);
SHORT   CstSendBMasterFH_Debug(USHORT usType, char *aszFilePath, int nLineNo);
#else
SHORT   CstSendMasterFH(USHORT usType);
SHORT   CstSendBMasterFH(USHORT usType);
#endif
SHORT   CstSendTerminalFH(USHORT usType, USHORT usTerminalPosition);
SHORT   SstReadFAsMasterFH(USHORT usType);
/*-------------------------
    CSSTBTTL.C 
--------------------------*/
SHORT   CstTtlResetConsecCo(UCHAR uchMajorClass, UCHAR  uchMDCBit);
SHORT   CstTtlUpdate(UCHAR *puchSendBuf, USHORT usSize);
SHORT   CstTtlMasterReset(VOID);
VOID    CstTtlSetCliTranNo (USHORT usTerminalPosition);

/*
===========================================================================
    MEMORY
===========================================================================
*/
//extern XGRAM           CliSndBuf;          /* Send Buffer */    
//extern XGRAM           CliRcvBuf;          /* Receive Buffer */
extern USHORT          husCliWaitDone;     /* Semapho, wait do by SERVER */
extern USHORT          husCliExeNet;       /* Semapho, ISP - APPLI */
extern USHORT          husCliExePara;      /* Semapho, ISP - APPL, para */
extern USHORT          husCliComDesc;      /* Semapho, ISP - APPL, descri. */
//extern CLICOMIF        CliMsg;             /* Message Structure */
extern PIFNETCONFIG    CliNetConfig;       /* Net Configration Structure */
extern USHORT          fsCliComDesc;       /* Descriptor control */
extern USHORT          usCliCpmReqNo;      /* CPM Request counter */


/*===== END OF DEFINITION =====*/
