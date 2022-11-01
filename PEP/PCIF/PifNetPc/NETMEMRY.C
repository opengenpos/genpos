/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 7450 POS *             AT&T GIS Japan, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995				||
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
*	Memory Model:
*
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

#if defined(SARATOGA) 
# include	<windows.h>							/* Windows header			*/
# include   "r20_pif.h"
#elif defined(SARATOGA_PC)
# include	<windows.h>							/* Windows header			*/
//#  include  "pif.h"
# include   "r20_pif.h"
#elif	defined (POS7450)						/* 7450 POS model ?				*/
# include	<phapi.h>							/* PharLap header			*/
#elif defined (POS2170)						/* 2170 POS model ?				*/
# include	"ecr.h"								/* ecr common header		*/
#elif defined (SERVER_NT)					/* WindowsNT model ?			*/
# include	<windows.h>							/* Windows header			*/
#endif

/* --- C-library header --- */

#if !defined(SARATOGA) 
#include	<dos.h>							/* for FP_OFF(), FP_SEG()		*/
#endif
#include	<memory.h>						/* memxxx() C-library header	*/
#include	<malloc.h>						/* memory allocation header		*/

/* --- our common headers --- */

#include	"piftypes.h"					/* type modifier header			*/
#define		NET_MEMORY
#include	"netmodul.h"					/* our module header			*/

/*==========================================================================*\
;+																			+
;+					L O C A L     D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- memory manager control --- */

typedef struct _MEMSEG {					/* memory segment block			*/
	HMEM	hMemory;							/* memory handle			*/
	USHORT	usBytes;							/* no. of bytes allocated	*/
} MEMSEG, FAR *PMEMSEG;

typedef struct _MEMBLK {					/* memory block					*/
	USHORT	usBytes;							/* no. of bytes allocated	*/
	USHORT	fUsed;								/* used / free block		*/
/*	BOOL	fUsed;								/ used / free block		*/
} MEMBLK, FAR *PMEMBLK;

/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/


/*==========================================================================*\
;+																			+
;+					S T A T I C   V A R I A B L E s							+
;+																			+
\*==========================================================================*/

/* --- memory resources --- */
#if defined (MEM_INTERNAL_LOGIC)
static	USHORT FAR fsMemUsers = 0;					/* memory users			*/
static	UCHAR  FAR aucMemResource[MEM_TOTAL_BYTES];	/* memory resources		*/
#endif
/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/


/*==========================================================================*\
;+																			+
;+						D e b u g g i n g									+
;+																			+
\*==========================================================================*/

#if	0

PVOID	pvPtr01 = NULL;
PVOID	pvPtr02 = NULL;
PVOID	pvPtr03 = NULL;
PVOID	pvPtr04 = NULL;
PVOID	pvPtr05 = NULL;
PVOID	pvPtr06 = NULL;
PVOID	pvPtr07 = NULL;
PVOID	pvPtr08 = NULL;
PVOID	pvPtr09 = NULL;
PVOID	pvPtr10 = NULL;

typedef struct _MEMINF {
	USHORT		usTest;
	PVOID 		*ppvPtr;
	USHORT		usBytes;
} MEMINF, *PMEMINF;


VOID	main() {

	USHORT	usBytes;
	HMEM	hMemory;
	PVOID * ppvPtr;
	PMEMINF	pTest;

	static	MEMINF	LoopTest[] = { { 1,	&pvPtr01,	100	},
								   { 1,	&pvPtr02,	199	},
								   { 1,	&pvPtr03,	300	},
								   { 1,	&pvPtr04,	399	},
								   { 2,	&pvPtr01,	0	},
								   { 2,	&pvPtr03,	0	},
								   { 2,	&pvPtr02,	0	},
								   { 2,	&pvPtr04,	0	},
								   { 0, NULL,		0	},
								 };

	if (! (hMemory = SysCreateMemory(0))) {
		printf("SysCreateMemory() error.\n");
		exit (1);
	}

	for (pTest = LoopTest; pTest->usTest; pTest++) {

		ppvPtr  = (pTest->ppvPtr);
		usBytes = pTest->usBytes;

		switch (pTest->usTest) {
		case 1:
			if (! (*ppvPtr = SysAllocMemory(hMemory, usBytes))) {
				printf("SysAllocMemory() error.\n");
			} else {
				memset(*ppvPtr, (pTest - LoopTest) + 1, usBytes);
			}
			break;
		default:
			SysFreeMemory(hMemory, *ppvPtr);
			break;
		}
	}

	SysDeleteMemory(hMemory);
}
#endif

/*==========================================================================*\
;+																			+
;+						I n i t i a l i z a t i o n							+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Initialize memory manager
;
;   format : VOID	SysInitializeMemory();
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID	SysInitializeMemory(VOID)
{
	/* --- initialize memory resources --- */
#if	defined (MEM_INTERNAL_LOGIC)
	fsMemUsers = 0;
	_fmemset(aucMemResource, '\0', sizeof(aucMemResource));
#endif
	/* --- exit ... --- */

	return;
}

/*==========================================================================*\
;+																			+
;+		M E M O R Y   M A N A G E R   B Y   I N T E R N A L   L O G I C		+
;+																			+
\*==========================================================================*/
#if defined (MEM_INTERNAL_LOGIC)
/**
;========================================================================
;
;   function : Create a Memory Pool
;
;   format : HMEM		SysCreateMemory(usBytes);
;
;   input  : USHORT		usBytes;		- no. of bytes
;
;   output : HMEM		hMemory;		- handle to access the memory
;
;========================================================================
**/

HMEM	SysCreateMemory(USHORT usBytes)
{
	USHORT		usLast, usAlloc, fsHandle;
	PMEMSEG		pSeg, pBest, pTry;
	PMEMBLK		pBlock;

	/* --- declare tricky memory segment for easy computing --- */

	static	MEMSEG	NullSeg = { 0, 0 };			/* null memory segment		*/

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/
	pBest  = &NullSeg;							/* init. segment block ptr.	*/

	/* --- is valid parameters ? --- */

	if (usBytes && (usBytes < sizeof(MEMSEG) + sizeof(MEMBLK))) {
		return ((HMEM)0);							/* no memory available	*/
	}

	/* --- is too many users ? --- */

	if (! ~fsMemUsers) {						/* too many users ?			*/
		return ((HMEM)0);							/* no memory available	*/
	}

	/* --- is first time call ? --- */

	if (! fsMemUsers) {							/* no user at present ?		*/
		pSeg->hMemory = 0;							/* init. memory handle	*/
		pSeg->usBytes = sizeof(aucMemResource);		/* no. of bytes avail	*/
	}

	/* --- normalize required bytes --- */

	usAlloc = usBytes + (usBytes & 0x0001);		/* for word alignment		*/

	/* --- look for best fit memory segment --- */

	while (usLast) {							/* look for a segment block	*/

		/* --- is free segment block ? --- */

		if (! pSeg->hMemory) {					/* nobody used ?			*/

			/* --- is fit in this segment ? --- */

			if (! usAlloc) {					/* max. available size ?	*/
				pBest = (pBest->usBytes > pSeg->usBytes) ? pBest : pSeg;
			} else {							/* some specific bytes ...	*/
				pTry  = (pSeg->usBytes >= usAlloc)       ? pSeg  : pBest;
				pBest = (pBest->usBytes && (pBest->usBytes < pTry->usBytes))
														 ? pBest : pTry;
			}
		}

		/* --- try to examine next segment --- */

		usLast       -= pSeg->usBytes;			/* compute remained byte	*/
		(PUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment		*/
	}

	/* --- can be found ? --- */

	if (pBest->usBytes) {						/* any bytes allocated ?	*/

		/* --- how many bytes allocate for it ? --- */

		usLast  = pBest->usBytes;					/* bytes in this segment*/
		usAlloc = usAlloc ? usAlloc : usLast;		/* normalize bytes		*/

		/* --- should be divided ? --- */

		if (usLast < usAlloc + 16) {				/* min 16 bytes required*/
			usAlloc = usLast;							/* give all memory	*/
		} else {									/* else, dividd 2 pieces*/
			pSeg          = (PMEMSEG)((LPUCHAR)pBest + usAlloc);
			pSeg->hMemory = 0;
			pSeg->usBytes = (usLast - usAlloc);
		}

		/* --- initialize the 1st block --- */

		pBlock          = (PMEMBLK)(pBest + 1);	/* ptr. to 1st block		*/
		pBlock->fUsed   = FALSE;				/* mark as free block		*/
		pBlock->usBytes = usAlloc - sizeof(MEMSEG);	/* no. of bytes free	*/

		/* --- compute user handle --- */

		for (fsHandle = 0x0001; fsMemUsers & fsHandle; fsHandle = (fsHandle << 1)) ;

		/* --- block the user --- */

		fsMemUsers    |= fsHandle;				/* block for the user		*/
		pBest->hMemory = fsHandle;				/* save user handle			*/
		pBest->usBytes = usAlloc;				/* no. of bytes allocted	*/
	}

	/* --- exit ... --- */

	return (pBest->hMemory);
}

/**
;========================================================================
;
;   function : Delete Memory Pool
;
;   format : VOID		SysDeleteMemory(hMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;
;   output : nothing
;
;========================================================================
**/

VOID	SysDeleteMemory(HMEM hMemory)
{
	USHORT		usLast;
	PMEMSEG		pSeg, pPrev, pNext;

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/
	pPrev  = _NULL;								/* init. previous segment	*/
	pNext  = _NULL;								/* init. next segment		*/

	/* --- look for the user segment --- */

	while (usLast) {							/* look for a segment block	*/

		/* --- is the same segment block ? --- */

		if (pSeg->hMemory != hMemory) {			/* is user segment ?		*/
			pPrev          = pSeg;					/* remember this segment*/
			usLast        -= pSeg->usBytes;			/* compute remained byte*/
			(LPUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment	*/
			continue;								/* try again			*/
		}

		/* --- discard this segment --- */

		fsMemUsers &= ~(pSeg->hMemory);			/* release from the user	*/

		/* --- merge with previous segment ? --- */

		if (pPrev && (! pPrev->hMemory)) {		/* tie up with the previous?*/
			pPrev->usBytes += pSeg->usBytes;		/* merge the segments	*/
			pSeg            = pPrev;				/* adjust segment ptr.	*/
		}

		/* --- compute next memory block --- */

		pNext = (PMEMSEG)((LPUCHAR)pSeg + pSeg->usBytes);
		pNext = (pSeg->usBytes < sizeof(aucMemResource)) ? pNext : _NULL;

		/* --- merge with next segment ? --- */

		if (pNext && (! pNext->hMemory)) {		/* tie up with the next ?	*/
			pSeg->usBytes += pNext->usBytes;		/* merge the segments	*/
		}

		/* --- break my job here --- */

		pSeg->hMemory = 0;						/* nobody use the segment	*/
		break;									/* no need to do my job !	*/
	}

	/* --- exit ... --- */

	return ;
}

/**
;========================================================================
;
;   function : Allocate A Memory Block
;
;   format : LPVOID		SysAllocMemory(hMemory, usBytes);
;
;   input  : HMEM		hMemory;		- memory handle
;			 USHORT		usBytes;		- no. of byte to be allocated
;
;   output : LPVOID		lpvMemory;		- ptr. to the memory
;
;========================================================================
**/

LPVOID	SysAllocMemory(HMEM hMemory, USHORT usBytes)
{
	USHORT		usLast, usAlloc;
	PMEMSEG		pSeg;
	PMEMBLK		pBlock, pUser;

	/* --- declare tricky memory block for easy computing --- */

	static	MEMBLK	NullBlock = { FALSE, 0 };	/* null memory block		*/

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/
	pUser  = &NullBlock;						/* init. user memory block	*/

	/* --- look for the user segment --- */

	while (usLast) {							/* look for a segment block	*/

		/* --- is the same segment block ? --- */

		if (pSeg->hMemory != hMemory) {			/* is user segment ?		*/
			usLast        -= pSeg->usBytes;			/* compute remained byte*/
			(LPUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment	*/
			continue;								/* try again			*/
		}

		/* --- initialize before allocating --- */

		usLast = pSeg->usBytes - sizeof(MEMBLK);/* segment size in byte		*/
		pBlock = (PMEMBLK)(pSeg + 1);			/* ptr. to 1st block		*/
		pUser  = &NullBlock;					/* init. user memory block	*/

		/* --- compute no. of bytes required --- */

		usAlloc  = usBytes + (usBytes & 1);		/* for word alignment		*/
		usAlloc += sizeof(MEMBLK);				/* required block control	*/

		/* --- look for the suitable block --- */

		while (usLast) {						/* look for suitable block	*/

			/* --- is free memory block & satisfied ? --- */

			if ((! pBlock->fUsed) && (pBlock->usBytes >= usAlloc)) {
				pUser = (pUser->usBytes && (pUser->usBytes < pBlock->usBytes))
					  ? pUser : pBlock;
			}

			/* --- try to examine next segment --- */

			usLast          -= pBlock->usBytes;		/* compute remained byte*/
			(LPUCHAR)pBlock += pBlock->usBytes;		/* skip to next segment	*/
		}

		/* --- can be found ? --- */

		if (pUser->usBytes) {					/* any bytes allocated ?	*/

			/* --- how many bytes allocate for it ? --- */

			usLast = pUser->usBytes;				/* bytes in this segment*/

			/* --- should be divided ? --- */

			if (usLast < usAlloc + sizeof(MEMBLK) + sizeof(USHORT)) {
				usAlloc = usLast;						/* give all memory	*/
			} else {								/* else, divide 2 pieces*/
				pBlock          = (PMEMBLK)((LPUCHAR)pUser + usAlloc);
				pBlock->fUsed   = FALSE;
				pBlock->usBytes = (usLast - usAlloc);
			}

			/* --- block the user --- */

			pUser->fUsed   = TRUE;					/* used this block		*/
			pUser->usBytes = usAlloc;				/* no. of bytes allocted*/
		}

		/* --- break my job --- */

		break;
	}

	/* --- exit ... --- */

	return ((LPVOID)(pUser->usBytes ? (LPVOID)(pUser + 1) : _NULL));
}

/**
;========================================================================
;
;   function : Release Memory Block
;
;   format : VOID		SysFreeMemory(lpvMemory);
;
;   input  : LPVOID		lpvMemory;		- ptr. to memory
;
;   output : nothing
;
;========================================================================
**/

VOID	SysFreeMemory(HMEM hMemory, LPVOID lpvMemory)
{
	USHORT		usLast;
	PMEMSEG		pSeg;
	PMEMBLK		pBlock, pTest, pPrev, pUser, pNext;

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/

	/* --- look for the user segment --- */

	while (usLast) {							/* look for a segment block	*/

		/* --- is the same segment block ? --- */

		if (pSeg->hMemory != hMemory) {			/* is user segment ?		*/
			(LPUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment	*/
			usLast        -= pSeg->usBytes;			/* compute remained byte*/
			continue;								/* try again			*/
		}

		/* --- compute memory block ptr. for the memory --- */

		usLast = pSeg->usBytes - sizeof(MEMBLK);/* total memosy size in seg	*/
		pBlock = (PMEMBLK)(pSeg + 1);			/* ptr. to the 1st block	*/
		pUser  = (PMEMBLK)((LPUCHAR)lpvMemory - sizeof(MEMBLK));

		/* --- compute next memory block --- */

		pTest = (PMEMBLK)((LPUCHAR)pBlock + usLast);	/* end of block ptr.*/
		pNext = (PMEMBLK)((LPUCHAR)pUser  + pUser->usBytes);
		pNext = (PMEMBLK)((pNext < pTest) ? pNext : _NULL);

		/* --- init. before looking the previous --- */

		pPrev  = pBlock;						/* init. previous block		*/
		pPrev  = (pBlock == pUser) ? _NULL : pPrev;
		usLast = (pBlock == pUser) ? 0     : usLast;

		/* --- look for the previous memory block --- */

		while (usLast) {						/* look for the prev. block	*/

			/* --- compute next block ptr. --- */

			pTest = (PMEMBLK)((LPUCHAR)pPrev + pPrev->usBytes);

			/* --- not come to the user block ? --- */

			if (pTest < pUser) {				/* not come to user block ?	*/
				usLast         -= pPrev->usBytes;	/* remained bytes		*/
				(LPUCHAR)pPrev += pPrev->usBytes;	/* next memory block	*/
				continue;							/* try to search		*/
			}

			/* --- else, over the user memory ? --- */

			else if (pTest > pUser) {			/* over than user block ?	*/
				pPrev = _NULL;						/* cannot found previous*/
				pNext = _NULL;						/* no need to do job	*/
				break;								/* something wrong !	*/
			}

			/* --- else, the just previous block --- */

			pUser->fUsed = FALSE;				/* nobody use the block		*/
			break;
		}

		/* --- merge with the previous block ? --- */

		if (pPrev && (! pPrev->fUsed)) {		/* merge with the previous ?*/
			pPrev->usBytes += pUser->usBytes;		/* merge the blocks		*/
			pUser           = pPrev;				/* adjust block ptr.	*/
		}

		/* --- merge with the next block ? --- */

		if (pNext && (! pNext->fUsed)) {		/* merge with the next ?	*/
			pUser->usBytes += pNext->usBytes;		/* merge the blocks		*/
		}

		/* --- break my job here --- */

		break;									/* no need to do my job !	*/
	}

	/* --- exit ... --- */

	return ;
}

/**
;========================================================================
;
;   function : Check memory consistency
;
;   format : BOOL		SysCheckMemory(hMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysCheckMemory(HMEM hMemory)
{
	BOOL		fValid;
	USHORT		usLast;
	PMEMSEG		pSeg;
	PMEMBLK		pBlock;

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/
	fValid = TRUE;								/* assume valid integrity	*/

	/* --- look for the user segment --- */

	while (fValid && usLast) {					/* look for a segment block	*/

		/* --- is the same segment block ? --- */

		if (pSeg->hMemory != hMemory) {			/* is user segment ?		*/
			fValid         = (usLast >= pSeg->usBytes) ? TRUE : FALSE;
			usLast        -= pSeg->usBytes;			/* compute remained byte*/
			(LPUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment	*/
			continue;								/* try again			*/
		}

		/* --- initialize before checking --- */

		usLast = pSeg->usBytes - sizeof(MEMSEG);/* memory segment size		*/
		pBlock = (PMEMBLK)(pSeg + 1);			/* ptr. to top of block		*/

		/* --- check memory consistency in the segment block --- */

		while (fValid && usLast) {				/* check its consistency	*/
			fValid           = (pBlock->usBytes <= usLast) ? TRUE   : FALSE;
			fValid           = (pBlock->usBytes)           ? fValid : FALSE;
			usLast          -= pBlock->usBytes;		/* compute remained byte*/
			(LPUCHAR)pBlock += pBlock->usBytes;		/* skip to next segment	*/
		}
	}

	/* --- exit ... --- */

	return (fValid);
}

/**
;========================================================================
;
;   function : Get memory info.
;
;   format : BOOL		SysInfoMemory(hMemory, pInfo);
;
;   input  : HMEM		hMemory;		- memory handle
;			 PMEMINF	pInfo;			- ptr. to respond info.
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysInfoMemory(HMEM hMemory, PMEMINF pInfo)
{
	BOOL		fValid;
	USHORT		usLast, usBytes;
	PMEMSEG		pSeg;
	PMEMBLK		pBlock;

	/* --- initialize --- */

	usLast = sizeof(aucMemResource);			/* total memory size		*/
	pSeg   = (PMEMSEG)aucMemResource;			/* ptr. to top of memory	*/
	fValid = TRUE;								/* assume valid integrity	*/

	/* --- initialize info. packet --- */

	memset(pInfo, '\0', sizeof(MEMINF));		/* clear info. packet		*/

	/* --- look for the user segment --- */

	while (fValid && usLast) {					/* look for a segment block	*/

		/* --- is the same segment block ? --- */

		if (pSeg->hMemory != hMemory) {			/* is user segment ?		*/
			fValid         = (usLast >= pSeg->usBytes) ? TRUE : FALSE;
			usLast        -= pSeg->usBytes;			/* compute remained byte*/
			(LPUCHAR)pSeg += pSeg->usBytes;			/* skip to next segment	*/
			continue;								/* try again			*/
		}

		/* --- initialize before checking --- */

		usLast = pSeg->usBytes - sizeof(MEMSEG);/* memory segment size		*/
		pBlock = (PMEMBLK)(pSeg + 1);			/* ptr. to top of block		*/

		/* --- parse memory statistics --- */

		while (fValid && usLast) {				/* check its consistency	*/

			/* --- analyze the block's statistics --- */

			usBytes              = pBlock->usBytes - sizeof(MEMBLK);
			pInfo->ulUsedMemory += (ULONG)(pBlock->fUsed ? usBytes : 0);
			pInfo->ulFreeMemory += (ULONG)(pBlock->fUsed ? 0       : usBytes);
			pInfo->ulMaxAlloc    = MAXOF(pInfo->ulMaxAlloc, 
										 (ULONG)(pBlock->fUsed ? 0 : usBytes));

			/* --- go to next memory block --- */

			fValid           = (pBlock->usBytes <= usLast) ? TRUE   : FALSE;
			fValid           = (pBlock->usBytes)           ? fValid : FALSE;
			usLast          -= pBlock->usBytes;		/* compute remained byte*/
			(LPUCHAR)pBlock += pBlock->usBytes;		/* skip to next segment	*/
		}
	}

	/* --- exit ... --- */

	return (fValid);
}
#endif

/*==========================================================================*\
;+																			+
;+		M E M O R Y   M A N A G E R   B Y   H E A P   A L G O R I T H M		+
;+																			+
\*==========================================================================*/
#if defined	(MEM_HEAP_BY_BMALLOC)
/**
;========================================================================
;
;   function : Create a Memory Pool
;
;   format : HMEM		SysCreateMemory(usBytes);
;
;   input  : USHORT		usBytes;		- no. of bytes
;
;   output : HMEM		hMemory;		- handle to access the memory
;
;========================================================================
**/

HMEM	SysCreateMemory(USHORT usBytes)
{
	/* --- create a new segment for the memory --- */

	return (_bheapseg(usBytes));
}

/**
;========================================================================
;
;   function : Delete Memory Pool
;
;   format : VOID		SysDeleteMemory(hMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;
;   output : nothing
;
;========================================================================
**/

VOID	SysDeleteMemory(HMEM hMemory)
{
	/* --- just discard it --- */

	_bfreeseg(hMemory);
}

/**
;========================================================================
;
;   function : Allocate A Memory Block
;
;   format : LPVOID		SysAllocMemory(hMemory, usBytes);
;
;   input  : HMEM		hMemory;		- memory handle
;			 USHORT		usBytes;		- no. of byte to be allocated
;
;   output : LPVOID		lpvMemory;		- ptr. to the memory
;
;========================================================================
**/

LPVOID	SysAllocMemory(HMEM hMemory, USHORT usBytes)
{
	VOID _based(hMemory) * lpvMemory;

	/* --- allocate a memory block --- */

	if ((lpvMemory = _bmalloc(hMemory, usBytes)) == _NULLOFF){
		return (_NULL);
	}

	/* --- exit ... --- */

	return (lpvMemory);
}

/**
;========================================================================
;
;   function : Release Memory Block
;
;   format : VOID		SysFreeMemory(hMemory, lpvMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;			 LPVOID		lpvMemory;		- ptr. to memory
;
;   output : nothing
;
;========================================================================
**/

VOID	SysFreeMemory(HMEM hMemory, LPVOID lpvMemory)
{
	/* --- just free it --- */

	_bfree(FP_SEG(lpvMemory), FP_OFF(lpvMemory));
}

/**
;========================================================================
;
;   function : Check memory consistency
;
;   format : BOOL		SysCheckMemory(hMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysCheckMemory(HMEM hMemory)
{
	/* --- just check it --- */

	return ((_bheapchk(hMemory) == _HEAPOK) ? TRUE : FALSE);
}

/**
;========================================================================
;
;   function : Get memory info.
;
;   format : BOOL		SysInfoMemory(hMemory, pInfo);
;
;   input  : HMEM		hMemory;		- memory handle
;			 PMEMINF	pInfo;			- ptr. to respond info.
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysInfoMemory(HMEM hMemory, PMEMINF pInfo)
{
	BOOL				fValid, fContinue;
    struct _heapinfo	infHeap;

	/* --- initialize --- */

	infHeap._pentry = NULL;						/* for first call			*/
	fValid          = TRUE;						/* assume valid segment		*/
	fContinue       = TRUE;						/* continue to get info.	*/

	/* --- clear packet data --- */

	memset(pInfo, '\0', sizeof(MEMINF));		/* clear info. packet		*/

	/* --- get info. --- */

	while (fValid && fContinue) {				/* get its statistics		*/

		/* --- walk every heap --- */

		switch (_bheapwalk(hMemory, &infHeap))	/* walk one heap			*/
		case _HEAPOK:
			if (infHeap._useflag == _USEDENTRY) {
				pInfo->ulUsedMemory += (ULONG)(infHeap._size);
				pInfo->ulMaxAlloc    = MAXOF(pInfo->ulMaxAlloc, 
											(ULONG)(infHeap._size));
			} else {
				pInfo->ulFreeMemory  = (ULONG)(infHeap._size);
			}
			break;
		case _HEAPEND:
			fContinue = FALSE:
			break;
		default:
			break;
		}
	}

	/* --- exit ... --- */

	return (fValid);
}
#endif

/*==========================================================================*\
;+                                                                          +
;+			M E M O R Y   M A N A G E R   B Y   W I N 3 2   H E A P			+
;+                                                                          +
\*==========================================================================*/
#if defined	(MEM_HEAP_BY_WIN32)
/**
;========================================================================
;
;   function : Create a memory pool
;
;   format : HMEM       SysCreateMemory(usBytes);
;
;   input  : USHORT     usBytes;        - no. of bytes
;
;   output : USHORT     usRet;
;
;========================================================================
**/

HMEM	SysCreateMemory(USHORT usBytes)
{
    DWORD   dwBytes;
    HANDLE  hMemory;

    /* --- initialize --- */

    dwBytes = usBytes ? (DWORD)usBytes : 0x10000L;  /* adjust size          */

    /* --- allocate memory by the size --- */

    hMemory = HeapCreate(0, dwBytes, dwBytes);		/* allocate a heap		*/

    /* --- exit ... --- */

    return ((HMEM)hMemory);
}

/**
;========================================================================
;
;   function : Delete a memory pool
;
;   format : VOID       SysDeleteMemory(hMemory);
;
;   input  : HMEM       hMemory;            - memory handle
;
;   output : nothing
;
;========================================================================
**/

VOID	SysDeleteMemory(HMEM hMemory)
{
	/* --- destory the memory --- */

    HeapDestroy(hMemory);							/* destroy the memory   */
}

/**
;========================================================================
;
;   function : Allocate A Memory Block
;
;   format : LPVOID		SysAllocMemory(hMemory, usBytes);
;
;   input  : HMEM		hMemory;		- memory handle
;			 USHORT		usBytes;		- no. of byte to be allocated
;
;   output : LPVOID		lpvMemory;		- ptr. to the memory
;
;========================================================================
**/

LPVOID	SysAllocMemory(HMEM hMemory, USHORT usBytes)
{
	LPVOID	pvPtr;

	/* --- allocate memory by the size --- */

    pvPtr = HeapAlloc(hMemory, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, usBytes);

	/* --- exit ... --- */

    return (pvPtr);
}

/**
;========================================================================
;
;   function : Release Memory Block
;
;   format : VOID		SysFreeMemory(hMemory, lpvMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;			 LPVOID		lpvMemory;		- ptr. to memory
;
;   output : nothing
;
;========================================================================
**/

VOID	SysFreeMemory(HMEM hMemory, LPVOID lpvBlock)
{
    /* --- free the memory --- */

	HeapFree(hMemory, HEAP_NO_SERIALIZE, lpvBlock);
}

/**
;========================================================================
;
;   function : Check memory consistency
;
;   format : BOOL		SysCheckMemory(hMemory);
;
;   input  : HMEM		hMemory;		- memory handle
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysCheckMemory(HMEM hMemory)
{
	/* --- just check it --- */

	return (HeapValidate(hMemory, 0, NULL));
}

/**
;========================================================================
;
;   function : Get memory info.
;
;   format : BOOL		SysInfoMemory(hMemory, pInfo);
;
;   input  : HMEM		hMemory;		- memory handle
;			 PMEMINF	pInfo;			- ptr. to respond info.
;
;   output : BOOL		fValid;			- valid or invalid
;
;========================================================================
**/

BOOL	SysInfoMemory(HMEM hMemory, PMEMINF pInfo)
{
#if defined(SARATOGA) 
    return TRUE;

    hMemory = hMemory;
    pInfo = pInfo;

#else
	BOOL				fValid;
	PROCESS_HEAP_ENTRY	infHeap;

	/* --- initialize --- */

	infHeap.lpData = NULL;						/* for first call			*/
	fValid         = TRUE;						/* assume valid segment		*/

	/* --- clear packet data --- */

	memset(pInfo, '\0', sizeof(MEMINF));		/* clear info. packet		*/

	/* --- get info. --- */

	while (HeapWalk(hMemory, &infHeap)) {	/* get its statistics		*/

		/* --- how's it going ? --- */

		if (infHeap.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
			pInfo->ulUsedMemory += (ULONG)(infHeap.cbData);
		} else {
			pInfo->ulFreeMemory += (ULONG)(infHeap.cbData);
			pInfo->ulMaxAlloc    = MAXOF(pInfo->ulMaxAlloc, 
										 (ULONG)(infHeap.cbData));
		}
	}

	/* --- completed ? --- */

	fValid = (GetLastError() == ERROR_NO_MORE_ITEMS) ? TRUE : FALSE;

	/* --- exit ... --- */

	return (fValid);
#endif
}
#endif
/*==========================================================================*\
;+						E n d   O f   P r o g r a m							+
\*==========================================================================*/
