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
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
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

//------------------------------------------------
//     SAT Status Definition
//     used with CliNetConfig.fchSatStatus to indicate
//     Satellite Terminal status.
//-------------------------------------------------
#define     CLI_SAT_DISCONNECTED    0x0001     // Terminal is a Disconnected Satellite, see also PIF_CLUSTER_DISCONNECTED_SAT
#define     CLI_SAT_UPTODATE        0x0002     // Terminal is up to date with parameters.
#define     CLI_SAT_JOINED          0x0004     // Terminal has been joined to a cluster.  see also PIF_CLUSTER_JOINED_SAT

/*------------------------------------------------
    NET Status Definition
	used with CliNetConfig.fchStatus to indicate
	network layer status
-------------------------------------------------*/
#define     CLI_NET_OPEN        0x01        /* NET opened */
#define     CLI_NET_SEND        0x02        /* NET sending */
#define     CLI_NET_RECEIVE     0x04        /* NET receiving */
#define     CLI_NET_BACKUP      0x10        /* NET back up system */
#define     CLI_NET_DISP20      0x20        /* NET display 2 x 20 */

/*------------------------------------------------
    ERROR CODE (Display)
-------------------------------------------------*/
#define     CLI_ERRORCODE_BUSY      10      /* Error Code, Master Busy */
#define     CLI_ERRORCODE_BACKUP    11      /* Error Code, Back Up */

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
    Request Message Structure
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;                  /* Function Code */
    CLIREQCOM   *pReqMsgH;                  /* Request Message Pointer */
    USHORT      usReqMsgHLen;               /* Request Message Length */
    UCHAR       *pauchReqData;              /* Request Data */
    USHORT      usReqLen;                   /* Request Data Length */
    SHORT       sError;                     /* SERVER ERROR CODE such as STUB_BUSY or STUB_SUCCESS */
    USHORT      usRetryCo;                  /* ERROR retry count.  see CstComErrHandle()  */
    SHORT       sRetCode;                   /* Function Return Code, value of pResp->sReturn put here by CstComRespHandle() */
    USHORT      usAuxLen;                   /* Received Data Length */
    CLIRESCOM   *pResMsgH;                  /* Response Message Pointer */
    USHORT      usResMsgHLen;               /* Response Message Length */
    UCHAR       *pauchResData;              /* Response Data Bufer */
    USHORT      usResLen;                   /* Response Dara Buffer Length */
} CLICOMIF;

/*--------------------------------------
    NET work configuration structure
---------------------------------------*/
typedef struct {
    UCHAR       auchFaddr[PIF_LEN_IP];      /* IP Address saved */
    USHORT      usHandle;                   /* NET work handle saved */
    UCHAR       fchStatus;                  /* NET work status */
    USHORT      fchSatStatus;              /* Satellite Status */
} CLINETCONFIG;

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
SHORT   SstReadAsMaster(VOID);
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
extern CLINETCONFIG    CliNetConfig;       /* Net Configration Structure */
extern USHORT          fsCliComDesc;       /* Descriptor control */
extern USHORT          usCliCpmReqNo;      /* CPM Request counter */


/*===== END OF DEFINITION =====*/
