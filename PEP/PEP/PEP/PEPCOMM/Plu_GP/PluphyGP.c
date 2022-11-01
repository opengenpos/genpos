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
*	Options:		/c /AM /Gs /Os /Za /Zp /W4 /G1
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*			 |			|										|
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

#include    <memory.h>							/* string C-library			*/
#include	<ecr.h>								/* 2170 system header		*/
#include    <r20_pif.h>								/* PifXXXX() header			*/
/*#include	<PLU.H>*/							/* PLU common header		*/
/*#include	<PLUDEFIN.H>*/						/* Mass Memory Module header*/
#include    "mypifdefs.h"

#include	<pepent.h>
#include	"PluPrcGP.H"						/* ### ADD Saratoga-PEP (02/08/2000) */
#include	"PluGP.H"							/* ### MOD Saratoga-PEP (02/08/2000) */
#include	"PluDefGP.h"						/* ### MOD Saratoga-PEP (02/08/2000) */

/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- adjustment for keeping handle from zero value --- */

#define		MAGIC_HANDLE		1				/* control handle value		*/

/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/

UCHAR FARCONST	aucReadOnly_GP[]  = "ro";				/* open w/ read mode	*/
UCHAR FARCONST	aucReadWrite_GP[] = "rwo";				/* open w/ read & write	*/
UCHAR FARCONST	aucCreateNew_GP[] = "rwn";				/* create newly			*/

/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

UCHAR	*SearchFreeByte_GP(UCHAR *, USHORT, USHORT);
USHORT	ComputeFreeBit_GP(UCHAR);
INDEX	ComputeIndexOffset_GP(USHORT);

/*==========================================================================*\
;+																			+
;+				P R O G R A M     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function :	Create a Mass Memory File
;
;   format : USHORT	PhyCreateFile(pszName, phf, usID, usType, usUnit, usRec);
;
;   input  : UCHAR FAR	*pszName;		- file spec. name
;			 USHORT		*phf;			- pointer to handle buffer
;			 USHORT		usID;			- file's unique ID
;			 USHORT		usType;			- file type
;			 USHORT		usUnit;			- bytes per record
;			 USHORT		usRec;			- no. of records
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyCreateFile_GP(WCHAR FAR *pszName,
					  USHORT	*phf,
					  USHORT    usID,
					  USHORT    usType,
					  USHORT    usUnit,
					  USHORT    usRec)
{
	UCHAR		aucBlock[SIZEDISKIO];
	USHORT		hf, usIndex;
	ULONG		ulBytes,usWrite, ulCells, ulNew;
	PFILEHDR	pHeader;

	/* --- create a file newly --- */

	if ((SHORT)(hf = PifOpenFile(pszName, aucCreateNew_GP)) < 0) {
		return (SPLU_SYSTEM_FAIL);
	}

	/* --- compute file size --- */

    if (usType == TYPE_INDEX) { /* R2.0 */
        usIndex = 0;
    } else {
    	usIndex = usRec / 8 + ((usRec % 8) ? 1 : 0);	/* size of index table 	*/
    }
	ulCells = (ULONG)usUnit * (ULONG)usRec;			/* size of rec. cells	*/
	ulBytes = (ULONG)sizeof(FILEHDR) + (ULONG)usIndex + ulCells;

	/* --- reserve a file w/ enough size --- */

	if (PifSeekFile(hf, ulBytes, &ulNew) < 0) {		/* reserve a memory		*/
		PifCloseFile(hf);								/* close it			*/
		PifDeleteFile(pszName);							/* delete it		*/
		return (SPLU_DEVICE_FULL);
	}

	/* --- return to top of a file --- */

	PifSeekFile(hf, 0L, &ulNew);					/* go back to top		*/

	/* --- make file header --- */

	memset(pHeader = (PFILEHDR)aucBlock, 0, sizeof(FILEHDR));
	pHeader->ulFileSize     = ulBytes;				/* file size in byte	*/
	pHeader->usFileNumber   = usID;					/* unique ID of file's	*/
	pHeader->usFileType     = usType;				/* file type			*/
	pHeader->usHashKey      = usRec;				/* key value for hashing*/
	pHeader->usMaxRec       = usRec;				/* no. of max. records	*/
	pHeader->usCurRec       = 0;					/* no. of cur. records	*/
	pHeader->usRecLen       = usUnit;				/* bytes per record		*/
	pHeader->usOffsetIndex  = sizeof(FILEHDR);		/* offset to index table*/
	pHeader->usBytesIndex   = usIndex;				/* bytes of index table	*/
	pHeader->usOffsetRecord	= sizeof(FILEHDR) + usIndex;	/* offset to rec*/
	pHeader->ulBytesRecord  = ulCells;				/* bytes of record area	*/

	/* --- write file header, however not knowing it's completed. --- */

	PifWriteFile(hf, pHeader, sizeof(FILEHDR));

	/* --- clear a buffer before initializing file --- */

	memset(aucBlock, 0, sizeof(aucBlock));			/* once, clear it		*/

	/* --- write initial contents of index table --- */

	while (usIndex) {								/* loop whole index size*/
		usWrite  = (sizeof(aucBlock) < usIndex) ? sizeof(aucBlock) : usIndex;
		usIndex -= (USHORT)usWrite;
		PifWriteFile(hf, aucBlock, usWrite);
	}

	/* --- respond a handle --- */

	*phf = (hf + MAGIC_HANDLE);

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function :	Delete a file from file system
;
;   format : USHORT		PhyDeleteFile(pszName);
;
;   input  : UCHAR FAR	*pszName;		- file spec. name
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyDeleteFile_GP(WCHAR FAR *pszName)
{
	/* --- just delete it --- */

	PifDeleteFile(pszName);							/* delete it			*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function :	Clear a Mass Memory File
;
;   format : USHORT		PhyClearFile(hf);
;
;   input  : USHORT		*phf;			- pointer to handle buffer
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyClearFile_GP(USHORT	hf)
{
	UCHAR		aucBlock[SIZEDISKIO];
	USHORT		usIndex, hfDirty;
	ULONG		usWrite;
	ULONG		ulNew;
	PFILEHDR	pHeader;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- return to top of a file --- */

	if (PifSeekFile(hfDirty, 0L, &ulNew) != PIF_OK) {/* go back to top		*/
		return (SPLU_FILE_BROKEN);
	}

	/* --- read a content of header --- */

	if (PifReadFile(hfDirty, aucBlock, sizeof(FILEHDR)) != sizeof(FILEHDR)) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- reset only no. of cur. records --- */

	pHeader           = (PFILEHDR)aucBlock;			/* top of file header	*/
	usIndex           = pHeader->usBytesIndex;		/* byte of index table	*/
	pHeader->usCurRec = 0;							/* reset no. of rec.	*/
	memset(&(pHeader->queListMainte), '\x00', sizeof(QUEUE));
	memset(&(pHeader->queReserved),   '\x00', sizeof(QUEUE));

	/* --- return to top of a file --- */

	if (PifSeekFile(hfDirty, 0L, &ulNew) != PIF_OK) {/* go back to top		*/
		return (SPLU_FILE_BROKEN);
	}

	/* --- write file header, however not knowing it's completed. --- */

	PifWriteFile(hfDirty, aucBlock, sizeof(FILEHDR));

	/* --- clear a buffer before initializing file --- */

	memset(aucBlock, 0, sizeof(aucBlock));			/* once, clear it		*/

	/* --- write initial contents of index table --- */

	while (usIndex) {								/* loop whole index size*/
		usWrite  = (sizeof(aucBlock) < usIndex) ? sizeof(aucBlock) : usIndex;
		usIndex -= (USHORT)usWrite;
		PifWriteFile(hfDirty, aucBlock, usWrite);
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Open a Mass Memory file
;
;   format : USHORT		PhyOpenFile(pszName, phf, fWrite);
;
;   input  : UCHAR FAR	*pszName;	- file spec. name
;			 USHORT		*phf;		- pointer to respond handle
;			 BOOL		fWrite;		- read only / read & write
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	PhyOpenFile_GP(WCHAR FAR *pszName, USHORT *phf, BOOL fWrite)
{
	USHORT		hf;
	UCHAR FAR	*pMode;

	/* --- compute accessing mode --- */

	pMode = (UCHAR FAR *)(fWrite ? aucReadWrite_GP : aucReadOnly_GP);

	/* --- open a file --- */

	if ((SHORT)(hf = PifOpenFile(pszName, pMode)) < 0) {
		return (SPLU_FILE_NOT_FOUND);
	}

	/* --- respond file handle --- */

	*phf = hf + MAGIC_HANDLE;

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function :	Audit a Mass Memory File
;
;   format : USHORT		PhyAuditFile(hf);
;
;   input  : USHORT		hf;				- file handle to audit
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyAuditFile_GP(USHORT hf)
{
	USHORT		hfDirty, usIndex;
	ULONG		ulRecord, ulTotal;
	ULONG		ulMoved;
	FILEHDR		hdrInf;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}

	/* --- read a record header --- */

	if (PifReadFile(hfDirty, &hdrInf, sizeof(FILEHDR)) != sizeof(FILEHDR)) {
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}

	/* --- is no. of records info. valid ? --- */

	if (hdrInf.usCurRec > hdrInf.usMaxRec) {	/* over than max. records ?	*/
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}
		
	/* --- compute each data block size in byte --- */

	usIndex  = hdrInf.usMaxRec / 8 + ((hdrInf.usMaxRec % 8) ? 1 : 0);
	ulRecord = (ULONG)(hdrInf.usRecLen) * (ULONG)(hdrInf.usMaxRec);
	ulTotal  = (ULONG)sizeof(FILEHDR) + (ULONG)usIndex + ulRecord;

	/* --- is valid file size (I can't confirm real file size.) ? --- */

	if ((hdrInf.ulFileSize != ulTotal)			/* invalid file size ?		*/
	   || (hdrInf.usOffsetIndex  != sizeof(FILEHDR))
	   || (hdrInf.usBytesIndex   != usIndex)
	   || (hdrInf.usOffsetRecord != (sizeof(FILEHDR) + usIndex))
	   || (hdrInf.ulBytesRecord  != ulRecord)) {
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Read directly from a Mass Memory file
;
;   format : USHORT		PhyReadFile(hf, pucBuffer, pusBytes, ulOffset);
;
;   input  : USHORT		hf;			- file handle
;			 UCHAR		*pucBuffer;	- output buffer ptr.
;			 USHORT		*pusBytes;	- buffer size / read length
;			 ULONG		ulOffset;	- file offset
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT
PhyReadFile_GP(USHORT hf, WCHAR *pucBuffer, ULONG *pusBytes, ULONG ulOffset)
{
	USHORT	hfDirty;
	ULONG	ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;				/* compute valid handle		*/

	/* --- move the file position --- */

	if (PifSeekFile(hfDirty, ulOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- read directly --- */

	*pusBytes = PifReadFile(hfDirty, pucBuffer, *pusBytes);

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Write directly to a Mass Memory file
;
;   format : USHORT		PhyWriteFile(hf, pucBuffer, pusBytes, ulOffset);
;
;   input  : USHORT		hf;			- file handle
;			 UCHAR		*pucBuffer;	- data buffer ptr.
;			 USHORT		*pusBytes;	- buffer size / written length
;			 ULONG		ulOffset;	- file offset
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT
PhyWriteFile_GP(USHORT hf, WCHAR *pucBuffer, ULONG *pusBytes, ULONG ulOffset)
{
	USHORT	hfDirty;
	ULONG	ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;				/* compute valid handle		*/

	/* --- move the file position --- */

	if (PifSeekFile(hfDirty, ulOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- write directly --- */

	PifWriteFile(hfDirty, pucBuffer, *pusBytes);

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Close a Mass Memory file
;
;   format : USHORT		PhyCloseFile(hf);
;
;   input  : USHORT		hf;			- file handle
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	PhyCloseFile_GP(USHORT hf)
{
	/* --- just close it --- */

	PifCloseFile(hf - MAGIC_HANDLE);				/* close it				*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Get an info. of Mass Memory file
;
;   format : USHORT		PhyGetInfo(hf, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 PFILEHDR	pHeader;	- pointer to respond header
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	PhyGetInfo_GP(USHORT hf, PFILEHDR pHeader)
{
	USHORT	hfDirty;
	ULONG	ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read a content of header --- */

	if (PifReadFile(hfDirty, pHeader, sizeof(FILEHDR)) != sizeof(FILEHDR)) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Update an info. of Mass Memory file
;
;   format : USHORT		PhyUpdateInfo(hf, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 PFILEHDR	pHeader;	- pointer to respond header
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	PhyUpdateInfo_GP(USHORT hf, PFILEHDR pHeader)
{
	USHORT		hfDirty;
	ULONG		ulMoved;
	FILEHDR		hdrInf;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read a record directly --- */

	if (PifReadFile(hfDirty, &hdrInf, sizeof(FILEHDR)) != sizeof(FILEHDR)) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- override only user area --- */

	hdrInf.usCurRec = pHeader->usCurRec;
	memcpy(&(hdrInf.queListMainte), &(pHeader->queListMainte), sizeof(QUEUE));
	memcpy(&(hdrInf.queReserved),   &(pHeader->queReserved),   sizeof(QUEUE));
	memcpy(hdrInf.aucAdjust, pHeader->aucAdjust, sizeof(pHeader->aucAdjust));

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- write a content of header --- */

	PifWriteFile(hfDirty, &hdrInf, sizeof(FILEHDR));

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT		PhyReadRecord(hf, usCell, pucBuffer, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 USHORT		usCell;		- cell no.
;			 UCHAR		*pucBuffer;	- pointer to respond a record
;			 PFILEHDR	pHeader;	- header information ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT
PhyReadRecord_GP(USHORT hf, USHORT usCell, WCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	hfDirty;
	ULONG	ulRec, usBytes, ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute a file position --- */

	usBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRec   = (ULONG)(pHeader->usOffsetRecord);		/* top of record data	*/
	ulRec  += (ULONG)usBytes * (ULONG)usCell;		/* add record offset	*/

	/* --- is in a file ? --- */

	if (usCell >= pHeader->usMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- move to the record position --- */

	if (PifSeekFile(hfDirty, ulRec, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- read a record directly --- */

	if (PifReadFile(hfDirty, pucBuffer, usBytes) != usBytes) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Write a record
;
;   format : USHORT		PhyWriteRecord(hf, usCell, pucBuffer, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 USHORT		usCell;		- cell no.
;			 UCHAR		*pucBuffer;	- pointer of record data
;			 PFILEHDR	pHeader;	- header information ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT
PhyWriteRecord_GP(USHORT hf, USHORT usCell, WCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	 hfDirty;
	ULONG	ulRec, usBytes, ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute a file position --- */

	usBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRec   = (ULONG)(pHeader->usOffsetRecord);		/* top of record data	*/
	ulRec  += (ULONG)usBytes * (ULONG)usCell;		/* add record offset	*/

	/* --- is in a file ? --- */

	if (usCell >= pHeader->usMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- move to the record position --- */

	if (PifSeekFile(hfDirty, ulRec, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- write a record directly --- */

	PifWriteFile(hfDirty, pucBuffer, usBytes);		/* write a record		*/

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Is a record occupied ?
;
;   format : USHORT		PhyExamOccupied(hf, usCell, pHeader, pfOccupied);
;
;   input  : USHORT		hf;				- file handle
;			 USHORT		usCell;			- cell no.
;			 PFILEHDR	pHeader;		- header information ptr.
;			 BOOL		*pfOccupied;	- occupied or not
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT
PhyExamOccupied_GP(USHORT hf, USHORT usCell, PFILEHDR pHeader, BOOL *pfOccupied)
{
	UCHAR	uchByte;
	USHORT	hfDirty;
	ULONG	ulPos, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset_GP(usCell);
	ulPos   = (ULONG)(pHeader->usOffsetIndex + BYTEFROMINDEX(indInfo));

	/* --- move a file pointer to just position --- */

	if (PifSeekFile(hfDirty, ulPos, &ulNew) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read the byte data --- */

	if (PifReadFile(hfDirty, &uchByte, sizeof(uchByte)) != sizeof(uchByte)) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- is occupied or not ? --- */

	*pfOccupied = (BOOL)((uchByte & PTRNFROMINDEX(indInfo)) ? TRUE : FALSE);

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Make the position occupied or vacant
;
;   format : USHORT		PhySetUpOccupied(hf, usCell, pHeader, fOccupied);
;
;   input  : USHORT		hf;				- file handle
;			 USHORT		usCell;			- cell no.
;			 PFILEHDR	pHeader;		- header information ptr.
;			 BOOL		fOccupied;		- occupied or not
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT
PhySetUpOccupied_GP(USHORT hf, USHORT usCell, PFILEHDR pHeader, BOOL fOccupied)
{
	UCHAR	uchByte;
	USHORT	hfDirty;
	ULONG	ulPos, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset_GP(usCell);		/* compute offset in index	*/
	ulPos   = (ULONG)(pHeader->usOffsetIndex + BYTEFROMINDEX(indInfo));

	/* --- move a file pointer to just position --- */

	if (PifSeekFile(hfDirty, ulPos, &ulNew) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read the byte data --- */

	if (PifReadFile(hfDirty, &uchByte, sizeof(uchByte)) != sizeof(uchByte)) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- is occupied or not ? --- */

	if (fOccupied) {							/* make it occupied ?		*/
		uchByte |= PTRNFROMINDEX(indInfo);			/* active a flag		*/
	} else {									/* else, make it vacant		*/
		uchByte &= ~(PTRNFROMINDEX(indInfo));		/* reset a flag			*/
	}

	/* --- move a file pointer to just position --- */

	if (PifSeekFile(hfDirty, ulPos, &ulNew) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- write the byte data --- */

	PifWriteFile(hfDirty, &uchByte, sizeof(uchByte));

	/* --- exit ... --- */

	return (0);
}

/**
;========================================================================
;
;   function : Look for vacant cell around the position
;
;   format : USHORT		PhySearchVacant(hf, usCell, pHeader, pusFree);
;
;   input  : USHORT		hf;				- file handle
;			 USHORT		usCell;			- cell no.
;			 PFILEHDR	pHeader;		- header information ptr.
;			 USHORT		*pusFree;		- pointer to vacant cell no.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT
PhySearchVacant_GP(USHORT hf, USHORT usCell, PFILEHDR pHeader, USHORT *pusFree)
{
	UCHAR	aucBlock[SIZEDISKIO], ucTail, *pucFound;
	USHORT	usRest, usStat, usAlign, usOrigin, hfDirty;
	ULONG	ulPos, usBytes, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset_GP(usCell);		/* compute offset in index	*/
	ulPos   = (ULONG)(pHeader->usOffsetIndex);	/* top of index table		*/
	usRest  = pHeader->usBytesIndex;			/* remained bytes of table	*/
	usStat  = SPLU_FILE_OVERFLOW;				/* assume not found status	*/

	/* --- compute pattern of bottom index table --- */

	ucTail  = (UCHAR)((UCHAR)0xFF >> ((pHeader->usMaxRec - 1) % 8 + 1));

	/* --- compute a block alignment --- */

	usAlign  = BYTEFROMINDEX(indInfo) / sizeof(aucBlock) * sizeof(aucBlock);
	usOrigin = BYTEFROMINDEX(indInfo) % sizeof(aucBlock);

	/* --- look for a vacant cell position --- */

	while (usRest) {

		/* --- get no. of bytes to be read --- */

		usBytes = pHeader->usBytesIndex - usAlign;	/* bytes to be read		*/
		usBytes = (usBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : usBytes;

		/* --- cheating for easy algorithm --- */

		if (usBytes != sizeof(aucBlock)) {		/* if less than buffer size	*/
			memset(aucBlock, '\xFF', sizeof(aucBlock));
		}

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + (ULONG)usAlign, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */

		if (PifReadFile(hfDirty, aucBlock, usBytes) != usBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((usAlign + usBytes) >= pHeader->usBytesIndex) {
			*(aucBlock + usBytes - 1) |= ucTail;
		}

		/* --- search inactive byte in the block --- */

		if (pucFound = SearchFreeByte_GP(aucBlock, usOrigin, sizeof(aucBlock))) {
			*pusFree  = (usAlign + (USHORT)(pucFound - aucBlock)) * 8;
			*pusFree += (7 - ComputeFreeBit_GP(*pucFound));
			usStat    = 0;
			break;
		}

		/* --- compute next reading information --- */

		usAlign += sizeof(aucBlock);			/* compute next alignment	*/
		usAlign  = (usAlign > pHeader->usBytesIndex) ? 0 : usAlign;
		usRest  -= (USHORT)usBytes;						/* remained bytes of table	*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Look for occupied cell
;
;   format : USHORT		PhySearchOccupied(hf, usCell, pHeader, pusFound);
;
;   input  : USHORT		hf;				- file handle
;			 USHORT		usCell;			- cell no. to start search
;			 PFILEHDR	pHeader;		- header information ptr.
;			 USHORT		*pusFound;		- pointer to occupied cell no.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT
PhySearchOccupied_GP(USHORT hf, USHORT usCell, PFILEHDR pHeader, USHORT *pusFound)
{
	UCHAR	aucBlock[SIZEDISKIO], *pucFound, ucTail, ucBit, ucMask;
	USHORT	usRest, usStat, usOffset, hfDirty, i;
	ULONG	ulPos, usBytes, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset_GP(usCell);		/* compute offset in index	*/
	ulPos   = (ULONG)(pHeader->usOffsetIndex);	/* top of index table		*/
	usStat  = SPLU_END_OF_FILE;					/* assume not found status	*/

	/* --- compute pattern of bottom index table --- */

	ucTail  = (UCHAR)((UCHAR)0xFF >> ((pHeader->usMaxRec - 1) % 8 + 1));

	/* --- compute a block offset --- */

	usOffset = BYTEFROMINDEX(indInfo);
	ucBit    = PTRNFROMINDEX(indInfo);
	usRest   = pHeader->usBytesIndex - usOffset;/* remained bytes of table	*/

	/* --- look for a vacant cell position --- */

	while (usRest) {

		/* --- get no. of bytes to be read --- */

		usBytes = pHeader->usBytesIndex - usOffset;	/* bytes to be read		*/
		usBytes = (usBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : usBytes;

		/* --- cheating for easy algorithm --- */

		if (usBytes != sizeof(aucBlock)) {		/* if less than buffer size	*/
			memset(aucBlock, '\x00', sizeof(aucBlock));
		}

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + (ULONG)usOffset, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */

		if (PifReadFile(hfDirty, aucBlock, usBytes) != usBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((usOffset + usBytes) >= pHeader->usBytesIndex) {
			*(aucBlock + usBytes - 1) &= ~ucTail;
		}

		/* --- set inactive at the top of position --- */

		ucMask       = 0xFF;
		ucMask     >>= (7 - ComputeFreeBit_GP((UCHAR)(~ucBit)));
		aucBlock[0] &= ucMask;

		/* --- search active byte in the block --- */

		for (pucFound = aucBlock, i = 0; i < usBytes; pucFound++, i++) {

			/* --- is busy at this byte ? --- */

			if (*pucFound) {

				/* --- compute its cell number --- */

				*pusFound  = (usOffset + (USHORT)(pucFound - aucBlock)) * 8;
				*pusFound += (7 - ComputeFreeBit_GP((UCHAR)(~(*pucFound))));
				usStat     = 0;
				break;
			}
		}

		/* --- could be found ? --- */

		if (! usStat) {
			break;
		}

		/* --- compute next reading information --- */

		usOffset += sizeof(aucBlock);			/* compute next alignment	*/
		usRest   -= (USHORT)usBytes;					/* remained bytes of table	*/
		ucBit     = MSB;						/* search from MSB			*/
	}

	/* --- exit ... --- */

	return (usStat);
}

/*==========================================================================*\
;+																			+
;+				M I S C .    S U B - R O U T I N E s						+
;+																			+
\*==========================================================================*/
#pragma	optimize("egl", off)
/**
;========================================================================
;
;   function : Search a free byte in a block
;
;   format : UCHAR		*SearchFreeByte_GP(pucBuffer, usOrigin, usSize);
;
;   input  : UCHAR		*pucBuffer;		- buffer ptr.
;			 USHORT		usOrigin;		- origin offset to search
;			 USHORT		usSize;			- buffer size in byte
;
;   output : UCHAR		*pucFound;		- buffer ptr. found free byte
;
;	notes  : If pucFound equals to NULL, then could not find.
;
;========================================================================
**/

UCHAR	*SearchFreeByte_GP(UCHAR *pucBuffer, USHORT usOrigin, USHORT usSize)
{
	USHORT	fFound;
	UCHAR	*pucFound;
	USHORT	usOffset;

	/* --- initialize --- */

	fFound = FALSE;								/* assume cannot be found	*/

	/* --- look for a byte which has at lease a free bit --- */
#ifdef junkyjunk
// RJC following code generates compiler error with Visual C++ 6.0
// replacing with code from NHPOS that does the same thing
// as part of 16 to 32 bit conversion.

	_asm {

		push	bx					; save working registers
		push	cx					;
		push	dx					;
		push	ds					;
		push	di					;
		push	si					;

		;--- get parameters & set up ---
#if (defined(M_I86SM) || defined(M_I86MM))
		mov		bx, pucBuffer		; (ds:bx) = pucBuffer, top ptr. of buffer
#else
		lds		bx, pucBuffer		; (ds:bx) = pucBuffer, top ptr. of buffer
#endif
		mov		di, usOrigin		; (di) = backward offset
		mov		si, di				;
		inc		si					; (si) = foreward offset
		mov		cx, usSize			; (cx) = no. of bytes of the buffer

		;--- are any bytes remained ? ---
SearchF01:
		jcxz	SearchF04			; break if no byte

		;--- is available on back side ? ---

		and		di, di				; is overflowed ?
		js		SearchF02			; skip if not available on back side

			;--- exist at least free bit ? ---

		dec		cx					; dec. counter by 1
		mov		al, [bx + di]		; (al) = a byte data
		mov		dx, di				; (dx) = current offset
		dec		di					; (di) = prepare next offset
		not		al					; take 1's complement
		and		al, al				; all occupied ?
		jnz		SearchF03			; break if found

		;--- is available on fore side ? ---
SearchF02:
		cmp		usSize, si			; is overflowed ?
		jbe		SearchF01			; go back if overflowed

			;--- exist at least free bit ? ---

		dec		cx					; dec. counter by 1
		mov		al, [bx + si]		; (al) = a byte data
		mov		dx, si				; (dx) = current offset
		inc		si					; (si) = prepare next offset
		not		al					; take 1's complement
		and		al, al				; all occupied ?
		jz		SearchF01			; go back if not found

		;--- case of the found ... ---
SearchF03:
		mov		fFound, ax			; set flag found, (ax) must be not zero
		mov		usOffset, dx		; save a offset

		;--- exit ... ---
SearchF04:
		pop		si					; recover registers
		pop		di					;
		pop		ds					;
		pop		dx					;
		pop		cx					;
		pop		bx					;
	}
#else
	_asm {

		push	ebx					; save working registers ### MOD 2172 Rel1.0
		push	ecx					;
		push	edx					;
		;;;push	ds					;
		push	edi					;
		push	esi					;

		xor	eax, eax			; clear regs ### ADD 2172 Rel1.0
		xor	ecx, ecx			;
		xor	edx, edx			;
		xor	esi, esi			;
		xor	edi, edi			;

		;--- get parameters & set up ---
		;lds		bx, pucBuffer		; (ds:bx) = pucBuffer, top ptr. of buffer
		mov		ebx, pucBuffer		; ebx = pucBuffer ### MOD 2172 Rel1.0
		mov		di, usOrigin		; (di) = backward offset
		mov		si, di				;
		inc		si					; (si) = foreward offset
		mov		cx, usSize			; (cx) = no. of bytes of the buffer

		;--- are any bytes remained ? ---
SearchF01:
		jcxz	SearchF04			; break if no byte

		;--- is available on back side ? ---

		and		di, di				; is overflowed ?
		js		SearchF02			; skip if not available on back side

			;--- exist at least free bit ? ---

		dec		cx					; dec. counter by 1
		;;;mov		al, [bx + di]		; (al) = a byte data
		mov		al, [ebx + edi]		; ### MOD 2172 Rel1.0
		mov		dx, di				; (dx) = current offset
		dec		di					; (di) = prepare next offset
		not		al					; take 1's complement
		and		al, al				; all occupied ?
		jnz		SearchF03			; break if found

		;--- is available on fore side ? ---
SearchF02:
		cmp		usSize, si			; is overflowed ?
		jbe		SearchF01			; go back if overflowed

			;--- exist at least free bit ? ---

		dec		cx					; dec. counter by 1
		;;;mov		al, [bx + si]		; (al) = a byte data
		mov		al, [ebx + esi]		; ### MOD 2172 Rel1.0 BUG(03/14/2000)
		mov		dx, si				; (dx) = current offset
		inc		si					; (si) = prepare next offset
		not		al					; take 1's complement
		and		al, al				; all occupied ?
		jz		SearchF01			; go back if not found

		;--- case of the found ... ---
SearchF03:
		mov		fFound, ax			; set flag found, (ax) must be not zero ### MOD 2172 Rel1.0
		mov		usOffset, dx		; save a offset

		;--- exit ... ---
SearchF04:
		pop		esi					; recover registers
		pop		edi					;
		;;;pop		ds					; ### DEL 2172 Rel1.0
		pop		edx					;
		pop		ecx					;
		pop		ebx					;
	}

#endif

	/* --- cound be found ? --- */

	pucFound = (UCHAR *)(fFound ? (pucBuffer + usOffset) : NULL);

	/* --- exit ... --- */

	return (pucFound);
}

/**
;========================================================================
;
;   function : Compute free bit's order
;
;   format : USHORT		ComputeFreeBit_GP(uchByte);
;
;   input  : UCHAR		uchByte;		- a byte data
;
;   output : USHORT		usOrder;		- free bit's order no.
;
;========================================================================
**/

USHORT	ComputeFreeBit_GP(UCHAR uchByte)
{
	USHORT	usOrder;

	/* --- compute free bit's order --- */

	_asm {

		;--- get a parameter ---

		mov		ah, uchByte			; (ah) = a byte data
		mov		al, 7				; (al) = bit number from MSB

		;--- is the current MSB free ? ---
ComputeF01:
		shl		ah, 1				; test the MSB active ?
		jnc		ComputeF02			; break if not active

			;--- adjust bit number ---

		dec		al					; adjust to next bit number
		jmp		ComputeF01			; loop to find

		;--- found a free bit ... ---
ComputeF02:
		xor		ah, ah				; (ax) = bit number
		mov		usOrder, ax			; save its order
	}

	/* --- exit ... --- */

	return (usOrder);
}

/**
;========================================================================
;
;   function :	Compute Offset of Index Table
;
;   format : INDEX		ComputeIndexOffset_GP(usCell);
;
;   input  : USHORT		usCell;		- cell no. (from 0 to (max. rec. - 1))
;
;   output : INDEX		indOffset;	- record index offset
;
;	notes  : Use macros below:
;
;				- BYTEFROMINDEX(indOffset) to compute byte offset
;				- BITNFROMINDEX(indOffset) to compute bit number
;				- PTRNFROMINDEX(indOffset) to compute bit pattern
;
;========================================================================
**/

INDEX	ComputeIndexOffset_GP(USHORT usCell)
{
	INDEX	indOffset;

	/* --- compute offset information --- */

	_asm {
		push	cx							; save registers
		mov		ax, usCell					; (ax) = record cell number
		mov		cl, al						;
		and		cl, 07h						; (cl) = (ax) % 8
		shr		ax, 3						; (ax) = (ax) / 8
		mov		word ptr indOffset[2], ax	;
		mov		al, MSB						; (al) = 10000000B
		shr		al, cl						; (al) = bit pattern
		mov		ah, 07h						;
		sub		ah, cl						; (ah) = bit number
		mov		word ptr indOffset[0], ax	;
		pop		cx							; recover registers
	}

	/* --- exit ... --- */

	return (indOffset);
}
#pragma	optimize("egl", on)
/*==========================================================================*\
||							End of PLUPHYSC.C								||
\*==========================================================================*/

