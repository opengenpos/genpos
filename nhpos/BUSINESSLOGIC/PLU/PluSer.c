#if defined(POSSIBLE_DEAD_CODE)
/*************************************************************************\
||																		||
||		*=*=*=*=*=*=*=*=*												||
||		*	NCR 2170	*			 NCR Corporation, E&M OISO			||
||	  @ *=*=*=*=*=*=*=*=*  0			 (C) Copyright, 1992			||
||	 <|\/~				~\/|>											||
||	_/^\_				 _/^\_										 	||
||																		||
\*************************************************************************/
/*========================================================================*\
*	Title			: Client/Server Reentrant module, Program List
*	Category		: Client/Server Reentrant module, NCR2170 US HOSPITALITY APPLICATION
*	Program Name	: PluSer.c											
*	------------------------------------------------------------------------
*	Compile			: VC++ Ver. 6.00 A by Microsoft Corp.				
*	Memory Model	:
*	Options			:
*	------------------------------------------------------------------------
*	Abstract		:The provided function names are as floolws.
*					SHORT	Rfl_SpBsearch		* bynary search *
*
*	------------------------------------------------------------------------
*	Update Histories:
*
*	Date			: Ver.Rev	: NAME			: Description
*	May-06-92		: 00.00.01	: M.YAMAMOTO	: Initial
*** 2172 Rel1.0
*	Nov-22-99		: 01.00.00	: K.Iwata		: Initial
\*=======================================================================*/
/*=======================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*-----------------------------------------------------------------------*/

#include <tchar.h>
#include	<ecr.h>
#include	<rfl.h>

/*
*==========================================================================
**	Synopsis:SHORT	Rfl_SpBsearch_PLU32(VOID *pKey, USHORT usRecord_Size, VOID *pTableBaseAddress, USHORT usNumberOfEntry,
*									VOID **pHitPoint,
*									SHORT (*psCompar)(VOID *pKey, VOID *pusHPoint))
*
*		Input:	VOID	*pKey
*				USHORT	usRecord_Size
*				VOID	*pTableBaseAddress
*				ULONG	ulNumberOfEntry	### MOD 2172 Rel1.0 (USHORT->ULONG)
*				SHORT	(*psCompar)(VOID *pKey, VOID *pusHPoint)
*		Output:	VOID	**pHitPoint
*		InOut:	Nothing
*
**	Return	:	RFL_HIT			
*				RFL_HEAD_OF_TABLE
*				RFL_DONT_HIT		
*				RFL_TAIL_OF_TABLE
**	Description:
*				Binary Search for PLU Module (32bit)
*==========================================================================
*/
SHORT	Rfl_SpBsearch_PLU32(VOID *pKey, USHORT usRecord_Size, VOID *pTableBaseAddress, 
	ULONG ulNumberOfEntry, VOID **pHitPoint, SHORT (*psCompar)(VOID *pKey, VOID *pusHPoint))
{

	UCHAR	*pchTableBaseAddressWork;
	SHORT	sStatus;
	UCHAR	*pchTableTopAddress;

	*pHitPoint = pchTableBaseAddressWork = (UCHAR *)pTableBaseAddress;

	sStatus = (*psCompar)(pKey, *pHitPoint);
	if ( sStatus < 0 ) {		/*	pKey < **pusHitPoint */
		return( RFL_HEAD_OF_TABLE );
	}
	if ( sStatus == 0 ) {
		return( RFL_HIT );
	}

	*pHitPoint = pchTableTopAddress = pchTableBaseAddressWork + (usRecord_Size * (ulNumberOfEntry - 1));

	sStatus = (*psCompar)(pKey, *pHitPoint);
	if ( sStatus > 0 ) {		/* pKey > **pusHitPoint */
		*pHitPoint = pchTableTopAddress + usRecord_Size;
		return( RFL_TAIL_OF_TABLE );
	}
	if ( sStatus == 0 ) {
		return( RFL_HIT );
	}
			
	while (pchTableTopAddress >= pchTableBaseAddressWork) {
		*pHitPoint = pchTableBaseAddressWork + usRecord_Size * (
						(pchTableTopAddress - pchTableBaseAddressWork) / ( 2 * usRecord_Size));
		sStatus = (*psCompar)(pKey, *pHitPoint);
		if (sStatus == 0) {
			return( RFL_HIT );
		}
		if (sStatus < 0) {
			pchTableTopAddress = *pHitPoint;
			pchTableTopAddress -= usRecord_Size;
		} else {
			pchTableBaseAddressWork = *pHitPoint;
			pchTableBaseAddressWork += usRecord_Size;
		}
	}
	if (sStatus > 0) {
		*pHitPoint = pchTableTopAddress + usRecord_Size;
	}
	return( RFL_DONT_HIT );
}
/* ------------------------------------------------------------------------- */
#endif