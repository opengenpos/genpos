/*
* ---------------------------------------------------------------------------
* Title         :   Local Header File of File Module
* Category      :   File, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   FILEL.H
* Copyright (C) :   1995, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract : The only File for PEP included this header file. This header
*           declares Structure Type Define Declarations, Define Declarations
*           and Function Prototype Declarations to use the functions of the
*           File for PEP.
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Feb-20-95 : 03.00.00 : H.ishida   : Add FILE_HEAD_VER_2_5
* Sep-13-95 : 03.00.00 : T.Nakahata : add FILE_FAT_ADDR
* Feb-01-96 : 03.00.05 : M.Suzuki   : add R3.1
* Sep-28-98 : 03.03.00 : A.Mitsui   : V3.3
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/

#ifndef FILE1_INCLUDED_DEFINE
#define FILE1_INCLUDED_DEFINE

#include <pepcomm.h>

#include    "mypifdefs.h"
#include	"csetk.h"

/************************\
|*  Define Declarations *|
\************************/
//#define FILE_MAXLEN_FILENAME    13
#define FILE_MAXLEN_TEXT        128
#define FILE_MAXLEN             256
#define FILE_MAXERROR			2048
#define FILE_TIMELEN			26
#define FILE_NEWLINE			1024

#define FILE_MADDR              0 /* 0x1F800 --> 0x21800 --> 0x21F00 */ /* V3.3 */
#define FILE_SADDR              0 /* 0x1F800 --> 0x21800 --> 0x21F00 */ /* V3.3 */
#define FILE_FAT_ADDR              7025460L   /* for ram disk driver use, (6.7MB) * 1024 * 1024 */
/* #define FILE_MADDR              0x21F00 /* 0x1F800 --> 0x21800 --> 0x21F00 */ /* V3.3 */
/* #define FILE_SADDR              0x21F00 /* 0x1F800 --> 0x21800 --> 0x21F00 */ /* V3.3 */
/* #define FILE_FAT_ADDR           0x800   / for ram disk driver use */

#define FILE_SIZE_SHORT         0x07FFF

#define FILE_NOTFOUND           0x0002  /* file not found (OpenFile) */

/* Saratoga */


#define FILE_NUM_ALLFILE          17   /* Include Temporary File (###ADD PLU Index/OEP file)*/

#define FILE_NUM_ALLFILE_HGV201   16

#define FILE_NUM_ALLFILE_HGV10  16   /* Include Temporary File (###ADD PLU Index/OEP file)*/
#define FILE_NUM_ALLFILE_V34    14   /* Include Temporary File (PLU, ETK, CAS)*/
#define FILE_NUM_ALLFILE_GP     12   /* GP Ver2.0/2.1/2.2 File */
#define FILE_NUM_DIVFILE        10   /* add MLD file (V3.3) */
#define FILE_NUM_TERM           2
#define FILE_NUM_VER            1
//#define FILE_NUM_KB             4    // Number of key board types see IDS_FILE_KEY_CONVENT
/* Moved to File.h, so that other modules can use this number, for instance, P97 also uses this information JHHJ*/
#define FILE_NUM_RAM            3
#define FILE_NUM_ERRMSG         16
#define FILE_NUM_CAP            7
#define FILE_NUM_DELERR         23

/* ###ADD Saratoga */
#define FILE_NUM_MEMORY          6

/* ----- dialog idintifiers ----- */
#define FILE_INIT_PROC          0
#define FILE_OPEN_PROC          1
#define FILE_SAVEAS_PROC        2
#define FILE_DELETE_PROC        3

/* */

/* table offset for error message   */
#define FILE_TBLERR_CONT        0
#define FILE_TBLERR_OVER        1
#define FILE_TBLERR_OPEN        2
#define FILE_TBLERR_SAVE        3
#define FILE_TBLERR_EXIST       4
#define FILE_TBLERR_INVALID     5
#define FILE_TBLERR_CHOOSE      6
#define FILE_TBLERR_EXIT1       7
#define FILE_TBLERR_NOTFILE     8
#define FILE_TBLERR_LOAD        9
#define FILE_TBLERR_EXIT2       10
#define FILE_TBLERR_CREATE      11
#define FILE_TBLERR_SATELLITE   12
#define FILE_TBLERR_MASTER      13
#define FILE_TBLERR_DELETE      14
#define FILE_TBLERR_DELMSG      15

/* ----- ID for delete error message ----- */
#define FILE_DELERR_MAX         21

/* table offset for caption */
#define FILE_TBLCAP_NEW         0
#define FILE_TBLCAP_OPEN        1
#define FILE_TBLCAP_SAVE        2
#define FILE_TBLCAP_SAVEAS      3
#define FILE_TBLCAP_CONF        4
#define FILE_TBLCAP_PEP         5
#define FILE_TBLCAP_DELETE      6

/* ----- offset of a drive name ----- */
#define FILE_DRVNAME_OFS        2

/* ----- return value for file header check -----
	These defines are used to indicate the version of
	PEP that created the PEP data file that is being
	opened.

	ULONG FileCheckHeader(HANDLE iFile)

 */
#define FILE_HEAD_INVALID       0
#define FILE_HEAD_SUCCESS       1
#define FILE_HEAD_OLDVER        2
#define FILE_HEAD_VER_2         3
#define FILE_HEAD_VER_2_5       4
#define FILE_HEAD_VER_3_0       5
#define FILE_HEAD_VER_3_1       6   /* add V3.3 */
#define FILE_HEAD_VER_3_3       7   /* Saratoga */
#define FILE_HEAD_VER_3_4       8   /* Saratoga */
#define FILE_HEAD_VER_GP20		9   /* Saratoga */
#define FILE_HEAD_VER_GP21		10   /* Saratoga */
#define FILE_HEAD_VER_GP22		11   /* Saratoga */
#define FILE_HEAD_VER_GP30		12   /* NHPOS Release 1.x for 7448 platform */
#define FILE_HEAD_VER_GP40		13   /* NHPOS Release 2.0 Multi-Lingual */
#define FILE_HEAD_VER_GP41		14   /* NHPOS Release 2.0 Multi-Lingual with multiple layout support*/ //ESMITH LAYOUT
#define FILE_HEAD_VER_GP42		15   /* NHPOS Release 2.0 Multi-Lingual with 20 tenders, 20 totals, 600 MDC JHHJ Release .31*/
#define	FILE_HEAD_VER_GP201		16   // NHPOS Release 2.0.1 or earlier but not one of the above
#define	FILE_HEAD_VER_GP203		17   // NHPOS Release 2.0.3
#define FILE_HEAD_VER_GP210		18   // NHPOS Release 2.1.0
#define FILE_HEAD_VER_GP204		19	// NHPOS Release 2.0.4
#define FILE_HEAD_VER_GP205		20	// NHPOS Release 2.0.5
#define FILE_HEAD_VER_GP212		22   // NHPOS Release 2.1.2
#define FILE_HEAD_VER_GP220		23	// GenPOS Release 2.2.0
#define FILE_HEAD_VER_GP214		24	// GenPOS Release 2.1.4
#define FILE_HEAD_VER_GP221		25	// GenPOS Release 2.2.1 for Amtrak and VCS phase 2
#define FILE_HEAD_VER_GP222		26	// GenPOS Release 2.2.2 with EMV chip and Out of Scope Electronic Payment GENPOS_REL_020300
#define FILE_HEAD_VER_GP230		26	// GenPOS Release 2.3.0 with EMV chip and Out of Scope Electronic Payment GENPOS_REL_020300
#define FILE_HEAD_VER_GP231		27	// GenPOS Release 2.3.1 with EMV chip and Out of Scope Electronic Payment GENPOS_REL_020300, Visual Studio 2015

#define FILE_HEAD_VER_UNKNOWN  255  // NHPOS Release is unknown

// Following are used to access the version information in USHORT  usFileVersionInfo[4]
#define FILE_HEAD_VER_BUILD  (usFileVersionInfo[3])         // provides the build number
#define FILE_HEAD_VER_ISSUE  (usFileVersionInfo[2])  // provides the issue number
#define FILE_HEAD_VER_MINOR  (usFileVersionInfo[1])  // provides the minor version number
#define FILE_HEAD_VER_MAJOR  (usFileVersionInfo[0])  // provides the major version number



#define FILE_TERM_MASTER        0   /* terminal type master(file config)    */
#define FILE_TERM_SATTELITE     1   /* terminal type sattelite(file config) */

#define OLD_FLEXMEM_SIZE        3   /* struct FLEXMEM old size, Saratoga*/
#define NEW_FLEXMEM_SIZE        5   /* struct FLEXMEM new size, Saratoga*/

#define FILE_BITMASK         0x01   /* for bit mask                     */
#define FILE_TAGSTS_ADDR        1   /* Coupon Target Status Address     */

/* ------ Error Codes for File Conversion ------ */
#define BLNKFLDERR				WIDE("WBLKFLD\n")
#define	DATARNGERR				WIDE("EDTARNG\n")
#define	DATAINCERR				WIDE("EDTAINC\n")
#define	MNEMNOTERR				WIDE("WMNTUSE\n")
#define BLNKMNEERR				WIDE("BLNKMNE\n")
#define FORMATSTRNG				WIDE("**********************************************\n")
#define	NOERRMESSG				WIDE("You have no errors within the PEP conversion\n")
#define	ERRORMESSG				WIDE("There were some erros when converting the PEP file see below for details")



#define LOGO_MAXLEN_FILENAME    13
#define LOGO_MAXLEN_TEXT        128
#define LOGO_MAXLEN             256

/* ----- dialog idintifiers ----- */
#define LOGO_INIT_PROC          0
#define LOGO_FILE_PROC          1

/* ----- offset of a drive name ----- */
#define LOGO_DRVNAME_OFS        2

/****************************************\
|*  Structure Type Define Declarations  *|
\****************************************/
//RJC added to check if memory size is packed or not
#pragma pack(push, 1)

#define FILE_LEN_ID 10
typedef struct {
    WCHAR    szName[FILE_LEN_ID];
    WCHAR    szClass[FILE_LEN_ID];
    WCHAR    szModel[FILE_LEN_ID];
    WCHAR    szVer[FILE_LEN_ID];
    WCHAR    szRptName[PEP_FILENAME_LEN];
	WCHAR	 szTouchLayoutName[PEP_FILENAME_LEN];  //ESMITH LAYOUT
	WCHAR	 szKeyboardLayoutName[PEP_FILENAME_LEN];  //ESMITH LAYOUT
	WCHAR    szIconFileDirectory[PEP_FILENAME_LEN];	//JHALL ICON TRANSFER
} FILEID;


typedef struct {
    UCHAR    szName[FILE_LEN_ID];
    UCHAR    szClass[FILE_LEN_ID];
    UCHAR    szModel[FILE_LEN_ID];
    UCHAR    szVer[FILE_LEN_ID];
    UCHAR    szRptName[PEP_FILENAME_LEN];
} FILEID_OLD;//Versions Prior to Multilingual

typedef struct {
    LONG    lOffset;
    LONG    lLen;
} FROOM;

typedef struct {
    FROOM   Para;
    FROOM   Config;
    FROOM   Trans;
} FMEMORY;

typedef struct {
    FROOM   aRoom[FILE_NUM_ALLFILE];
} FFILE;

typedef struct {
    FROOM   aRoom[FILE_NUM_ALLFILE_HGV201];
} FFILE_HGV201203;

typedef struct {
    FILEID  Fid;
    FMEMORY Mem;
    FFILE   File;
	/*In order to add the logo path to the file memory, we must add it here
	because this is where the extra space was reserved. */
	WCHAR	 szReceiptLogoName[PEP_FILENAME_LEN];
} FHEADER;

typedef struct {
    WCHAR    szName[FILE_LEN_ID];
    WCHAR    szClass[FILE_LEN_ID];
    WCHAR    szModel[FILE_LEN_ID];
    WCHAR    szVer[FILE_LEN_ID];
    WCHAR    szRptName[PEP_FILENAME_LEN];
	WCHAR	 szTouchLayoutName[PEP_FILENAME_LEN];  //ESMITH LAYOUT
	WCHAR	 szKeyboardLayoutName[PEP_FILENAME_LEN];  //ESMITH LAYOUT
} FILEID_HGV201203;

typedef struct {
    FILEID_HGV201203  Fid;
    FMEMORY           Mem;
    FFILE_HGV201203   File;
	/*In order to add the logo path to the file memory, we must add it here
	because this is where the extra space was reserved. */
	WCHAR	 szReceiptLogoName[PEP_FILENAME_LEN];
} FHEADER_HGV201203;

/* ==================================================
	This is the pre-multiple layout FILEID and File
	header needed to migrate version 2.0 header files
	prior to the availability of multiple layout files.
	ESMITH LAYOUT
   =================================================== */
typedef struct {
    WCHAR    szName[FILE_LEN_ID];
    WCHAR    szClass[FILE_LEN_ID];
    WCHAR    szModel[FILE_LEN_ID];
    WCHAR    szVer[FILE_LEN_ID];
    WCHAR    szRptName[PEP_FILENAME_LEN];
	WCHAR	 szLayoutName[PEP_FILENAME_LEN];  //Added 11-25-03, For Layout Manager JHHJ
} FILEID_GPV40;

typedef struct {
    WCHAR    szName[FILE_LEN_ID];
    WCHAR    szClass[FILE_LEN_ID];
    WCHAR    szModel[FILE_LEN_ID];
    WCHAR    szVer[FILE_LEN_ID];
    WCHAR    szRptName[PEP_FILENAME_LEN];
	WCHAR	 szLayoutName[PEP_FILENAME_LEN];  //Added 11-25-03, For Layout Manager JHHJ
	WCHAR	 szTouchLayoutName[PEP_FILENAME_LEN]; 
} FILEID_GPV42;

typedef struct {
    FROOM   aRoom[FILE_NUM_ALLFILE_HGV10];
} FFILE_GPV40;

typedef struct {
    FILEID_GPV40  Fid;
    FMEMORY       Mem;
    FFILE_GPV40   File;
} FHEADER_GPV40;

//----------------------------------------------------------
// Following are file information for NHPOS Release 1.x
// These structures are used for conversion from NHPOS Release 1.x
// data file to NHPOS Release 2.0, Multi-lingual

typedef struct {
    UCHAR    szName[FILE_LEN_ID];
    UCHAR    szClass[FILE_LEN_ID];
    UCHAR    szModel[FILE_LEN_ID];
    UCHAR    szVer[FILE_LEN_ID];
    UCHAR    szRptName[PEP_FILENAME_LEN];
} FILEID_HGV10;

typedef struct {
    LONG    lOffset;
    LONG    lLen;
} FROOM_HGV10;

typedef struct {
    FROOM_HGV10   Para;
    FROOM_HGV10   Config;
    FROOM_HGV10   Trans;
} FMEMORY_HGV10;

typedef struct {
    FROOM_HGV10   aRoom[FILE_NUM_ALLFILE_HGV10];
} FFILE_HGV10;

typedef struct {
    FILEID_HGV10  Fid;
    FMEMORY       Mem;
    FFILE_HGV10   File;
} FHEADER_HGV10;


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

#define ETK_MAX_NAME_SIZE_HGV10		 16		/* Max ETK Mnemonic*/

typedef struct {
    ULONG    ulEmployeeNo;           /* Employee Number */
    ETK_JOB  EtkJC;                  /* Job code */
    UCHAR    auchEmployeeName[ETK_MAX_NAME_SIZE_HGV10];    /* Employee Name, R3.1 */
} ETK_INDEX_HGV10;

typedef struct {
    USHORT    usEmployeeNo;           /* Employee Number */
    ETK_JOB  EtkJC;                   /* Job code */
} ETK_INDEX_OLD;

//--------------------------------------------------

typedef struct {
    UCHAR    szName[FILE_LEN_ID];
    UCHAR    szClass[FILE_LEN_ID];
    UCHAR    szModel[FILE_LEN_ID];
    UCHAR    szVer[FILE_LEN_ID];
    UCHAR    szRptName[PEP_FILENAME_LEN];    /* V3.3 */
} FILEIDOLD;        /* This File Header is V3.1 File */

typedef struct {
    FILEIDOLD  Fid;
    FMEMORY Mem;
    FFILE   File;
} FHEADEROLD;

/* Saratoga */

typedef struct {
    UCHAR    szName[FILE_LEN_ID];
    UCHAR    szClass[FILE_LEN_ID];
    UCHAR    szModel[FILE_LEN_ID];
    UCHAR    szVer[FILE_LEN_ID];
    UCHAR    szRptName[PEP_FILENAME_LEN];    /* V3.3 */
} FILEID_V34;        /* This File Header is V3.1 File */

typedef struct {
    LONG    lOffset;
    LONG    lLen;
} FROOM_V34;

typedef struct {
    FROOM   Para;
    FROOM   Config;
    FROOM   Trans;
} FMEMORY_V34;

typedef struct {
    FROOM   aRoom[FILE_NUM_DIVFILE];
} FFILE_V34;

typedef struct {
    FILEID_V34  Fid;
    FMEMORY_V34 Mem;
    FFILE_V34   File;
} FHEADER_V34;

#define FILEGP_LEN_ID 10

typedef struct {
    LONG    lOffset;
    LONG    lLen;
} FROOM_GP;

typedef struct {
    UCHAR    szName[FILEGP_LEN_ID];
    UCHAR    szClass[FILEGP_LEN_ID];
    UCHAR    szModel[FILEGP_LEN_ID];
    UCHAR    szVer[FILEGP_LEN_ID];
} FILEID_GP;

typedef struct {
    FROOM_GP   Para;
    FROOM_GP   Config;
    FROOM_GP   Trans;
} FMEMORY_GP;

typedef struct {
    FROOM_GP   aRoom[FILE_NUM_ALLFILE_GP];
} FFILE_GP;

typedef struct {
    FILEID_GP     Fid;
    FMEMORY_GP    Mem;
    FFILE_GP      File;
} FHEADER_GP;

typedef struct {
    FILEID_GPV42  Fid;
    FMEMORY Mem;
    FFILE   File;
	/*In order to add the logo path to the file memory, we must add it here
	because this is where the extra space was reserved. */
	WCHAR	 szReceiptLogoName[PEP_FILENAME_LEN];
} FHEADER_GPV42;

typedef struct {
	char *convError[FILE_MAXLEN];
	char *convWarning[FILE_MAXLEN];
	char *convInformation[FILE_MAXLEN];
	CONST VOID* convMsg;
} CONV;

//RJC added to check if memory size is packed or not
#pragma pack (pop)

ULONG  ulFileVersionInfo;

/************************************\
|*  Function Prototype Declarations *|
\************************************/
/* Dialog Procedure */
BOOL WINAPI FileOpenDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI FileSaveAsDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI FileConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI FileDeleteDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL FileBind(HANDLE, HWND, HANDLE);
BOOL FileDivide(HWND, HANDLE);
ULONG FileCheckHeader(HANDLE);

/* the edit functions   */
BOOL FileFromEachFile(HANDLE iFile, WCHAR *paszFileName, LONG *plSize);
BOOL FileToEachFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL FileToDeptFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL FileHGV203To210DeptFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL File30ToDeptFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL FileToDept34File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); /* Saratoga */
BOOL FileToDeptHGV10File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); /* Saratoga */
BOOL FileToPpiFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);
BOOL FileToCpn34File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);  /* Saratoga */
BOOL FileToCpn210File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName);
BOOL FileToCpn203File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //RLZ ACC
BOOL FileToCashierFile_210(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);
BOOL FileToCashierFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);
BOOL FileToPlu203File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //RLZ ACC
//#if defined(POSSIBLE_DEAD_CODE)
BOOL FileToPlu210File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //RLZ ACC
//#endif
BOOL FileHGV10ToDeptFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL FileToMLD34File(HANDLE iFile, LONG lOffset, LONG lSize);                     /* Saratoga */
BOOL FileHGV10ToDeptFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName);
BOOL OpETKMigrateHGV10File(HANDLE iFile,WCHAR* pszFileName);
BOOL FileToMLDHGV10File(HANDLE iFile, LONG lOffset, LONG lSize);
BOOL FileToCpnHGV10File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); /*JHHJ for R2- migration*/
BOOL FileToDirectFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName);
BOOL GPFileToDeptFile(LONG lSize); //SR 210 JHHJ 1-27-04
BOOL FileHGV10ToCSTRFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //ESMITH SR 160
BOOL FileToPPI210File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //CSMALL
BOOL FileToCAS210File(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR *pszFileName); //PDINU

UINT    FileGetErrMsg(UINT);

BOOL    FileOpenProc(HANDLE, HWND);
BOOL    FileSaveAsProc(HANDLE, HWND);
BOOL    FileDeleteProc(HANDLE, HWND);

VOID    PLUConversion(WCHAR*);

BOOL	CurrentLogoFile(HANDLE, HWND);

BOOL  FileToCouponFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);
BOOL  FileToEmployeeTimeFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);
BOOL  FileToDepartmentFile(HANDLE iFile, LONG lOffset, LONG lSize, WCHAR* pszFileName, int iStartConvert);

// added 2010-01-08 to resolve compiler warnings in fileopen.c
#ifdef FILE_MIGRATION_PROTOTYPES
// we include the following only if we are using the conversion functions.
// You will probably have to also have #define    NO_PARA_DEFINES
// near the top of the include file list in order to turn off the defines of
// the macros for accessing Para members in pararam.h
#include "UNI_2_0_0.h"
#include "UNI_2_0_1.h"
#include "UNI_2_0_3.h"
#include "UNI_2_0_5.h"
#include "UNI_2_1_0.h"
#include "UNI_2_1_2.h"
#include "UNI_2_2_0.h"

BOOL  FileMigrateGP_40_Files (HANDLE iFile, UNINIRAM_HGV_2_0_0_x *pOldPara, int fRet);
BOOL  FileMigrateGP_40_210_Para (UNINIRAM_HGV_2_0_0_x *pOldPara, UNINIRAM *pNewPara);
BOOL  FileMigrateGP_42_Files (HANDLE iFile, UNINIRAM_HGV_2_0_0 *pOldPara, int fRet);
BOOL  FileMigrateGP_42_210_Para (UNINIRAM_HGV_2_0_0 *pOldPara, UNINIRAM *pNewPara);

BOOL  FileMigrateGP_201_212_Para (UNINIRAM_HGV_2_0_1 *pOldPara, UNINIRAM_HGV_2_1_2 *pNewPara);
BOOL  FileMigrateGP_203_Files (HANDLE iFile, UNINIRAM_HGV_2_0_3 *pOldPara, int fRet);
BOOL  FileMigrateGP_203_212_Para (UNINIRAM_HGV_2_0_3 *pOldPara, UNINIRAM_HGV_2_1_2 *pNewPara);
BOOL  FileMigrateGP_205_Files (HANDLE iFile, UNINIRAM_HGV_2_0_5 *pOldPara, int fRet);
BOOL  FileMigrateGP_205_212_Para (UNINIRAM_HGV_2_0_5 *pOldPara, UNINIRAM_HGV_2_1_2 *pNewPara);
BOOL  FileMigrateGP_210_Files (HANDLE iFile, UNINIRAM_HGV_2_1_0 *pOldPara, int fRet);
BOOL  FileMigrateGP_210_220_Para (UNINIRAM_HGV_2_1_0 *pOldPara, UNINIRAM_HGV_2_2_0 *pNewPara);
BOOL  FileMigrateGP_212_Files (HANDLE iFile, UNINIRAM_HGV_2_1_2 *pOldPara, int fRet);
BOOL  FileMigrateGP_212_220_Para (UNINIRAM_HGV_2_1_2 *pOldPara, UNINIRAM_HGV_2_2_0 *pNewPara);
BOOL  FileMigrateGP_220_221_Para (UNINIRAM_HGV_2_2_0 *pOldPara, UNINIRAM *pNewPara);
#endif

/************************************\
|*  External Work Area Declarations *|
\************************************/
extern  WCHAR       szFileSaveName[FILE_MAXLEN];
extern  FCONFIG     Fconfig;
extern  FHEADER     Fheader;

extern  USHORT       usFileVersionInfo[4];

#endif
/* ===== End of FILEL.H ===== */

