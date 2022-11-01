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
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*** Saratoga PEP (for 2170GP files)
*  02/09/00  | 1.00.00  | initial                               | K.Iwata
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

#include    <memory.h>							/* memory C-library			*/
#include	<ecr.h>								/* 2170 system header		*/
#include    <r20_pif.h>								/* PifXXXX() header			*/

/*#include	<PLU.H>*/							/* PLU common header		*/
/*#include	<PLUDEFIN.H>*/						/* Mass Memory Module header*/
#include	<pepent.h>
#include	"PluPrcGP.H"						/* ### ADD Saratoga-PEP (02/08/2000) */
#include	"PluGP.H"							/* ### MOD Saratoga-PEP (02/08/2000) */
#include	"PluDefGP.h"						/* ### MOD Saratoga-PEP (02/08/2000) */
#include    "mypifdefs.h"



/*==========================================================================*\
;+																			+
;+					L O C A L    D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- exclusive semaphore against API calls --- */

static	USHORT	usApiSem_GP;					/* semaphore handle			*/

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
;   format : VOID	PluInitialize(void);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID PluInitialize_GP(VOID)
{
	usApiSem_GP = PifCreateSem(1);					/* allocate a semaphore		*/
	ExeInitialize_GP(FALSE);						/* for first initialization for GPfiles	*/
	return;
}

/**
;========================================================================
;
;   function :	Finalize Mass Memory Module
;
;   format : VOID	PluFinalize(void);
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID PluFinalize_GP(VOID)
{
	PifDeleteSem(usApiSem_GP);						/* discard a semaphore		*/
	ExeFinalize_GP();
	return;
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

USHORT PluEnterCritMode_GP(USHORT usFile,
							   USHORT usFunc,
							   USHORT *pusHandle,
							   VOID   *pvParam)
{
	USHORT	usStat;
	PifRequestSem(usApiSem_GP);
	usStat = ExeEnterCritMode_GP(usFile, usFunc, pusHandle, pvParam);
	PifReleaseSem(usApiSem_GP);
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

USHORT PluExitCritMode_GP(USHORT usHandle, USHORT usReserve)
{
	USHORT	usStat;
	PifRequestSem(usApiSem_GP);
	usStat = ExeExitCritMode_GP(usHandle, usReserve);
	PifReleaseSem(usApiSem_GP);
	return (usStat);
}

/**
;========================================================================
;
;   function : Report a record sequentialy
;
;   format : USHORT		PluReportRecord(usHandle, pvBuffer, pusRet, usSemNo);
;
;   input  : USHORT		usHandle;		- handle value
;			 VOID		*pvBuffer;		- buffer ptr. to respond rec.
;			 USHORT		*pusRet;		- buffer ptr. to respond rec. length
;
;   output : USHORT		usStat;			- status
;
;========================================================================
**/

USHORT PluReportRecord_GP(USHORT usHandle, VOID *pvBuffer, USHORT *pusRet)
{
	USHORT	usStat;
	PifRequestSem(usApiSem_GP);
	usStat = ExeReportRecord_GP(usHandle, pvBuffer, NULL, pusRet);
	PifReleaseSem(usApiSem_GP);
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

USHORT PluSenseFile_GP(USHORT usFile, PMFINFO pFileInfo)
{
	USHORT	usStat;

	PifRequestSem(usApiSem_GP);
	usStat = ExeSenseFile_GP(usFile, pFileInfo);
	PifReleaseSem(usApiSem_GP);
	return (usStat);
}


/*==========================================================================*\
||							End of PLUMAIN.C								||
\*==========================================================================*/
