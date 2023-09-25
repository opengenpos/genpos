/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 2170     *             NCR Corporation, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993				||
||    <|\/~               ~\/|>												||
||   _/^\_                 _/^\_											||
||																			||
\****************************************************************************/

/*==========================================================================*\
*	Title:
*	Category:
*	Program Name:
* ---------------------------------------------------------------------------
*	Compiler:		MS-C Ver. 6.00 A by Microsoft Corp.              
*	Memory Model:	Medium Model
*	Options:		/c /AM /G1s /Os /Za /Zp /W4
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*  Sep/16/93 |			| Correct a glitch in MnpRemoveRecord()	|
*** 2172 Rel1.0
*            | 1.00.00  | initial
*  Jan/18/00 |          | AddRecIndex modified                  | K.Iwata   
\*==========================================================================*/

/*==========================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+					I N C L U D E    F I L E s								+
;+																			+
\*==========================================================================*/

#include	<memory.h>							/* memxxx() header			*/
#include	<ecr.h>								/* 2170 system header		*/
#include    <rfl.h>                             /* reentrant library header */
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/
#include	"AW_Interfaces.h"

/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

#ifdef PEP
#define     RECIDX_BUFFER       200         /* buffer size for index search */
#else
#define     RECIDX_BUFFER       100         /* buffer size for index search */
#endif

/* --- mirror file definition --- */

#define		MASK_MIRROR_HANDLE	0x00FF			/* mask for file handle		*/
												/* to mirror file ID		*/
#define		TOMIRROR(h, n)  (USHORT)(((h) & MASK_MIRROR_HANDLE) | ((n) << 8))
												/* is mirror file or not	*/
#define		ISMIRROR(f)		(BOOL)(((f) & ~MASK_MIRROR_HANDLE) ? TRUE : FALSE)
												/* handle of mirror file	*/
#define		HFMIRROR(f)		(USHORT)((f) & MASK_MIRROR_HANDLE)
												/* file no. of mirror file	*/
#define		FNMIRROR(f)		(USHORT)((USHORT)(f) >> 8)

/* --- mirror file ID adjustment --- */

#define		ADJUSTFILE(f)		(MnpIsMirrorFile(f) ? MnpFileOfMirror(f) : f)


/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- seq. access control tables --- */

static	SEQMNP	aManipBlock[MAX_USERS];

/* --- access control as default --- */

static	SEQMNP	aDefaultBlock[DEFAULT_HANDLES];

/* --- file handle array --- */

static	USHORT	hPluFile;					/* handle of PLU file			*/
static	USHORT	hMixFile;					/* handle of mix-match file		*/
/*static	USHORT	hPpiFile;*/				/* handle of PPI file			*//* ### 2172 Rel1.0 */
static	USHORT	hPluMaint;					/* handle of PLU mainte. file	*/
static	USHORT	hPluIndex;					/* handle of PLU index file,R20	*/


/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/
/*							measure:	12345678							*/
// All files in this list should begin with the capital letter P.  See check in function MnpInitialize().
// The check is a sanity check on memory buffer overwrites that have been seen with loading Para. Aug-12-2014
// These names are used in struct FileInfo[].
static	WCHAR FARCONST	szPluFile[]  = WIDE("PARAM$PL");	/* PLU file name		*/
static	WCHAR FARCONST	szMixFile[]  = WIDE("PARAM$MX");	/* mix-match file		*/
/*static	UCHAR FARCONST	szPpiFile[]  = "PARAM$PP";*//* PPI file name		*//* ### DEL 2172 Rel1.0 */
static	WCHAR FARCONST	szPluMaint[] = WIDE("PARAM!PL");	/* PLU maintenance		*/
static	WCHAR FARCONST	szPluIndex[] = WIDE("PARAMIDX");	/* PLU index file,R20	*/


/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/* --- record manipulating primitives --- */

static	USHORT	AddRecHash(USHORT, VOID *, PKEYINF, PFILEHDR);
static	USHORT	AddRecord(USHORT, ULONG, WCHAR *, PFILEHDR);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	USHORT	CheckAdjective(WCHAR *, WCHAR *, PKEYINF, SHORT *);
static	USHORT	AddRecRelative(USHORT, VOID *, PKEYINF, PFILEHDR);
static	USHORT	AddRecMainte(USHORT, VOID *, PKEYINF, PFILEHDR, ULONG *);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	USHORT	AddRecIndex(USHORT, VOID *, PFILEHDR);
static	USHORT	DelRecHash(USHORT, WCHAR *, USHORT, PKEYINF, PFILEHDR);
static	USHORT	DelRecRelative(USHORT, WCHAR *, PKEYINF, PFILEHDR);
static	USHORT	DelRecIndex(USHORT, VOID *, PFILEHDR);
static	USHORT	ReadRecHash(USHORT, WCHAR *, WCHAR *, USHORT, PKEYINF, PFILEHDR, ULONG *);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	USHORT	ReadRecRelative(USHORT, WCHAR *, WCHAR *, PKEYINF, PFILEHDR, ULONG *);		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static  USHORT	ReadRecIndex(USHORT, WCHAR *, WCHAR *, PFILEHDR, ULONG *);					/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	USHORT	TraceRecHash(USHORT, WCHAR *, PKEYINF, PFILEHDR, PSEQMNP);
static	USHORT	TraceRecRelative(USHORT, WCHAR *, PKEYINF, PFILEHDR, PSEQMNP);
static	USHORT	TraceRecMainte(USHORT, WCHAR *, PKEYINF, PFILEHDR, PSEQMNP);
static  USHORT	TraceRecIndex(USHORT, USHORT, USHORT, WCHAR *, PKEYINF, PFILEHDR, PSEQMNP);

/* --- misc. sub-functions --- */

static	USHORT	EvaluateFileInfo(USHORT, USHORT *, PINFTBL *);
static	PINFTBL	SearchFileInfo(USHORT);
static  PKEYINF ReplaceKeyInfo(USHORT usFile, USHORT usFileVer, PKEYINF pKey);
static	USHORT	HandleToFileNo(USHORT);
static	VOID	MoveRecordPtr(USHORT, ULONG, ULONG);			/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	VOID	DeleteRecordPtr(USHORT, ULONG, ULONG, BOOL);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	ULONG	ComputeHash(WCHAR FAR *, USHORT, ULONG);			/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
static	ULONG	ComputeRelative(ULONG);							/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

static  SHORT	ComputeIndex(WCHAR *, USHORT, PFILEHDR, ULONG *, WCHAR *);

SHORT    CompIndex(RECIDX *pKeyRecIdx, RECIDX *pHitPoint);
#ifndef PEP
static  SHORT   (*pComp)(VOID *pKey, VOID *pusHPoint) = CompIndex;
#endif

ULONG	ComputeHash_64(WCHAR *, USHORT, ULONG);	/* ### ADD 2172 Rel1.0 (PLU No 7 -> 13) */

/* --- forward declarations for data tables.  See actual definitions at bottom of file  --- */

KEYINF FARCONST		PluFileKey[];				/* key info. for PLU rec.	*/
KEYINF FARCONST		MixFileKey[];				/* key info. for Mix-Match	*/
/*KEYINF FARCONST		PpiFileKey[];*/			/* key info. for PPI rec.	*//*### DEL 2172 Rel1.0 */
KEYINF FARCONST		PluMaintKey[];				/* key info. for PLU mainte.*/
KEYINF FARCONST		PluIndexKey[];				/* key info. for PLU index  */
KEYINF FARCONST		Plu10FileKey[];				/* key info. for PLU rec.	*/
RECDIC FARCONST		PluRecord[];				/* rec. dictinary for PLU	*/
RECDIC FARCONST		MixRecord[];				/* rec. dictinary for Mix-M	*/
/*RECDIC FARCONST		PpiRecord[];*/			/* rec. dictinary for PPI	*//* ### DEL 2172 Rel1.0 */
RECDIC FARCONST		PluMainte[];				/* rec. dictinary for PLU M	*/
RECDIC FARCONST		IndexRecord[];				/* rec. dictinary for Index	*/
RECDIC FARCONST		PluRecordR10[];				/* rec. dictinary for PLU	*/

/**
;========================================================================
;
;   function : File relation table
;
;   format : INFTBL		FileInfo[i];
;
;   input  : USHORT		i;			- table offset
;
;   output : USHORT		FileInfo[i].usFileNo;		- file no.
;			 USHORT		FileInfo[i].usFileType;		- file type
;			 USHORT		*FileInfo[i].pusHandle;		- pointer to handle
;			 UCHAR		*FileInfo[i].pszFileName;	- pointer to file name
;
;========================================================================
**/
static INFTBL	FARCONST	FileInfo[] = {
 {	FILE_PLU,		TYPE_HASH,		&hPluFile,	szPluFile,	PluFileKey		},
 {	FILE_MIX_MATCH,	TYPE_RELATIVE,	&hMixFile,	szMixFile,	MixFileKey		},
/* {	FILE_PPI,		TYPE_RELATIVE,	&hPpiFile,	szPpiFile,	PpiFileKey		},*//* ### DEL 2172 Rel1.0 */
 {	FILE_MAINT_PLU,	TYPE_MAINTE,	&hPluMaint,	szPluMaint,	PluMaintKey		},
 {	FILE_PLU_INDEX,	TYPE_INDEX,		&hPluIndex,	szPluIndex,	PluIndexKey		},
 {	TBLEND,			TBLEND,	(USHORT *)NULL,	(WCHAR *)NULL,	(PKEYINF)NULL	},
};



/*==========================================================================*\
;+																			+
;+				P R O G R A M     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/



/**
;========================================================================
;
;   function : Initialize manipulation
;
;   format : USHORT		MnpInitialize(fRefresh);
;
;   input  : BOOL		fRefresh;		- refresh or initialize
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/
USHORT	MnpInitialize(BOOL fRefresh)
{
	/* --- initialize Sequential Access Control table --- */
	memset(aManipBlock,   0, sizeof(aManipBlock));	/* clear table w/ 0		*/
	memset(aDefaultBlock, 0, sizeof(aDefaultBlock));/* clear table w/ 0		*/

	/* --- for first initialization ? --- */
	if (! fRefresh) {
		PINFTBL	pTable;

		/* --- open existing files in advance --- */
		for (pTable = FileInfo; pTable->usFileNo != TBLEND; pTable++) {
			/* --- open it w/ read & write mode --- */
			if (pTable->pszFileName[0] != _T('P')) {
				__debugbreak();  // prep for ARM build _asm int 3;
			}
			if (PhyOpenFile(pTable->pszFileName, pTable->pusHandle, TRUE)) {
				*(pTable->pusHandle) = 0;			/* mark 0 if fail ...	*/
			}
		}
	}

	return (0);
}

/**
;========================================================================
;
;   function : Finalize manipulation
;
;   format : USHORT		MnpFinalize(VOID);
;
;   input  : nothing
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	MnpFinalize(VOID)
{
	PINFTBL	pTable;

	/* --- initialize Sequential Access Control table --- */

	memset(aManipBlock,   0, sizeof(aManipBlock));	/* clear table w/ 0		*/
	memset(aDefaultBlock, 0, sizeof(aDefaultBlock));/* clear table w/ 0		*/

	/* --- open existing files in advance --- */

	for (pTable = FileInfo; pTable->usFileNo != TBLEND; pTable++) {

		/* --- close it --- */

		if (*(pTable->pusHandle)) {				/* is opened file ?			*/
			PhyCloseFile(*(pTable->pusHandle));		/* close the file		*/
			*(pTable->pusHandle) = 0;				/* mark 0 for closed	*/
		}
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Create a file
;
;   format : USHORT		MnpCreateFile(usFile, ulRec);
;
;   input  : USHORT		usFile;		- file no.
;			 ULONG		ulRec;		- no. of records	### MOD 2172 Rel1.0 (USHORT->ULONG) 
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpCreateFile(USHORT usFile, ULONG ulRec)
{
	USHORT		usBytes, usStat;
	PINFTBL		pTable;

	/* --- looking for file relation table --- */

	pTable = SearchFileInfo(usFile);			/* look for file info.		*/

	/* --- is valid parameters given ? --- */

	if ((ulRec == 0) || (pTable == NULL)) {		/* is valid parameters ?	*/
		return (SPLU_INVALID_DATA);					/* bye bye, wrong data !*/
	}

	/* --- is opened file ? --- */

	if (*(pTable->pusHandle)) {					/* being opened file ?		*/

		/* --- close it --- */

		PhyCloseFile(*(pTable->pusHandle));			/* once close it		*/
		*(pTable->pusHandle) = 0;					/* reset handle			*/

		/* --- delete it before creating --- */

		PhyDeleteFile(pTable->pszFileName);			/* delete from system	*/
	}

	/* --- compute bytes per record --- */

	usBytes = (pTable->pKeyInfo)->usBytesRec + (pTable->pKeyInfo)->usBytesSys;

	/* --- (re)create the file --- */

	usStat = PhyCreateFile(pTable->pszFileName,	/* create a file			*/
						   pTable->pusHandle,		/* pointer for handle	*/
						   usFile,					/* unique ID of file's	*/
						   pTable->usFileType,		/* file type			*/
						   usBytes,					/* bytes per record		*/
						   ulRec);					/* no. of records		*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a file
;
;   format : USHORT		MnpDeleteFile(usFile);
;
;   input  : USHORT		usFile;		- file no.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpDeleteFile(USHORT usFile)
{
	USHORT		usRet;
	PINFTBL		pTable;

	/* --- looking for file relation table --- */

	if (! (pTable = SearchFileInfo(usFile))) {	/* look for file info.		*/
		return (SPLU_INVALID_DATA);					/* bye bye, wrong data !*/
	}

	/* --- is created file ? --- */

	if (! *(pTable->pusHandle)) {				/* being opened file ?		*/
		return (SPLU_FILE_NOT_FOUND);				/* error as not found	*/
	}

	/* --- close it --- */

	PhyCloseFile(*(pTable->pusHandle));			/* once close it			*/
	*(pTable->pusHandle) = 0;						/* reset handle			*/

	/* --- delete it --- */

	usRet = PhyDeleteFile(pTable->pszFileName);	/* delete it from system	*/

	/* --- exit ... --- */

	return (usRet);
}

/**
;========================================================================
;
;   function : Clear all of the record in the file
;
;   format : USHORT		MnpClearFile(usFile);
;
;   input  : USHORT		usFile;			- file ID
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	MnpClearFile(USHORT usFile)
{
	USHORT		usStat, usHandle;
	PINFTBL		pTable;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- clear all records in the file --- */

	usStat = PhyClearFile(usHandle);			/* just clear it			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get file information
;
;   format : USHORT		MnpGetFileInfo(usFile, pSpec);
;
;   input  : USHORT		usFile;			- file ID
;			 PFSPECS	pSpec;			- pointer to file spec.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	MnpGetFileInfo(USHORT usFile, PSPECS pSpec)
{
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- get information of the file --- */

	if (! (usStat = PhyGetInfo(usHandle, &hdrFile))) {

		/* --- respond it --- */

		pSpec->usFileNo     = hdrFile.usFileNumber;	/* file number			*/
		pSpec->ulFileSize   = hdrFile.ulFileSize;	/* file size in byte	*/
		pSpec->ulMaxRec     = hdrFile.ulMaxRec;		/* max. records			*/
		pSpec->ulCurRec		= hdrFile.ulCurRec;		/* cur. records			*/
		pSpec->usRecLen		= hdrFile.usRecLen;		/* record length		*/
		pSpec->ulListMainte	= hdrFile.queListMainte.ulQueCount;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record
;
;   format : USHORT		MnpAddRecord(usFile, pvRec, pulCell);
;
;   input  : USHORT		usFile;		- file ID
;			 VOID		*pvRec;		- pointer to record data
;			 ULONG		*pulCell;	- cell no. located (only mainte. rec.)### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpAddRecord(USHORT usFile, VOID *pvRec, ULONG *pulCell)
{
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;

	/* --- initialize --- */

	if (pulCell) {								/* cell info. required ?	*/
		*pulCell = 0;								/* initialize it		*/
	}

	/* --- evaluate file information --- */
	usStat = EvaluateFileInfo(usFile, &usHandle, &pTable);
	if (usStat != 0) {	/* ### */
		return (usStat);
	}

	/* --- get file information --- */
	usStat = PhyGetInfo(usHandle, &hdrFile);
	if (usStat != 0) {	/* ### */
		return (usStat);
	}

	/* --- is any space in a file ? --- */

	if (hdrFile.ulCurRec >= hdrFile.ulMaxRec) {	/* if overflow from a file	*/
		return (SPLU_FILE_OVERFLOW);				/* hey, you're greedy !	*/
	}

	/* --- get key information table --- */

	pKey = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- pass control to each file's algorithm --- */

	switch (pTable->usFileType) {				/* branch by file type		*/
	case TYPE_HASH:								/* is hash file ?			*/
		usStat = AddRecHash(usHandle, pvRec, pKey, &hdrFile);
		break;
	case TYPE_RELATIVE:							/* is relative file ?		*/
		usStat = AddRecRelative(usHandle, pvRec, pKey, &hdrFile);
		break;
	case TYPE_MAINTE:							/* is mainte. file ?		*/
		usStat = AddRecMainte(usHandle, pvRec, pKey, &hdrFile, pulCell);
		break;
	case TYPE_INDEX:							/* is index file ? R2.0 	*/
		usStat = AddRecIndex(usHandle, pvRec, &hdrFile);
		break;
	default:									/* others ...				*/
		usStat = SPLU_INVALID_DATA;
		break;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record
;
;   format : USHORT		MnpDeleteRecord(usFile, pucMainKey, usSubKey);
;
;   input  : USHORT		usFile;			- file ID
;			 UCHAR		*pucMainKey;	- pointer to main key no.
;			 USHORT		usSubKey;		- sub key no.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpDeleteRecord(USHORT usFile, WCHAR *pucMainKey, USHORT usSubKey)
{
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- is this a mainte. file ? --- */

	if (pTable->usFileType == TYPE_MAINTE) {	/* is mainte. file ?		*/
		return (SPLU_INVALID_DATA);					/* bye, can't support !	*/
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get key information table --- */

	pKey = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- pass control to each file's algorithm --- */

	switch (pTable->usFileType) {				/* branch by file type		*/
	case TYPE_HASH:								/* is hash file ?			*/
		usStat = DelRecHash(usHandle, pucMainKey, usSubKey, pKey, &hdrFile);
		break;
	case TYPE_RELATIVE:							/* is relative file ?		*/
		usStat = DelRecRelative(usHandle, pucMainKey, pKey, &hdrFile);
		break;
	case TYPE_INDEX:							/* is index file ? R2.0		*/
		usStat = DelRecIndex(usHandle, pucMainKey, &hdrFile);
		break;
	default:									/* others ...				*/
		usStat = SPLU_INVALID_DATA;
		break;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record by random access
;
;   format : USHORT
;			 MnpReadRecord(usFile, usManip, pucMain, usSub, pucBuffer);
;
;   input  : USHORT		usFile;			- file ID
;			 USHORT		usManip;		- handle to reserve rec.
;			 UCHAR		*pucMainKey;	- pointer to main key no.
;			 USHORT		usSubKey;		- sub key no.
;			 UCHAR		*pucBuffer;		- response buffer ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpReadRecord(USHORT usFile,
					  USHORT usManip,
					  WCHAR  *pucMain,
					  USHORT usSub,
					  WCHAR  *pucBuffer)
{
	USHORT		usStat, usHandle;
	ULONG		ulCell;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- is this a mainte. file ? --- */

	if (pTable->usFileType == TYPE_MAINTE) {	/* is mainte. file ?		*/
		return (SPLU_INVALID_DATA);					/* bye, can't support !	*/
	}

	/* --- is valid handle given to reserve a rec. ? --- */

	switch (usManip) {							/* is valid handle given ?	*/
	case 0:										/* not to requeire to resv.	*/
		pManip = (PSEQMNP)NULL;						/* mark as not to req.	*/
		break;
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get key information table --- */

	pKey = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- pass control to each file's algorithm --- */

	switch (pTable->usFileType) {				/* branch by file type		*/
	case TYPE_HASH:								/* is hash file ?			*/
		usStat = ReadRecHash(usHandle,				/* read from the file	*/
							 pucBuffer,					/* buffer ptr.		*/
							 pucMain,					/* ptr. to main key	*/
							 usSub,						/* sub key			*/
							 pKey,						/* ptr. to key info	*/
							 &hdrFile,					/* ptr. to header	*/
							 &ulCell);					/* ptr. to cell inf	*/
		break;
	case TYPE_RELATIVE:							/* is relative file ?		*/
		usStat = ReadRecRelative(usHandle,			/* read from the file	*/
								 pucBuffer,				/* buffer ptr.		*/
								 pucMain,				/* ptr. to main key	*/
								 pKey,					/* ptr. to key info	*/
								 &hdrFile,				/* ptr. to header	*/
								 &ulCell);				/* ptr. to cell inf	*/
		break;
	case TYPE_INDEX:								/* is index file ?, R2.0*/
		usStat = ReadRecIndex(usHandle,				/* read from the file	*/
							 pucBuffer,					/* buffer ptr.		*/
                             pucMain,                   /* ptr. to index    */
							 &hdrFile,					/* ptr. to header	*/
							 &ulCell);					/* ptr. to cell inf	*/
		break;
	default:									/* others ...				*/
		usStat = SPLU_INVALID_DATA;
		break;
	}

	/* --- is completed ? --- */

	if ((! usStat) && pManip) {					/* require to reserve it ?	*/
		pManip->fchFlags = SEQMNP_IN_USE + SEQMNP_VALID_NO;	/* in use		*/
		pManip->usFileNo = usFile;					/* remember file ID		*/
		pManip->ulCellNo = ulCell;					/* remember cell no.	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get a handle to access sequentially
;
;   format : USHORT		MnpBeginLookUp(usFile, pusManip, USHORT *pusSeq);
;
;   input  : USHORT		usFile;		- file ID
;			 USHORT		*pusManip;	- buffer ptr. to respond handle
;            USHORT     usSeq;      - sequential access option R2.0
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpBeginLookUp(USHORT usFile, USHORT *pusManip, USHORT *pusSeq, USHORT usFileVer)
{
	USHORT		i, usHandle, usStat;
	PSEQMNP		pManip;
	PINFTBL		pTable;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

    /* --- evaluate index file information, R2.0 --- */

    if (ADJUSTFILE(usFile) == FILE_PLU) {
	    usStat = EvaluateFileInfo(ExeSetIndexFileId(usFile), &usHandle, &pTable);

        if (!*pusSeq) {
            usStat = 1;                 /* no sequential # report */
        }
        *pusSeq = (usStat ? 0 : 1);     /* return index file status */

    }

	/* --- look for free table --- */

	for (pManip = aManipBlock, i = 0; i < ARRAYS(aManipBlock, SEQMNP); i++) {

		/* --- is it a free table ? --- */

		if (! (pManip->fchFlags & SEQMNP_IN_USE)) {

			/* --- allocate its table --- */

			pManip->fchFlags = SEQMNP_IN_USE + SEQMNP_INITIAL;	/* set flag	*/
			pManip->usFileNo = usFile;				/* remember file no.	*/
            if ((ADJUSTFILE(usFile) == FILE_PLU) && (!usStat)) {
    			pManip->usIndexFileNo = ExeSetIndexFileId(usFile);/* remember file no.	*/
            } else {
                pManip->usIndexFileNo = 0;
            }
            pManip->usFileVer = usFileVer;          /* set file version for migration */
			pManip->ulHashNo= 0;					/* from top of file		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			pManip->ulCellNo = 0;					/* from top of file		*/
			pManip->ulOffset = 0L;					/* from top of file		*/
			*pusManip        = (i + 1);				/* respond handle		*/
			break;
		}

		/* --- go to next table --- */

		pManip++;
	}

	/* --- exit ... --- */

	return ((i == ARRAYS(aManipBlock, SEQMNP)) ? SPLU_TOO_MANY_USERS : 0);
}

/**
;========================================================================
;
;   function : Release a handle for sequential access
;
;   format : USHORT		MnpEndLookUp(usManip);
;
;   input  : USHORT		usManip;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	MnpEndLookUp(USHORT usManip)
{
	PSEQMNP		pManip;

	/* --- is valid handle given ? --- */

	if ((! usManip) || (usManip > ARRAYS(aManipBlock, SEQMNP))) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- compute table ptr. from the handle --- */

	pManip = aManipBlock + usManip - 1;				/* evaluate its table	*/

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- free the table --- */

	pManip->fchFlags = 0;						/* free the table			*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Read from a file by sequential
;
;   format : USHORT		MnpReadFile(usManip, pucBuffer, usSize, pusRead);
;
;   input  : USHORT		usManip;		- handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 USHORT		usSize;			- no. of bytes to read
;			 USHORT		*pusRead;		- no. of bytes to have been read
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	MnpReadFile(USHORT usManip, WCHAR *pucBuffer, USHORT usSize, USHORT *pusRead)
{
	USHORT		usStat,usHandle;
	ULONG		usBytes;
	FILEHDR		hdrFile;
	PSEQMNP		pManip;
	PINFTBL		pTable;

	/* --- initialize --- */

	*pusRead = 0;								/* assume no data read		*/
	usBytes  = usSize;							/* no. of bytes to read		*/

	/* --- is valid handle given ? --- */

	if ((! usManip) || (usManip > ARRAYS(aManipBlock, SEQMNP))) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- compute table ptr. from the handle --- */

	pManip = aManipBlock + usManip - 1;			/* evaluate its table		*/

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- come to an end of file ? --- */

	if (pManip->ulOffset >= hdrFile.ulFileSize) {
		return (SPLU_END_OF_FILE);
	}
	
	/* --- read from a file --- */

	usStat = PhyReadFile(usHandle, pucBuffer, &usBytes, pManip->ulOffset);

	/* --- completed ? --- */

	if (! usStat) {								/* completed ?				*/
		*pusRead          = (USHORT)usBytes;				/* respond no. of bytes	*/
		pManip->ulOffset += usBytes;			/* next file position	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Write a file by sequential
;
;   format : USHORT		MnpWriteFile(usManip, pucBuffer, usBytes, pusWritten);
;
;   input  : USHORT		usManip;		- handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to data
;			 USHORT		usBytes;		- data length in byte
;			 USHORT		*pusWritten;	- no. of bytes to have been written
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	MnpWriteFile(USHORT usMh, WCHAR *pucData, USHORT usBytes, USHORT *pusWritten)
{
	USHORT		usStat, usHandle;
	ULONG		ulLength;
	FILEHDR		hdrFile;
	PSEQMNP		pManip;
	PINFTBL		pTable;

	/* --- initialize --- */

	*pusWritten = 0;							/* assume no data written	*/

	/* --- is valid handle given ? --- */

	if ((! usMh) || (usMh > ARRAYS(aManipBlock, SEQMNP))) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- compute table ptr. from the handle --- */

	pManip = aManipBlock + usMh - 1;			/* evaluate its table		*/

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {	/* is it in use ?			*/
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- come to an end of file ? --- */

	if (pManip->ulOffset >= hdrFile.ulFileSize) {
		return (SPLU_FILE_OVERFLOW);
	}

	/* --- compute no. of byte available --- */

	ulLength = MINOF((hdrFile.ulFileSize - pManip->ulOffset), (ULONG)usBytes);

	/* --- write a file --- */

	usStat = PhyWriteFile(usHandle,				/* write to a file			*/
						  pucData,					/* data buffer ptr.		*/
						  (&ulLength),	/* data size / write	*/
						  pManip->ulOffset);		/* file position		*/

	/* --- completed ? --- */

	if (! usStat) {								/* completed ?				*/
		*pusWritten       = (USHORT)ulLength;		/* respond no. of bytes	*/
		pManip->ulOffset += ulLength;				/* next file position	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Seek a file position
;
;   format : USHORT		MnpSeekFile(usManip, lMoves, usOrigin, pulNewPtr);
;
;   input  : USHORT		usManip;		- handle
;			 LONG		lMoves;			- no. of byte to move
;			 USHORT		usOrigin;		- origin point
;			 ULONG		*pulNewPtr;		- new file position
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	MnpSeekFile(USHORT usManip, LONG lMoves, USHORT usOrigin, ULONG *pulNewPtr)
{
	USHORT		usStat, usHandle;
	ULONG		ulOffset;
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PSEQMNP		pManip;

	/* --- is valid handle given ? --- */

	if ((! usManip) || (usManip > ARRAYS(aManipBlock, SEQMNP))) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- compute table ptr. from the handle --- */

	pManip = aManipBlock + usManip - 1;			/* evaluate its table		*/

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {	/* is it in use ?			*/
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- evaluate new file position --- */

	switch (usOrigin) {							/* what origin point ?		*/
	case PLU_SB_CUR:							/* from current position ?	*/
		ulOffset = pManip->ulOffset;				/* current offset		*/
		break;
	case PLU_SB_END:							/* from end of file ?		*/
		ulOffset = hdrFile.ulFileSize;				/* bottom offset		*/
		break;
	case PLU_SB_TOP:							/* from top of file ?		*/
		ulOffset = 0L;								/* top offset			*/
		break;
	default:									/* others ...				*/
		return (SPLU_INVALID_DATA);					/* give me valid origin	*/
		break;
	}

	/* --- compute new file position --- */

	ulOffset += lMoves;							/* new file position		*/

	/* --- can be moved ? --- */

	usStat = (ulOffset <= hdrFile.ulFileSize) ? 0 : SPLU_FILE_OVERFLOW;

	/* --- update file position --- */

	pManip->ulOffset = usStat ? pManip->ulOffset : ulOffset;
	*pulNewPtr       = pManip->ulOffset;		/* respond file offset		*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get a record by sequential
;
;   format : USHORT		MnpGetLookUp(usManip, pucBuffer);
;
;   input  : USHORT		usManip;		- handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	MnpGetLookUp(USHORT usManip, WCHAR *pucBuffer)
{
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;

	/* --- is valid handle given ? --- */

	if ((! usManip) || (usManip > ARRAYS(aManipBlock, SEQMNP))) {
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- compute table ptr. from the handle --- */

	pManip = aManipBlock + usManip - 1;				/* evaluate its table	*/

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {	/* is it in use ?			*/
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- evaluate file information --- */

    if (pManip->usIndexFileNo) {

        /* --- read index file handle and table R2.0 --- */

    	if (usStat = EvaluateFileInfo(pManip->usIndexFileNo, &usHandle, &pTable)) {
	    	return (usStat);
    	}

    } else {

    	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
	    	return (usStat);
    	}
    }

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get key information table --- */

	pKey = pTable->pKeyInfo;					/* ptr. to key information	*/


    /* correct key information for old file migration, R2.0 */
#ifdef PEP
    pKey = ReplaceKeyInfo(pManip->usFileNo, pManip->usFileVer, pKey);
#endif

	/* --- pass control to each file's algorithm --- */

	switch (pTable->usFileType) {				/* branch by file type		*/
	case TYPE_HASH:								/* is hash file ?			*/
    	usStat = TraceRecHash(usHandle,				/* read from the file	*/
	    					  pucBuffer,				/* buffer ptr.		*/
							  pKey,						/* ptr. to key info	*/
		    				  &hdrFile,					/* ptr. to header	*/
			    			  pManip);					/* ptr. to seq. cnt */
        
		break;
	case TYPE_RELATIVE:							/* is relative file ?		*/
		usStat = TraceRecRelative(usHandle,			/* read from the file	*/
								  pucBuffer,			/* buffer ptr.		*/
								  pKey,					/* ptr. to key info	*/
								  &hdrFile,				/* ptr. to header	*/
								  pManip);				/* ptr. to seq. cnt	*/
		break;
	case TYPE_MAINTE:							/* is mainte. file ?		*/
		usStat = TraceRecMainte(usHandle,			/* read from the file	*/
								pucBuffer,				/* buffer ptr.		*/
								pKey,					/* ptr. to key info	*/
								&hdrFile,				/* ptr. to header	*/
								pManip);				/* ptr. to seq. cnt	*/
		break;
    case TYPE_INDEX:                            /* is index file ?, R2.0    */
    	usStat = TraceRecIndex(usHandle,				/* read from the file	*/
                              usManip,
                              pManip->usFileNo,
	    					  pucBuffer,				/* buffer ptr.		*/
							  pKey,						/* ptr. to key info	*/
		    				  &hdrFile,					/* ptr. to header	*/
			    			  pManip);					/* ptr. to seq. cnt */
        break;
	default:									/* others ...				*/
		usStat = SPLU_INVALID_DATA;
		break;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Modify a record
;
;   format : USHORT		MnpModifyRecord(usManip, pvRec);
;
;   input  : USHORT		usManip;	- handle to reserve rec.
;			 VOID		*pvRec;		- pointer to record
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpModifyRecord(USHORT usManip, VOID *pvRec)
{
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usHandle, usStat;
	ULONG		ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* given a 0 value ?		*/
		return (SPLU_INVALID_HANDLE);				/* error as invalid		*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is storing valid cell no. ? --- */

	if (! (pManip->fchFlags & SEQMNP_VALID_NO)) {
		return (SPLU_INVALID_ACCESS);
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- looking for file relation table --- */

	ulCell = pManip->ulCellNo;					/* cell no. of the file		*/
	pKey   = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- other than relative files ? --- */

	if (pTable->usFileType != TYPE_RELATIVE) {		/* is out of relative ?	*/

		/* --- get its complete record --- */

		if (usStat = PhyReadRecord(usHandle, ulCell, aucRecord, &hdrFile)) {
			return (usStat);
		}
	}

	/* --- override new record --- */

	memcpy(aucRecord, pvRec, pKey->usBytesRec);

	/* --- write to record --- */

	usStat = PhyWriteRecord(usHandle, ulCell, aucRecord, &hdrFile);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Peek a record
;
;   format : USHORT		MnpPeekRecord(usManip, pvRec);
;
;   input  : USHORT		usManip;	- handle to reserve rec.
;			 VOID		*pvRec;		- buffer pointer to respond record
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpPeekRecord(USHORT usManip, VOID *pvRec)
{
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usHandle, usStat;
	ULONG		ulCell;		/*### MOD 2172 Rel1.0 (USHORT->ULONG) */
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is storing valid cell no. ? --- */

	if (! (pManip->fchFlags & SEQMNP_VALID_NO)) {
		return (SPLU_INVALID_ACCESS);
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- looking for file relation table --- */

	ulCell = pManip->ulCellNo;					/* cell no. of the file		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	pKey   = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get its complete record --- */

	if (usStat = PhyReadRecord(usHandle, ulCell, aucRecord, &hdrFile)) {
		return (usStat);
	}

	/* --- respond a record --- */

	memcpy(pvRec, aucRecord, pKey->usBytesRec);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Remove a maintenance record
;
;   format : USHORT		MnpRemoveRecord(usManip);
;
;   input  : USHORT		usManip;	- handle to reserve rec.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpRemoveRecord(USHORT usManip)
{
	BOOL		fEffect;
	WCHAR		aucRecord[ENOUGHREC];
	USHORT		usHandle, usStat, usFile;
	ULONG		ulCell, ulPrev, ulNext; /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;
	PMNTCNT		pChain;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is storing valid cell no. ? --- */

	if (! (pManip->fchFlags & SEQMNP_VALID_NO)) {
		return (SPLU_INVALID_ACCESS);
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- is this a mainte. file ? --- */

	if (pTable->usFileType != TYPE_MAINTE) {	/* is out of mainte. file ?	*/
		return (SPLU_INVALID_DATA);					/* bye, can't support !	*/
	}

	/* --- get access information --- */

	ulCell = pManip->ulCellNo;					/* cell no. of the file		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	pKey   = pTable->pKeyInfo;					/* ptr. to key information	*/
	usFile = HandleToFileNo(usHandle);			/* get access file number	*/

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get its complete record --- */

	if (usStat = PhyReadRecord(usHandle, ulCell, aucRecord, &hdrFile)) {
		return (usStat);
	}

	/* --- get chaining condition --- */

	pChain = (PMNTCNT)((BYTE *)aucRecord + pKey->usBytesRec);
	ulNext = pChain->ulNext;
	ulPrev = pChain->ulPrev;

	/* --- is top of queued cell ? --- */

	if (hdrFile.queListMainte.ulQueFirst == ulCell) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		hdrFile.queListMainte.ulQueFirst = ulNext;
	}

	/* --- else middle position ... --- */

	else {

		/* --- get previous record --- */

		if (usStat = PhyReadRecord(usHandle, ulPrev, aucRecord, &hdrFile)) {
			return (usStat);
		}

		/* --- adjust chain --- */

		pChain->ulNext = ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- overwrite it --- */

		if (usStat = PhyWriteRecord(usHandle, ulPrev, aucRecord, &hdrFile)) {
			return (usStat);
		}
	}

	/* --- is bottom queued cell ? --- */

	if (hdrFile.queListMainte.ulQueLast == ulCell) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		hdrFile.queListMainte.ulQueLast = ulPrev;
	}

	/* --- else middle position ... --- */

	else {

		/* --- get previous record --- */

		if (usStat = PhyReadRecord(usHandle, ulNext, aucRecord, &hdrFile)) {
			return (usStat);
		}

		/* --- adjust chain --- */

		pChain->ulPrev = ulPrev;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- overwrite it --- */

		if (usStat = PhyWriteRecord(usHandle, ulNext, aucRecord, &hdrFile)) {
			return (usStat);
		}
	}

	/* --- adjust sequential record ptr. --- */

	fEffect = (ulNext == MNT_NO_LINKED) ? FALSE : TRUE;
	DeleteRecordPtr(usFile, ulCell, ulNext, fEffect);

	/* --- remove the record by index --- */

	if (usStat = PhySetUpOccupied(usHandle, ulCell, &hdrFile, FALSE)) {
		return (usStat);
	}

	/* --- adjust no. of records in header --- */

	(hdrFile.queListMainte.ulQueCount)--;		/* delete 1 record			*/
	(hdrFile.ulCurRec)--;						/* delete 1 record			*/

	/* --- update file header --- */

	usStat = PhyUpdateInfo(usHandle, &hdrFile);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Keep current file position for seq. read
;
;   format : USHORT		MnpKeepFilePtr(usManip);
;
;   input  : USHORT		usManip;	- handle to keep position
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpKeepFilePtr(USHORT usManip)
{
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is storing valid cell no. ? --- */

	if (! (pManip->fchFlags & SEQMNP_VALID_NO)) {
		return (SPLU_INVALID_ACCESS);
	}

	/* --- keep the position for later call --- */

	pManip->fchFlags |= SEQMNP_READABLE;		/* readable it directly !	*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Move current file position to specified rec. position
;
;   format : USHORT		MnpMoveFilePtr(usManip, pItemNumber);
;
;   input  : USHORT		usManip;		- handle to move position
;			 PITEMNO	pItemNumber;	- item no. to locate
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpMoveFilePtr(USHORT usManip, VOID *pItemNumber)
{
	USHORT		usStat, usHandle;
	ULONG		ulCell;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PKEYINF		pKey;
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is it in use ? --- */

	if (! (pManip->fchFlags & SEQMNP_IN_USE)) {	/* is it in use ?			*/
		return (SPLU_INVALID_HANDLE);				/* assume invalid		*/
	}

	/* --- evaluate file information --- */

    if (pManip->usIndexFileNo) {

        /* --- read index file handle and table R2.0 --- */

    	if (usStat = EvaluateFileInfo(pManip->usIndexFileNo, &usHandle, &pTable)) {
	    	return (usStat);
    	}

    } else {

    	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
	    	return (usStat);
    	}
    }

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- get key information table --- */

	pKey = pTable->pKeyInfo;					/* ptr. to key information	*/

	/* --- pass control to each file's algorithm --- */

	switch (pTable->usFileType) {				/* branch by file type		*/
	case TYPE_HASH:								/* is hash file ?			*/
		usStat = ReadRecHash(usHandle,				/* read from the file	*/
							 NULL,						/* buffer ptr.		*/
							 (WCHAR *)pItemNumber,		/* ptr. to main key	*/
							 0,							/* sub key			*/
							 pKey,						/* ptr. to key info	*/
							 &hdrFile,					/* ptr. to header	*/
							 &ulCell);					/* ptr. to cell inf	*/
		usStat = (usStat == SPLU_ADJ_VIOLATION) ? 0 : usStat;
		break;
	case TYPE_RELATIVE:							/* is relative file ?		*/
		usStat = ReadRecRelative(usHandle,			/* read from the file	*/
								 NULL,					/* buffer ptr.		*/
								 (WCHAR *)pItemNumber,	/* ptr. to main key	*/
								 pKey,					/* ptr. to key info	*/
								 &hdrFile,				/* ptr. to header	*/
								 &ulCell);				/* ptr. to cell inf	*/
		break;
	case TYPE_INDEX:								/* is index file ?, R2.0*/
		usStat = ReadRecIndex(usHandle,				/* read from the file	*/
							 NULL,					/* buffer ptr.		*/
							 (WCHAR *)pItemNumber,	/* ptr. to main key	*/
							 &hdrFile,					/* ptr. to header	*/
							 &ulCell);					/* ptr. to cell inf	*/

        if (usStat == SPLU_REC_NOT_FOUND) {         /* ignore not on file */

            usStat = 0;
        }
        break;
	default:									/* others ...				*/
		usStat = SPLU_INVALID_DATA;
		break;
	}

	/* --- is completed ? --- */

	if (! usStat) {								/* require to reserve it ?	*/
		pManip->fchFlags &= ~SEQMNP_INITIAL;	/* not the 1st call			*/
		pManip->fchFlags |= SEQMNP_READABLE;	/* readable directly later	*/
		pManip->fchFlags |= SEQMNP_VALID_NO;	/* mark as valid cell no.	*/
        if (pTable->usFileType != TYPE_INDEX) {	/* branch by file type		*/
    		pManip->ulCellNo  = ulCell;			/* remember its cell no.	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
        }
		pManip->ulHashNo = ulCell;				/* remember its hash no.	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get current file position for seq. read
;
;   format : USHORT		MnpGetFilePtr(usManip, pulCell);
;
;   input  : USHORT		usManip;	- handle to get position
;			 ULONG		*pulCell;	- current cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpGetFilePtr(USHORT usManip, ULONG *pulCell)
{
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- is storing valid cell no. ? --- */

	if (! (pManip->fchFlags & SEQMNP_VALID_NO)) {
		return (SPLU_INVALID_ACCESS);
	}

	/* --- respond current file position --- */

	*pulCell = pManip->ulCellNo;				/* current cell no.			*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Set file position for seq. read
;
;   format : USHORT		MnpSetFilePtr(usManip, ulCell);
;
;   input  : USHORT		usManip;	- handle to set position
;			 ULONG		ulCell;		- cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	MnpSetFilePtr(USHORT usManip, ULONG ulCell)
{
	BOOL		fOccupy;
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;
	PSEQMNP		pManip;

	/* --- compute table ptr. from the handle --- */

	switch (usManip) {							/* what kind of handle ?	*/
	case 0:										/* is 0 ?					*/
		return (SPLU_INVALID_HANDLE);				/* invalid				*/
	case HMNP_REC_MAIN:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[0];					/* evaluate its table	*/
		break;
	case HMNP_REC_SUB:							/* for default record 1 ?	*/
		pManip = &aDefaultBlock[1];					/* evaluate its table	*/
		break;
	default:									/* others ...				*/
		if (usManip > ARRAYS(aManipBlock, SEQMNP)) {/* is valid handle ?	*/
			return (SPLU_INVALID_HANDLE);				/* assume invalid	*/
		}
		pManip = aManipBlock + usManip - 1;			/* evaluate its table	*/
		break;
	}

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(pManip->usFileNo, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- get file information --- */

	if (usStat = PhyGetInfo(usHandle, &hdrFile)) {
		return (usStat);
	}

	/* --- examine the cell --- */

	if (usStat = PhyExamOccupied(usHandle, ulCell, &hdrFile, &fOccupy)) {
		return (usStat);
	}

	/* --- is any record on it ? --- */

	if (! fOccupy) {							/* no record on it ?		*/
		return (SPLU_REC_NOT_FOUND);				/* assume no record		*/
	}

	/* --- set the position for later call --- */

	pManip->ulCellNo  = ulCell;					/* set new sell no.			*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	pManip->fchFlags &= ~SEQMNP_INITIAL;		/* not first call			*/
	pManip->fchFlags |= SEQMNP_READABLE;		/* readable it directly !	*/
	pManip->fchFlags |= SEQMNP_VALID_NO;		/* stored valid no.			*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Get file relation table
;
;   format : USHORT		MnpQInfoTable(usFile, ppInfo);
;
;   input  : USHORT		usFile;			- file ID
;			 PKEYINF	*ppInfo;		- pointer to table
;
;   output : USHORT		usStat;			- status
;
;	notes  : If pInfo equals to NULL, could not be found.
;
;========================================================================
**/

USHORT	MnpQInfoTable(USHORT usFile, PKEYINF *ppInfo)
{
	USHORT		usHandle, usStat;
	PINFTBL		pTable;

	/* --- evaluate file information --- */

	if (usStat = EvaluateFileInfo(usFile, &usHandle, &pTable)) {
		return (usStat);
	}

	/* --- could be found ? --- */

	*ppInfo = pTable ? pTable->pKeyInfo : (PKEYINF)NULL;

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Open a mirror file
;
;   format : USHORT		MnpOpenMirror(pszFileName, pusMirrorFile);
;
;   input  : UCHAR FAR	*pszFileName;	- ptr. to file name
;			 USHORT		*pusMirrorFile;	- buffer ptr. to receive file ID
;
;   output : USHORT		usStat;
;
;========================================================================
**/

USHORT	MnpOpenMirror(WCHAR FAR *pszFileName, USHORT *pusMirrorFile)
{
	USHORT		usStat, usHandle;
	FILEHDR		hdrFile;
	PINFTBL		pTable;

	/* --- initialize --- */

	*pusMirrorFile = 0;							/* initialize handle value	*/

	/* --- open the file w/ read & write mode --- */

	if (PhyOpenFile(pszFileName, &usHandle, TRUE)) {
		return (SPLU_FILE_NOT_FOUND);
	}

	/* --- audit the file --- */

	if (! (usStat = PhyAuditFile(usHandle))) {	/* is valid file struture ?	*/

		/* --- get file information --- */

		if (! (usStat = PhyGetInfo(usHandle, &hdrFile))) {

			/* --- assume invalid mass memory file --- */

			usStat = SPLU_INVALID_FILE;			/* assume invalid file		*/

			/* --- is valid file number stored ? --- */

			if (pTable = SearchFileInfo(hdrFile.usFileNumber)) {

				/* --- is valid file type stored ? --- */

				if (pTable->usFileType == hdrFile.usFileType) {
					usStat = 0;					/* OK ! Valid file given !	*/
				}
			}
		}
	}

	/* --- is valid file given ? --- */

	if (! usStat) {								/* is valid file given ?	*/
		*pusMirrorFile = TOMIRROR(usHandle, hdrFile.usFileNumber);
	} else {									/* else, unknown file		*/
		PhyCloseFile(usHandle);						/* close it				*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Close a mirror file
;
;   format : USHORT		MnpCloseMirror(usMirrorFile);
;
;   input  : USHORT		usMirrorFile;	- mirror file ID
;
;   output : USHORT		usStat;
;
;========================================================================
**/

USHORT	MnpCloseMirror(USHORT usMirrorFile)
{
	USHORT	usStat;

	/* --- is valid file ID given ? --- */

	if (! ISMIRROR(usMirrorFile)) {				/* is valid file ID given ?	*/
		return (SPLU_INVALID_DATA);					/* assume invalid		*/
	}

	/* --- just close it --- */

	usStat = PhyCloseFile(HFMIRROR(usMirrorFile));

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Is a mirror file ID or not
;
;   format : USHORT		MnpIsMirrorFile(usFile);
;
;   input  : USHORT		usFile;		- file ID
;
;   output : BOOL		fMirror;	- mirror file or not
;
;========================================================================
**/

BOOL	MnpIsMirrorFile(USHORT usFile)
{
	/* --- just compute it --- */

	return (ISMIRROR(usFile));
}

/**
;========================================================================
;
;   function : Get a file ID of mirror file
;
;   format : USHORT		MnpFileOfMirror(usMirrorFile);
;
;   input  : USHORT		usMirrorFile;		- mirror file ID
;
;   output : USHORT		usFile;				- fil number
;
;========================================================================
**/

USHORT	MnpFileOfMirror(USHORT usMirrorFile)
{
#ifdef	__DELETED__
	USHORT		usStat, usFile;
	FILEHDR		hdrFile;

	/* --- initialize --- */

	usFile = 0;									/* assume no file ID		*/

	/* --- is valid file ID given ? --- */

	if (! ISMIRROR(usMirrorFile)) {				/* is valid file ID given ?	*/
		return (0);									/* assume invalid		*/
	}

	/* --- get header information of the file --- */

	if (! (usStat = PhyGetInfo(HFMIRROR(usMirrorFile), &hdrFile))) {

		/* --- respond file ID --- */

		usFile = hdrFile.usFileNumber;			/* respond file number		*/
	}

	/* --- exit ... --- */

	return (usFile);
#endif	/* __DELETED */
	/* --- just compute it --- */

	return (FNMIRROR(usMirrorFile));
}

/*==========================================================================*\
;+																			+
;+		R E C O R D    M A N I P U L A T E    P R I M I T I V E s			+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Add a record for hash file
;
;   format : USHORT		AddRecHash(hf, pvRec, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	AddRecHash(USHORT hf, VOID *pvRec, PKEYINF pKey, PFILEHDR pHeader)
{
	BOOL	fSynonym, fOccupy, fAdjItem, fAddTail, fBrokenChain;
	WCHAR	aucNative[ENOUGHREC * 50], aucPrev[ENOUGHREC * 50];
	SHORT	sComp;
	USHORT	usStat, usFile;
	ULONG	ulCell, ulFree, ulPrev, ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PRECCNT	pChainNative, pChainPrev;

	/* --- initialize --- */
	pChainNative = (RECCNT*)((BYTE *)aucNative + pKey->usBytesRec);
	pChainPrev   = (RECCNT*)((BYTE *)aucPrev   + pKey->usBytesRec);
	fBrokenChain = FALSE;
	usFile       = HandleToFileNo(hf);


	/* --- compute file cell no. --- */

	ulCell = ComputeHash(pvRec, pKey->usKeyLen, pHeader->ulHashKey);

	/* --- get current state --- */

	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is the cell already occupied ? --- */

	if (! fOccupy) {								/* if virgin cell ...	*/

		/* --- make the record data --- */

		memcpy(aucNative, pvRec, pKey->usBytesRec);
		pChainNative->fcInfo = REC_LEGAL_POS;		/* set original flag	*/
		pChainNative->ulNext = (ULONG)(-1);		/* usualy out of file	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- add a record simply --- */

		return (AddRecord(hf, ulCell, aucNative, pHeader));
	}

	/* --- look for a free cell --- */

	if (usStat = PhySearchVacant(hf, ulCell, pHeader, &ulFree)) {
		return (usStat);
	}

	/* --- get a record of the cell's --- */

	if (usStat = PhyReadRecord(hf, ulCell, aucNative, pHeader)) {
		return (usStat);
	}

	/* --- is it an original record ? --- */

	if (pChainNative->fcInfo & REC_LEGAL_POS) {	/* is original record ?		*/
		fSynonym = TRUE;							/* add as synonym rec.	*/
		fAdjItem = FALSE;							/* not yet adjective	*/
		ulPrev   = ulCell;							/* search from the top	*/
	} else {									/* else, it's chained rec.	*/
		fSynonym = FALSE;							/* add as original rec.	*/
		ulPrev   = ComputeHash((WCHAR *)aucNative, pKey->usKeyLen, pHeader->ulHashKey); /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- search a record position to be added --- */

	while (FOREVER) {

		/* --- get a record --- */

		if (usStat = PhyReadRecord(hf, ulPrev, aucPrev, pHeader)) {
			return (usStat);
		}

		/* --- check against adjective record --- */

		if (fSynonym) {							/* only synonym rec. add	*/

			/* --- is same item no. ? --- */

			if (! memcmp(aucPrev, pvRec, pKey->usKeyLen)) {

				/* --- check adjective number rule --- */

				if (usStat = CheckAdjective(aucPrev, pvRec, pKey, &sComp)) {
					return (usStat);
				}

				/* --- is this a larger adj. no. ? --- */

				if (sComp > 0) {				/* ADJ(aucPrev) < ADJ(pvRec)*/
					fAddTail = FALSE;				/* add in the middle	*/
					break;							/* found a pos. to add	*/
				}

				/* --- this must be adj. record --- */

				fAdjItem = TRUE;
			}

			/* --- under searching an adj. item ? --- */

			else if (fAdjItem) {				/* under searching Adj. Item*/
				fAddTail = FALSE;					/* add in the middle	*/
				break;								/* found a pos. to add	*/
			}
		}

		/* --- does the record chain next ? --- */

		if (! (pChainPrev->fcInfo & REC_LINK_NEXT)) {
			fAddTail = TRUE;					/* add at the bottom		*/
			break;
		}

		/* --- get next chain address --- */

		ulNext = pChainPrev->ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- found a previous rec. on adding as original ? --- */

		if ((! fSynonym) && (ulNext == ulCell)) {

			/* --- move the record to free cell --- */

			if (usStat = AddRecord(hf, ulFree, aucNative, pHeader)) {
				return (usStat);
			}

			/* --- adjust sequential ptr. --- */

			MoveRecordPtr(usFile, ulCell, ulFree);

			/* --- adjust previous record's chain --- */

			pChainPrev->ulNext = ulFree;			/* a cell no. moved		*/

			/* --- override it --- */

			if (usStat = PhyWriteRecord(hf, ulPrev, aucPrev, pHeader)) {
				return (usStat);
			}

			/* --- go to add a new record --- */

			break;
		}

		/* --- is this address valid ? --- */

		if (ulNext >= pHeader->ulMaxRec) {		/* point to a cell in file ?*/
			fBrokenChain = TRUE;					/* assume broken chain	*/
			fAddTail     = TRUE;					/* add at the bottom	*/
			break;									/* break if overflow	*/
		}

		/* --- get the cell's state --- */

		if (usStat = PhyExamOccupied(hf, ulNext, pHeader, &fOccupy)) {
			return (usStat);
		}

		/* --- located a record there ? --- */

		if (! fOccupy) {						/* is a record located ?	*/
			fBrokenChain = TRUE;					/* assume broken chain	*/
			fAddTail     = TRUE;					/* add at the bottom	*/
			break;									/* break if no rec.		*/
		}

		/* --- continue to search a position --- */

		ulPrev = ulNext;
	}

	/* --- add a record as synonym record ? --- */

	if (fSynonym) {

		/* --- make up complete record format --- */

		if (fAddTail) {
			memcpy(aucNative, pvRec,   pKey->usBytesRec);
			pChainNative->fcInfo = 0;
			pChainNative->ulNext = (ULONG)(-1);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		} else {
			memcpy(aucNative, aucPrev, pKey->usBytesRec + pKey->usBytesSys);
			memcpy(aucPrev,   pvRec,   pKey->usBytesRec);
			pChainPrev->fcInfo = (UCHAR)(pChainNative->fcInfo & REC_LEGAL_POS);
			pChainNative->fcInfo &= (UCHAR)(~REC_LEGAL_POS);
			MoveRecordPtr(usFile, ulPrev, ulFree);
		}

		/* --- add a new record at free cell --- */

		if (usStat = AddRecord(hf, ulFree, aucNative, pHeader)) {
			return (usStat);
		}

		/* --- adjust previous record's chain --- */

		pChainPrev->fcInfo |= REC_LINK_NEXT;		/* reset chain flag		*/
		pChainPrev->ulNext  = ulFree;				/* link to a rec. above	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- override it --- */

		usStat = PhyWriteRecord(hf, ulPrev, aucPrev, pHeader);
	}

	/* --- else, add a record as original --- */

	else {

		/* --- is chain broken ? --- */

		if (fBrokenChain) {						/* chain broken ?			*/

			/* --- make a complete record format --- */

			pChainPrev->fcInfo &= ~REC_LINK_NEXT;	/* reset chain flag		*/
			pChainPrev->ulNext  = (ULONG)(-1);		/* usualy out of file	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		
			/* --- adjust its record's contents --- */

			if (usStat = PhyWriteRecord(hf, ulPrev, aucPrev, pHeader)) {
				return (usStat);
			}
		}

		/* --- make a complete record format --- */

		memcpy(aucNative, pvRec, pKey->usBytesRec);
		pChainNative->fcInfo = REC_LEGAL_POS;		/* set original flag	*/
		pChainNative->ulNext = (ULONG)(-1);			/* usualy out of file	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		
		/* --- add a record as original --- */

		usStat = PhyWriteRecord(hf, ulCell, aucNative, pHeader);
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record in hash file
;
;   format : USHORT		AddRecord(hf, ulCell, aucRec, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 ULONG		ulCell;			- location cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 UCHAR		*aucRec;		- pointer to record
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	AddRecord(USHORT hf, ULONG ulCell, WCHAR *aucRec, PFILEHDR pHeader)
{
	USHORT	usStat;

	/* --- write a record at the position --- */

	if (usStat = PhyWriteRecord(hf, ulCell, aucRec, pHeader)) {
		return (usStat);
	}

	/* --- set index of the location active --- */

	if (usStat = PhySetUpOccupied(hf, ulCell, pHeader, TRUE)) {
		return (usStat);
	}

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)++;			/* add 1 record				*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Check adjective number rule
;
;   format : USHORT		CheckAdjective(puc1st, puc2nd, pKey, psComp);
;
;   input  : UCHAR		*puc1st;		- 1st record data ptr.
;			 UCHAR		*puc2nd;		- 2nd record data ptr.
;			 PKEYINF	pKey;			- key record information
;			 SHORT		*psComp;		- ptr. to respond comparison
;
;   output : USHORT		usStat;			- status
;
;	notes  : *psComp = ADJ(puc1st) - ADJ(puc2nd);
;
;========================================================================
**/


USHORT	CheckAdjective(WCHAR *puc1st, WCHAR *puc2nd, PKEYINF pKey, SHORT *psComp)
{
	USHORT	us1stAdj, us2ndAdj;

	/* --- is adjective field defined ? --- */

	if (! pKey->usSubKeyOffset) {				/* is adj. field defined ?	*/
		return (SPLU_REC_DUPLICATED);				/* assume same item no.	*/
	}

	/* --- evaluate adjective number --- */

	if (pKey->usSubKeyType == TYPUCH) {			/* UCHAR type ?				*/
		us1stAdj = (USHORT)(*((BYTE *)puc1st + pKey->usSubKeyOffset));
		us2ndAdj = (USHORT)(*((BYTE *)puc2nd + pKey->usSubKeyOffset));
	} else {									/* else, assume USHORT type	*/
		us1stAdj = *(USHORT *)((BYTE *)puc1st + pKey->usSubKeyOffset);
		us2ndAdj = *(USHORT *)((BYTE *)puc2nd + pKey->usSubKeyOffset);
	}

	/* --- is same adjective number ? --- */

	if (us1stAdj == us2ndAdj) {					/* same adjective number ?	*/
		return (SPLU_REC_DUPLICATED);				/* assume same item no.	*/
	}

	/* --- is whichever 0 adj. no. ? --- */

	if ((! us1stAdj) || (! us2ndAdj)) {			/* both have valid no. ?	*/
		return (SPLU_ADJ_VIOLATION);				/* adj. rule error !	*/
	}

	/* --- compare those adj. no. --- */

	*psComp = (SHORT)(us1stAdj - us2ndAdj);

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Add a record for relative file
;
;   format : USHORT		AddRecRelative(hf, pvRec, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	AddRecRelative(USHORT hf, VOID *pvRec, PKEYINF pKey, PFILEHDR pHeader)
{
	BOOL	fOccupy;
	USHORT	usStat;
	ULONG	ulCell,ulKey;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- get a key no. --- */

	ulKey = (pKey->usKeyType == TYPUCH)
							? (ULONG)(*(UCHAR *)pvRec) : (ULONG)(*(USHORT *)pvRec); /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- is valid key given ? --- */

	if (! ulKey) {								/* is the key 0 ?			*/
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}

	/* --- compute file cell no. --- */

	ulCell = ComputeRelative(ulKey);			/* compute record cell no.	*/

	/* --- is within a file --- */

	if (ulCell >= pHeader->ulMaxRec) {			/* if out of a file ...		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		return (SPLU_FILE_OVERFLOW);				/* give me a valid no.	*/
	}

	/* --- get current state --- */

	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is already located ? --- */

	if (fOccupy) {								/* is already located ?		*/
		return (SPLU_REC_DUPLICATED);				/* duplicated error !	*/
	}

	/* --- write a record at the position --- */

	if (usStat = PhyWriteRecord(hf, ulCell, (WCHAR *)pvRec, pHeader)) {
		return (usStat);
	}

	/* --- set index of the location active --- */

	if (usStat = PhySetUpOccupied(hf, ulCell, pHeader, TRUE)) {
		return (usStat);
	}

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)++;		/* add 1 record				*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record for maintenance file's list queue
;
;   format : USHORT		AddRecMainte(hf, pvRec, pKey, pHeader, pulCell);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 ULONG		*pulCell;		- buffer ptr. to respond cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	AddRecMainte(USHORT hf, VOID *pvRec, PKEYINF pKey, PFILEHDR pHeader,
					ULONG *pulCell)
{
	WCHAR	aucRecord[ENOUGHREC];
	USHORT	usStat;
	ULONG	ulFree, ulTail, ulCount;
	PMNTCNT	pChain;

	/* --- look for a free cell --- */

	if (usStat = PhySearchVacant(hf, 0, pHeader, &ulFree)) {
		return (usStat);
	}

	/* --- get current bottom record's address --- */

	ulCount = (pHeader->queListMainte).ulQueCount;	/* no. of records		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	ulTail  = ulCount ? (pHeader->queListMainte).ulQueLast : MNT_NO_LINKED;

	/* --- make a complete record format --- */

	memcpy(aucRecord, pvRec, pKey->usBytesRec);		/* open in a buffer		*/
	pChain         = (PMNTCNT)((BYTE *)aucRecord + pKey->usBytesRec);
	pChain->ulPrev = ulTail;						/* link w/ previous rec	*/
	pChain->ulNext = MNT_NO_LINKED;					/* no link rec.			*/

	/* --- write a record at the position --- */

	if (usStat = PhyWriteRecord(hf, ulFree, aucRecord, pHeader)) {
		return (usStat);
	}

	/* --- set index of the location active --- */

	if (usStat = PhySetUpOccupied(hf, ulFree, pHeader, TRUE)) {
		return (usStat);
	}

	/* --- respond cell no. to have been located --- */

	if (pulCell) {									/* require to respond ?	*/
		*pulCell = ulFree;								/* return cell no.	*/
	}

	/* --- link with a bottom record --- */

	if (ulCount) {

		/* --- get a record --- */

		if (usStat = PhyReadRecord(hf, ulTail, aucRecord, pHeader)) {
			return (usStat);
		}

		/* --- chain to a new record --- */

		pChain         = (PMNTCNT)((BYTE *)aucRecord + pKey->usBytesRec);
		pChain->ulNext = ulFree;					/* chain to new rec.	*/

		/* --- write a record at the position --- */

		if (usStat = PhyWriteRecord(hf, ulTail, aucRecord, pHeader)) {
			return (usStat);
		}
	}

	/* --- adjust information in header --- */

	(pHeader->ulCurRec)++;						/* add 1 record				*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	((pHeader->queListMainte).ulQueCount)++;	/* add 1 record				*/
	(pHeader->queListMainte).ulQueLast = ulFree;/* new bottom record		*/
	if (! ulCount) {							/* is it a virgin file ?	*/
		(pHeader->queListMainte).ulQueFirst = ulFree;	/* this is top rec.	*/
	}

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record for index file, R2.0
;
;   format : USHORT		AddRecIndex(hf, pvRec, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	AddRecIndex(USHORT hf, VOID *pvRec, PFILEHDR pHeader)
{
    RECIDX  aBuffer[RECIDX_BUFFER];
    SHORT   sStatus;
    USHORT  usStat;
    ULONG    ulHitOffset, usBytes, ulWriteOffset;
#ifndef PEP
    HUGEINT   huProdWork;
#endif

	/* --- compute the offset of index --- */

	sStatus = ComputeIndex(pvRec, hf, pHeader, &ulHitOffset, (WCHAR *)&aBuffer);

    if (sStatus < 0) {
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}
    
	/* --- is already located ? --- */

	if (sStatus == 0) {								/* is already located ?		*/
		return (SPLU_REC_DUPLICATED);				/* duplicated error !	*/
	}

	/* --- write a record at the position --- */

    ulWriteOffset = sizeof(FILEHDR);
#ifdef PEP
    ulWriteOffset += (ULONG)((ULONG)sizeof(RECIDX) * (ULONG)(pHeader->ulCurRec));
#else
    RflMul32by32(&huProdWork, (ULONG)sizeof(RECIDX), (ULONG)pHeader->ulCurRec);
    ulWriteOffset += *((LONG *)(&huProdWork));
#endif

    while(ulWriteOffset > ulHitOffset) {
        /* ### OLD if ((ulWriteOffset - (ULONG)sizeof(aBuffer)) >= ulHitOffset)*/
		if((ulWriteOffset - ulHitOffset) > (ULONG)sizeof(aBuffer)){ /* ### MOD 2172 Rel1.0 (01/17/00) */
            usBytes = sizeof(aBuffer);
            ulWriteOffset -= sizeof(aBuffer);
        } else {
            usBytes = (USHORT)(ulWriteOffset - ulHitOffset);
            ulWriteOffset = ulHitOffset;
        }

        usStat = PhyReadFile(hf, (WCHAR *)&aBuffer, &usBytes, ulWriteOffset);
            
    	usStat = PhyWriteFile(hf,                   /* write to a file			*/
						  (WCHAR *)&aBuffer,        /* data buffer ptr.		*/
						  &usBytes,                 /* data size / write	*/
						  ulWriteOffset+sizeof(RECIDX));/* file position		*/
    }

    usBytes = sizeof(RECIDX);

	usStat = PhyWriteFile(hf,                   /* write to a file			*/
					  pvRec,                    /* data buffer ptr.		*/
					  &usBytes,                 /* data size / write	*/
					  ulHitOffset);             /* file position		*/

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)++;						/* add 1 record				*/

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record for hash file
;
;   format : USHORT		DelRecHash(hf, pucMainKey, usSubKey, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucMainKey;	- pointer to main key
;			 USHORT		usSubKey;		- sub key no.
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	DelRecHash(USHORT   hf,
				   WCHAR    *pucMainKey,
				   USHORT   usSubKey,
				   PKEYINF  pKey,
				   PFILEHDR pHeader)
{
	BOOL	fOccupy, fAdjItem, fEffect;
	WCHAR	aucPrev[ENOUGHREC * 2], aucRecord[ENOUGHREC * 2];
	USHORT	usAdj, usStat, usFile;
	ULONG	ulPrev, ulCell, ulAccess;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PRECCNT	pChainPrev, pChainRec;

	/* --- initialize --- */

	pChainPrev = (RECCNT*)((BYTE *)aucPrev   + pKey->usBytesRec);
	pChainRec  = (RECCNT*)((BYTE *)aucRecord + pKey->usBytesRec);

	/* --- compute its record's hash value --- */

	ulCell = ComputeHash(pucMainKey, pKey->usKeyLen, pHeader->ulHashKey);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	usFile = HandleToFileNo(hf);

	/* --- get current state --- */

	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is there a record ? --- */

	if (! fOccupy) {							/* is any record ?			*/
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- get the record --- */

	if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
		return (usStat);
	}

	/* --- is this an original record ? --- */

	if (! (pChainRec->fcInfo & REC_LEGAL_POS)) {
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- initialize before searching --- */

	fAdjItem = FALSE;							/* assume not adj. item		*/

	/* --- look for the record following list --- */

	while (FOREVER) {

		/* --- is same item number ? --- */

		if (! memcmp(aucRecord, pucMainKey, pKey->usKeyLen)) {

			/* --- is adjective field defined ? --- */

			if (! pKey->usSubKeyOffset) {		/* is adj. field defined ?	*/
				break;
			}

			/* --- evaluate adjective number --- */

			if (pKey->usSubKeyType == TYPUCH) {	/* is UCHAR type ?			*/
				usAdj = (USHORT)(*((BYTE *)aucRecord + pKey->usSubKeyOffset));
			} else {							/* else, assume USHORT type	*/
				usAdj = *(USHORT *)((BYTE *)aucRecord + pKey->usSubKeyOffset);
			}

			/* --- is same adjective number ? --- */

			if (usAdj == usSubKey) {			/* same adjective number ?	*/
				break;
			}

			/* --- is whichever 0 adj. no. ? --- */

			if ((! usAdj) || (! usSubKey)) {	/* both have valid no. ?	*/
				return (SPLU_ADJ_VIOLATION);		/* adj. rule error !	*/
			}

			/* --- is large adj. no. ? --- */

			if (usAdj > usSubKey) {				/* come to a large adj. ?	*/
				return (SPLU_REC_NOT_FOUND);		/* rec. not found		*/
			}

			/* --- mark as adj. item --- */

			fAdjItem = TRUE;					/* this must be adj. item	*/
		}

		/* --- else, is under searching adj. no. ? --- */

		else if (fAdjItem) {					/* under searching adj. ?	*/
			return (SPLU_REC_NOT_FOUND);			/* rec. not found		*/
		}

		/* --- is chained next rec. ? --- */

		if (! (pChainRec->fcInfo & REC_LINK_NEXT)) {
			return (SPLU_REC_NOT_FOUND);			/* rec. not found		*/
		}

		/* --- remember this record as previous --- */

		memcpy(aucPrev, aucRecord, pKey->usBytesRec + pKey->usBytesSys);
		ulPrev = ulCell;

		/* --- get chain cell no. --- */

		ulCell = pChainRec->ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- read it --- */

		if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
			return (usStat);
		}
	}

	/* --- is this record as synonym ? --- */

	if (! (pChainRec->fcInfo & REC_LEGAL_POS)) {

		/* --- copy chain field of deleted rec. --- */

		pChainPrev->fcInfo &= (UCHAR)(~REC_LINK_NEXT);
		pChainPrev->fcInfo |= (UCHAR)(pChainRec->fcInfo & REC_LINK_NEXT);
		pChainPrev->ulNext  = pChainRec->ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- adjust previous record's chain --- */

		if (usStat = PhyWriteRecord(hf, ulPrev, aucPrev, pHeader)) {
			return (usStat);
		}

		/* --- adjust sequential record ptr. --- */

		fEffect  = (pChainRec->fcInfo & REC_LINK_NEXT) ? TRUE : FALSE;
		ulAccess = pChainRec->ulNext;
	}

	/* --- else, is this an original with linking next --- */

	else if (pChainRec->fcInfo & REC_LINK_NEXT) {

		/* --- get chain cell no. --- */

		ulPrev   = ulCell;						/* remember current location*/
		ulCell   = pChainRec->ulNext;			/* get next chain address	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		fEffect  = TRUE;						/* set readable cell no.	*/
		ulAccess = ulPrev;						/* accessable cell no.		*/

		/* --- read it --- */

		if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
			return (usStat);
		}

		/* --- make it an original record --- */

		pChainRec->fcInfo |= REC_LEGAL_POS;

		/* --- move it to legal position --- */

		if (usStat = PhyWriteRecord(hf, ulPrev, aucRecord, pHeader)) {
			return (usStat);
		}
	}

	/* --- else, original rec. w/p linking next --- */

	else {
		fEffect  = FALSE;						/* no valid next cell		*/
		ulAccess = 0;							/* no accessable cell no.	*/
	}

	/* --- free the cell --- */

	if (usStat = PhySetUpOccupied(hf, ulCell, pHeader, FALSE)) {
		return (usStat);
	}

	/* --- adjust sequential record ptr. --- */

	DeleteRecordPtr(usFile, ulCell, ulAccess, fEffect);

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)--;						/* delete 1 record			*/

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record for relative file
;
;   format : USHORT		DelRecRelative(hf, pucMainKey, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucMainKey;	- pointer to main key
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	DelRecRelative(USHORT hf, WCHAR *pucMainKey, PKEYINF pKey, PFILEHDR pHeader)
{
	BOOL	fOccupy;
	USHORT	usStat, usFile;
	ULONG	ulCell, ulKey;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- get a key no. --- */

	ulKey = (pKey->usKeyType == TYPUCH)
							? (ULONG)(*pucMainKey) : (ULONG)(*(USHORT *)pucMainKey); /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- is valid key given ? --- */

	if (! ulKey) {								/* is the key 0 ?			*/
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}

	/* --- compute file cell no. --- */

	ulCell = ComputeRelative(ulKey);			/* compute record cell no.	*/

	/* --- is within a file --- */

	if (ulCell >= pHeader->ulMaxRec) {			/* if out of a file ...		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		return (SPLU_REC_NOT_FOUND);				/* give me a valid no.	*/
	}

	/* --- get current state --- */

	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is there a record ? --- */

	if (! fOccupy) {							/* is any record ?			*/
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- remove the record by index --- */

	if (usStat = PhySetUpOccupied(hf, ulCell, pHeader, FALSE)) {
		return (usStat);
	}

	/* --- adjust sequential record ptr. --- */

	usFile = HandleToFileNo(hf);
	DeleteRecordPtr(usFile, ulCell, 0, FALSE);

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)--;		/* delete 1 record			*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record for index file, R2.0
;
;   format : USHORT		DelRecIndex(hf, pvRec, pKey, pHeader);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	DelRecIndex(USHORT hf, VOID *pvRec, PFILEHDR pHeader)
{
	RECIDX	aBuffer[RECIDX_BUFFER];
	SHORT	sStatus;
	USHORT	usStat;
	ULONG	ulHitOffset, usBytes, ulWriteOffset, ulTailOffset;
#ifndef PEP
    HUGEINT   huProdWork;
#endif

	if (pHeader->ulCurRec == 0) {
		return (SPLU_REC_NOT_FOUND);				/* give me a valid no.	*/
    }
	/* --- compute the offset of index --- */

	sStatus = ComputeIndex(pvRec, hf, pHeader, &ulHitOffset, (WCHAR *)&aBuffer);

    if (sStatus < 0) {
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}
    
	/* --- is there a record ? --- */

	if (sStatus > 0) {								/* is already located ?		*/
		return (SPLU_REC_NOT_FOUND);				/* give me a valid no.	*/
	}

	/* --- write a record at the position --- */

    ulTailOffset = sizeof(FILEHDR);
#ifdef PEP
    ulTailOffset += (ULONG)((ULONG)sizeof(RECIDX) * (pHeader->ulCurRec -1L));
#else
    RflMul32by32(&huProdWork, (ULONG)sizeof(RECIDX), (pHeader->ulCurRec -1L));	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
    ulTailOffset += *((LONG *)(&huProdWork));
#endif
    ulWriteOffset = ulHitOffset;

    while(ulWriteOffset < ulTailOffset) {

        if ((ulTailOffset - ulWriteOffset) >= sizeof(aBuffer)) {

            usBytes = sizeof(aBuffer);
        } else {
            usBytes = (USHORT)(ulTailOffset - ulWriteOffset);
        }
        usStat = PhyReadFile(hf, (WCHAR *)&aBuffer, &usBytes, ulWriteOffset+sizeof(RECIDX));
            
    	usStat = PhyWriteFile(hf,                   /* write to a file			*/
						  (WCHAR *)&aBuffer,        /* data buffer ptr.		*/
						  &usBytes,                 /* data size / write	*/
						  ulWriteOffset);           /* file position		*/

        ulWriteOffset += usBytes;
    }

	/* --- adjust no. of records in header --- */

	(pHeader->ulCurRec)--;	/* delete 1 record	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- update file header --- */

	usStat = PhyUpdateInfo(hf, pHeader);

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record for hash file
;
;   format : USHORT		ReadRecHash(hf, pucBuffer, pucMainKey, usSubKey,
;												pKey, pHeader, pulCellInfo);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 UCHAR		*pucMainKey;	- pointer to main key
;			 USHORT		usSubKey;		- sub key no.
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 ULONG		*pulCellInfo;	- pointer to record cell ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ReadRecHash(USHORT   hf, WCHAR *pucBuffer, WCHAR *pucMainKey, USHORT usSubKey,
					PKEYINF pKey, PFILEHDR pHeader, ULONG *pulCellInfo)
{
	BOOL	fOccupy, fAdjItem;
	WCHAR	aucRecord[ENOUGHREC * 50];
	USHORT	usAdj, usStat;
	ULONG	ulCell;		/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PRECCNT	pChain;

	/* --- initialize --- */

	pChain = (RECCNT*)((BYTE *)aucRecord + pKey->usBytesRec);

	/* --- compute its record's hash value --- */

	ulCell = ComputeHash(pucMainKey, pKey->usKeyLen, pHeader->ulHashKey);	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- get current state --- */


	
	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is there a record ? --- */

	if (! fOccupy) {							/* is any record ?			*/
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- get the record --- */

	if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
		return (usStat);
	}

	/* --- is this an original record ? --- */

	if (! (pChain->fcInfo & REC_LEGAL_POS)) {	/* is it an oriinal rec. ?	*/
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- initialize before searching --- */

	fAdjItem = FALSE;							/* assume not adj. item		*/

	/* --- look for the record following list --- */

	while (FOREVER) {

		/* --- is same item number ? --- */

		if (! memcmp(aucRecord, pucMainKey, pKey->usKeyLen)) {

			/* --- is adjective field defined ? --- */

			if (! pKey->usSubKeyOffset) {		/* is adj. field defined ?	*/
				break;
			}

			/* --- evaluate adjective number --- */
			

			if (pKey->usSubKeyType == TYPUCH) {	/* is UCHAR type ?	*/		
				usAdj = (USHORT)(*((BYTE *)aucRecord + pKey->usSubKeyOffset));
			} else {							/* else, assume USHORT type	*/
				usAdj = *(USHORT *)((BYTE *)aucRecord + pKey->usSubKeyOffset);
			}

			/* --- is same adjective number ? --- */

			if (usAdj == usSubKey) {			/* same adjective number ?	*/
				break;
			}

			/* --- is whichever 0 adj. no. ? --- */

			if ((! usAdj) || (! usSubKey)) {	/* both have valid no. ?	*/
				usStat = SPLU_ADJ_VIOLATION;		/* adj. rule error !	*/
				break;								/* try to respond rec.	*/
			}

			/* --- is large adj. no. ? --- */

			if (usAdj > usSubKey) {				/* come to a large adj. ?	*/
				return (SPLU_REC_NOT_FOUND);		/* rec. not found		*/
			}

			/* --- mark as adj. item --- */

			fAdjItem = TRUE;					/* this must be adj. item	*/
		}

		/* --- else, is under searching adj. no. ? --- */

		else if (fAdjItem) {					/* under searching adj. ?	*/
			return (SPLU_REC_NOT_FOUND);			/* rec. not found		*/
		}

		/* --- is chained next rec. ? --- */

		if (! (pChain->fcInfo & REC_LINK_NEXT)) {
			return (SPLU_REC_NOT_FOUND);			/* rec. not found		*/
		}

		/* --- get chain cell no. --- */

		ulCell = pChain->ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- read it --- */

		if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
			return (usStat);
		}
	}

	/* --- respond it --- */

	if (pucBuffer) {							/* is buffer prepared ?		*/
		memcpy(pucBuffer, aucRecord, pKey->usBytesRec);
	}

	/* --- respond file cell information --- */

	*pulCellInfo = ulCell;						/* this rec.'s cell no.		*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record for relative file
;
;   format : USHORT		ReadRecRelative(hf, pucBuffer, pucMainKey,
;												pKey, pHeader, pulCellInfo);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 UCHAR		*pucMainKey;	- pointer to main key
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 ULONG		*pulCellInfo;	- pointer to record cell ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ReadRecRelative(USHORT hf, WCHAR *pucBuffer, WCHAR *pucMainKey,
					PKEYINF  pKey, PFILEHDR pHeader, ULONG *pulCellInfo)
{
	BOOL	fOccupy;
	USHORT	usStat;
	ULONG	ulCell, ulKey;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- get a key no. --- */

	ulKey = (pKey->usKeyType == TYPUCH)
							? (ULONG)(*pucMainKey) : (ULONG)(*(USHORT *)pucMainKey); /* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- is valid key given ? --- */

	if (! ulKey) {								/* is the key 0 ?			*/
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}

	/* --- compute file cell no. --- */

	ulCell = ComputeRelative(ulKey);			/* compute record cell no.	*/

	/* --- is within a file --- */

	if (ulCell >= pHeader->ulMaxRec) {			/* if out of a file ...		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		return (SPLU_REC_NOT_FOUND);				/* give me a valid no.	*/
	}

	/* --- get current state --- */

	if (usStat = PhyExamOccupied(hf, ulCell, pHeader, &fOccupy)) {
		return (usStat);
	}

	/* --- is there a record ? --- */

	if (! fOccupy) {							/* is any record ?			*/
		return (SPLU_REC_NOT_FOUND);				/* record not found !	*/
	}

	/* --- read it --- */

	if (pucBuffer) {							/* is buffer prepared ?		*/
		usStat = PhyReadRecord(hf, ulCell, pucBuffer, pHeader);
	}

	/* --- respond cell information --- */

	*pulCellInfo = ulCell;

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record for index file, R2.0
;
;   format : USHORT		ReadRecIndex(hf, pvRec, pKey, pHeader, pulCellInfo);
;
;   input  : USHORT		hf;				- file handle
;			 VOID		*pvRec;			- pointer to record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 ULONG		*pulCellInfo;	- pointer to record cell ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	ReadRecIndex(USHORT hf, WCHAR *pucBuffer, WCHAR *pvRec,
					PFILEHDR pHeader, ULONG *pulCellInfo)
{
	RECIDX	aBuffer[RECIDX_BUFFER];
	SHORT	sStatus;
	USHORT	usStat;
	ULONG	ulHitOffset, usBytes, ulOffsetSave;
#ifndef PEP
    HUGEINT	huProdWork;
    LONG	ulRemind;
#endif

	/* --- compute the offset of index --- */

	sStatus = ComputeIndex(pvRec, hf, pHeader, &ulHitOffset, (WCHAR *)&aBuffer);

    if (sStatus < 0) {
		return (SPLU_INVALID_DATA);					/* invalid key !		*/
	}
    
	/* --- respond cell information --- */

    ulOffsetSave = ulHitOffset;

    ulHitOffset -= sizeof(FILEHDR);
#ifdef PEP
    ulHitOffset /= (ULONG)sizeof(RECIDX);
#else
    RflCast32to64(&huProdWork, ulHitOffset);
    RflDiv64by32(&huProdWork, &ulRemind, (LONG)sizeof(RECIDX));
    ulHitOffset = *((LONG *)(&huProdWork));
#endif
	*pulCellInfo = ulHitOffset;

	/* --- is there a record ? --- */

	if (sStatus > 0) {								/* is already located ?		*/
		return (SPLU_REC_NOT_FOUND);				/* give me a valid no.	*/
    }

	/* --- read a record at the position --- */

    if (pucBuffer) {

        usBytes = sizeof(RECIDX);

        usStat = PhyReadFile(hf, pucBuffer, &usBytes, ulOffsetSave);
        
	} else {

        usStat = SPLU_COMPLETED;                    /* successed to find */
    }
	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Trace a record for hash file
;
;   format : USHORT
;			 TraceRecHash(hf, pucBuffer, pKey, pHeader, pSeqence);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 PSEQMNP	pSequence;		- sequence control table
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	TraceRecHash(USHORT hf, WCHAR *pucBuffer, PKEYINF pKey, PFILEHDR pHeader,
					PSEQMNP pSequence)
{
	BOOL	fSearchOriginal, fBreakRead;
	WCHAR	aucRecord[ENOUGHREC];
	UCHAR	fchFlags;
	USHORT	usStat;
	ULONG	ulCell, ulFrom;
	PRECCNT	pChain;

	/* --- initialize --- */

	pChain     = (RECCNT*)((BYTE *)aucRecord + pKey->usBytesRec);
	fchFlags   = pSequence->fchFlags;
	fBreakRead = FALSE;

	/* --- must be search original rec. ? --- */

	if ((! (fchFlags & SEQMNP_VALID_NO)) && (! (fchFlags & SEQMNP_READABLE))){
		fSearchOriginal = TRUE;						/* search original rec.	*/
		ulFrom = pSequence->ulHashNo;	/* from current hash	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- is this a first call ? --- */

	else if (fchFlags & SEQMNP_INITIAL) {		/* is first call ?			*/
		fSearchOriginal = TRUE;						/* search original rec.	*/
		ulFrom          = 0;						/* from top of the file	*/
	}

	/* --- else, get current accessing cell no. --- */

	else {										/* else, trace next rec.	*/
		fSearchOriginal = FALSE;					/* not original search	*/
		ulCell          = pSequence->ulCellNo;		/* cell no. to search	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		ulFrom          = pSequence->ulHashNo;		/* from current hash	*/
	}

	/* --- trace a record --- */

	while (FOREVER) {

		/* --- is searching an original rec. ? --- */

		if (fSearchOriginal) {					/* is under searching orig ?*/

			/* --- evaluate a record position --- */

			if (usStat = PhySearchOccupied(hf, ulFrom, pHeader, &ulCell)) {
				break;
			}

			/* --- remember its hash number --- */

			ulFrom = ulCell;						/* current hash no.		*/
		}

		/* --- read it --- */

		if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
			break;
		}

		/* --- is readble rec. directly ? --- */

		if ((fchFlags & SEQMNP_READABLE) || fBreakRead) {
			break;									/* break searching		*/
		}

		/* --- is searching an original rec. & found it ? --- */

		if (fSearchOriginal && (pChain->fcInfo & REC_LEGAL_POS)) {
			break;									/* break to search		*/
		}

		/* --- is chained next rec. ? --- */

		if ((! fSearchOriginal) && (pChain->fcInfo & REC_LINK_NEXT)) {
			ulCell     = pChain->ulNext;			/* read next rec.		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			fBreakRead = TRUE;						/* respond its rec.		*/
			continue;								/* continue to search	*/
		}

		/* --- go to search an original rec. --- */

		fSearchOriginal = TRUE;					/* search an original		*/
		ulFrom++;								/* try next position		*/
	}

	/* --- could be found ? --- */

	if (! usStat) {

		/* --- respond only record --- */

		memcpy(pucBuffer, aucRecord, pKey->usBytesRec);

		/* --- adjust current data ptr. --- */

		pSequence->fchFlags &= ~SEQMNP_INITIAL;	/* not the 1st call			*/
		pSequence->fchFlags &= ~SEQMNP_READABLE;/* not readable directly	*/
		pSequence->fchFlags |= SEQMNP_VALID_NO;	/* mark as valid cell no.	*/
		pSequence->ulCellNo  = ulCell;			/* remember its cell no.	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		pSequence->ulHashNo = ulFrom;			/* remember its hash no.	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Trace a record for relative file
;
;   format : USHORT
;			 TraceRecRelative(hf, pucBuffer, pKey, pHeader, pSeqence);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 PSEQMNP	pSequence;		- sequence control table
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	TraceRecRelative(USHORT hf, WCHAR *pucBuffer, PKEYINF pKey, PFILEHDR pHeader,
					PSEQMNP pSequence)
{
	USHORT	usStat;
	ULONG	ulFrom, ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* --- compute current cell no. --- */

	if (pSequence->fchFlags & SEQMNP_INITIAL) {	/* is the first call ?		*/
		ulFrom = 0;									/* from top of cell		*/
	} else {									/* else, after second call	*/
		ulFrom = pSequence->ulCellNo + 1;			/* from next cell		*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- search a next record --- */

	if (usStat = PhySearchOccupied(hf, ulFrom, pHeader, &ulCell)) {
		return (usStat);
	}

	/* --- read it --- */

	if (! (usStat = PhyReadRecord(hf, ulCell, pucBuffer, pHeader))) {

		/* --- adjust current data ptr. --- */

		pSequence->fchFlags &= ~SEQMNP_INITIAL;	/* not the 1st call			*/
		pSequence->fchFlags |= SEQMNP_VALID_NO;	/* mark as valid cell no.	*/
		pSequence->ulCellNo  = ulCell;			/* remember its cell no.	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Trace a record for maintenance file
;
;   format : USHORT
;			 TraceRecMainte(hf, pucBuffer, pKey, pHeader, pSeqence);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 PSEQMNP	pSequence;		- sequence control table
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	TraceRecMainte(USHORT hf, WCHAR *pucBuffer, PKEYINF pKey, PFILEHDR pHeader,
					PSEQMNP pSequence)
{
	BOOL	fReadNext;
	WCHAR	aucRecord[ENOUGHREC];
	USHORT	usStat;
	ULONG	ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	PMNTCNT	pChain;

	/* --- initialize --- */

	pChain    = (PMNTCNT)((BYTE *)aucRecord + pKey->usBytesRec);
	fReadNext = FALSE;

	/* --- is this a first call ? --- */

	if (pSequence->fchFlags & SEQMNP_INITIAL) {

		/* --- evaluate first cell no. --- */

		ulCell = (pHeader->queListMainte).ulQueFirst;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

		/* --- is any record on the list ? --- */

		if (! (pHeader->queListMainte).ulQueCount) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			return (SPLU_END_OF_FILE);				/* assume EOF			*/
		}
	}

	/* --- else, readable directly ? --- */

	else if (pSequence->fchFlags & SEQMNP_READABLE) {
		ulCell = pSequence->ulCellNo;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- else, get next chain address ? --- */

	else if (pSequence->fchFlags & SEQMNP_VALID_NO) {
		ulCell    = pSequence->ulCellNo;
		fReadNext = TRUE;
	}

	/* --- else, assume come to an end of file --- */

	else {
		return (SPLU_END_OF_FILE);					/* assume EOF			*/
	}		

	/* --- search a target record --- */

	while (FOREVER) {

		/* --- read it --- */

		if (usStat = PhyReadRecord(hf, ulCell, aucRecord, pHeader)) {
			break;
		}

		/* --- is enough to read this rec. ? --- */

		if (! fReadNext) {

			/* --- respond only record --- */

			memcpy(pucBuffer, aucRecord, pKey->usBytesRec);

			/* --- adjust current data ptr. --- */

			pSequence->fchFlags &= ~SEQMNP_INITIAL;	/* not the 1st call		*/
			pSequence->fchFlags &= ~SEQMNP_READABLE;/* not readable directly*/
			pSequence->fchFlags |= SEQMNP_VALID_NO;	/* mark as valid cell no*/
			pSequence->ulCellNo  = ulCell;			/* remember its cell no.*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			break;
		}

		/* --- does chain to next ? --- */

		if (pChain->ulNext == MNT_NO_LINKED) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			usStat = SPLU_END_OF_FILE;
			break;
		}

		/* --- get chain address --- */

		ulCell    = pChain->ulNext;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		fReadNext = FALSE;
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Trace a record for hash file by index file, R2.0
;
;   format : USHORT
;			 TraceRecIndex(hf, pucBuffer, pKey, pHeader, pSeqence);
;
;   input  : USHORT		hf;				- file handle
;			 UCHAR		*pucBuffer;		- buffer ptr. to respond record
;			 PKEYINF	pKey;			- pointer to key information
;			 PFILEHDR	pHeader;		- pointer to file header
;			 PSEQMNP	pSequence;		- sequence control table
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	TraceRecIndex(USHORT hf, USHORT usManip, USHORT usFile, WCHAR *pucBuffer,
					PKEYINF pKey, PFILEHDR pHeader, PSEQMNP pSequence)
{
	UCHAR	fchFlags;
	USHORT	usStat;
	ULONG	ulCell;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
    RECIDX  RecIdx;

	/* --- initialize --- */

	fchFlags   = pSequence->fchFlags;

	/* --- is this a first call ? --- */

	if (fchFlags & SEQMNP_INITIAL) {                /* is first call ?			*/
		ulCell = 0;                                 /* from top of the file	*/
	}

	/* --- must be search original rec. ? --- */

	else {										/* else, trace next rec.	*/
		ulCell  = pSequence->ulHashNo;          /* from current hash	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	if (ulCell >= pHeader->ulCurRec) {			/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		return (SPLU_END_OF_FILE);              /* end of file          */
	}

	/* --- trace a record --- */

    /* --- read index file --- */

    if (usStat = PhyReadRecord(hf, ulCell, (WCHAR *)&RecIdx, pHeader)) {

        if (usStat == SPLU_FILE_OVERFLOW) {

            usStat = SPLU_END_OF_FILE;
        }
        return (usStat);
    }

    if (ADJUSTFILE(usFile) == FILE_PLU_INDEX) {
        
    	/* --- respond only index record --- */

		memcpy(pucBuffer, &RecIdx, sizeof(RECIDX));

        usStat = SPLU_COMPLETED;                    /* successed to find */

    } else {

        /* --- read the target record from the hash file --- */

	    usStat = MnpReadRecord(usFile,      /* read the record			*/
                            usManip,		/* manipulation handle	*/
			    			(WCHAR *)&RecIdx.aucPluNumber,/* ptr. to key			*/
				    		(USHORT)RecIdx.uchAdjective,/* adjective number		*/
					    	pucBuffer);			 /* buffer ptr.			*/
    }

	/* --- could be found ? --- */

	if (! usStat) {

		ulCell++;

		/* --- adjust current data ptr. --- */

		pSequence->fchFlags &= ~SEQMNP_INITIAL;	/* not the 1st call			*/
		pSequence->fchFlags &= ~SEQMNP_READABLE;/* not readable directly	*/
		pSequence->fchFlags |= SEQMNP_VALID_NO;	/* mark as valid cell no.	*/
		pSequence->ulHashNo = ulCell;			/* remember its cell no.	*//* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	}

	/* --- exit ... --- */

	return (usStat);
}

/*==========================================================================*\
;+																			+
;+				M I S C .    S U B - R O U T I N E s						+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Evaluate file information
;
;   format : USHORT		EvaluateFileInfo(usFile, pusHandle, ppInfo);
;
;   input  : USHORT		usFile;			- file ID
;			 USHORT		usHandle;		- file access handle
;			 PINFTBL	*ppInfo;		- buffer ptr. to receive table ptr.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	EvaluateFileInfo(USHORT usFile, USHORT *pusHandle, PINFTBL *ppInfo)
{
	USHORT		usRealFile, usStat;
	FILEHDR		hdrFile;
	PINFTBL		pTable;

	/* --- initialize --- */

	*pusHandle = 0;								/* init. access handle		*/
	*ppInfo    = (PINFTBL)NULL;					/* init. table ptr.			*/
	usRealFile = usFile;						/* assume file ID			*/

	/* --- is mirror file ? --- */

	if (ISMIRROR(usFile)) {						/* is valid file ID given ?	*/

		/* --- get header information of the file --- */

		if (usStat = PhyGetInfo(HFMIRROR(usFile), &hdrFile)) {
			return (usStat);
		}

		/* --- get file ID from header --- */

		usRealFile = hdrFile.usFileNumber;		/* get file number			*/
	}

	/* --- look for file relation table --- */

	if (! (pTable = SearchFileInfo(usRealFile))) {
		return (SPLU_INVALID_DATA);					/* bye bye, wrong data !*/
	}

	/* --- respond file handle to access --- */

	*pusHandle = ISMIRROR(usFile) ? HFMIRROR(usFile) : *(pTable->pusHandle);
	*ppInfo    = pTable;

	/* --- is created file ? --- */

	usStat = *pusHandle ? 0 : SPLU_FILE_NOT_FOUND;	/* evaluate handle		*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Search a file relation table
;
;   format : PINFTBL	SearchFileInfo(usFile);
;
;   input  : USHORT		usFile;			- file ID
;
;   output : PINFTBL	pInfo;			- pointer to table
;
;	notes  : If pInfo equals to NULL, could not be found.
;
;========================================================================
**/

PINFTBL	SearchFileInfo(USHORT usFile)
{
	PINFTBL		pTable;

	/* --- looking for file relation table --- */

	for (pTable = FileInfo; pTable->usFileNo != usFile; pTable++) {

		/* --- found a table ? --- */

		if (pTable->usFileNo == TBLEND) {
			pTable = (PINFTBL)NULL;
			break;
		}
	}

	/* --- exit ... --- */

	return (pTable);
}

/**
;========================================================================
;
;   function : Search a file relation table
;
;   format : PKEYINF    ReplaceKeyInfo(usFile, usFileVer, pKey);
;
;   input  : USHORT		usFile;			- file ID
;            USHORT     usFileVer;      - file version
;            PKEYINF    pKey;           - key information
;
;   output : PINFTBL	pKey;			- pointer to key information
;
;	notes  : replace key information for old file migration, R2.0
;
;========================================================================
**/

PKEYINF ReplaceKeyInfo(USHORT usFile, USHORT usFileVer, PKEYINF pKey)
{
    if (ADJUSTFILE(usFile) == FILE_PLU) {

        if (usFileVer == PLU_FILE_R10) {

            pKey = Plu10FileKey;    /* correct as r1.0 plu file */
        }
    }

    return (pKey);
}

/**
;========================================================================
;
;   function : Compute file no. from file handle
;
;   format : USHORT		HandleToFileNo(hf);
;
;   input  : USHORT		hf;				- file handle
;
;   output : USHORT		usFileNo;		- file no.
;
;========================================================================
**/

USHORT	HandleToFileNo(USHORT hf)
{
	USHORT		usFileNo, usStat;
	FILEHDR		hdrFile;
	PINFTBL		pTable;

	/* --- initialize --- */

	usFileNo = 0;								/* assume not found			*/

	/* --- looking for file relation table --- */

	for (pTable = FileInfo; pTable->usFileNo != TBLEND; pTable++) {

		/* --- found the same file handle ? --- */

		if (*(pTable->pusHandle) == hf) {
			usFileNo = pTable->usFileNo;
			break;
		}
	}

	/* --- could be found ? --- */

	if (! usFileNo) {							/* not found file no. ?		*/

		/* --- get header information of the file --- */

		if (! (usStat = PhyGetInfo(hf, &hdrFile))) {

			/* --- get file ID from header --- */

			usFileNo = hdrFile.usFileNumber;		/* get file number		*/
		}
	}

	/* --- exit ... --- */

	return (usFileNo);
}

/**
;========================================================================
;
;   function : Move a record pointer
;
;   format : VOID		MoveRecordPtr(usFile, ulCell, ulMove);
;
;   input  : USHORT		usFile;			- file no.
;			 ULONG		ulCell;			- cell no. to be moved	### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 ULONG		ulMove;			- new cell no.			### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : nothing
;
;========================================================================
**/

VOID	MoveRecordPtr(USHORT usFile, ULONG ulCell, ULONG ulMove)
{
	USHORT		i;
	PSEQMNP		pManip;

	/* --- initialize --- */

	pManip = aManipBlock;						/* top of control buffer	*/

	/* --- trace whole control buffer array --- */

	for (i = 0; i < ARRAYS(aManipBlock, SEQMNP); i++) {

		/* --- is used array ? --- */

		if (pManip->fchFlags & SEQMNP_IN_USE) {

			/* --- is holding a valid cell no. ? --- */

			if (pManip->fchFlags & SEQMNP_VALID_NO) {

				/* --- is same file no. ? --- */

				if (pManip->usFileNo == usFile) {

					/* --- is same cell no. ? --- */

					if (pManip->ulCellNo == ulCell) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
						pManip->ulCellNo = ulMove;	/* move it to new cell	*/
					}
				}
			}
		}

		/* --- go ahead to next array --- */

		pManip++;
	}

	/* --- exit ... --- */

	return;
}

/**
;========================================================================
;
;   function : Delete a record pointer
;
;   format : VOID		DeleteRecordPtr(usFile, ulCell, ulAccess, fEffect);
;
;   input  : USHORT		usFile;			- file no.
;			 ULONG		ulCell;			- cell no. to be removed	### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 ULONG		ulAccess;		- accessable cell no.		### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 BOOL		fEffect;		- exist accessable cell no. or not
;
;   output : nothing
;
;========================================================================
**/


VOID	DeleteRecordPtr(USHORT usFile, ULONG ulCell, ULONG ulAccess, BOOL fEffect)
{
	USHORT		i;
	PSEQMNP		pManip;

	/* --- initialize --- */

	pManip = aManipBlock;						/* top of control buffer	*/

	/* --- trace whole control buffer array --- */

	for (i = 0; i < ARRAYS(aManipBlock, SEQMNP); i++) {

		/* --- is used array ? --- */

		if (pManip->fchFlags & SEQMNP_IN_USE) {

			/* --- is holding a valid cell no. ? --- */

			if (pManip->fchFlags & SEQMNP_VALID_NO) {

				/* --- is same file no. ? --- */

				if (pManip->usFileNo == usFile) {

					/* --- is same cell no. ? --- */

					if (pManip->ulCellNo == ulCell) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

						/* --- move to new accessable cell --- */

						pManip->ulCellNo  = ulAccess;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
						pManip->fchFlags &= ~SEQMNP_VALID_NO;
						pManip->fchFlags |= fEffect ? SEQMNP_READABLE : 0;
					}
				}
			}
		}

		/* --- go ahead to next array --- */

		pManip++;
	}

	/* --- exit ... --- */

	return;
}

/* #pragma	optimize("egl", off) *//* ### DEBUG */
/**
;========================================================================
;
;   function :	Compute Hash Value
;
;   format : ULONG		ComputeHash(pucKey, usLen, ulBase);### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   input  : UCHAR FAR	*pucKey;	- pointer to key number (Packed BCD)
;			 USHORT		usLen;		- length of key number
;			 ULONG		ulBase;		- base value ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : ULONG		ulHash;		- hash value ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;========================================================================
**/

ULONG	ComputeHash(WCHAR *pucKey, USHORT usLen, ULONG ulBase)
{
	/* ### New 2172 Rel1.0 */
	// PLU length has been more than 7 digits since NHPOS Rel 1.4
	// Just use the ComputerHast_64() function always.
	//    Richard Chambers, Mar-08-2023
	return	ComputeHash_64(pucKey, usLen, ulBase);

#if defined(POSSIBLE_DEAD_CODE)
	ULONG	ulHash;			/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
#if	!defined (SMARTALG)
	ULONG	ulSum;
#endif

	/* ### New 2172 Rel1.0 */
	if (usLen > 7){
		return	ComputeHash_64(pucKey,usLen,ulBase);
	}

	/* --- compute hash value --- */

	_asm {

		push	bx							; save registers ### MOD 2172 Rel1.0    Saratoga
		push	cx							; Saratoga
		push	dx							; Saratoga
		push	si							; Saratoga
		push	di							; Saratoga
		;;;push	ds							; ### DEL 2172 Rel1.0

		;--- initialize ---
		xor		ax, ax					; ### ADD 2172 Rel1.0   Saratoga
		xor		cx, cx					; Saratoga
		xor		dx, dx					; Saratoga

		cld								; reset F(D)=0, forward direction
		xor		bx, bx					; ### MOD 2172 Rel1.0       Saratoga
		xor		di, di					; (di:bx) = work reg.       Saratoga
		mov		cx, usLen					; (cx) = no. of bytes
		jcxz	ComputeH04					; break if no data

		;--- calculate hash value ( stage no. 1 ) ---

		lds		si, pucKey					; (ds:si) = pucKey          Saratoga
		;mov		esi,pucKey					; ### MOD 2172 Rel1.0   Saratoga
		inc		si							; advance data ptr. by 1    Saratoga
		shr		cx, 1						; (cx) = no. of words
		cmc									; F(CY) = 1 <-> 0
		sbb		cx, 0						; adjust counter
		jcxz	ComputeH02					; skip if no need to compute
ComputeH01:
		lodsw								; get key data
		xchg	ah, al						; set INTEL format
		call	ConvBcd2Hex					; convert to HEX data
		mul		ax							; (ax) = (ax) * (ax)
		add		bx, ax						;
		adc		di, dx						; (di:bx) = total value
		loop	ComputeH01					; loop by no. of digits

			;--- multiple by 3 ---

		mov		ax, bx						;
		mov		dx, di						; (dx:ax) = cur. total
		shl		ax, 1						;
		rcl		dx, 1						; (dx:ax) = (dx:ax) * 2
		add		bx, ax						;
		adc		di, dx						; (di:bx) = (di:bx) * 3

		;--- calculate hash value ( stage no. 2 ) ---
ComputeH02:
        mov     si, word ptr pucKey[0]      ; (ds:si) = pucKey                  Saratoga
;        mov		si, pucKey			; (ds:si) = pucKey ### MOD 2172 Rel1.0  Saratoga
		mov		cx, usLen					; (cx) = no. of bytes
		shr		cx, 1						; (cx) = no. of words
		jcxz	ComputeH04					; break if only 1 byte
ComputeH03:
		lodsw								; get key data
		xchg	ah, al						; chane to little endianness
		call	ConvBcd2Hex					; convert to HEX data
		mul		ax							; (ax) = (ax) * (ax)
		add		bx, ax						;
		adc		di, dx						; (di:bx) = total value
		loop	ComputeH03					; loop by no. of digits

		;--- calculate hash value ( stage no. 3 ) ---
ComputeH04:
#if	!defined (SMARTALG)
		mov		word ptr ulSum[0], bx		;
		mov		word ptr ulSum[2], di		; save calculated sum
#else
		mov		ax, bx						;
		mov		dx, di						; (dx:ax) = dividend
		mov		bx, usBase					;
		xor		cx, cx						; (cx:bx) = divisor
		call	DivideLong					; long division
		mov		usHash, bx					; (bx) = modulus
#endif
		;--- recover registers ---

		;;;pop		ds							; recover registers
		pop		di							; Saratoga
		pop		si							; Saratoga
		pop		dx							; Saratoga
		pop		cx							; Saratoga
		pop		bx							; Saratoga
	}

	/* --- compute hash --- */

#if	!defined (SMARTALG)
	ulHash = ulSum % ulBase;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
#endif

	/* --- respond hash value --- */

	return ulHash;				/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */

	/* ------------------------------------------------------ */
	/* --- Listed below are some sub-routines to help it. --- */
	/* ------------------------------------------------------ */

	_asm {

#if	defined (SMARTALG)
	;========================================================
	;
	; Func.  : long (32 bits) division
	;
	; Input  : (dx:ax) = dividend
	;		   (cx:bx) = divisor
	;
	; Output : (dx:ax) = (dx:ax) div (cx:bx)
	;		   (cx:bx) = (dx:ax) mod (cx:bx)
	;
	; Broken : none
	;
	;========================================================
DivideLong:
		push	bp					; save registers
		push	si					;
		push	di					;

    	;--- divisor == 0 ? ---

		mov		bp, cx				; (bp:bx) = divisor
		or		cx, bx				; check divisor
		jnz		DivideL00			;

			;--- call 0 divide exception ---

		int		0					; 0 divide exception
		jmp		short DivideL04		; exit if divisor = 0

		;--- calculate long division ---
DivideL00:
		xor		si, si				;
		xor		di, di				; (di:si) = 0 ... work reg
		mov		cx, 32				; (cx) = loop counter
DivideL01:
		shl		ax, 1				;
		rcl		dx, 1				; (dx:ax) shl 1
		rcl		si, 1				;
		rcl		di, 1				; (di:si) shl 1 w/ high bit (dx:ax)
		sub		si, bx				; test to dec.
		sbb		di, bp				;
		jnc		DivideL02			; skip if not overflow

		add		si, bx				; return to orig. data
		adc		di, bp				;
		jmp		short DivideL03		;
DivideL02:
		inc		al					; active LSB bit
DivideL03:
		loop	DivideL01			; loop by 32 bits
	
		;--- exit ... ---

		mov		cx, di				;
		mov		bx, si				; (di:si) = modulus
DivideL04:
		pop		di					; recover registers
		pop		si					;
		pop		bp					;
		ret							;
#endif
	;========================================================
	;
	; Func.  : Convert BCD data into hex data
	;
	; Input  : (ax) = BCD data
	;
	; Output : (ax) = hash value
	;
	; Broken : none
	;
	;========================================================
ConvBcd2Hex:
		push	bx				; save regsiters
		mov		bx, ax			; (bx) = org. data
		mov		al, bh			; (al) = 1000'th & 100'th grade BCD data
		call	Bcd2Hex			; convert to hex data
		mov		ah, 100			;
		mul		ah				; (ax) = 1000'th & 100'th grade hex data
		xchg	ax, bx			; (bx) = 10'th & 1'st grade BCD data
		call	Bcd2Hex			;
		xor		ah, ah			; (ah) = 0
		add		ax, bx			; (ax) = HEX data
		pop		bx				;
		ret						;

	;========================================================
	;
	; Func.  : Convert BCD data into hex data in byte
	;
	; Input  : (al) = BCD data
	;
	; Output : (al) = hex data
	;
	; Broken : none
	;
	;========================================================
Bcd2Hex:
		push	bx				; save registers
		mov		bl, al			; (bl) = org. data
		and		bl, 0Fh			; (bl) = 1'st grade BCD data
		and		al, 0F0h		; (al) = 10'th grade BCD data
		ror		al, 1			;
		mov		bh, al			;
		ror		al, 2			;
		add		al, bh			;
		add		al, bl			;
		pop		bx				; recover registers
		ret						;
	}
#endif
}
/* #pragma	optimize("egl", on) *//* ### DEBUG */

/**
;========================================================================
;
;   function :	Compute Relative Value offset
;
;   format : ULONG		ComputeRelative(ulKey); ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   input  : ULONG		ulKey;		- key value ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : ULONG 		usOffset;	- offset value
;
;========================================================================
**/

ULONG	ComputeRelative(ULONG ulKey)
{
	return (ulKey - 1);
}


/**
;========================================================================
;
;   function :	Compute Index Value offset, R2.0
;
;   format : SHORT		ComputeIndex(usKey, usHandle, pHeader, *pulOffset, *auchBuffer);
;
;   input  : USHORT		usKey;		- key value
;            USHORT     usHandle;   - handle for index file
;            PFILEHDR   pHeader;    - file header
;            UCHAR      auchBuffer; - search buffer
;
;   output : ULONG      pulOffset;	- offset value
;
;========================================================================
**/

SHORT	ComputeIndex(WCHAR *pucKey, USHORT usHandle, PFILEHDR pHeader,
                        ULONG *pulOffset, WCHAR *auchBuffer)
{
	RECIDX	*pHitPoint;									/* search hit point */
	ULONG	usBytes;
	ULONG	ulBufferRec;								/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
	SHORT   sStatus = RFL_TAIL_OF_TABLE;				/* return status save area */
	ULONG	ulHeadBlock, ulCenterBlock, ulTailBlock;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
#ifndef PEP
	HUGEINT	huProdWork;
#endif

    /* ---- initialize no. of block, center block and read block */
    ulHeadBlock = 0;
    ulTailBlock = pHeader->ulCurRec / RECIDX_BUFFER;	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
    ulCenterBlock = ulTailBlock / 2;

    while (sStatus != RFL_HIT) {

        *pulOffset = sizeof(FILEHDR);
#ifdef PEP
        *pulOffset += (ULONG)(ulCenterBlock * (ULONG)RECIDX_BUFFER * (ULONG)sizeof(RECIDX));    /* Saratoga */
#else
        RflMul32by32(&huProdWork, ulCenterBlock, (ULONG)RECIDX_BUFFER);
        RflMul32by32(&huProdWork, *((ULONG *)(&huProdWork)), (ULONG)sizeof(RECIDX));
        *pulOffset += *((ULONG *)&huProdWork);
#endif
        if (ulCenterBlock == pHeader->ulCurRec / RECIDX_BUFFER) {	/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
			ulBufferRec = pHeader->ulCurRec - (ulCenterBlock * RECIDX_BUFFER);

            if (ulBufferRec == 0) {
                return (1); /* not found, but insert position is *pulOffset */
            } else
            if (ulBufferRec < 0) {
                return(-1);     /* failed to find */
            }
        } else {
			ulBufferRec = RECIDX_BUFFER;
        }

		usBytes = (USHORT)(ulBufferRec * sizeof(RECIDX));			/* ### MOD 2172 Rel1.0 (USHORT->ULONG) */
		_PLU_ASSERT("ComputeIndex",NULL,(65535 >= (ulBufferRec * sizeof(RECIDX))));	/* ### ADD 2172 Rel1.0 */

		if ((SPLU_COMPLETED == PhyReadFile(usHandle, auchBuffer, &usBytes, *pulOffset)) 
            && (usBytes)) {

#ifdef PEP
            sStatus = Rfl_SpBsearch_PLU32(pucKey, sizeof(RECIDX),   
                                auchBuffer,     /* read buffer */
                                ulBufferRec,     /* number of key  */
                                (VOID FAR *)&pHitPoint, /* hit point */
                                (SHORT (FAR *)(VOID *pKey, VOID *pusHPoint))CompIndex); /* compare routine */

#else
            sStatus = Rfl_SpBsearch_PLU32(pucKey, sizeof(RECIDX),   
                                auchBuffer,     /* read buffer */
                                ulBufferRec,     /* number of key  */
                                &pHitPoint,     /* hit point */
                                pComp);         /* compare routine */
#endif

            if (sStatus == RFL_TAIL_OF_TABLE) {
                if (ulCenterBlock < ulTailBlock) {

                    ulHeadBlock = ++ulCenterBlock;
                    ulCenterBlock = (ulTailBlock - ulHeadBlock) / 2;
                    ulCenterBlock += ulHeadBlock;
                    continue;
                }
            } else
            if (sStatus == RFL_HEAD_OF_TABLE) {
                if (ulCenterBlock > ulHeadBlock) {

                    ulTailBlock = --ulCenterBlock;
                    ulCenterBlock = (ulTailBlock - ulHeadBlock)/2;
                    ulCenterBlock += ulHeadBlock;
                    continue;
                }
            }

            *pulOffset += (ULONG)((UCHAR *)pHitPoint - (UCHAR *)auchBuffer);
            if (sStatus == RFL_HIT) {
                return (0); /* sccessed to find */
            } else {
                return (1); /* not found, but insert position is *pulOffset */
            }
        } else {
            return(-1);     /* failed to find */
        }
    }
    return(-1);     /* failed to find *//* ### ADD 2172 Rel1.0 */
}

/*
*==========================================================================
**    Synopsis:   SHORT    CompIndex(RECIDX *pKeyRecIdx, 
*                                        RECIDX *pHitPoint )
*
*       Input:    *pKeyRecIdx      Target key
*                 *pHitPoint       Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :   Normal End: 0x00   : Match
*              Abnormal End: < 0x00 : Target key > Checked key
*                            > 0x00 : Target key < Checked key    
*
**  Description: Compare routine for index fie (R2.0).
*==========================================================================
*/
SHORT    CompIndex(RECIDX *pKeyRecIdx, RECIDX *pHitPoint)
{
    return ((SHORT)memcmp(pKeyRecIdx, pHitPoint, sizeof(RECIDX)));
}

/*==========================================================================*\
;+																			+
;+				T A B L E    D A T A    o n    R O M    A R E A				+
;+																			+
\*==========================================================================*/



/**
;========================================================================
;
;   function : Record key information
;
;   format : KEYINF		PluFileKey[];
;			 KEYINF		MixFileKey[];
;			 KEYINF		PpiFileKey[];
;			 KEYINF		PluMaintKey[];
;			 KEYINF		PluIndexKey[];
;
;   input  : nothing
;
;	output : PRECDIC	PluFileKey[].pDictionary;
;			 USHORT		PluFileKey[].usKeyLen;
;			 USHORT		PluFileKey[].usKeyType;
;			 USHORT		PluFileKey[].usSubKeyOffset;
;			 USHORT		PluFileKey[].usSubKeyType;
;			 USHORT		PluFileKey[].usBytesRec;
;			 USHORT		PluFileKey[].usBytesSys;
;
;========================================================================
**/

KEYINF	FARCONST	PluFileKey[] = {			/* key info. of PLU file	*/
						PluRecord,						/* ptr. to rec. dic	*/
						LONGOF(aucPluNumber, RECPLU),	/* main key length	*/
						TYPBCD,							/* main key type	*/
						FAROF(uchAdjective, RECPLU),	/* sub key offset	*/
						TYPUCH,							/* sub key type		*/
						sizeof(RECPLU),					/* record length	*/
						sizeof(RECCNT)					/* system field		*/
					};

KEYINF	FARCONST	MixFileKey[] = {			/* key info. of mix file	*/
						MixRecord,						/* ptr. to rec. dic	*/
						LONGOF(uchMixMatch, RECMIX),	/* main key length	*/
						TYPUCH,							/* main key type	*/
						0,								/* sub key offset	*/
						0,								/* sub key type		*/
						sizeof(RECMIX),					/* record length	*/
						0								/* system field		*/
					};
#ifdef _DEL_2172_R10 /* ### DEL 2172 Rel1.0 */
KEYINF	FARCONST	PpiFileKey[] = {			/* key info. of PPI file	*/
						PpiRecord,						/* ptr. to rec. dic	*/
						LONGOF(uchPPI, RECPPI),			/* main key length	*/
						TYPUCH,							/* main key type	*/
						0,								/* sub key offset	*/
						0,								/* sub key type		*/
						sizeof(RECPPI),					/* record length	*/
						0								/* system field		*/
					};
#endif /* _DEL_2172_R10 */
KEYINF	FARCONST	PluMaintKey[] = {			/* key info. of PLU Mainte.	*/
						PluMainte,						/* ptr. to rec. dic	*/
						0,								/* main key length	*/
						0,								/* main key type	*/
						0,								/* sub key offset	*/
						0,								/* sub key type		*/
						sizeof(MNTPLU),					/* record length	*/
						sizeof(MNTCNT)					/* system field		*/
					};

KEYINF	FARCONST	PluIndexKey[] = {			/* key info. of mix file	*/
						IndexRecord,					/* ptr. to rec. dic	*/
						LONGOF(aucPluNumber, RECPLU),	/* main key length	*/
						TYPBCD,							/* main key type	*/
						FAROF(uchAdjective, RECPLU),	/* sub key offset	*/
						TYPUCH,							/* sub key type		*/
						sizeof(RECIDX), 				/* record length	*/
						0								/* system field		*/
					};

KEYINF	FARCONST	Plu10FileKey[] = {			/* key info. of PLU R1.0 file	*/
						PluRecordR10,					/* ptr. to rec. dic	*/
						LONGOF(aucPluNumber, RECPLU22),	/* main key length	*/
						TYPBCD,							/* main key type	*/
						FAROF(uchAdjective, RECPLU22),	/* sub key offset	*/
						TYPUCH,							/* sub key type		*/
						sizeof(RECPLU22),				/* record length	*/
						sizeof(RECCNT)					/* system field		*/
					};


/**
;========================================================================
;
;   function : Record field Dictionary
;
;   format : KEYINF		PluFileKey[];
;			 KEYINF		MixFileKey[];
;			 KEYINF		PpiFileKey[];
;			 KEYINF		PluMaintKey[];
;			 KEYINF		PluIndexKey[];
;
;   input  : nothing
;
;   output : USHORT		PluFileKey[].usKeyLen;
;			 USHORT		PluFileKey[].usKeyType;
;			 USHORT		PluFileKey[].usSubKeyOffset;
;			 USHORT		PluFileKey[].usSubKeyType;
;			 USHORT		PluFileKey[].usBytesRec;
;			 USHORT		PluFileKey[].usBytesSys;
;
;========================================================================
**/

RECDIC	FARCONST	PluRecord[] = {
 { PLUID_PLU_NUMBER,	TYPBIN,
	FAROF(aucPluNumber,      RECPLU), LONGOF(aucPluNumber,      RECPLU)	},
 { PLUID_ADJECTIVE,		TYPUCH,
	FAROF(uchAdjective,		 RECPLU), LONGOF(uchAdjective,	    RECPLU)	},
 { PLUID_DEPT_NUMBER,	TYPUCH,
	FAROF(usDeptNumber,	 RECPLU), LONGOF(usDeptNumber,	    RECPLU)	}, /* ### MOD 2172 Rel1.0 */
 { PLUID_UNIT_PRICE,	TYPULG,
	FAROF(ulUnitPrice,		 RECPLU), LONGOF(ulUnitPrice,	    RECPLU)	},
 { PLUID_PMUL_WEIGHT,	TYPUCH,
	FAROF(uchPriceMulWeight, RECPLU), LONGOF(uchPriceMulWeight, RECPLU)	},
 { PLUID_MNEMONIC,		TYPBIN,
	FAROF(aucMnemonic,		 RECPLU), LONGOF(aucMnemonic,		RECPLU)	},
 { PLUID_STATUS,		TYPBIN,
	FAROF(aucStatus,		 RECPLU), LONGOF(aucStatus,		    RECPLU)	},
 { PLUID_FAMILY_CODE,	TYPUSH,
	FAROF(usFamilyCode,		 RECPLU), LONGOF(usFamilyCode,	    RECPLU)	},/* ### ADD Saratoga (051700) */
 { PLUID_MMATCH_TARE,	TYPUCH,
	FAROF(uchMixMatchTare,	 RECPLU), LONGOF(uchMixMatchTare,	RECPLU)	},
 { PLUID_REPORT_CODE,	TYPUCH,
	FAROF(uchReportCode,	 RECPLU), LONGOF(uchReportCode,	    RECPLU)	},
 { PLUID_LINK_NUMBER,	TYPBIN,
	FAROF(usLinkNumber,	 RECPLU), LONGOF(usLinkNumber,			RECPLU)	},	/* ### MOD 2172 Rel1.0 */
 { PLUID_RESTRICT,		TYPUCH,
	FAROF(uchRestrict,		 RECPLU), LONGOF(uchRestrict,		RECPLU)	},
 { PLUID_LINK_PPI,		TYPUCH,
	FAROF(uchLinkPPI,		 RECPLU), LONGOF(uchLinkPPI,		RECPLU)	},

 { PLUID_TABLE_NUMBER,		TYPUCH,
	FAROF(uchTableNumber,	RECPLU), LONGOF(uchTableNumber,	RECPLU)	},	/* ### ADD 2172 rel1.0 */
 { PLUID_GROUP_NUMBER,		TYPUCH,
	FAROF(uchGroupNumber,	RECPLU), LONGOF(uchGroupNumber,	RECPLU)	},	/* ### ADD 2172 rel1.0 */
 { PLUID_PRINT_PRIORITY,	TYPUCH,
	FAROF(uchPrintPriority,	 RECPLU), LONGOF(uchPrintPriority,	RECPLU)	},	/* ### ADD 2172 rel1.0 */

/* ### DEL 2172 Rel1.0  { PLUID_FAMILY_CODE,	TYPUSH,
	FAROF(usFamilyCode,		 RECPLU), LONGOF(usFamilyCode,	    RECPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_ITEM_COUNTS,	TYPLNG,
	FAROF(lItemCounts,		 RECPLU), LONGOF(lItemCounts,		RECPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_SALES_TOTAL,	TYPLNG,
	FAROF(lSalesTotal,		 RECPLU), LONGOF(lSalesTotal,		RECPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_DISC_TOTAL,	TYPLNG,
	FAROF(lDiscTotal,		 RECPLU), LONGOF(lDiscTotal,		RECPLU)	},*/
/* ### DEL 2172 Rel1.0 { PLUID_BONUS_STATUS,	TYPUCH,
	FAROF(uchBonusStatus,	 RECPLU), LONGOF(uchBonusStatus,	RECPLU)	},*/
 { PLUID_BONUS_STATUS,	TYPUCH,
	FAROF(uchItemType,	 RECPLU), LONGOF(uchItemType,	RECPLU)	},
/* ### DEL 2172 Rel1.0  { PLUID_FQS_VALUE,	    TYPUSH,
	FAROF(usFqsValue,	     RECPLU), LONGOF(usFqsValue,	    RECPLU)	},*/
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};

RECDIC	FARCONST	PluRecordR10[] = { /* ### MOD 2172 rel1.0 */
 { PLUID_PLU_NUMBER_22,		TYPBIN,
	FAROF(aucPluNumber,      RECPLU22), LONGOF(aucPluNumber,	RECPLU22)	},
 { PLUID_ADJECTIVE_22,		TYPUCH,
	FAROF(uchAdjective,		 RECPLU22), LONGOF(uchAdjective,	RECPLU22)	},
 { PLUID_DEPT_NUMBER_22,	TYPUCH,
	FAROF(uchDeptNumber,	 RECPLU22), LONGOF(uchDeptNumber,    RECPLU22)	},
 { PLUID_UNIT_PRICE_22,		TYPULG,
	FAROF(ulUnitPrice,		 RECPLU22), LONGOF(ulUnitPrice,	    RECPLU22)	},
 { PLUID_PMUL_WEIGHT_22,	TYPUCH,
	FAROF(uchPriceMulWeight, RECPLU22), LONGOF(uchPriceMulWeight, RECPLU22)	},
 { PLUID_MNEMONIC_22,		TYPBIN,
	FAROF(aucMnemonic,		 RECPLU22), LONGOF(aucMnemonic,		RECPLU22)	},
 { PLUID_STATUS_22,			TYPBIN,
	FAROF(aucStatus,		 RECPLU22), LONGOF(aucStatus,		RECPLU22)	},
 { PLUID_MMATCH_TARE_22,	TYPUCH,
	FAROF(uchMixMatchTare,	 RECPLU22), LONGOF(uchMixMatchTare,	RECPLU22)	},
 { PLUID_REPORT_CODE_22,	TYPUCH,
	FAROF(uchReportCode,	 RECPLU22), LONGOF(uchReportCode,	RECPLU22)	},
 { PLUID_LINK_NUMBER_22,	TYPUCH,
	FAROF(aucLinkNumber,	 	RECPLU22), LONGOF(aucLinkNumber,	    RECPLU22)	},
 { PLUID_RESTRICT_22,		TYPUCH,
	FAROF(uchRestrict,		 RECPLU22), LONGOF(uchRestrict,		RECPLU22)	},
 { PLUID_LINK_PPI_22,		TYPUCH,
	FAROF(uchLinkPPI,		 RECPLU22), LONGOF(uchLinkPPI,		RECPLU22)	},
 { PLUID_FAMILY_CODE_22,	TYPUSH,
	FAROF(usFamilyCode,		 RECPLU22), LONGOF(usFamilyCode,	RECPLU22)	},
 { PLUID_ITEM_COUNTS_22,	TYPLNG,
	FAROF(lItemCounts,		 RECPLU22), LONGOF(lItemCounts,		RECPLU22)	},
 { PLUID_SALES_TOTAL_22,	TYPLNG,
	FAROF(lSalesTotal,		 RECPLU22), LONGOF(lSalesTotal,		RECPLU22)	},
 { PLUID_DISC_TOTAL_22,		TYPLNG,
	FAROF(lDiscTotal,		 RECPLU22), LONGOF(lDiscTotal,		RECPLU22)	},
 { PLUID_BONUS_STATUS_22,	TYPUCH,
	FAROF(uchBonusStatus,	 RECPLU22), LONGOF(uchBonusStatus,	RECPLU22)	},
 { PLUID_FQS_VALUE_22,	    TYPUSH,
	FAROF(usFqsValue,	     RECPLU22), LONGOF(usFqsValue,	    RECPLU22)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};

RECDIC	FARCONST	MixRecord[] = {
 { MIXID_MIX_NUMBER,	TYPUCH,
	FAROF(uchMixMatch,       RECMIX), LONGOF(uchMixMatch,       RECMIX)	},
 { MIXID_PRICE_MULTI,	TYPUCH,
	FAROF(uchPriceMulti,	 RECMIX), LONGOF(uchPriceMulti,	    RECMIX)	},
 { MIXID_UNIT_PRICE,	TYPLNG,
	FAROF(lUnitPrice,        RECMIX), LONGOF(lUnitPrice,	    RECMIX)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};

#ifdef _DEL_2172_R10	/* ### DEL 2172 Rel1.0 */
RECDIC	FARCONST	PpiRecord[] = {
 { PPIID_PPI_CODE,		TYPUCH,
	FAROF(uchPPI,            RECPPI), LONGOF(uchPPI,            RECPPI)	},
 { PPIID_NO_OF_LEVEL,	TYPUCH,
	FAROF(uchNoOfLevel,      RECPPI), LONGOF(uchNoOfLevel,      RECPPI)	},
 { PPIID_LEVEL1,		TYPUCH,
	FAROF(uchLevel1,         RECPPI), LONGOF(uchLevel1,         RECPPI)	},
 { PPIID_PRICE1,		TYPULG,
	FAROF(ulPrice1,          RECPPI), LONGOF(ulPrice1,          RECPPI)	},
 { PPIID_LEVEL2,		TYPUCH,
	FAROF(uchLevel2,         RECPPI), LONGOF(uchLevel2,         RECPPI)	},
 { PPIID_PRICE2,		TYPULG,
	FAROF(ulPrice2,          RECPPI), LONGOF(ulPrice2,          RECPPI)	},
 { PPIID_LEVEL3,		TYPUCH,			  
	FAROF(uchLevel3,         RECPPI), LONGOF(uchLevel3,         RECPPI)	},
 { PPIID_PRICE3,		TYPULG,
	FAROF(ulPrice3,          RECPPI), LONGOF(ulPrice3,          RECPPI)	},
 { PPIID_LEVEL4,		TYPUCH,
	FAROF(uchLevel4,         RECPPI), LONGOF(uchLevel4,         RECPPI)	},
 { PPIID_PRICE4,		TYPULG,
	FAROF(ulPrice4,          RECPPI), LONGOF(ulPrice4,          RECPPI)	},
 { PPIID_LEVEL5,		TYPUCH,
	FAROF(uchLevel5,         RECPPI), LONGOF(uchLevel5,         RECPPI)	},
 { PPIID_PRICE5,		TYPULG,
	FAROF(ulPrice5,          RECPPI), LONGOF(ulPrice5,          RECPPI)	},
 { PPIID_LEVEL6,		TYPUCH,
	FAROF(uchLevel6,         RECPPI), LONGOF(uchLevel6,         RECPPI)	},
 { PPIID_PRICE6,		TYPULG,
	FAROF(ulPrice6,          RECPPI), LONGOF(ulPrice6,          RECPPI)	},
 { PPIID_LEVEL7,		TYPUCH,
	FAROF(uchLevel7,         RECPPI), LONGOF(uchLevel7,         RECPPI)	},
 { PPIID_PRICE7,		TYPULG,
	FAROF(ulPrice7,          RECPPI), LONGOF(ulPrice7,          RECPPI)	},
 { PPIID_LEVEL8,		TYPUCH,
	FAROF(uchLevel8,         RECPPI), LONGOF(uchLevel8,         RECPPI)	},
 { PPIID_PRICE8,		TYPULG,
	FAROF(ulPrice8,          RECPPI), LONGOF(ulPrice8,          RECPPI)	},
 { PPIID_LEVEL9,		TYPUCH,
	FAROF(uchLevel9,         RECPPI), LONGOF(uchLevel9,         RECPPI)	},
 { PPIID_PRICE9,		TYPULG,
	FAROF(ulPrice9,          RECPPI), LONGOF(ulPrice9,          RECPPI)	},
 { PPIID_LEVEL10,		TYPUCH,
	FAROF(uchLevel10,        RECPPI), LONGOF(uchLevel10,        RECPPI)	},
 { PPIID_PRICE10,		TYPULG,
	FAROF(ulPrice10,         RECPPI), LONGOF(ulPrice10,         RECPPI)	},
/* --- V2.1 --- */
 { PPIID_LEVEL11,		TYPUCH,
	FAROF(uchLevel11,         RECPPI), LONGOF(uchLevel11,         RECPPI)	},
 { PPIID_PRICE11,		TYPULG,
	FAROF(ulPrice11,          RECPPI), LONGOF(ulPrice11,          RECPPI)	},
 { PPIID_LEVEL12,		TYPUCH,
	FAROF(uchLevel12,         RECPPI), LONGOF(uchLevel12,         RECPPI)	},
 { PPIID_PRICE12,		TYPULG,
	FAROF(ulPrice12,          RECPPI), LONGOF(ulPrice12,          RECPPI)	},
 { PPIID_LEVEL13,		TYPUCH,			  
	FAROF(uchLevel13,         RECPPI), LONGOF(uchLevel13,         RECPPI)	},
 { PPIID_PRICE13,		TYPULG,
	FAROF(ulPrice13,          RECPPI), LONGOF(ulPrice13,          RECPPI)	},
 { PPIID_LEVEL14,		TYPUCH,
	FAROF(uchLevel14,         RECPPI), LONGOF(uchLevel14,         RECPPI)	},
 { PPIID_PRICE14,		TYPULG,
	FAROF(ulPrice14,          RECPPI), LONGOF(ulPrice14,          RECPPI)	},
 { PPIID_LEVEL15,		TYPUCH,
	FAROF(uchLevel15,         RECPPI), LONGOF(uchLevel15,         RECPPI)	},
 { PPIID_PRICE15,		TYPULG,
	FAROF(ulPrice15,          RECPPI), LONGOF(ulPrice15,          RECPPI)	},
 { PPIID_LEVEL16,		TYPUCH,
	FAROF(uchLevel16,         RECPPI), LONGOF(uchLevel16,         RECPPI)	},
 { PPIID_PRICE16,		TYPULG,
	FAROF(ulPrice16,          RECPPI), LONGOF(ulPrice16,          RECPPI)	},
 { PPIID_LEVEL17,		TYPUCH,
	FAROF(uchLevel17,         RECPPI), LONGOF(uchLevel17,         RECPPI)	},
 { PPIID_PRICE17,		TYPULG,
	FAROF(ulPrice17,          RECPPI), LONGOF(ulPrice17,          RECPPI)	},
 { PPIID_LEVEL18,		TYPUCH,
	FAROF(uchLevel18,         RECPPI), LONGOF(uchLevel18,         RECPPI)	},
 { PPIID_PRICE18,		TYPULG,
	FAROF(ulPrice18,          RECPPI), LONGOF(ulPrice18,          RECPPI)	},
 { PPIID_LEVEL19,		TYPUCH,
	FAROF(uchLevel19,         RECPPI), LONGOF(uchLevel19,         RECPPI)	},
 { PPIID_PRICE19,		TYPULG,
	FAROF(ulPrice19,          RECPPI), LONGOF(ulPrice19,          RECPPI)	},
 { PPIID_LEVEL20,		TYPUCH,
	FAROF(uchLevel20,        RECPPI), LONGOF(uchLevel20,        RECPPI)	},
 { PPIID_PRICE20,		TYPULG,
	FAROF(ulPrice20,         RECPPI), LONGOF(ulPrice20,         RECPPI)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};
#endif	/* ### _DEL_2172_R10 */

RECDIC	FARCONST	PluMainte[] = {
 { PLUID_PLU_NUMBER,	TYPBIN,
	FAROF(aucPluNumber,      MNTPLU), LONGOF(aucPluNumber,      MNTPLU)	},
 { PLUID_ADJECTIVE,		TYPUCH,
	FAROF(uchAdjective,		 MNTPLU), LONGOF(uchAdjective,	    MNTPLU)	},
 { PLUID_DEPT_NUMBER,	TYPUCH,
	FAROF(usDeptNumber,	 MNTPLU), LONGOF(usDeptNumber,	    MNTPLU)	},	/* ### MOD 2172 Rel1.0 */
 { PLUID_UNIT_PRICE,	TYPULG,
	FAROF(ulUnitPrice,		 MNTPLU), LONGOF(ulUnitPrice,	    MNTPLU)	},
 { PLUID_PMUL_WEIGHT,	TYPUCH,
	FAROF(uchPriceMulWeight, MNTPLU), LONGOF(uchPriceMulWeight, MNTPLU)	},
 { PLUID_MNEMONIC,		TYPBIN,
	FAROF(aucMnemonic,		 MNTPLU), LONGOF(aucMnemonic,		MNTPLU)	},
 { PLUID_STATUS,		TYPBIN,
	FAROF(aucStatus,		 MNTPLU), LONGOF(aucStatus,		    MNTPLU)	},
 { PLUID_MMATCH_TARE,	TYPUCH,
	FAROF(uchMixMatchTare,	 MNTPLU), LONGOF(uchMixMatchTare,	MNTPLU)	},
 { PLUID_REPORT_CODE,	TYPUCH,
	FAROF(uchReportCode,	 MNTPLU), LONGOF(uchReportCode,	    MNTPLU)	},
 { PLUID_LINK_NUMBER,	TYPUSH,
	FAROF(usLinkNumber,	 MNTPLU), LONGOF(usLinkNumber,	    MNTPLU)	},	/* ### MOD 2172 Rel1.0 (UCHAR->USHORT:TYPUSH) */
 { PLUID_RESTRICT,		TYPUCH,
	FAROF(uchRestrict,		 MNTPLU), LONGOF(uchRestrict,		MNTPLU)	},
 { PLUID_LINK_PPI,		TYPUCH,
	FAROF(uchLinkPPI,		 MNTPLU), LONGOF(uchLinkPPI,		MNTPLU)	},
 { PLUID_TABLE_NUMBER,		TYPUCH,
	FAROF(uchTableNumber,	RECPLU), LONGOF(uchTableNumber,	RECPLU)	},	/* ### ADD 2172 rel1.0 */
 { PLUID_GROUP_NUMBER,		TYPUCH,
	FAROF(uchGroupNumber,	RECPLU), LONGOF(uchGroupNumber,	RECPLU)	},	/* ### ADD 2172 rel1.0 */
 { PLUID_PRINT_PRIORITY,	TYPUCH,
	FAROF(uchPrintPriority,	 RECPLU), LONGOF(uchPrintPriority,	RECPLU)	},	/* ### ADD 2172 rel1.0 */

/* ### DEL 2172 Rel1.0  { PLUID_FAMILY_CODE,	TYPUSH,
	FAROF(usFamilyCode,		 MNTPLU), LONGOF(usFamilyCode,	    MNTPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_ITEM_COUNTS,	TYPLNG,
	FAROF(lItemCounts,		 MNTPLU), LONGOF(lItemCounts,		MNTPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_SALES_TOTAL,	TYPLNG,
	FAROF(lSalesTotal,		 MNTPLU), LONGOF(lSalesTotal,		MNTPLU)	},*/
/* ### DEL 2172 Rel1.0  { PLUID_DISC_TOTAL,	TYPLNG,
	FAROF(lDiscTotal,		 MNTPLU), LONGOF(lDiscTotal,		MNTPLU)	},*/
 { PLUID_BONUS_STATUS,	TYPUCH,
	FAROF(uchBonusStatus,	 MNTPLU), LONGOF(uchBonusStatus,	MNTPLU)	},
/* ### DEL 2172 Rel1.0  { PLUID_FQS_VALUE,	    TYPUSH,
	FAROF(usFqsValue,	     MNTPLU), LONGOF(usFqsValue,	    MNTPLU)	},*/
 { MNTID_BATCH_NUMBER,	TYPUSH,
	FAROF(usMntBatch,        MNTPLU), LONGOF(usMntBatch,        MNTPLU)	},
 { MNTID_TYPE,			TYPUSH,
	FAROF(usMntType,         MNTPLU), LONGOF(usMntType,         MNTPLU)	},
 { MNTID_CHANGES,		TYPULG,
	FAROF(flMntChanges,      MNTPLU), LONGOF(flMntChanges,      MNTPLU)	},
 { MNTID_STATUS,		TYPUSH,
	FAROF(usMntStatus,       MNTPLU), LONGOF(usMntStatus,       MNTPLU)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};

#ifdef	_DEL_2172_R10	/* ### DEL 2172 Rel1.0 */
RECDIC	FARCONST	PluMainteR10[] = {
 { PLUID_PLU_NUMBER,	TYPBIN,
	FAROF(aucPluNumber,      MNTPLU), LONGOF(aucPluNumber,      MNTPLU)	},
 { PLUID_ADJECTIVE,		TYPUCH,
	FAROF(uchAdjective,		 MNTPLU), LONGOF(uchAdjective,	    MNTPLU)	},
 { PLUID_DEPT_NUMBER,	TYPBIN,
	FAROF(usDeptNumber,	 MNTPLU), LONGOF(uchDeptNumber,	    MNTPLU)	},
 { PLUID_UNIT_PRICE,	TYPULG,
	FAROF(ulUnitPrice,		 MNTPLU), LONGOF(ulUnitPrice,	    MNTPLU)	},
 { PLUID_PMUL_WEIGHT,	TYPUCH,
	FAROF(uchPriceMulWeight, MNTPLU), LONGOF(uchPriceMulWeight, MNTPLU)	},
 { PLUID_MNEMONIC,		TYPBIN,
	FAROF(aucMnemonic,		 MNTPLU), LONGOF(aucMnemonic,		MNTPLU)	},
 { PLUID_STATUS,		TYPBIN,
	FAROF(aucStatus,		 MNTPLU), LONGOF(aucStatus,		    MNTPLU)	},
 { PLUID_MMATCH_TARE,	TYPUCH,
	FAROF(uchMixMatchTare,	 MNTPLU), LONGOF(uchMixMatchTare,	MNTPLU)	},
 { PLUID_REPORT_CODE,	TYPUCH,
	FAROF(uchReportCode,	 MNTPLU), LONGOF(uchReportCode,	    MNTPLU)	},
 { PLUID_LINK_NUMBER,	TYPBIN,
	FAROF(uchLinkNumber,	 MNTPLU), LONGOF(uchLinkNumber,	    MNTPLU)	},
 { PLUID_RESTRICT,		TYPUCH,
	FAROF(uchRestrict,		 MNTPLU), LONGOF(uchRestrict,		MNTPLU)	},
 { PLUID_LINK_PPI,		TYPUCH,
	FAROF(uchLinkPPI,		 MNTPLU), LONGOF(uchLinkPPI,		MNTPLU)	},
 { PLUID_FAMILY_CODE,	TYPUSH,
	FAROF(usFamilyCode,		 MNTPLU), LONGOF(usFamilyCode,	    MNTPLU)	},
 { PLUID_ITEM_COUNTS,	TYPLNG,
	FAROF(lItemCounts,		 MNTPLU), LONGOF(lItemCounts,		MNTPLU)	},
 { PLUID_SALES_TOTAL,	TYPLNG,
	FAROF(lSalesTotal,		 MNTPLU), LONGOF(lSalesTotal,		MNTPLU)	},
 { PLUID_DISC_TOTAL,	TYPLNG,
	FAROF(lDiscTotal,		 MNTPLU), LONGOF(lDiscTotal,		MNTPLU)	},
 { MNTID_BATCH_NUMBER,	TYPUSH,
	FAROF(usMntBatch,        MNTPLU), LONGOF(usMntBatch,        MNTPLU)	},
 { MNTID_TYPE,			TYPUSH,
	FAROF(usMntType,         MNTPLU), LONGOF(usMntType,         MNTPLU)	},
 { MNTID_CHANGES,		TYPULG,
	FAROF(flMntChanges,      MNTPLU), LONGOF(flMntChanges,      MNTPLU)	},
 { MNTID_STATUS,		TYPUSH,
	FAROF(usMntStatus,       MNTPLU), LONGOF(usMntStatus,       MNTPLU)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};
#endif /* ### _DEL_2172_R10 */


RECDIC	FARCONST	IndexRecord[] = {
/* { IDXID_DEPT_NUMBER,	TYPUCH,
	FAROF(usDeptNumber,     RECIDX), LONGOF(usDeptNumber,     RECIDX)	},*//* ### DEL 2172 Rel1.0 (01/07/00) */
 { IDXID_PLU_NUMBER,	TYPBCD,
	FAROF(aucPluNumber,	    RECIDX), LONGOF(aucPluNumber,	    RECIDX)	},
 { IDXID_ADJECTIVE,     TYPUCH,
	FAROF(uchAdjective,      RECIDX), LONGOF(uchAdjective,      RECIDX)	},
 { (UCHAR)TBLEND,	(UCHAR)TBLEND,		TBLEND,			TBLEND			},
};


/*==========================================================================*\
||							End of PLUMANIP.C								||
\*==========================================================================*/
