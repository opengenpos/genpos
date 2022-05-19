/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER , Header file for internal                        
*   Category           : Client/Server OPERATIONAL PARAMATER  module, HOSPITAL FOR US                                                 
*   Program Name       : CSOPIN.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*          
*   Date      : Ver.Rev  :NAME       :Description
*   Nov-03-93 : 00.00.01 :H.YAMAGUCHI:Increase PLU 1000 --> 3000
*   Feb-23-95 : 03.00.00 :H.Ishida   :Add OPE & Coupon
*   Feb-27-95 : 03.00.00 :H.Ishida   :Add OP_CPN_FILE
*   Mar-31-95 : 03.00.00 :H.Ishida   :Add OP_CSTR_FILE
*   Jan-11-96 : 03.01.00 :T.Nakahata :R3.1 Initial
*       Add PPI File and Increase Size of Dept Control Code
** NCR2172 **
*
*   Oct-05-99 : 01.00.00 :M.Teraki   : Added #pragma(...)
*   Dec-14-99 : 01.00.00 :hrkato     : Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#define CSOPH_INCLUDED

/*
===========================================================================
    DEFINE
===========================================================================
*/
#define  OP_MAX_PLU_THIRD           (OP_NUMBER_OF_MAX_PLU / 3)
#define  OP_MAX_PLU_SIXTH           (OP_NUMBER_OF_MAX_PLU / 6)
#define  OP_DEPT_NUMBER_OF_MAX     250  /* 2172 */
#define  OP_CPN_NUMBER_OF_MAX      300  //100 Coupon Max Change 100=>300 JHHJ 3-15-04
#define  OP_PLU_INDEXBLK_SIZE        2  
#define  OPPLU_PARA_EMPTY_TABLE_SIZE  (OP_NUMBER_OF_MAX_PLU / 8) + (7 + (OP_NUMBER_OF_MAX_PLU % 8)) / 8
/* Add 2172 Rel 1.0 */
//#define  OP_OEPPLU_PARA_EMPTY_TABLE_SIZE  (OP_NUMBER_OF_MAX_OEP / 8) + (7 + (OP_NUMBER_OF_MAX_OEP % 8)) / 8

#define  OP_HALO_SIZE                1
#define  OP_CONTROL_CODE_SIZE        6  /* increase at R3.1 */
/*#define  OP_CONTROL_CODE_SIZE        5*/
#define  OP_MNEMONIC_SIZE           20  /* Saratoga */
#define  OP_FILE_HEAD_POSITION       0L
/* #define  OP_DEPT_FILE_HEAD_SIZE      2*/

/* 2172 */
#define  OP_MAX_DEPT_THIRD           (OP_NUMBER_OF_MAX_DEPT / 3)
#define  OP_MAX_DEPT_SIXTH           (OP_NUMBER_OF_MAX_DEPT / 6)

#define  OP_DEPT_INDEXBLK_SIZE        2  
#define  OPDEPT_PARA_EMPTY_TABLE_SIZE  (OP_DEPT_NUMBER_OF_MAX / 8) + (7 + (OP_DEPT_NUMBER_OF_MAX % 8)) / 8

/* === Add Promotional Pricing Item (Release 3.1) BEGIN === */
#define  OP_PPI_INDEXBLK_SIZE        2
#define  OPPPI_PARA_EMPTY_TABLE_SIZE  (OP_MAX_PPI_NO / 8) + (7 + (OP_MAX_PPI_NO % 8)) / 8
/* === Add Promotional Pricing Item (Release 3.1) END === */

#define  OP_RPRTPLUREAD              1
#define  OP_DEPTPLUREAD              2
#define  OP_RETRY_CO                 3
#define  OP_OEPPLUREAD               4
/*
===========================================================================
   DEFINE FOR ERROR 
===========================================================================
*/

/*
===========================================================================
   DEFINE FOR ERROR CODE
===========================================================================
*/

/*
===========================================================================
   DEFINE FOR OTHER
===========================================================================
*/
#define  OP_REGIST              1
#define  OP_SEARCH_ONLY         2
#define  OP_FOREVER             1

#define  OP_CLASS_EOT           0xff

#define  OP_MAX_BROADCAST_IP    16   /* Hosts address boradcast, V1.0.04, PIF_NET_MAX_IP */

#define RFL_HIT                 0
#define OP_EOT                  0
#define OP_CONTINUE             1
#define OP_END                  2

/* BackUp File Define */
#define OP_PARA_FILE            0    // First record for transfer of PARARAM Para data area.  Initial value will be incremented
#define OP_DEPT_FILE          203    // Transfer Department File.  large sequence jump from OP_PARA_FILE needed
#define OP_PLU_FILE           204
#define OP_CPN_FILE           205
#define OP_CSTR_FILE          206
/* === Add Promotional Pricing Item (Release 3.1) BEGIN === */
#define OP_PPI_FILE           207   /* PPI File Backup, R3.1 */
/* === Add Promotional Pricing Item (Release 3.1) END === */
#define OP_MLD_FILE           208   /* MLD File Backup, V3.3 */
/* Add 2172 Rel 1.0 */
#define OP_OEP_FILE           209
#define OP_HOSTS_FILE         210
#define OP_BIOMETRICS_FILE    211
#define OP_REASONCODES_FILE   212

/*
===========================================================================
    STRUCT
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef  struct {
   ULONG    ulPluFileSize;
   USHORT   usNumberOfMaxPlu;
   USHORT   usNumberOfResidentPlu;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTblBlk;
   ULONG    offulOfIndexTblRept;
   ULONG    offulOfIndexTblDept;
   ULONG    offulOfIndexTblGrpNo;
   ULONG    offulOfParaEmpTbl;
   ULONG    offulOfParaTbl;
   ULONG    offulOfWork;
} OPPLU_FILEHED;

typedef  struct {
   ULONG    ulPluFileSize;
   USHORT   usNumberOfMaxPlu;
   USHORT   usNumberOfResidentPlu;
   USHORT   offusOffsetOfIndexTbl;
   USHORT   offusOffsetOfIndexTblBlk;
   USHORT   offusOffsetOfIndexTblRept;
   USHORT   offusOffsetOfIndexTblDept;
   USHORT   offusOffsetOfParaEmpTbl;
   USHORT   offusOffsetOfParaTbl;
} OPPLU_FILEHED_THO;

/* Add 2172 Rel 1.0 */
typedef  struct {
   ULONG    ulOepFileSize;
   USHORT   usNumberOfMaxOep;
   USHORT   usNumberOfResidentOep;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTbl2;
   USHORT   usNumberOfResidentOep2;
} OPOEP_FILEHED;

/* 2172 */
typedef  struct {
   ULONG    ulDeptFileSize;
   USHORT   usNumberOfMaxDept;
   USHORT   usNumberOfResidentDept;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTblBlk;
   /* ULONG    offulOfIndexTblMDeptt; */
   ULONG    offulOfParaEmpTbl;
   ULONG    offulOfParaTbl;
   ULONG    offulOfWork;
} OPDEPT_FILEHED;

typedef  struct {
   ULONG    ulDeptFileSize;
   USHORT   usNumberOfMaxDept;
   USHORT   usNumberOfResidentDept;
   USHORT   offusOffsetOfIndexTbl;
   /* USHORT   offusOffsetOfIndexTblMDept; */
   USHORT   offusOffsetOfParaEmpTbl;
   USHORT   offusOffsetOfParaTbl;
} OPDEPT_FILEHED_THO;

/*
typedef  struct {
   USHORT   usNumberOfDept;   
} OPDEPT_FILEHED;
*/

typedef  struct {
   USHORT   usNumberOfCpn;   
} OPCPN_FILEHED;

typedef  struct {
   USHORT   usNumberOfCstr;
   USHORT   usOffsetOfTransCstr;
} OPCSTR_FILEHED;

/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
typedef  struct {
   USHORT   usNumberOfPpi;
} OPPPI_FILEHED;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */

typedef  struct {
   TCHAR    auchPluNo[OP_PLU_LEN];        /* Saratoga */
   UCHAR    uchAdjective;
} OPPLU_INDEXENTRY;

typedef  struct {
   UCHAR    uchReportNo;
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYREPORT;

typedef struct {
   USHORT   usDeptNo;                       /* Saratoga */
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYDEPT;

typedef struct {
   UCHAR    uchGroupNo;
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYGROUPNO;

typedef struct {
   USHORT         usDept;                   /* Saratoga */
   UCHAR          uchRept;
   UCHAR          auchContOther[5];
} OPPLU_CONTROL;

/* Add 2172 Rel1.0 */
typedef  struct {
   USHORT  usGroupNo;                  // contains either grouop number, department number, etc depending on OEP type
   TCHAR   auchOepPluNo[OP_PLU_OEP_LEN];  // contains three digit priority and the PLU number
   UCHAR   uchAdjective;               // for adjective PLUs, contains a PLU adjective index
} OPOEP_INDEXENTRY; /* 2172 */

typedef  struct {
   USHORT   usDeptNo;
} OPDEPT_INDEXENTRY;    /* 2172 */

typedef struct {
   UCHAR          auchControlCode[OP_CONTROL_CODE_SIZE];
   UCHAR          uchHalo[OP_HALO_SIZE];
   TCHAR          auchMnemonic[OP_MNEMONIC_SIZE];
   UCHAR          uchPrintPriority;                    /* Print Priority */
} OPDEPT_PARA;

/* Add 2172 Rel1.0 */
typedef  struct {
   UCHAR   uchGroupNo;
   TCHAR   auchPluNo[OP_PLU_LEN];
   UCHAR   uchAdjective;
} OPOEP_PARA;

typedef struct {
   UCHAR          uchClass;
   ULONG          ulOffset;
} OPBRD_PARA;

typedef  struct {
   UCHAR    uchTableNo;
   USHORT   usIndexNo;
} OP_SUBINDEXENTRY;

typedef struct {
    UCHAR       uchClass;          // indicates type of file to transfer such as OP_OEP_FILE
    UCHAR       uchSubClass;       // indicates the sub type of file for file classes that have multiple files
    ULONG       offulFilePosition;
    USHORT      usSeqNo;
	USHORT      usTransferOptions;
    USHORT      usRecordNo;        // Normally max number of records to be transferred from a file header but not for some files.
    UCHAR       uchPTDFlag;
    UCHAR       uchStatus;
}OP_BACKUP;

/* MLD Mnemonics File, V3.3 */
typedef  struct {
   USHORT   usNumberOfAddress;
} OPMLD_FILEHED;

/* Hosts address boradcast, V1.0.04 */
typedef struct {
    TCHAR   auchHostName[PIF_LEN_HOST_NAME];
    UCHAR   auchIPAddress[PIF_LEN_IP];
} HOSTNAMEIP;

typedef struct {
    HOSTNAMEIP aHostNameIP[OP_MAX_BROADCAST_IP];
} HOSTSIP;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
   PROTOTYPE
===========================================================================
*/

SHORT   Op_LockCheck(USHORT usLockHnd);
VOID    Op_ClosePluFileReleaseSem(VOID);
/* Add 2172 Rel 1.0 */
VOID    Op_CloseOepFileReleaseSem(VOID);
VOID    Op_CloseDeptFileReleaseSem(VOID);
VOID    Op_CloseCpnFileReleaseSem(VOID);
VOID    Op_CloseCstrFileReleaseSem(VOID);
SHORT   Op_ReadDeptFile(ULONG  offulFileSeek, 
                        VOID   *pTableHeadAddress, 
                        ULONG  ulSizeofRead);
/* Add 2172 Rel 1.0 */
// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function CstConvertError () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to CstConvertError()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   ----------------------------------    ====++++====")
#pragma message("  ====++++====   Op_ReadOepFile_Debug() is ENABLED     ====++++====\z")
#define Op_ReadOepFile(offulFileSeek, pTableHeadAddress, ulSizeofRead) Op_ReadOepFile_Debug(offulFileSeek, pTableHeadAddress, ulSizeofRead, __FILE__, __LINE__)
SHORT  Op_ReadOepFile_Debug(ULONG  offulFileSeek, VOID *pTableHeadAddress, ULONG ulSizeofRead, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
SHORT   Op_ReadOepFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, ULONG ulSizeofRead);
#endif
SHORT   Op_ReadCpnFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, ULONG  ulSizeofRead);
SHORT   Op_ReadCstrFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, ULONG  ulSizeofRead);
VOID    Op_WriteDeptFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, USHORT cusSizeofRead);
/* Add 2172 Rel 1.0 */
VOID    Op_WriteOepFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, USHORT cusSizeofRead);
VOID    Op_WriteCpnFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, USHORT cusSizeofRead);
VOID    Op_WriteCstrFile(ULONG  offulFileSeek, VOID *pTableHeadAddress, USHORT cusSizeofRead);
SHORT   Op_CompSubIndex(OP_SUBINDEXENTRY *pArg, VOID *offulOfIndexTbl);
SHORT   Op_SubIndexSer(UCHAR uchSubIndexId, OP_SUBINDEXENTRY *SubIndexEntry, ULONG *ulHitPoint);
VOID    Op_AllFileCreate(VOID);
VOID    Op_PluAbort(USHORT usCode);
/* Add 2172 Rel 1.0 */
SHORT   Op_DeptOpenFile(VOID);
/* Add 2172 Rel 1.0 */
SHORT   Op_OepOpenFile(USHORT  usOepTableNo);
SHORT   Op_CpnOpenFile(VOID);
SHORT   Op_CstrOpenFile(VOID);

/* === New Functions - Promotional Pricing Item (Release 3.1) BEGIN === */
SHORT   Op_PpiOpenFile(VOID);
VOID    Op_ClosePpiFileReleaseSem(VOID);
SHORT   Op_ReadPpiFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG ulSizeofRead);
VOID    Op_WritePpiFile(ULONG offulFileSeek,
                        VOID  *pTableHeadAddress,
                        USHORT usSizeofWrite);
/* === New Functions - Promotional Pricing Item (Release 3.1) END === */
/* MLD Mnemonics File, V3.3 */
SHORT   Op_MldOpenFile(VOID);
VOID    Op_CloseMldFileReleaseSem(VOID);
SHORT   Op_ReadMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG ulSizeofRead);
VOID    Op_WriteMldFile(ULONG offulFileSeek,
                        VOID  *pTableHeadAddress,
                        USHORT usSizeofWrite);
SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed, 
                     UCHAR         uchModeType, 
                     USHORT        usDeptNo, 
                     USHORT        *usParaBlockNo);
/* Add 2172 Rel 1.0 */
SHORT   Op_OepIndex(OPOEP_FILEHED *OepFileHed, 
					USHORT        usFilePart,
                    UCHAR         uchModeType, 
                    USHORT        usGroupNo, 
                    TCHAR         *uchPluNumber,
                    UCHAR         uchAdjecive,
                    ULONG         *poffulSearchPoint);
VOID    Op_DeptParaWrite(OPDEPT_FILEHED  *DeptFileHed, 
                        USHORT          usParaBlockNo, 
                        OPDEPT_PARAENTRY *SubIndexEntry);
VOID    Op_DeptParaRead(OPDEPT_FILEHED   *DeptFileHed, 
                       USHORT          usParaBlockNo, 
                       OPDEPT_PARAENTRY *SubIndexEntry);
SHORT   Op_GetDept(OPDEPT_FILEHED *DeptFileHed, 
                  ULONG         culCounter, 
                  DEPTIF         *pArg);
SHORT   Op_DeptCompIndex(VOID *usDeptNo, 
                     VOID *pusCompKey);
/* Add 2172 Rel 1.0 */
SHORT   Op_OepCompIndex(VOID *uchGroupNo, 
                     VOID *pusCompKey);
VOID    Op_DeptInsertTable(USHORT offusTableHeadAddress, 
                       USHORT usTableSize, 
                       USHORT offusInsertPoint,    
                       VOID   *pInsertData, 
                       USHORT usDataSize );
/* Add 2172 Rel 1.0 */
VOID    Op_OepInsertTable(ULONG offulTableHeadAddress, 
                       ULONG ulTableSize, 
                       ULONG  offulInsertPoint,    
                       VOID   *pInsertData, 
                       USHORT usDataSize );
SHORT   Op_DeptSearchFile(USHORT offusTableHeadAddress, 
                      USHORT usTableSize, 
                      USHORT *offusSearchPoint, 
                      VOID   *pSearchData, 
                      USHORT usSearchDataSize, 
                      SHORT  (*Comp)(VOID *pKey, VOID *pusCompKey));
/* Add 2172 Rel 1.0 */
SHORT   Op_OepSearchFile(ULONG offulTableHeadAddress, 
                      ULONG  ulTableSize, 
                      ULONG  *offulSearchPoint, 
                      VOID   *pSearchData, 
                      USHORT usSearchDataSize, 
                      SHORT  (*Comp)(VOID *pKey, VOID *pusCompKey));
SHORT   Op_DeptEmpBlkGet(OPDEPT_FILEHED *DeptFileHed, 
                     USHORT        *usParaBlockNo);
VOID    Op_DeptBlkEmp(OPDEPT_FILEHED *DeptFileHed, 
                  USHORT        usParaBlockNo);
VOID    Op_DeptDeleteTable(USHORT offusTableHeadAddress, 
                       USHORT usTableSize, 
                       USHORT offusDeletePoint, 
                       USHORT DeleteDataSize );
SHORT   Op_DeptIndexDel(OPDEPT_FILEHED  *DeptFileHed, 
                    USHORT         usDeptNo); 
VOID    Op_DeptNoGet(OPDEPT_FILEHED *DeptileHed, 
                 ULONG         culCounter, 
                 USHORT        *usDeptNo, 
                 USHORT        *usParaBlockNo);

SHORT   OpReqHostsIP(USHORT usFunc, USHORT usLockHnd);
SHORT   OpResHostsIP(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd);

/*
===========================================================================
    MEMORY
===========================================================================
*/
extern USHORT  husOpSem;           /* Semaphore for Operational Paramater */
extern USHORT  husOpLockHandle;    /* Lock Handle save area */
extern SHORT   hsOpPluFileHandle;  /* for plu file handle */
extern SHORT   hsOpDeptFileHandle; /* for dept file handle */
/* Add 2172 Rel 1.0 */
extern SHORT   hsOpOepFileHandle;/* for oep file handle */
extern SHORT   hsOpCpnFileHandle;  /* for Combination Coupon file handle */
extern SHORT   hsOpCstrFileHandle; /* for Control String file handle */
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
extern SHORT   hsOpPpiFileHandle; /* for PPI file handle */
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */
/* MLD Mnemonics File, V3.3 */
extern SHORT   hsOpMldFileHandle; /* for MLD Mnemonics file handle */

/*-----------------------------------------------
     File Names for the Database folder files.
	 See file nwop.c for the definitions.
-----------------------------------------------*/
extern TCHAR CONST auchOP_PLU_OLD[];
extern TCHAR CONST auchOP_PLU[];
extern TCHAR CONST auchOP_DEPT[];
extern TCHAR CONST auchOP_OEP[];     /* Add 2172 Rel 1.0 */
extern TCHAR CONST auchOP_CPN[];
extern TCHAR CONST auchOP_CSTR[];
extern TCHAR CONST auchOP_PPI[];     /* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
extern TCHAR CONST auchOP_MLD[];     /* Mld Mnemonics File, V3.3 */
extern TCHAR CONST auchOP_RSN[];     /* Reason Code file for Rel 2.2.1 and Amtrak/VCS */

/* ====== End of Header ========= */