/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
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
*   Options            :  /c /AM /Gs /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories    :
*
*   Date           :NAME           :Description
*   May-07-92      :H.NAKASHIMA    :Initial
*
*
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

/*------------------------------------------------
    Terminal Configuration Definition
-------------------------------------------------*/
#define     CLI_IAM_MASTER      (CstIamMaster() == STUB_SUCCESS)
#define     CLI_IAM_BMASTER     (CstIamBMaster() == STUB_SUCCESS)
#define     CLI_IAM_SATELLITE   (CstIamSatellite() == STUB_SUCCESS)

/*
------------------------------------------
    Communication Status Definition
------------------------------------------
*/
#define     CLI_STS_M_UPDATE     0x0001     /* master U/D or O/D */
#define     CLI_STS_BM_UPDATE    0x0002     /* B. master U/D or O/D */
#define     CLI_STS_M_OFFLINE    0x0004     /* master off-line */
#define     CLI_STS_BM_OFFLINE   0x0008     /* B. master off-line */
#define     CLI_STS_STOP         0x0020     /* STOP requet */
#define     CLI_STS_INQUIRY      0x0040     /* inquiry status */
#define     CLI_STS_INQTIMEOUT   0x0080     /* inquiry time out */

/*------------------------------------------------
    NET Status Definition
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

/*------------------------------------------------
    DESCRIPTOR POSITION (Display Communication)
-------------------------------------------------*/
#define     CLI_DESC_COMM           0       /* descriptor position */

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
/*--------------------------------------
    Request Message Structure
---------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
typedef struct {
    USHORT      usFunCode;                  /* Function Code */
	BOOL		fCliMultiRcv;				/* Multiple receive control */
    CLIREQCOM   *pReqMsgH;                  /* Request Message Pointer */
    USHORT      usReqMsgHLen;               /* Request Message Length */
    UCHAR       *pauchReqData;              /* Request Data */
    USHORT      usReqLen;                   /* Request Data Length */
    SHORT       sError;                     /* SERVER ERROR CODE */
    USHORT      usRetryCo;                  /* ERROR retry co. */
    SHORT       sRetCode;                   /* Function Return Code */
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
	USHORT      usCliTranNo;                /* transaction # of my terminal */
    UCHAR       fchStatus;                  /* NET work status */
} CLINETCONFIG;
    
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

VOID    CstInitialize(VOID);
SHORT   CstIamMaster(VOID);
SHORT   CstIamBMaster(VOID);
SHORT   CstIamSatellite(VOID);
SHORT   CstSendMaster(VOID);
SHORT   CstSendBMaster(VOID);
SHORT   SstUpdateAsMaster(VOID);
VOID    SstUpdateBackUp(VOID);
SHORT   SstReadAsMaster(VOID);
SHORT   SstReadFAsMaster(VOID);

VOID    CstComMakeMessage(UCHAR uchServer);
SHORT   CstComSendReceive(VOID);
VOID    CstComChekInquiry(VOID);
USHORT  CstComMakeMultiData(VOID);
VOID    CstComCheckError(VOID);
VOID    CstComRespHandle(VOID);
VOID    CstComErrHandle(VOID);
VOID    CstComSendConfirm(VOID);
USHORT  CstComReadStatus(VOID);

VOID    CstDisplayError(SHORT sError);
UCHAR   CstGetMessage(USHORT usErrCode);
VOID    CstDispMessage(UCHAR uchAddr, USHORT usErrCode);   
VOID    CstBackUpError(VOID);
VOID    CstBMOutOfDate(VOID);
VOID    CstMTOutOfDate(VOID);
USHORT  CstReadMDCPara(UCHAR address);

VOID    CstSleep(VOID);
VOID    CstNetOpen(VOID);
VOID    CstNetClose(VOID);
SHORT   CstNetSend(USHORT usSize);
SHORT   CstNetReceive(VOID);

/*
===========================================================================
    MEMORY
===========================================================================
*/
extern XGRAM           CliSndBuf;          /* Send Buffer */    
extern XGRAM           CliRcvBuf;          /* Receive Buffer */
extern USHORT          husCliExeNet;       /* Semapho, ISP - APPLI */
extern CLICOMIF        CliMsg;             /* Message Structure */
extern CLINETCONFIG    CliNetConfig;       /* Net Configration Structure */

/*===== END OF DEFINITION =====*/






