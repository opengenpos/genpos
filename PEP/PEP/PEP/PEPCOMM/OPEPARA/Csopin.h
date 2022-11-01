/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
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
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*
*   Date      :Ver.Rev :NAME       :Description
*   Nov-08-93 :00.00.01:H.YAMAGUCHI:Increase 3000 PLU for HOTEL
*   Mar-07-95 :03.00.00:H.Ishida   :Add OPE & Combination Coupon
*   Mar-14-95 :03.00.00:H.Ishida   :Add Print Priorty by OPDEPT_PARA
*   Apr-12-95 :03.00.00:H.Ishida   :Add Control String
*   Sep-09-98 :03.03.00:A.Mitsui   :Add V3.3 (MLD Mnemonic File)
*   Nov-29-99 :        :K.Yannagida:NCR2172 (DEPT MAX SIZE 50->250)
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

#define CSOPH_INCLUDED

/*
===========================================================================
    DEFINE
===========================================================================
*/

/*** NCR2172
#define  OP_DEPT_NUMBER_OF_MAX      50
***/
#define  OP_DEPT_NUMBER_OF_MAX     250

#define  OP_CPN_NUMBER_OF_MAX      300  //100 JHHJ  03-15-04
#define  OP_PPI_NUMBER_OF_MAX      300 	//100 ESMITH
#define  OP_COUNTER_OF_SEM          32
#define  OP_PLU_INDEXBLK_SIZE        2
#define  OPPLU_PARA_EMPTY_TABLE_SIZE  (OP_NUMBER_OF_MAX_PLU / 8) + (7 + (OP_NUMBER_OF_MAX_PLU % 8)) / 8
#define  OP_HALO_SIZE                1
#define  OP_CONTROL_CODE_SIZE        6
#define  OP_MNEMONIC_SIZE           12
#define  OP_FILE_HEAD_POSITION       0L

/*** NCR2172
#define  OP_DEPT_FILE_HEAD_SIZE      2
***/
/*** NCR2172 ***/
#define  OP_DEPT_INDEXBLK_SIZE        2
#define  OPDEPT_PARA_EMPTY_TABLE_SIZE  (OP_DEPT_NUMBER_OF_MAX / 8) + (7 + (OP_DEPT_NUMBER_OF_MAX % 8)) / 8

#define  OP_DEPT_NAME_SIZE          20
#define  OP_DEPT_HALO_SIZE           1

#define  OP_PPI_INDEXBLK_SIZE        2
#define  OPPPI_PARA_EMPTY_TABLE_SIZE  (OP_MAX_PPI_NO / 8) + (7 + (OP_MAX_PPI_NO % 8)) / 8

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
#define  OP_BACKUP_WORK_SIZE     450

#define  OP_BROADCAST_SIZE       450
#define  OP_CLASS_EOT           0xff

#define RFL_HIT                 0
#define OP_EOT                  0
#define OP_CONTINUE             1
#define OP_END                  2

/* BackUp File Define */

#define OP_DEPT_FILE          203
#define OP_PLU_FILE           204
#define OP_CPN_FILE           205
#define OP_CSTR_FILE          206
/* === Add Promotional Pricing Item (Release 3.1) BEGIN === */
#define OP_PPI_FILE           207   /* PPI File Backup, R3.1 */
/* === Add Promotional Pricing Item (Release 3.1) END === */

#define OP_MLD_FILE           208   /* MLD File Backup, V3.3 */

/*
===========================================================================
    STRUCT
===========================================================================
*/
#pragma pack(push, 1)

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
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTblBlk;
   ULONG    offulOfIndexTblRept;
   ULONG    offulOfIndexTblDept;
   ULONG    offulOfParaEmpTbl;
   ULONG    offulOfParaTbl;
   ULONG    offulOfWork;
} OPPLU_FILEHED_R25;

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

/* Add 2172 Rel 1.0 Saratoga */
typedef  struct {
   ULONG    ulOepFileSize;
   USHORT   usNumberOfMaxOep;
   USHORT   usNumberOfResidentOep;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTbl2;
   USHORT   usNumberOfResidentOep2;
} OPOEP_FILEHED;


/*** NCR2172 ***/
/*
typedef  struct {
   ULONG    ulDeptFileSize;
   USHORT   usNumberOfMaxDept;
   USHORT   usNumberOfResidentDept;
   ULONG    offulOfIndexTbl;
   ULONG    offulOfIndexTblBlk;
   ULONG    offulOfParaEmpTbl;
   ULONG    offulOfParaTbl;
   ULONG    offulOfWork;
} OPDEPT_FILEHED;
*/

typedef  struct {
   ULONG    ulDeptFileSize;
   USHORT   usNumberOfMaxDept;
   USHORT   usNumberOfResidentDept;
   USHORT   offusOffsetOfIndexTbl;
   USHORT   offusOffsetOfParaEmpTbl;
   USHORT   offusOffsetOfParaTbl;
} OPDEPT_FILEHED_THO;


/*
typedef  struct {
   USHORT   usNumberOfDept;
} OPDEPT_FILEHED;

typedef  struct {
   USHORT   usNumberOfCpn;
} OPCPN_FILEHED;

typedef  struct {
   USHORT   usNumberOfCstr;
   USHORT   usOffsetOfTransCstr;
} OPCSTR_FILEHED;

typedef  struct {
   USHORT   usNumberOfPpi;
} OPPPI_FILEHED;
*/

typedef  struct {
/*   USHORT   usPluNo; */
   USHORT   auchPluNo[OP_PLU_LEN];  /* Saratoga */
   UCHAR    uchAdjective;
} OPPLU_INDEXENTRY;

typedef  struct {
   UCHAR    uchReportNo;
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYREPORT;

typedef struct {
   UCHAR    uchDeptNo;
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYDEPT;

typedef struct {
   UCHAR    uchGroupNo;
   USHORT   usPluIndexOffset;
} OPPLU_INDEXENTRYBYGROUPNO;

typedef struct {
   UCHAR          uchDept;
   UCHAR          uchRept;
   UCHAR          auchContOther[5];
/*   UCHAR          auchContOther[6]; */
} OPPLU_CONTROL;

typedef  struct {
   USHORT   usDeptNo;
} OPDEPT_INDEXENTRY;    /* 2172 */

/* Saratoga */
typedef  struct {
   USHORT  usGroupNo;
   WCHAR   auchPluNo[OP_PLU_OEP_LEN];
   UCHAR   uchAdjective;
} OPOEP_INDEXENTRY;

typedef struct {
   UCHAR          auchControlCode[OP_CONTROL_CODE_SIZE];
   UCHAR          uchHalo[OP_HALO_SIZE];
   WCHAR          auchMnemonic[OP_MNEMONIC_SIZE];
   UCHAR          uchPrintPriority;                    /* Print Priority */
} OPDEPT_PARA;

typedef struct {
   UCHAR          auchControlCode[OP_CONTROL_CODE_SIZE];
   UCHAR          uchHalo[OP_HALO_SIZE];
   UCHAR          auchMnemonic[OP_MNEMONIC_SIZE];
} OPDEPT_PARA_OLD;

typedef struct {
   UCHAR          uchClass;
   ULONG          ulOffset;
} OPBRD_PARA;

typedef  struct {
   UCHAR    uchTableNo;
   USHORT   usIndexNo;
} OP_SUBINDEXENTRY;


/* MLD Mnemonics File, V3.3 */
typedef  struct {
   USHORT   usNumberOfAddress;
} OPMLD_FILEHED;

#pragma pack(pop)
/*
===========================================================================
   PROTOTYPE
===========================================================================
*/

SHORT   Op_LockCheck(USHORT usLockHnd);
VOID    Op_WritePluFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        USHORT cusSizeofWrite);
VOID    Op_ClosePluFileReleaseSem(VOID);
VOID    Op_CloseDeptFileReleaseSem(VOID);
VOID    Op_CloseCpnFileReleaseSem(VOID);
VOID    Op_CloseCstrFileReleaseSem(VOID);
VOID    Op_ClosePpiFileReleaseSem(VOID);
SHORT   Op_ReadDeptFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        ULONG cusSizeofRead);
SHORT   Op_ReadCpnFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        ULONG cusSizeofRead);
SHORT   Op_ReadPluFile(ULONG  offulFileSeek,
                       VOID   *pTableHeadAddress,
                       ULONG cusSizeofWrite);
SHORT   Op_ReadCstrFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        ULONG cusSizeofWrite);
SHORT   Op_ReadPpiFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        ULONG cusSizeofWrite);
SHORT   Op_IndexDel(OPPLU_FILEHED  *PluFileHed,
                    USHORT         usPluNo,
                    UCHAR          uchPluAdj);
VOID    Op_NoGet(OPPLU_FILEHED *PluFileHed,
                 ULONG         culCounter,
                 USHORT        *usPluNo,
                 USHORT        *usParaBlockNo);
VOID    Op_WriteDeptFile(ULONG  offulFileSeek,
                         VOID   *pTableHeadAddress,
                         ULONG cusSizeofRead);
VOID    Op_WriteCpnFile(ULONG  offulFileSeek,
                         VOID   *pTableHeadAddress,
                         ULONG cusSizeofRead);
VOID    Op_WriteCstrFile(ULONG  offulFileSeek,
                         VOID   *pTableHeadAddress,
                         ULONG cusSizeofRead);
VOID    Op_WritePpiFile(ULONG  offulFileSeek,
                        VOID   *pTableHeadAddress,
                        ULONG cusSizeofRead);
SHORT   Op_Index(OPPLU_FILEHED *PluFileHed,
                 UCHAR         uchModeType,
                 USHORT        usPluNo,
                 UCHAR         uchPluAdj,
                 USHORT        *usParaBlockNo);
VOID    Op_PluParaWrite(OPPLU_FILEHED  *PluFileHed,
                        USHORT          usParaBlockNo,
                        OPPLU_PARAENTRY *SubIndexEntry);
VOID    Op_PluParaRead(OPPLU_FILEHED   *PluFileHed,
                       USHORT          usParaBlockNo,
                       OPPLU_PARAENTRY *SubIndexEntry);
SHORT   Op_GetPlu(OPPLU_FILEHED *PluFileHed,
                  ULONG         culCounter,
                  PLUIF         *pArg);
/*** NCR2172 (nwopin.c)***/
SHORT   Op_DeptIndex(OPDEPT_FILEHED *DeptFileHed,
                     UCHAR          uchModeType,
                     USHORT         usDeptNo,
                     USHORT         *pusParaBlockNo);

SHORT   Op_DeptIndexDel(OPDEPT_FILEHED *DeptFileHed,
                        USHORT         usDeptN);

VOID    Op_DeptNoGet(OPDEPT_FILEHED *OpDeptFileHed,
                     ULONG           culCounter,
                     USHORT          *usDeptNo,
                     USHORT          *usParaBlockNo );

VOID    Op_DeptParaRead(OPDEPT_FILEHED   *DeptFileHed,
                        USHORT           usParaBlockNo,
                        OPDEPT_PARAENTRY *ParaEntry);

VOID    Op_DeptParaWrite(OPDEPT_FILEHED   *DeptFileHed,
                         USHORT           usParaBlockNo,
                         OPDEPT_PARAENTRY *ParaEntry);

SHORT   Op_GetDept(OPDEPT_FILEHED *pDeptFileHed,
                   ULONG           culCounter,
                   DEPTIF          *pParaEntry);

VOID    Op_DeptInsertTable( ULONG offusTableHeadAddress,
                            USHORT usTableSize,
                            USHORT offusInsertPoint,
                            VOID   *pInsertData,
                            USHORT usInsertDataSize);

VOID    Op_DeptDeleteTable(USHORT offusTableHeadAddress,
                           USHORT usTableSize,
                           USHORT offusDeletePoint,
                           USHORT usDeleteDataSize);

SHORT   Op_DeptSearchFile(USHORT offusTableHeadAddress,
                          USHORT usTableSize,
                          USHORT *poffusSearchPoint,
                          VOID   *pSearchData,
                          USHORT usSearchDataSize,
                          SHORT (*Comp)(VOID *pKey, VOID *pusCompKey));

SHORT   Op_DeptEmpBlkGet( OPDEPT_FILEHED *DeptFileHed, USHORT *pusParaBlockNo );
VOID    Op_DeptBlkEmp( OPDEPT_FILEHED *DeptFileHed, USHORT usParaBlockNo );


/* VOID    Op_AllFileCreate(VOID); */
/* VOID    Op_PluAbort(USHORT usCode); */
/* VOID    Op_DeptAbort(USHORT usCode); */
/* VOID    Op_CpnAbort(USHORT usCode); */
/* VOID    Op_CstrAbort(USHORT usCode); */
/* VOID    Op_BakupAbort(VOID); */
/* SHORT   Op_PluOpenFile(VOID); */
/* SHORT   Op_DeptOpenFile(VOID); */
/* SHORT   Op_CpnOpenFile(VOID); */
/* SHORT   Op_CstrOpenFile(VOID); */
SHORT   Op_PpiOpenFile(VOID);
VOID    Op_PpiAbort(USHORT usCode);

/* MLD Mnemonics File, V3.3 */
SHORT   Op_MldOpenFile(VOID);
VOID    Op_CloseMldFileReleaseSem(VOID);
SHORT   Op_ReadMldFile(ULONG offulFileSeek,
                       VOID  *pTableHeadAddress,
                       ULONG usSizeofRead);
VOID    Op_WriteMldFile(ULONG offulFileSeek,
                        VOID  *pTableHeadAddress,
                        ULONG usSizeofWrite);
VOID    Op_MldAbort(USHORT usCode);
VOID    Op_WriteOepFile(ULONG  offulFileSeek,
                         VOID   *pTableHeadAddress,
                         ULONG cusSizeofRead);             /* Saratoga */
SHORT   Op_OepOpenFile(USHORT  usOepTableNo);                                /* Saratoga */
SHORT   Op_ReadOepFile(ULONG offulFileSeek,
                        VOID  *pTableHeadAddress,
                        ULONG usSizeofRead);               /* Saratoga */
SHORT   Op_OepIndex(OPOEP_FILEHED *OepFileHed,
					USHORT         usFilePart,
                    WCHAR          uchModeType,
                    UCHAR          uchGroupNo,
                    WCHAR          *uchPluNumber,
                    WCHAR          uchAdjective,
                    ULONG          *poffulSearchPoint);     /* Saratoga */
VOID    Op_CloseOepFileReleaseSem(VOID);                    /* Saratoga */
SHORT   Op_DeptCompIndex(OPDEPT_INDEXENTRY *Key, OPDEPT_INDEXENTRY *pusHitPoint);   /* Saratoga */
SHORT   Op_OepCompIndex(OPOEP_INDEXENTRY *Key, OPOEP_INDEXENTRY *pusHitPoint);  /* Saratoga */
SHORT   Op_OepSearchFile(ULONG  offulTableHeadAddress,
                         ULONG  ulTableSize,
                         ULONG  *poffulSearchPoint,
                         VOID   *pSearchData,
                         USHORT usSearchDataSize,
                         SHORT (*Comp)(VOID *pKey, VOID *pusCompKey));          /* Saratoga */
VOID   Op_OepInsertTable( ULONG  offulTableHeadAddress,
                          ULONG  ulTableSize,
                          ULONG  offulInsertPoint,
                          VOID   *pInsertData,
                          USHORT usInsertDataSize);         /* Saratoga */

/*
===========================================================================
    MEMORY
===========================================================================
*/
extern USHORT  husOpSem;				/* Semaphore for Operational Paramater */
extern USHORT  husOpLockHandle;			/* Lock Handle save area */
#if defined(POSSIBLE_DEAD_CODE)
extern SHORT   hsOpPluIndexFileHandle;	/* for plu index file handle */
#endif
extern SHORT   hsOpPluFileHandle;		/* for plu file handle */
extern SHORT   hsOpOldPluFileHandle;	/* for old plu file handle JHHJ*/
extern SHORT   hsOpDeptFileHandle;		/* for dept file handle */
extern SHORT   hsOpCpnFileHandle;		/* for Combination Coupon file handle */
extern SHORT   hsOpCstrFileHandle;		/* for Control String file handle */
extern SHORT   hsOpPpiFileHandle;		/* for Ppi file handle */
extern SHORT   hsOpNewPluFileHandle;	/* for new plu file handle */
extern SHORT   hsOpMldFileHandle;		/* for MLD Mnemonics file handle V3.3 */
extern SHORT   hsOpOepFileHandle;		/* Saratoga */

/*-----------------------------------
    File Name and Read/Write Mode
-----------------------------------*/
extern UCHAR FARCONST auchNEW_FILE_WRITE[];
extern UCHAR FARCONST auchOLD_FILE_WRITE[];
extern UCHAR FARCONST auchOLD_FILE_READ_WRITE[];
extern UCHAR FARCONST auchOLD_FILE_READ[];
extern WCHAR FARCONST auchOP_PLU[];
extern WCHAR FARCONST auchOP_DEPT[];
extern WCHAR FARCONST auchOP_CPN[];
extern WCHAR FARCONST auchOP_CSTR[];
extern WCHAR FARCONST auchOP_PPI[];
extern UCHAR FARCONST auchFlexMem[];
extern WCHAR FARCONST auchOP_PLU_OLD[];
extern WCHAR FARCONST auchOP_PLU_IDX[];
extern WCHAR FARCONST auchOP_PLU_TMP_IDX[];
extern WCHAR FARCONST auchOP_PLU_V34[];  /* Saratoga */
extern WCHAR FARCONST auchOP_MLD[]; /* V3.3 */
extern WCHAR FARCONST auchOP_OEP[]; /* Saratoga */


/*=========== End of csopin.h ==================*/
