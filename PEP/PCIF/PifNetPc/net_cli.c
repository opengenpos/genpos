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
;+                  I N C L U D E    F I L E s                              +
;+                                                                          +
\*==========================================================================*/

# include   <windows.h>                         /* Windows header           */
# include   "r20_pif.h"

/* --- C-library header --- */

#include    "piftypes.h"                    /* type modifier header         */
#include    <memory.h>                      /* memxxx() C-library header    */
#include    <stdlib.h>                      /* standard library header      */

#include    <stdio.h>

/* --- our common headers --- */
#include    "pifnet.h"                      /* PifNet common header         */
#include    "netmodul.h"                    /* my module header             */

/*==========================================================================*\
;+                                                                          +
;+                  L O C A L     D E F I N I T I O N s                     +
;+                                                                          +
\*==========================================================================*/

/* --- some macros --- */
                                            /* compute tick counts          */
#define     TICK(m, t)              (((m) / (t)) + (((m) % (t)) ? 1 : 0))
#define     LASTERROR(last, new)    ((last) ? (last) : (new))

/* --- network manager thread I/F --- */

typedef struct _NETARG {                    /* argument for NetManager()    */
    USHORT  usStatus;                           /* initialized status       */
    EVTOBJ  evtAck;                             /* acknowledge signal       */
    USHORT  usQueue;                            /* ID of my queue           */
    PSZ     pszLocalPort;                       /* my port no. on UDP/IP    */
    PSZ     pszRemotePort;                      /* server's port no. on UDP */
    USHORT  usMaxLength;                        /* maximum data length      */
    USHORT  usMaxUsers;                         /* max. no. of users        */
    USHORT  usMaxQueues;                        /* max. no. of queues       */
    USHORT  usWorkMemory;                       /* bytes for working memory */
    USHORT  usIdleTime;                         /* idle time in msec.       */
} NETARG, *PNETARG;

/* --- continuous receiver thread I/F --- */

typedef struct _RCVARG {                    /* argument for NetReceiver()   */
    USHORT  usStatus;                           /* initialized status       */
    EVTOBJ  evtAck;                             /* acknowledge signal       */
    USHORT  usQueue;                            /* ID of my queue           */
    INT     iSocket;                            /* handle of socket         */
    USHORT  usIdle;                             /* idle timer value in msec */
} RCVARG, *PRCVARG;

/* --- sleep timer thread I/F --- */

typedef struct _TMRARG {                    /* argument for SleepTimer()    */
    USHORT  usStatus;                           /* initialized status       */
    EVTOBJ  evtAck;                             /* acknowledge signal       */
    USHORT  usQueue;                            /* ID of my queue           */
} TMRARG, *PTMRARG;

/* --- SendAckMessage() I/F --- */

typedef struct _ACKDAT {                    /* ack. message data            */
    ULONG       ulTargetAddr;                   /* target IP address        */
    USHORT      usIpPortNo;                     /* UDP/IP port number       */
    USHORT      usTargetPort;                   /* target port no.          */
    USHORT      usNumber;                       /* message seq. no.         */
    USHORT      usLocalPort;                    /* local port no.           */
    USHORT      usStatus;                       /* status code              */
} ACKDAT, *PACKDAT;

/* --- user management table --- */

typedef struct _SNDINF {                    /* send message info.           */
    USHORT      usSeqNo;                        /* send sequence counter    */
    USHORT      usTargetPort;                   /* target port no.          */
    ULONG       ulTargetAddr;                   /* target address           */
} SNDINF, FAR *PSNDINF;

typedef struct _USRTBL {                    /* user management table        */
    USHORT      fsFlags;                        /* optional flags           */
    ULONG       ulRemoteAddr;                   /* target IP address        */
    USHORT      usRemotePort;                   /* target port number       */
    USHORT      usLocalPort;                    /* user's port number       */
    USHORT      usSendTimer;                    /* send timer value in msec */
    USHORT      usSendTick;                     /* last receive time out    */
    ULONG       ulRecvTimer;                    /* recv timer value in msec */
    ULONG       ulRecvTick;                     /* last receive time out    */
    PMSGHDR     pSendMessage;                   /* ptr. to send req. message*/
    PMSGHDR     pRecvMessage;                   /* ptr. to recv req. message*/
    SNDINF      infSending;                     /* send message info.       */
    QUEPTR      queReceived;                    /* received message queue   */
#if defined (NET_TALLY)
    USRTLY      infTally;                       /* tally area               */
#endif
} USRTBL, FAR *PUSRTBL;

typedef struct _RCVPND {                    /* receive pending message      */
    QUEBUF      queControl;                     /* queue control area       */
    BOOL        fAck;                           /* datagram ack. required   */
    USHORT      usSeqNo;                        /* sequence counter         */
    USHORT      usSourcePort;                   /* sender's port no.        */
    ULONG       ulSourceAddr;                   /* sender's IP address      */
    USHORT      usPendTick;                     /* timer value to live      */
    USHORT      usBytes;                        /* data length in byte      */
    UCHAR       auchMessage[1];                 /* top of message area      */
                                                /* & data area usBytes - 1  */
} RCVPND, FAR *PRCVPND;

/* --- function control packet --- */

typedef struct _PKTINF {                    /* function information packet  */
    PMSGHDR     pRequest;                       /* ptr. to a req. message   */
    USHORT      usManager;                      /* manager's queue ID       */
    PUSRTBL     pUsers;                         /* user management table    */
    USHORT      usUsers;                        /* no. of table arrays      */
    HMEM        hMemory;                        /* memory handle            */
    USHORT      usTick;                         /* time / tick              */
    USHORT      usPortLocal;                    /* my port no. on UDP/IP    */
    USHORT      usPortRemote;                   /* remote port no. on UDP/IP*/
    INT         iSocket;                        /* socket handle            */
    USHORT      usIdleTime;                     /* idle time in sec.        */
    USHORT      usMaxQueues;                    /* max. no. of queues       */
    USHORT      usSizeBuffer;                   /* buffer size in byte      */
    USHORT      usSendReservedBytes;            /* send reserved data bytes */
    USHORT      usRecvReservedBytes;            /* recv reserved data bytes */
    LPVOID      pvSendBuffer;                   /* ptr. to a send buffer    */
    LPVOID      pvRecvBuffer;                   /* ptr. to a receive buffer */
#if defined (NET_TALLY)
    SYSTLY      infTally;                       /* tally area               */
#endif
} PKTINF, *PPKTINF;

/* --- kernel debugging information --- */
#if defined (NET_DEBUG)
typedef struct _DBGINF {                    /* kernel debugging info.       */
    USHORT      idThread;                       /* thread ID                */
    USHORT      usDS;                           /* DS register              */
    USHORT      usSS;                           /* SS register              */
    USHORT      usCS;                           /* CS register              */
    USHORT      usState;                        /* present state            */
    USHORT      usLoops;                        /* no. of loops             */
} DBGINF, *PDBGINF;

#define DBG_STATE_READ_QUEUE    (('Q' << 8) + 'R')  /* reading a mail       */
#define DBG_STATE_WRITE_QUEUE   (('Q' << 8) + 'W')  /* writing a mail       */
#define DBG_STATE_DOS_SLEEP     (('L' << 8) + 'S')  /* sleeping             */
#define DBG_STATE_DEAD          (('E' << 8) + 'D')  /* dead                 */
#define DBG_STATE_RECVFROM      (('F' << 8) + 'R')  /* socket recvfrom()    */
#endif

/*==========================================================================*\
;+                                                                          +
;+      N E T W O R K    P R T O C O L     D E F I N I T I O N s            +
;+                                                                          +
\*==========================================================================*/

/* -------------------------------------------- *\
 +  Message header should be packed every byte. +
\* -------------------------------------------- */

#pragma pack(1)                             /* should be packed every byte  */
/*
 +  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+- - -+---+
 +  | ID| SP| NO| DP| BC|  BLI1 |BT1|  BLI2 |BT2|  BLI3 |BT3|     Data    |
 +  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+- - -+---+
*/

/* --- MPH header --- */

typedef struct _MPHHDR {                    /* MPH header contents          */
    UCHAR   uchId;                              /* message ID               */
    UCHAR   uchSp;                              /* source process ID        */
    UCHAR   uchNo;                              /* message sequence no.     */
    UCHAR   uchDp;                              /* destination process ID   */
    UCHAR   uchBc;                              /* buffer counts            */
} MPHHDR, FAR *PMPHHDR;

#define     ID_REPLY_BIT    0x80            /* reply bit in ID field        */
#define     ID_MESSAGE      0x1C            /* message data in ID field     */
#define     BC_MAX_COUNTS      3            /* max. no. of buffers / message*/

/* --- buffer control --- */

typedef struct _BUFCNT {                    /* buffer control               */
    USHORT  usLength;                           /* buffer length indicator  */
    UCHAR   uchType;                            /* buffer type              */
} BUFCNT, FAR *PBUFCNT;

#define     BT_FUNCTION     1               /* function data                */
#define     BT_STATUS       2               /* status data                  */
#define     BT_USER_DATA    4               /* user data                    */

/* --- function buffer --- */

typedef struct _FNCBUF {                    /* function buffer              */
    UCHAR   uchCode1;                           /* function code 1          */
    UCHAR   uchCode2;                           /* function code 2          */
} FNCBUF, FAR *PFNCBUF;

#define     FC_DEF_CODE1        0x80        /* code 1 pattern by default    */
#define     FC_DEF_CODE2        0x00        /* code 2 pattern by default    */
#define     FC_DGRAM_ACK        0x60        /* datagram ack. required       */
#define     FC_MSEC_TIMER       0x7F        /* test bits for msec timer     */

/* --- status buffer --- */

typedef struct _STTBUF {                    /* status buffer                */
    USHORT  usStatus;                           /* status                   */
} STTBUF, FAR *PSTTBUF;

#define     ST_NO_ERROR         0x0000      /* no error                     */
#define     ST_TARGET_BUSY      0x3909      /* target busy                  */
#define     ST_UNDELIVER        0x3D09      /* undeliverable                */
#define     ST_INVALID_MSG      0x3B09      /* invalid message format       */

/* --- MPH length in normal user message --- */

#define     MPH_LENGTH  (sizeof(MPHHDR) + sizeof(BUFCNT) * 2 + sizeof(FNCBUF))

/* -------------------------------------------- *\
 +  There is no limitation for the structures.  +
\* -------------------------------------------- */

#pragma pack()                              /* reinstate structure packing  */

PKTINF      infPacket;                      /* function info. packet    */
/* ---- Packet Buffer ---

	This receive buffer is used to receive information from terminals.
	Currently the maximum amount of information that can be transmitted
	to and from any terminal, using the XGRAM structure,can be found in 
	the following defines:
	
	  PIF_LEN_UDATA_EX, PIF_LEN_UDATA, CLI_MAX_REQDATA

	If these values increase from their current (1400 value) this buffer
	size should increase as well.

	The buffer is composed of the XGRAM structure, terminal number, and PHPHHDR
	structure.
*/
UCHAR   auchRcvBuffer[PIF_NETINF_VAL_MAX_LENGTH];

/*==========================================================================*\
;+                                                                          +
;+          P R O T O T Y P E     D E C L A R A T I O N s                   +
;+                                                                          +
\*==========================================================================*/

/* ---------------------------- *\
 +    Network Manager Thread    +
\* ---------------------------- */

/* VOID THREAD NetManager(PNETARG); */
USHORT      CreateEndPoint(USHORT, PINT);

/* --- tick timer procedure --- */

USHORT  TimerProtocol(PMSGHDR, PPKTINF);
USHORT  IssueTimerRequest(USHORT, PMSGHDR, PPKTINF);

/* --- receiver procedure --- */

USHORT  ReceiverProtocol(PMSGHDR, PPKTINF);
USHORT  ReceiverMessageProtocol(PMPHHDR, USHORT, ULONG, PPKTINF);
USHORT  ReceiverReplyProtocol(PMPHHDR, USHORT, ULONG, PPKTINF);
USHORT  IssueReceiverRequest(USHORT, PMSGHDR, PPKTINF);
USHORT  ChangeReceiverSocket(USHORT, PMSGHDR, PPKTINF);

/* --- executable routines --- */
/*
USHORT  ExeNetOpen(POPNDAT, USHORT, PPKTINF);
USHORT  ExeNetReceive(USHORT, PRCVDAT, USHORT, PPKTINF);
USHORT  ExeNetSend(USHORT, PSNDDAT, USHORT, PPKTINF);
USHORT  ExeNetControl(USHORT, PCNTDAT, USHORT, PPKTINF);
USHORT  ExeNetClose(USHORT, PPKTINF);
USHORT  ExeNetShutdown(USHORT, PPKTINF);
*/
USHORT  SendAckMessage(PACKDAT, LPUCHAR, USHORT, PPKTINF);
VOID    TellPowerDownEvent(PPKTINF);

/* --- diag functions --- */

USHORT  ExeNetDiag(PPKTINF, PDGDATA, PDGINFO, USHORT);
USHORT  DiagGetSystem(PPKTINF, PREQINF, PRSPSYS, USHORT, PUSHORT);
USHORT  DiagGetUser(PPKTINF, PREQINF, PRSPUSR, USHORT, PUSHORT);
USHORT  DiagSetValue(PPKTINF, PREQSET, PVOID, USHORT, PUSHORT);
USHORT  DiagCheckMemory(PPKTINF, PVOID, PRSPMEM, USHORT, PUSHORT);

/* --- sub-routines --- */

PUSRTBL GetUserTable(USHORT, PUSRTBL, USHORT);
PUSRTBL GetLocalUser(USHORT, PUSRTBL, USHORT);
USHORT  GetFreeUserTable(PUSRTBL, USHORT);

/* ---------------------------- *\
 +  Continuous Receiver Thread  +
\* ---------------------------- */

USHORT      SpawnReceiver(USHORT, INT, USHORT);
VOID THREAD NetReceiver(PRCVARG);

/* ---------------------------- *\
 +      Sleep Timer Thread      +
\* ---------------------------- */

USHORT      SpawnTimer(USHORT);
VOID THREAD SleepTimer(PTMRARG);

/* ---------------------------- *\
 +        Sub-Routines          +
\* ---------------------------- */
#if defined (NET_DEBUG)
VOID        InitializeDebugging(PDBGINF);
# define    DBGCOUNTUP(x)           ((x).usLoops)++
# define    DBGSETSTATE(x, s)       ((x).usState = s)
#else
# define    DBGCOUNTUP(x)           ((void)0)
# define    DBGSETSTATE(x, s)       ((void)0)
#endif
/* ---------------------------- *\
 +        Tally  Macros         +
\* ---------------------------- */

#if defined (NET_TALLY)
# define    STLYTOTALSENT(tly)      ((tly).ulTotalSent)++
# define    STLYSENTERROR(tly)      ((tly).ulSentError)++
# define    STLYTOTALRECV(tly)      ((tly).ulTotalRecv)++
# define    STLYRECVERROR(tly)      ((tly).ulRecvError)++
# define    STLYOUTOFRESOURCES(tly) ((tly).ulOutOfResources)++
# define    STLYILLEGALMESSAGE(tly) ((tly).ulIllegalMessage)++
# define    STLYRECVUNDELIVER(tly)  ((tly).ulRecvUndeliver)++
# define    UTLYTOTALSENT(tly)      ((tly).usTotalSent)++
# define    UTLYSENTERROR(tly)      ((tly).usSentError)++
# define    UTLYTOTALRECV(tly)      ((tly).usTotalRecv)++
# define    UTLYRECVERROR(tly)      ((tly).usRecvError)++
# define    UTLYRECVLOST(tly)       ((tly).usRecvLost)++
# define   STLYLASTSENTERROR(tly, s) (tly).usLastSentError = (USHORT)WSAGetLastError()
# define   STLYLASTRECVERROR(tly, s) (tly).usLastRecvError = (USHORT)WSAGetLastError()
#else
# define    STLYTOTALSENT(tly)          ((void)0)
# define    STLYSENTERROR(tly)          ((void)0)
# define    STLYTOTALRECV(tly)          ((void)0)
# define    STLYRECVERROR(tly)          ((void)0)
# define    STLYOUTOFRESOURCES(tly)     ((void)0)
# define    STLYILLEGALMESSAGE(tly)     ((void)0)
# define    STLYRECVUNDELIVER(tly)      ((void)0)
# define    STLYLASTSENTERROR(tly, s)   ((void)0)
# define    STLYLASTRECVERROR(tly, s)   ((void)0)
# define    UTLYTOTALSENT(tly)          ((void)0)
# define    UTLYSENTERROR(tly)          ((void)0)
# define    UTLYTOTALRECV(tly)          ((void)0)
# define    UTLYRECVERROR(tly)          ((void)0)
# define    UTLYRECVLOST(tly)           ((void)0)
#endif


/*==========================================================================*\
;+                                                                          +
;+                  S T A T I C   V A R I A B L E s                         +
;+                                                                          +
\*==========================================================================*/

/* --- kernel debugging information --- */
#if defined (NET_DEBUG)
 PPKTINF    pNetPacketInfo;                     /* ptr. to packet info.     */
 DBGINF     infNetManager;                      /* net manager thread info  */
 DBGINF     infNetReceiver;                     /* net manager thread info  */
 DBGINF     infNetTimer;                        /* net manager thread info  */
#else
 UCHAR      pNetPacketInfo  = 'B';              /* ptr. to packet info.     */
 UCHAR      infNetManager   = 'A';              /* net manager thread info  */
 UCHAR      infNetReceiver  = 'D';              /* net manager thread info  */
 UCHAR      infNetTimer     = '!';              /* net manager thread info  */
#endif

/*==========================================================================*\
;+                                                                          +
;+              C O N S T A N T    D E F I N I T I O N s                    +
;+                                                                          +
\*==========================================================================*/


/*==========================================================================*\
;+                                                                          +
;+              N e t w o r k   M a n a g e r   T h r e a d                 +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Spawn network manager thread
;
;   format : USHORT     SpawnNetManager(usQueue, pInfo);
;
;   input  : USHORT     usQueue;        - queue ID
;            PNETINF    pInfo;          - ptr. to information
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT  SpawnNetManager(USHORT usQueue, PNETINF pInfo)
{
    SHORT sRet;
    static NETARG       argManager;
    PNETINF     pDef, pNew;

    /* --- allocate working stack area --- */
#if defined (POS2170)
    static  UCHAR   auchMgrStack[4096 + 150];
    #define         SIZE_MGR_STACK  sizeof(auchMgrStack)
#else
    #define         auchMgrStack    NULL
    #define         SIZE_MGR_STACK  4096
#endif

    /* --- define default parameters --- */

    static  NETINF  DefaultInfo[] = { "3000",   /* my local port no.        */
                                      "3000",   /* remote port no.          */
                   PIF_NETINF_VAL_MAX_USERS ,   /* max. users               */
                   PIF_NETINF_VAL_MAX_QUEUE ,   /* max. queues              */
                  PIF_NETINF_VAL_MAX_LENGTH ,   /* max. length in byte      */
                                         32 ,   /* work memory size in KByte*/
                   PIF_NETINF_VAL_IDLE_TIME     /* idle time                */
                                    };

    /* --- make argument --- */

    argManager.usStatus = 0;                    /* init. execution status   */
    argManager.usQueue  = usQueue;              /* queue ID for the thread  */

    /* --- make additional parameters --- */

    pDef                     = DefaultInfo;         /* for abbreviation     */
    pNew                     = pInfo ? pInfo : pDef;/* evaluate info. ptr.  */
    argManager.pszLocalPort  = pNew->pszLocalPort  ? pNew->pszLocalPort : pDef->pszLocalPort;
    argManager.pszRemotePort = pNew->pszRemotePort ? pNew->pszRemotePort : pDef->pszRemotePort;
    argManager.usMaxLength   = pNew->usMaxLength   ? pNew->usMaxLength : pDef->usMaxLength;
    argManager.usMaxUsers    = pNew->usMaxUsers    ? pNew->usMaxUsers : pDef->usMaxUsers;
    argManager.usMaxQueues   = pNew->usMaxQueues   ? pNew->usMaxQueues : pDef->usMaxQueues;
    argManager.usWorkMemory  = pNew->usWorkMemory  ? pNew->usWorkMemory : pDef->usWorkMemory;
    argManager.usIdleTime    = pNew->usIdleTime    ? pNew->usIdleTime : pDef->usIdleTime;

    sRet = NetHandler(&argManager);

    /* --- exit ... --- */

    return ((sRet == -1) ? PIF_ERROR_NET_CRITICAL : argManager.usStatus);
}

SHORT NetHandler(PNETARG pArg)
{
    USHORT      usRet, usStat, usBytes;
#if !defined(SARATOGA) && !defined (POS2170)
    PSZ         pszLoc, pszRem;
#endif
    WSADATA          wsaData;
    struct servent * pService;
    /* --- declare my important resources & information --- */

    USHORT      usManager,                      /* my queue ID              */
                usReceiver,                     /* receiver queue ID        */
                usTimer;                        /* timer queue ID           */
//    PKTINF      infPacket;                      /* function info. packet    */
    LPVOID      pvPtr;

    /* --- save debugging info. --- */
#if defined (NET_DEBUG)
    InitializeDebugging(&infNetManager);
    pNetPacketInfo = &infPacket;
#endif
    /* --- initialize --- */

    usStat = 0;                                 /* assume no error          */
    memset(&infPacket, '\0', sizeof(infPacket));

    /* --- get & compute parameters --- */

    infPacket.usManager     = pArg->usQueue;    /* my queue ID              */
    infPacket.usMaxQueues   = pArg->usMaxQueues;/* max. no. of queue buffers*/
    infPacket.usSizeBuffer  = pArg->usMaxLength;/* max. data length in byte */
    infPacket.usSizeBuffer += MPH_LENGTH;       /* adjust MPH header length */
    infPacket.usTick        = 100;              /* tick every 100 msec.     */
    infPacket.usIdleTime    = pArg->usIdleTime; /* idle time in msec.       */
    usManager               = pArg->usQueue;    /* my queue ID              */
    usReceiver              = (usManager + 1);  /* cont. receiver's queue ID*/
    usTimer                 = (usManager + 2);  /* sleep timer's queue ID   */

    /* --- initialize socket surroundings --- */
//  WSAStartup(MAKEWORD(1,1), &wsaData);        /* initialize winsock       */
    WSAStartup(MAKEWORD(2,1), &wsaData);        /* initialize winsock       */

	/* --- compute port numbers --- */
    infPacket.usPortLocal  = (USHORT)strtoul(pArg->pszLocalPort,  &pszLoc, 0);
    infPacket.usPortRemote = (USHORT)strtoul(pArg->pszRemotePort, &pszRem, 0);

	if (*pszLoc) {                              /* failed in analysis ?     */
        pService               = getservbyname(pArg->pszLocalPort, "UDP");
        infPacket.usPortLocal  = (USHORT)(pService ? pService->s_port : 0);
    }
    if (*pszRem) {                              /* failed in analysis ?     */
        pService               = getservbyname(pArg->pszRemotePort, "UDP");
        infPacket.usPortRemote = (USHORT)(pService ? pService->s_port : 0);
    }

	usRet  = infPacket.usPortLocal  ? 0     : PIF_ERROR_NET_BAD_DATA;
    usRet  = infPacket.usPortRemote ? usRet : PIF_ERROR_NET_BAD_DATA;
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- create my end point --- */

    usRet  = CreateEndPoint(infPacket.usPortLocal, &(infPacket.iSocket));
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- get reserved header length in 2170 POS socket --- */
    infPacket.usSendReservedBytes = 0;          /* no adjust bytes          */
    infPacket.usRecvReservedBytes = 0;          /* no adjust bytes          */

	/* --- compute size of working memory --- */
#if defined (MEM_INTERNAL_LOGIC)
    usBytes = 0;                                /* create whole memory      */
#else
    usBytes =  pArg->usWorkMemory * KBYTES;     /* create by given size     */
    usRet   = (pArg->usWorkMemory <= 64) ? 0 : PIF_ERROR_NET_BAD_DATA;
    usStat  = LASTERROR(usStat, usRet);         /* evaluate return stat */
#endif

    /* --- create my memory segment --- */

    infPacket.hMemory = SysCreateMemory(usBytes);   /* create memory segment*/
    usRet             = infPacket.hMemory ? 0 : PIF_ERROR_NET_MEMORY;
    usStat            = LASTERROR(usStat, usRet);   /* evaluate return stat */

    /* --- initialize before creating user management table --- */

    infPacket.pUsers  = _NULL;                  /* ptr. to user table       */
    infPacket.usUsers = 0;                      /* no. of users             */
    usBytes           = sizeof(USRTBL) * pArg->usMaxUsers;

    /* --- create user management table --- */

    if (pvPtr = SysAllocMemory(infPacket.hMemory, usBytes)) {
        infPacket.pUsers  = (PUSRTBL)pvPtr;         /* ptr. to user table   */
        infPacket.usUsers = pArg->usMaxUsers;       /* no. of users         */
        _fmemset(pvPtr, 0, usBytes);                /* clear user table     */
    } else {
        usStat            = (USHORT)(PIF_ERROR_NET_MEMORY);
    }

    /* --- create send buffer --- */

    if (pvPtr = SysAllocMemory(infPacket.hMemory, infPacket.usSizeBuffer)) {
        infPacket.pvSendBuffer = pvPtr;
    } else {
        usStat                 = (USHORT)(PIF_ERROR_NET_MEMORY);
    }

    pArg->usStatus = usStat;                    /* set my initialized status*/

    return (PIF_OK);
}

/**
;========================================================================
;
;   function : Open user port
;
;   format : USHORT     ExeNetOpen(pOpen, usBytes, pPacket);
;
;   input  : POPNDAT    pOpen;      - ptr. to parameters
;            USHORT     usBytes;    - data length
;            PPKTINF    pPacket;    - ptr. to a packet
;
;   output : USHORT     usRet;      - status
;
;========================================================================
**/

USHORT  ExeNetOpenCli(POPNDAT pOpen)
{
    USHORT      usNet, usLocalPort;
    PUSRTBL     pUser;
    PPKTINF pPacket;

    /* --- is valid port no. given ? --- */
    if (pOpen->usLport > PORT_LOCAL_MAX) {      /* over max. port no. ?     */
        return ((USHORT)(PIF_ERROR_NET_BAD_DATA));
    }

    pPacket = &infPacket;

    /* --- get the user table of the port no. on purpose --- */
    pUser = GetLocalUser(pOpen->usLport, pPacket->pUsers, pPacket->usUsers);

    /* --- already exist the same port user ? --- */
    if ((pOpen->usLport != ANYPORT) && pUser) { /* might be duplicate user  */
        return ((USHORT)(PIF_ERROR_NET_BAD_DATA));
    }

    /* --- could be found ? --- */
    if (! (usNet = GetFreeUserTable(pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_MANY_USERS));
    }

    /* --- compute its user table ptr. --- */
    pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers);

    /* --- normalize my local port number --- */
    if (! (usLocalPort = (pOpen->usLport == ANYPORT) ? 0 : pOpen->usLport)) {
        usLocalPort = usNet + PORT_AUTO_ASSIGN;
    }

    /* --- initialize user control table --- */
    memset (pUser, '\0', sizeof(USRTBL));      /* clear user table         */

    /* --- make up user table --- */
    pUser->fsFlags      = PIF_NET_USED;         /* block this table         */
    pUser->ulRemoteAddr = pOpen->ulFaddr;       /* target address           */
    pUser->usRemotePort = pOpen->usFport;       /* target port number       */
    pUser->usLocalPort  = usLocalPort;          /* local port number        */
    pUser->usSendTimer  = DEF_SEND_TIME;        /* timeout value in msec.   */
    pUser->fsFlags     |= (pOpen->usFport == ANYPORT) ? PIF_NET_NMODE : PIF_NET_DMODE;
    return (usNet);
}

/**
;========================================================================
;
;   function : Send datagram ack. message
;
;   format : USHORT     SendAckMessage(pInfo, puchData, usLength, pPacket);
;
;   input  : PACKDAT    pInfo;          - ptr. to an ack. info.
;            PUCHAR     puchData;       - optional data
;            USHORT     usLength;       - optional data length
;            PPKTINF    pPacket;        - ptr. to packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/
USHORT  SendAckMessage(PACKDAT  pInfo, LPUCHAR  puchData, USHORT   usLength, PPKTINF  pPacket)
{
    BOOL                fAnyData;
    SHORT               sRet;
    USHORT              usBytes;
    PMPHHDR             pMph;
    PBUFCNT             pBuf;
    PSTTBUF             pStat;
    struct sockaddr_in  sinClient;

    /* --- initialize ---
		Now lets see if the caller wants to add some additional data.
		If so, then we will add another buffer header with the buffer type and size.
	 */
    fAnyData = (puchData && usLength) ? TRUE : FALSE;

    /* --- make up target address --- */
    memset(&sinClient, '\0', sizeof(sinClient));
    sinClient.sin_family      = AF_INET;
    sinClient.sin_addr.s_addr = pInfo->ulTargetAddr;
	if (pInfo->usIpPortNo)
	{
		sinClient.sin_port = htons(pInfo->usIpPortNo);
	}
	else
	{
		sinClient.sin_port = htons(pPacket->usPortRemote);
	}

    /* --- make base MPH header --- */
    pMph           = (PMPHHDR)(pPacket->pvSendBuffer);  /* ptr. to a buffer */
    (LPUCHAR)pMph += pPacket->usSendReservedBytes;  /* adjust top of ptr.   */
    pMph->uchId    = ID_MESSAGE | ID_REPLY_BIT;     /* reply message ID     */
    pMph->uchSp    = (UCHAR)(pInfo->usLocalPort);   /* source port no.      */
    pMph->uchNo    = (UCHAR)(pInfo->usNumber);      /* message seq. no.     */
    pMph->uchDp    = (UCHAR)(pInfo->usTargetPort);  /* target port no.      */
    pMph->uchBc    = (UCHAR)(fAnyData ? 2 : 1);     /* no. of buffers       */

    /* --- make status buffer control --- */
    pBuf           = (PBUFCNT)(pMph + 1);       /* ptr. to 1st buffer area  */
    pBuf->usLength = sizeof(STTBUF);                /* bytes of func. data  */
    pBuf->uchType  = BT_STATUS;                     /* buffer type for stat */

    /* --- make optional data buffer control --- */
    if (fAnyData) {                             /* any optional data ?      */
        pBuf++;                                 /* ptr. to 2nd buffer area  */
        pBuf->usLength = usLength;                  /* bytes of user data   */
        pBuf->uchType  = BT_USER_DATA;              /* buffer type for data */
    }

    /* --- make status buffer ---
		The first buffer is the status buffer into which we are going to put
		our status information for the reply.  After inserting the status
		for the reply message, advance the pointer to after the status buffer
		which is where the user specified data will go if there is any.
	 */
    pStat           = (PSTTBUF)(pBuf + 1);      /* ptr. to function buffer  */
    pStat->usStatus = pInfo->usStatus;              /* status data          */
    pStat++;                                        /* go to next data area */

    /* --- make up user data ---
		If there is user specified data, then copy it into the reply message
		and increment the total number of bytes to send by the specified length
		of the message.
	 */
    if (fAnyData) {                             /* any optional data ?      */
        memcpy(pStat, puchData, usLength);        /* copy user data       */
    }

    /* --- compute total data length --- */
    usBytes  = (USHORT)((LPUCHAR)pStat - (LPUCHAR)pMph);
    usBytes += (fAnyData ? usLength : 0);

    /* --- send a message --- */
    sRet = sendto(pPacket->iSocket, (LPUCHAR)(pMph), usBytes, 0, (struct sockaddr *)(&sinClient), sizeof(sinClient));
    if (sRet != (INT)usBytes) {   // check that sendto() was successful
		ULONG  ulLastError = WSAGetLastError();
		if (ulLastError != 10065) {
			char  xBuff[64];
			sprintf (xBuff, "SendAckMessage(): WSAGetLastError() = %d", ulLastError);
			return ((USHORT)(PIF_ERROR_NET_ERRORS));
		}
		else {
			return ((USHORT)(PIF_ERROR_NET_UNREACHABLE));
		}
    }

    return (0);
}


/**
;========================================================================
;
;   function : Receive from user port
;
;   format : USHORT     ExeNetReceive(usHandle, pReceive, usBytes, pPacket);
;
;   input  : USHORT     usHandle;       - network handle
;            PRCVDAT    pReceive;       - ptr. to a receive data
;            USHORT     usBytes;        - no. of bytes
;            PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT ExeNetReceiveCli(USHORT usNet, PRCVDAT pReceive)
{
    PUSRTBL     pUser;
    PXGRAM      pHeader;
    PMPHHDR     pMph;
    PBUFCNT     pBuf;
    PFNCBUF     pFunc;
    PXGADDR     pSender;
    LPUCHAR     puchData, puchUser, puchTarget;
    ULONG       ulRecvIpAddress = 0;
    BOOL        fValid, fNormalMode, i, fSendAck = FALSE;
    USHORT      usRecvIpPort = 0;
    USHORT      usLength, usLast;
    SHORT       sRead;
    USHORT      usSize;
    
    struct sockaddr_in  sinClient;
    INT                 iClient;

    PPKTINF pPacket;
    int retval;
    ULONG   ulTime, ulStartTime, ulCurTime;
    SYSTEMTIME  SystemTime;

    pPacket = &infPacket;

    /* --- compute its user table ptr. --- */
    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

    /* --- is opened user ? --- */
    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* invalid handle       */
    }

    /* --- power down occurred ? --- */
    if (! ((~pUser->fsFlags) & (PIF_NET_EVENT_MODE | PIF_NET_POWER_FAIL))) {
        return ((USHORT)(PIF_ERROR_NET_POWER_FAILURE));
    }

    GetLocalTime(&SystemTime);

    ulStartTime =  SystemTime.wMinute * 60;
    ulStartTime += SystemTime.wSecond;
    ulStartTime *= 1000;

    /* --- wait for a message --- */
    pMph    = (PMPHHDR)(auchRcvBuffer); /* ptr. to received */
    usSize = MPH_LENGTH + pReceive->usBytes;
    iClient = sizeof(sinClient);
    
    /* time out */
    ulTime = (pUser->fsFlags & PIF_NET_TMODE) ? pUser->ulRecvTimer : 0;
        
    do {
		TIMEVAL *ptimeout = NULL;
		TIMEVAL timeout;
		fd_set readfds, exceptfds;

        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_SET(pPacket->iSocket, &readfds);
                                                  
        if (ulTime) {
            timeout.tv_sec = ulTime/1000;
            timeout.tv_usec = ulTime%1000;
            ptimeout = &timeout;
        }

        retval = select(0, &readfds, NULL, &exceptfds, ptimeout);
        if (retval == SOCKET_ERROR) {
			int  iLastError = WSAGetLastError();
			return PIF_ERROR_NET_ERRORS;
        }
        if (retval==0){
            /* time out */
            return PIF_ERROR_NET_TIMEOUT;
        }

        ulRecvIpAddress = 0;
        usRecvIpPort = 0;
        if (FD_ISSET(pPacket->iSocket, &readfds)){
            sRead = recvfrom(pPacket->iSocket, (UCHAR *)pMph, usSize, 0, (struct sockaddr *)&sinClient, &iClient);
			if (sRead == SOCKET_ERROR) {
				int  iLastError = WSAGetLastError();
				return PIF_ERROR_NET_ERRORS;
			}
			if (sRead > 0) {     /* buffer size      */
                ulRecvIpAddress = sinClient.sin_addr.s_addr;
                usRecvIpPort = ntohs(sinClient.sin_port);
			} else {
				sRead = -1;
			}
        } else {
            sRead = -1;
        }

        if (sRead < 0) {
			int  iLastError = WSAGetLastError();
            return PIF_ERROR_NET_TIMEOUT;
        }
            
		if (pUser->fsFlags & PIF_NET_SRVRMODE) {
			break;
		}

        /* --- is same message sequence no. etc. ? --- */
        if ((pMph->uchNo != (UCHAR)(pUser->infSending).usSeqNo)      ||
            (pMph->uchSp != (UCHAR)(pUser->infSending).usTargetPort) ||
            (sinClient.sin_addr.s_addr != (pUser->infSending).ulTargetAddr))
		{
			if (ulTime) {
                /* time out */
            
                /* get expired time from time stamp */
                GetLocalTime(&SystemTime);

                ulCurTime =  SystemTime.wMinute * 60;
                ulCurTime += SystemTime.wSecond;
                ulCurTime *= 1000;
                if (ulCurTime < ulStartTime) {
                    ulCurTime += 60*60*1000;
                }
                
                ulCurTime -= ulStartTime;
                if (ulCurTime > ulTime ) {
                    return PIF_ERROR_NET_TIMEOUT;
                }
            }
            continue;
        }
    } while(pMph->uchSp != (UCHAR)(pUser->infSending).usTargetPort);
    
    /* --- initialize --- */
    usLength = 0;                               /* init. user data length   */
    pFunc    = _NULL;                           /* init. function code ptr. */
    usLast   =  sRead;                        /* received data length     */
    usLast  -= (sRead > sizeof(MPHHDR)) ? sizeof(MPHHDR) : usLast;

    /* --- evaluate valid message or not --- */
    fValid   =  usLast                       ? TRUE   : FALSE;
    fValid   = (pMph->uchBc < BC_MAX_COUNTS) ? fValid : FALSE;
    pBuf     = (PBUFCNT)(pMph + 1);             /* ptr. to 1st buffer       */
    puchData = (LPUCHAR)(pBuf + pMph->uchBc);   /* ptr. to 1st data         */
    
    /* --- compute & get message info. --- */
    for (i = 0; fValid && (i < pMph->uchBc); i++) {
        /* --- what type of buffer ? --- */
        switch (pBuf->uchType) {                /* what type of buffer ?    */
        case BT_FUNCTION:                       /* function data ?          */
            pFunc     = (PFNCBUF)puchData;          /* ptr. to func. buffer */
            puchData += sizeof(FNCBUF);             /* prepare to next buff */
            fValid    = (pBuf->usLength == sizeof(FNCBUF)) ? TRUE : FALSE;
            break;
        case BT_USER_DATA:                      /* user data ?              */
            puchUser  = puchData;                   /* ptr. to user buffer  */
            usLength  = pBuf->usLength;             /* user data length     */
            puchData += pBuf->usLength;             /* prepare to next buff */
            break;
        case BT_STATUS:                         /* status data ?            */
            puchData += sizeof(STTBUF);             /* prepare to next buff */
            fValid    = (pBuf->usLength == sizeof(STTBUF)) ? TRUE : FALSE;
            break;
        default:                                /* unknown buffer type ...  */
            fValid = FALSE;
            break;
        }

        /* --- go to next buffer --- */
        usLast -= pBuf->usLength + sizeof(BUFCNT);
        pBuf++;                                 /* go to next buffer        */
    }

    /* --- is valid data received ? --- */
    if ((! fValid) || (usLast) || (! pFunc) || (! puchUser)) {
        return PIF_ERROR_NET_TIMEOUT;   /* cannot treat it !    */
    }

	pHeader     = (PXGRAM)(pReceive->pvBuffer);
    pSender     =         (pReceive->pSender);
    puchTarget  = (LPUCHAR)(pReceive->pvBuffer);
    usSize      =          (pReceive->usBytes);
    fNormalMode = pUser->fsFlags & PIF_NET_NMODE;
    fNormalMode = pSender ? FALSE : fNormalMode;
    puchTarget += fNormalMode ? FAROF(auchData, XGRAM) : 0;
    usSize     -= fNormalMode ? FAROF(auchData, XGRAM) : 0;
    
    /* --- respond received data to user --- */
    memcpy (puchTarget, puchUser, MINOF(usSize, usLength));

    /* --- respond sender address info. --- */
	if (pUser->fsFlags & PIF_NET_SRVRMODE) {
		// If we are in server mode then we want to set up the header information
		// in our receive buffer so that the caller will know who has sent something
		// and where to send the response.
        *(LPULONG)(pHeader->auchFaddr) = sinClient.sin_addr.s_addr;
        pHeader->usFport               = pMph->uchSp;
        pHeader->usLport               = pUser->usLocalPort;
        usLast                         = FAROF(auchData, XGRAM);
		fNormalMode = TRUE;
	}
    else if (pSender) {                      /* prepared sender info. ?  */
        *(LPULONG)(pSender->auchAddr) = sinClient.sin_addr.s_addr;
        pSender->usPort               = pMph->uchSp;
        usLast                        = 0;  /* no adjusted bytes    */
    } else if (fNormalMode) {           /* normal mode ?            */
        *(LPULONG)(pHeader->auchFaddr) = sinClient.sin_addr.s_addr;
        pHeader->usFport               = pMph->uchSp;
        pHeader->usLport               = pUser->usLocalPort;
        usLast                         = FAROF(auchData, XGRAM);
    } else {                            /* other mode ...           */
        usLast                        = 0;  /* no adjusted bytes    */
    }
    
    sRead = (usSize < usLength) ? usSize : usLength;
    if (fNormalMode) {           /* normal mode ?            */
        sRead += FAROF(auchData, XGRAM);
    }

    fSendAck = pFunc->uchCode1 & FC_DGRAM_ACK;  /* ack. control required ?  */

	    /* --- datagram ack. required ? --- */
    if (fSendAck) {                             /* datagram ack. required ? */
		ACKDAT      infAck;
		USHORT      usRet;

        /* --- make ack. data info. --- */
        infAck.ulTargetAddr = ulRecvIpAddress;      /* target IP address    */
        infAck.usIpPortNo   = usRecvIpPort;         /* target UDP/IP port number */
        infAck.usTargetPort = pMph->uchSp;          /* target port no.      */
        infAck.usNumber     = pMph->uchNo;          /* message number       */
        infAck.usLocalPort  = pMph->uchDp;          /* source port no.      */
        infAck.usStatus     = ST_NO_ERROR;          /* status               */

        /* --- send ack. message --- */
        puchUser = NULL;
        usLength = 0;
        usRet    = SendAckMessage(&infAck, puchUser, usLength, pPacket);
    }

    return (sRead);

}

/**
;========================================================================
;
;   function : Send data
;
;   format : USHORT     ExeNetSend(usHandle, pSend, usBytes, pPacket);
;
;   input  : USHORT     usHandle;       - network handle
;            PSNDDAT    pSend;          - ptr. to a send data
;            USHORT     usBytes;        - no. of bytes
;            PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT ExeNetSendCli(USHORT usNet, PSNDDAT pSend)
{
    UCHAR               uchTimer;
    SHORT               sRet;
    USHORT              usRet, usSize, usHeader, usTargetPort;
    ULONG               ulTargetAddr;
    LPUCHAR             puchBuffer;
    PUSRTBL             pUser;
    PSNDINF             pInfo;
    PXGRAM              pHeader;
    PMPHHDR             pMph;
    PBUFCNT             pBuf;
    PFNCBUF             pFunc;
    struct sockaddr_in  sinClient;
    PPKTINF pPacket;
    fd_set writefds, exceptfds;
    int retval;

    pPacket = &infPacket;

    /* --- initialize --- */
    puchBuffer = (LPUCHAR)(pSend->pvBuffer);    /* top of buffer ptr.       */
    pHeader    = ( PXGRAM)(pSend->pvBuffer);    /* top of header            */
    usSize     = pSend->usBytes;                /* buffer size in byte      */
    usRet      = PIF_OK;                        /* assume good status       */

    /* --- compute its user table ptr. --- */
    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

    /* --- is opened user ? --- */
    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* invalid handle       */
    }

    /* --- power down occurred ? --- */
    if (! ((~pUser->fsFlags) & (PIF_NET_EVENT_MODE | PIF_NET_POWER_FAIL))) {
        return ((USHORT)(PIF_ERROR_NET_POWER_FAILURE));
    }

    /* --- is normal mode ? --- */
    if (pUser->fsFlags & PIF_NET_NMODE) {       /* normal mode ?            */
        puchBuffer   = pHeader->auchData;           /* ptr. to data sent    */
        usSize      -= FAROF(auchData, XGRAM);      /* compute data length  */
        ulTargetAddr = *(LPULONG)(pHeader->auchFaddr);  /* target IP address*/
        usTargetPort = pHeader->usFport;            /* target user port no. */
    } else {                                    /* else, data only mode     */
        ulTargetAddr = pUser->ulRemoteAddr;         /* target IP address    */
        usTargetPort = pUser->usRemotePort;         /* target user port no. */
    }

    /* --- make up target address --- */
    memset(&sinClient, '\0', sizeof(sinClient));
    sinClient.sin_family      = AF_INET;
    sinClient.sin_addr.s_addr = ulTargetAddr;
    sinClient.sin_port        = htons(pPacket->usPortRemote);

    /* --- compute timer value in 100th msec unit --- */
    if (pUser->usSendTimer != PIF_NO_WAIT) {    /* datagram ack. required ? */
        uchTimer = (UCHAR)TICK(pUser->usSendTimer - ADJ_SEND_TIME, 100);
    } else {                                    /* else, not required ...   */
        uchTimer = 0;
    }

    /* --- make info. of sending data --- */
    pInfo               = &(pUser->infSending); /* ptr. to control array    */
    pInfo->usSeqNo     += 1;                    /* count up seq. no.        */
    pInfo->usTargetPort = usTargetPort;         /* target user port no.     */
    pInfo->ulTargetAddr = ulTargetAddr;         /* target IP address        */

    /* --- make complete packet data --- */
    pMph             = (PMPHHDR)(pPacket->pvSendBuffer);
    (PUCHAR)pMph    += pPacket->usSendReservedBytes;/* adjust top of ptr.   */
    pMph->uchId      = ID_MESSAGE;                  /* normal message ID    */
    pMph->uchSp      = (UCHAR)(pUser->usLocalPort); /* source port no.      */
    pMph->uchNo      = (UCHAR)(pInfo->usSeqNo);     /* message seq. no.     */
    pMph->uchDp      = (UCHAR)(usTargetPort);       /* target port no.      */
    pMph->uchBc      = 2;                           /* no. of buffers       */
    pBuf             = (PBUFCNT)(pMph + 1);     /* ptr. to 1st buffer area  */
    pBuf->usLength   = sizeof(FNCBUF);              /* bytes of func. data  */
    pBuf->uchType    = BT_FUNCTION;                 /* buffer type for func.*/
    pBuf++;                                     /* ptr. to 2nd buffer area  */
    pBuf->usLength   = usSize;                      /* bytes of user data   */
    pBuf->uchType    = BT_USER_DATA;                /* buffer type for data */
    pFunc            = (PFNCBUF)(pBuf + 1);     /* ptr. to function buffer  */
    pFunc->uchCode1  = FC_DEF_CODE1;                /* functon code 1       */
    pFunc->uchCode1 |= (UCHAR)(uchTimer ? FC_DGRAM_ACK : 0);
    pFunc->uchCode2  = FC_DEF_CODE2;                /* function code 2      */
    pFunc->uchCode2 |= (UCHAR)(uchTimer & FC_MSEC_TIMER);
    usHeader         = (USHORT)((LPUCHAR)(++pFunc) - (LPUCHAR)pMph);

    /* --- is enough buffer size ? --- */
    if (pPacket->usSizeBuffer < (usSize + usHeader + pPacket->usSendReservedBytes)) {
        return ((USHORT)(PIF_ERROR_NET_TOO_LONG));
    }

    /* --- make up user data --- */
    memcpy (pFunc, puchBuffer, usSize);        /* copy user data           */

    /* --- send a message --- */
    /* wait for write event */
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(pPacket->iSocket, &writefds);
    retval = select(0, NULL, &writefds, &exceptfds, NULL);
    if (retval == SOCKET_ERROR || retval == 0) {
        usRet = ((USHORT)(PIF_ERROR_NET_WRITE));
    }

    if (FD_ISSET(pPacket->iSocket, &writefds)){
		usRet = PIF_OK;                        /* assume good status       */
        sRet = sendto (pPacket->iSocket, (LPUCHAR)(pMph), usHeader + usSize, 0, (struct sockaddr *)(&sinClient), sizeof(sinClient));
        if (sRet != (INT)(usSize + usHeader)) {
            usRet = ((USHORT)(PIF_ERROR_NET_WRITE));
        }
	} else {
		usRet = ((USHORT)(PIF_ERROR_NET_WRITE));
	}

    return (usRet);
}

/**
;========================================================================
;
;   function : Control user port
;
;   format : USHORT     ExeNetControl(usHandle, pControl, usBytes, pPacket);
;
;   input  : USHORT     usHandle;       - network handle
;            PCNTDAT    pControl;       - ptr. to a control data
;            USHORT     usBytes;        - no. of bytes
;            PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT ExeNetControlCli(USHORT usNet, PCNTDAT pControl)
{
    BOOL        fSystem;
    USHORT      usRet, usMode, fsFlags, usTime;
    PUSRTBL     pUser;
    PPKTINF pPacket;

    pPacket = &infPacket;

    /* --- initialize --- */

    usRet = PIF_OK;                             /* assume no error          */

    /* --- compute its user table ptr. --- */

    fSystem = (usNet == PIF_NET_HANDLE) ? TRUE : FALSE;
    pUser   = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers);
    fsFlags = fSystem ? PIF_NET_USED : (USHORT)(pUser ? pUser->fsFlags : 0);

    /* --- is valid user handle ? --- */

    if (! (fsFlags & PIF_NET_USED)) {           /* valid user table ?       */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

    /* --- power down occurred ? --- */

    if (! ((~fsFlags) & (PIF_NET_EVENT_MODE | PIF_NET_POWER_FAIL))) {
        return ((USHORT)(PIF_ERROR_NET_POWER_FAILURE));
    }

    /* --- do each function --- */

    switch (pControl->usFunction) {             /* what function ?          */
    case PIF_NET_SET_MODE:                      /* set a mode ?             */
        switch (usMode = pControl->usModeMode) {    /* function mode        */
        case PIF_NET_NMODE:                         /* normal mode ?        */
        case PIF_NET_DMODE:                         /* data only mode ?     */
        case PIF_NET_NMODE | PIF_NET_TMODE:         /* normal w/ timeout ?  */
        case PIF_NET_NMODE | PIF_NET_TMODE | PIF_NET_SRVRMODE:  // run in server mode?
        case PIF_NET_DMODE | PIF_NET_TMODE:         /* data only w/ timeout */
            pUser->fsFlags &= ~(PIF_NET_NMODE | PIF_NET_DMODE | PIF_NET_TMODE | PIF_NET_SRVRMODE);
            pUser->fsFlags |= usMode;                   /* set them         */
            break;
        default:                                    /* others ...           */
            usRet = (USHORT)(PIF_ERROR_NET_BAD_DATA);
            break;
        }
        break;
    case PIF_NET_SET_TIME:                      /* set timeout value ?      */
    case PIF_NET_SET_TIME_EX:                   /* set timeout value ?      */
        pUser->ulRecvTimer = pControl->ulTimeTime;  /* set receive timer    */
        break;
    case PIF_NET_CLEAR:                         /* clear queued messages ?  */
        break;
    case PIF_NET_SET_STIME:                     /* set send timer value ?   */
        switch (usTime = pControl->usStimeTime) {   /* how milli-seconds ?  */
        case PIF_NO_WAIT:                           /* no wait ?            */
            pUser->usSendTimer = usTime;                /* it's good !      */
            break;
        default:                                    /* some milli-sec. ...  */
            if ((MIN_SEND_TIME <= usTime) && (usTime <= MAX_SEND_TIME)) {
                pUser->usSendTimer = usTime;
            } else {
                usRet = (USHORT)PIF_ERROR_NET_BAD_DATA;
            }
            break;
        }
        break;
    default:                                    /* others ...               */
        usRet = (USHORT)(PIF_ERROR_NET_BAD_DATA);
        break;
    }

    /* --- exit ... --- */

    return (usRet);
}

/**
;========================================================================
;
;   function : Close user port
;
;   format : USHORT     ExeNetClose(usHandle, pPacket);
;
;   input  : USHORT     usHandle;       - network handle
;            PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT  ExeNetCloseCli(USHORT usNet)
{
    PUSRTBL     pUser;
    PPKTINF pPacket;

    pPacket = &infPacket;

    /* --- compute its user table ptr. --- */

    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

    /* --- is opened user ? --- */

    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* bad handle given     */
    }

    /* --- free the table --- */

    pUser->fsFlags = 0;                         /* free the table           */

    /* --- exit ... --- */

    return (PIF_OK);
}

/**
;========================================================================
;
;   function : Shutdown a network
;
;   format : USHORT     ExeNetShutdown(usHandle, pPacket);
;
;   input  : USHORT     usHandle;       - network handle
;            PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT  ExeNetShutdownCli(USHORT usNet)
{
    PPKTINF pPacket;

    /* --- is valid user handle ? --- */

    if (usNet != PIF_NET_HANDLE) {              /* system handle ?          */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* bad handle given     */
    }

    pPacket = &infPacket;

    /* --- close my socket --- */
    shutdown(pPacket->iSocket, 2);
    closesocket(pPacket->iSocket);
    
    SysFreeMemory(infPacket.hMemory, infPacket.pvSendBuffer);
    SysFreeMemory(infPacket.hMemory, infPacket.pUsers);
    SysDeleteMemory(infPacket.hMemory);
    
    /* --- tell windows socket --- */
    WSACleanup();
    return (PIF_OK);
}


/**
;========================================================================
;
;   function : Create my end point
;
;   format : USHORT     CreateEndPoint(usPortNo, piSocket);
;
;   input  : USHORT     usPortNo;       - local port no.
;            PINT       piSocket;       - ptr. to respond socket handle
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/

USHORT  CreateEndPoint(USHORT usPortNo, PINT piSocket)
{
    INT                 iOpt = 1, iOptLen = sizeof(INT);
    INT                 iSocket;
    struct sockaddr_in  sinAddress;
    BOOL    fBlocking;
	USHORT  usFirstPortNo = usPortNo;
	int     nBindStatus = WSAENETDOWN;

    /* --- open a socket --- */

    if ((iSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return ((USHORT)(PIF_ERROR_NET_NOT_PROVIDED));
    }

    /* --- enable broadcasting mode --- */
    if (setsockopt(iSocket, SOL_SOCKET, SO_BROADCAST, (const PCHAR)(&iOpt), sizeof(iOpt))) {
        return ((USHORT)(PIF_ERROR_NET_ERRORS));
    }

	// Using getsockopt() we found the default Windows 7 socket buffer is 8192 bytes or 8K so we are increasing
	// the size of the send and receive network buffers in WinSock to a minimum of 16K.
	{
		int    error = 0;
		int    iOpt = 1, iOptLen = sizeof(int);

		iOptLen = sizeof(int);
		error = getsockopt (iSocket, SOL_SOCKET, SO_RCVBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < 1024 * 16) {
			iOpt = 1024 * 16;
			error = setsockopt(iSocket, SOL_SOCKET, SO_RCVBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
		iOptLen = sizeof(int);
		error = getsockopt (iSocket, SOL_SOCKET, SO_SNDBUF, (PCHAR)(&iOpt), &iOptLen);
		if (error < 0) {
			error = WSAGetLastError();
		} else if (iOpt < 1024 * 16) {
			iOpt = 1024 * 16;
			error = setsockopt(iSocket, SOL_SOCKET, SO_SNDBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
			if (error < 0) {
				error = WSAGetLastError();
			}
		}
	}

	/* --- make local address --- */
    /* --- bind with local address --- */

    memset(&sinAddress, '\0', sizeof(sinAddress));
    sinAddress.sin_family      = AF_INET;
    sinAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	for (usFirstPortNo = usPortNo; usPortNo < usFirstPortNo + 10; usPortNo++)
	{
		sinAddress.sin_port        = htons(usPortNo);
		nBindStatus = bind(iSocket, (struct sockaddr far *)(&sinAddress), sizeof(sinAddress));
		if (0 == nBindStatus)
		{
			break;
		}
	}
	if (0 != nBindStatus)
	{
		return ((USHORT)(PIF_ERROR_NET_BIND_ERR));
	}

    fBlocking = FALSE;
    if (ioctlsocket(iSocket, FIONBIO, &fBlocking) == SOCKET_ERROR) {
        WSACleanup();
        return ((USHORT)(PIF_ERROR_NET_ERRORS));
    }

    /* --- respond socket handle --- */

    *piSocket = iSocket;                        /* respond socket handle    */

    /* --- exit ... --- */

    return (0);
}

/*==========================================================================*\
;+                                                                          +
;+                      S u b - R o u t i n e s                             +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Get user table ptr.
;
;   format : PUSRTBL    GetUserTable(usHandle, pTable, usArrays);
;
;   input  : USHORT     usHandle;       - user handle
;            PUSRTBL    pTable;         - top of user table
;            USHORT     usArrays;       - no. of arrays
;
;   output : PUSRTBL    pTable;         - ptr. to table
;
;========================================================================
**/

PUSRTBL GetUserTable(USHORT usHandle, PUSRTBL pTable, USHORT usArrays)
{
    PUSRTBL     pUser;

    /* --- is valid handle given ? --- */

    if ((! usHandle) || (usHandle > usArrays)) {
        return (_NULL);
    }

    /* --- compute this users table ptr. --- */

    pUser = pTable + (usHandle - 1);            /* compute table ptr.       */

    /* --- exit ... --- */

    return (pUser);
}

/**
;========================================================================
;
;   function : Get user table ptr. by local port no.
;
;   format : PUSRTBL    GetLocalUser(usLocal, pTable, usArrays);
;
;   input  : USHORT     usLocal;        - local port no.
;            PUSRTBL    pTable;         - top of user table
;            USHORT     usArrays;       - no. of arrays
;
;   output : PUSRTBL    pTable;         - ptr. to table
;
;========================================================================
**/

PUSRTBL GetLocalUser(USHORT usLocal, PUSRTBL pTable, USHORT usArrays)
{
    USHORT      i;
    PUSRTBL     pUser, pLocal;

    /* --- initialize --- */

    pLocal = _NULL;                             /* assume not found         */

    /* --- look for the proper user table --- */

    for (pUser = pTable, i = 0; i < usArrays; pUser++, i++) {

        /* --- is this table being used ? --- */

        if (! (pUser->fsFlags & PIF_NET_USED)) {/* is free table ?          */
            continue;                               /* try next table       */
        }

        /* --- is same local port no. ? --- */

        if (pUser->usLocalPort == usLocal) {    /* is same local port no. ? */
            pLocal = pUser;                         /* get its address      */
            break;                                  /* break my job         */
        }
    }

    /* --- exit ... --- */

    return (pLocal);
}

/**
;========================================================================
;
;   function : Get free user table
;
;   format : USHORT     GetFreeUserTable(pTable, usArrays);
;
;   input  : PUSRTBL    pTable;         - top of user table
;            USHORT     usArrays;       - no. of arrays
;
;   output : USHORT     usHandle;       - handle value
;
;========================================================================
**/

USHORT  GetFreeUserTable(PUSRTBL pTable, USHORT usArrays)
{
    USHORT      usHandle, i;
    PUSRTBL     pUser;

    /* --- initialize --- */

    pUser    = pTable;                          /* ptr. to top of table     */
    usHandle = 0;                               /* initialize handle        */

    /* --- look for vacant user table --- */

    for (i = 0; i < usArrays; i++) {            /* look for free table      */

        /* --- could be found ? --- */

        if (! (pUser->fsFlags & PIF_NET_USED)) {/* is vacant cell ?         */
            usHandle = i + 1;                       /* compute handle       */
            break;                                  /* break my job         */
        }

        /* --- how about net table ? --- */

        pUser++;                                /* go to next user table    */
    }

    /* --- exit ... --- */

    return (usHandle);
}

#if defined(POSSIBLE_DEAD_CODE)

/**
;========================================================================
;
;   function : Exchange USHORT data type
;
;   format : USHORT     ExchangeUshort(usValue);
;
;   input  : USHORT     usValue;
;
;   output : USHORT     usRet;
;
;========================================================================
**/

USHORT  ExchangeUshort(USHORT usValue)
{
    USHORT      usRet;

    /* --- change endianness --- */

    _asm {
        mov     ax, usValue         ; (ax) = original value
        xchg    ah, al              ; (ax) = converted value
        mov     usRet, ax           ;
    }

    /* --- exit ... --- */

    return (usRet);
}

/**
;========================================================================
;
;   function : Exchange ULONG data type
;
;   format : ULONG      ExchangeUlong(ulValue);
;
;   input  : ULONG      ulValue;
;
;   output : ULONG      ulRet;
;
;========================================================================
**/

ULONG   ExchangeUlong(ULONG ulValue)
{
    ULONG       ulRet;

    /* --- change endianness --- */

    _asm {
        mov     ax, word ptr ulValue[0]     ; (ax) = original value
        xchg    ah, al                      ; (ax) = converted value
        mov     word ptr ulRet[2], ax       ;
        mov     ax, word ptr ulValue[2]     ; (ax) = original value
        xchg    ah, al                      ; (ax) = converted value
        mov     word ptr ulRet[0], ax       ;
    }

    /* --- exit ... --- */

    return (ulRet);
}
#endif

/*==========================================================================*\
;+                      E n d   O f   P r o g r a m                         +
\*==========================================================================*/
