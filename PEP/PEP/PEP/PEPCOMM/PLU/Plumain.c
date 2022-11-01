/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 2170     *             NCR Corporation, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994				||
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
*  Date     : Ver.Rev. :   Name     : Description
*
** NCR2172 **
* Nov-20-99 : 1.00.00  : K.Iwata    : initial (for 2172)
* Jan-12-00 :          : K.Iwata    : ADD (CheckPluNumber)
*           :          :            :
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

#include    <stdlib.h>
#include    <memory.h>							/* memory C-library			*/
#include	<ecr.h>								/* 2170 system header		*/
#include    <r20_pif.h>								/* PifXXXX() header			*/
#include    "mypifdefs.h"
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/

#include	<pepent.h>


/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

USHORT	CheckPluNumber(const SHORT nFileType,const VOID *pvBuf);


/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- exclusive semaphore against API calls --- */
static	USHORT	usApiSem = -1;						/* semaphore handle			*/


/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/




/*==========================================================================*\
;+																			+
;+					M a s s    M e m o r y    A P I s						+
;+																			+
\*==========================================================================*/



/**
;========================================================================
;
;   function :	Initialize Mass Memory Module
;
;   format : VOID	PluInitialize(VOID);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/
VOID PluInitialize(VOID)
{
	if (!(usApiSem >= 0 && usApiSem < 0x7ff)) {
		/* --- allocate a semaphore against duplicate API calls --- */
		usApiSem = PifCreateSem(1);					/* allocate a semaphore		*/
		/* --- pass the control to executive routine --- */
		ExeInitialize(FALSE);						/* for first initialization	*/
	}
	return;
}

/**
;========================================================================
;
;   function :	Finalize Mass Memory Module
;
;   format : VOID	PluFinalize(VOID);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/
VOID PluFinalize(VOID)
{
	if (usApiSem >= 0 && usApiSem < 0x7ff) {
		/* --- discard a semaphore --- */
		PifDeleteSem(usApiSem);						/* discard a semaphore		*/
		/* --- pass the control to executive routine --- */
		ExeFinalize();
		usApiSem = -1;
	}
	return;
}

/**
;========================================================================
;
;   function :	Refresh Mass Memory Module
;
;   format : VOID	PluRefresh(VOID);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID PluRefresh(VOID)
{
	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	ExeInitialize(TRUE);						/* for refresh				*/

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return;
}

/**
;========================================================================
;
;   function : Create a Mass Memory file
;
;   format : USHORT		PluCreateFile(usFile, usNoOfRec);
;
;   input  : USHORT		usFile;			- file ID to create
;			 ULONG		ulNoOfRec;		- no. of records	### MOD 2172 Rel1.0 (USHORT->ULONG) 100,000items
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluCreateFile(USHORT usFile, ULONG ulNoOfRec)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeCreateFile(usFile, ulNoOfRec);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a Mass Memory file
;
;   format : USHORT		PluDeleteFile(usFile);
;
;   input  : USHORT		usFile;			- file ID to delete
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluDeleteFile(USHORT usFile)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeDeleteFile(usFile);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Clear contents of a Mass Memory file
;
;   format : USHORT		PluClearFile(usFile);
;
;   input  : USHORT		usFile;			- file ID to clear
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluClearFile(USHORT usFile)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeClearFile(usFile);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Get contents of a Mass Memory file
;
;   format : USHORT		PluSenseFile(usFile, pFileInfo);
;
;   input  : USHORT		usFile;			- file ID to get info.
;			 PMFINFO	pFileInfo;		- buffer ptr. to respond info.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluSenseFile(USHORT usFile, PMFINFO pFileInfo)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeSenseFile(usFile, pFileInfo);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Enter a critical mode
;
;   format : USHORT		PluEnterCritMode(usFile, usFunc, pusHandle, pvParam);
;
;   input  : USHORT		usFile;			- file ID to enter a crit. mode
;			 USHORT		usFunc;			- function no.
;			 USHORT		*pusHandle;		- buffer ptr. to respond handle
;			 VOID		*pvParam;		- parameter data ptr.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluEnterCritMode(USHORT usFile, USHORT usFunc, USHORT *pusHandle, VOID *pvParam)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeEnterCritMode(usFile, usFunc, pusHandle, pvParam);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Exit from a critical mode
;
;   format : USHORT		PluExitCritMode(usHandle, usReserve);
;
;   input  : USHORT		usHandle;		- handle value
;			 USHORT		usReserve;		- reserved (must be 0)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluExitCritMode(USHORT usHandle, USHORT usReserve)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeExitCritMode(usHandle, usReserve);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Kill all users in a critical mode
;
;   format : USHORT		PluKillCritMode(usFile, usReserve);
;
;   input  : USHORT		usFile;			- file number
;			 USHORT		usReserve;		- reserved (must be 0)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluKillCritMode(USHORT usFile, USHORT usReserve)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeKillCritMode(usFile, usReserve);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add a record
;
;   format : USHORT		PluAddRecord(usFile, pvRec);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluAddRecord(USHORT usFile, VOID *pvRec)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(usFile,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeAddRecord(usFile, pvRec);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Replace a record
;
;   format : USHORT		PluReplaceRecord(usFile, pvRec, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT PluReplaceRecord(USHORT usFile, VOID *pvRec, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(usFile,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReplaceRecord(usFile, pvRec, pvBuffer, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Change a record
;
;   format : USHORT		PluChangeRecord(usFile, pvRec, flChanges,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;			 ULONG		flChanges;		- change fields flag
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluChangeRecord(USHORT usFile, VOID *pvRec, ULONG flChanges, USHORT fsProg, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(usFile,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeChangeRecord(usFile,
							 pvRec,
							 flChanges,
							 fsProg,
							 pvBuffer,
							 pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a record
;
;   format : USHORT		PluDeleteRecord(usFile, pItemNumber, usAdj,
;												fsProg, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 USHORT		fsProg;			- programability
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*usRet;			- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluDeleteRecord(USHORT  usFile, VOID *pItemNumber, USHORT usAdj, 
							USHORT fsProg, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(-1,pItemNumber))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeDeleteRecord(usFile,
							 pItemNumber,
							 usAdj,
							 fsProg,
							 pvBuffer,
							 pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Delete a reserved record
;
;   format : USHORT		PluDeleteReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluDeleteReserved(USHORT usHandle)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeDeleteReserved(usHandle);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Reset a reserved record
;
;   format : USHORT		PluResetReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluResetReserved(USHORT usHandle)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeResetReserved(usHandle);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Rewrite a reserved record
;
;   format : USHORT		PluRewriteReserved(usHandle, pvRecord);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvRecord;		- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluRewriteReserved(USHORT usHandle, VOID *pvRecord)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeRewriteReserved(usHandle, pvRecord);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Cancel a reserved record
;
;   format : USHORT		PluCancelReserved(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluCancelReserved(USHORT usHandle)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeCancelReserved(usHandle);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Append a maintenance record
;
;   format : USHORT		PluAppendMainte(usFile, pvRec);
;
;   input  : USHORT		usFile;			- file ID
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluAppendMainte(USHORT usFile, VOID *pvRec)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(usFile,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeAppendMainte(usFile, pvRec);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Purge a maintenance record
;
;   format : USHORT		PluPurgeMainte(usHandle);
;
;   input  : USHORT		usHandle;		- handle value
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluPurgeMainte(USHORT usHandle)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExePurgeMainte(usHandle);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Apply a maintenance record
;
;   format : USHORT		PluApplyMainte(usHandle, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;			 USHORT		*pusApplied;	- buffer ptr. to respond applied stat
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluApplyMainte(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet, USHORT *pusApplied)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeApplyMainte(usHandle,			/* apply a mainte. record	*/
							pvBuffer,				/* buffer for old rec.	*/
							pusRet,					/* buffer rec. length	*/
							NULL,					/* none for maint. rec	*/
							(USHORT *)NULL,			/* none for its length	*/
							pusApplied);			/* applied status		*/

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Apply a maintenance record 2nd version
;
;   format : USHORT
;			 PluApplyMainte2(usHandle, pvBuffer, pusRet, pvMainte, pusLen);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond old rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;			 VOID		*pvMainte;		- buffer ptr. to receive mainte. rec
;			 USHORT		*pusLen;		- buffer ptr. to receive its length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluApplyMainte2(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet, VOID *pvMainte, USHORT *pusLen)
{
	USHORT	usStat, usTrash;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeApplyMainte(usHandle,			/* apply a mainte. record	*/
							pvBuffer,				/* buffer for old rec.	*/
							pusRet,					/* buffer rec. length	*/
							pvMainte,				/* buffer for maint. rec*/
							pusLen,					/* buffer for its length*/
							&usTrash);				/* none for applied stat*/

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Report associated maintenance records
;
;   format : USHORT		PluReportMainte(usHandle, pvBuffer, pulKey, pusRet);
;
;   input  : USHORT		usHandle;		- handle
;			 VOID		*pvBuffer;		- buffer ptr. to respond record
;			 ULONG		*pulKey;		- buffer ptr. to respond key	### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. len.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReportMainte(USHORT usHandle, VOID *pvBuffer, ULONG *pulKey, USHORT *pusRet)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReportRecord(usHandle, pvBuffer, pulKey, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Remove associated maintenance records
;
;   format : USHORT		PluRemoveMainte(usHandle, ulKey);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to remove	### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluRemoveMainte(USHORT usHandle, ULONG ulKey)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeRemoveMainte(usHandle, ulKey);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Modify associated maintenance records
;
;   format : USHORT		PluModifyMainte(usHandle, usKey, pvRec);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to modify	### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 VOID		*pvRec;			- ptr. to record data
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluModifyMainte(USHORT usHandle, ULONG ulKey, VOID *pvRec)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(FILE_MAINT_PLU,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeModifyMainte(usHandle, ulKey, pvRec);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read associated maintenance records
;
;   format : USHORT		PluReadMainte(usHandle, ulKey, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle
;			 ULONG		ulKey;			- key of record to modify ### MOD 2172 Rel1.0 (USHORT->ULONG)
;			 VOID		*pvBuffer;		- buffer ptr. to respond record
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. len.
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReadMainte(USHORT usHandle, ULONG ulKey, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReadMainte(usHandle, ulKey, pvBuffer, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Add associated maintenance records
;
;   format : USHORT		PluAddMainte(usHandle, pvRec, pulKey);
;
;   input  : USHORT		usHandle;		- handle
;			 VOID		*pvRec;			- ptr. to record data
;			 ULONG		*pulKey;		- buffer ptr. to respond key ### MOD 2172 Rel1.0 (USHORT->ULONG)
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluAddMainte(USHORT usHandle, VOID *pvRec, ULONG *pulKey)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(FILE_MAINT_PLU,pvRec))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeAddMainte(usHandle, pvRec, pulKey);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record
;
;   format : USHORT
;			 PluReadRecord(usFile, pItemNumber, usAdj, pvBuffer, pusRet);
;
;   input  : USHORT		usFile;			- file ID
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReadRecord(USHORT  usFile,
							PITEMNO pItemNumber,
							USHORT  usAdj,
							VOID    *pvBuffer,
							USHORT  *pusRet)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(-2,pItemNumber))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReadRecord(usFile, pItemNumber, usAdj, pvBuffer, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read a record and reserve it
;
;   format : USHORT
;			 PluReserveRecord(usHandle, pItemNumber, usAdj, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle value
;			 PITEMNO	pItemNumber;	- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReserveRecord(USHORT  usHandle, PITEMNO pItemNumber, USHORT  usAdj,
						VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(-2,pItemNumber))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReserveRecord(usHandle, pItemNumber, usAdj, pvBuffer, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Report a record sequentialy
;
;   format : USHORT		PluReportRecord(usHandle, pvBuffer, pusRet);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReportRecord(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReportRecord(usHandle, pvBuffer, NULL, pusRet);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Update a record
;
;   format : USHORT		PluUpdateRecord(usFile, pNumber, usAdj, pUpdate,
;														pvBuffer, pfsStatus);
;
;   input  : USHORT		usHandle;		- handle value
;			 PITEMNO	pNumber;		- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 PUPDINF	pUpdate;		- ptr. to update info. buffer
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pfsStatus;		- buffer ptr. to respond status
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluUpdateRecord(USHORT usFile, PITEMNO pNumber, USHORT usAdj, PUPDINF pUpdate, VOID *pvBuffer, USHORT *pfsStatus)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(-2,pNumber))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeUpdateRecord(usFile,
							 pNumber,
							 usAdj,
							 pUpdate,
							 (PUPDINF)NULL,
							 pvBuffer,
							 pfsStatus);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Update a record
;
;   format : USHORT
;			 PluUpdateRecord2(usFile, pNumber, usAdj, pMainInfo, pLinkInfo,
;														pvBuffer, pfsStatus);
;
;   input  : USHORT		usHandle;		- handle value
;			 PITEMNO	pNumber;		- ptr. to item number
;			 USHORT		usAdj;			- adjective number
;			 PUPDINF	pMainInfo;		- ptr. to update info. for main rec.
;			 PUPDINF	pLinkInfo;		- ptr. to update info. for link rec.
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pfsStatus;		- buffer ptr. to respond status
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluUpdateRecord2(USHORT  usFile, PITEMNO pNumber, USHORT  usAdj,
						PUPDINF pMainInfo, PUPDINF pLinkInfo, VOID *pvBuffer, USHORT *pfsStatus)
{
	USHORT	usStat;

	/* ### ADD 2172 Rel1.0 */
	if(! CheckPluNumber(-2,pNumber))
		return	SPLU_INVALID_DATA;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeUpdateRecord(usFile,
							 pNumber,
							 usAdj,
							 pMainInfo,
							 pLinkInfo,
							 pvBuffer,
							 pfsStatus);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Read from a file
;
;   format : USHORT		PluReadBlock(usHandle, pucBuffer, usSize, pusRead);
;
;   input  : USHORT		usHandle;		- handle value
;			 UCHAR		*pucBuffer;		- output buffer ptr.
;			 USHORT		usSize;			- output buffer size
;			 USHORT		*pusRead;		- no. of bytes to have been read
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT PluReadBlock(USHORT usHandle, WCHAR *pucBuffer, USHORT usSize, USHORT *pusRead)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeReadBlock(usHandle, pucBuffer, usSize, pusRead);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Write to a file
;
;   format : USHORT		PluWriteBlock(usHandle, pucData, usBytes, pusWritten);
;
;   input  : USHORT		usHandle;		- handle value
;			 UCHAR		*pucData;		- data buffer ptr.
;			 USHORT		usBytes;		- data length
;			 USHORT		*pusWritten;	- no. of bytes to have been written
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT PluWriteBlock(USHORT usHf, WCHAR *pucData, USHORT usBytes, USHORT *pusWritten)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeWriteBlock(usHf, pucData, usBytes, pusWritten);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Seek a file pointer
;
;   format : USHORT		PluSeekBlock(usHandle, lMoves, usOrigin, pulNewPtr);
;
;   input  : USHORT		usHandle;		- handle value
;			 LONG		lMoves;			- value to move
;			 USHORT		usOrigin;		- origin parameter
;			 ULONG		ulNewPtr;		- new file pointer
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/


USHORT PluSeekBlock(USHORT usHandle, LONG lMoves, USHORT usOrigin, ULONG *pulNewPtr)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeSeekBlock(usHandle, lMoves, usOrigin, pulNewPtr);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Open a mirror file
;
;   format : USHORT		PluOpenMirror(pszFileName, pusMirrorFile);
;
;   input  : UCHAR FAR	*pszFileName;		- ptr. to file name
;			 USHORT		*pusMirrorFile;		- buffer ptr. to receive file ID
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluOpenMirror(WCHAR FAR *pszFileName, USHORT *pusMirrorFile)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeOpenMirror(pszFileName, NULL, pusMirrorFile);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Open a mirror file with index file, R2.0
;
;   format : USHORT		PluOpenMirror2(pszFileName, pszIndexName, pusMirrorFile);
;
;   input  : UCHAR FAR	*pszFileName;		- ptr. to file name
;			 USHORT		*pusMirrorFile;		- buffer ptr. to receive file ID
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluOpenMirror2(WCHAR FAR *pszFileName, WCHAR FAR *pszIndexName, USHORT *pusMirrorFile)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeOpenMirror(pszFileName, pszIndexName, pusMirrorFile);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Close a mirror file
;
;   format : USHORT		PluCloseMirror(pusMirrorFile);
;
;   input  : USHORT		usMirrorFile;	- file ID of mirror file
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluCloseMirror(USHORT usMirrorFile)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeCloseMirror(usMirrorFile);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/**
;========================================================================
;
;   function : Inform PLU Module Status             R1.1
;
;   format : USHORT		PluInformStatus(husHandle, *pPlu_Status);
;
;   input  : USHORT		usMirrorFile;	- file ID of mirror file
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluInformStatus(USHORT usHandle, PLU_STATUS *pPlu_Status)
{
	USHORT	usStat;

	/* --- get a semaphore prviledge --- */

	PifRequestSem(usApiSem);					/* block against dup. call	*/

	/* --- pass the control to executive routine --- */

	usStat = ExeInformStatus(usHandle, pPlu_Status);

	/* --- release a semaphore --- */

	PifReleaseSem(usApiSem);					/* free a dup. call			*/

	/* --- exit ... --- */

	return (usStat);
}

/*==========================================================================*\
	2172 Rel1.0
\*==========================================================================*/

/**
;========================================================================
;   ### ADD 2172 Rel1.0 (01/12/00)
;   function : check the plunumber 
;
;   format : USHORT	CheckPluNumber(const SHORT nFileType,const VOID *pvBuf){
;
;   input  : SHORT      nFileType ;	    - file type
;            VOID*      pvBuf ;         - record/key ptr
;
;   output : USHORT		1 (TRUE)        - OK
;                       0 (FALSE)       - error
;
;========================================================================
**/
USHORT	CheckPluNumber(const SHORT nFileType,const VOID *pvBuf){
	WCHAR	*pPluNo;
	SHORT	cnt,nLen;

	nLen = PLU_NUMBER_LEN;
	switch(nFileType){
		case	FILE_PLU:       /* PLU file */
			pPluNo = ((RECPLU *)pvBuf)->aucPluNumber ;
			break;
		case	FILE_MAINT_PLU: /* mainte file */
			pPluNo = ((MNTPLU *)pvBuf)->aucPluNumber ;
			break;
/*
		case OLDTYPE:
			pPluNo = ((RECPLU22 *)pvBuf)->aucPluNumber ;
			nLen = PLU_NUMBER_LEN_22;
			break;
*/
		case	FILE_PLU_INDEX:	/* index file */
			pPluNo = ((RECIDX *)pvBuf)->aucPluNumber ;
			break;

		case	-2:					/* struct ITEMNO */
			pPluNo = ((ITEMNO *)pvBuf)->aucPluNumber ;
			break;

		case	-1:                /* key etc */
			pPluNo = (WCHAR *)pvBuf;
			break;

		default:					/* no check */
			return	1;
	}
	
	for(cnt = 0;cnt < nLen;cnt++){
		/*if( pPluNo[cnt] < (UCHAR)0x30 || (UCAHR)0x39 < pPluNo[cnt] )*/
		switch(pPluNo[cnt]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				break;
			default:
				return	0;
		}
	}
	return	1;
}


/*==========================================================================*\
||							End of PLUMAIN.C								||
\*==========================================================================*/
