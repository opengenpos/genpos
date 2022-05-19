/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*                                                 **
**        *  NCR 2170   *              NCR Corporation, E&M OISO          **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1996              **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Charge Posting Manager, Local Header File
* Category    : Charge Postig Manager, NCR 2170 Charge Posting Manager Application
* Program Name: EEPTL.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Zap
* --------------------------------------------------------------------------
* Abstract:
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date      Ver.        Name            Description
* Nov-01-93 00.00.01  : Yoshiko. Jim  : Initial
* Dec-04-96 02.00.00  : hrkato        : R2.0
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

#if !defined(EEPTL_H_INCLUDED)
#define EEPTL_H_INCLUDED

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
/**     Task control          **/

#define     EEPT_SEND                0           /* send task                */
#define     EEPT_RECEIVE             1           /* receive task             */

/**     RS-232 Port control   **/
#define		EEPT_PORT_USEDLL		 -3			//use DLL rathern than RS-232
#define     EEPT_PORT_USENET         -2          /* use LAN network rather than RS-232  */
#define     EEPT_PORT_NONE           -1          /* not exist RS-232 port    */
#define     EEPT_RETRY_COUNT         3           /* retry counter            */
#define     EEPT_WAIT_PORT           3           /* retry counter for port   */
#define     EEPT_WAIT_PORT_TIME      5           /* retry timer 5 sec.       */

/**
		status of Enhanced Electronic Payment Terminal Manager machine

	These states are used to indicate the current status of the EEPT Manager
	state machine which is processing user requests to the EEPT Host as well
	as processing unsolicited status messages from the EEPT Host.

	Function EEptWriteStatus () is used to update the EPT status (as indicated by the
	global variable uchEEptStatus) which is guarded by the semaphore usEEptSemState.

	See the function EEptExeUnsoli () which contains the logic for dealing with EPT Host
	unsolicited messages and modifies the uchEEptStatus status variable accordingly.

	See the function EEptSendMessage () which shows a user request being sent and how the
	status variable uchEEptStatus is checked to determine if the send should fail or not
	depending on the status of the EPT state machine.  It also shows the status being set
	to indicate that a user request is being sent to the EPT Host.

	See the function EEptExeResponse () which shows the handling of the response from the
	EPT Host.
 **/
#define     EEPT_STATE_OUTSIDE       1           /* ready                    */
#define     EEPT_STATE_SEND_OK       2           /* wait response data       */
#define     EEPT_STATE_INSIDE        3           /* sending request data     */
#define     EEPT_STATE_UNSOLI        4           /* processing unsoli func.(R2.0) */
#define     EEPT_STATE_ERROR         5           /* processing an error */

/**     sub status of EEPT (R2.0) **/

#define     EEPT_SUBSTATE_ENQACK     1           /* sent ENQ and wait ACK */
#define     EEPT_SUBSTATE_REQACK     2           /* sent REQ and wait ACK */
#define     EEPT_SUBSTATE_RESP       3           /* sent or received ACK and wait RESP */
#define     EEPT_SUBSTATE_ACKEOT     4           /* sent ACK and wait EOT */
#define     EEPT_SUBSTATE_REQ        5           /* sent ACK and wait REQ */

/**     buffer(send/spool) control  **/
#define     EEPT_SEND_SIZE   sizeof(EEPTREQUNION)  /* size of send area,   R2.0 */
#define     EEPT_SPOOL_SIZE  sizeof(EEPTRSPDATA)  /* size of spool area,  R2.0 */
#define     MEEPT_SPOOL_SIZE  sizeof(MERCURYEPTRSPDATA)  /* size of spool area,  R2.0 */

#define     EEPT_FIX_TIME            1           /* timer during spool msg   */
#define     EEPT_SLEEP_TIME          1           /* timer from getting port  */
#define     EEPT_SLEEP_COUNT         10          /* counter from getting port*/

#define     EEPT_START_CHAR       0x20           /* received data start char (SP) */
#define     EEPT_END_CHAR         0x0d           /* received data end char   (CR) */

/* Transmission Control Character (R2.0) */
#define     EEPT_ACK_CHAR         0x06           /* Acknowledge  char         (ACK) */
#define     EEPT_NAK_CHAR         0x15           /* Negative Acknowledge char (NAK) */
#define     EEPT_ENQ_CHAR         0x05           /* Enquiry char              (ENQ) */
#define     EEPT_EOT_CHAR         0x04           /* End of Transmission char  (EOT) */

#define     EEPT_REQ              0x00           /* Request data                    */


/**     response control  **/
#define     EEPT_NOISE               0           /* read 1byte data is noize */
#define     EEPT_CONT                1           /* message continued        */
#define     EEPT_END                 2           /* message end              */
#define     EEPT_TRACON              3           /* transmission control char(R2.0)*/


#define     EEPT_TYPE_USER_RESP      0           /* response data for user   */
#define     EEPT_TYPE_ERR_RESP       1           /* other data               */
#define     EEPT_TYPE_UNSOLI         2           /* unsoli. data from PMS    */
#define     EEPT_TYPE_TRACON         3           /* transmission control char(R2.0)*/

/**     other control    **/
#define     EEPT_SAME                0           /* current & prev. is same  */
#define     EEPT_NOT_SAME            1           /* current & prev. not same */

#define     EEPT_PASS                0           /* timer value OK           */
#define     EEPT_READ_AGAIN          0           /* PifReadCom() again       */
#define     EEPT_MAX_SET_TIMER       60          /* max time for Pifsleep(),
                                                   PifControlCom()          */
#define     EEPT_WAIT_RECEIVE        44          /* sleep time(sec) of receive thread (R2.0) */

#define     EEPT_MIN_TEXT_LEN        4           /* minimum text lemgth (STX,text,ETX,LRC) (R2.0) */

#define     EEPT_UNSOLI_1ST          0           /* 1st REQ data of Unsoli. Rep. Func (R2.0) */
#define     EEPT_UNSOLI_NEXT         1           /* next REQ data of Unsoli. Rep. Func (R2.0) */

/* for the EEPT R2.0 RS232C LOG Log (R2.0) */

#define EEPT_MAX_LOG_NUMBER           10
#define EEPT_LOG_DATA_SIZE            11

#define EEPT_MODULE_RECEIVECOM         1     /* EEptReceiveCom() Module Id */
#define EEPT_MODULE_SENDCOM            2     /* EEptSendCom()    Module Id */
#define EEPT_MODULE_SENDCOMTC          3     /* EEptSendComTC()  Module Id */

#define EEPT_UNSOLI_OK                 0     /* complite unsoli request */
#define EEPT_UNSOLI_BUSY               1     /* incomplite unsoli request */

/**	Request Which EEPT_DLL to use	**/
// Since we may also have multiple EPT vendors we are setting this up
// as the first few types are mutually exclusive meaning that you will
// use either one or the other.  However other types, such as FreedomPay,
// may be used in combination with other EPT vendors.
#define EEPT_DLL_UNAVAILABLE		0		 /* DLL Interface Unavailable */
#define EEPT_DLL_DATACAP			1		 /* Datacap dll */
#define EEPT_DLL_MERCURY			2		 /* Mercury Payments dll */
#define EEPT_DLL_DATACAP_SECURE		3		 /* Datacap Secure dll with dsiPDCX and dszEMVX Out Of Scope */
#define EEPT_DLL_FREEDOMPAY     0x100        // FreedomPay EPT interface being used.

// The following defines are used to denote the type of PINPad and
// MSR device being used with the Electronic Payment Interface.
#define EEPT_PINPAD_UNAVAILABLE		    0		/* Pin Pad Not Set */
#define EEPT_PINPAD_2000			    1		/* Verifone pinpad 2000	*/
#define EEPT_PINPAD_EVEREST			    2		/* Verifone everest pinpad */
#define EEPT_PINPAD_1000			    3		/* Verifone pinpad 1000	*/
#define EEPT_PDC_WITHPINPAD			    4		/* PDC card reader with PinPad */
#define EEPT_TERMINAL_MSR			    5		// using Terminal MSR without a PINpad
#define EEPT_PINPAD_OPOS			    6		// using OPOS MSR and PinPad
#define EEPT_PINPAD_SECUREDEVICE	    7		// using Terminal MSR with Datacap Secure Device such as VX-805
//#define EEPT_VIRTUALKEY_FREEDOMPAY  0x100       // BL_DEVICE_DATA_VIRTUALKEY, VIRTUALKEY_BUFFER, VIRTUALKEY_EVENT_FREEDOMPAY

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

typedef struct  {
        USHORT         usLen;                  /* send data length     */
        EEPTREQUNION   auchReqData;            /* send data save area  */
}EEPTSAVESEND;

typedef struct  {
        UCHAR   auchData[EEPT_SPOOL_SIZE];           /* read data save area  */
        USHORT  usLen;                              /* read length          */
        SHORT   sErrCode;                           /* success or err code  */
}EEPTSPOOL;

typedef struct {
        UCHAR   uchHour;
        UCHAR   uchMin;
        UCHAR   uchSec;
} EEPTTIMER;

typedef struct {
    UCHAR       auchFaddr[PIF_LEN_IP];       /* IP Address */
	USHORT      usFaddrPort;                 // Port number at IP address
    USHORT      usHandle;                    /* NET work handle */
    UCHAR       fchStatus;                   /* NET work status */
} EEPTHOSTCONFIG;

typedef struct  {
        UCHAR   auchData[EEPT_SPOOL_SIZE];            /* read data save area  */
        USHORT  usLen;								  /* read length          */
        SHORT   sErrCode;                             /* success or err code  */
}MERCURYEEPTSPOOL;


/* for the EEPT R2.0 RS232C LOG Log (R2.0) */

typedef struct {
    USHORT  usThreadId;                     /* thread Id */
    UCHAR   uchModuleId;                    /* module Id */
    UCHAR   uchMin;                         /* log time(minutes)    */
    UCHAR   uchSec;                         /* log time(second)     */
    UCHAR   uchStatus;                      /* uchEEptStatus */
    UCHAR   uchSubStatus;                   /* uchSendStatus/uchReceiveStatus */
    SHORT   sRetCode;                       /* Return code */
    USHORT  usLen;                          /* send/receive data length */
    UCHAR   auchData[EEPT_LOG_DATA_SIZE];    /* send/receive data */
} EEPT_LOGITEM;

typedef struct {
    UCHAR   uchExecuteFlag;                 /* logging execute flag        */
    USHORT  usCurrentLogId;                 /* Current log id              */
    USHORT  usSendthreadId;                 /* Send thread Id save area    */
    USHORT  usReceivethreadId;              /* Receive thread Id save area */
    EEPT_LOGITEM aEEptLogItem[EEPT_MAX_LOG_NUMBER];
} EEPT_232C_LOG;

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/**     protocol       **/
extern  PROTOCOL    EEptProtocol;                    /* protocol for EEPT     */

/**     semaphore       **/
extern  USHORT  usEEptSemMain;                       /* for access to stub   */
extern  USHORT  usEEptSemSend;                       /* for start send thred */
extern  USHORT  usEEptSemRcv;                        /* for inform user I/F  */
extern  USHORT  usEEptSemSpool;                      /* for EEptSpool         */
extern  USHORT  usEEptSemState;                      /* for uchEEptStatus     */
extern  USHORT  usEEptSemHandle;                     /* for hsEEptPortHandle  */

/**     RS-232 Port     **/
extern  CHAR    chEEptPortID;                        /* RS-232 Port connect# */
extern  SHORT   hsEEptPortHandle;                    /* port handle saved    */
extern  SHORT   hsEEptSaveHandle;                    /* port handle backup   */

/**     IHC Port     **/
extern  SHORT   hsEEptNetHandle;                     /* save network handle  */

/**     EEPT STATUS      **/
extern  UCHAR  uchEEptStatus;                        /* current state of EEPT */

/**     SEND DATA       **/
extern  EEPTSAVESEND EEptSaveSend;                    /* send data save area  */
extern  USHORT      usEEptSeqNo;                     /* request data seq no. (R2.0) */

/**     RECEIVE DATA    **/
extern  EEPTSPOOL EEptSpool;                          /* receive data spool   */

/**     send time       **/
extern  EEPTTIMER    EEptSendTOD;                     /* message send time    */

/**     EEPT SUB STATUS  (R2.0) **/
extern  UCHAR   uchEEPTReceiveStatus;                   /* Receive thread sub status */
extern  UCHAR   uchEEPTSendStatus;                      /* Send Thread sub Status    */




#ifdef DEBUG
/**     LOGGING (R2.0)  **/
extern  EEPT_232C_LOG aEEptLog;                       /* RS-232/C data log    */
#endif

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* -- EEPTIF.C -- */

/* -- EEPTRCV.C -- */
SHORT   EEptCreateMessage(VOID);
SHORT   EEptReceiveCom(SHORT sCurrent);
SHORT   EEptEEptReceiveCom(SHORT sCurrent);                         /* (R2.0) */
SHORT   EEptOpenSetPort(USHORT usWaitTimer);
SHORT   EEpt1ByteTimeOut(VOID);
SHORT   EEpt1BytePowerOff(VOID);
SHORT   EEptCheckMessageType(UCHAR *pData);
SHORT   EEptSetReadTimer(SHORT sCurrent);
SHORT   EEptRecoverPort(VOID);
SHORT   EEptExeResponse(VOID);
SHORT   EEptExeUnsoli(UCHAR uchState);
VOID    EEptExeError(SHORT sErrCode);
SHORT   EEptCreateUnsMessage(VOID);                                /* (R2.0) */
VOID	EEptSetReceiveRS232(EEPTRSPDATA_v14* pEEPTRespv14, EEPTRSPDATA *pEEPTResp);

/* -- EEPTSEND.C -- */
SHORT   EEptSendCom(VOID);
SHORT   EEptEEptSendCom(VOID);                                      /* (R2.0) */

/* -- EEPTSUB.C -- */
VOID    EEptSetProtocol(VOID);
USHORT  EEptGetMDCTime(USHORT uchAddress);
SHORT   EEptCheckComError(SHORT sRet, USHORT usReqLen);                /* (R2.0) */
VOID    EEptWriteStatus(UCHAR uchState);
USHORT  EEptComparePrevious(UCHAR *pSendData, USHORT usSendLen);
USHORT  EEptCheckSpoolData(VOID);
USHORT  EEptCheckLeftTime(VOID);
SHORT   EEptSendComTC(UCHAR ucCode);                                    /* (R2.0) */
SHORT   EEptEEptSendComTC(UCHAR ucCode);                                /* (R2.0) */
VOID    EEptLog(UCHAR uchoduleId,SHORT sRet,UCHAR uchTC);              /* (R2.0) */

// EEPTHOSTIF.c
VOID	EEptHostOpen(VOID);
SHORT   EEptHostRecv(VOID *pEEptHostSndBuf, USHORT usLen);
SHORT   EEptHostSend(VOID *pEEptHostSndBuf, USHORT usLen);

// EEPTDLLif.c
VOID    XEptDllOpen(TCHAR* tchDeviceName, TCHAR* tchDllFileName);
VOID    XEptDllClose(UCHAR uchEEPTDLL);
BOOL	XEptDllCheckPinPad();
VOID	XEptDllSetCurrentDLL(UCHAR uchEEPTDLL);
USHORT	XEptDllGetPinPadType();
SHORT   XEptDllSend(VOID *pEEptDllSndBuf, USHORT usLen);
SHORT	XEptStoreForwardReport(short sTypeDsi, EEPTF_HEADER * S_F_Data);
ULONG	XEptCheckAndRecoverFile(short sTypeDsi);
SHORT	XEptStartForwardThread();
SHORT	XEptToggleStoreForward(SHORT onOff);
SHORT   XEptDllRecv(VOID *pEEptRcvBuf, USHORT usLen);
SHORT	XEptDllCollectStripe(SHORT sCardTimeOut, BOOL bScreen);
SHORT	XEptDllCollectStripeAndPin(TCHAR* strAmount, SHORT sCardTimeOut, SHORT sPinTimeOut, BOOL bScreen, UCHAR uchDecimal);
SHORT	XEptDllCollectPin(TCHAR* strAmount, TCHAR* strAcctNo, SHORT sPinTimeOut, BOOL bScreen, UCHAR uchDecimal);
VOID	XEptDllGetLastStripe(TCHAR* tchMagStripe);
VOID	XEptDllResetPinPad();
VOID	XEptDllCancelRequest(VOID);
VOID	XEptSetUpMaskInformation(USHORT usTenderNo, ULONG *plMaskData);
LONG	XEptGetNextBlock(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader);
LONG	XEptGetNextBlockReset(short sTypeDsi);
LONG	XEptGetNextBlockIterateAll(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader);
LONG	XEptGetNextBlockIterate(short sTypeDsi, EEPT_BLOCK_HEADER * blockHeader);
LONG	XEptGetSpecifiedBlockHeader(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
LONG	XEptUpdateSpecifiedBlock(short sTypeDsi, ULONG ulBlockOffset, EEPT_BLOCK_HEADER * blockHeader);
VOID	XEptSetUpGiftMaskInfo(ULONG *plMaskData);

#endif
/* ---------- End of EEPTL.H ---------- */
