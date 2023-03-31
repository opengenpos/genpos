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
*    USHORT  TimerProtocol(PMSGHDR, PPKTINF);
*    USHORT  IssueTimerRequest(USHORT, PMSGHDR, PPKTINF);
*    
*    USHORT  ReceiverProtocol(PMSGHDR, PPKTINF);
*    USHORT  ReceiverMessageProtocol(PMPHHDR, USHORT, ULONG, USHORT usIpPortNo, PPKTINF);
*    USHORT  ReceiverReplyProtocol(PMPHHDR, USHORT, ULONG, PPKTINF);
*    USHORT  IssueReceiverRequest(USHORT, PMSGHDR, PPKTINF);
*    USHORT  ChangeReceiverSocket(USHORT, PMSGHDR, PPKTINF);
*    
*    USHORT  ExeNetOpen(POPNDAT, USHORT, PPKTINF);
*    USHORT  ExeNetReceive(USHORT, PRCVDAT, USHORT, PPKTINF);
*    USHORT  ExeNetSend(USHORT, PSNDDAT, USHORT, PPKTINF);
*    USHORT  ExeNetControl(USHORT, PCNTDAT, USHORT, PPKTINF);
*    USHORT  ExeNetClose(USHORT, PPKTINF);
*    USHORT  ExeNetShutdown(USHORT, PPKTINF);
*    USHORT  SendAckMessage(PACKDAT, LPUCHAR, USHORT, PPKTINF);
*    VOID    TellPowerDownEvent(PPKTINF);
*
* ---------------------------------------------------------------------------
*   Update Histories:
* ---------------------------------------------------------------------------
*   Date     | Version  | Descriptions                          | By
* ---------------------------------------------------------------------------
*  Oct-06-15 | 02.02.01 | correct problem with cluster slowness | R.Chambers
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

/* --- C-library header --- */
#include    <memory.h>                      /* memxxx() C-library header    */
#include    <stdlib.h>                      /* standard library header      */
#include	<stdio.h>

#include   <ecr.h>
#include   <pif.h>

/* --- our common headers --- */
#include    "piftypes.h"                    /* type modifier header         */
#include    "log.h"                      /* PifNet common header         */
#include    "pifnet.h"                      /* PifNet common header         */
#include    "netmodul.h"                    /* my module header             */

/*==========================================================================*\
;+                                                                          +
;+      N E T W O R K    P R T O C O L     D E F I N I T I O N s            +
;+                                                                          +
\*==========================================================================*/

#if 0
// Notes on Network layer changes for US Customs lag issue in Rel 2.2.1 with Windows 7.
// R.Chambers, Oct-15-2015
//
// It appears that changes in Windows 7 network layer functionality has resulted in GenPOS
// behavior changes with messaging that cause problems that are not seen with Windows XP.
//
// Files primarily affected: netsystm.c, netqueue.c, netsem.c
// Files also affected: rptconfig.c, pif.h, pifnet.h, pifnet.c
//
// Changes to address user interface lag at US Customs due to Satellite Terminal to Master Terminal
// communications issues which result in time outs and retries by the Server Thread and Client Thread.
// 
// We have introduced a new network statistics, usTimerSendWithRecvPending, which measures the count
// of times when the TimerProtocol() function ages a pending send waiting on an acknowledgement and
// a message is received however the acknowledgement was not. We modified the AC888 report to include
// this new statistic.
// 
// We are now treating a received message for a user thread to be the same as an acknowledgement within
// the TimerProtocol() timer aging logic.
// 
// We have cleaned up a number of pieces in the network layer including:
//  - eliminated the source in netsem.c and using Windows Semaphore API directly
//  - eliminated use of SysEnterQueueRegion() and SysLeaveQueueRegion() as unnecessary
//  - using getsockopt() and setsockopt() to ensure send and receive buffers are 16K, not 8K
// 
// The result of these changes is that the lag seen with Windows 7 has been eliminated according to
// our testing.
// 
// What we have seen is that in some cases the Satellite Terminal would send a request message to the
// Master Terminal which would receive it. However the Satellite Terminal would never see the Ack
// from the Master Terminal so the pending Send of the Client Thread would time out even though the
// Master Terminal would send a response.
// 
// We have addressed this through two changes: (1) increase the send and receive buffers of the
// socket from 8K to 16K and (2) treat a received response message as an Acknowledgment for a pending
// Send that is waiting on an Ack.
// 
// It also appears that the queue functionality was over engineered so we have corrected that as well.
#endif

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
} MPHHDR, *PMPHHDR;

#define     ID_REPLY_BIT    0x80            /* reply bit in ID field        */
#define     ID_MESSAGE      0x1C            /* message data in ID field     */
#define     BC_MAX_COUNTS      3            /* max. no. of buffers / message*/

/* --- buffer control --- */

typedef struct _BUFCNT {                    /* buffer control               */
    USHORT  usLength;                           /* buffer length indicator  */
    UCHAR   uchType;                            /* buffer type              */
} BUFCNT, *PBUFCNT;

#define     BT_FUNCTION     1               /* function data                */
#define     BT_STATUS       2               /* status data                  */
#define     BT_USER_DATA    4               /* user data                    */

/* --- function buffer --- */

typedef struct _FNCBUF {                    /* function buffer              */
    UCHAR   uchCode1;                           /* function code 1          */
    UCHAR   uchCode2;                           /* function code 2          */
} FNCBUF, *PFNCBUF;

#define     FC_DEF_CODE1        0x80        /* code 1 pattern by default    */
#define     FC_DEF_CODE2        0x00        /* code 2 pattern by default    */
#define     FC_DGRAM_ACK        0x60        /* datagram ack. required       */
#define     FC_MSEC_TIMER       0x7F        /* test bits for msec timer     */

/* --- status buffer --- */

typedef struct _STTBUF {                    /* status buffer                */
    USHORT  usStatus;                           /* status                   */
} STTBUF, *PSTTBUF;

#define     ST_NO_ERROR         0x0000      /* no error                     */
#define     ST_TARGET_BUSY      0x3909      /* target busy                  */
#define     ST_UNDELIVER        0x3D09      /* undeliverable                */
#define     ST_INVALID_MSG      0x3B09      /* invalid message format       */
#define     ST_TARGET_CLEAR     0x3C09      /* message was cleared from receive queue */

/* --- MPH length in normal user message --- */

#define     MPH_LENGTH  (sizeof(MPHHDR) + sizeof(BUFCNT) * 2 + sizeof(FNCBUF))

/* -------------------------------------------- *\
 +  There is no limitation for the structures.  +
\* -------------------------------------------- */

#pragma pack()                              /* reinstate structure packing  */

/*==========================================================================*\
;+                                                                          +
;+                  L O C A L     D E F I N I T I O N s                     +
;+                                                                          +
\*==========================================================================*/

/* --- some macros --- */
                                            /* compute tick counts          */
#define     TICK(m, t)              (((m) / (t)) + (((m) % (t)) ? 1 : 0))
#define     LASTERROR(last, new)    ((last) ? (last) : (new))

// Number of milliseconds between each timer event within NetManager () thread
#define     TIMERTICKMS     100

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

/* --- continuous receiver thread I/F data struct for arguments --- */
/* --- sleep timer thread I/F data struct for arguments --- */

typedef struct _RCVARG {                    /* argument for NetReceiver()   */
    USHORT  usStatus;                           /* initialized status       */
    EVTOBJ  evtAck;                             /* acknowledge signal       */
    USHORT  usQueue;                            /* ID of my queue           */
    INT     iSocket;                            /* handle of socket         */
    USHORT  usIdle;                             /* idle timer value in msec */
} RCVARG, *PRCVARG;


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
	MPHHDR      RequestMphHdr;                  // copy of the MPHHDR struct from the last received message
} SNDINF, FAR *PSNDINF;

typedef struct {
	USHORT  usLastSenduchNo;          // pMph->uchNo value of last sent message
	USHORT  usLastRecvuchNo;
	SHORT   sLastSendPifError;
	SHORT   sLastRecvPifError;
	int     iLastSendErrorWsa;
	SHORT   sLastSendErrorPif;
	int     iLastRecvErrorWsa;
	SHORT   sLastRecvErrorPif;
} USRTBLINFO;

typedef struct _USRTBL {                    /* user management table        */
    USHORT      fsFlags;                        /* optional flags           */
    ULONG       ulRemoteAddr;                   /* target IP address        */
    ULONG       ulSourceAddr;                   /* source IP address        */
    USHORT      usIpPortNo;                     /* UDP/IP port number       */
    USHORT      usRemotePort;                   /* target NHPOS port number */
    USHORT      usLocalPort;                    /* user's port number       */
    USHORT      usSendTimer;                    /* send timer value in msec */
    USHORT      usSendTick;                     /* last receive time out    */
    ULONG       ulRecvTimer;                    /* recv timer value in msec */
    ULONG       ulRecvTick;                     /* last receive time out    */
    PMSGHDR     pSendMessage;                   /* ptr. to send req. message*/
    PMSGHDR     pRecvMessage;                   /* ptr. to recv req. message*/
    SNDINF      infSending;                     /* send message info.       */
    QUEPTR      queReceived;                    /* received message queue   */
	USRTBLINFO  usrStatusInfo;                  /* generated for PIF_NET_GET_USER_INFO */
#if defined (NET_TALLY)
    USRTLY      infTally;                       /* tally area               */
#endif
} USRTBL, FAR *PUSRTBL;

typedef struct _RCVPND {                    /* receive pending message      */
    QUEBUF      queControl;                     /* queue control area       */
    BOOL        fAck;                           /* datagram ack. required   */
    USHORT      usSeqNo;                        /* sequence counter         */
    USHORT      usSourcePort;                   /* sender's NHPOS port no.  */
    ULONG       ulSourceAddr;                   /* sender's IP address      */
    USHORT      usIpPortNo;                     /* UDP/IP port number       */
    USHORT      usPendTick;                     /* timer value to live      */
	USHORT      usReturn;                       /* status reported by sending user */
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


static const  ULONG  ulNetManagerStatusTimerCheck   =  0x00000001;

static ULONG  ulNetManagerStatusFlags = 0;

/*==========================================================================*\
;+                                                                          +
;+          P R O T O T Y P E     D E C L A R A T I O N s                   +
;+                                                                          +
\*==========================================================================*/

/* ---------------------------- *\
 +    Network Manager Thread    +
\* ---------------------------- */

VOID THREAD NetManager(PNETARG);
USHORT      CreateEndPoint(USHORT, PINT);

/* --- tick timer procedure --- */

USHORT  TimerProtocol(PMSGHDR, PPKTINF);
USHORT  IssueTimerRequest(USHORT, PMSGHDR, PPKTINF);

/* --- receiver procedure --- */

USHORT  ReceiverProtocol(PMSGHDR, PPKTINF);
USHORT  ReceiverMessageProtocol(PMPHHDR, USHORT, ULONG, USHORT usIpPortNo, PPKTINF);
USHORT  ReceiverReplyProtocol(PMPHHDR, USHORT, ULONG, PPKTINF);
USHORT  IssueReceiverRequest(USHORT, PMSGHDR, PPKTINF);
USHORT  ChangeReceiverSocket(USHORT, PMSGHDR, PPKTINF);

/* --- executable routines --- */

USHORT  ExeNetOpen(POPNDAT, USHORT, PPKTINF);
USHORT  ExeNetReceive(USHORT, PRCVDAT, USHORT, PPKTINF);
USHORT  ExeNetSend(USHORT, PSNDDAT, USHORT, PPKTINF);
USHORT  ExeNetControl(USHORT, PCNTDAT, USHORT, PPKTINF);
USHORT  ExeNetClose(USHORT, PPKTINF);
USHORT  ExeNetShutdown(USHORT, PPKTINF);
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

USHORT      SpawnReceiver(USHORT, PPKTINF);
VOID THREAD NetReceiver(PRCVARG);

/* ---------------------------- *\
 +      Sleep Timer Thread      +
\* ---------------------------- */

USHORT      SpawnTimer(USHORT);
VOID THREAD SleepTimer(PRCVARG);

/* ---------------------------- *\
 +        Sub-Routines          +
\* ---------------------------- */

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
# define    STLYRECVLOSTTIMEOUT(tly)    ((tly).ulRecvLostTimeOut)++
# define    STLYRECVLOSTQUEUEFULL(tly)  ((tly).ulRecvLostQueueFull)++
# define    STLYLASTSENTERROR(tly, s) (tly).usLastSentError = (USHORT)WSAGetLastError()
# define    STLYLASTRECVERROR(tly, s) (tly).usLastRecvError = (USHORT)WSAGetLastError()
# define    STLYRECVLOSTNETCLOSE(tly)  ((tly).ulRecvLostNetClosed)++
# define    STLYACKSENTSTATUSBUSY(tly)  ((tly).ulAckSentStatusBusy)++
# define    STLYSENTUNREACHABLEERROR(tly)  ((tly).ulSendUnreachableError)++
# define    UTLYTOTALSENT(tly)      ((tly).usTotalSent)++
# define    UTLYSENTERROR(tly)      ((tly).usSentError)++
# define    UTLYTOTALRECV(tly)      ((tly).usTotalRecv)++
# define    UTLYRECVERROR(tly)      ((tly).usRecvError)++
# define    UTLYRECVLOST(tly)       ((tly).usRecvLost)++
# define    UTLYPENDHIGH(tly)       ((tly).usPendingHighWater)++
# define    UTLYRECVLOSTTIMEOUT(tly)  ((tly).usRecvLostTimeOut)++
# define    UTLYRECVLOSTQUEUEFULL(tly) ((tly).usRecvLostQueueFull)++
# define    UTLYRECVREQSTTIMEOUT(tly)  ((tly).usRecvRequestTimeOut)++
# define    UTLYSENDREQSTTIMEOUT(tly)  ((tly).usSendRequestTimeOut)++
# define    UTLYSENDTARGETBUSY(tly)    ((tly).usSendTargetBusy)++
# define    UTLYRECVTARGETCLEAR(tly)    ((tly).usRecvTargetClear)++
# define    UTLYSENDTARGETCLEAR(tly)    ((tly).usSendTargetClear)++
# define    UTLYRECVPUTONQUEUE(tly)     ((tly).usRecvPutOnQueue)++
# define    UTLYRECVTAKENOFFQUEUE(tly)  ((tly).usRecvTakenOffQueue)++
# define    UTLYRECVGIVENNOW(tly)       ((tly).usRecvGivenNow)++
# define    UTLYRECVBADSEQNOPORT(tly)   ((tly).usRecvBadSeqNoPort)++
# define    UTLYRECVOUTOFRESOURCES(tly) ((tly).usRecvOutOfResources)++
# define    UTLYACKSENTSTATUSBUSY(tly)  ((tly).usAckSentStatusBusy) ++
# define    UTLYRECVLOSTNETCLOSE(tly)   ((tly).usRecvLostNetClosed)++
# define    UTLYRECVREQUESTWAITSTART(tly)   ((tly).usRecvRequestWaitStart)++
# define    UTLYRECVMESSAGEALREADYEXIST(tly)   ((tly).usRecvMessageAlreadyExist)++
# define    UTLYSENDMESSAGEALREADYEXIST(tly)   ((tly).usSendMessageAlreadyExist)++
# define    UTLYEXENETSENDCOUNT(tly)           ((tly).usExeNetSendCount)++
# define    UTLYEXENETRECEIVECOUNT(tly)        ((tly).usExeNetReceiveCount)++
# define    UTLYTIMERCHECKSKIPPED(tly)         ((tly).usTimerCheckSkipped)++
# define    UTLYTIMERSENDWITHRECVPENDING(tly)  ((tly).usTimerSendWithRecvPending)++
#else
# define    STLYTOTALSENT(tly)          ((void)0)
# define    STLYSENTERROR(tly)          ((void)0)
# define    STLYTOTALRECV(tly)          ((void)0)
# define    STLYRECVERROR(tly)          ((void)0)
# define    STLYOUTOFRESOURCES(tly)     ((void)0)
# define    STLYILLEGALMESSAGE(tly)     ((void)0)
# define    STLYRECVUNDELIVER(tly)      ((void)0)
# define    STLYRECVLOSTTIMEOUT(tly)    ((void)0)
# define    STLYRECVLOSTQUEUEFULL(tly)  ((void)0)
# define    STLYLASTSENTERROR(tly, s)   ((void)0)
# define    STLYLASTRECVERROR(tly, s)   ((void)0)
# define    UTLYTOTALSENT(tly)          ((void)0)
# define    UTLYSENTERROR(tly)          ((void)0)
# define    UTLYTOTALRECV(tly)          ((void)0)
# define    UTLYRECVERROR(tly)          ((void)0)
# define    UTLYRECVLOST(tly)           ((void)0)
# define    UTLYPENDHIGH(tly)           ((void)0)
# define    UTLYRECVLOSTTIMEOUT(tly)    ((void)0)
# define    UTLYRECVLOSTQUEUEFULL(tly)  ((void)0)
#endif


/*==========================================================================*\
;+                                                                          +
;+                  S T A T I C   V A R I A B L E s                         +
;+                                                                          +
\*==========================================================================*/

ULONG  ulMasterTerminalSourceAddr = 0;
ULONG  ulBackupMasterTerminalSourceAddr = 0;


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
    ULONG            idRet;
    static NETARG    argManager;
    PNETINF          pNew;

    /* --- define default parameters --- */
    static  NETINF  DefaultInfo   = { "3000",   /* DefaultInfo.pszLocalPort - my local port no.       */
                                      "3000",   /* DefaultInfo.pszRemotePort - remote port no.        */
                                         15 ,   /* DefaultInfo.usMaxUsers - max. users                */
                                          8 ,   /* DefaultInfo.usMaxQueues - max. queues usMaxQueues  */
                        #if defined (POS2170)
                              PIF_LEN_UDATA ,   /* max. length in byte      */
                        #else
                                       1500 ,   /* DefaultInfo.usMaxLength - max. length in byte      */
                        #endif
                                         48 ,   /* DefaultInfo.usWorkMemory - work memory size in KByte */
                                         50     /* DefaultInfo.usIdleTime - idle time  usIdleTime     */
                                    };

    /* --- make argument --- */
    argManager.usStatus = 0;                    /* init. execution status   */
    argManager.usQueue  = usQueue;              /* queue ID for the thread  */

    /* --- initialize surroundings --- */
    SysInitializeMemory();                      /* initialize memory manager*/
    SysInitializeQueue();                       /* initialize queue manager */

    /* --- create an event signal --- */
    SysEventCreate(&(argManager.evtAck), TRUE); /* create a signal          */

    /* --- make additional parameters --- */
    pNew                     = pInfo ? pInfo : &DefaultInfo;  /* evaluate info. ptr.  */
    argManager.pszLocalPort  = pNew->pszLocalPort  ? pNew->pszLocalPort : DefaultInfo.pszLocalPort;
    argManager.pszRemotePort = pNew->pszRemotePort ? pNew->pszRemotePort : DefaultInfo.pszRemotePort;
    argManager.usMaxLength   = pNew->usMaxLength   ? pNew->usMaxLength : DefaultInfo.usMaxLength;
    argManager.usMaxUsers    = pNew->usMaxUsers    ? pNew->usMaxUsers : DefaultInfo.usMaxUsers;
    argManager.usMaxQueues   = pNew->usMaxQueues   ? pNew->usMaxQueues : DefaultInfo.usMaxQueues;
    argManager.usWorkMemory  = pNew->usWorkMemory  ? pNew->usWorkMemory : DefaultInfo.usWorkMemory;
    argManager.usIdleTime    = pNew->usIdleTime    ? pNew->usIdleTime : DefaultInfo.usIdleTime;

    /* --- create network manager thread --- */
    if ((idRet = SysBeginThread (NetManager, 0, 0, PRIO_NET_MGR, &argManager)) != -1L) {
        SysEventWait(&(argManager.evtAck), WAIT_FOREVER);
    }

    /* --- close the event signal --- */
    SysEventClose(&(argManager.evtAck));        /* close event signal       */

    return ((idRet == -1) ? PIF_ERROR_NET_CRITICAL : argManager.usStatus);
}

/**
;========================================================================
;
;   function : Network management thread
;
;   format : VOID       NetManager(pArg);
;
;   input  : PNETARG    pArg;
;
;   output : nothing
;
;========================================================================
**/

VOID THREAD NetManager(PNETARG pArg)
{
	ULONG       ulBytes;
    USHORT      usRet, usStat, usRequester, usNet, usBytes;
    PMGRMSG     pMessage;
    PMSGHDR     pHeader;
    LPVOID      pvPtr;
    WSADATA     wsaData;

    /* --- declare my important resources & information --- */
    USHORT      usManager;         /* my queue ID              */
    USHORT      usReceiver;        /* receiver queue ID        */
    USHORT      usTimer;           /* timer queue ID           */
    PKTINF      infPacket;         /* function info. packet    */

    /* --- allocate receiver interface message --- */
    UCHAR       aucReceiver[sizeof(MSGHDR) + sizeof(RCVMSG)];

    /* --- allocate timer interface message --- */
    UCHAR       aucTimer[sizeof(MSGHDR) + sizeof(TMRMSG)];

    /* --- initialize --- */
    usStat = 0;                                 /* assume no error          */
    memset(&infPacket, '\0', sizeof(infPacket));

     /* --- get & compute parameters ---
		Be careful when modifying these parameters.  There are some places in
		which these parameters modify several different types of behavior such
		as usTick which influences timers, queue management, and other time
		related behaviors.
	 */
    infPacket.usManager     = pArg->usQueue;    /* my queue ID              */
    infPacket.usMaxQueues   = pArg->usMaxQueues;/* max. no. of queue buffers*/
    infPacket.usSizeBuffer  = pArg->usMaxLength;/* max. data length in byte */
    infPacket.usSizeBuffer += MPH_LENGTH;       /* adjust MPH header length */
    infPacket.usTick        = TIMERTICKMS;      /* tick time used in IssueTimerRequest () */
    infPacket.usIdleTime    = pArg->usIdleTime; /* idle time in msec.       */
    usManager               = pArg->usQueue;    /* my queue ID              */
    usReceiver              = (usManager + 1);  /* cont. receiver's queue ID*/
    usTimer                 = (usManager + 2);  /* sleep timer's queue ID   */

    /* --- initialize socket surroundings --- */
	// Release 2.0 uses WinSock 2.2 whereas old Rel 1.4.3 uses WinSock 1.1
    WSAStartup(MAKEWORD(2,2), &wsaData);        /* initialize winsock       */

	/* --- compute port numbers --- */
    infPacket.usPortLocal  = (USHORT)atol(pArg->pszLocalPort);
    infPacket.usPortRemote = (USHORT)atol(pArg->pszRemotePort);

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
    ulBytes =  pArg->usWorkMemory * KBYTES;     /* create by given size     */
    usRet   = (pArg->usWorkMemory <= 1000) ? 0 : PIF_ERROR_NET_BAD_DATA;
    usStat  = LASTERROR(usStat, usRet);         /* evaluate return stat */
#endif
    /* --- create my memory segment --- */
    infPacket.hMemory = SysCreateMemory(ulBytes);   /* create memory segment*/
    usRet             = infPacket.hMemory ? 0 : PIF_ERROR_NET_MEMORY;
    usStat            = LASTERROR(usStat, usRet);   /* evaluate return stat */

    /* --- initialize before creating user management table --- */
    infPacket.pUsers  = NULL;                   /* ptr. to user table       */
    infPacket.usUsers = 0;                      /* no. of users             */
    usBytes           = sizeof(USRTBL) * pArg->usMaxUsers;

    /* --- create user management table --- */
    if (pvPtr = SysAllocMemory(infPacket.hMemory, usBytes)) {
        infPacket.pUsers  = (PUSRTBL)pvPtr;         /* ptr. to user table   */
        infPacket.usUsers = pArg->usMaxUsers;       /* no. of users         */
        memset(pvPtr, 0, usBytes);                /* clear user table     */
    } else {
        usStat            = (USHORT)(PIF_ERROR_NET_MEMORY);
    }

    /* --- create send buffer --- */
    if (pvPtr = SysAllocMemory(infPacket.hMemory, infPacket.usSizeBuffer)) {
        infPacket.pvSendBuffer = pvPtr;
    } else {
        usStat                 = (USHORT)(PIF_ERROR_NET_MEMORY);
    }

    /* --- create receive buffer --- */
    if (pvPtr = SysAllocMemory(infPacket.hMemory, infPacket.usSizeBuffer)) {
        infPacket.pvRecvBuffer = pvPtr;
    } else {
        usStat                 = (USHORT)(PIF_ERROR_NET_MEMORY);
    }

    /* --- create my queue --- */
    usRet  = SysCreateQueue(usManager) ? PIF_ERROR_NET_QUEUE : usRet;
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- spawn timer thread --- */
    usRet  = SpawnTimer(usTimer);               /* spawn sleep timer thread */
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- spawn receiver thread --- */
    usRet  = SpawnReceiver(usReceiver, &infPacket);
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- make default message for timer --- */
    usRet  = IssueTimerRequest(usTimer, (PMSGHDR)aucTimer, &infPacket);
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- issue a receiver message --- */
    usRet  = IssueReceiverRequest(usReceiver, (PMSGHDR)aucReceiver, &infPacket);
    usStat = LASTERROR(usStat, usRet);          /* evaluate return status   */

    /* --- how's it going ? --- */
    pArg->usStatus = usStat;                    /* set my initialized status*/

    /* --- issue a signal --- */
    SysEventSignal(&(pArg->evtAck));            /* tell I'm ready           */

    /* --- do my job forever --- */
    while (! usStat) {                          /* do my job                */
        /* --- wait for a message requesting an action of some kind.
			We are looking for messages from the following sources:
				Timer message indicating time to do queue aging
				Receiver message indicating to process incoming data packet
				Process message indicating I/O request from a thread

			We have primed the Timer message and the Receiver message pumps
			by the IssueTimerRequest() and the IssueReceiverRequest() function
			calls above.  Now whenever we get a message from one of these sources,
			we will process the message and then re-issue the request.
		 */
        if (usRet = SysReadQueue(usManager, &pHeader, QUE_WAIT_FOREVER)) {
            usRet = (USHORT)(PIF_ERROR_NET_QUEUE);
            break;
        }

		if (!pHeader) continue;

        /* --- get information on the message --- */
        usRequester        = pHeader->usSender; /* who sent this message    */
        infPacket.pRequest = pHeader;           /* ptr. to a req. message   */

        /* --- who sent the message to me ? --- */
        if (usRequester == usTimer) {           /* from sleep timer thread ?*/
            TimerProtocol((PMSGHDR)(aucTimer), &infPacket);
            continue;
        }
        else if (usRequester == usReceiver) {   /* from receiver thread ?   */
			/* --- some received message ? --- */
            ReceiverProtocol((PMSGHDR)(aucReceiver), &infPacket);
            continue;
        }

        /* --- else, user requests a function ... --- */
        pMessage = (PMGRMSG)(pHeader->aucMessage);  /* ptr. to a message    */
        usNet    = pMessage->usHandle;          /* network handle           */
        pvPtr    = &(pMessage->datOption);      /* ptr. to optional data    */
        usBytes  = pMessage->usLength;          /* optional data length     */

        /* --- pass a control to each function --- */
        switch (pMessage->usCommand) {          /* what command ?           */
        case CMD_RECEIVE:                       /* PifNetReceive() ?        */
            usRet = ExeNetReceive(usNet, pvPtr, usBytes, &infPacket);
            break;
        case CMD_SEND:                          /* PifNetSend() ?           */
            usRet = ExeNetSend(usNet, pvPtr, usBytes, &infPacket);
            break;
        case CMD_CONTROL:                       /* PifNetControl() ?        */
            usRet = ExeNetControl(usNet, pvPtr, usBytes, &infPacket);
            break;
        case CMD_OPEN:                          /* PifNetOpen() ?           */
            usRet = ExeNetOpen(pvPtr, usBytes, &infPacket);  // Handle _NetOpen() request
            break;
        case CMD_CLOSE:                         /* PifNetClose() ?          */
            usRet = ExeNetClose(usNet, &infPacket);
            break;
        case CMD_SHUTDOWN:                      /* PifNetFinalize() ?       */
            usRet = ExeNetShutdown(usNet, &infPacket);
            break;
        default:                                /* invalid command ...      */
            usRet = (USHORT)(PIF_ERROR_NET_CRITICAL);
			break;
        }

        /* ---
			Was the user request handled or does the request need to be
			rescheduled?  If the request could be completed, then we will
			just signal the user thread that their I/O request is complete.

			The I/O request may have completed successfully or not.

			Rescheduling of I/O requests normally happen in the case of a CMD_RECEIVE
			request in which there is no data waiting for a read request and a timeout
			period was specified on the I/O request.  In that case the read request has
			to wait for a data packet to come in for that particular thread.

			Rescheduling is done in a sideways fashion.  Rather than putting the I/O
			request back on the manager queue, ExeNetReceive indicates that there
			is a pending read I/O request by setting the pUser->pRecvMessage
			variable to the requesting thread's receiving buffer address.  Then when
			ReceiverProtocol() is processing an incoming message, if it sees that the
			value of pUser->pRecvMessage is non-zero, it knows there is a pending read
			I/O request and the data is transferred and the thread signaled to complete
			the I/O request.
		 */
        if (usRet != (USHORT)(PIF_ERROR_NET_RE_SCHEDULE)) {
            /* --- respond status --- */
            pMessage->usReturn = usRet;             /* return status        */
            pHeader->usSender  = usManager;         /* my signature         */

            /* --- issue a done signal --- */
            SysEventSignal(&(pHeader->evtSignal));  /* issue a signal       */
        }
    }

    /* --- well mannered ... --- */
    SysEndThread();
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
	const int RcvBufSize = 2048 * 16;    // network buffer size in bytes per terminal times number of terminals, setsockopt()

	int error = 0;
    INT                 iOpt = 1, iOptLen = sizeof(INT);
    INT                 iSocket;
    struct sockaddr_in  sinAddress;

    /* --- open a socket --- */
    if ((iSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        return ((USHORT)(PIF_ERROR_NET_NOT_PROVIDED));
    }

    /* --- set non-blocking state --- */
    /* --- enable broadcasting mode --- */
    if (setsockopt(iSocket, SOL_SOCKET, SO_BROADCAST, (const PCHAR)(&iOpt), sizeof(iOpt))) {
        return ((USHORT)(PIF_ERROR_NET_ERRORS));
    }

	// US Customs has reported problems with lagging of the GenPOS user interface on Satellite Terminals
	// in which starting a transaction by entering the first PLU number takes a long time as in several
	// seconds before the response is seen.
	// Investigation has found that there are circumstances in which the Satellite Terminal sends a request
	// to the Master Terminal and is waiting for an Acknowledgement message which never arrives so that
	// the PifNetSend() does a time out on the Satellite and the Master Terminal sees a Busy response when
	// it sends the response to the Satellite containing the data requested.
	// This behavior seems to be only with Windows 7 and not with Windows XP.
	// Using getsockopt() we found the default Windows 7 socket buffer is 8192 bytes or 8K so we are increasing
	// the size of the send and receive network buffers in WinSock to a minimum of 16K.
	iOptLen = sizeof(INT);
	error = getsockopt (iSocket, SOL_SOCKET, SO_RCVBUF, (PCHAR)(&iOpt), &iOptLen);
	if (error < 0) {
		error = WSAGetLastError();
	} else if (iOpt < RcvBufSize) {
		iOpt = RcvBufSize;
		error = setsockopt(iSocket, SOL_SOCKET, SO_RCVBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
		if (error < 0) {
			error = WSAGetLastError();
		}
	}
	iOptLen = sizeof(INT);
	error = getsockopt (iSocket, SOL_SOCKET, SO_SNDBUF, (PCHAR)(&iOpt), &iOptLen);
	if (error < 0) {
		error = WSAGetLastError();
	} else if (iOpt < RcvBufSize) {
		iOpt = RcvBufSize;
		error = setsockopt(iSocket, SOL_SOCKET, SO_SNDBUF, (const PCHAR)(&iOpt), sizeof(iOpt));
		if (error < 0) {
			error = WSAGetLastError();
		}
	}

    /* --- make local address --- */
    memset(&sinAddress, '\0', sizeof(sinAddress));
    sinAddress.sin_family      = AF_INET;
    sinAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    sinAddress.sin_port        = htons(usPortNo);

    /* --- bind with local address --- */
    if (error = bind(iSocket, (struct sockaddr far *)(&sinAddress), sizeof(sinAddress))) {
        error = WSAGetLastError();
		return ((USHORT)(PIF_ERROR_NET_ERRORS));
    }

    /* --- respond socket handle --- */
    *piSocket = iSocket;                        /* respond socket handle    */

    return (0);
}

/*==========================================================================*\
;+                                                                          +
;+                  T i c k   T i m e r   I / F                             +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Manage tick signal procedure
;
;   format : USHORT     TimerProtocol(pInterface, pPacket);
;
;   input  : PMSGHDR    pInterface;     - ptr. to a timer I/F message
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;   description:  Called by the NetManager() thread when it receives a timer
;                 message in order to perform user thread queue aging.  This
;                 function reviews all of the user thread queues decrementing
;                 the time to live tick count and if the tick count has expired
;                 removing the message from the user thread queue.
;========================================================================
**/
USHORT  TimerProtocol(PMSGHDR pInterface, PPKTINF pPacket)
{
    USHORT      i, usRet;
    QUEPTR      queCheck;
    ACKDAT      infAck;
    PUSRTBL     pUser;
    PMSGHDR     pMessage;
    PMGRMSG     pRequest;
    PRCVPND     pPending;

    /* --- re-issue tick request --- */
    IssueTimerRequest(pInterface->usSender, pInterface, pPacket);

	ulNetManagerStatusFlags |= ulNetManagerStatusTimerCheck;

    /* --- check all of users state --- */
    for (pUser = pPacket->pUsers, i = 0; i < pPacket->usUsers; pUser++, i++) {
        /* --- is this table being used ? --- */
        if (!pUser || ! (pUser->fsFlags & PIF_NET_USED)) {
            continue;                               /* try next table       */
        }

        /*
			--- is under requesting PifNetSend() ? ---

			We check to see if there is a pending PifNetSend() request.
			If there is, check to see if the timer has expired.
			If the timer has expired, then we send a signal back to the
			requesting thread that their specified time limit for the
			send request has expired.

			We set pSendMessage to NULL so as to indicate to other parts
			of net_body that there is no longer a pending send request
			for this user (thread).

		 */
		if (pUser->pSendMessage) {
			if (pUser->usSendTick > 0)
				pUser->usSendTick -= 1;
			if (QuePeekBuffer(&(pUser->queReceived))) {
				pMessage            = (PMSGHDR)(pUser->pSendMessage);
				pMessage->usSender  = pPacket->usManager;
				pRequest            = (PMGRMSG)(pMessage->aucMessage);
				pRequest->usReturn  = (USHORT)(PIF_OK);
				pUser->pSendMessage = NULL;
				SysEventSignal(&(pMessage->evtSignal));
				UTLYTIMERSENDWITHRECVPENDING(pUser->infTally);
			} else if (pUser->usSendTick == 0) {
				pMessage            = (PMSGHDR)(pUser->pSendMessage);
				pMessage->usSender  = pPacket->usManager;
				pRequest            = (PMGRMSG)(pMessage->aucMessage);
				pRequest->usReturn  = (USHORT)(PIF_ERROR_NET_TIMEOUT);
				pUser->pSendMessage = NULL;
				SysEventSignal(&(pMessage->evtSignal));
				UTLYSENDREQSTTIMEOUT(pUser->infTally);
				UTLYSENTERROR(pUser->infTally);
			}
        }

        /*
			--- is under requesting PifNetReceive() ? ---

			We check to see if there is a pending PifNetReceive() request.
			If there is, check to see if the timer has expired.
			If the timer has expired, then we send a signal back to the
			requesting thread that their specified time limit for the
			read request has expired.

			We set pRecvMessage to NULL so as to indicate to other parts
			of net_body that there is no longer a pending receive request
			for this user (thread).
		 */
		if((pUser->pRecvMessage) && QuePeekBuffer(&(pUser->queReceived))) {
			UTLYTIMERCHECKSKIPPED(pUser->infTally);
			continue;
		}

		// if the value of ulRecvTick is zero then we do not want to subtract anything from
		// it since that will cause it to roll over to a -1 which happens to be PIF_FOREVER.
		// this change corrects a problem seen at US Customs with slowness of a cluster
		// with Satellite Terminals.
		if (pUser->ulRecvTick > 0) {
			pUser->ulRecvTick -= (pUser->ulRecvTick != (ULONG)PIF_FOREVER) ? 1L : 0L;
		}
        if ((pUser->pRecvMessage) && (! pUser->ulRecvTick)) {
			// if there is a read request pending, a read buffer has been specified, and
			// the tick timer has expired then we issue a timeout error to the calling thread.
            pMessage            = (PMSGHDR)(pUser->pRecvMessage);
            pMessage->usSender  = pPacket->usManager;
            pRequest            = (PMGRMSG)(pMessage->aucMessage);
            pRequest->usReturn  = (USHORT)(PIF_ERROR_NET_TIMEOUT);
            pUser->pRecvMessage = NULL;
            SysEventSignal(&(pMessage->evtSignal));
            UTLYRECVREQSTTIMEOUT(pUser->infTally);
            UTLYRECVERROR(pUser->infTally);
        }

        /*
			--- inspect all of pending messages for this user ---

			The method we use here is to make a copy of the pending queue
			in our own area, initialize the user queue thereby removing any
			pending messages on the queue, and then putting any messages
			that still have some time to live back onto the user's queue.

			Those messages whose time has expired are not put back on the
			queue.  The system resources for expired messages is reclaimed.

			This particular method is dangerous in that we are manipulating
			the queue when others may be wanting to do something with the
			same queue.  We are synchronized with other users of QueGetBuffer ()
			for the pUser queue by the fact that all functions that are using
			these pUser queues are invoked by NetManager ().  NetManager ()
			handles only one message at a time thereby enforcing synchronization
			since all functions that modify the pUser queues are within the same
			NetManager () thread.
		 */
		queCheck = pUser->queReceived;
        QueInitialize(&(pUser->queReceived));

        /* --- inspect all of pending messages --- */
        while (pPending = (PRCVPND)QueGetBuffer(&queCheck)) {
            /* --- is still active message ? --- */
            if (pPending->usPendTick > 1) {   /* is any time to live ?    */
				--(pPending->usPendTick);     /* decrement the timer tick count and put back on queue    */
                QueListBuffer(&(pUser->queReceived), &(pPending->queControl));
                continue;
            }

             /* --- respond busy message ---
				We respond with a busy message if the sending user has requested
				an acknowledgement back.

				If the user which sent the message asked for an ack, then we send an ack
				with ST_TARGET_BUSY indicating we have dropped the message.  The usual
				reason for time out is because the receiving thread is busy doing something
				else and has requested a new message.

			 */
            if (pPending->fAck) {               /* datagram ack. required ? */
                /* --- make ack. data info. --- */
                infAck.ulTargetAddr = pPending->ulSourceAddr;
                infAck.usIpPortNo   = pPending->usIpPortNo;
                infAck.usTargetPort = pPending->usSourcePort;
                infAck.usNumber     = pPending->usSeqNo;
                infAck.usLocalPort  = pUser->usLocalPort;
                infAck.usStatus     = ST_TARGET_BUSY;

                /* --- send ack. message --- */
                usRet = SendAckMessage(&infAck, NULL, 0, pPacket);
				UTLYTOTALSENT(pUser->infTally);         /* count up tally, STLYTOTALSENT() done in SendAckMessage()  */
				UTLYACKSENTSTATUSBUSY(pUser->infTally);
				STLYACKSENTSTATUSBUSY(pPacket->infTally);
			}

            /* --- discard resources --- */
            SysFreeMemory(pPacket->hMemory, pPending);
            UTLYRECVLOST(pUser->infTally);
            UTLYRECVLOSTTIMEOUT(pUser->infTally);
			STLYRECVLOSTTIMEOUT(pPacket->infTally);       /* count up tally       */
		}
    }

	ulNetManagerStatusFlags &= ~ulNetManagerStatusTimerCheck;

    return (0);
}

/**
;========================================================================
;
;   function : Issue a timer request message
;
;   format : USHORT     IssueTimerRequest(usQueue, pRequest, pPacket);
;
;   input  : USHORT     usQueue;        - timer queue ID
;            PMSGHDR    pRequest;       - ptr. to a timr message
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;========================================================================
**/
USHORT  IssueTimerRequest(USHORT usQueue, PMSGHDR pRequest, PPKTINF pPacket)
{
    USHORT      usRet;
    PTMRMSG     pTimer;

    /* --- set sender ID --- */
    pRequest->usSender = pPacket->usManager;    /* my signature             */

    /* --- make timer value --- */
    pTimer             = (PTMRMSG)(pRequest->aucMessage);
    pTimer->usMilliSec = pPacket->usTick;

    /* --- issue timer message --- */
    if (usRet = SysWriteQueue(usQueue, &(pRequest->queControl))) {
        return ((USHORT)(PIF_ERROR_NET_QUEUE));
    }

    return (0);
}

/*==========================================================================*\
;+                                                                          +
;+              C o n t i n u o u s   R e c e i v e r   I / F               +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Manage received message procedure
;
;   format : USHORT     ReceiverProtocol(pInterface, pPacket);
;
;   input  : PMSGHDR    pInterface;     - ptr. to a receiver I/F message
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;   description:  Called by the NetManager() thread when it receives a message
;                 received message in order to process the message and give it
;                 to the proper user thread.  If there is not a pending read
;                 from the user thread, the message is put on the user thread's
;                 FIFO queue for processing the next time the user thread does
;                 a read.
;========================================================================
**/
USHORT  ReceiverProtocol(PMSGHDR pInterface, PPKTINF pPacket)
{
    USHORT      usBytes;
    ULONG       ulFrom;
	USHORT      usIpPortNo;
    PRCVMSG     pReceiver;
    PMPHHDR     pMph;

    /* --- normalize message address --- */
	if (pInterface == 0) return (0);
    pReceiver = (PRCVMSG)(pInterface->aucMessage);  /* ptr. to a message    */

    /* --- it was socket command ? --- */
    if (pReceiver->usCommand == CMD_CHANGE_SOCKET) {    /* change socket ?  */
        /* then, just go to receive data */
    }
    else if (! pReceiver->usStatus) {           /* no error in receiving ?  */
		/* --- is successfully received ? --- */
        /* --- count up tally --- */
        STLYTOTALRECV(pPacket->infTally);       /* count up tally           */

        /* --- what kind of message ?
			Normal messages, messages sent by ExeNetSend () have only ID_MESSAGE set.
			Reply messages, messages sent by SendAckMessage () also set ID_REPLY_BIT
		 */
        pMph    = (PMPHHDR)(pReceiver->puchRecvBuffer); /* ptr. to received */
        usBytes = pReceiver->usRecvBytes;           /* received length      */
        ulFrom  = pReceiver->ulRecvAddress;         /* sender IP address    */
        usIpPortNo  = pReceiver->usRecvIpPort;      /* sender UDP/IP port number   */

        /* --- what kind of message ? --- */
        switch (pMph->uchId) {                  /* what kind of data ?      */
        case ID_MESSAGE:                        /* normal message ?         */
            ReceiverMessageProtocol(pMph, usBytes, ulFrom, usIpPortNo, pPacket);
            break;
        case ID_MESSAGE | ID_REPLY_BIT:         /* datagram ack. message ?  */
            ReceiverReplyProtocol(pMph, usBytes, ulFrom, pPacket);
            break;
        default:                                /* unknown message ...      */
            break;
        }
    }
    else if (pReceiver->usStatus >= WSAEINTR) {   /* socket is closed by power down */
		/* --- else, power down occurred ? --- */
        /* --- once close my socket --- */
        closesocket(pPacket->iSocket);          /* close my socket          */

        /* --- tell power down event to all of users --- */
        /* TellPowerDownEvent(pPacket);            / tell power down event    */

        /* --- re-create new socket handle --- */
        CreateEndPoint(pPacket->usPortLocal, &(pPacket->iSocket));

        /* --- go to change socket handle --- */
        ChangeReceiverSocket(pInterface->usSender, pInterface, pPacket);
        return (0);
    }
    else {                                      /* failed to receive data   */
		/* --- else, something wrong ... --- */
        STLYRECVERROR(pPacket->infTally);
        STLYLASTRECVERROR(pPacket->infTally, pReceiver->usStatus);
        SysSleep((ULONG)(pPacket->usIdleTime));     /* discard my CPU time  */
    }

    /* --- re-issue receive request --- */
    IssueReceiverRequest(pInterface->usSender, pInterface, pPacket);

    return (0);
}

/**
;========================================================================
;
;   function : Manage received normal message procedure
;
;   format : USHORT     ReceiverMessageProtocol(pMph, usBytes, ulFrom, pPacket);
;
;   input  : PMPHHDR    pMph;           - ptr. to received data
;            USHORT     usBytes;        - no. of bytes
;            ULONG      ulFrom;         - sender IP address
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;========================================================================
**/

USHORT  ReceiverMessageProtocol(PMPHHDR pMph, USHORT  usBytes, ULONG  ulFrom, USHORT  usIpPortNo, PPKTINF pPacket)
{
    BOOL        fValid, fNormalMode, fSendAck = FALSE;
    USHORT      i, usRet, usLength, usLast, usSize;
	USHORT      usQueEnumBufferCount;
    USHORT      usStat;      // Network layer status for the receiving user
	USHORT      usReturn;    // Network layer status reported by the sending user
    ACKDAT      infAck;
    LPUCHAR     puchData, puchUser, puchTarget;
    PUSRTBL     pUser;
    PMSGHDR     pMessage = NULL;
    PMGRMSG     pRequest;
    PRCVDAT     pReceive;
    PRCVPND     pPending;
    PBUFCNT     pBuf;
    PFNCBUF     pFunc;
    PXGRAM      pHeader;
    PXGADDR     pSender;
    PSTTBUF     pStat = NULL;

    /* --- initialize --- */
    puchUser = NULL;                           /* init. user data ptr.     */
    usLength = 0;                               /* init. user data length   */
    pFunc    = NULL;                           /* init. function code ptr. */
    usLast   =  usBytes;                        /* received data length     */
    usLast  -= (usBytes > sizeof(MPHHDR)) ? sizeof(MPHHDR) : usLast;

    /* --- evaluate valid message or not --- */
    fValid   =  usLast                       ? TRUE   : FALSE;
    fValid   = (pMph->uchBc < BC_MAX_COUNTS) ? fValid : FALSE;
    pBuf     = (PBUFCNT)(pMph + 1);             /* ptr. to 1st buffer       */
    puchData = (LPUCHAR)(pBuf + pMph->uchBc);   /* ptr. to 1st data         */

     /* --- compute & get message info. ---
		For a message we have the following structure:
			a MPHHDR which contains the message header
			one or more BUFCNT which contains buffer headers, one per buffer
			one or more buffer areas of various kinds such as STTBUF or FNCBUF or other	
		
			Standard messages, as opposed to reply messages, don't normally
			have a BT_STATUS section but in the case where sending user is
			sending a response message due to ST_TARGET_BUSY or other network
			layer issue, the network layer will provide a BT_STATUS buffer in the
			message.
.
			Standard messages are messages to transmit info or a request and
			are then replied to with either the requesting info or an acknowledgement.
			See ExeNetSend () for how a standard message is built.
	 */
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
            pStat     = (PSTTBUF)puchData;          /* ptr. to status buffer*/
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
        STLYILLEGALMESSAGE(pPacket->infTally);      /* count up tally       */
        return (0);                                 /* cannot treat it !    */
    }

    /* --- initialize before responding data ---
		We are using two different variables to monitor two different kinds of status
		at the network layer.  The first variable, usStat, is used to determine
		the network layer status on the receiving end which is this current thread.
		The variable usReturn is used to contain the network layer status of the
		sending user as reported by the sending user.

		We will report back up into the application layer the usReturn variable
		which is the status reported by the sending user.

		If the sending user has requested an acknowledgment, then when we send the
		acknowledgment, we will report the receiving user's network layer status so
		we send back to the sending user the usStat variable in the acknowledgement.
	 */
    fSendAck = pFunc->uchCode1 & FC_DGRAM_ACK;  /* ack. control required ?  */
    usStat   = ST_UNDELIVER;                    /* assume undeli. status    */

    /*
		--- Locate the user in the user table and process the message ---

		Using the port number specified in the incoming message, look up the port
		number in the user table to determine which user should received this message.

		If found, either give the message directly to the user, if the user is
		ready to process a message (indicated by pRecvMessage containing address
		of a buffer for the message) or put the received message on the user's
		received messages queue.
	 */
    if (! (pUser = GetLocalUser(pMph->uchDp, pPacket->pUsers, pPacket->usUsers))){
		/* --- no proper user found --- */
        STLYRECVUNDELIVER(pPacket->infTally);       /* count up tally       */
    }
    else {
		/* proper user found ...

			--- compute execution status ---

			If the message contains a status buffer, then we determine the status
			to be returned by NetReceive () or PifNetReceive () by determing a
			standard PIF_ERROR return code based on the status specified.

			If there is no status buffer, then we will return PIF_OK and assume that
			there was no problem on the part of the user who sent the message.  In
			this case, the only error that can be detected is if the receiving user
			thread has specified a buffer that is too small to hold the sent message.
		 */
		usReturn = PIF_OK;

		if (pStat) {
			switch (pStat->usStatus) {                  /* how's it going ?         */
			case ST_NO_ERROR:                           /* no error ?               */
				usReturn = PIF_OK;
				break;
			case ST_TARGET_BUSY:                        /* target busy ?            */
				usReturn = (USHORT)(PIF_ERROR_NET_BUSY);
				UTLYSENDTARGETBUSY(pUser->infTally);
				UTLYRECVERROR(pUser->infTally);
				break;
			case ST_TARGET_CLEAR:
				usReturn = (USHORT)(PIF_ERROR_NET_CLEAR);
				UTLYSENDTARGETCLEAR(pUser->infTally);
				UTLYRECVERROR(pUser->infTally);
				break;
			case ST_UNDELIVER:                          /* undeliverable ?          */
				usReturn = (USHORT)(PIF_ERROR_NET_UNREACHABLE);
				UTLYRECVERROR(pUser->infTally);
				break;
			case ST_INVALID_MSG:                        /* invalid message ?        */
				usReturn = (USHORT)(PIF_ERROR_NET_MESSAGE);
				UTLYRECVERROR(pUser->infTally);
				break;
			default:                                    /* unknown error ...        */
				break;
			}
		}

		if (pUser->fsFlags & PIF_NET_RESTRICTED) {
			USHORT  fsRestricted = 1;    // Assume IP address is not ok

			if (ulMasterTerminalSourceAddr && ulFrom == ulMasterTerminalSourceAddr) {
				fsRestricted = 0;  // Matches the IP address of the Master Terminal so IP address is ok.
			}

			if (ulBackupMasterTerminalSourceAddr && ulFrom == ulBackupMasterTerminalSourceAddr) {
				fsRestricted = 0;  // Matches the IP address of the Master Terminal so IP address is ok.
			}

			if (fsRestricted) {
				return (0);
			}
		}

        /* ---
			is the user in NetReceive () waiting on a message requested
			through PifNetReceive ()?  If so, lets provide this message
			to the user.  Otherwise we will put it on the user's received
			messages queue.
		 */
        if (pMessage = pUser->pRecvMessage) {
            /* --- compute & get user info. ---
				pMessage is a pointer to a MSGHDR struct.  MSGHDR structs contain
				the header used within the queue primitives for managing messages
				that are put on queues.  This MSGHDR struct contains as the last
				part of the struct an array of bytes, aucMessage[1] which is
				used to indicate the beginning of the rest of the buffer which
				contains the network layer and the application layer parts of the
				message that has been queued.

				pRequest is a pointer to a MGRMSG struct.  This struct contains a
				header followed by a union of the different types of messages that
				the network layer puts on the user queues.  This header contains
				the network base layer management information which is used to
				manage the buffers flowing within the NetManager () thread.

				If you look at NetReceive () in NET_APIS.C, you will see that
				NetReceive () creates a buffer that is a MSGHDR struct with a
				MGRMSG struct appended to it.  NetReceive () then initializes the
				MGRMSG struct with the command, CMD_RECEIVE, and the pifnet network
				handle.  NetReceive () then initializes the RCVDAT part of the datoption
				union with the pointer to the user's (application layer) buffer and its
				size.  Notice also that NetReceive () returns whatever status is in
				pRequest->usReturn as it's return value which is what the user will
				see as the return code from PifReceive ().

				pReceive is a pointer to a RCVDAT struct which is the kind of message
				which flows between the NetReceiver () thread and the NetManager ()
				thread via the queue and back up to NetReceive.

				Within the pReceive struct is located the actual message or XGRAM that
				was received from a sending thread, probably on some other terminal.
			 */
            pRequest    = (PMGRMSG)(pMessage->aucMessage);
            pReceive    = (PRCVDAT)(&((pRequest->datOption).datReceive));
            pHeader     = (PXGRAM)(pReceive->pvBuffer);
            pSender     =         (pReceive->pSender);
            puchTarget  = (LPUCHAR)(pReceive->pvBuffer);
            usSize      =          (pReceive->usBytes);
            fNormalMode = pUser->fsFlags & PIF_NET_NMODE;
            fNormalMode = pSender ? FALSE : fNormalMode;
            puchTarget += fNormalMode ? FAROF(auchData, XGRAM) : 0;
            usSize     -= fNormalMode ? FAROF(auchData, XGRAM) : 0;

            usReturn    = (usSize < usLength) ? PIF_ERROR_NET_BUFFER_OVERFLOW : usReturn;
			usStat      = (usSize < usLength) ? ST_INVALID_MSG : ST_NO_ERROR;


			/* --- respond received data to user --- */
            memcpy(puchTarget, puchUser, MINOF(usSize, usLength));

            /* --- respond sender address info. --- */
            if (pSender) {                      /* prepared sender info. ?  */
                *(LPULONG)(pSender->auchAddr) = ulFrom;
                pSender->usPort               = pMph->uchSp;
                usLast                        = 0;  /* no adjusted bytes    */
            } else if (fNormalMode) {           /* normal mode ?            */
                *(LPULONG)(pHeader->auchFaddr) = ulFrom;
                pHeader->usFport               = pMph->uchSp;
                pHeader->usLport               = pUser->usLocalPort;
                usLast                         = FAROF(auchData, XGRAM);
            } else {                            /* other mode ...           */
                usLast                        = 0;  /* no adjusted bytes    */
            }

            /* --- wake up the user ---
				If the user was waiting on a message, then we will just return the
				message directly by setting the appropriate status information
				into the message control header, MSGHDR, pointed to by pMessage
				and the MGRMSG control header used by NetReceive (), and then
				sending the user a signal to inform them a message has been received.
			 */
            pMessage->usSender  = pPacket->usManager;
			if (usReturn == PIF_OK) {
				pRequest->usReturn  = usLength;
				pRequest->usReturn += usLast;       /* adjust return length     */
			}
			else {
				pRequest->usReturn  = usReturn;
			}
			pUser->usIpPortNo = usIpPortNo;
            pUser->pRecvMessage = NULL;
            SysEventSignal(&(pMessage->evtSignal));
			UTLYRECVGIVENNOW(pUser->infTally);

            /* --- count up tally --- */
            if (usStat != ST_NO_ERROR) {             /* somthing wrong ?         */
                UTLYRECVERROR(pUser->infTally);
            }
        }

        /* --- compute buffer size for pending buffer ---
			Now we check to see if we had a pending receive request and therefore
			we have already provided this message to the receiving user.  If so,
			nothing to do but fall through to where we check to see if an acknowledgment
			should be sent.

			However, if the user was not waiting for a message then we need to
			queue this message for the user to retrieve when they next want a
			message.  We thus check to see if the user's queue is full or does
			it have room for this message.  If the queue is full, we respond with
			a ST_TARGET_BUSY.

			Before actually queuing the message, we allocate the system resources
			to hold the message.  If the allocation fails, we respond with a ST_TARGET_BUSY
			otherwise, we put the received message on the user's queue.  When adding
			to the queue, we do not want to send an acknowledgement.  The user will
			acknowledge the received message when they retrieve it from the queue or the
			queue aging in TimerProtocol () will send a ST_TARGET_BUSY if the message
			Time To Live expires.  We also get a ST_TARGET_BUSY if system resources don't
			allow the message buffering to operate properly.
		 */
        usSize = usLength + sizeof(RCVPND);     /* buffer size for pending  */

		usQueEnumBufferCount = QueEnumBuffer(&(pUser->queReceived));
		if (usQueEnumBufferCount > pUser->infTally.usPendingHighWater)
			pUser->infTally.usPendingHighWater = usQueEnumBufferCount;

        if (pMessage) { /* completed in advance ?   */
			; // Nothing to do except fall through and send an acknowledgement if needed
        }
        else if (usQueEnumBufferCount >= pPacket->usMaxQueues) {
			/* --- else, message should be queued ..., however check if over queued ? --- */
            UTLYRECVLOST(pUser->infTally);
            UTLYRECVLOSTQUEUEFULL(pUser->infTally);
			STLYRECVLOSTQUEUEFULL(pPacket->infTally);       /* count up tally       */
            usStat = ST_TARGET_BUSY;
			{
				char  xBuff[128];

				sprintf (xBuff, " Queued limit reached. ulFrom 0x%x, usQueEnumBufferCount = %d", ulFrom, usQueEnumBufferCount);
				NHPOS_ASSERT_TEXT(!ST_TARGET_BUSY, xBuff);
			}
        }
        else if (! (pPending = SysAllocMemory(pPacket->hMemory, usSize))) {
			/* --- else, no more resources ? --- */
            UTLYRECVLOST(pUser->infTally);
			UTLYRECVOUTOFRESOURCES(pUser->infTally);
            STLYOUTOFRESOURCES(pPacket->infTally);
            usStat = ST_TARGET_BUSY;
			{
				char  xBuff[128];

				sprintf (xBuff, " SysAllocMemory failed. ulFrom 0x%x, GetLastError = %d", ulFrom, GetLastError());
				NHPOS_ASSERT_TEXT(!ST_TARGET_BUSY, xBuff);
			}
        }
        else {
			/* --- else, set the message queued ? --- */
			/* set the message queued 
				The user did not have a NetReceive () or PifNetReceive () request
				outstanding so we put it on the user's received message queue to be
				picked up at a later time such as the next time the user issues
				a PifNetReceive ().  Aging of queued messages is handled by TimerProtocol ()
				which will periodically check the queues and provide to requesting users
				new messages that are on the queue.

				We set the fSendAck flag to false since the message is going on the user's
				receive queue and something may happen between now and when the user actually
				pulls the message from their queue.
			 */
            fSendAck               = FALSE;
            pPending->fAck         = pFunc->uchCode1 & FC_DGRAM_ACK;
            pPending->usSeqNo      = pMph->uchNo;
            pPending->usSourcePort = pMph->uchSp;
            pPending->ulSourceAddr = ulFrom;
			pPending->usIpPortNo   = usIpPortNo;
            pPending->usPendTick   = pFunc->uchCode1 & FC_DGRAM_ACK ? pFunc->uchCode2 & FC_MSEC_TIMER : DEF_PEND_TICK;
//			if (pPending->usPendTick > 2 && pPending->usPendTick < 0x7ff)
//				pPending->usPendTick--;      // subtract one tick to account for network latency
            pPending->usBytes      = usLength;
			pPending->usReturn     = usReturn;
            memcpy(pPending->auchMessage, puchUser, usLength);
            QueListBuffer(&(pUser->queReceived), (PQUEBUF)pPending);
			UTLYRECVPUTONQUEUE(pUser->infTally);
        }
		pUser->infSending.RequestMphHdr = *pMph;
    }

    /* --- datagram ack. required ? --- */
    if (fSendAck) {                             /* datagram ack. required ? */
        /* --- make ack. data info. --- */
        infAck.ulTargetAddr = ulFrom;               /* target IP address    */
        infAck.usIpPortNo   = usIpPortNo;           /* target UDP/IP port number */
        infAck.usTargetPort = pMph->uchSp;          /* target port no.      */
        infAck.usNumber     = pMph->uchNo;          /* message number       */
        infAck.usLocalPort  = pMph->uchDp;          /* source port no.      */
        infAck.usStatus     = usStat;               /* status               */

        /* --- send ack. message --- */
        puchUser = (LPUCHAR)((usStat == ST_UNDELIVER) ? puchUser : NULL);
        usLength =          ((usStat == ST_UNDELIVER) ? usLength : 0);
        usRet    = SendAckMessage(&infAck, puchUser, usLength, pPacket);
		if (pUser) {
			// If there is a valid user then modify its tallys
			// this may be hit if no valid user and we are replying with undelivered.
			UTLYTOTALSENT(pUser->infTally);      /* count up tally, STLYTOTALSENT() done in SendAckMessage() */
		}
    }

	return (0);
}

/**
;========================================================================
;
;   function : Manage received reply message procedure
;
;   format : USHORT     ReceiverReplyProtocol(pMph, usBytes, ulFrom, pPacket);
;
;   input  : PMPHHDR    pMph;           - ptr. to received data
;            USHORT     usBytes;        - no. of bytes
;            ULONG      ulFrom;         - sender IP address
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;========================================================================
**/
USHORT  ReceiverReplyProtocol(PMPHHDR pMph, USHORT  usBytes, ULONG   ulFrom, PPKTINF pPacket)
{
    BOOL        fValid;
    USHORT      i, usLast;
	USHORT      usReturn;  // PIF_ERROR status to return to user
    LPUCHAR     puchData;
    PUSRTBL     pUser;
    PMSGHDR     pMessage;
    PMGRMSG     pRequest;
    PBUFCNT     pBuf;
    PSTTBUF     pStat;

    /* --- initialize --- */
    pStat    = NULL;                           /* init. status buffer ptr. */
    usLast   =  usBytes;                        /* received data length     */
    usLast  -= (usBytes > sizeof(MPHHDR)) ? sizeof(MPHHDR) : usLast;

    /* --- evaluate valid message or not --- */
    fValid   =  usLast                       ? TRUE   : FALSE;
    fValid   = (pMph->uchBc < BC_MAX_COUNTS) ? fValid : FALSE;
    pBuf     = (PBUFCNT)(pMph + 1);             /* ptr. to 1st buffer       */
    puchData = (LPUCHAR)(pBuf + pMph->uchBc);   /* ptr. to 1st data         */

    /* --- compute & get message info. ---
		For a message we have the following structure:
			a MPHHDR which contains the message header
			one or more BUFCNT which contains buffer headers, one per buffer
			one or more buffer areas of various kinds such as STTBUF or FNCBUF or other		
	 */
    for (i = 0; fValid && (i < pMph->uchBc); i++) {
        /* --- what type of buffer ? --- */
        switch (pBuf->uchType) {                /* what type of buffer ?    */
        case BT_FUNCTION:                       /* function data ?          */
            puchData += sizeof(FNCBUF);             /* prepare to next buff */
            fValid    = (pBuf->usLength == sizeof(FNCBUF)) ? TRUE : FALSE;
            break;
        case BT_USER_DATA:                      /* user data ?              */
            puchData += pBuf->usLength;             /* prepare to next buff */
            break;
        case BT_STATUS:                         /* status data ?            */
            pStat     = (PSTTBUF)puchData;          /* ptr. to status buffer*/
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
    if ((! fValid) || (usLast) || (! pStat)) {  /* valid data received ?    */
        STLYILLEGALMESSAGE(pPacket->infTally);      /* count up tally       */
        return (0);                                 /* cannot treat it !    */
    }

    /*
		--- Locate the user in the user table and process the message ---

		Using the port number specified in the incoming message, look up the port
		number in the user table to determine which user should received this message.

		If found, either give the message directly to the user, if the user is
		ready to process a message (indicated by pRecvMessage containing address
		of a buffer for the message) or put the received message on the user's
		received messages queue.
	 */
    if (! (pUser = GetLocalUser(pMph->uchDp, pPacket->pUsers, pPacket->usUsers))) {
        return (0);                                 /* cannot found ...     */
    }

    /* --- is the user under sending ? ---
		Why does this routine, an acknowledgement sending routine, care about
		whether the user thread is sending?

		This routine actually provides the acknowledgement reply message to
		the user who is currently blocked on a NetSend ().  The way that an
		acknowledged send appears to work is for the sending user to send a message
		and then be blocked until an acknowledgment is returned from the target.

		The sending user can then check the PifNetSend () status return to see
		what kind of status the receiving user has returned.
	 */
    if (! pUser->pSendMessage) {                /* out of sending ?         */
        return (0);                                 /* done my job          */
    }

    /* --- is same message sequence no. etc. ? --- */
    if ((pMph->uchNo != (UCHAR)(pUser->infSending).usSeqNo)      ||
        (pMph->uchSp != (UCHAR)(pUser->infSending).usTargetPort) ||
        (ulFrom      !=        (pUser->infSending).ulTargetAddr)) {
		UTLYRECVBADSEQNOPORT(pUser->infTally);
        return (0);
    }

    /* --- compute execution status --- */
    switch (pStat->usStatus) {                  /* how's it going ?         */
    case ST_NO_ERROR:                           /* no error ?               */
        usReturn = PIF_OK;
        break;
    case ST_TARGET_BUSY:                        /* target busy ?            */
        usReturn = (USHORT)(PIF_ERROR_NET_BUSY);
		UTLYSENDTARGETBUSY(pUser->infTally);
        UTLYSENTERROR(pUser->infTally);
        break;
	case ST_TARGET_CLEAR:
		usReturn = (USHORT)(PIF_ERROR_NET_CLEAR);
		UTLYSENDTARGETCLEAR(pUser->infTally);
		UTLYSENTERROR(pUser->infTally);
		break;
    case ST_UNDELIVER:                          /* undeliverable ?          */
        usReturn = (USHORT)(PIF_ERROR_NET_UNREACHABLE);
        UTLYSENTERROR(pUser->infTally);
        break;
    case ST_INVALID_MSG:                        /* invalid message ?        */
        usReturn = (USHORT)(PIF_ERROR_NET_MESSAGE);
        UTLYSENTERROR(pUser->infTally);
        break;
    default:                                    /* unknown error ...        */
        usReturn = (USHORT)(PIF_ERROR_NET_CRITICAL);
        UTLYSENTERROR(pUser->infTally);
        break;
    }

    /* --- wake up the user --- */
    pMessage            = pUser->pSendMessage;  /* ptr. to req. message     */
    pMessage->usSender  = pPacket->usManager;   /* my signature             */
    pRequest            = (PMGRMSG)(pMessage->aucMessage);
    pRequest->usReturn  = usReturn;             /* execution status         */
    pUser->pSendMessage = NULL;                 /* done the send job        */
    SysEventSignal(&(pMessage->evtSignal));     /* signalize the user       */

    return (0);
}

/**
;========================================================================
;
;   function : Issue a receiver request message
;
;   format : USHORT     IssueReceiverRequest(usQueue, pRequest, pPacket);
;
;   input  : USHORT     usQueue;        - receiver queue ID
;            PMSGHDR    pRequest;       - ptr. to a receiver message
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;   Description:  This function puts a new request for a receive on the
					Receiver queue which will cause the NetReceiver ()
					thread to issue a new read request on our socket.
					When NetReceiver () gets a new UDP message in, it will
					put the message on the queue for the function ReceiverProtocol ()
					within the NetManager () thread to process.

					The last step that ReceiverProtocol () does is to call this
					function so that our transaction monitor will continue to
					run accepting and processing messages.

;========================================================================
**/
USHORT IssueReceiverRequest(USHORT usQueue, PMSGHDR pRequest, PPKTINF pPacket)
{
    USHORT      usRet;
    PRCVMSG     pReceiver;

    /* --- set sender ID --- */
    pRequest->usSender = pPacket->usManager;    /* my signature             */

    /* --- make parameters for receiver --- */
    pReceiver                  = (PRCVMSG)(pRequest->aucMessage);
    pReceiver->usCommand       = CMD_RECEIVE_DATA;
    pReceiver->puchRecvBuffer  = (LPUCHAR)(pPacket->pvRecvBuffer);
    pReceiver->puchRecvBuffer += pPacket->usRecvReservedBytes;
    pReceiver->usRecvSize      = pPacket->usSizeBuffer; /* buffer size      */
    pReceiver->usRecvSize     -= pPacket->usRecvReservedBytes;

    /* --- issue receiver message --- */
    if (usRet = SysWriteQueue(usQueue, &(pRequest->queControl))) {
        return ((USHORT)(PIF_ERROR_NET_QUEUE));
    }

    return (0);
}

/**
;========================================================================
;
;   function : Change my socket handle
;
;   format : USHORT     ChangeReceiverSocket(usQueue, pRequest, pPacket);
;
;   input  : USHORT     usQueue;        - receiver queue ID
;            PMSGHDR    pRequest;       - ptr. to a receiver message
;            PPKTINF    pPacket;        - ptr. to a packet
;
;   output : USHORT     usRet;          - status
;
;========================================================================
**/
USHORT ChangeReceiverSocket(USHORT usQueue, PMSGHDR pRequest, PPKTINF pPacket)
{
    USHORT      usRet;
    PRCVMSG     pReceiver;

    /* --- set sender ID --- */
    pRequest->usSender = pPacket->usManager;    /* my signature             */

    /* --- make parameters for receiver --- */
    pReceiver              = (PRCVMSG)(pRequest->aucMessage);
    pReceiver->usCommand   = CMD_CHANGE_SOCKET;
    pReceiver->iRecvSocket = pPacket->iSocket;

    /* --- issue receiver message --- */
    if (usRet = SysWriteQueue(usQueue, &(pRequest->queControl))) {
        return ((USHORT)(PIF_ERROR_NET_QUEUE));
    }

    return (0);
}

/*==========================================================================*\
;+                                                                          +
;+              E x e c u t a b l e    R o u t i n e s                      +
;+                                                                          +
\*==========================================================================*/

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
;   description:  This function handles the CMD_OPEN command.
;                 This function is triggered by a call to _NetOpen()
;========================================================================
**/
USHORT  ExeNetOpen(POPNDAT pOpen, USHORT usBytes, PPKTINF pPacket)
{
    USHORT      usNet, usLocalPort;
    PUSRTBL     pUser = NULL;
	USHORT      usPortFlags;

	usPortFlags = pOpen->usLport;
	pOpen->usLport &= PIF_PORT_FLAG_REMOVE;

    /* --- is valid port no. given ? --- */
    if (pOpen->usLport > PORT_LOCAL_MAX) {      /* over max. port no. ?     */
        return ((USHORT)(PIF_ERROR_NET_BAD_DATA));
    }

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
	if (pUser) {
		/* --- normalize my local port number --- */
		if (! (usLocalPort = (pOpen->usLport == ANYPORT) ? 0 : pOpen->usLport)) {
			usLocalPort = usNet + PORT_AUTO_ASSIGN;
		}

		/* --- initialize user control table --- */
		memset(pUser, 0, sizeof(USRTBL));      /* clear user table         */

		/* --- make up user table --- */
		pUser->fsFlags      = PIF_NET_USED;         /* block this table         */
		pUser->ulRemoteAddr = pOpen->ulFaddr;       /* target address           */
		pUser->usRemotePort = pOpen->usFport;       /* target port number       */
		pUser->usLocalPort  = usLocalPort;          /* local port number        */
		pUser->usSendTimer  = DEF_SEND_TIME;        /* timeout value in msec.   */
		pUser->fsFlags     |= (pOpen->usFport == ANYPORT) ? PIF_NET_NMODE : PIF_NET_DMODE;
		pUser->fsFlags     |= (usPortFlags & PIF_PORT_FLAG_CLUSTER) ? PIF_NET_RESTRICTED : 0;  // port restricted to cluster messages only
		pUser->ulSourceAddr = 0;

		/* --- initialize user queue --- */
		QueInitialize(&(pUser->queReceived));       /* initialize pending queue */
	}

    return (usNet);
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
USHORT ExeNetReceive(USHORT usNet, PRCVDAT pReceive, USHORT usBytes, PPKTINF pPacket)
{
    SHORT       sRead;
    USHORT      usSize;
    ULONG       ulTime;
    ACKDAT      infAck;
    LPUCHAR     puchBuffer = NULL;
    PUSRTBL     pUser = NULL;
    PXGRAM      pHeader = NULL;
    PRCVPND     pPending = NULL;

    /* --- compute its user table ptr. --- */
    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

	UTLYEXENETRECEIVECOUNT(pUser->infTally);

    /* --- is opened user ? --- */
    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* invalid handle       */
    }

	/* --- is already scheduled ? --- */
    if (pUser->pRecvMessage) {                  /* is already scheduled ?   */
		UTLYRECVMESSAGEALREADYEXIST(pUser->infTally);
        return ((USHORT)(PIF_ERROR_NET_ALREADY_EXIST));
    }

    /* --- power down occurred ? --- */
    if (! ((~pUser->fsFlags) & (PIF_NET_EVENT_MODE | PIF_NET_POWER_FAIL))) {
        return ((USHORT)(PIF_ERROR_NET_POWER_FAILURE));
    }

    /* --- count up tally --- */
    UTLYTOTALRECV(pUser->infTally);             /* count up tally           */

    /* --- is any received data ? ---
		Check the user's queue to determine if the user has any
		received messages that have been put on the user's queue
		by the ReceiverProtocol () message functions.

		If there are not pending messages, then if there is a timeout
		period associated with this PifReceive () then we will mark the
		user as waiting on a message and return.  The TimerProtocol ()
		function will handle the timeout for the waiting request.

		If there is a pending message, lets give it to the user.

		If the user has specified PIF_NO_WAIT in the read request, then we
		immediately return back indicating a timeout for the read request.

		Otherwise, we will set pRecvMessage to point to the user's read buffer
		and then indicate that we are scheduling the read request to be
		satisfied later.  We set the Time To Live for the read request so that
		it can be aged appropriately within the Timer functionality of net_body.

	 */
	NHPOS_ASSERT_TEXT(((ulNetManagerStatusFlags & ulNetManagerStatusTimerCheck) == 0), "==WARNING: timer check.");

    if (! (pPending = (PRCVPND)QuePeekBuffer(&(pUser->queReceived)))) {
        /* --- compute wait timer value --- */
        ulTime = (pUser->fsFlags & PIF_NET_TMODE) ? pUser->ulRecvTimer : (ULONG)PIF_FOREVER;

        /* --- immediate return ? --- */
        if (ulTime == (ULONG)PIF_NO_WAIT) {     /* no wait ?                */
            UTLYRECVERROR(pUser->infTally);
            return ((USHORT)(PIF_ERROR_NET_TIMEOUT));/* no data received !  */
        }
        else if (ulTime == (ULONG)PIF_FOREVER) {/* forever ?                */
			/* --- else, wait until received ? --- */
            pUser->pRecvMessage = pPacket->pRequest;/* save req. msg. addr. */
            pUser->ulRecvTick   = (ULONG)PIF_FOREVER;   /* wait forever !   */
			UTLYRECVREQUESTWAITSTART(pUser->infTally);
            return ((USHORT)(PIF_ERROR_NET_RE_SCHEDULE));
        }
        else {                                  /* spcific time ?           */
			/* --- else, some time value given ... --- */
            ulTime              = (ULONG)(pPacket->usTick);
            pUser->pRecvMessage = pPacket->pRequest;/* save req. msg. addr. */
            pUser->ulRecvTick   = TICK(pUser->ulRecvTimer, ulTime);
//			if (pUser->ulRecvTimer < 0x7f0)
//				pUser->ulRecvTick += 2;    // increment the wait timer by 2 ticks to account for network latency.
			UTLYRECVREQUESTWAITSTART(pUser->infTally);
            return ((USHORT)(PIF_ERROR_NET_RE_SCHEDULE));
        }
    }

    /* --- compute user buffer address ? --- */
    if ((pUser->fsFlags & PIF_NET_NMODE) && (! pReceive->pSender)) {
        pHeader    = (PXGRAM)(pReceive->pvBuffer);  /* top of header        */
        puchBuffer = pHeader->auchData;             /* buffer ptr.          */
        usSize     = pReceive->usBytes - FAROF(auchData, XGRAM);
    } else {                                    /* data only mode ...       */
        puchBuffer = (LPUCHAR)(pReceive->pvBuffer); /* top of buffer ptr.       */
        usSize     = pReceive->usBytes;         /* buffer size in byte      */
    }

    /* --- prepared a buffer with enough size ? --- */
    if (pPending->usBytes > usSize) {           /* user buffer small ?      */
        UTLYRECVERROR(pUser->infTally);
        return ((USHORT)(PIF_ERROR_NET_BUFFER_OVERFLOW));
    }

    /* --- get the buffer from the queue ---
		Thus far we have performed checks without actually taking the received
		message from the pending received message queue.  Now that the basic
		checks are out of the way, we pull the received message from the queue
		and give it to the receiving user thread which has a PifNetReceive ()
		outstanding.
	 */
	NHPOS_ASSERT_TEXT(((ulNetManagerStatusFlags & ulNetManagerStatusTimerCheck) == 0), "==WARNING: timer check.");
    pPending = (PRCVPND)QueGetBuffer(&(pUser->queReceived));
	if (pPending == NULL) {
		NHPOS_NONASSERT_NOTE("==STATE", "==STATE: ExeNetReceive() (pPending == NULL) return PIF_ERROR_NET_TIMEOUT");
        return ((USHORT)(PIF_ERROR_NET_TIMEOUT)); /* buffer address is invalid   */
	}

    sRead    = (SHORT)(pPending->usBytes);      /* data length in byte      */

	{
		USHORT  fsProcessMessage = 1;
		if (pUser->fsFlags & PIF_NET_RESTRICTED) {
			USHORT  fsRestricted = 1;    // Assume IP address is not ok

			if (ulMasterTerminalSourceAddr && pPending->ulSourceAddr == ulMasterTerminalSourceAddr) {
				fsRestricted = 0;  // Matches the IP address of the Master Terminal so IP address is ok.
			}

			if (ulBackupMasterTerminalSourceAddr && pPending->ulSourceAddr == ulBackupMasterTerminalSourceAddr) {
				fsRestricted = 0;  // Matches the IP address of the Master Terminal so IP address is ok.
			}

			if (fsRestricted) {
				/* --- compute wait timer value --- */
				ulTime = (pUser->fsFlags & PIF_NET_TMODE) ? pUser->ulRecvTimer : (ULONG)PIF_FOREVER;

				/* --- immediate return ? --- */
				if (ulTime == (ULONG)PIF_NO_WAIT) {     /* no wait ?                */
					UTLYRECVERROR(pUser->infTally);
					return ((USHORT)(PIF_ERROR_NET_TIMEOUT));/* no data received !  */
				}
				else if (ulTime == (ULONG)PIF_FOREVER) {/* forever ?                */
					/* --- else, wait until received ? --- */
					pUser->pRecvMessage = pPacket->pRequest;/* save req. msg. addr. */
					pUser->ulRecvTick   = (ULONG)PIF_FOREVER;   /* wait forever !   */
					return ((USHORT)(PIF_ERROR_NET_RE_SCHEDULE));
				}
				else {                                  /* spcific time ?           */
					/* --- else, some time value given ... --- */
					ulTime              = (ULONG)(pPacket->usTick);
					pUser->pRecvMessage = pPacket->pRequest;/* save req. msg. addr. */
					pUser->ulRecvTick   = TICK(pUser->ulRecvTimer, ulTime);
					return ((USHORT)(PIF_ERROR_NET_RE_SCHEDULE));
				}
			}
		}

		if (fsProcessMessage) {
			pUser->usIpPortNo = pPending->usIpPortNo;  // remember the IP port from which this message came
			pUser->ulSourceAddr = pPending->ulSourceAddr;  // remember the IP address from which this message came

			/* --- respond received data --- */
			memcpy(puchBuffer, pPending->auchMessage, pPending->usBytes);

			/* --- in normal mode ? --- */
			if (pReceive->pSender) {                    /* prepared sender info. ?  */
				*(LPULONG)(pReceive->pSender->auchAddr) = pPending->ulSourceAddr;
				pReceive->pSender->usPort               = pPending->usSourcePort;
			} else if (pUser->fsFlags & PIF_NET_NMODE) {/* normal mode ?            */
				sRead                         += FAROF(auchData, XGRAM);
				*(LPULONG)(pHeader->auchFaddr) = pPending->ulSourceAddr;
				pHeader->usFport               = pPending->usSourcePort;
				pHeader->usLport               = pUser->usLocalPort;
			}

			/* --- required ack. control ? ---
				If the sending user has requested an acknowledgement, then send
				an ack indicating everything is fine.
			 */
			if (pPending->fAck) {                       /* datagram ack. required ? */
				/* --- make ack. data info. --- */
				infAck.ulTargetAddr = pPending->ulSourceAddr;   /* target IP address*/
				infAck.usIpPortNo   = pPending->usIpPortNo;     /* target UDP/IP port number */
				infAck.usTargetPort = pPending->usSourcePort;   /* target port no.  */
				infAck.usNumber     = pPending->usSeqNo;    /* message number       */
				infAck.usLocalPort  = pUser->usLocalPort;   /* source port no.      */
				infAck.usStatus     = ST_NO_ERROR;          /* status               */

				/* --- send ack. message --- */
				SendAckMessage(&infAck, NULL, 0, pPacket); /* send ack. message    */
				UTLYTOTALSENT(pUser->infTally);             /* count up tally, STLYTOTALSENT() done in SendAckMessage() */
			}
			UTLYRECVTAKENOFFQUEUE(pUser->infTally);         /* count up tally           */
		}
	}

    /* --- discard received buffer --- */
    SysFreeMemory(pPacket->hMemory, pPending);  /* discard resources        */
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
USHORT ExeNetSend(USHORT usNet, PSNDDAT pSend, USHORT usBytes, PPKTINF pPacket)
{
    UCHAR               uchTimer;
    SHORT               sRet;
    USHORT              usRet, usHeader, usSize, usTargetPort;
    ULONG               ulTargetAddr;
    LPUCHAR             puchBuffer;
    PUSRTBL             pUser;
    PXGRAM              pHeader;
    PMPHHDR             pMph;
    PBUFCNT             pBuf;
    PFNCBUF             pFunc;
    struct sockaddr_in  sinClient;

    /* --- initialize --- */

    puchBuffer = (LPUCHAR)(pSend->pvBuffer);    /* top of buffer ptr.       */
    pHeader    = ( PXGRAM)(pSend->pvBuffer);    /* top of header            */
    usSize     = pSend->usBytes;                /* buffer size in byte      */
    usRet      = PIF_OK;                        /* assume good status       */

    /* --- compute its user table ptr. --- */
    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

	UTLYEXENETSENDCOUNT(pUser->infTally);

    /* --- is opened user ? --- */
    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* invalid handle       */
    }

    /* --- is already scheduled ? --- */
    if (pUser->pSendMessage) {                  /* is already scheduled ?   */
		UTLYSENDMESSAGEALREADYEXIST(pUser->infTally);
        return ((USHORT)(PIF_ERROR_NET_ALREADY_EXIST));
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
	if (pUser->usIpPortNo)
	{
		sinClient.sin_port = htons(pUser->usIpPortNo);
	}
	else
	{
		sinClient.sin_port = htons(pPacket->usPortRemote);
	}

    /* --- compute timer value in 100th msec unit --- */
    if (pUser->usSendTimer != PIF_NO_WAIT) {    /* datagram ack. required ? */
        // uchTimer = (UCHAR)TICK(pUser->usSendTimer - ADJ_SEND_TIME, 100);
		uchTimer = TICK(pUser->usSendTimer, pPacket->usTick) + 1;
    } else {                                    /* else, not required ...   */
        uchTimer = 0;
    }

    /* --- make info. of sending data --- */
    pUser->infSending.usSeqNo     += 1;                    /* count up seq. no.        */
    pUser->infSending.usTargetPort = usTargetPort;         /* target user port no.     */
    pUser->infSending.ulTargetAddr = ulTargetAddr;         /* target IP address        */

	/* --- make complete packet data ---
		A complete data packet is composed of the following sections:
			A MPHHDR struct with the network layer data used for managing data messages.
			One or more BUFCNT structs with headers describing the message contents.
			One or more buffers of message contents.

		Most user level data messages contain a BT_FUNCTION type of buffer and a BT_USER_DATA
		type of buffer.  Acknowledgement reply messages contain a BT_STATUS type of buffer
		with a status code data buffer.  See SendAckMessage () below for the function that
		builds those types of messages.

		Notice that this function sends an ID_MESSAGE type of message.
		Review the source code in function ReceiverProtocol () above which looks at the message
		type in pMph->uchId and determines how to decode the message.
		
		Also notice that if there is a timeout then an Acknowledgement is requested which means
		that this message is sent and the user thread will receive an acknowledgement message
		back from the receiving user (message with pMph->uchId = ID_MESSAGE | ID_REPLY).
	 */

    pMph             = (PMPHHDR)(pPacket->pvSendBuffer);
    (PUCHAR)pMph    += pPacket->usSendReservedBytes;/* adjust top of ptr.   */
    pMph->uchId      = ID_MESSAGE;                  /* normal message ID    */
    pMph->uchSp      = (UCHAR)(pUser->usLocalPort); /* source port no.      */
    pMph->uchNo      = (UCHAR)(pUser->infSending.usSeqNo);     /* message seq. no.     */
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
    memcpy(pFunc, puchBuffer, usSize);        /* copy user data           */

    /* --- count up tally --- */
    UTLYTOTALSENT(pUser->infTally);             /* count up tally           */
    STLYTOTALSENT(pPacket->infTally);           /* count up tally           */

    /* --- send a message --- */
    sRet = sendto(pPacket->iSocket, (LPUCHAR)(pMph), usHeader + usSize, 0, (struct sockaddr *)(&sinClient), sizeof(sinClient));
    if (sRet != (INT)(usSize + usHeader)) {
		ULONG  ulLastError = WSAGetLastError();
        UTLYSENTERROR(pUser->infTally);
        STLYSENTERROR(pPacket->infTally);
        STLYLASTSENTERROR(pPacket->infTally, sRet);
		if (ulLastError != 10065) {
			char  xBuff[64];
			sprintf (xBuff, "ExeNetSend(): WSAGetLastError() = %d", ulLastError);
			NHPOS_ASSERT_TEXT(sRet != SOCKET_ERROR, xBuff);
			return ((USHORT)(PIF_ERROR_NET_ERRORS));
		}
		else {
			STLYSENTUNREACHABLEERROR(pPacket->infTally);
			return ((USHORT)(PIF_ERROR_NET_UNREACHABLE));
		}
    }
	NHPOS_ASSERT(sRet <= PIF_LEN_UDATA);

    /* --- is datagram ack required ? ---
		Check to see if there is a time out period specified by the sending
		user thread.  If so, then we are going to wait until the receiving
		user's network layer sends us an acknowledgement before we unblock
		the sending user to indicate that the send is complete.

		In other words, if the sending user has specified a time out then
		the sending user is blocked until both the actual sending and the
		acknowledgement is received from the receiving user before the sending
		user is signaled and the PifNetSend () completes.
	 */
    if (uchTimer) {                             /* datagram ack. required ? */
        pUser->pSendMessage = pPacket->pRequest;
        pUser->usSendTick   = TICK(pUser->usSendTimer, pPacket->usTick);
		if (pUser->usSendTick < 0x7f0)
			pUser->usSendTick += 2;    // increment the wait timer by 2 ticks to account for network latency.
        usRet               = (USHORT)(PIF_ERROR_NET_RE_SCHEDULE);
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
USHORT ExeNetControl(USHORT usNet, PCNTDAT pControl, USHORT usBytes, PPKTINF pPacket)
{
    BOOL        fSystem;
    USHORT      usRet, usMode, usTime, fsFlags;
    PUSRTBL     pUser;
    PRCVPND     pPending;
    UCHAR            szName[32];
    struct hostent * pEntry;

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
        case PIF_NET_DMODE | PIF_NET_TMODE:         /* data only w/ timeout */
            pUser->fsFlags &= ~(PIF_NET_NMODE | PIF_NET_DMODE | PIF_NET_TMODE);
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

	case PIF_NET_RESET_SEQNO:
		(pUser->infSending).usSeqNo = 0;
		if (pUser->infSending.RequestMphHdr.uchNo > 0) {
			(pUser->infSending).usSeqNo = pUser->infSending.RequestMphHdr.uchNo;
			(pUser->infSending).usSeqNo--;
		}
		break;

    case PIF_NET_CLEAR:                         /* clear queued messages ?  */
		{
			while (pPending = (PRCVPND)QueGetBuffer(&(pUser->queReceived))) {
#if 0
				 /* --- respond busy message ---
					We respond with a busy message if the sending user has requested
					an acknowledgement back.

					If the user which sent the message asked for an ack, then we send an ack
					with ST_TARGET_BUSY indicating we have dropped the message.  The usual
					reason for time out is because the receiving thread is busy doing something
					else and has requested a new message.

				 */
				if (pPending->fAck) {               /* datagram ack. required ? */
					ACKDAT      infAck;

					/* --- make ack. data info. --- */
					infAck.ulTargetAddr = pPending->ulSourceAddr;
					infAck.usIpPortNo   = pPending->usIpPortNo;
					infAck.usTargetPort = pPending->usSourcePort;
					infAck.usNumber     = pPending->usSeqNo;
					infAck.usLocalPort  = pUser->usLocalPort;
					infAck.usStatus     = ST_TARGET_CLEAR;

					/* --- send ack. message --- */
					usRet = SendAckMessage(&infAck, NULL, 0, pPacket);
					UTLYTOTALSENT(pUser->infTally);       /* count up tally, STLYTOTALSENT() done in SendAckMessage() */
				}
#endif
				/* --- discard resources --- */
				SysFreeMemory(pPacket->hMemory, pPending);
				UTLYRECVLOST(pUser->infTally);
				UTLYRECVTARGETCLEAR(pUser->infTally);
			}
		}
        break;

    case PIF_NET_GET_NAME:                      /* get local name ?         */
		pControl->ulLastSourceAddr = (! fSystem) ? pUser->ulSourceAddr : 0;
		pControl->usLastIpPort = (! fSystem) ? pUser->usIpPortNo : 2172;
        pControl->usNamePort = (! fSystem) ? pUser->usLocalPort : pPacket->usPortLocal;
		// WARNING: gethostbyname() may fail if a terminal has been turned off for several minutes
		//          or if after being off for some time, the terminal has just been turned on.
		//          there is a chance the host name information in the directory may have gone
		//          stale due to lack of communication with a terminal and been discarded.
        if (gethostname(szName, sizeof(szName))) {
            return ((USHORT)(PIF_ERROR_NET_ERRORS));
        } else if (! (pEntry = gethostbyname(szName))) {
            return ((USHORT)(PIF_ERROR_NET_ERRORS));
        }
        pControl->ulNameAddr = *(LPULONG)(pEntry->h_addr);
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

    case PIF_NET_SET_REMOTE:                    /* set remote address ?     */
        pUser->ulRemoteAddr = pControl->ulSendAddr; /* target address       */
        pUser->usRemotePort = pControl->usSendPort; /* target port no.      */
        break;

    case PIF_NET_SET_EVENT:                     /* set power down event ?   */
        if (pControl->fEventEvent) {                /* required control ?   */
            pUser->fsFlags |=  PIF_NET_EVENT_MODE;      /* set mode         */
            pUser->fsFlags &= ~PIF_NET_POWER_FAIL;      /* reset old event  */
        } else {                                    /* else, not required   */
            pUser->fsFlags &= ~PIF_NET_EVENT_MODE;      /* reset mode       */
            pUser->fsFlags &= ~PIF_NET_POWER_FAIL;      /* reset old event  */
        }
        break;

    case PIF_NET_SET_STANDBY:                   /* set standby state ?      */
        if (! fSystem) {                            /* not system user ?    */
            return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
        }
        break;

    case PIF_NET_DIAG:                          /* diag functions ?         */
//        if (! fSystem) {                            /* not system user ?    */
//           return ((USHORT)PIF_ERROR_NET_BAD_HANDLE);  /* bad handle given */
//        }
        usRet = ExeNetDiag(pPacket,                 /* invoke to diag func. */
                           pControl->pDiagData,         /* ptr. to req. data*/
                           pControl->pDiagBuffer,       /* ptr. to buffer   */
                           pControl->usDiagBytes);      /* buffer size      */
        break;

    case PIF_NET_SET_LOG:                       /* set LOG function addr. ? */
        SysSetErrorLog(pControl->pfnAddress);       /* set the function addr*/
        break;

	case PIF_NET_GET_USER_INFO:
		if (pControl->datFunc.usrInfo.pfnAddr) {
			pControl->datFunc.usrInfo.pfnAddr->usLastSenduchNo = pUser->usrStatusInfo.usLastSenduchNo;
			pControl->datFunc.usrInfo.pfnAddr->usLastRecvuchNo = pUser->usrStatusInfo.usLastRecvuchNo;
			pControl->datFunc.usrInfo.pfnAddr->sLastSendPifError = pUser->usrStatusInfo.sLastSendPifError;
			pControl->datFunc.usrInfo.pfnAddr->sLastRecvPifError = pUser->usrStatusInfo.sLastRecvPifError;
			pControl->datFunc.usrInfo.pfnAddr->iLastSendErrorWsa = pUser->usrStatusInfo.iLastSendErrorWsa;
			pControl->datFunc.usrInfo.pfnAddr->sLastSendErrorPif = pUser->usrStatusInfo.sLastSendErrorPif;
			pControl->datFunc.usrInfo.pfnAddr->iLastRecvErrorWsa = pUser->usrStatusInfo.iLastRecvErrorWsa;
			pControl->datFunc.usrInfo.pfnAddr->sLastRecvErrorPif = pUser->usrStatusInfo.sLastRecvErrorPif;
		}
		break;

#if defined (SASRATOGA)
    case PIF_NET_POWER_FAILURE:
        /* --- once close my socket --- */
        closesocket(pPacket->iSocket);          /* close my socket          */
        break;
#else
        break;
#endif
    default:                                    /* others ...               */
        usRet = (USHORT)(PIF_ERROR_NET_BAD_DATA);
        break;
    }

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
USHORT  ExeNetClose(USHORT usNet, PPKTINF pPacket)
{
    ACKDAT      infAck;
    PUSRTBL     pUser;
    PMSGHDR     pMessage;
    PMGRMSG     pRequest;
    PRCVPND     pPending;

    /* --- compute its user table ptr. --- */
    if (! (pUser = GetUserTable(usNet, pPacket->pUsers, pPacket->usUsers))) {
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));
    }

    /* --- is opened user ? --- */
    if (! (pUser->fsFlags & PIF_NET_USED)) {    /* not opened user ?        */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* bad handle given     */
    }

    /* --- is under requesting PifNetSend() ? --- */
    if (pUser->pSendMessage) {                  /* requesting send() ?      */
        pMessage           = (PMSGHDR)(pUser->pSendMessage);
        pMessage->usSender = pPacket->usManager;
        pRequest           = (PMGRMSG)(pMessage->aucMessage);
        pRequest->usReturn = (USHORT)(PIF_ERROR_NET_ABORTED);
        SysEventSignal(&(pMessage->evtSignal));     /* signalize the user   */
    }

    /* --- is under requesting PifNetReceive() ? --- */
    if (pUser->pRecvMessage) {                  /* requesting receive() ?   */
        pMessage           = (PMSGHDR)(pUser->pRecvMessage);
        pMessage->usSender = pPacket->usManager;
        pRequest           = (PMGRMSG)(pMessage->aucMessage);
        pRequest->usReturn = (USHORT)(PIF_ERROR_NET_ABORTED);
        SysEventSignal(&(pMessage->evtSignal));     /* signalize the user   */
    }

    /* --- discard all of resources --- */
    while (pPending = (PRCVPND)QueGetBuffer(&(pUser->queReceived))) {
        /* --- datagram ack. required ? --- */
        if (pPending->fAck) {                   /* datagram ack. required ? */
            /* --- make ack. data info. --- */
            infAck.ulTargetAddr = pPending->ulSourceAddr;
			infAck.usIpPortNo   = pPending->usIpPortNo;     /* target UDP/IP port number */
            infAck.usTargetPort = pPending->usSourcePort;
            infAck.usNumber     = pPending->usSeqNo;
            infAck.usLocalPort  = pUser->usLocalPort;
            infAck.usStatus     = ST_UNDELIVER;

            /* --- respond undeliverable message --- */
            SendAckMessage(&infAck, pPending->auchMessage, pPending->usBytes, pPacket);
			UTLYTOTALSENT(pUser->infTally);    /* count up tally, STLYTOTALSENT() done in SendAckMessage() */
        }

        /* --- discard resources --- */
        SysFreeMemory(pPacket->hMemory, pPending);
		UTLYRECVLOSTNETCLOSE(pUser->infTally);
		STLYRECVLOSTNETCLOSE(pPacket->infTally);
    }

    /* --- free the table --- */
    pUser->fsFlags = 0;                         /* free the table           */

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
USHORT  ExeNetShutdown(USHORT usNet, PPKTINF pPacket)
{
    /* --- is valid user handle ? --- */
    if (usNet != PIF_NET_HANDLE) {              /* system handle ?          */
        return ((USHORT)(PIF_ERROR_NET_BAD_HANDLE));/* bad handle given     */
    }

    /* --- close my socket --- */
//  shutdown(pPacket->iSocket, 2);
    closesocket(pPacket->iSocket);

    /* --- tell windows socket --- */
//  WSACleanup();

    return (PIF_OK);
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

    /* --- count up tally --- */

    STLYTOTALSENT(pPacket->infTally);           /* count up tally           */

    /* --- send a message --- */
    sRet = sendto(pPacket->iSocket, (LPUCHAR)(pMph), usBytes, 0, (struct sockaddr *)(&sinClient), sizeof(sinClient));
    if (sRet != (INT)usBytes) {   // check that sendto() was successful
		ULONG  ulLastError = WSAGetLastError();
        STLYSENTERROR(pPacket->infTally);
        STLYLASTSENTERROR(pPacket->infTally, sRet);
		if (ulLastError != 10065) {
			char  xBuff[64];
			sprintf (xBuff, "SendAckMessage(): WSAGetLastError() = %d", ulLastError);
			NHPOS_ASSERT_TEXT(sRet != SOCKET_ERROR, xBuff);
			return ((USHORT)(PIF_ERROR_NET_ERRORS));
		}
		else {
			STLYSENTUNREACHABLEERROR(pPacket->infTally);
			return ((USHORT)(PIF_ERROR_NET_UNREACHABLE));
		}
    }
	NHPOS_ASSERT(sRet <= PIF_LEN_UDATA);

    return (0);
}

/**
;========================================================================
;
;   function : Tell power down event
;
;   format : VOID       TellPowerDownEvent(pPacket);
;
;   input  : PPKTINF    pPacket;        - ptr. to packet info.
;
;   output : nothing
;
;========================================================================
**/
VOID    TellPowerDownEvent(PPKTINF pPacket)
{
    USHORT      i;
    PMSGHDR     pMessage;
    PMGRMSG     pRequest;
    PRCVPND     pFree;
    PUSRTBL     pUser;

    /* --- tell power down event to all of users --- */
    for (pUser = pPacket->pUsers, i = 0; i < pPacket->usUsers; i++) {
        /* --- is used table ? --- */
        if (pUser->fsFlags & PIF_NET_USED) {    /* opened user ?            */
            /* --- mark power down event --- */
            pUser->fsFlags |= PIF_NET_POWER_FAIL;   /* mark power down      */

            /* --- discard all of pending buffers --- */
            while (pFree = (PRCVPND)QueGetBuffer(&(pUser->queReceived))) {
                SysFreeMemory(pPacket->hMemory, pFree);
            }

            /* --- is under requesting PifNetSend() ? --- */
            if (pUser->pSendMessage) {          /* under requesting ?       */
                pMessage            = (PMSGHDR)(pUser->pSendMessage);
                pMessage->usSender  = pPacket->usManager;
                pRequest            = (PMGRMSG)(pMessage->aucMessage);
                pRequest->usReturn  = (USHORT)PIF_ERROR_NET_POWER_FAILURE;
                pUser->pSendMessage = NULL;
                SysEventSignal(&(pMessage->evtSignal));
            }

            /* --- is under requesting PifNetReceive() ? --- */
            if (pUser->pRecvMessage) {          /* under requesting ?       */
                pMessage            = (PMSGHDR)(pUser->pRecvMessage);
                pMessage->usSender  = pPacket->usManager;
                pRequest            = (PMGRMSG)(pMessage->aucMessage);
                pRequest->usReturn  = (USHORT)PIF_ERROR_NET_POWER_FAILURE;
                pUser->pRecvMessage = NULL;
                SysEventSignal(&(pMessage->evtSignal));
            }
        }

        /* --- go to next user table --- */
        pUser++;
    }

    return ;
}

/*==========================================================================*\
;+                                                                          +
;+                      D i a g    F u n c t i o n                          +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Execute diag function
;
;   format : USHORT     ExeNetDiag(pPacket, pDiag, pBuffer, usBytes);
;
;   input  : PPKTINF    pPacket;        - ptr. to a packet info.
;            PDGDATA    pDiag;          - ptr. to request data
;            PDGINFO    pBuffer;        - ptr. to response buffer
;            USHORT     usBytes;        - buffer size
;
;   output : nothing
;
;========================================================================
**/
USHORT ExeNetDiag(PPKTINF pPacket, PDGDATA pDiag, PDGINFO pBuffer, USHORT usBytes)
{
    USHORT      usRet, usOutput;

    /* --- pass a control to each function --- */
    switch (pDiag->usFunc) {                    /* what diag function ?     */
    case DIAG_GET_SYSTEM:                       /* get system info. ?       */
        usRet = DiagGetSystem(pPacket, (PREQINF)(pDiag->auchData), (PRSPSYS)(pBuffer->auchData), (USHORT)(usBytes - FAROF(auchData, DGINFO)), &usOutput);
        break;
    case DIAG_GET_USER:                         /* get user info. ?         */
        usRet = DiagGetUser(pPacket, (PREQINF)(pDiag->auchData), (PRSPUSR)(pBuffer->auchData), (USHORT)(usBytes - FAROF(auchData, DGINFO)), &usOutput);
        break;
    case DIAG_SET_VALUE:                        /* set values ?             */
        usRet = DiagSetValue(pPacket, (PREQSET)(pDiag->auchData), pBuffer->auchData, (USHORT)(usBytes - FAROF(auchData, DGINFO)), &usOutput);
        break;
    case DIAG_CHK_MEMORY:                       /* check memroy ?           */
        usRet = DiagCheckMemory(pPacket, pDiag->auchData, (PRSPMEM)(pBuffer->auchData), (USHORT)(usBytes - FAROF(auchData, DGINFO)), &usOutput);
        break;
    default:
        usOutput = 0;
        usRet    = (USHORT)PIF_ERROR_NET_BAD_DATA;
        break;
    }

    /* --- make output packet --- */
    pBuffer->usFunc  = pDiag->usFunc;           /* executed function code   */
    pBuffer->usBytes = usOutput;                /* output data length       */

    return (usRet ? usRet : PIF_OK);
}

/**
;========================================================================
;
;   function : Get system information
;
;   format : VOID   DiagGetSystem(pPacket, pDiag, pBuffer, usSize, pusOutput);
;
;   input  : PPKTINF    pPacket;        - ptr. to a packet info.
;
;   output : nothing
;
;========================================================================
**/
USHORT  DiagGetSystem(PPKTINF  pPacket, PREQINF  pDiag, PRSPSYS  pBuffer, USHORT   usSize, PUSHORT  pusOutput)
{
    BOOL        fReset;
    USHORT      i, usLast, usRet;
    PUSRTBL     pUser;
    PUSRINF     pInfo;

    /* --- initialize --- */
    fReset = pDiag->fResetTally;                /* reset a tally ?          */
    usLast = usSize;                            /* avaialble buffer size    */

    /* --- minimum buffer size given ? --- */
    if (usLast < sizeof(RSPSYS)) {              /* too small ?              */
        return ((USHORT)PIF_ERROR_NET_BUFFER_OVERFLOW);
    }

    /* --- make up system information --- */
    pBuffer->usPortLocal  = pPacket->usPortLocal;
    pBuffer->usPortRemote = pPacket->usPortRemote;
    pBuffer->ulSocket     = (ULONG)(pPacket->iSocket);
    pBuffer->usIdleTime   = pPacket->usIdleTime;
    pBuffer->usMaxQueues  = pPacket->usMaxQueues;
    pBuffer->usMaxSize    = pPacket->usSizeBuffer;
    pBuffer->usMaxUsers   = pPacket->usUsers;

    /* --- respond system tally --- */
#if defined (NET_TALLY)
    memcpy(&(pBuffer->infTally), &(pPacket->infTally), sizeof(SYSTLY));
#else
    memset(&(pBuffer->infTally), '\0',                 sizeof(SYSTLY));
#endif
    /* --- reset tally ? --- */
#if defined (NET_TALLY)
    if (fReset) {                               /* reset tally ?            */
        memset(&(pPacket->infTally), '\0', sizeof(SYSTLY));
    }
#endif
    /* --- initialize before making up user info. --- */
    pInfo   = pBuffer->ainfUsers;               /* top of response buffer   */
    usLast -= FAROF(ainfUsers, RSPSYS);         /* remained buffer size     */
    usRet   = 0;                                /* assume good status       */

    /* --- make up user info. --- */
    for (pUser = pPacket->pUsers, i = 0; i < pPacket->usUsers; pUser++, i++) {
        /* --- if user using ? --- */
        if (!pUser || ! (pUser->fsFlags & PIF_NET_USED)) {    /* any user ?           */
            continue;
        }

        /* --- any buffer available ? --- */
        if (usLast < sizeof(USRINF)) {              /* not enough buffer ?  */
            usRet = (USHORT)PIF_ERROR_NET_BUFFER_OVERFLOW;
            break;
        }

        /* --- make up user info. --- */
        pInfo->usHandle = (pUser - pPacket->pUsers) + 1;
        pInfo->usPortNo = pUser->usLocalPort;
        pInfo++;
    }

    /* --- compute no. of users --- */
    pBuffer->usNoOfUsers = (pInfo - pBuffer->ainfUsers);

    /* --- respond no. of bytes --- */
    *pusOutput = (PUCHAR)pInfo - (PUCHAR)pBuffer;

    return (usRet);
}

/**
;========================================================================
;
;   function : Get user information
;
;   format : VOID       DiagGetUser(pPacket, pDiag, pusBytes);
;
;   input  : PPKTINF    pPacket;        - ptr. to a packet info.
;            PRDGMSG    pDiag;          - ptr. to a message
;            PUSHORT    pusBytes;       - ptr. to a buffer to respond size
;
;   output : nothing
;
;========================================================================
**/
USHORT  DiagGetUser(PPKTINF  pPacket, PREQINF  pDiag, PRSPUSR  pBuffer, USHORT   usSize, PUSHORT  pusOutput)
{
    BOOL        fReset;
    USHORT      i, usPort, usLast, usRet;
    PUSRTBL     pUser;
    PRSPUSR     pInfo;

    /* --- initialize --- */
    usPort = pDiag->usUserPort;                 /* port no. to get info.    */
    fReset = pDiag->fResetTally;                /* reset a tally ?          */
    pInfo  = pBuffer;                           /* top of buffer ptr.       */
    usLast = usSize;                            /* avaialble buffer size    */
    usRet  = (USHORT)PIF_ERROR_NET_BAD_DATA;    /* assume bad data          */

    /* --- make up user info. --- */
    for (pUser = pPacket->pUsers, i = 0; i < pPacket->usUsers; pUser++, i++) {
        /* --- is user using ? --- */
        if (!pUser || ! (pUser->fsFlags & PIF_NET_USED)) {
            continue;
        }

        /* --- is the user ? --- */
        if ((pUser->usLocalPort != usPort) && usPort) {
            continue;
        }

        /* --- minimum buffer size given ? --- */
        if (usLast < sizeof(RSPUSR)) {          /* too small ?              */
            usRet = (USHORT)PIF_ERROR_NET_BUFFER_OVERFLOW;
            break;
        }

        /* --- make up system information --- */
        pInfo->fsUserFlags   = pUser->fsFlags;
        pInfo->ulRemoteAddr  = pUser->ulRemoteAddr;
        pInfo->usRemotePort  = pUser->usRemotePort;
        pInfo->usLocalPort   = pUser->usLocalPort;
        pInfo->usSendTimer   = pUser->usSendTimer;
        pInfo->ulRecvTimer   = pUser->ulRecvTimer;
        pInfo->fsUserState   = pUser->pSendMessage ? STATE_IN_SEND : 0;
        pInfo->fsUserState  |= pUser->pRecvMessage ? STATE_IN_RECV : 0;
        pInfo->usNoOfPending = QueEnumBuffer(&(pUser->queReceived));

        /* --- respond system tally --- */
#if defined (NET_TALLY)
        pInfo->infTally = pUser->infTally;
#else
        memset(&(pInfo->infTally), '\0',               sizeof(USRTLY));
#endif
        /* --- reset tally ? --- */
#if defined (NET_TALLY)
        if (fReset) {                           /* reset tally ?            */
            memset(&(pUser->infTally), '\0', sizeof(USRTLY));
        }
#endif
        /* --- adjust status --- */
        pInfo++;
        usRet = 0;
    }

    /* --- respond no. of bytes --- */
    *pusOutput = (PUCHAR)pInfo - (PUCHAR)pBuffer;

    return (usRet);
}

/**
;========================================================================
;
;   function : Set new parameters
;
;   format : VOID       DiagSetValue(pPacket, pDiag, pusBytes);
;
;   input  : PPKTINF    pPacket;        - ptr. to a packet info.
;            PRDGMSG    pDiag;          - ptr. to a message
;            PUSHORT    pusBytes;       - ptr. to a buffer to respond size
;
;   output : nothing
;
;========================================================================
**/
USHORT  DiagSetValue(PPKTINF  pPacket, PREQSET  pDiag, PVOID pvBuffer, USHORT usSize, PUSHORT  pusOutput)
{
    USHORT      i, usRet, fsSet, usPort, usSendTimer;
    ULONG       ulRecvTimer;
    PUSRTBL     pUser;

    /* --- initialize --- */
    fsSet       = pDiag->fsSetWhat;             /* options for setting      */
    usPort      = pDiag->usUserPort;            /* port no. to change       */
    usSendTimer = pDiag->usSendTimer;           /* send timer value         */
    ulRecvTimer = pDiag->ulRecvTimer;           /* recv timer value         */
    usRet       = (USHORT)PIF_ERROR_NET_BAD_DATA;   /* assume bad data      */

    /* --- get user info. --- */
    for (pUser = pPacket->pUsers, i = 0; i < pPacket->usUsers; pUser++, i++) {
        /* --- is user using ? --- */
        if (!pUser || ! (pUser->fsFlags & PIF_NET_USED)) {
            continue;
        }

        /* --- is the user ? --- */
        if ((pUser->usLocalPort != usPort) && (usPort)) {
            continue;
        }

        /* --- set new values --- */
        pUser->usSendTimer = (fsSet & SET_SEND_TIMER) ? usSendTimer : pUser->usSendTimer;
        pUser->ulRecvTimer = (fsSet & SET_RECV_TIMER) ? ulRecvTimer : pUser->ulRecvTimer;
        pUser->fsFlags    |= (fsSet & SET_RECV_TIMER) ? PIF_NET_TMODE : 0;

        /* --- adjust status --- */
        usRet = 0;
    }

    /* --- respond no. of bytes --- */
    *pusOutput = 0;

    return (usRet);
}

/**
;========================================================================
;
;   function : Check memory heap
;
;   format : VOID       DiagCheckMemory(pPacket, pDiag, pusBytes);
;
;   input  : PPKTINF    pPacket;        - ptr. to a packet info.
;            PRDGMSG    pDiag;          - ptr. to a message
;            PUSHORT    pusBytes;       - ptr. to a buffer to respond size
;
;   output : nothing
;
;========================================================================
**/

USHORT  DiagCheckMemory(PPKTINF  pPacket, PVOID pvDiag, PRSPMEM  pBuffer, USHORT   usSize, PUSHORT  pusOutput)
{
    MEMINF      infMemory;

    /* --- get heap info. --- */
    SysInfoMemory(pPacket->hMemory, &infMemory);    /* get heap info.       */

    /* --- make up packet --- */
    pBuffer->fHeapFine     = SysCheckMemory(pPacket->hMemory);
    pBuffer->ulInitialSize = 0;                     /* unknown initial size */
    pBuffer->ulUsedMemory  = infMemory.ulUsedMemory;/* used memory bytes    */
    pBuffer->ulFreeMemory  = infMemory.ulFreeMemory;/* free memory bytes    */
    pBuffer->ulMaxAlloc    = infMemory.ulMaxAlloc;  /* max. allocatable     */

    /* --- respond no. of bytes --- */
    *pusOutput = sizeof(RSPMEM);

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
        return (NULL);
    }

    /* --- compute this users table ptr. --- */
    pUser = pTable + (usHandle - 1);            /* compute table ptr.       */

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
    PUSRTBL     pLocal = NULL;                             /* assume not found         */

	if (pTable) {
		PUSRTBL     pUser;
		USHORT      i;

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
	}

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
    USHORT  usHandle = 0;                               /* initialize handle        */

	if (pTable) {
		PUSRTBL  pUser;
		USHORT   i;

		/* --- look for vacant user table --- */
		for (pUser = pTable, i = 0; i < usArrays; i++) {            /* look for free table      */
			/* --- could be found ? --- */
			if (! (pUser->fsFlags & PIF_NET_USED)) {/* is vacant cell ?         */
				usHandle = i + 1;                       /* compute handle       */
				break;                                  /* break my job         */
			}

			/* --- how about net table ? --- */
			pUser++;                                /* go to next user table    */
		}
	}

    return (usHandle);
}


/*==========================================================================*\
;+                                                                          +
;+          C o n t i n u o u s   R e c e i v e   T h r e a d               +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Spawn continuous receiver thread
;
;   format : USHORT     SpawnReceiver(usQueue, iSocket, usIdle);
;
;   input  : USHORT     usQueue;        - queue ID
;            INT        iSocket;        - socket handle
;            USHORT     usIdle;         - idle timer value in msec.
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/
USHORT  SpawnReceiver(USHORT usQueue, PPKTINF pinfPacket)
{
    static RCVARG     argReceiver = {0};
    ULONG             idRet;

    /* --- make argument --- */
    argReceiver.usStatus = 0;                       /* init. execution status   */
    argReceiver.usQueue  = usQueue;                 /* queue ID for the thread  */
    argReceiver.iSocket  = pinfPacket->iSocket;     /* socket handle            */
    argReceiver.usIdle   = pinfPacket->usIdleTime;  /* idle timer value in msec */

    /* --- create an event signal --- */
    SysEventCreate(&(argReceiver.evtAck), TRUE);/* create a signal          */

    /* --- create network manager thread --- */
    if ((idRet = SysBeginThread (NetReceiver, 0, 0, PRIO_NET_MGR, &argReceiver)) != -1L) {
        SysEventWait(&(argReceiver.evtAck), WAIT_FOREVER);
    }

    /* --- close the event signal --- */
    SysEventClose(&(argReceiver.evtAck));       /* close event signal       */

    return ((idRet == -1) ? PIF_ERROR_NET_CRITICAL : argReceiver.usStatus);
}

/**
;========================================================================
;
;   function : Continuous receiver thread
;
;   format : VOID       NetReceiver(pArg);
;
;   input  : PRCVARG    pArg;
;
;   output : nothing
;
;========================================================================
**/
VOID THREAD NetReceiver(PRCVARG pArg)
{
    BOOL                fDoMyJob;
    SHORT               sRead;
    USHORT              usRet, usQueue, usIdle, usWhy = 0;
    INT                 iSocket;

    /* --- initialize --- */
    usQueue = pArg->usQueue;                    /* my queue ID              */
    iSocket = pArg->iSocket;                    /* socket handle            */
    usIdle  = pArg->usIdle;                     /* idle timer value in msec */
    usRet   = 0;                                /* assume no error          */

    /* --- create my queue --- */
    usRet = SysCreateQueue(usQueue) ? PIF_ERROR_NET_QUEUE : usRet;
    usRet = LASTERROR(usRet, usRet);            /* evaluate return status   */

    /* --- respond status --- */
    pArg->usStatus = usRet;                     /* respond init. status     */

    /* --- issue a signal --- */
    SysEventSignal(&(pArg->evtAck));            /* tell I'm ready           */

    /* --- do my job --- */
    while (! usRet) {                           /* do my job                */
		PMSGHDR       pHeader;
		PRCVMSG       pMessage;
		USHORT        usRequester;

        /*    wait for a request message
		 *  request messages are generated by ReceiverProtocol() after it processes a
		 *  network read. the lifecycle is a network message is received and it is sent
		 *  to the network manager thread which then processes it with ReceiverProtocol()
		 *  and after processing the message ReceiverProtocol() puts another request message
		 *  into the NetReceiver() thread queue to start the next network read.
		 */
        if (usRet = SysReadQueue(usQueue, &pHeader, QUE_WAIT_FOREVER)) {
            usWhy = EVENT_READ_QUEUE;
            break;
        }

		if (!pHeader) continue;

        /* --- get information of the message --- */
        usRequester = pHeader->usSender;        /* get sender of this msg.  */
        pMessage    = (PRCVMSG)(pHeader->aucMessage);

        /* --- what command ? --- */
        switch (pMessage->usCommand) {          /* what function do I do ?  */
        case CMD_CHANGE_SOCKET:                 /* change socket handle ?   */
            iSocket            = pMessage->iRecvSocket; /* chnage my handle */
            pMessage->usStatus = 0;                     /* that's OK !      */
            fDoMyJob           = FALSE;                 /* not my main job  */
            break;
        case CMD_RECEIVE_DATA:                  /* receive data ?           */
            fDoMyJob           = TRUE;                  /* do my main job   */
            break;
        default:                                /* others ...               */
            pMessage->usStatus = (USHORT)PIF_ERROR_NET_CRITICAL;
            fDoMyJob           = FALSE;                 /* not my main job  */
            break;
        }

        /* --- receiver from a network --- */
        while (fDoMyJob) {                      /* receiver from a network  */
			struct sockaddr_in  sinClient = {0};
			INT                 iClient;

            /* --- initialize buffer --- */
			iClient = sizeof(sinClient);

            /* --- wait for a message --- */
            if ((sRead = recvfrom(iSocket, pMessage->puchRecvBuffer, pMessage->usRecvSize, 0, (struct sockaddr *)&sinClient, &iClient)) > 0) {     /* buffer size      */
//				NHPOS_ASSERT(sRead <= PIF_LEN_UDATA);
                pMessage->usStatus      = 0;        /* that's OK !          */
                pMessage->usRecvBytes   = sRead;    /* no. of bytes received*/
                pMessage->ulRecvAddress = sinClient.sin_addr.s_addr;
                pMessage->usRecvIpPort = ntohs(sinClient.sin_port);
                break;
            }

            /* --- critical error ? --- */
            if (sRead < 0) {
                pMessage->usStatus = (USHORT)WSAGetLastError();
                break;                              /* cannot do my job !   */
            }

            /* --- wait for a while if no data received ... --- */
            SysSleep((ULONG)usIdle);            /* pass a control to others */
        }

        /* --- complete the message --- */
        pHeader->usSender = usQueue;            /* my signature             */

        /* --- dispose the message --- */
        if (usRet = SysWriteQueue(usRequester, &(pHeader->queControl))) {
            usWhy = EVENT_WRITE_QUEUE;
            break;
        }
    }

    /* --- record the event --- */
    SysErrorLog(FAULT_AT_PIFNET, usWhy, 0, 0, 0, NULL);
    SysErrorLog(FAULT_AT_PIFNET, usRet, 0, 0, 0, NULL);

    /* --- well mannered ... --- */
    SysEndThread();
}

/*==========================================================================*\
;+                                                                          +
;+                  S l e e p   T i m e r   T h r e a d                     +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Spawn sleep timer thread
;
;   format : USHORT     SpawnTimer(usQueue);
;
;   input  : USHORT     usQueue;        - queue ID
;
;   output : USHORT     usRet;          - return status
;
;========================================================================
**/
USHORT  SpawnTimer(USHORT usQueue)
{
	static RCVARG    argTimer = {0};
    ULONG            idRet;

    /* --- make argument --- */
    argTimer.usStatus = 0;                      /* init. execution status   */
    argTimer.usQueue  = usQueue;                /* queue ID for timer thread*/

    /* --- create an event signal --- */
    SysEventCreate(&(argTimer.evtAck), TRUE);   /* create a signal          */

    /* --- create network manager thread --- */
    if ((idRet = SysBeginThread (SleepTimer, 0, 0, PRIO_NET_MGR, &argTimer)) != -1L) {
        SysEventWait(&(argTimer.evtAck), WAIT_FOREVER);
    }

    /* --- close the event signal --- */
    SysEventClose(&(argTimer.evtAck));          /* close event signal       */

    return ((idRet == -1) ? PIF_ERROR_NET_CRITICAL : argTimer.usStatus);
}

/**
;========================================================================
;
;   function : Sleep timer thread
;
;   format : VOID       SleepTimer(pArg);
;
;   input  : PRCVARG    pArg;
;
;   output : nothing
;
;========================================================================
**/
VOID THREAD SleepTimer(PRCVARG pArg)
{
    USHORT      usRet, usQueue, usRequester, usWhy = 0;
    PMSGHDR     pHeader;
    PTMRMSG     pTimer;

    /* --- initialize --- */
    usQueue = pArg->usQueue;                    /* my queue ID              */
    usRet   = 0;                                /* assume no error          */

    /* --- create my queue --- */
    usRet = SysCreateQueue(usQueue) ? PIF_ERROR_NET_QUEUE : usRet;
    usRet = LASTERROR(usRet, usRet);            /* evaluate return status   */

    /* --- respond status --- */
    pArg->usStatus = usRet;                     /* respond my status        */

    /* --- issue a signal --- */
    SysEventSignal(&(pArg->evtAck));            /* tell I'm ready           */

    /* --- do my job --- */
    while (! usRet) {                           /* do my job                */
        /* --- wait for a request message --- */
        if (usRet = SysReadQueue(usQueue, &pHeader, QUE_WAIT_FOREVER)) {
            usWhy = EVENT_READ_QUEUE;
            break;
        }

		if (!pHeader) continue;

        /* --- get information of the message --- */
        usRequester = pHeader->usSender;        /* get sender of this msg.  */
        pTimer      = (PTMRMSG)(pHeader->aucMessage);

        /* --- sleep by the specified milli. sec. --- */
        SysSleep((ULONG)(pTimer->usMilliSec));  /* sleep by the time        */

        /* --- complete the message --- */
        pHeader->usSender = usQueue;            /* my signature             */

        /* --- dispose the message --- */
        if (usRet = SysWriteQueue(usRequester, &(pHeader->queControl))) {
            usWhy = EVENT_WRITE_QUEUE;
            break;
        }
    }

    /* --- record the event --- */
    SysErrorLog(FAULT_AT_PIFNET, usWhy, 0, 0, 0, NULL);
    SysErrorLog(FAULT_AT_PIFNET, usRet, 0, 0, 0, NULL);

    /* --- well mannered ... --- */
    SysEndThread();
}


/*==========================================================================*\
;+                      E n d   O f   P r o g r a m                         +
\*==========================================================================*/
