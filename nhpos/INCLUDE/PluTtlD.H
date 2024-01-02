/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------

        NeighborhoodPOS Software Product Development Project
        Department of Information Systems
        College of Information Technology
        Georgia Southern University
        Statesboro, Georgia

        Copyright Georgia Southern University Foundation

*---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1999 NCR Corporation, All rights reserved.
//
// --------------------------------------------------------------------------
//    Georgia Southern University Research Services Foundation
//    donated by NCR to the research foundation in 2002 and maintained here
//    since.
//       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
//       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
//       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
//       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
//       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
//       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
//       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
//
//    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
// --------------------------------------------------------------------------
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Sep-23-2023  0x.04.00 R.Chambers   removed the unused definition of 'PLUTOTAL_DEBUG_BREAK'
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef __PLUTTLD_H
#define __PLUTTLD_H

#include "ecr.h"

#ifdef  _cplusplus
    extern  "C" {
#endif

#if defined(_PLUTOTAL_DLL)
    #define PLUTOTAL_DECLSPEC  __declspec( dllexport )
#else
    #define PLUTOTAL_DECLSPEC  __declspec( dllimport )
#endif


#define PLUTTLAPI   

#ifndef BYTE
    typedef unsigned char  BYTE;
#endif

/*** table index ***/
#define PLUTOTAL_ID_DAILY_CUR       (0x01)      /* Current/Daily    TOTAL   */
#define PLUTOTAL_ID_DAILY_CUR_S     (0x11)      /*                  STATUS  */
#define PLUTOTAL_ID_DAILY_SAV       (0x02)      /* Saved/Daily TOTAL        */
#define PLUTOTAL_ID_DAILY_SAV_S     (0x12)      /*                  STATUS  */
#define PLUTOTAL_ID_PTD_CUR         (0x03)      /* Current/PTD TOTAL        */
#define PLUTOTAL_ID_PTD_CUR_S       (0x13)      /*                  STATUS  */
#define PLUTOTAL_ID_PTD_SAV         (0x04)      /* Saved/PTD                */
#define PLUTOTAL_ID_PTD_SAV_S       (0x14)      /*                  STATUS  */

#define PLUTOTAL_ID_TMP_CUR         (0x05)      /*                  TOTAL   */
#define PLUTOTAL_ID_TMP_CUR_S       (0x15)      /*                  STATUS  */

/*** database files ***/
// Following strings are used for SQL Server database backup which is used for the AC42 and other
// PLU Totals synchronization. 
#define PLUTOTAL_DB_BACKUP_PATH     L"C:\\TempDisk\\NCR\\Saratoga\\Database\\"    // was _T("C:\\Program Files\\Microsoft SQL Server\\MSSQL\\Data\\")
#define PLUTOTAL_DB_BACKUP_RESTORE  L"RESTORE DATABASE %s FROM DISK = 'C:\\TempDisk\\NCR\\Saratoga\\Database\\%s_BAK.dat' WITH REPLACE"
#define PLUTOTAL_DB_BACKUP_BACKUP   L"BACKUP DATABASE %s TO DISK = 'C:\\TempDisk\\NCR\\Saratoga\\Database\\%s_BAK.dat' WITH INIT"

/* database files path */
#define PLUTOTAL_DBNM_DAILY_CUR   _T("PluTtlDC")  /* Current/Daily            */
#define PLUTOTAL_DBNM_DAILY_SAV   _T("PluTtlDS")  /* Saved/Daily              */
#define PLUTOTAL_DBNM_PTD_CUR     _T("PluTtlPC")  /* Current/PTD              */
#define PLUTOTAL_DBNM_PTD_SAV     _T("PluTtlPS")  /* Saved/PTD                */

#define PLUTOTAL_DBNM_TMP_CUR     _T("PluTtlTC")  /*                          */

/*** names of the tables stored in each database ***/
/*** for SARATOGA/NCR2172A ***/
#define PLUTPTAL_TBNM_TOTAL       _T("Total")     /* PLU totals data reflecting sales made */
#define PLUTPTAL_TBNM_STATUS      _T("Status")    /* status of the PLU totals */

// Totals Totals table columns for maintaining PLU totals
#define SQLVAR_PLUNUMBER     _T("PluNumber")
#define SQLVAR_ADJINDEX      _T("AdjectiveIndex")
#define SQLVAR_TOTAL         _T("Total")
#define SQLVAR_COUNTER       _T("Counter")

// Totals Status Table columns for maintaining PLU data status
#define SQLVAR_VERSION       _T("Version")
#define SQLVAR_RESETSTATUS   _T("ResetStatus")
#define SQLVAR_FROMDATE      _T("FromDate")
#define SQLVAR_TODATE        _T("ToDate")
#define SQLVAR_PLUALLTOTAL   _T("PluAllTotal")
#define SQLVAR_PLUAMOUNT     _T("PluAmount")

/*** table & index format (SQL) ***/
#if defined(DCURRENCY_LONGLONG)
// We are using 64 bit integers, long long, for storing currency so we need to change the database
// schema so that Totals that were int, 32 bit long, are now bigint, 64 bit long long.
#define PLUTOTAL_TOTAL_TBL_FRM   _T("PluNumber varchar(14),AdjectiveIndex smallint,Total bigint,Counter int,TotalVoid bigint,CounterVoid int,TotalRtn1 bigint,CounterRtn1 int,TotalRtn2 bigint,CounterRtn2 int,TotalRtn3 bigint,CounterRtn3 int ")
#define PLUTOTAL_STATUS_TBL_FRM  _T("ResetStatus int,FromDate datetime,ToDate datetime,PluAllTotal bigint,PluAmount bigint,Version int ") //RPH ResetStatus int/*uint*/

// define these here since only used with 64 bit totals and we want compiler errors if used
// when 64 bit totals are turned off.
#define SQLVAR_TOTALVOID     _T("TotalVoid")
#define SQLVAR_COUNTERVOID   _T("CounterVoid")
#define SQLVAR_TOTALRTN1     _T("TotalRtn1")
#define SQLVAR_COUNTERRTN1   _T("CounterRtn1")
#define SQLVAR_TOTALRTN2     _T("TotalRtn2")
#define SQLVAR_COUNTERRTN2   _T("CounterRtn2")
#define SQLVAR_TOTALRTN3     _T("TotalRtn3")
#define SQLVAR_COUNTERRTN3   _T("CounterRtn3")
#else
#define PLUTOTAL_TOTAL_TBL_FRM  "PluNumber varchar(14),AdjectiveIndex smallint,Total int,Counter int "
/* ### MOD (041800) #define PLUTOTAL_STATUS_TBL_FRM "ResetStatus uint,FromDate datetime,ToDate datetime,PluAllTotal int,Version int "*/
#ifdef XP_PORT //uint not supported with MSDE (ResetStatus uint->int)
#define PLUTOTAL_STATUS_TBL_FRM "ResetStatus int,FromDate datetime,ToDate datetime,PluAllTotal int,PluAmount int,Version int "//RPH ResetStatus int/*uint*/
#else
#define PLUTOTAL_STATUS_TBL_FRM "ResetStatus uint,FromDate datetime,ToDate datetime,PluAllTotal int,PluAmount int,Version int "
#endif
#endif


/*** dbfile option ***/
#define PLUTOTAL_DBFILE_READ          (0x0001)    /* read                     */
#define PLUTOTAL_DBFILE_CREATEWRITE   (0x0002)    /* write                    */
#define PLUTOTAL_DBFILE_CREATEREAD    (0x0004)    /* create                   */

#define PLUTOTAL_PLUNO_LEN          14          /* length of PLU number     */
#define PLUTOTAL_REC_MAX            10000       /* 10,000 records           */

/*** search option & SQL codes ***/
#define PLUTOTAL_SORT_TOTAL         (0x01)      /*                          */
#define PLUTOTAL_SORT_PLUNO         (0x02)      /*                          */
#define PLUTOTAL_SEARCH_CNT         (0x04)      /* Counter > 0              */

/*** database handle ***/
#define PLUTOTAL_HANDLE_NULL        (0)         /*                          */

/*** STATUS codes ***/
#define PLUTOTAL_SUCCESS            0           /* no error                 */

#define PLUTOTAL_BOF                100         /* BOF                      */
#define PLUTOTAL_EOF                101         /* EOF                      */
#define PLUTOTAL_NOT_FOUND          102         /* record not found         */

#define PLUTOTAL_E_OPENED           1000        /* already opened           */
#define PLUTOTAL_E_CLOSED           1001        /* closed                   */
#define PLUTOTAL_E_EXIST            1002        /*                          */
#define PLUTOTAL_E_DUP              1003        /* duplicate                */
#define PLUTOTAL_E_FULL             1004        /* over max record          */
#define PLUTOTAL_E_ILLEAGAL         9998        /* illeagal parameter       */
#define PLUTOTAL_E_FAILURE          9999        /* failure                  */
/* ### ADD Saratoga Backup status (060100) */
#define PLUTOTAL_BK_STATUS_READY         2001        /* copy is not working                      */
#define PLUTOTAL_BK_STATUS_BUSY          2002        /* copy is working                          */
#define PLUTOTAL_BK_ACCEPTED_CLEARLY     2003        /* accepted normally                        */
#define PLUTOTAL_BK_ACCEPTED_LOWSTORAGE  2004        /* accepted but free diskspace is now low   */
#define PLUTOTAL_BK_DENIED_BUSY          2005        /* denied because of copy is working        */
#define PLUTOTAL_BK_DENIED_NOHEAP        2006        /* denied because of not enough heap        */
#define PLUTOTAL_BK_DENIED_NOSTORAGE     2007        /* denied because of not enough diskspace   */

#define PLUTOTAL_DB_ERROR_MSDEDOWN		16389		//The HRESULT returns this value when the MSDE engine
													//is not running on the terminal, I looked into the MSDN for 
													// a better way to define this, but could not JHHJ
                                                    //
                                                    //NOTE: Also this error may indicate SQLite OBDC driver not installed
                                                    //      if using the SQLIte PLU database functionality. This appears
                                                    //      to be a general error meaning an OBDC driver error. RJC

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

/*** typedef ***/
typedef long    PLUTOTAL_DB_HANDLE;             /* database handle          */
typedef long    PLUTOTAL_FILE_HANDLE;           /* database file handle     */

#if defined(DCURRENCY_LONGLONG)
/*** Total Table ***/
    typedef struct {
        USHORT      usSize;                              /* record size              */
        TCHAR       auchPluNumber[PLUTOTAL_PLUNO_LEN];   /* PLU No.                  */
        SHORT       sAdjIdx;                             /* Adjective Index          */
        DCURRENCY   lTotal;                              /* Total, All types         */
        LONG        lCounter;                            /* Counter, All types       */
        DCURRENCY   lTotalVoid;                          /* Total, Void                      */
        LONG        lCounterVoid;                        /* Counter, Void                    */
        DCURRENCY   lTotalRtn1;                          /* Total, TTL_TUP_SALES_RETURN_1    */
        LONG        lCounterRtn1;                        /* Counter, TTL_TUP_SALES_RETURN_1  */
        DCURRENCY   lTotalRtn2;                          /* Total, TTL_TUP_SALES_RETURN_2    */
        LONG        lCounterRtn2;                        /* Counter, TTL_TUP_SALES_RETURN_2  */
        DCURRENCY   lTotalRtn3;                          /* Total, TTL_TUP_SALES_RETURN_3    */
        LONG        lCounterRtn3;                        /* Counter, TTL_TUP_SALES_RETURN_3  */
    } PLUTOTAL_REC;
#else
/*** Total Table ***/
    typedef struct {
        USHORT      usSize;                              /* record size              */
        TCHAR       auchPluNumber[PLUTOTAL_PLUNO_LEN];   /* PLU No.                  */
        SHORT       sAdjIdx;                             /* Adjective Index          */
        DCURRENCY   lTotal;                              /* Total, All types         */
        LONG        lCounter;                            /* Counter, All types       */
    } PLUTOTAL_REC;
#endif

#if defined(DCURRENCY_LONGLONG)
#define PLUTOTAL_REC_FLD_NUM    6               /* number of fields         */
#else
#define PLUTOTAL_REC_FLD_NUM    4               /* number of fileds         */
#endif

#define PLUTOTAL_STSREC_FLD_NUM 6               

/*** Date structure ***/
    typedef struct {
        USHORT      usYear;                     /* year (1999,2000,2001...) */
        USHORT      usMonth;                    /* month                    */
        USHORT      usDay;                      /* day                      */
        USHORT      usHour;                     /* hour                     */
        USHORT      usMin;                      /* minute                   */
        USHORT      usSec;                      /* second                   */
    } PLUTOTAL_DATE;

/*** Status Table ***/
#define PLUTOTAL_DB_VERSION     (100)           /* 0100 -> 01.00            */
    typedef struct{
        USHORT      usSize;                     /* record size              */
        ULONG       ulResetStatus;              /* reset status             */
        PLUTOTAL_DATE   dateFrom;               /* From date                */
        PLUTOTAL_DATE   dateTo;                 /* To date                  */
        DCURRENCY   lAllTotal;                  /* Plu All Total            */
        DCURRENCY   lAmount;                    /* Plu Amount               *//* ### ADD (041800) */
        LONG        lVersion;                   /* table version            */
    } PLUTOTAL_STATUS;

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

#ifdef  _cplusplus
} /* from extern "C" */
#endif

/*----- API entries ---*/
#ifdef  __cplusplus
/*  #pragma message("--- C++ ---")*/
    #define PLUTOTAL_EXTERN_C   extern "C"
#else
/*  #pragma message("--- standard C ---")*/
    #define PLUTOTAL_EXTERN_C
#endif

/*** open/close database ***/
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalOpen(PLUTOTAL_DB_HANDLE * phdDB, SHORT fPTD, SHORT fTmp);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalClose(const PLUTOTAL_DB_HANDLE hdDB);

PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalGetRecCnt(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG *ulRecCnt);
PLUTOTAL_EXTERN_C	PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalRestoreDB(const PLUTOTAL_DB_HANDLE hdDB, const SHORT nTblID);


/*** create/drop tables ***/
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalCreateTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const ULONG ulRec);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalDropTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalMoveTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nFromTblID,const SHORT nToTblID);
PLUTOTAL_EXTERN_C	PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalExportTblToFile(const PLUTOTAL_DB_HANDLE hdDB, const SHORT nFromTblID, const TCHAR *pFileName);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalCheckTbl(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);

PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalInsert(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC pRecData);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalDelete(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalUpdate(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_REC RecData);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalFind(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const TCHAR pPluNo[],const BYTE byAdjIdx,PLUTOTAL_REC *pRecBuf);

PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalExecSQL_Dbg(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const CHAR * szSqlCode);

/*** recored set ***/
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalSelectRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG SearchCond);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalFirstRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalLastRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalNextRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalPrevRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalGetRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_REC *pRecBuf);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalGetNumRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,ULONG * pulRecNum);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalReleaseRec(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID);

PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalGetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,PLUTOTAL_STATUS * pStatusRec);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalSetStsInfo(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,const PLUTOTAL_STATUS StatusRec);

/*** database files ***/
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalOpenFile(PLUTOTAL_DB_HANDLE * phdDbFile,const SHORT nTblID,const ULONG ulOption);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalCloseFile(const PLUTOTAL_DB_HANDLE hdDbFile);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalReadFile(const PLUTOTAL_DB_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,UCHAR * pucBuf,LONG * plRedLen);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalWriteFile(const PLUTOTAL_DB_HANDLE hdDBFile,const ULONG uloffset,const LONG lLen,const UCHAR * pucBuf);

PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalDelFile(const SHORT nTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalMoveFile(const SHORT nFromTblID, const SHORT nToTblID);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalCopyFile(const SHORT nFromTblID, const SHORT nToTblID);

/* ### ADD Saratoga Backup DB files (053000) */
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalStartBackup(void);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalStartRestore(void);
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalGetBackupStatus(void);

/*** DEBUG & TESTS ***/
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC ULONG   PLUTTLAPI  PluTotalMakeTestData_Dbg(const PLUTOTAL_DB_HANDLE hdDB,const SHORT nTblID,
    const LONG lFrom,const LONG lTo,const LONG lStep);
#if defined(_PLUTOTAL_DLL)
PLUTOTAL_EXTERN_C   PLUTOTAL_DECLSPEC VOID    PLUTTLAPI  PluTotalPutLog_Dbg(LPCTSTR tMsg1,LPCTSTR tMsg2,LPCTSTR tMsg3);
#endif

#endif  // __PLUTTLD_H

