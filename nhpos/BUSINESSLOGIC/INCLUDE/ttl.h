/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Header File
* Category    : TOTAL, NCR 2170 US Hospitality Application
* Program Name: TTL.H
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
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Data   : Ver.Rev. :   Name     : Description
* May-06-92 : 00.00.01 :            :
* May-17-94 : 02.05.00 : Yoshiko.Jim: add TtlcmpWaiNo()
* Mar- 3-95 : 03.00.00 : hkato      : R3.0
* Nov-22-95 : 03.01.00 : T.Nakahata : R3.1 Initial
*   Add Service Time Total (Daily/PTD) and Individual Terminal Financial (Daily)
*   Increase Regular Discount ( 2 => 6 )
*   Add Net Total at Register Financial Total
*   Add Some Discount and Void Elements at Server Total
* Mar-21-96 : 03.01.01 :T.Nakahata  : Cashier Total Offset USHORT to ULONG
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
*
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Oct-05-99 :            M.Teraki   : Add #pragma pack(...)
*
* GenPOS Rel 2.2
* Apr-19-15 : 02.02.01 : R.Chambers  : replace TTL_MAX_BLOCK with STD_MAX_HOURBLK in ecr.h
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

#if !defined(TTL_H_INCLUDED)
#define TTL_H_INCLUDED

#include "csttl.h"

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

/* Base File Size */
#define     TTL_FIN_SIZE    sizeof(TTLCSREGFIN) /* Financial Total Size */
#define     TTL_HOUR_SIZE   sizeof(TTLCSHOURLY) /* Hourly Total Size */
#define     TTL_BASE_SIZE   ((TTL_FIN_SIZE * 4) + (TTL_HOUR_SIZE * 4))

/* 2172 */
#define     TTL_DEPTHEAD_SIZE    sizeof(TTLCSDEPTHEAD)    /* Dept Header Size */
#define     TTL_DEPTMISC_SIZE    sizeof(TTLCSDEPTMISC)    /* Dept Misc Total Size*/
#define     TTL_DEPTEMP_SIZE     (FLEX_DEPT_MAX / 8) + ((FLEX_DEPT_MAX % 8) ? 1 : 0)
                                                        /* Dept Enpty Table Size */
#define     TTL_DEPTBLOCKRECORD_SIZE     sizeof(USHORT)  /* Dept Block One Record Size */
#define     TTL_DEPTTOTALRECORD_SIZE     sizeof(TOTAL)   /* Dept Total One Record Size */


#ifdef  __DEL_2172  /* 08.30.15, RJC */
// These defines were used for the PLU totals files back before NHPOS 1.4 when flat files were being
// used to store PLU totals possible during the time of the NCR 2170 register.
// With NHPOS 1.3 and 1.4 a change was to use the built into database engine of Windows CE 2.12
// and then with NHPOS 2.0 leading to GenPOS Rel 2.1 and 2.2 we are using SQL Server Express.
//
// #define     TTL_PLUHEAD_SIZE    sizeof(TTLCSPLUHEAD)    /* PLU Header Size */
// #define     TTL_PLUMISC_SIZE    sizeof(TTLCSPLUMISC)    /* PLU Misc Total Size*/
// #define     TTL_PLUEMP_SIZE     (TTL_MAX_PLU / 8) + ((TTL_MAX_PLU % 8) ? 1 : 0)
                                                        /* PLU Enpty Table Size */
// #define     TTL_PLUBLOCKRECORD_SIZE     sizeof(USHORT)  /* PLU Block One Record Size */
// #define     TTL_PLUTOTALRECORD_SIZE     sizeof(TOTAL)   /* PLU Total One Record Size */
#endif  // __DEL_2172

/*----- R3.0 -----*/ 
#define     TTL_CPNHEAD_SIZE   sizeof(TTLCSCPNHEAD)     /* Coupon Header Size */
#define     TTL_CPNMISC_SIZE   sizeof(TTLCSCPNMISC)     /* Coupon Misc Total Size */

#define     TTL_CASHEAD_SIZE    sizeof(TTLCSCASHEAD)    /* Cashier Header Size */
#define     TTL_CASEMP_SIZE     (FLEX_CAS_MAX / 8) + ((FLEX_CAS_MAX % 8) ? 1 : 0)
                                                        /* Cashier Empty Table Size */
#define     TTL_CASTTL_SIZE     sizeof(TTLCSCASTOTAL)   /* Cashier Total Size */

#define     TTL_UPHEAD_SIZE     sizeof(TTLCSUPHEAD)     /* Update Header Size */
#define     TTL_UPDATE_WORK_SIZE  (4098)                        /* Update Work Buffer Size  Add R3.0 */
#define     TTL_UPDATE_FILESIZE   (TTL_UPDATE_WORK_SIZE * 50)   /* TOTALUPD file size, 04/24/01 */

/* ===== New Totals - Individual Financial File (Release 3.1) BEGIN ===== */
#define     TTL_INDHEAD_SIZE    sizeof(TTLCSINDHEAD)    /* Individual Financial Header, R3.1 */
#define     TTL_INDEMP_SIZE     (TTL_MAX_INDFIN / 8) + ((TTL_MAX_INDFIN % 8) ? 1 : 0)
                                                        /* Cashier Empty Table Size */
#define     TTL_INDTTL_SIZE     sizeof(TTLCSREGFIN)     /* Individual Financial, R3.1 */
/* ===== New Totals - Individual Financial File (Release 3.1) END ===== */

/* ===== New Totals - Service Time File (Release 3.1) BEGIN ===== */
#define     TTL_SERTTL_SIZE     sizeof(TTLCSSERTIME)    /* Service Time Total Size */
/* ===== New Totals - Service Time File (Release 3.1) END ===== */


/* File Offset */
/* Financial File */
#define     TTL_FIN_DAYCURR     0   /* Daily Current Finanicial Total Offset */                   
#define     TTL_FIN_PTDCURR     (TTL_FIN_SIZE * 2)      
                                    /* PTD Current Financial Total Offset */
#define     TTL_FIN_FMDATE      1   /* From Date Offset of Financial Total File */
#define     TTL_FIN_TODATE      9   /* To Date Offset of Financial Total File */

/* Hourly File */
#define     TTL_HOUR_DAYCURR    (TTL_FIN_SIZE * 4)
                                    /* Daily Current Hourly Total Offset */
#define     TTL_HOUR_PTDCURR    (TTL_FIN_SIZE * 4) + (TTL_HOUR_SIZE * 2)
                                    /* PTD Current Hourly Total Offset */
#define     TTL_HOUR_FMDATE     1   /* From Date Offset of Hourly Total File */
#define     TTL_HOUR_TODATE     9   /* To Date Offset of Hourly Total File */

/* Dept */
#define     TTL_DEPT_FMDATE     1   /* From Date Offset of Dept Total File */
#define     TTL_DEPT_TODATE     9   /* From Date Offset of Dept Total File */

#define     TTL_DEPT_TMPBUF_INDEXREAD   TTL_TMPBUF_SIZE / sizeof(TTLCSDEPTINDX)
                                    /* Enable Number of Dept Index in Temporary Buffer */
#define     TTL_DEPT_TMPBUF_BLOCKREAD   TTL_TMPBUF_SIZE / sizeof(USHORT)
                                    /* Enable Number of Dept Block in Temporary Buffer */

// These defines were used for the PLU totals files back before NHPOS 1.4 when flat files were being
// used to store PLU totals possible during the time of the NCR 2170 register.
// With NHPOS 1.3 and 1.4 a change was to use the built into database engine of Windows CE 2.12
// and then with NHPOS 2.0 leading to GenPOS Rel 2.1 and 2.2 we are using SQL Server Express.
//
/* PLU */
// #define     TTL_PLU_FMDATE      1   /* From Date Offset of PLU Total File */
// #define     TTL_PLU_TODATE      9   /* From Date Offset of PLU Total File */

#if defined(DCURRENCY_LONGLONG)
// With DCURRENCY defined as a long long or 64 bit variable we need extra space for
// totals calculation.
#define     TTL_TMPBUF_SIZE     3000/*2000*//* Temporary Buffer Size,   Saratoga */
#else
#define     TTL_TMPBUF_SIZE     2000/*1524*//* Temporary Buffer Size,   Saratoga */
#endif

/* #define     TTL_TMPBUFCPN_SIZE  640 / Temporary Buffer Size, R3.0 */
// #define     TTL_PLU_TMPBUF_INDEXREAD   TTL_TMPBUF_SIZE / sizeof(TTLCSPLUINDX)
                                    /* Enable Number of PLU Index in Temporary Buffer */
// #define     TTL_PLU_TMPBUF_BLOCKREAD   TTL_TMPBUF_SIZE / sizeof(USHORT)
                                    /* Enable Number of PLU Block in Temporary Buffer */
#define     TTL_CAS_TMPBUF_INDEXREAD   TTL_TMPBUF_SIZE / sizeof(TTLCSCASINDX)
                                    /* Enable Number of Index in Temporary Buffer */
/*----- Coupon, R3.0 -----*/
#define     TTL_CPN_FMDATE      1   /* From Date Offset of Coupon Total File */
#define     TTL_CPN_TODATE      9   /* From Date Offset of Coupon Total File */

/* ===== New Totals (Release 3.1) BEGIN ===== */
#define     TTL_INDFIN_DAYCURR  0   /* Daily Current Individual Financial */
#define     TTL_INDFIN_FMDATE   1   /* From Date Offset of Individual Financial */
#define     TTL_INDFIN_TODATE   9   /* To Date Offset of Individual Financial */

#define     TTL_SERTIME_DAYCURR 0   /* Daily Current Service Time Total Offset */
#define     TTL_SERTIME_PTDCURR (TTL_SERTTL_SIZE * 2)
                                    /* PTD Current Service Time Total Offset */
#define     TTL_SERTIME_FMDATE  1   /* From Date Offset of Service Time */
#define     TTL_SERTIME_TODATE  9   /* To Date Offset of Service Time */
/* ===== New Totals (Release 3.1) END ===== */

#define     TTL_EMPBIT_SET      0   /* Set Empty Table */
#define     TTL_EMPBIT_RESET    1   /* Reset Empty Table */

/* --- EOD/PTD Execute MDC Define --- */
#define     TTL_BIT_FIN_EXE     0x0001  /* Financial Report */
#define     TTL_BIT_DEPT_EXE    0x0002  /* Dept Report */
#define     TTL_BIT_PLU_EXE     0x0004  /* PLU Report */
#define     TTL_BIT_HOUR_EXE    0x0008  /* Hourly Report */
#define     TTL_BIT_CPN_EXE     0x0080  /* Coupon Report, R3.0 */
/* ===== New Totals (Release 3.1) BEGIN ===== */
#define     TTL_BIT_INDFIN_EXE  0x0100  /* Individual Financial Total,  R3.1 */
#define     TTL_BIT_SERTIME_EXE 0x0200  /* Service Time Total,          R3.1 */
/* ===== New Totals (Release 3.1) END ===== */


/* --- Total Flag Area Define ---
	used with TtlUphdata.uchTtlFlag0 to indicate status
**/
#define     TTL_UPBUFF_FULL     0x01    /* Update Buffer Full Flag */
#define     TTL_UPBUFF_RUNNING  0x02    /* Total Update thread is running */
#define		TTL_UPDBBUFF_NEARFULL	0x10 /* update buffer for DB file is nearing full*/

#define     TTL_SEARCH_EQU      0       /* Search method is Equ */
#define     TTL_SEARCH_GT       1       /* Search method is GT */
#define     TTL_MASK_LOW        0x0f    /* Mask Low Digits */
#define     TTL_BONUS_MAX       STD_NO_BONUS_TTL

#define     TTL_TUM_UNLOCK      0       /* Update Total File Write UNLOCK */
#define     TTL_TUM_LOCK        1       /* Update Total File Write LOCK */

/* --- Total file backup --- */
#define     TTL_BAK_BUFFER  1250     /* backup data max size */

#define     TTL_BAK_BASE    1       /* Base Total File */
#define     TTL_BAK_DEPT    2       /* Department Total File */
#define     TTL_BAK_PLU     3       /* PLU Total File */
#define     TTL_BAK_CASHIER 4       /* Cashier Total File */
#define     TTL_BAK_WAITER  5       /* Waiter Total File */
#define     TTL_BAK_CPN     6       /* Coupon Total File, R3.0 */
/* ===== New Totals (Release 3.1) BEGIN ===== */
#define     TTL_BAK_SERTIME 7       /* Service Time Total,   R3.1 */
#define     TTL_BAK_INDFIN  8       /* Individual Financial, R3.1 */
/* ===== New Totals (Release 3.1) END ===== */

#define     TTL_SETFLAG     0       /* Issued Method to Set */
#define     TTL_RESETFLAG   1       /* Issued Method to Reset */
#define     TTL_CHECKFLAG   2       /* Issued Method to Check */


/*                 
*===========================================================================
*   Type Declarations 
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


// union used in various places with sizeof() to check temporary buffer size
// needed to process totals.  this union provides a max buffer size needed
// for totals processing.
typedef union {
    TTLREPORT     Rpt;
    TTLREGFIN     RegFin;
    TTLHOURLY     Hourly;
    TTLDEPT       Dept;
    TTLPLU        Plu;
    TTLCPN        Cpn;
    TTLCASHIER    Cashier;
	TTLSERTIME    SerTime;
} TTLCSREPORTHEAD;

#ifdef  __DEL_2172  /* 10.25.16, RJC */
typedef struct {
    USHORT  usMaxNo;            /* Save Max Number of PLU */
    USHORT  usPLUNo;            /* Save Number of PLU */
    ULONG   ulPositionMsc;      /* Save Seek Position */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionBlk;      /* Save Seek Position */
    ULONG   ulPositionEmp;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
} TTLCSFLPLUHEAD;
#endif

/* 2172 */
typedef struct {
    USHORT  usMaxNo;            /* Save Max Number of Dept */
    USHORT  usDeptNo;            /* Save Number of Dept */
    ULONG   ulPositionMsc;      /* Save Seek Position */
    ULONG   ulPositionInd;      /* Save Seek Position */
    ULONG   ulPositionBlk;      /* Save Seek Position */
    ULONG   ulPositionEmp;      /* Save Seek Position */
    ULONG   ulPositionTtl;      /* Save Seek Position */
} TTLCSFLDEPTHEAD;

/*********************************************
*   Total Backup Request Data Structure      *
*********************************************/
typedef struct {
    UCHAR       uchFileType;        /* what file sould be backuped */
    ULONG       ulOffSet;           /* how many bytes should be seeked */
    USHORT      usCount;            /* request counter */

    SHORT       sFileSubType;       /* ### ADD 2172 Rel1.0 (01/25/00) */
} TTLBACKUP_REQ;

/*********************************************
*   Total Backup Response Data Structure     *
*********************************************/
typedef struct {
    UCHAR       uchFileType;        /* what file sould be backuped */
    ULONG       ulOffSet;           /* how many bytes should be seeked */
    USHORT      usCount;            /* request counter */
    UCHAR       uchEOF;             /* 0: continue, 1: EOF */
    SHORT       sError;             /* data error detected */

    SHORT       sFileSubType;       /* ### ADD 2172 Rel1.0 (01/25/00) */
} TTLBACKUP_RESFIX;  

typedef struct {
    USHORT      usDataSize;                 /* send data length */
    UCHAR       auchData[TTL_BAK_BUFFER];   /* send data */
} TTLBACKUP_RESDAT;    

typedef struct {
    TTLBACKUP_RESFIX    F;          /* fixed information */
    TTLBACKUP_RESDAT    D;          /* data area */
} TTLBACKUP_RES;

typedef union {
	TTLCSREGFIN     RegFinTtlCs;
	TTLCSHOURLY     HourlyTtlCs;
	TTLCSDEPTHEAD   DeptTtlCs;
	TTLCSDEPTMISC   DeptMiscTtlCs;
	TTLCSCPNHEAD    CpnTtlCs;
	TTLCSCPNMISC    CpnMiscTtlCs;
	TTLCSCASTOTAL   CashierTtlCs;                      // see TtlGetCasOffset().
	TTLCSCASHEAD    CashierHeadTtlCs;
	TTLCSCASINDX    CashierIndxTtlCs[FLEX_CAS_MAX];    // see TtlCasISearch();
	TTLCSSERTIME    SerTimeTtlCs;
	UCHAR           uchCasEmp[TTL_CASEMP_SIZE];        // uchTmpBuff[] needed for some 
	UCHAR           uchBuff[TTL_TMPBUF_SIZE];          // uchTmpBuff[] old style declaration we are replacing.
} TTLCS_TMPBUF_WORKING[1];


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*                             
*===========================================================================
*   Extern Declarations 
*===========================================================================
*/
/* --- Total File Handle --- */
extern  SHORT   hsTtlBaseHandle;    /* Base Total File Handle */
extern  SHORT   hsTtlDeptHandle;    /* Department Total File Handle */
extern  SHORT   hsTtlPLUHandle;     /* PLU Total File Handle */
extern  SHORT   hsTtlCpnHandle;     /* Coupon Total File Handle,  R3.0 */
extern  SHORT   hsTtlCasHandle;     /* Cashier Total File Handle */
extern  SHORT   hsTtlWaiHandle;     /* Waiter Total File Handle */
extern  SHORT   hsTtlUpHandle;      /* Total Update File Handle */
extern  SHORT	hsTtlUpHandleDB;	/* Total Delayed Balance Update File Handle EOD JHHJ*/
/* ===== New Totals (Release 3.1) BEGIN ===== */
extern  SHORT   hsTtlIndFinHandle;  /* Individual Financial Handle, R3.1 */
extern  SHORT   hsTtlSerTimeHandle; /* Service Time Total Handle, R3.1 */
/* ===== New Totals (Release 3.1) END ===== */
extern  SHORT   hsTtlSavFileHandle;

/* --- Semaphore Handle --- */
extern  USHORT  usTtlSemRW;         /* Semaphore Handle for Total File Read & Write */
extern  USHORT  usTtlSemUpdate;     /* Semaphore Handle for Update(TUM) */
extern  USHORT  usTtlQueueJoinUpdate;       /* Queue Handle for Update Totals with Join from Disconnected Satellite, TtlTUMJoinDisconnectedSatellite */
extern  USHORT  usTtlQueueUpdateTotalsMsg;  /* Queue Handle for Update Totals, TtlTUMUpdateTotalsMsg */
extern  USHORT  usTtlSemWrite;      /* Semaphore Handle for Update File Read&Write */

extern  USHORT  usTtlSemDBBackup;   /* Semaphore Handle for Total database File backup */

/* --- Update Lock Handle --- */
extern  CHAR    chTtlLock;          /* Update Lock Handle */
extern  CHAR    chTtlUpdateLock ;   /* Update Total File Write Lock */
                                    
/* --- File Name --- */
extern  TCHAR FARCONST     szTtlBaseFileName[];/* Base Total File Name */
extern  TCHAR FARCONST     szTtlDeptFileName[];/* Dept Total File Name */
extern  TCHAR FARCONST     szTtlPLUFileName[]; /* PLU Total File Name */
extern  TCHAR FARCONST     szTtlCpnFileName[]; /* Coupon Total File Name, R3.0 */
extern  TCHAR FARCONST     szTtlCasFileName[]; /* Cashier Total File Name */
extern  TCHAR FARCONST     szTtlWaiFileName[]; /* Waiter Total File Name */
extern  TCHAR FARCONST     szTtlUpFileName[];  /* Update Total File Name */
extern  CHAR FARCONST     szTtlTUMName[];     /* Total Update Module */
extern  TCHAR FARCONST	   szTtlUpDBFileName[]; /* Delayed Balance Update File EOD JHHJ */
/* ===== New Totals (Release 3.1) BEGIN ===== */
extern  TCHAR FARCONST     szTtlIndFileName[]; /* Individual Financial, R3.1 */
extern  TCHAR FARCONST     szTtlSerTimeName[]; /* Service Time Total,   R3.1 */
/* ===== New Totals (Release 3.1) END ===== */

/* extern  TTLCSUPHEAD UNINT    TtlUphdata;  */     /* Update Total File Header */

extern	UCHAR uchTtlUpdDelayBalanceFlag;
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

/*
*--------------------------------------
*       Totalfl.c
*--------------------------------------
*/

/* --- shutdown/suspend total update module --- */
VOID TtlShutDown(VOID);

/* --- resume from suspend total update module --- */
VOID TtlResume(VOID);

/*
*--------------------------------------
*       Ttlflsub.c
*--------------------------------------
*/

N_DATE *TtlCopyNdateFromDateTime (N_DATE *pNdate, DATE_TIME *pDateTime);

/* --- Financial Total File Read --- */
SHORT TtlTreadFin(TTLREGFIN *pTotal);

/* --- Hourly Total File Read --- */
SHORT TtlTreadHour(TTLHOURLY *pTotal);
SHORT TtlTwriteHour(TTLHOURLY *pTotal);

/* --- Hourly Total File Incremental Read --- */
SHORT TtlTreadIncrHour(TTLHOURLY *pTotal, VOID *pTmpBuff);

/* --- Dept Total File Read --- */
SHORT TtlTreadDept(TTLDEPT *pTotal, VOID *pTmpBuff);

/* --- PLU Total File Read --- */
/*SHORT TtlTreadPLU(TTLPLU *pTotal, VOID *pTmpBuff);*/

/* --- Coupon Total File Read, R3.0 --- */
SHORT TtlTreadCpn(TTLCPN *pTotal);

/* --- Cashier Total File Read --- */
SHORT TtlTreadCas(TTLCASHIER *pTotal);          

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Service Time File Read --- */
SHORT TtlTreadSerTime(TTLSERTIME *pTotal);

/* --- Individual Financial File Read --- */
SHORT TtlTreadIndFin(TTLREGFIN *pTotal);
/* ===== New Functions (Release 3.1) END ===== */

/* --- EOD Total File Reset --- */
SHORT TtlTresetEODPTD(USHORT usMDCBit, UCHAR uchClass, TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Financial Total File Reset --- */
SHORT TtlTresetFin(UCHAR uchClass, TTLCSREGFIN *pTmpBuff);

/* --- Hourly Total File Reset --- */
SHORT TtlTresetHour(UCHAR uchClass, TTLCSHOURLY *pTmpBuff);

/* --- Dept Total File Reset --- */
SHORT TtlTresetDept(UCHAR uchClass, VOID *pTmpBuff);

/* --- PLU Total File Reset --- */
/*SHORT TtlTresetPLU(UCHAR uchClass, VOID *pTmpBuff);*/

/* --- Coupon Total File Reset, R3.0 --- */
SHORT TtlTresetCpn(UCHAR uchClass, VOID *pTmpBuff);

/* --- Cashier Total File Reset --- */
SHORT TtlTresetCas(TTLCASHIER *pTotal, VOID *pTmpBuff);

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Service Time File Reset --- */
SHORT TtlTresetSerTime(UCHAR uchClass, VOID *pTmpBuff);

/* --- Individual Financial File Reset --- */
SHORT TtlTresetIndFin(USHORT usMDCBit, TTLREGFIN *pTotal, VOID *pTmpBuff);
/* --- Individual Financial File Delete --- */
SHORT TtlTDeleteIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff);
/* ===== New Functions (Release 3.1) END ===== */

/* --- Cashier Total File Delete --- */
SHORT TtlTDeleteCas(TTLCASHIER *pTotal, VOID *pTmpBuff);

SHORT TtlTDeletePLU(UCHAR uchClass, TTLPLU *pTmpBuff);

/*
*--------------------------------------
*       Ttlfliss.c
*--------------------------------------
*/

/* --- Set Reset Report Enable Status of Financial Total File --- */
SHORT TtlTIssuedFin(TTLREGFIN *pTotal, VOID *pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* --- Set Reset Report Enable Status of Hourly Total File --- */
SHORT TtlTIssuedHour(TTLHOURLY *pTotal, VOID *pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* --- Set Reset Report Enable Status of Dept Total File --- */
SHORT TtlTIssuedDept(TTLDEPT *pTotal, VOID *pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* --- Set Reset Report Enable Status of PLU Total File --- */
/*SHORT TtlTIssuedPLU(TTLPLU *pTotal, VOID *pTmpBuff, UCHAR uchStat,
                    UCHAR uchMethod);*/

/* --- Set Reset Report Enable Status of Coupon Total File, R3.0 --- */
SHORT TtlTIssuedCpn(TTLCPN *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* --- Set Reset Report Enable Status of Cashier Total File --- */
SHORT TtlTIssuedCas(TTLCASHIER *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Set Reset Report Enable Status of Service Time Total File, R3.1 --- */
SHORT TtlTIssuedSerTime(TTLSERTIME *pTotal, TTLCSSERTIME *pTmpBuff, UCHAR uchStat, UCHAR uchMethod);

/* --- Set Reset Report Enable Status of Individual Financial Total File, R3.1 --- */
SHORT TtlTIssuedIndFin(TTLREGFIN *pTotal, TTLCS_TMPBUF_WORKING pTmpBuff, UCHAR uchStat, UCHAR uchMethod);
/* ===== New Functions (Release 3.1) END ===== */

/* --- Check Total of Financial Total File --- */
SHORT TtlTTotalCheckFin(TTLREGFIN *pTotal, TTLCSREGFIN *pTmpBuff);

/* --- Check Total of Hourly Total File --- */
SHORT TtlTTotalCheckHour(TTLHOURLY *pTotal, TTLCSHOURLY *pTmpBuff);

/* --- Check Total of Dept Total File --- */
SHORT TtlTTotalCheckDept(TTLDEPT *pTotal, VOID *pTmpBuff);

/* --- Check Total of PLU Total File --- */
/*SHORT TtlTTotalCheckPLU(TTLPLU *pTotal, VOID *pTmpBuff);*/

/* --- Check Total of Coupon Total File, R3.0 --- */
SHORT TtlTTotalCheckCpn(TTLCPN *pTotal, VOID *pTmpBuff);

/* --- Check Total of Cashier Total File --- */
SHORT TtlTTotalCheckCas(TTLCASHIER *pTotal, VOID *pTmpBuff);

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Check Total of Service Time Total File, R3.1 --- */
SHORT TtlTTotalCheckSerTime(TTLSERTIME *pTotal, VOID *pTmpBuff);

/* --- Check Total of Individual Financial Total File, R3.1 --- */
SHORT TtlTTotalCheckIndFin(TTLREGFIN *pTotal, VOID *pTmpBuff);
/* ===== New Functions (Release 3.1) END ===== */

BOOL TtlTotalCheckPLUNo(TCHAR *puchPluNo);      /* Saratoga */

/*
*--------------------------------------
*       Ttlflini.c
*--------------------------------------
*/

/* --- Init Base Total File --- */
SHORT TtlBaseInit(TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Init Dept Total File --- */
SHORT TtlDeptInit(TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Init PLU Total File --- */
/*SHORT TtlPLUInit(VOID *pTmpBuff);*/

/* --- Init Coupon Total File, R3.0 --- */
SHORT TtlCpnInit(TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Init Cashier Total File --- */
SHORT TtlCasInit(TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Init Update Total File --- */
SHORT TtlUpdateInit(TTLCS_TMPBUF_WORKING pTmpBuff, ULONG ulNoofItem);  /* 2172 */
/* SHORT TtlUpdateInit(VOID *pTmpBuff, USHORT usNoofItem); */

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Initialize Individual Financail File --- */
SHORT TtlIndFinInit(TTLCS_TMPBUF_WORKING pTmpBuff);

/* --- Initialize Service Time Total File --- */
SHORT TtlSerTimeInit(TTLCS_TMPBUF_WORKING pTmpBuff);
/* ===== New Functions (Release 3.1) END ===== */
/*
*--------------------------------------
*           Ttlsubrd.c
*--------------------------------------
*/

/* --- File Read --- */
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlReadFile () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlReadFile()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message(" \z")
#pragma message("  ====++++====   ----------------------------------    ====++++====")
#pragma message("  ====++++====   TtlReadFile_Debug() is ENABLED    ====++++====")
#pragma message("  ====++++====   File: Include\\ttl.h                 ====++++====")
SHORT TtlReadFile_Debug(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize, char *aszFileName, int LineNo);
#define TtlReadFile(han,ulp,pbuf,uls) TtlReadFile_Debug(han,ulp,pbuf,uls,__FILE__,__LINE__)
#else
SHORT TtlReadFile(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize);
#endif

/* --- Financial Total File Read --- */
SHORT TtlreadFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff);

/* --- Hourly Total File Read --- */
SHORT TtlreadHour(UCHAR uchMinorClass, UCHAR uchOffset, HOURLY *pTmpBuff);

/* --- Dept Total File Read --- */
SHORT TtlDeptHeadRead(TTLCSDEPTHEAD *Depthd);
SHORT TtlDeptMiscRead(ULONG ulPosition, TTLCSDEPTMISC *DeptMisc);
SHORT TtlDeptISearch(ULONG ulPosition, USHORT usDeptNo, TTLCSDEPTINDX *pluind,
                    VOID *pTmpBuff, ULONG *ulOffset, UCHAR uchMethod);
SHORT TtlDeptBlockRead(ULONG ulPosition, ULONG usOffset, USHORT *usDeptBlk);
SHORT TtlDeptTotalRead(ULONG ulPosition, USHORT usDeptBlk, TOTAL *DeptTtl);
SHORT TtlDeptEmptyRead(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlDeptAssin(TTLDEPT *pTotal, TTLCSFLDEPTHEAD *DeptSave, ULONG ulOffset, 
                  VOID *pTmpBuff);
SHORT TtlreadDept(TTLDEPT *pTotal, VOID *pTmpBuff, ULONG *pulPositionMsc,
                 ULONG *pulPositionTtl, UCHAR uchMethod);
#if 0
SHORT TtlDeptTotalRead(ULONG ulPosition, USHORT usDeptNum,
                        VOID *pTmpBuff, USHORT usSize);
SHORT TtlreadDept(TTLDEPT *pTotal, VOID *pTmpBuff, USHORT *pusPosition);
#endif

#ifdef  __DEL_2172  /* 08.30.15, RJC */
/* --- PLU Total File Read --- */
SHORT TtlPLUHeadRead(TTLCSPLUHEAD *PLUhd);
SHORT TtlPLUMiscRead(ULONG ulPosition, TTLCSPLUMISC *PLUMisc);
SHORT TtlPLUISearch(ULONG ulPosition, USHORT usPLUNo, TTLCSPLUINDX *pluind,
                    VOID *pTmpBuff, ULONG *ulOffset, UCHAR uchMethod);
SHORT TtlPLUBlockRead(ULONG ulPosition, ULONG ulOffset, USHORT *usPLUBlk);
SHORT TtlPLUTotalRead(ULONG ulPosition, USHORT usPLUBlk, TOTAL *PLULttl);
SHORT TtlPLUEmptyRead(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlPLUAssin(TTLPLU *pTotal, TTLCSFLPLUHEAD *PLUSave, ULONG ulOffset, 
                  VOID *pTmpBuff);
SHORT TtlreadPLU(TTLPLU *pTotal, VOID *pTmpBuff, ULONG *pulPositionMsc,
                 ULONG *pulPositionTtl, UCHAR uchMethod);
#endif  /* __DEL_2172 08.30.15, RJC */

/* --- Coupon Total File Read, R3.0 --- */
SHORT TtlCpnHeadRead(TTLCSCPNHEAD *Cpnhd);
SHORT TtlCpnMiscRead(ULONG ulPosition, TTLCSCPNMISC *CpnMisc);
SHORT TtlCpnTotalRead(ULONG ulPosition, USHORT usCpnNum,
                        VOID *pTmpBuff, USHORT usSize);
SHORT TtlreadCpn(TTLCPN *pTotal);

/* --- Cashier Total File Read --- */
SHORT TtlCasHeadRead(TTLCSCASHEAD *Cashd);
SHORT TtlCasIndexRead(ULONG ulPosition, USHORT usCasNum, VOID *pTmpBuff);
SHORT TtlCasEmptyRead(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlCasEmptyAssin(USHORT usMaxNo, USHORT usNumberOfCashiers, ULONG ulCashierNumber,
                        ULONG ulPositionInd, ULONG ulPositionEmp,
                        ULONG *pulPositionTtl, VOID *pTmpBuff);
SHORT TtlCasAssin(UCHAR uchMinorClass, ULONG ulCashierNumber,
                  ULONG *pulPositionTtl, VOID *pTmpBuff);
SHORT TtlCasTotalRead(ULONG ulPosition, TTLCSCASTOTAL *CasTotal);
SHORT TtlGetCasOffset(UCHAR uchMinorClass, ULONG ulCashierNumber,   
                      ULONG *pulPositionTtl, VOID *pTmpBuff);
SHORT TtlreadCas(UCHAR uchMinorClass, ULONG ulCashierNumber, VOID *pTmpBuff);

SHORT TtlCasISearch(ULONG ulPosition, USHORT usNumberOfCashiers, TTLCSCASINDX *pluind,
                    VOID *pTmpBuff, ULONG *ulOffset, UCHAR uchMethod);
SHORT TtlCasIAssin(ULONG ulPosition, USHORT usCasNo, TTLCSCASINDX *pCasInd, 
                   VOID *pTmpBuff, ULONG ulOffset);
SHORT TtlCasIErase(ULONG ulPosition, USHORT usCasNo, VOID *pTmpBuff, ULONG ulOffset);

/* --- Read Update Header --- */
SHORT TtlUpHeadRead(TTLCSUPHEAD *pUphd);


// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlLog () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   TtlLog_Debug() is ENABLED     ====++++==== \z")
#define TtlLog(usModule, sErrCode, usNumber) TtlLog_Debug(usModule, sErrCode, usNumber, __FILE__, __LINE__)
VOID  TtlLog_Debug(USHORT usModule, SHORT sErrCode, USHORT usNumber, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
VOID   TtlLog(USHORT usModule, SHORT sErrCode, USHORT usNumber);
#endif

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlAbort () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 1
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   TtlAbort_Debug() is ENABLED: File: Include\\ttl.h     ====++++====")
#define TtlAbort(usModule, sErrCode) TtlAbort_Debug(usModule, sErrCode, __FILE__, __LINE__)
VOID   TtlAbort_Debug(USHORT usModule, SHORT sErrCode, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
VOID   TtlAbort(USHORT usModule, SHORT ErrCode);
#endif

SHORT TtlGetFinFilePosition(UCHAR uchClass, ULONG *pulPosition);
SHORT TtlGetHourFilePosition(UCHAR uchClass, ULONG *pulPosition);
SHORT TtlGetDeptFilePosition(UCHAR uchClass, ULONG *pulPosition, TTLCSDEPTHEAD *pDepthd);
/* --- R3.0 --- */
SHORT TtlGetCpnFilePosition(UCHAR uchClass, ULONG *pulPosition, TTLCSCPNHEAD *pCpnhd);

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Individual Financial File Read, R3.1 --- */
SHORT TtlIndFinHeadRead(TTLCSINDHEAD *pIndHead);
SHORT TtlIndFinTotalRead(ULONG ulPosition, TTLCSREGFIN *pTotal);
SHORT TtlIndFinIndexRead(ULONG ulPosition, USHORT usTermNum, VOID *pTmpBuff);
SHORT TtlIndFinISearch(VOID *pTmpBuff, USHORT usTermNo, USHORT usNumber,  UCHAR *puchOffset, UCHAR *puchNumber);

SHORT TtlGetIndFinOffset(UCHAR uchMinorClass, USHORT usTerminalNumber, ULONG *pulPositionTtl, TTLCS_TMPBUF_WORKING pTmpBuff);
SHORT TtlreadIndFin(UCHAR uchMinorClass, USHORT usTerminalNumber, VOID *pTmpBuff);
SHORT TtlIndFinAssin(UCHAR uchMinorClass, USHORT usTerminalNumber, ULONG *pulPositionTtl, TTLCS_TMPBUF_WORKING pTmpBuff);
SHORT TtlIndFinEmptyAssin(USHORT usMaxNo, USHORT usTermNo, USHORT usTerminalNumber,
                          USHORT usPositionInd, USHORT usPositionEmp,
                          ULONG *pulPositionTtl, VOID *pTmpBuff);
SHORT TtlIndFinEmptyRead(ULONG ulPosition, VOID *pTmpBuff);
/* --- Service Time File Read, R3.1 --- */
SHORT TtlreadSerTime(UCHAR uchMinorClass, UCHAR uchHourOffset, UCHAR uchBorder, TOTAL *pTmpBuff);
SHORT TtlGetSerTimeFilePosition(UCHAR uchClass, ULONG *pulPosition);
/* ===== New Functions (Release 3.1) END ===== */
SHORT TtlTumUpdateDelayedBalance(VOID);
/*
*--------------------------------------
*           Ttlsubwr.c
*--------------------------------------
*/

/* --- File Read --- */
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function TtlWriteFile () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to TtlWriteFile()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message(" \z")
#pragma message("  ====++++====   ----------------------------------    ====++++====")
#pragma message("  ====++++====   TtlWriteFile_Debug() is ENABLED    ====++++====")
#pragma message("  ====++++====   File: Include\\ttl.h                 ====++++====")
SHORT TtlWriteFile_Debug(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize, char *aszFileName, int LineNo);
#define TtlWriteFile(han,ulp,pbuf,uls) TtlWriteFile_Debug(han,ulp,pbuf,uls,__FILE__,__LINE__)
#else
SHORT TtlWriteFile(USHORT hsHandle, ULONG ulPosition, VOID *pTmpBuff, ULONG ulSize);
#endif

VOID TtlNullWrite(SHORT hsHandle, ULONG ulLength, VOID *TempBuff, USHORT usSize);

/* --- Financial Total File Write --- */
SHORT TtlwriteFin(UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff);

/* --- Hourly Total File Write --- */
SHORT TtlwriteHour(UCHAR uchMinorClass, UCHAR uchOffset, HOURLY *pTmpBuff);

/* --- Dept Total File Write --- */
SHORT TtlDeptHeadWrite(TTLCSDEPTHEAD *pDepthd);
SHORT TtlDeptGTotalWrite(ULONG ulPosition, LTOTAL *plTotal);
SHORT TtlDeptMiscWrite(ULONG ulPosition, TTLCSDEPTMISC *DeptMisc);
SHORT TtlDeptEmptyWrite(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlDeptTotalWrite(ULONG ulPosition, USHORT usDeptBlk, TOTAL *pTotal);
SHORT TtlDeptIAssin(ULONG ulPosition, USHORT usDeptNo, TTLCSDEPTINDX *pDeptInd, VOID *pTmpBuff, ULONG ulOffset);
SHORT TtlDeptBAssin(ULONG ulPosition, USHORT usDeptNo, ULONG ulTtlOffset, VOID *pTmpBuff, ULONG ulOffset);

/* --- Coupon Total File Write, R3.0 --- */
SHORT TtlCpnHeadWrite(TTLCSCPNHEAD *pCpnhd);
SHORT TtlCpnTotalWrite(ULONG ulPosition, UCHAR uchCpnNumber, VOID *pTotal, USHORT usSize);

/* --- Cashier Total File Write --- */
SHORT TtlCasHeadWrite(TTLCSCASHEAD *Cashd);
SHORT TtlCasIndexWrite(ULONG ulPosition, USHORT usCasNum, VOID *pTmpBuff);
SHORT TtlCasEmptyWrite(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlCasTotalWrite(ULONG ulPosition, VOID *pTmpBuff);

/* --- Empty Table Update --- */
SHORT TtlEmptyTableUpdate(USHORT *pTmpBuff, USHORT usMaxNo, USHORT usOffset, USHORT usMethod);

/* --- Update Total File Write --- */
SHORT TtlUpHeadWrite(TTLCSUPHEAD *pUphd);

/* ===== New Functions (Release 3.1) BEGIN ===== */
/* --- Individual Financial File Write, R3.1 --- */
SHORT TtlIndFinHeadWrite(TTLCSINDHEAD *pIndHead);
SHORT TtlIndFinIndexWrite(ULONG ulPosition, USHORT usTermNum, VOID *pTmpBuff);
SHORT TtlIndFinEmptyWrite(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlIndFinTotalWrite(ULONG ulPosition, VOID *pTmpBuff);
SHORT TtlIndFinIAssin(VOID *pTmpBuff, USHORT usMaxNo, USHORT usTermNo, USHORT usNumber,  UCHAR uchOffset);
SHORT TtlIndFinIErase(VOID *pTmpBuff, USHORT usTermNum, UCHAR uchOffset);

/* --- Service Time File Write, R3.1 --- */
SHORT TtlwriteSerTime(UCHAR uchMinorClass, UCHAR uchOffset, UCHAR uchTimeBlock, TOTAL *pTmpBuff);
/* ===== New Functions (Release 3.1) END ===== */

/*
*--------------------------------------
*           Ttlmdcch.c
*--------------------------------------
*/
UCHAR TtlMDCCheck(UCHAR uchAddress, UCHAR uchField);

/*
*--------------------------------------
*           Totaltum.c
*--------------------------------------
*/

VOID THREADENTRY TtlTUMMain(VOID);
VOID THREADENTRY TtlTUMJoinDisconnectedSatellite(VOID);
VOID THREADENTRY TtlTUMUpdateTotalsMsg(VOID);

/*
*--------------------------------------
*           Ttlbkup.c
*--------------------------------------
*/
VOID TtlTUMShutDown(VOID);
VOID TtlTUMResume(VOID);
VOID TtlTUMRestoreTtlData(VOID);

/*
*--------------------------------------
*           Ttlbakup.c
*--------------------------------------
*/

SHORT   TtlReqBackEach(SHORT sHandle, UCHAR uchType);
USHORT  TtlResBackEach(TTLBACKUP_RES *pSend, USHORT usMaxLen, SHORT hsHandle);


/*
*--------------------------------------
*           TtlPlu.c    
*   ### ADD 2172 Rel1.0
*--------------------------------------
*/
SHORT TtlPLUOpenDB(VOID);  /* ### ADD 2172 Rel1.0 (01/14/00) */
SHORT TtlPLUCloseDB(VOID); /* ### ADD 2172 Rel1.0 (02/08/00) */
SHORT TtlPLUInit(VOID *pTmpBuff);
SHORT TtlTreadPLU(TTLPLU *pTotal, VOID *pTmpBuff);
SHORT TtlTresetPLU(UCHAR uchClass, TTLPLU  *pPLUTotal);
SHORT TtlWriteSavFilePlu(UCHAR uchClass);
SHORT TtlExportPLU(UCHAR uchClass, TCHAR *pFileName);
SHORT TtlTIssuedPLU(TTLPLU *pTotal, VOID *pTmpBuff, UCHAR uchStat,UCHAR uchMethod);
SHORT TtlTTotalCheckPLU(TTLPLU *pTotal, VOID *pTmpBuff);

SHORT   TtlReqBackEachPlu(const SHORT nMinorClass);                     /* ### ADD 2172 Rel1.0 (01/25/00) */
USHORT  TtlResBackEachPlu(TTLBACKUP_RES *pSend,const USHORT usMaxLen);  /* ### ADD 2172 Rel1.0 (01/25/00) */

SHORT TtlPLUOpenRec(const SHORT nTblId,const ULONG ulSearchOpt,ULONG *pulRecNum);
SHORT TtlPLUOpenRecNoSem(const SHORT nTblID, const ULONG ulSearchOpt, ULONG *pulRecNum);
SHORT TtlPLUGetRec(TTLPLU * pDst);
SHORT TtlPLUGetRecNoSem(TTLPLU * pDst);
SHORT TtlPLUCloseRec(VOID);
SHORT TtlPLUCloseRecNoSem(VOID);
VOID  TtlPLUGetDbError(ULONG * pulPluTtlErr,ULONG * pulDBError);
SHORT   TtlPLUGetGetRecStatus(VOID);

SHORT TtlPLUStartBackup(VOID);      /* ### ADD Saratoga (053100) */
SHORT TtlPLUStartRestore(VOID);     /* ### ADD Saratoga (060700) */
SHORT TtlPLUGetBackupStatus(VOID);  /* ### ADD Saratoga (053100) */
SHORT TtlPLUOptimizeDB(UCHAR uchMinorClass);

/****** end of file ******/

#endif