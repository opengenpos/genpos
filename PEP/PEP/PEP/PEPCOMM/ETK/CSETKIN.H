/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
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
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date          Ver.Rev :NAME         :Description
*   Oct-07-93   : 00.00.01:H.YAMAGUCHI  :Modified FVT comments
*   Jan-29-96   : 03.01.00:M.SUZUKI     : R3.1 Initial
*                                       Increase No. of Employees (200 => 250)
*                                       Increase Employee No. ( 4 => 9digits)
*                                       Add Employee Name (Max. 16 Char.)
*
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

//pragma pack added 12/02/02 RPH
#pragma pack(push, 1)

typedef struct {
    USHORT  usEtkFileSize;          /* size of File */
    USHORT  usNumberOfMaxEtk;       /* number of Max Employee */
    USHORT  usNumberOfEtk;          /* number of current Etk */
} ETK_FILEHEAD;

typedef struct {
    ULONG    ulEmployeeNo;           /* Employee Number */
    ETK_JOB  EtkJC;                  /* Job code */
    WCHAR    auchEmployeeName[ETK_MAX_NAME_SIZE];    /* Employee Name, R3.1 */
} ETK_INDEX;                                               

typedef struct {
    USHORT    usEmployeeNo;           /* Employee Number */
    ETK_JOB  EtkJC;                   /* Job code */
} ETK_INDEX_OLD;                                               

#pragma pack(pop)

/*  ISSUED FLAG DEFINE */

#define ETK_COUNTER_OF_SEM      1   /* Semaphore Counter */

/* #define ETK_ISSUED_RESET        0 */  /* ISSUED RESET */
/* #define ETK_INDEX_RECORD_SIZE   (sizeof(ETK_INDEX))  */ /* INDEX RECORD TABLE SIZE */
/* #define ETK_PARA_EMPTY_TABLE_SIZE  (((ETK_MAX_EMPLOYEE - 1) / 8) + 1)     */

/* #define ETK_NUMBER_OF_FIELD     15   */  /* ETK FIELD(JOB/DATE) / 1 EMPLOYEE */
/* #define ETK_MASK_FIELD_POINTER  0x0f */  /* MASK FOR FIELD POINTER */
/* #define ETK_CUR                 1    */  /* ID OF CURRENT BUFFER */
/* #define ETK_SAV                 2    */  /* ID OF SAVED BUFFER */

/* Etk_Index() 's DEFINE */
#define ETK_SEARCH_ONLY        1    /* Search Mode Id */
#define ETK_REGIST             2    /* Regist Mode Id */
#define ETK_RESIDENT           3    /* Employee in File */
#define ETK_REGISTED           4    /* Employee already in file */

/*  Issued Flag set */
/* #define ETK_FLAG_SET           0xff */ /* data for setting Issued Flag */
/* #define ETK_FLAG_CLEAR         0    */ /* data for clearing Issued Flag */

/* File Control */
#define ETK_FILE_HED_POSITION  0L  /* for File Seek */
#define ETK_NO_READ_SIZE       -31 /* for File Read */
#define ETK_UNLOCK             -32 /* for File Read */

/* time in / time out EtkIndex.uchStatus */

/* #define ETK_TIME_OUT            0x00 */
/* #define ETK_TIME_IN             0X80 */

/*
***************************************
    STATIC MEMORY DEFINE              *
***************************************
*/
extern  WCHAR   FARCONST  auchETK_FILE[];
#if defined(POSSIBLE_DEAD_CODE)
extern  WCHAR   FARCONST  auchETK_OLD[];
#endif
extern  USHORT  husEtkSem;    
extern  SHORT   hsEtkFileHandle;
#if defined(POSSIBLE_DEAD_CODE)
extern  UCHAR   uchEtkAllLockMem;
extern  SHORT   hsEtkNewFileHandle;
#endif
/*
***************************************
    PROTOTYPE                         *
***************************************
*/

VOID   Etk_GetHeader(ETK_FILEHEAD *pEtkFileHed);
VOID   Etk_PutHeader(ETK_FILEHEAD *pEtkFileHed);

SHORT  Etk_Index(ETK_FILEHEAD *pEtkHead,
                 USHORT       usIndexMode,
                 ULONG        ulEtkNo,
                 ETK_INDEX    *pEtkEntry,
                 ULONG        *ulIndexPosition);

SHORT  Etk_IndexDel(ETK_FILEHEAD *pEtkHead, 
                    ULONG        ulEtkNo );

SHORT  Etk_LockCheck(VOID);
SHORT  Etk_CompIndex(ULONG      *pulKey,    
                     ETK_INDEX  *pusHitPoint);

VOID   Etk_WriteFile(ULONG  offulFileSeek,
                     VOID   *pTableHeadAddress,
                     ULONG usSizeofWrite);

SHORT  Etk_ReadFile(ULONG  offulFileSeek,
                    VOID   *pTableHeadAddress,
                    ULONG usSizeofRead);

VOID   Etk_CloseFileReleaseSem(VOID);
#if defined(POSSIBLE_DEAD_CODE)
VOID   Etk_MakeHeader(ETK_FILEHEAD *pEtkFileHed, 
                      USHORT       usNumberOfEmployee);
#endif
/*------------------------------------------
    External symbol access for file 
-------------------------------------------*/
extern UCHAR FARCONST  auchNEW_FILE_WRITE[];        /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_WRITE[];        /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_READ_WRITE[];   /* to csop.c */
extern UCHAR FARCONST  auchOLD_FILE_READ[];         /* to csop.c */

/*==== End of define =====*/
