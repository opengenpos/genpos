/****************************************************************************\
||                                                                          ||
||        *=*=*=*=*=*=*=*=*                                                 ||
||        *  NCR 7450 POS *             AT&T GIS Japan, E&M OISO            ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995              ||
||    <|\/~               ~\/|>                                             ||
||   _/^\_                 _/^\_                                            ||
||                                                                          ||
\****************************************************************************/

/*==========================================================================*\
*   Title:
*   Category:
*   Program Name:
* ---------------------------------------------------------------------------
*   Compiler:       MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model:
*   Options:
* ---------------------------------------------------------------------------
*   Abstract:
*
* ---------------------------------------------------------------------------
*   Update Histories:
* ---------------------------------------------------------------------------
*   Date     | Version  | Descriptions                          | By
* ---------------------------------------------------------------------------
*            |          |                                       |
\*==========================================================================*/

/*==========================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:   $Revision$
:   $Date$
:   $Author$
:   $Log$
\*==========================================================================*/

/*==========================================================================*\
;+                                                                          +
;+                  G l o b a l    D e f i n i t i o n                      +
;+                                                                          +
\*==========================================================================*/

/* --- check target module --- */

#if !defined(SARATOGA) && !defined(SARATOGA_PC) && !defined (POS7450) && !defined (POS2170) && !defined (SERVER_NT)
# error Target module name undefined !
#endif


#ifdef __cplusplus
extern "C"{
#endif


#if defined(SARATOGA)
#ifdef PIFNET_EXPORTS
#define PIFNET_API __declspec(dllexport)
#else
#define PIFNET_API __declspec(dllimport)
#endif
#endif
#if defined(SARATOGA_PC)
#ifdef PIFNETPC_EXPORTS
#define PIFNET_API __declspec(dllexport)
#else
#define PIFNET_API __declspec(dllimport)
#endif
#endif

/* --- data structure control --- */

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

/* --- PifNetXxxx() function modifier --- */
#if defined(SARATOGA) 
# define    NETENTRY    PIFNET_API              /* PIF API entry keyword    */
#elif defined(SARATOGA_PC)
# define    NETENTRY    PIFNET_API              /* PIF API entry keyword    */
#elif defined (POS7450)
# define    NETENTRY    _cdecl _far _loadds     /* PIF API entry keyword    */
#elif defined (POS2170)
# define    NETENTRY    _cdecl _far _loadds     /* PIF API entry keyword    */
#elif defined (SERVER_NT)
# define    NETENTRY    _cdecl far              /* PIF API entry keyword    */
#endif

/* --- PifNetXxxx() definition --- */
#define     PIF_NET_BROADCAST   0xFFFFFFFFL     /* broadcast address        */
#define     PIF_NET_HANDLE      (USHORT)(-1)    /* network system handle    */

/* --- network module information packet --- */

typedef struct {                                /* NETINF structure         */
    PSZ     pszLocalPort;                           /* local port number    */
    PSZ     pszRemotePort;                          /* remote port number   */
    USHORT  usMaxUsers;                             /* no. of max. users    */
    USHORT  usMaxQueues;                            /* no. of max. pendings */
    USHORT  usMaxLength;                            /* max. data length     */
    USHORT  usWorkMemory;                           /* bytes for work memory*/
    USHORT  usIdleTime;                             /* idle time in msec.   */
} NETINF, *PNETINF;


#if 0
//Notes: The standard XGRAM structure is defined by the compatibility against the old
//      PifNetXxxx() module. However, the following structure using with the
//      macro XGSIZE() is desirable for the smart PifNetXxxx() algorithm.
typedef struct {                                /* XGRAM structure          */
    UCHAR   auchFaddr[PIF_LEN_IP];                  /* ULONG format IP      */
    USHORT  usFport;                                /* foreign port no.     */
    USHORT  usLport;                                /* local port number    */
    UCHAR   auchData[1];                            /* data area            */
} XGRAM, *PXGRAM;
#endif
                                            /* structure member's offset    */
#define     OFFSETMBR(member, type) (USHORT)(&(((type *)NULL)->member))
                                            /* compute proper size of buffer*/
#define     XGSIZE(x)               (USHORT)((x) + OFFSETMBR(auchData, XGRAM))

/* --- general address --- */

typedef struct {                                /* general address          */
    UCHAR   auchAddr[PIF_LEN_IP];               /* ULONG format IP      */
    USHORT  usIpPortNo;                         /* UDP/IP port number   */
    USHORT  usPort;                             /* port no.             */
} XGADDR,  *PXGADDR;

/* --- LOG function prototype --- */

#if defined(SARATOGA)
typedef SHORT (_cdecl FAR *PFUNLOG)(USHORT, /* module ID            */
                                            USHORT, /* exception code       */
                                            USHORT, /* severity             */
                                            BOOL,   /* option               */
                                            SHORT,  /* status code          */
                                            PSZ);   /* ptr. error message   */
#elif defined (SARATOGA_PC)
typedef SHORT (_cdecl FAR *PFUNLOG)(USHORT, /* module ID            */
                                            USHORT, /* exception code       */
                                            USHORT, /* severity             */
                                            BOOL,   /* option               */
                                            SHORT,  /* status code          */
                                            PSZ);   /* ptr. error message   */
#else
typedef SHORT (_cdecl FAR _loadds *PFUNLOG)(USHORT, /* module ID            */
                                            USHORT, /* exception code       */
                                            USHORT, /* severity             */
                                            BOOL,   /* option               */
                                            SHORT,  /* status code          */
                                            PSZ);   /* ptr. error message   */
#endif


#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

/*==========================================================================*\
;+                                                                          +
;+                      D I A G    I N T E R F A C E                        +
;+                                                                          +
\*==========================================================================*/

// This header will be referred by all of modules including server module
// based on WindowsNT operating system. Therefore, definitions concerning
// to interface via network cannot take the INT and BOOL data types.
// Moreover, the alignment of each structures mush be packed.

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

/* --- remote function I/F message --- */

typedef struct _RMTFNC {                    /* remote call function         */
    USHORT  usSign;                             /* signature                */
    USHORT  usAux;                              /* auxiliary (status)       */
    USHORT  usBytes;                            /* data length following    */
    UCHAR   auchData[1];                        /* top of data              */
} RMTFNC, *PRMTFNC;

#define     PIF_NET_REMOTE_CALL     0xFFFF  /* signature                    */

/* --- network tallies --- */
// changes to following structs must be synchronized with changes to PIF_NETSYSTALLY and PIF_NETUSERTALLY
typedef struct _SYSTLY {                    /* system tally. Same as PIF_NETSYSTALLY */
    ULONG   ulTotalSent;                        /* no. of sent totally      */
    ULONG   ulSentError;                        /* no. of sent errors       */
    USHORT  usLastSentError;                    /* last sent error code     */
    ULONG   ulTotalRecv;                        /* no. of recv totally      */
    ULONG   ulRecvError;                        /* no. of recv errors       */
    USHORT  usLastRecvError;                    /* last sent error code     */
    ULONG   ulOutOfResources;                   /* no. of out of resources  */
    ULONG   ulIllegalMessage;                   /* no. of illegal messages  */
    ULONG   ulRecvUndeliver;                    /* no. of recv undeliverable*/
    ULONG   ulRecvLostTimeOut;                  /* no. of recv lost due to time expired */
    ULONG   ulRecvLostQueueFull;                /* no. of recv lost due to queue full */
    ULONG   ulRecvLostResources;                /* no. of recv lost due to insufficient resources */
    ULONG   ulRecvLostNetClosed;                /* no. of recv lost due to network close */
    ULONG   ulAckSentStatusBusy;                /* no. of recv lost due to time out with an Ack sent status busy */
    ULONG   ulSendUnreachableError;             /* no. of send errors due to address unreachable */
} SYSTLY, *PSYSTLY;

typedef struct _USRTLY {                    /* user tally. Same as PIF_NETUSERTALLY  */
    USHORT  usTotalSent;                        /* no. of sent totally      */
    USHORT  usSentError;                        /* no. of sent errors       */
    USHORT  usTotalRecv;                        /* no. of recv totally      */
    USHORT  usRecvError;                        /* no. of recv errors       */
    USHORT  usRecvLost;                         /* no. of recv lost         */
	USHORT  usPendingHighWater;                 // max number of pending for this user
    USHORT  usRecvLostTimeOut;                  /* no. of recv lost due to time expired */
    USHORT  usRecvLostQueueFull;                /* no. of recv lost due to queue full */
    USHORT  usRecvRequestTimeOut;               /* no. of recv requests that timed out without a message coming in */
    USHORT  usSendRequestTimeOut;               /* no. of send requests that timed out without ack being returned */
    USHORT  usSendTargetBusy;                   /* no. of send requests that returned ST_TARGET_BUSY ack being returned */
    USHORT  usRecvTargetClear;                  /* no. of recv requests that were cleared from the receive queue */
    USHORT  usSendTargetClear;                  /* no. of send requests that returned ST_TARGET_CLEAR ack being returned */
    USHORT  usRecvPutOnQueue;                   /* no. of received messages put on the user's queue */
    USHORT  usRecvGivenNow;                     /* no. of received messages given to a pending receive and not put on queue */
    USHORT  usRecvBadSeqNoPort;                 /* no. of received messages that have a bad sequence number or wrong port */
    USHORT  usRecvTakenOffQueue;                /* no. of received messages taken off the user's queue (actual number given to user) */
    USHORT  usRecvOutOfResources;               /* no. of received messages received that could not be queue due to lack of resources */
    USHORT  usAckSentStatusBusy;                /* no. of Ack messages sent with a status of ST_TARGET_BUSY */
    USHORT  usRecvLostNetClosed;                /* no. of messages removed from queue due to Net Closed */
    USHORT  usRecvRequestWaitStart;             /* no. of message requests with no input waiting so put on wait queue */
    USHORT  usRecvMessageAlreadyExist;          /* no. of receive message requests with buffer already allocated and assigned */
    USHORT  usSendMessageAlreadyExist;          /* no. of send message requests with buffer already allocated and assigned */
    USHORT  usExeNetSendCount;                  /* no. of calls to function ExeNetSend() */
    USHORT  usExeNetReceiveCount;               /* no. of calls to function ExeNetReceive() */
    USHORT  usTimerCheckSkipped;                /* no. of times TimerProtocol() found pending receive and message on the receive queue */
    USHORT  usTimerSendWithRecvPending;         /* no. of times TimerProtocol() found pending send and message on the receive queue, Ack missed? */
} USRTLY, *PUSRTLY;

/* --- diag request message structure --- */

typedef struct _DGDATA {                    /* diag request message structre*/
    USHORT  usFunc;                             /* function code            */
    USHORT  usBytes;                            /* optional data length     */
    UCHAR   auchData[1];                        /* top of optional data     */
} DGDATA, *PDGDATA;

typedef struct _DGINFO {                    /* diag resp. message structre  */
    USHORT  usFunc;                             /* function code            */
    USHORT  usBytes;                            /* optional data length     */
    UCHAR   auchData[1];                        /* top of optional data     */
} DGINFO, *PDGINFO;

#define     DIAG_GET_SYSTEM         1       /* get system info.             */
#define     DIAG_GET_USER           2       /* get user info.               */
#define     DIAG_SET_VALUE          3       /* set optional parameters      */
#define     DIAG_CHK_MEMORY         4       /* check memory                 */

/* --- system / user info. interface --- */

typedef struct _REQINF {                    /* request for user info.       */
    USHORT      usUserPort;                     /* user port no.            */
    USHORT      fResetTally;                    /* reset tally ?            */
} REQINF, *PREQINF;

typedef struct _USRINF {                    /* user information             */
    USHORT      usHandle;                       /* network handle           */
    USHORT      usPortNo;                       /* user port no.            */
} USRINF, *PUSRINF;

typedef struct _RSPSYS {                    /* response for system ino.     */
    SYSTLY      infTally;                       /* system tally             */
    USHORT      usPortLocal;                    /* my port no. on UDP/IP    */
    USHORT      usPortRemote;                   /* remote port no. on UDP/IP*/
    ULONG       ulSocket;                       /* socket handle            */
    USHORT      usIdleTime;                     /* idle time in sec.        */
    USHORT      usMaxQueues;                    /* max. no. of queues       */
    USHORT      usMaxSize;                      /* buffer size in byte      */
    USHORT      usMaxUsers;                     /* no. of max. users        */
    USHORT      usNoOfUsers;                    /* no. of current users     */
    USRINF      ainfUsers[1];                   /* user info.               */
} RSPSYS, *PRSPSYS;

typedef struct _RSPUSR {                    /* response for user ino.       */
    USRTLY      infTally;                       /* user tally               */
    USHORT      fsUserFlags;                    /* user option flags        */
    ULONG       ulRemoteAddr;                   /* target IP address        */
    USHORT      usRemotePort;                   /* target port number       */
    USHORT      usLocalPort;                    /* user's port number       */
    USHORT      usSendTimer;                    /* send timer value in msec */
    ULONG       ulRecvTimer;                    /* recv timer value in msec */
    USHORT      fsUserState;                    /* current state            */
    USHORT      usNoOfPending;                  /* no. of pendings          */
} RSPUSR, *PRSPUSR;

#define     STATE_IN_SEND       0x0001      /* in sending state             */
#define     STATE_IN_RECV       0x0002      /* in receiving state           */

/* --- set new value interface --- */

typedef struct _REQSET {                    /* request for set value        */
    USHORT  usUserPort;                         /* user port no.            */
    USHORT  fsSetWhat;                          /* set what ?               */
    USHORT  usSendTimer;                        /* send timer in msec       */
    ULONG   ulRecvTimer;                        /* recv timer in msec       */
    USHORT  usIdleTimer;                        /* idle timer in msec.      */
    USHORT  usMaxQueues;                        /* no. of max. queues       */
    USHORT  usRemotePort;                       /* remote port no.          */
} REQSET, *PREQSET;

#define     SET_SEND_TIMER      0x0001      /* set send timer               */
#define     SET_RECV_TIMER      0x0002      /* set recv timer               */
#define     SET_IDLE_TIMER      0x0004      /* set idle time value          */
#define     SET_MAX_QUEUES      0x0008      /* set no. of max. queues       */
#define     SET_REMOTE_PORT     0x0010      /* set remote port no.          */

/* --- check memory interface --- */

typedef struct _RSPMEM {                    /* response check memory        */
    USHORT  fHeapFine;                          /* has integrity ?          */
    ULONG   ulInitialSize;                      /* initial memory size      */
    ULONG   ulUsedMemory;                       /* no. of bytes used        */
    ULONG   ulFreeMemory;                       /* no. of bytes free        */
    ULONG   ulMaxAlloc;                         /* no. of byte allocatable  */
} RSPMEM, *PRSPMEM;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */


/*==========================================================================*\
;+                                                                          +
;+      P R O T O T Y P E s   o f   P U B L I C   P R O G R A M s           +
;+                                                                          +
\*==========================================================================*/

/* ---- network routines --- */
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, VOID FAR *, ULONG);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);
# define    _h                          PIF_NET_HANDLE
# define    PifNetSetStandby()          NetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)         NetControl(_h, PIF_NET_DIAG, x, y, z)

/* --- data conversion routines --- */
USHORT  ExchangeUshort(USHORT);                 /* exchange USHORT data     */
ULONG   ExchangeUlong(ULONG);                   /* exchange ULONG data      */

#ifdef __cplusplus
}
#endif

/*==========================================================================*\
;+                      E n d    O f    H e a d e r                         +
\*==========================================================================*/
