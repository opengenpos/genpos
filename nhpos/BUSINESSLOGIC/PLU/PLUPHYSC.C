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
* 10/19/18 : 02.02.02  : R.Chambers  : replace magic constant 0 with defined constant SPLU_COMPLETED
* 10/19/18 : 02.02.02  : R.Chambers  : update comments.
* 10/19/18 : 02.02.02  : R.Chambers  : eliminate FARCONST and FAR.
* 10/19/18 : 02.02.02  : R.Chambers  : make local functions (SearchFreeByte, etc.) static.
* 10/19/18 : 02.02.02  : R.Chambers  : replace memcpy() with struct assignments.
* 10/19/18 : 02.02.02  : R.Chambers  : localized variables, eliminated compiler warnings.
* 10/30/18 : 02.02.02  : R.Chambers  : check file handle provided is not zero in some functions.
*
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

#include	<tchar.h>
#include    <memory.h>							/* string C-library			*/

#include	<ecr.h>								/* 2170 system header		*/
#include    <pif.h>								/* PifXXXX() header			*/
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/



/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- adjustment for keeping handle from zero value to allow check nonzero for open --- */

#define		MAGIC_HANDLE		1		/* file handle value adjustment for nonzero or TRUE value, see struct INFTBL */



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

static UCHAR 	aucReadOnly[]  = "ro";				/* open w/ read mode, file must exist	*/
static UCHAR 	aucReadWrite[] = "rwof";			/* open w/ read & write, file must exist	*/
static UCHAR 	aucCreateNew[] = "rwnf";			/* create newly			*/



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
;   input  : UCHAR   	*pszName;		- file spec. name
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

USHORT	PhyCreateFile(TCHAR  *pszName, USHORT *phf, USHORT usID, USHORT usType,
					 USHORT usUnit, ULONG ulRec)
{
	SHORT		hf;
	ULONG		ulBytes, ulCells, ulNew, ulIndex;

	*phf = 0;    // default file handle to zero indicating error if open fails.

	/* --- create a file newly --- */

	if ((hf = PifOpenFile(pszName, aucCreateNew)) < 0) {
		return (SPLU_SYSTEM_FAIL);
	}

	/* --- compute file size --- */

    if (usType == TYPE_INDEX) { /* R2.0 */
        ulIndex = 0;
    } else {
    	ulIndex = ulRec / 8L + ((ulRec % 8L) ? 1L : 0L);	/* size of index table 	*/
    }
	ulCells = (ULONG)usUnit * ulRec;			/* size of rec. cells	*/
	ulBytes = (ULONG)sizeof(FILEHDR) + ulIndex + ulCells;

	/* --- reserve a file w/ enough size --- */

	if (PifSeekFile(hf, ulBytes, &ulNew) < 0) {		/* reserve a memory		*/
		PifCloseFile(hf);								/* close it			*/
		PifDeleteFile(pszName);							/* delete it		*/
		return (SPLU_DEVICE_FULL);
	}

	/* --- return to top of a file --- */

	PifSeekFile(hf, 0L, &ulNew);					/* go back to top		*/

	/* --- make file header --- */

	{
		FILEHDR	    Header = {0};

		Header.ulFileSize     = ulBytes;				/* file size in byte	*/
		Header.usFileNumber   = usID;					/* unique ID of file's	*/
		Header.usFileType     = usType;				/* file type			*/
		Header.ulHashKey      = ulRec;				/* key value for hashing*/
		Header.ulMaxRec       = ulRec;				/* no. of max. records	*/
		Header.ulCurRec       = 0;					/* no. of cur. records	*/
		Header.usRecLen       = usUnit;				/* bytes per record		*/
		Header.ulOffsetIndex  = sizeof(FILEHDR);		/* offset to index table*/
		Header.ulBytesIndex   = ulIndex;				/* bytes of index table	*/
		Header.ulOffsetRecord	= sizeof(FILEHDR) + ulIndex;	/* offset to rec*/
		Header.ulBytesRecord  = ulCells;				/* bytes of record area	*/

		/* --- write file header, however not knowing it's completed. --- */

		PifWriteFile(hf, &Header, sizeof(FILEHDR));
	}

	/* --- write initial contents of index table --- */

	while (ulIndex) {								/* loop whole index size*/
		ULONG		ulWrite;
		UCHAR		aucBlock[SIZEDISKIO] = {0};

		ulWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;
		ulIndex -= ulWrite;
		PifWriteFile(hf, aucBlock, ulWrite);
	}

	/* --- respond a handle --- */

	*phf = (hf + MAGIC_HANDLE);    // transform to dirty, nonzero file handle

	return (SPLU_COMPLETED);
}

/**
;========================================================================
;
;   function :	Delete a file from file system
;
;   format : USHORT		PhyDeleteFile(pszName);
;
;   input  : UCHAR 	*pszName;		- file spec. name
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT	PhyDeleteFile(TCHAR  *pszName)
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
	USHORT		hfDirty;
	ULONG		ulNew, ulIndex;

	if (hf < 1) return (SPLU_INVALID_FILE);		/* bad file handle !	*/

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- read a content of header --- */
	{
		ULONG		ulActualBytesRead; //JHHJ;
		FILEHDR	    Header = {0};
		QUEUE       QueueInit = {0};   // initial queue state.

		/* --- return to top of a file to read the header --- */
		if (PifSeekFile(hfDirty, 0L, &ulNew) != PIF_OK) {
			return (SPLU_FILE_BROKEN);
		}

		//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
		PifReadFile(hfDirty, &Header, sizeof(FILEHDR), &ulActualBytesRead);
		if ( ulActualBytesRead != sizeof(FILEHDR)) {
			return (SPLU_FILE_BROKEN);
		}

		/* --- reset only no. of cur. records --- */
		ulIndex   = Header.ulBytesIndex;		/* byte of index table	*/
		Header.ulCurRec = 0;					/* reset no. of rec.	*/
		Header.queListMainte = QueueInit;
		Header.queReserved = QueueInit;

		/* --- return to top of a file to write the modified header --- */
		if (PifSeekFile(hfDirty, 0L, &ulNew) != PIF_OK) {
			return (SPLU_FILE_BROKEN);
		}

		/* --- write file header, however not knowing it's completed. --- */
		PifWriteFile(hfDirty, &Header, sizeof(FILEHDR));
	}

	/* --- write initial contents of index table --- */

	while (ulIndex > 0) {/* ### */					/* loop whole index size*/
		ULONG		ulWrite;
		UCHAR		aucBlock[SIZEDISKIO] = {0};

		ulWrite  = (sizeof(aucBlock) < ulIndex) ? sizeof(aucBlock) : ulIndex;
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
;   input  : UCHAR  	*pszName;	- file spec. name
;			 USHORT		*phf;		- pointer to respond handle
;			 BOOL		fWrite;		- read only / read & write
;
;   output : USHORT		usStat;		- status
;
;========================================================================
**/

USHORT	PhyOpenFile(TCHAR  *pszName, USHORT *phf, BOOL fWrite)
{
	SHORT		hf;
	UCHAR   	*pMode = (fWrite ? aucReadWrite : aucReadOnly);

	*phf = 0;    // default file handle to zero indicating error if open fails.

	/* --- open a file --- */

	if ((hf = PifOpenFile(pszName, pMode)) < 0) {
		return (SPLU_FILE_NOT_FOUND);
	}

	/* --- respond file handle --- */

	*phf = hf + MAGIC_HANDLE;    // transform to dirty, nonzero file handle

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
	ULONG		ulRecord, ulTotal,ulIndex, ulActualBytesRead;//JHHJ
	ULONG		ulMoved;
	FILEHDR		hdrInf;

	/* --- compute dirty handle --- */

	if (hf < 1) return (SPLU_INVALID_FILE);		/* bad file handle !	*/

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_INVALID_FILE);					/* out of PLU file !	*/
	}

	/* --- read a record header --- */
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, &hdrInf, sizeof(FILEHDR), &ulActualBytesRead);
	if ( ulActualBytesRead != sizeof(FILEHDR)) {
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


USHORT	PhyReadFile(USHORT hf, UCHAR *pucBuffer, ULONG *pusBytes, ULONG ulOffset, ULONG *pulActualBytesRead)//JHHJ
{
	USHORT	hfDirty;
	ULONG	ulMoved;

	if (hf < 1) return (SPLU_INVALID_FILE);		/* bad file handle !	*/

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move the file position --- */

	if (PifSeekFile(hfDirty, ulOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- read directly --- */

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, pucBuffer, *pusBytes, pulActualBytesRead);

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


USHORT	PhyWriteFile(USHORT hf, UCHAR *pucBuffer, ULONG *pulBytes, ULONG ulOffset)
{
	USHORT	hfDirty;
	ULONG	ulMoved;

	if (hf < 1) return (SPLU_INVALID_FILE);		/* bad file handle !	*/

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move the file position --- */

	if (PifSeekFile(hfDirty, ulOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- write directly --- */

	PifWriteFile(hfDirty, pucBuffer, *pulBytes);

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

	if (hf < 1) return (SPLU_INVALID_FILE);		/* bad file handle !	*/

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
#if defined(PhyGetInfo)
USHORT	PhyGetInfo_Special(USHORT hf, PFILEHDR pHeader);

USHORT	PhyGetInfo_Debug(USHORT hf, PFILEHDR pHeader, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "  PhyGetInfo_Debug(): hf = %d, File %s, lineno = %d", hf, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return PhyGetInfo_Special(hf, pHeader);
}

USHORT	PhyGetInfo_Special(USHORT hf, PFILEHDR pHeader)
#else
USHORT	PhyGetInfo(USHORT hf, PFILEHDR pHeader)
#endif
{
	USHORT	hfDirty;
	ULONG	ulMoved, ulActualBytesRead;//JHHJ 11-10-03

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read a content of header --- */
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, pHeader, sizeof(FILEHDR), &ulActualBytesRead);
	if ( ulActualBytesRead != sizeof(FILEHDR)) {
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
	ULONG		ulMoved, ulActualBytesRead;//JHHJ 11-10-03
	FILEHDR		hdrInf;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move a file position to top --- */

	if (PifSeekFile(hfDirty, 0L, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read a record directly --- */
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, &hdrInf, sizeof(FILEHDR), &ulActualBytesRead);
	if ( ulActualBytesRead != sizeof(FILEHDR)) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- override only user area --- */

	hdrInf.ulCurRec = pHeader->ulCurRec;
	hdrInf.queListMainte = pHeader->queListMainte;
	hdrInf.queReserved = pHeader->queReserved;
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


USHORT	PhyReadRecord(USHORT hf, ULONG ulCell, UCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	hfDirty;
	ULONG	ulRecOffset/* ### */, ulBytes, ulMoved, ulActualBytesRead;//JHHJ 11-10-03

	/* --- compute a file position --- */

	ulBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRecOffset   = (pHeader->ulOffsetRecord);		/* top of record data	*/
	ulRecOffset  += (ulBytes * ulCell);				/* add record offset	*/

	/* --- is in a file ? --- */

	if (ulCell >= pHeader->ulMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move to the record position --- */

	if (PifSeekFile(hfDirty, ulRecOffset, &ulMoved) != PIF_OK) {
		return (SPLU_FILE_BROKEN);						/* must be broken	*/
	}

	/* --- read a record directly --- */
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, pucBuffer, ulBytes, &ulActualBytesRead);
	if ( ulActualBytesRead != ulBytes) {
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


USHORT	PhyWriteRecord(USHORT hf, ULONG ulCell, UCHAR *pucBuffer, PFILEHDR pHeader)
{
	USHORT	usBytes, hfDirty;
	ULONG	ulRec, ulMoved;

	/* --- compute a file position --- */

	usBytes = pHeader->usRecLen;					/* record length in byte*/
	ulRec   = pHeader->ulOffsetRecord;				/* top of record data	*/
	ulRec  += (ULONG)usBytes * ulCell;				/* add record offset	*/

	/* --- is in a file ? --- */

	if (ulCell >= pHeader->ulMaxRec) {				/* if overflow ...		*/
		return (SPLU_FILE_OVERFLOW);					/* overflow !		*/
	}

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

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
	ULONG	ulPos, ulNew, ulActualBytesRead;//JHHJ 11-10-03
	INDEX	indInfo;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);
	ulPos   = pHeader->ulOffsetIndex + BYTEFROMINDEX(indInfo);

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move a file pointer to just position --- */

	if (PifSeekFile(hfDirty, ulPos, &ulNew) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read the byte data --- */
	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, &uchByte, sizeof(uchByte), &ulActualBytesRead);
	if ( ulActualBytesRead != sizeof(uchByte)) {
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
	ULONG	ulPos, ulNew, ulActualBytesRead;//JHHJ 11-10-03
	INDEX	indInfo;

	/* --- compute offset of index table --- */

	indInfo = ComputeIndexOffset(ulCell);		/* compute offset in index	*/
	ulPos   = pHeader->ulOffsetIndex + BYTEFROMINDEX(indInfo);

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

	/* --- move a file pointer to just position --- */

	if (PifSeekFile(hfDirty, ulPos, &ulNew) != PIF_OK) {
		return (SPLU_FILE_BROKEN);
	}

	/* --- read the byte data --- */

	//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
	PifReadFile(hfDirty, &uchByte, sizeof(uchByte), &ulActualBytesRead);
	if (ulActualBytesRead != sizeof(uchByte)) {
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
	USHORT	 usStat, usOrigin, hfDirty;
	ULONG	ulPos, ulAlign, ulRest;
	INDEX	indInfo;

	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

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
		ULONG	ulBytes, ulNew;
		ULONG	ulActualBytesRead; //JHHJ 11-10-03

		/* --- get no. of bytes to be read --- */

		ulBytes = (pHeader->ulBytesIndex - ulAlign);	/* bytes to be read		*/
		ulBytes = (ulBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : ulBytes;

		/* --- cheating for easy algorithm, init to not occupied if not full read. --- */

		if (ulBytes != sizeof(aucBlock)) {		/* if less than buffer size	*/
			memset(aucBlock, '\xFF', sizeof(aucBlock));
		}

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + ulAlign, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */
		//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
		PifReadFile(hfDirty, aucBlock, ulBytes, &ulActualBytesRead);
		if ( ulActualBytesRead != ulBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((ulAlign + ulBytes) >= pHeader->ulBytesIndex) {
			*(aucBlock + ulBytes - 1) |= ucTail;
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
		ulRest  -= ulBytes;						/* remained bytes of table	*/
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
	UCHAR	*pucFound, ucTail, ucBit, ucMask;
	USHORT usStat, hfDirty, i;
	ULONG	ulPos, ulRest, ulOffset;
	INDEX	indInfo;
	/* --- compute dirty handle --- */

	hfDirty = hf - MAGIC_HANDLE;    // transform back to actual, real file handle

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
		ULONG	ulBytes, ulNew;
		ULONG	ulActualBytesRead; //JHHJ 11-10-03
		UCHAR   aucBlock[SIZEDISKIO] = {0};  // init to not occupied in case of not full read.

		/* --- get no. of bytes to be read --- */

		ulBytes = (pHeader->ulBytesIndex - ulOffset);	/* bytes to be read		*/
		ulBytes = (ulBytes > sizeof(aucBlock)) ? sizeof(aucBlock) : ulBytes;

		/* --- move a file pointer to just position --- */

		if (PifSeekFile(hfDirty, ulPos + ulOffset, &ulNew) != PIF_OK) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- read a part (or the whole) of table contents --- */
		//11-10-03 JHHJ Change made in PifReadFile, see piffile.c for new usage
		PifReadFile(hfDirty, aucBlock, ulBytes, &ulActualBytesRead);

		if ( ulActualBytesRead != ulBytes) {
			usStat = SPLU_FILE_BROKEN;				/* file must be broken	*/
			break;									/* break my job			*/
		}

		/* --- does read the bottom block of table ? --- */

		if ((ulOffset + ulBytes) >= pHeader->ulBytesIndex) {
			*(aucBlock + ulBytes - 1) &= ~ucTail;
		}

		/* --- set inactive at the top of position --- */

		ucMask       = 0xFF;
		ucMask     >>= (7 - ComputeFreeBit((UCHAR)(~ucBit)));
		aucBlock[0] &= ucMask;

		/* --- search active byte in the block --- */

		for (pucFound = aucBlock, i = 0; i < ulBytes; pucFound++, i++) {

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

		if (SPLU_COMPLETED == usStat) {
			break;
		}

		/* --- compute next reading information --- */

		ulOffset += sizeof(aucBlock);			/* compute next alignment	*/
		ulRest   -= ulBytes;					/* remained bytes of table	*/
		ucBit     = MSB;						/* search from MSB			*/
	}

	return (usStat);
}

/*==========================================================================*\
;+																			+
;+				M I S C .    S U B - R O U T I N E s						+
;+																			+
\*==========================================================================*/

// From Microsoft Visual Studio 2015 MSDN online documentation.
//
// The optimize pragma must appear outside a function and takes effect at the
// first function defined after the pragma is seen. The on and off arguments
// turn options specified in the optimization-list on or off.
// These are the same letters used with the /O compiler options. For
// example, the following pragma is equivalent to the /Os compiler option:
//     #pragma optimize( "ts", on )
//
// In Visual Studio 2015 the only options seem to be the following so
// of the "egl" it looks like only "g" is still valid. Probably a holdover
// from Visual Studio 6.x and possibly to do with segmented memory addressing?
//   g         Enable global optimizations.
//   s or t    Specify short or fast sequences of machine code.
//   y         Generate frame pointers on the program stack.

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

UCHAR	*SearchFreeByte(UCHAR *pucBuffer, USHORT usOrigin, USHORT usSize)
{
#if 0
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
	SHORT	fFound = FALSE;	/* assume cannot be found	*/	/* ### MOD 2172 Rel1.0 (BOOL->SHORT) */
	UCHAR	*pucFound;
	USHORT	usOffset;

	/* --- look for a byte which has at lease a free bit --- */
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

	/* --- cound be found ? --- */
	pucFound = (fFound ? (pucBuffer + usOffset) : NULL);

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
;   description:  This function examines a byte iterating from most
;                 significant bit to least significant bit looking for
;                 a bit with a value of 0. The offset of that bit from
;                 the first bit of the byte, a value from 0 to 7, is
;                 returned.
;
;                 This function assumes there is at least one free bit
;                 in the byte.
;========================================================================
**/

USHORT	ComputeFreeBit(UCHAR uchByte)
{
#if 0
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

INDEX	ComputeIndexOffset(ULONG ulCell)/* ### MOD 2172 Rel1.0 (100,000item)*/
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

