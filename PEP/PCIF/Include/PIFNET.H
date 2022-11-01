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
#if !defined(SARATOGA) && !defined(SARATOGA_PC)
#define     PIF_LEN_IP      sizeof(ULONG)       /* ULONG IP length          */
#define     PIF_LEN_UDATA       1400             /* packet data length  - These defines should match CLI_MAX_REQDATA define*/     */
//#define     PIF_LEN_UDATA       477             /* packet data length       */

#define     ANYPORT               0             /* any port assigned        */
#endif
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

/* --- name buffer structure --- */
#if !defined(SARATOGA) && !defined(SARATOGA_PC)
typedef struct {                                /* XGHEADER structure       */
    UCHAR   auchFaddr[PIF_LEN_IP];                  /* ULONG format IP      */
    USHORT  usFport;                                /* foreign port no.     */
    USHORT  usLport;                                /* local port number    */
} XGHEADER, FAR *PXGHEADER;
#else
typedef XGHEADER FAR *PXGHEADER;
#endif
/* --- my name buffer structure --- */

typedef struct {                                /* XGNAME structure         */
    UCHAR   auchLaddr[PIF_LEN_IP];                  /* ULONG format IP      */
	UCHAR   auchSaddr[PIF_LEN_IP];              /* ULONG format Source IP last receive  */
	USHORT  usIpPortNo;							/* UDP/IP port number last receive */
    USHORT  usLport;                                /* local port number    */
} XGNAME, FAR *PXGNAME;

/* --- send / receive interface --- */

#if !defined(SARATOGA) && !defined(SARATOGA_PC)
typedef struct {                                /* XGRAM structure          */
    UCHAR   auchFaddr[PIF_LEN_IP];                  /* ULONG format IP      */
    USHORT  usFport;                                /* foreign port no.     */
    USHORT  usLport;                                /* local port number    */
    UCHAR   auchData[PIF_LEN_UDATA];                /* data area            */
} XGRAM, FAR *PXGRAM;
#else
typedef XGRAM FAR *PXGRAM;
#endif

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
//Notes: The above sturcture is defined by the compatibility against the old
//      PifNetXxxx() module. However, the following structure using with the
//      macro XGSIZE() is desirable for the smart PifNetXxxx() algorithm.
typedef struct {                                /* XGRAM structure          */
    UCHAR   auchFaddr[PIF_LEN_IP];                  /* ULONG format IP      */
    USHORT  usFport;                                /* foreign port no.     */
    USHORT  usLport;                                /* local port number    */
    UCHAR   auchData[1];                            /* data area            */
} XGRAM, FAR *PXGRAM;
#endif
                                            /* structure member's offset    */
#define     OFFSETMBR(member, type) (USHORT)(&(((type *)NULL)->member))
                                            /* compute proper size of buffer*/
#define     XGSIZE(x)               (USHORT)((x) + OFFSETMBR(auchData, XGRAM))

/* --- general address --- */

typedef struct {                                /* general address          */
    UCHAR   auchAddr[PIF_LEN_IP];                   /* ULONG format IP      */
    USHORT  usPort;                                 /* port no.             */
} XGADDR, FAR *PXGADDR;

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
/* --- timer values --- */

#define     PIF_NO_WAIT   ((USHORT)( 0))        /* no wait option           */
#define     PIF_FOREVER    ((SHORT)(-1))        /* wait forever             */

/* --- activity mode definitions --- */

#define     PIF_NET_SET_MODE         1          /* set function mode        */
#define     PIF_NET_CLEAR            2          /* clear received messages  */
#define     PIF_NET_SET_TIME         3          /* set receive timer value  */
#define     PIF_NET_SET_STIME        5          /* set send timer value     */
#define     PIF_NET_GET_NAME        20          /* get local name           */
#define     PIF_NET_SET_REMOTE      21          /* set remote address       */
#define     PIF_NET_SET_EVENT       22          /* set power down event     */
#define     PIF_NET_SET_STANDBY     23          /* set standby state        */
#define     PIF_NET_SET_TIME_EX     24          /* set receive timer value  */
#define     PIF_NET_DIAG            25          /* execute diag. function   */
#define     PIF_NET_SET_LOG         26          /* set LOG function address */
#define     PIF_NET_POWER_FAILURE   27          /* set power down           */

#define     PIF_NET_NMODE             0x01      /* normal mode              */
#define     PIF_NET_DMODE             0x02      /* data only mode           */
#define     PIF_NET_TMODE             0x20      /* timeout options          */
#define		PIF_NET_SRVRMODE		  0x40  // Setup for server mode as opposed to normal client mode
#define     PIF_NET_EVENT_MODE      0x2000      /* power failure event mode */
#define     PIF_NET_POWER_FAIL      0x4000      /* power down     (internal)*/
#define     PIF_NET_USED            0x8000      /* user used cell (internal)*/

/* --- error codes --- */

#if !defined(SARATOGA) && !defined(SARATOGA_PC)
#define PIF_OK                             1    /* completed                */
#define PIF_ERROR_NET_POWER_FAILURE      (-2)   /* power failure            */
#define PIF_ERROR_NET_TIMEOUT            (-3)   /* network timeout          */
#define PIF_ERROR_NET_NOT_PROVIDED       (-4)   /* not supported            */
#define PIF_ERROR_NET_ALREADY_EXIST      (-5)   /* already exists           */
#define PIF_ERROR_NET_EOF                (-6)   /* end of file ?            */
#define PIF_ERROR_NET_DISK_FULL          (-7)   /* disk full ?              */
#define PIF_ERROR_NET_NOT_FOUND          (-8)   /* network not found ?      */
#define PIF_ERROR_NET_ABORTED            (-9)   /* function aborted         */
#define PIF_ERROR_NET_ACCESS_DENIED     (-10)   /* access denied ?          */
#define PIF_ERROR_NET_READ              (-11)   /* reading failure ?        */
#define PIF_ERROR_NET_WRITE             (-12)   /* writing failure ?        */
#define PIF_ERROR_NET_OFFLINE           (-13)   /* offline                  */
#define PIF_ERROR_NET_UNREACHABLE       (-14)   /* unreachable              */
#define PIF_ERROR_NET_MESSAGE           (-15)   /* message corrupt          */
#define PIF_ERROR_NET_BUSY              (-16)   /* target busy              */
#define PIF_ERROR_NET_ERRORS            (-20)   /* network errors           */
#endif
#define PIF_ERROR_NET_BUFFER_OVERFLOW   (-21)   /* buffer short             */
#define PIF_ERROR_NET_TOO_LONG          (-22)   /* too long data            */

/* --- the following errors are internal use or critical errors --- */

#define PIF_ERROR_NET_RE_SCHEDULE       (-30)   /* re-scheduled   (internal)*/
#define PIF_ERROR_NET_MANY_USERS        (-35)   /* too many users (critical)*/
#define PIF_ERROR_NET_BAD_HANDLE        (-36)   /* bad handle     (critical)*/
#define PIF_ERROR_NET_BAD_DATA          (-37)   /* bad data       (critical)*/
#define PIF_ERROR_NET_CRITICAL          (-38)   /* critical error (critical)*/
#define PIF_ERROR_NET_QUEUE             (-39)   /* queue error    (critical)*/
#define PIF_ERROR_NET_MEMORY            (-40)   /* memory error   (critical)*/
#define PIF_ERROR_NET_NOT_WORK          (-41)   /* not initialized(critical)*/
/* --- data structure control --- */

/* for PifNetXXXX() module (5/15/96) */
#if (defined(SARATOGA) || defined(SARATOGA_PC))
#define MODULE_PIF_NETWORK          166
#define FAULT_AT_PIFNET             MODULE_PIF_NETWORK
/* added the above         (5/15/96) */
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

typedef struct _SYSTLY {                    /* system tally                 */
    ULONG   ulTotalSent;                        /* no. of sent totally      */
    ULONG   ulSentError;                        /* no. of sent errors       */
    USHORT  usLastSentError;                    /* last sent error code     */
    ULONG   ulTotalRecv;                        /* no. of recv totally      */
    ULONG   ulRecvError;                        /* no. of recv errors       */
    USHORT  usLastRecvError;                    /* last sent error code     */
    ULONG   ulOutOfResources;                   /* no. of out of resources  */
    ULONG   ulIllegalMessage;                   /* no. of illegal messages  */
    ULONG   ulRecvUndeliver;                    /* no. of recv undeliverable*/
} SYSTLY, *PSYSTLY;

typedef struct _USRTLY {                    /* user tally                   */
    USHORT  usTotalSent;                        /* no. of sent totally      */
    USHORT  usSentError;                        /* no. of sent errors       */
    USHORT  usTotalRecv;                        /* no. of recv totally      */
    USHORT  usRecvError;                        /* no. of recv errors       */
    USHORT  usRecvLost;                         /* no. of recv lost         */
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
#if defined(SARATOGA)
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);
# define    _h                          PIF_NET_HANDLE
# define    PifNetSetStandby()          NetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)         NetControl(_h, PIF_NET_DIAG, x, y, z)
#elif defined(SARATOGA_PC)
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpenEx(PXGHEADER);
//SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);
# define    _h                          PIF_NET_HANDLE
# define    PifNetSetStandby()          NetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)         NetControl(_h, PIF_NET_DIAG, x, y, z)
#elif defined (POS7450)
SHORT NETENTRY PifNetInitialize(PNETINF);
SHORT NETENTRY PifNetOpen(PXGHEADER);
SHORT NETENTRY PifNetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY PifNetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY PifNetSend(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY PifNetControl(USHORT, USHORT, ...);
SHORT NETENTRY PifNetClose(USHORT);
SHORT NETENTRY PifNetFinalize(VOID);
# define    _h                      PIF_NET_HANDLE
# define    PifNetSetStandby()      PifNetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)     PifNetControl(_h, PIF_NET_DIAG, x, y, z)
#elif defined (POS2170)
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);
# define    PifNetInitialize(a)         NetInitialize(a)
# define    PifNetOpen(a)               NetOpen(a)
# define    PifNetReceive(a, b, c)      NetReceive(a, b, c)
# define    PifNetReceive2(a, b, c, d)  NetReceive2(a, b, c, d)
# define    PifNetSend(a, b, c)         NetSend(a, b, c)
# define    PifNetControl               NetControl
# define    PifNetClose(a)              NetClose(a)
# define    PifNetFinalize()            NetFinalize()
# define    _h                          PIF_NET_HANDLE
# define    PifNetSetStandby()          NetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)         NetControl(_h, PIF_NET_DIAG, x, y, z)
SHORT NETENTRY PifNetControl(USHORT, USHORT, ...);
#elif defined (SERVER_NT)
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetReceive2(USHORT, VOID FAR *, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, VOID FAR *, USHORT);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);
# define    _h                          PIF_NET_HANDLE
# define    PifNetSetStandby()          NetControl(_h, PIF_NET_SET_STANDBY)
# define    PifNetDiag(x, y, z)         NetControl(_h, PIF_NET_DIAG, x, y, z)
#endif

/* --- data conversion routines --- */

#if !defined(SARATOGA) && !defined(SARATOGA_PC)

#define htons(x)        ExchangeUshort(x)       /* host to network USHORT   */
#define ntohs(x)        ExchangeUshort(x)       /* network to host USHORT   */
#define htonl(x)        ExchangeUlong(x)        /* host to network ULONG    */
#define ntohl(x)        ExchangeUlong(x)        /* network to host ULONG    */

#endif

USHORT  ExchangeUshort(USHORT);                 /* exchange USHORT data     */
ULONG   ExchangeUlong(ULONG);                   /* exchange ULONG data      */


#ifdef __cplusplus
}
#endif

/*==========================================================================*\
;+                      E n d    O f    H e a d e r                         +
\*==========================================================================*/
