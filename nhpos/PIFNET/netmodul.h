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

/* --- examine given macro --- */
#if !defined (NET_MEMORY) && !defined (NET_QUEUE) && !defined (NET_SYSTEM)
#define  NET_MODULE
#define  NET_MEMORY
#define  NET_QUEUE
#define  NET_SYSTEM
#endif

/* --- some useful language definitions --- */
#define     FOREVER         1               /* forever loop                 */
                                            /* no. of arrays                */
#define     ARRAYS(object)          (sizeof(object) / sizeof(object[0]))
                                            /* structure member's offset    */
#define     FAROF(member, type)     (USHORT)(&(((type *)NULL)->member))     
                                            /* structure member's size      */
#define     LONGOF(member, type)    sizeof(((type *)NULL)->member)
                                            /* which is max. data ?         */
#define     MAXOF(x, y)         (((x) > (y)) ? (x) : (y))
                                            /* which is min. data ?         */
#define     MINOF(x, y)         (((x) < (y)) ? (x) : (y))
                                            /* change endianness            */
#define     EXCHANGE(x)         (((USHORT)(x) >> 8) + ((USHORT)(x) << 8))
#if !defined LOUSHORT                       /* get lower USHORT from ULONG  */
# define    LOUSHORT(l)         ((USHORT)(ULONG)(l))
#endif
#if !defined HIUSHORT                       /* get higher USHORT from ULONG */
# define    HIUSHORT(l)         ((USHORT)(((ULONG)(l) >> 16) & 0xFFFF))
#endif
#if !defined MAKEULONG                      /* make ULONG data from 2 USHORT*/
# define    MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#endif

/* --- tally & debugging control --- */
# define    NET_TALLY                           /* enable/disable tally     */

/* --- event log --- */
#define EVENT_READ_QUEUE        1           /* unexpected SysReadQueue()    */
#define EVENT_WRITE_QUEUE       2           /* unexpected SysWriteQueue()   */
#define EVENT_SOCKET_RECV       3           /* unexpected recv() error      */

/* -------------------------------- *\
 +   D e f a u l t   V a l u e s    +
\* -------------------------------- */
#ifdef  NET_MODULE

/* --- user port no. control --- */
#define     DEF_PEND_TICK        50         /* def time to live (100ms unit)*/
#define     PORT_LOCAL_MAX      200         /* max. port value              */
#define     PORT_AUTO_ASSIGN    201         /* automatic assign port no.    */

/* --- send timer values --- */
#define     DEF_SEND_TIME        5000       /* default send time value      */
#define     MIN_SEND_TIME        1000       /* min. send timer value        */
#define     MAX_SEND_TIME       12700       /* max. send timer value        */
#define     ADJ_SEND_TIME         300       /* adjust send timer in sending */

/* --- default port number --- */
#define     PORT_ECHO_SERVER    7           /* echo server's port number    */

/* --- threads' priority --- */
# define    PRIO_NET_MGR    THREAD_PRIORITY_HIGHEST
# define    PRIO_NET_ECHO   THREAD_PRIORITY_HIGHEST

#endif

/* -------------------------------- *\
 +          S Y S T E M             +
\* -------------------------------- */

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

#ifdef  NET_SYSTEM

/* --- thread handling --- */
# define    THREAD
 typedef VOID (THREAD *PFNTHREAD)(PVOID);
/* --- semaphore / event signal handlings --- */

  typedef struct _SEMOBJ {                      /* semaphore object         */
//    USHORT  usSemaphore;                            /* handle of semaphore  */
	HANDLE  hSemaphoreHandle;   // actual Windows OS Semaphore handle using Windows API
  } SEMOBJ, *PSEMOBJ;

  typedef struct _EVTOBJ {                      /* event signal object      */
    HANDLE  hEvent;                                 /* handle of event      */
  } EVTOBJ, *PEVTOBJ;

# define    WAIT_FOREVER    INFINITE            /* wait forever             */

#endif

/* -------------------------------- *\
 +   M e m o r y   M a n a g e r    +
\* -------------------------------- */

#ifdef  NET_MEMORY

/* --- misc. definition --- */
#define     KBYTES              1024        /* no. of bytes in Kbyte        */
/* --- definition of memory handle --- */
 typedef    HANDLE              HMEM;           /* memory handle            */

/* --- memory info. I/F --- */
typedef struct _MEMINF {                    /* memory info.                 */
    ULONG   ulUsedMemory;                       /* total bytes of used mem  */
    ULONG   ulFreeMemory;                       /* total bytes of free mem  */
    ULONG   ulMaxAlloc;                         /* max. bytes allocatable   */
} MEMINF, *PMEMINF;

#endif  // NET_MEMORY

/* -------------------------------- *\
 +     Q u e u e   M a n a g e r    +
\* -------------------------------- */

#ifdef  NET_QUEUE

/* --- system setting definition --- */
#define     NO_OF_QUEUES        5           /* no. of queues                */

/* --- queue & buffer --- */
typedef struct _QUEBUF {                    /* queue buffer                 */
    struct _QUEBUF FAR * pChain;                /* next record chain        */
} QUEBUF, *PQUEBUF;

typedef struct _QUEPTR {                    /* queue                        */
    PQUEBUF     pFirst;                         /* first chaind record      */
    PQUEBUF     pLast;                          /* last chained record      */
    USHORT      usCount;                        /* no. of records           */
} QUEPTR, *PQUEPTR;

/* --- queue read options --- */
#define QUE_WAIT_FOREVER    WAIT_FOREVER    /* wait forever                 */

/* --- error codes --- */
#define ERROR_SYS_INVALID_DATA      0x1093  /* invalid data given           */
#define ERROR_SYS_NO_RESOURCE       0x1095  /* no resource available        */
#define ERROR_SYS_DUP_RESOURCE      0x1096  /* duplicated resource          */
#define ERROR_SYS_TIMEOUT           0x1097  /* time out                     */
#define ERROR_SYS_QUE_EMPTY         0x1098  /* queue is empty               */

#endif

/* -------------------------------- *\
 +  M o d u l e  I n t e r f a c e  +
\* -------------------------------- */

#ifdef  NET_MODULE

/* --- module interface message format --- */
typedef struct _MSGHDR {                        /* I/F message header       */
    QUEBUF  queControl;                             /* queue control area   */
    USHORT  usSender;                               /* sender queue ID      */
    EVTOBJ  evtSignal;                              /* signal for done      */
    UCHAR   aucMessage[1];                          /* top of message       */
} MSGHDR, *PMSGHDR;

/* --- function command code & optional data --- */
#define     CMD_OPEN            1               /* PifNetOpen()             */
#define     CMD_RECEIVE         2               /* PifNetReceive()          */
#define     CMD_SEND            3               /* PifNetSend()             */
#define     CMD_CONTROL         4               /* PifNetControl()          */
#define     CMD_CLOSE           5               /* PifNetClose()            */
#define     CMD_SHUTDOWN        6               /* PifFinalize()            */

typedef struct _OPNDAT {                        /* PifNetOpen()             */
    ULONG   ulFaddr;                                /* target address       */
    USHORT  usFport;                                /* target port no.      */
    USHORT  usLport;                                /* local port no.       */
} OPNDAT, *POPNDAT;

typedef struct _RCVDAT {                        /* PifNetReceive()          */
    USHORT  usBytes;                                /* receive buffer size  */
    LPVOID  pvBuffer;                               /* ptr. to a buffer     */
    PXGADDR pSender;                                /* ptr. to a sender addr*/
} RCVDAT, *PRCVDAT;

typedef struct _SNDDAT {                        /* PifNetSend()             */
    USHORT  usBytes;                                /* receive buffer size  */
    LPVOID  pvBuffer;                               /* ptr. to a buffer     */
} SNDDAT, *PSNDDAT;

typedef struct _CNTDAT {                        /* PifNetControl()          */
    USHORT  usFunction;                             /* sub function code    */
    union {                                         /* optional parameters  */
        struct { USHORT  usMode; } datMode;             /* PIF_NET_SET_MODE */
        struct { ULONG   ulTime; } datTime;             /* PIF_NET_SET_TIME */
        struct { ULONG   ulAddr;
                 ULONG   ulSourceAddr;                  /* source IP address */
                 USHORT  usIpPortNo;
                 USHORT  usPort; } datName;             /* PIF_NET_GET_NAME */
        struct { USHORT  usTime; } datStime;            /* PIF_NET_SET_STIME*/
        struct { ULONG   ulAddr;
                 USHORT  usPort; } datSend;             /* PIF_NET_SET_REMOT*/
        struct { BOOL    fEvent; } datEvent;            /* PIF_NET_SET_EVENT*/
        struct { PDGDATA pRequest;
                 PDGINFO pBuffer;
                 USHORT  usBytes; } datDiag;            /* PIF_NET_DIAG     */
        struct { PFUNLOG pfnAddr; } datLog;             /* PIF_NET_SET_LOG  */
        struct { PPIFUSRTBLINFO pfnAddr; } usrInfo;      /* PIF_NET_GET_USER_INFO  */
    } datFunc;
} CNTDAT, *PCNTDAT;

#define usModeMode  datFunc.datMode.usMode      /* mode of SET_MODE         */
#define ulTimeTime  datFunc.datTime.ulTime      /* time of SET_TIME         */
#define ulNameAddr  datFunc.datName.ulAddr      /* address of GET_NAME      */
#define usNamePort  datFunc.datName.usPort      /* port of GET_NAME         */
#define usLastIpPort  datFunc.datName.usIpPortNo  /* UDP/IP port of GET_NAME         */
#define ulLastSourceAddr  datFunc.datName.ulSourceAddr /* source IP address */
#define usStimeTime datFunc.datStime.usTime     /* time of SET_STIME        */
#define ulSendAddr  datFunc.datSend.ulAddr      /* address of SET_REMOTE    */
#define usSendPort  datFunc.datSend.usPort      /* port of SET_REMOTE       */
#define fEventEvent datFunc.datEvent.fEvent     /* event of SET_EVENT       */
#define pDiagData   datFunc.datDiag.pRequest    /* data ptr. of DIAG        */
#define pDiagBuffer datFunc.datDiag.pBuffer     /* buffer ptr. of DIAG      */
#define usDiagBytes datFunc.datDiag.usBytes     /* buffer size of DIAG      */
#define pfnAddress  datFunc.datLog.pfnAddr      /* function ptr. of SET_LOG */

/* --- main network manager interface --- */
typedef struct _MGRMSG {                        /* manager I/F message      */
    USHORT  usHandle;                               /* network handle       */
    USHORT  usCommand;                              /* function command code*/
    USHORT  usReturn;                               /* execution status     */
    USHORT  usLength;                               /* optional data length */
    union {                                         /* optional data area   */
        OPNDAT  datOpen;                                /* for CMD_OPEN     */
        RCVDAT  datReceive;                             /* for CMD_RECEIVE  */
        SNDDAT  datSend;                                /* for CMD_SEND     */
        CNTDAT  datControl;                             /* for CMD_CONTROL  */
    } datOption;
} MGRMSG, *PMGRMSG;


/* --- continuous receiver interface --- */
typedef struct _RCVMSG {                        /* receiver I/F message     */
    USHORT  usCommand;                              /* command code         */
    USHORT  usStatus;                               /* execution status     */
    union {                                         /* optional data        */
        INT     iSocket;                                /* new socket handle*/
        struct {                                        /* receive data info*/
            LPUCHAR puchBuffer;                             /* ptr. to a buf*/
            USHORT  usSize;                                 /* buffer size  */
            USHORT  usBytes;                                /* no. of bytes */
            ULONG   ulAddress;                              /* IP address   */
            USHORT  usIpPortNo;                     /* UDP/IP port number   */
        } datRecv;
    } datRecvOption;
} RCVMSG, *PRCVMSG;

#define CMD_CHANGE_SOCKET       1               /* command for change socket*/
#define CMD_RECEIVE_DATA        2               /* receive data             */
#define iRecvSocket         datRecvOption.iSocket
#define puchRecvBuffer      datRecvOption.datRecv.puchBuffer
#define usRecvSize          datRecvOption.datRecv.usSize
#define usRecvBytes         datRecvOption.datRecv.usBytes
#define ulRecvAddress       datRecvOption.datRecv.ulAddress
#define usRecvIpPort        datRecvOption.datRecv.usIpPortNo

/* --- tick timer interface --- */
typedef struct _TMRMSG {                        /* tick timer I/F message   */
    USHORT  usMilliSec;                             /* timer value in msec  */
} TMRMSG, *PTMRMSG;

#endif

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */


#ifdef  NET_MODULE

/* --- internal APIs --- */
SHORT NETENTRY NetInitialize(PNETINF);
SHORT NETENTRY NetOpen(PXGHEADER);
SHORT NETENTRY NetReceive(USHORT, LPVOID, USHORT);
SHORT NETENTRY NetReceive2(USHORT, LPVOID, USHORT, PXGADDR);
SHORT NETENTRY NetSend(USHORT, LPVOID, ULONG);
SHORT NETENTRY NetControl(USHORT, USHORT, ...);
SHORT NETENTRY NetClose(USHORT);
SHORT NETENTRY NetFinalize(VOID);

/* --- network manager --- */
USHORT  SpawnNetManager(USHORT, PNETINF);       /* spawn network manager    */

/* --- echo server --- */
USHORT  SpawnEchoServer(USHORT);                /* spawn echo server        */
#endif

/* -------------------------------- *\
 +          S Y S T E M             +
\* -------------------------------- */

#ifdef  NET_SYSTEM

/* --- thread handling --- */
ULONG   SysBeginThread(PFNTHREAD, LPVOID, USHORT, ULONG, LPVOID);
VOID    SysEndThread(VOID);
VOID    SysSleep(ULONG);

/* --- semaphore handling --- */
BOOL    SysSemCreate(PSEMOBJ, BOOL);            /* create a semaphore       */
BOOL    SysSemClear(PSEMOBJ);                   /* clear a semaphore        */
BOOL    SysSemRequest(PSEMOBJ, ULONG);          /* request a semaphore      */
BOOL    SysSemClose(PSEMOBJ);                   /* close a semaphore        */

/* --- event signal handling --- */
BOOL    SysEventCreate(PEVTOBJ, BOOL);          /* create an signal event   */
BOOL    SysEventBlock(PEVTOBJ);                 /* set signal blocking state*/
BOOL    SysEventWait(PEVTOBJ, ULONG);           /* wait a signal event      */
BOOL    SysEventSignal(PEVTOBJ);                /* signalize a event signal */
BOOL    SysEventClose(PEVTOBJ);                 /* close an event signal    */

/* --- error log handling --- */
VOID    SysSetErrorLog(PFUNLOG);                /* set error log function   */
VOID    SysErrorLog(USHORT, USHORT, USHORT, BOOL, SHORT, PSZ);
#endif

/* -------------------------------- *\
 +   M e m o r y   M a n a g e r    +
\* -------------------------------- */

#ifdef  NET_MEMORY

/* --- memory manager --- */
VOID    SysInitializeMemory(VOID);              /* initialize memory manager*/
HMEM    SysCreateMemory(ULONG ulBytes);         /* create memory segment    */
VOID    SysDeleteMemory(HMEM);                  /* delete memory segment    */
LPVOID  SysAllocMemory(HMEM, USHORT);           /* allocate memory block    */
VOID    SysFreeMemory(HMEM, LPVOID);            /* free memory block        */
BOOL    SysCheckMemory(HMEM);                   /* check memory             */
BOOL    SysInfoMemory(HMEM, PMEMINF);           /* get memory info.         */

#endif

/* -------------------------------- *\
 +     Q u e u e   M a n a g e r    +
\* -------------------------------- */

#ifdef  NET_QUEUE

/* --- queue manager --- */
USHORT  SysInitializeQueue(VOID);               /* initialize queue manager */
USHORT  SysDestroyQueue(VOID);                  /* destroy queue manager    */
USHORT  SysCreateQueue(USHORT);                 /* create a queue           */
USHORT  SysReadQueue(USHORT, LPVOID *, ULONG);  /* read from a queue        */
USHORT  SysWriteQueue(USHORT, LPVOID);          /* write to a queue         */
USHORT  SysCloseQueue(USHORT);                  /* close a queue            */

/* --- queue handling primitives --- */
VOID    QueInitialize(PQUEPTR);                 /* initialize a queue       */
VOID    QueListBuffer(PQUEPTR, PQUEBUF);        /* list a buffer            */
PQUEBUF QueGetBuffer(PQUEPTR);                  /* get a buffer             */
PQUEBUF QuePeekBuffer(PQUEPTR);                 /* peek a buffer            */
USHORT  QueEnumBuffer(PQUEPTR);                 /* get no. of buffers       */

#endif

#ifdef _DEBUG
VOID	debugOut(int lineNumber, DWORD dwEvent);
VOID  debugDump (UCHAR *filename, int lineNumber, void *p, int nBytes);
#endif
/*==========================================================================*\
;+                      E n d    O f    H e a d e r                         +
\*==========================================================================*/
