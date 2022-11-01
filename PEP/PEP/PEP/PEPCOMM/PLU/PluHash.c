/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 2172     *             NCR Corporation, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999				||
||    <|\/~               ~\/|>												||
||   _/^\_                 _/^\_											||
||																			||
\****************************************************************************/

/*==========================================================================*\
*	Title:			ComputeHash 64 (Saratoga & PEP)
*	Category:		PLU module
*	Program Name:	PluHash.c
* ---------------------------------------------------------------------------
*	Compiler:		VC++ Ver. 6.00 A by Microsoft Corp. (Saratoga)
*	(16bit)	:		VC++ Ver. 1.50 by Microsoft Corp. (PEP)
*	Memory Model:	
*	Options:		
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*  Nov/26/99 |			| 2172 initial							| K.Iwata
*  Feb/16/00 |			| PEP									| K.Iwata
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

#include	<memory.h>
#include	<stdlib.h>
#include	<ecr.h>								/* 2172 system header		*/
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/
#include	<TCHAR.H>


#ifdef	PEP
ULONG	ComputeHash_64(WCHAR *pNum, USHORT usLen,ULONG ulDiv){
	WCHAR	*	pStop;
	WCHAR		uchUpr[10],uchLw[10];
	ULONG		ulUp,ulLw,lHash = 0L;

	//memcpy(uchUpr,pNum,9); uchUpr[9] = '\0';
	wcsncpy(uchUpr, pNum, 9); uchUpr[9] = '\0';
	//memcpy(uchLw,&pNum[9],4); uchLw[4] = '\0';
	wcsncpy(uchLw,&pNum[9],5); uchLw[5] = '\0';
	ulUp = wcstoul((const WCHAR *)uchUpr,&pStop,10);
	ulLw = wcstoul((const WCHAR *)uchLw,&pStop,10);

	ulLw += (ULONG)((ULONG)(ulUp % ulDiv)*10000L);
	return	(ulLw % ulDiv);

	usLen = usLen;
}
#endif

/*==========================================================================*/

