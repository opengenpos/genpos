/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*                                                 **
**        *  NCR 2170   *              NCR Corporation, E&M OISO          **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993              **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Charge Posting Manager, Local Header File
* Category    : Charge Postig Manager, NCR 2170 Charge Posting Manager Application
* Program Name: CPMLOCAL.H
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
*
*
*** NCR2172 ***
*
* Oct-04-99 01.00.00  : M.Teraki      : Initial
*                                       Add #pragma pack(...)
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


#define     CPM_SEND                0           /* send task                */
#define     CPM_RECEIVE             1           /* receive task             */

/**     RS-232 Port control   **/
#define     CPM_PORT_USENET         -2          /* use LAN network rather than RS-232  */
#define     CPM_PORT_NONE           -1          /* not exist RS-232 port    */
#define     CPM_RETRY_COUNT         3           /* retry counter            */
#define     CPM_WAIT_PORT           3           /* retry counter for port   */
#define     CPM_WAIT_PORT_TIME      5           /* retry timer 5 sec.       */

/**
		status of Charge Post Manager machine

	These states are used to indicate the current status of the Charge Post Manager
	state machine which is processing user requests to the Charge Post Host as well
	as processing unsolicited status messages from the Charge Post Host.

	Function CpmWriteStatus () is used to update the CPM status (as indicated by the
	global variable uchCpmStatus) which is guarded by the semaphore usCpmSemState.

	See the function CpmExeUnsoli () which contains the logic for dealing with CPM Host
	unsolicited messages and modifies the uchCpmStatus status variable accordingly.

	See the function CpmSendMessage () which shows a user request being sent and how the
	status variable uchCpmStatus is checked to determine if the send should fail or not
	depending on the status of the CPM state machine.  It also shows the status being set
	to indicate that a user request is being sent to the CPM Host.

	See the function CpmExeResponse () which shows the handling of the response from the
	CPM Host and the possible forwarding of the response to another terminal if the original
	request came from some other terminal.
 **/
#define     CPM_STATE_STOP          0	// indicates the CPM is in a stopped state and is not processing user requests
#define     CPM_STATE_OUTSIDE       1	// indicates the CPM is waiting for the next user request
#define     CPM_STATE_SEND_OK       2   // indicates the CPM is waiting on a response to a request sent to the CPM Host
#define     CPM_STATE_INSIDE        3   // indicates the CPM is sending a user request to the CPM Host

/**     buffer(send/spool) control  **/
#define     CPM_SEND_SIZE   sizeof(CPMRCREQDATA)    /* size of send area    */
#define     CPM_SPOOL_SIZE  sizeof(CPMRCRSPDATA)    /* size of spool area   */

#define     CPM_FIX_TIME            1           /* timer during spool msg   */
#define     CPM_SLEEP_TIME          1           /* timer from getting port  */
#define     CPM_SLEEP_COUNT         10          /* counter from getting port*/

#define     CPM_START_CHAR       0x20           /* received data start char */
#define     CPM_END_CHAR         0x0d           /* received data end char   */

/**     response control  **/
#define     CPM_NOISE               0           /* read 1byte data is noize */ 
#define     CPM_CONT                1           /* message continued        */ 
#define     CPM_END                 2           /* message end              */
 
#define     CPM_TYPE_USER_RESP      0           /* response data for user   */ 
#define     CPM_TYPE_ERR_RESP       1           /* other data               */
#define     CPM_TYPE_UNSOLI         2           /* unsoli. data from PMS    */ 

/**     other control    **/
#define     CPM_SAME                0           /* current & prev. is same  */
#define     CPM_NOT_SAME            1           /* current & prev. not same */

#define     CPM_PASS                0           /* timer value OK           */
#define     CPM_READ_AGAIN          0           /* PifReadCom() again       */
#define     CPM_MAX_SET_TIMER       60          /* max time for Pifsleep(),
                                                   PifControlCom()          */

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


typedef struct  {
        UCHAR   auchData[CPM_SEND_SIZE];            /* send data save area  */
        USHORT  usLen;                              /* send data length     */
        UCHAR   uchTerminal;                        /* from Self/Not Self   */
}CPMSAVESEND;

typedef struct  {
        UCHAR   auchData[CPM_SPOOL_SIZE];           /* read data save area  */
        USHORT  usLen;                              /* read length          */
        SHORT   sErrCode;                           /* success or err code  */
}CPMSPOOL;

typedef struct {
        UCHAR   uchHour;
        UCHAR   uchMin;
        UCHAR   uchSec;
} CPMTIMER;

typedef struct {
    UCHAR       auchFaddr[PIF_LEN_IP];       /* IP Address */
	USHORT      usFaddrPort;                 // Port number at IP address
    USHORT      usHandle;                    /* NET work handle */
    UCHAR       fchStatus;                   /* NET work status */
} CPMHOSTCONFIG;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/**     protocol       **/  
extern  PROTOCOL    CpmProtocol;                    /* protocol for CPM     */

/**     semaphore       **/  
extern  USHORT  usCpmSemMain;                       /* for access to stub   */
extern  USHORT  usCpmSemSend;                       /* for start send thred */
extern  USHORT  usCpmSemRcv;                        /* for inform user I/F  */
extern  USHORT  usCpmSemSpool;                      /* for CpmSpool         */
extern  USHORT  usCpmSemState;                      /* for uchCpmStatus     */
extern  USHORT  usCpmSemHandle;                     /* for hsCpmPortHandle  */

/**     RS-232 Port     **/  
extern  CHAR    chCpmPortID;                        /* RS-232 Port connect# */
extern  SHORT   hsCpmPortHandle;                    /* port handle saved    */
extern  SHORT   hsCpmSaveHandle;                    /* port handle backup   */

/**     IHC Port     **/  
extern  SHORT   hsCpmNetHandle;                     /* save network handle  */

/**     CPM STATUS      **/  
extern  UCHAR  uchCpmStatus;                        /* current state of CPM */

/**     SEND DATA       **/  
extern  CPMSAVESEND CpmSaveSend;                    /* send data save area  */

/**     RECEIVE DATA    **/  
extern  CPMSPOOL CpmSpool;                          /* receive data spool   */

/**     send time       **/  
extern  CPMTIMER    CpmSendTOD;                     /* message send time    */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* -- CPMIF.C -- */
VOID    CpmInitialArea(VOID);
SHORT   CpmGetComPort(VOID);

/* -- CPMRCV.C -- */
SHORT   CpmCreateMessage(VOID);
SHORT   CpmReceiveCom(SHORT sCurrent);
SHORT   CpmOpenSetPort(USHORT usWaitTimer);
SHORT   Cpm1ByteTimeOut(VOID);
SHORT   Cpm1BytePowerOff(VOID);
SHORT   CpmCheckMessageType(UCHAR *pData);
SHORT   CpmSetReadTimer(SHORT sCurrent);
SHORT   CpmRecoverPort(VOID);
VOID    CpmExeResponse(VOID);
VOID    CpmExeUnsoli(UCHAR uchState);
VOID    CpmExeError(SHORT sErrCode);

/* -- CPMSEND.C -- */
SHORT   CpmSendCom(VOID);
SHORT   CpmWaitPortRecovery(VOID);


/* -- CPMSUB.C -- */
VOID    CpmSetProtocol(VOID);
USHORT  CpmGetMDCTime(UCHAR uchAddress);
SHORT   CpmCheckComError(SHORT sRet, USHORT usReqLen, UCHAR uchTask);
VOID    CpmWriteStatus(UCHAR uchState);
USHORT  CpmComparePrevious(UCHAR *pSendData, USHORT usSendLen);
USHORT  CpmCheckSpoolData(VOID);
USHORT  CpmCheckLeftTime(VOID);

/* -- CPMNET.C -- */
VOID    CpmReturnMessage(UCHAR *pMessage, USHORT usLen);
VOID    CpmReturnErrMessage(SHORT sErrCode);
VOID    CpmNetOpen(VOID);
VOID    CpmNetClose(VOID);
SHORT   CpmNetSend(USHORT usLen);

// CPMHOSTIF.c
VOID	CpmHostOpen(VOID);
SHORT   CpmHostRecv(VOID *pCpmHostSndBuf, USHORT usLen);
SHORT   CpmHostSend(VOID *pCpmHostSndBuf, USHORT usLen);

/* ---------- End of CpmLocal.H ---------- */
