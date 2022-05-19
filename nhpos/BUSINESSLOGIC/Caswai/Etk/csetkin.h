/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server ETK module, Header file for Internal.
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETKIN.H
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date          Ver.Rev :NAME         :Description
*   Jun-16-93   : 00.00.01:M.YAMAMOTO   :Initial
*   Mar-14-96   : 03.01.00:T.Nakahata   : R3.1 Initial
*       Increase No. of Employees (200 => 250)
*       Increase Employee No. ( 4 => 9digits)
*       Add Employee Name (Max. 16 Char.)
*       Reduce work buffer for index search
*   May-09-96   : 03.01.05:T.Nakahata   : Reduce ETK_NO_OF_BUFFER size
*
*** NCR2172 ***
*
*   Oct-04-99   : 01.00.00 : M.Teraki  : Initial
*                                        Add #pragma pack(...)
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    TYPEDEF STRUCTURE
===========================================================================
*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/* PARAMATER FILE STRUCTURES */

typedef struct {
   USHORT  usFileSize;           /* Paramater File size */
   USHORT  usNumberOfMaxEtk;     /* File's max recoreds */
   USHORT  usNumberOfEtk;        /* Assign's records */
} ETK_PARAHEAD;

typedef struct {
   ULONG    ulEmployeeNo;        /* Employee No. */
   ETK_JOB  EtkJob;              /* Job Code Structure */
   TCHAR    auchEmployeeName[STD_CASHIERNAME_LEN];    /* Employee Name, R3.1 */
} ETK_PARAINDEX;

/* TOTAL FILE STRUCTURE */

typedef struct {
    USHORT     usNumberOfEtk;      /* Number Of Employee in Index Table */
    ULONG      ulOffsetOfIndex;    /* Offset of Index table from file head */
    ULONG      ulOffsetOfParaEmp;  /* Offset of Total Empty Block Table from file head */
    ULONG      ulOffsetOfPara;     /* Offset fo Total Table from File Head */
} ETK_HEAD; 

typedef struct {
    ULONG      ulFileSize;         /* file size */
    USHORT     usNumberOfMaxEtk;   /* Number of max etk records */
    UCHAR      uchIssuedFlag;      /* Issued Flag */
    ETK_HEAD   EtkCur;             /* Current Table's header with Offset data */
    ETK_HEAD   EtkSav;             /* Saved Table's header with Offset data */
    ETK_TIME   EtkTime;            /* last Reset date/time, time stamp of last Saved Table */
} ETK_TOTALHEAD;

typedef struct {
    ULONG   ulEmployeeNo;           /* Employee Number */
    USHORT  usBlockNo;              /* Block Number */
    UCHAR   uchStatus;              /* Status  */
} ETK_TOTALINDEX;                                               

typedef struct  {
    USHORT  usMonth;                 /* Month to be time in */
    USHORT  usDay;                   /* Day to be time in */
    UCHAR   uchJobCode;              /* Job Code to be time in */
    USHORT  usTimeinTime;            /* Time to be time in */
    USHORT  usTimeinMinute;          /* Minute to be time in */
    USHORT  usTimeOutTime;           /* Time to be time out */
    USHORT  usTimeOutMinute;         /* Minute to be time out */
}ETK_FIELD_IN_FILE;

typedef struct {
	ETK_PARAINDEX  paraData;         // employee identifying data
    USHORT         usBlockNo;        /* ETK total file Block Number from ETK_TOTALINDEX */
    UCHAR          uchStatus;        /* ETK total file Status from ETK_TOTALINDEX  */
} ETK_PARA_TOTAL;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/*  ISSUED FLAG DEFINE */

#define ETK_PARA_EMPTY_TABLE_SIZE  (((FLEX_ETK_MAX - 1) / 8) + 1)       /* EMPTY TABLE SIZE */

#define ETK_MASK_FIELD_POINTER  0x0f /* MASK FOR FIELD POINTER */

/* Etk_Index() 's DEFINE */
#define ETK_SEARCH_ONLY        1    /* Search Mode Id */
#define ETK_REGIST             2    /* Regist Mode Id */
#define ETK_RESIDENT           3    /* Employee in File */
#define ETK_REGISTED           4    /* Employee already in file */

/*  Issued Flag set */
#define ETK_FLAG_SET           0xff /* data for setting Issued Flag */
#define ETK_FLAG_CLEAR         0    /* data for clearing Issued Flag */

/* File Control */
#define ETK_FILE_HED_POSITION  0L  /* for File Seek to the file management header */
#define ETK_NO_OF_BUFFER       10  /* for Parameter File Read */

/* time in / time out EtkIndex.uchStatus */

#define ETK_TIME_OUT            0x00
#define ETK_TIME_IN             0X80

/*
***************************************
    STATIC MEMORY DEFINE              *
***************************************
*/
extern  TCHAR   CONST  auchETK_PARA_FILE[];
extern  TCHAR   CONST  auchETK_TOTAL_FILE[];
extern  USHORT  husEtkSem;    
extern  SHORT   hsEtkParaFile;
extern  SHORT   hsEtkTotalFile;

/*
***************************************
    PROTOTYPE                         *
***************************************
*/
SHORT   Etk_Index(ETK_HEAD *pEtkHead,
                  USHORT usIndexMode,
                  ULONG ulEtkNo,
                  ETK_TOTALINDEX *pEtkEntry,
                  USHORT *usParaBlockNo,
                  ULONG  *ulIndexPosition);
SHORT  Etk_IndexDel(ETK_TOTALHEAD *pEtkHead, ULONG ulEtkNo );
VOID   Etk_TimeFieldRead(ETK_HEAD *pEtkHead,
                         ETK_TOTALINDEX *pEtkTotalIndex,
                         USHORT usParaBlockNo,
                         ETK_FIELD_IN_FILE *pEtkField);
VOID   Etk_TimeFieldWrite(ETK_HEAD *pEtkHead,
                         ETK_TOTALINDEX *pEtkTotalIndex,
                         USHORT usParaBlockNo,
                         ETK_FIELD_IN_FILE *pEtkField);
SHORT  Etk_EmpBlkGet(ETK_HEAD *pEtkHead, USHORT *usParaBlockNo);
VOID   Etk_BlkEmp(ETK_HEAD *pEtkHead, USHORT usParaBlockNo);
VOID   Etk_WriteFile(SHORT hsFileHandle, ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG usSizeofWrite);
SHORT  Etk_ReadFile(SHORT  hsFileHandle, ULONG  offulFileSeek, VOID   *pTableHeadAddress, ULONG ulSizeofRead);
VOID   Etk_CloseAllFileReleaseSem(VOID);
VOID   Etk_CloseIndFileReleaseSem(SHORT hsFileHandle);
VOID   Etk_ClearStatus(ETK_HEAD *pEtkHead);
SHORT  Etk_CheckStatus(ETK_HEAD *pEtkHead);
SHORT  Etk_CheckAllowJob(ETK_JOB *pEtkJob, UCHAR uchJobCode);
VOID   Etk_AllowJobRead(ETK_HEAD *pEtkHead, USHORT usParaBlockNo, ETK_JOB *pEtkJob);
VOID   Etk_AllowJobWrite(ETK_HEAD *pEtkHead, USHORT usParaBlockNo, ETK_JOB *pEtkJob);
SHORT  Etk_MakeParaFile(USHORT  usNumberOfEmployee);
SHORT  Etk_MakeTotalFile(USHORT  usNumberOfEmployee);
SHORT  Etk_IndexDelPara( ULONG ulEtkNo );
SHORT  Etk_IndexPara(USHORT usIndexMode, ULONG ulEtkNo, ETK_PARAINDEX *pEtkEntry, ULONG  *pulIndexPosition);
SHORT  Etk_OpenAllFile(VOID);
SHORT  Etk_OpenIndFile(USHORT usFileType);
SHORT  Etk_GetParaTotalBlocks (ETK_PARA_TOTAL *pList, SHORT sMaxCount);


/* ===== End of File ( CSETKIN.H ) ===== */
