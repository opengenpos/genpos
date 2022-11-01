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

/* --- data types modifier --- */

#ifndef     FAR
# define    FAR         _far                /* FAR modifier                 */
#endif

#ifndef     HSEM
 typedef    VOID FAR *  HSEM;               /* semaphore handle             */
 typedef    HSEM FAR *  PHSEM;              /* ptr. to semaphore handle     */
#endif

#if defined (POS2170)
 typedef    int         INT;                /* integer data type            */
#endif

/* --- far data pointing modifier --- */

#if !defined (SERVER_NT) && !defined(SARATOGA) && !defined(SARATOGA_PC)
 typedef    unsigned short far * LPBOOL;    /*                              */
 typedef    void           far * LPVOID;    /*                              */
 typedef    unsigned char  far * LPBYTE;    /*                              */
 typedef             int   far * LPINT;     /*                              */
 typedef             long  far * LPLONG;    /*                              */
#endif
typedef              char  far * LPCHAR;    /*                              */
typedef     unsigned char  far * LPUCHAR;   /*                              */
typedef     unsigned char  far * LPSZ;      /*                              */
typedef              short far * LPSHORT;   /*                              */
typedef     unsigned short far * LPUSHORT;  /*                              */
typedef     unsigned int   far * LPUINT;    /*                              */
typedef     unsigned long  far * LPULONG;   /*                              */
#define     _NULL       ((void far *)0)     /* far NULL pointer             */

/* --- constants values --- */

#ifndef     TRUE
# define    TRUE        1                   /* TRUE for boolean data        */
#endif

#ifndef     FALSE
# define    FALSE       0                   /* FALSE for boolean data       */
#endif

#ifndef     NULL
# define    NULL        ((void *)0)         /* NULL pointer                 */
#endif

/* --- OS/2 compatibility data value --- */

#if defined (POS7450) && !defined (NO_DOS_API)

#ifndef     SEM_INDEFINITE_WAIT
# define    SEM_INDEFINITE_WAIT     (-1L)   /* infinite wait for semaphore  */
#endif

/* DosGetPrty/DosSetPrty usScope */
#define PRTYS_PROCESS       0
#define PRTYS_PROCESSTREE   1
#define PRTYS_THREAD        2

/* DosGetPrty/DosSetPrty priority classes */
#define PRTYC_NOCHANGE      0
#define PRTYC_IDLETIME      1
#define PRTYC_REGULAR       2
#define PRTYC_TIMECRITICAL  3
#define PRTYC_FOREGROUNDSERVER  4

/* DosSetPrty priority deltas (sChange) */
#define PRTYD_MINIMUM       (-31)
#define PRTYD_MAXIMUM       31

/* PID & TID */

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef USHORT    PID;      /* pid  */
typedef PID FAR *PPID;

typedef USHORT    TID;      /* tid  */
typedef TID FAR *PTID;

typedef struct _PIDINFO {   /* pidi */
    PID pid;
    TID tid;
    PID pidParent;
} PIDINFO;
typedef PIDINFO FAR *PPIDINFO;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

/* --- OS/2 compatible functions --- */

USHORT APIENTRY DosSleep(ULONG ulTime);
USHORT APIENTRY DosSemClear(HSEM hsem);
USHORT APIENTRY DosSemSet(HSEM hsem);
USHORT APIENTRY DosSemWait(HSEM hsem, LONG lTimeOut);
USHORT APIENTRY DosSemSetWait(HSEM hsem, LONG lTimeOut);
USHORT APIENTRY DosSemRequest(HSEM hsem, LONG lTimeOut);
USHORT APIENTRY DosSetPrty(USHORT usScope, USHORT fPrtyClass, SHORT sChange,
               USHORT id);
USHORT APIENTRY DosGetPID(PPIDINFO ppidInfo);

#endif  // (POS7450)

/*==========================================================================*\
;+                      E n d    O f    H e a d e r                         +
\*==========================================================================*/
