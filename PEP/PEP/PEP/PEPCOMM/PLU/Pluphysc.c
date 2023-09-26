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
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*			 |			|										|

*
*** GenPOS **
* 09/25/23 : 02.04.00  : R.Chambers  : replace magic constant 0 with defined constant SPLU_COMPLETED
* 09/25/23 : 02.04.00  : R.Chambers  : eliminate FARCONST and FAR.
* 09/25/23 : 02.04.00  : R.Chambers  : make local functions (SearchFreeByte, etc.) static.
* 09/25/23 : 02.04.00  : R.Chambers  : allow for ARM builds with no _asm using define SET_NO_ASM

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

#include <stdio.h>
#include    <memory.h>							/* string C-library			*/
#include	<ecr.h>								/* 2170 system header		*/
#include    <r20_pif.h>								/* PifXXXX() header			*/
#include    "mypifdefs.h"
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/



/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- adjustment for keeping handle from zero value --- */

#define		MAGIC_HANDLE		1				/* control handle value		*/

// #define     SET_NO_ASM          1       /* compile using the no _asm version of functions. */

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

static UCHAR aucReadOnly[]  = "ro";				/* open w/ read mode	*/
static UCHAR aucReadWrite[] = "rwo";				/* open w/ read & write	*/
static UCHAR aucCreateNew[] = "rwn";				/* create newly			*/



/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

static UCHAR	*SearchFreeByte(UCHAR *, USHORT, USHORT);
static USHORT	ComputeFreeBit(UCHAR);
static INDEX	ComputeIndexOffset(ULONG);	/* ### 2172 Rel1.0(100,000item) */



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
;			 ULONG		ulRec;			- no. of records　### MOD 2172 Rel1.0
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyCreateFile(WCHAR *pszName, USHORT *phf, USHORT usID, USHORT usType,
					 USHORT usUnit, ULONG ulRec)
{
	UCHAR		aucBlock[SIZEDISKIO];
    USHORT		hf;
	SHORT       nRslt = 0;
    ULONG		ulBytes, ulCells, ulNew, ulIndex, ulWrite;
	PFILEHDR	pHeader;

	/* --- create a file newly --- */

	if ((SHORT)(hf = PifOpenFile(pszName, aucCreateNew)) < 0) {
		return (SPLU_SYSTEM_FAIL);
	}

	/* --- compute file size --- */

	nRslt = 0;

    if (usType == TYPE_INDEX) { /* R2.0 */
        ulIndex = 0;
    } else {
    	ulIndex = ulRec / 8L + ((ulRec % 8L) ? 1L : 0L);	/* size of index table 	*/
    }
	ulCells = (ULONG)usUnit * ulRec;			/* size of rec. cells	*/
	ulBytes = (ULONG)sizeof(FILEHDR) + ulIndex + ulCells;

	/* --- reserve a file w/ enough size --- */

	nRslt = PifSeekFile(hf, ulBytes, &ulNew);	/* reserve a memory		*/

	if (nRslt < 0) {		/* reserve a memory		*/
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
	pHeader->ulHashKey      = ulRec;				/* key value for hashing*/
	pHeader->ulMaxRec       = ulRec;				/* no. of max. records	*/
	pHeader->ulCurRec       = 0;					/* no. of cur. records	*/
	pHeader->usRecLen       = usUnit;				/* bytes per record		*/
	pHeader->ulOffsetIndex  = sizeof(FILEHDR);		/* offset to index table*/
	pHeader->ulBytesIndex   = ulIndex;				/* bytes of index table	*/
	pHeader->ulOffsetRecord	= sizeof(FILEHDR) + ulIndex;	/* offset to rec*/
	pHeader->ulBytesRecord  = ulCells;				/* bytes of record area	*/

	/* --- write file header, however not knowing it's completed. --- */

	PifWriteFile(hf, pHeader, sizeof(FILEHDR));

	/* --- clear a buffer before initializing file --- */

	memset(aucBlock, 0, sizeof(aucBlock));			/* once, clear it		*/

	/* --- write initial contents of index table --- */

	while (ulIndex) {								/* loop whole index size */
/*        usWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;	*/
/*		ulIndex -= usWrite; */
/*		PifWriteFile(hf, aucBlock, usWrite);    */
        ulWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;   /* Saratoga */
		ulIndex -= ulWrite;
		PifWriteFile(hf, aucBlock, ulWrite);
	}

	/* --- respond a handle --- */

	*phf = (hf + MAGIC_HANDLE);

	return (SPLU_COMPLETED);
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

USHORT	PhyDeleteFile(WCHAR *pszName)
{
	/* --- just delete it --- */

	PifDeleteFile(pszName);							/* delete it			*/

	return (SPLU_COMPLETED);
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

USHORT	PhyClearFile(USHORT	hf)
{
	UCHAR		aucBlock[SIZEDISKIO];
	USHORT		hfDirty;
	ULONG		ulNew,ulIndex, ulWrite;
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
	ulIndex           = pHeader->ulBytesIndex;		/* byte of index table	*/
	pHeader->ulCurRec = 0;							/* reset no. of rec.	*/
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

	while (ulIndex > 0) {/* ### */					/* loop whole index size*/
/*      usWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;	*/
/*      ulIndex -= usWrite; */
/*      PifWriteFile(hfDirty, aucBlock, usWrite);   */
        ulWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;   /* Saratoga */
		ulIndex -= ulWrite;
		PifWriteFile(hfDirty, aucBlock, ulWrite);
    }

	return (SPLU_COMPLETED);
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

USHORT	PhyOpenFile(WCHAR *pszName, USHORT *phf, BOOL fWrite)
{
	USHORT	hf;
	UCHAR 	*pMode;

	/* --- compute accessing mode --- */

	pMode = (UCHAR *)(fWrite ? aucReadWrite : aucReadOnly);

	/* --- open a file --- */

	if ((SHORT)(hf = PifOpenFile(pszName, pMode)) < 0) {
		return (SPLU_FILE_NOT_FOUND);
	}

	/* --- respond file handle --- */

	*phf = hf + MAGIC_HANDLE;

	return (SPLU_COMPLETED);
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

USHORT	PhyAuditFile(USHORT hf)
{
	USHORT		hfDirty;
	ULONG		ulRecord, ulTotal,ulIndex;
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

	if (hdrInf.ulCurRec > hdrInf.ulMaxRec) {	/* over than max. records ?	*/
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}
		
	/* --- compute each data block size in byte --- */

	ulIndex  = hdrInf.ulMaxRec / 8L + ((hdrInf.ulMaxRec % 8L) ? 1L : 0L);
	ulRecord = (ULONG)(hdrInf.usRecLen) * hdrInf.ulMaxRec;
	ulTotal  = (ULONG)sizeof(FILEHDR) + ulIndex + ulRecord;

	/* --- is valid file size (I can't confirm real file size.) ? --- */

	if ((hdrInf.ulFileSize != ulTotal)			/* invalid file size ?		*/
	   || (hdrInf.ulOffsetIndex  != sizeof(FILEHDR))
	   || (hdrInf.ulBytesIndex   != ulIndex)
	   || (hdrInf.ulOffsetRecord != (sizeof(FILEHDR) + ulIndex))
	   || (hdrInf.ulBytesRecord  != ulRecord)) {
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}

	return (SPLU_COMPLETED);
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


USHORT	PhyReadFile(USHORT hf, WCHAR *pucBuffer, ULONG *pusBytes, ULONG ulOffset)
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

	return (SPLU_COMPLETED);
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


USHORT	PhyWriteFile(USHORT hf, WCHAR *pucBuffer, ULONG *pusBytes, ULONG ulOffset)
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

	return (SPLU_COMPLETED);
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

USHORT	PhyCloseFile(USHORT hf)
{
	/* --- just close it --- */

	PifCloseFile((USHORT)(hf - MAGIC_HANDLE));	/* close it *//* ### MOD 2172 Rel1.0 */

	return (SPLU_COMPLETED);
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

USHORT	PhyGetInfo(USHORT hf, PFILEHDR pHeader)
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

	return (SPLU_COMPLETED);
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

USHORT	PhyUpdateInfo(USHORT hf, PFILEHDR pHeader)
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

	hdrInf.ulCurRec = pHeader->ulCurRec;
	memcpy(&(hdrInf.queListMainte), &(pHeader->queListMainte), sizeof(QUEUE));
	memcpy(&(hdrInf.queReserved),   &(pHeader->queReserved),   sizeof(QUEUE));
	memcpy(hdrInf.aucAdjust, pHeader->aucAdjust, sizeof(pHeader->aucAdjust));

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- write a content of header --- */

	PifWriteFile(hfDirty, &hdrInf, sizeof(FILEHDR));

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT		PhyReadRecord(hf, usCell, pucBuffer, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 ULONG 		ulCell;		- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 UCHAR		*pucBuffer;	- pointer to respond a record
;			 PFILEHDR	pHeader;	- header information ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/


USHORT	PhyReadRecord(USHORT hf, ULONG ulCell, WCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	hfDirty;
	ULONG	ulRecOffset/* ### */,usBytes,  ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute a file position --- */

	usBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRecOffset   = (ULONG)(pHeader->ulOffsetRecord);		/* top of record data	*/
	ulRecOffset  += (ULONG)usBytes * ulCell;				/* add record offset	*/

	/* --- is in a file ? --- */

	if (ulCell >= pHeader->ulMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- move to the record position --- */

	if (PifSeekFile(hfDirty, ulRecOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- read a record directly --- */

	if (PifReadFile(hfDirty, pucBuffer, usBytes) != usBytes) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Write a record
;
;   format : USHORT		PhyWriteRecord(hf, ulCell, pucBuffer, pHeader);
;
;   input  : USHORT		hf;			- file handle
;			 ULONG 		ulCell;		- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 UCHAR		*pucBuffer;	- pointer of record data
;			 PFILEHDR	pHeader;	- header information ptr.
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/


USHORT	PhyWriteRecord(USHORT hf, ULONG ulCell, WCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	usBytes, hfDirty;
	ULONG	ulRec, ulMoved;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute a file position --- */

	usBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRec   = pHeader->ulOffsetRecord;				/* top of record data	*/
	ulRec  += (ULONG)usBytes * ulCell;				/* add record offset	*/

	/* --- is in a file ? --- */

	if (ulCell >= pHeader->ulMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- move to the record position --- */

	if (PifSeekFile(hfDirty, ulRec, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- write a record directly --- */

	PifWriteFile(hfDirty, pucBuffer, usBytes);		/* write a record		*/

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Is a record occupied ?
;
;   format : USHORT		PhyExamOccupied(hf, ulCell, pHeader, pfOccupied);
;
;   input  : USHORT		hf;				- file handle
;			 ULONG 		ulCell;			- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 PFILEHDR	pHeader;		- header information ptr.
;			 BOOL		*pfOccupied;	- occupied or not
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	PhyExamOccupied(USHORT hf, ULONG ulCell, PFILEHDR pHeader, BOOL *pfOccupied)
{
	UCHAR	uchByte;
	USHORT	hfDirty;
	ULONG	ulPos, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);
	ulPos   = pHeader->ulOffsetIndex + BYTEFROMINDEX(indInfo);

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

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Make the position occupied or vacant
;
;   format : USHORT		PhySetUpOccupied(hf, usCell, pHeader, fOccupied);
;
;   input  : USHORT		hf;				- file handle
;			 ULONG 		ulCell;			- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 PFILEHDR	pHeader;		- header information ptr.
;			 BOOL		fOccupied;		- occupied or not
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	PhySetUpOccupied(USHORT hf, ULONG ulCell, PFILEHDR pHeader, BOOL fOccupied)
{
	UCHAR	uchByte;
	USHORT	hfDirty;
	ULONG	ulPos, ulNew;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);		/* compute offset in index	*/
	ulPos   = pHeader->ulOffsetIndex + BYTEFROMINDEX(indInfo);

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

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function : Look for vacant cell around the position
;
;   format : USHORT		PhySearchVacant(hf, ulCell, pHeader, pulFree);
;
;   input  : USHORT		hf;				- file handle
;			 ULONG 		ulCell;			- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 PFILEHDR	pHeader;		- header information ptr.
;			 LONG		*pulFree;		- pointer to vacant cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	PhySearchVacant(USHORT hf, ULONG ulCell, PFILEHDR pHeader, ULONG *pulFree)
{
	UCHAR	aucBlock[SIZEDISKIO], ucTail, *pucFound;
	USHORT	usStat, usOrigin, hfDirty;
	ULONG	ulPos, ulNew, ulAlign,usBytes,  ulRest;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);		/* compute offset in index	*/
	ulPos   = pHeader->ulOffsetIndex;			/* top of index table		*/
	ulRest  = pHeader->ulBytesIndex;			/* remained bytes of table	*/
	usStat  = SPLU_FILE_OVERFLOW;				/* assume not found status	*/

	/* --- compute pattern of bottom index table --- */

	ucTail  = (UCHAR)((UCHAR)0xFF >> ((pHeader->ulMaxRec - 1) % 8 + 1));

	/* --- compute a block alignment --- */

	ulAlign  = BYTEFROMINDEX(indInfo) / (ULONG)sizeof(aucBlock) * (ULONG)sizeof(aucBlock);
	usOrigin = (USHORT)(BYTEFROMINDEX(indInfo) % sizeof(aucBlock));

	/* --- look for a vacant cell position --- */

	while (ulRest) {

		/* --- get no. of bytes to be read --- */

		usBytes = (USHORT)(pHeader->ulBytesIndex - ulAlign);	/* bytes to be read		*/
		usBytes = (usBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : usBytes;

		/* --- cheating for easy algorithm --- */

		if (usBytes != sizeof(aucBlock)) {		/* if less than buffer size	*/
			memset(aucBlock, '\xFF', sizeof(aucBlock));
		}

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + ulAlign, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */

		if (PifReadFile(hfDirty, aucBlock, usBytes) != usBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((ulAlign + (ULONG)usBytes) >= pHeader->ulBytesIndex) {
			*(aucBlock + usBytes - 1) |= ucTail;
		}

		/* --- search inactive byte in the block --- */

		if (pucFound = SearchFreeByte(aucBlock, usOrigin, sizeof(aucBlock))) {
			*pulFree  = (ulAlign + (ULONG)(pucFound - aucBlock)) * 8L;
			*pulFree += (ULONG)(7 - ComputeFreeBit(*pucFound));
			usStat    = SPLU_COMPLETED;
			break;
		}

		/* --- compute next reading information --- */

		ulAlign += sizeof(aucBlock);			/* compute next alignment	*/
		ulAlign  = (ulAlign > pHeader->ulBytesIndex) ? 0 : ulAlign;
		ulRest  -= usBytes;						/* remained bytes of table	*/
	}

	return (usStat);
}

/**
;========================================================================
;
;   function : Look for occupied cell
;
;   format : USHORT		PhySearchOccupied(hf, ulCell, pHeader, pulFound);
;
;   input  : USHORT		hf;				- file handle
;			 ULONG 		ulCell;			- cell no.　### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 PFILEHDR	pHeader;		- header information ptr.
;			 ULONG		*pulFound;		- pointer to occupied cell no. ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT	PhySearchOccupied(USHORT hf, ULONG ulCell, PFILEHDR pHeader, ULONG *pulFound)
{
	UCHAR	aucBlock[SIZEDISKIO], *pucFound, ucTail, ucBit, ucMask;
	USHORT	usStat, hfDirty, i;
	ULONG	ulPos, usBytes, ulNew, ulRest, ulOffset;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);		/* compute offset in index	*/
	ulPos   = pHeader->ulOffsetIndex;			/* top of index table		*/
	usStat  = SPLU_END_OF_FILE;					/* assume not found status	*/

	/* --- compute pattern of bottom index table --- */

	ucTail  = (UCHAR)((UCHAR)0xFF >> ((pHeader->ulMaxRec - 1L) % 8L + 1L));

	/* --- compute a block offset --- */

	ulOffset = BYTEFROMINDEX(indInfo);
	ucBit    = PTRNFROMINDEX(indInfo);
	ulRest   = pHeader->ulBytesIndex - ulOffset;/* remained bytes of table	*/

	/* --- look for a vacant cell position --- */

	while (ulRest) {

		/* --- get no. of bytes to be read --- */

		usBytes = (USHORT)(pHeader->ulBytesIndex - ulOffset);	/* bytes to be read		*/
		usBytes = (usBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : usBytes;

		/* --- cheating for easy algorithm --- */

		if (usBytes != sizeof(aucBlock)) {		/* if less than buffer size	*/
			memset(aucBlock, '\x00', sizeof(aucBlock));
		}

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + ulOffset, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */

		if (PifReadFile(hfDirty, aucBlock, usBytes) != usBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((ulOffset + (ULONG)usBytes) >= pHeader->ulBytesIndex) {
			*(aucBlock + usBytes - 1) &= ~ucTail;
		}

		/* --- set inactive at the top of position --- */

		ucMask       = 0xFF;
		ucMask     >>= (7 - ComputeFreeBit((UCHAR)(~ucBit)));
		aucBlock[0] &= ucMask;

		/* --- search active byte in the block --- */

		for (pucFound = aucBlock, i = 0; i < usBytes; pucFound++, i++) {

			/* --- is busy at this byte ? --- */

			if (*pucFound) {

				/* --- compute its cell number --- */

				*pulFound  = (ulOffset + (ULONG)(pucFound - aucBlock)) * 8;
				*pulFound += (7 - ComputeFreeBit((UCHAR)(~(*pucFound))));
				usStat     = SPLU_COMPLETED;
				break;
			}
		}

		/* --- could be found ? --- */

		if (! usStat) {
			break;
		}

		/* --- compute next reading information --- */

		ulOffset += sizeof(aucBlock);			/* compute next alignment	*/
		ulRest   -= usBytes;					/* remained bytes of table	*/
		ucBit     = MSB;						/* search from MSB			*/
	}

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
;   format : UCHAR		*SearchFreeByte(pucBuffer, usOrigin, usSize);
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

static UCHAR	*SearchFreeByte(UCHAR *pucBuffer, USHORT usOrigin, USHORT usSize)
{
#if defined(SET_NO_ASM)
	// ensure usOrigin is within the buffer size. if not then set to end.
	// si will not advance further but we will go backwards in a search for
	// a free position.
	if (usOrigin >= usSize) usOrigin = usSize - 1;

	{
		// (di) = backward offset, (si) = foreward offset
		LONG   di = usOrigin, si = usOrigin + 1;


		/* --- look for a byte which has at least one free bit --- */
		for (; di >= 0 || si < usSize; di--, si++) {
			if (di >= 0 && pucBuffer[di] != 0xff)  // --- is available on back side ? ---
				return pucBuffer + di;
			else if (si < usSize && pucBuffer[si] != 0xff) // --- is available on fore side ? ---
				return pucBuffer + si;
		}
	}
	return NULL;
#else
	SHORT	fFound;		/* ### MOD 2172 Rel1.0 (BOOL->SHORT) */
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

		push	bx					; save working registers ### MOD 2172 Rel1.0    Saratoga
		push	cx					; Saratoga
		push	dx					; Saratoga
		;;;push	ds					; Saratoga
		push	di					; Saratoga
		push	si					; Saratoga

		xor	ax, ax			; clear regs ### ADD 2172 Rel1.0    /* Saratoga */
		xor	cx, cx			;
		xor	dx, dx			;
		xor	si, si			;
		xor	di, di			;

		;--- get parameters & set up ---
		lds		bx, pucBuffer		; (ds:bx) = pucBuffer, top ptr. of buffer   Saratoga
;		mov		bx, pucBuffer		; ebx = pucBuffer ### MOD 2172 Rel1.0   Saratoga
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
		mov		al, [bx + di]		; ### MOD 2172 Rel1.0           Saratoga
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
		;;;mov		al, [bx + di]		; ### MOD 2172 Rel1.0       Saratoga
		mov		al, [bx + si]		; ### MOD 2172 Rel1.0           Saratoga (BUG Fix 03/31/00)
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
		pop		si					; recover registers         Saratoga
		pop		di					;                           Saratoga
		;;;pop		ds					; ### DEL 2172 Rel1.0   Saratoga
		pop		dx					;                           Saratoga
		pop		cx					;                           Saratoga
		pop		bx					;                           Saratoga
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
#endif
}

/**
;========================================================================
;
;   function : Compute free bit's order
;
;   format : USHORT		ComputeFreeBit(uchByte);
;
;   input  : UCHAR		uchByte;		- a byte data
;
;   output : USHORT		usOrder;		- free bit's order no.
;
;========================================================================
**/

static USHORT	ComputeFreeBit(UCHAR uchByte)
{
#if defined(SET_NO_ASM)
	UCHAR  usPat = 0x01;

	for (SHORT sCount = 7; sCount >= 0; sCount--) {
		if (((usPat << sCount) & uchByte) == 0) return sCount;
	}

	return 255; // Same value provided by assembler version of this function that is being replaced.
#else
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
#endif
}

/**
;========================================================================
;
;   function :	Compute Offset of Index Table
;
;   format : INDEX		ComputeIndexOffset(usCell);
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

static INDEX	ComputeIndexOffset(ULONG ulCell)/* ### MOD 2172 Rel1.0 (100,000item)*/
{
	INDEX	indInfo;
	UCHAR	uchBitmap = MSB;

	indInfo.ulOffset = ulCell / 8L;
	indInfo.usBitNumber = (USHORT)(0x7 - (ulCell % 8L));
	indInfo.uchBitPattern = (UCHAR)(uchBitmap >> (ulCell % 8L));
	return	indInfo;

#ifdef	_DEL_2172_R10	/* ### MOD 2172 Rel1.0 (100,000item)*/
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
	return (indOffset);
#endif
}
#pragma	optimize("egl", on)
/*==========================================================================*\
||							End of PLUPHYSC.C								||
\*==========================================================================*/

