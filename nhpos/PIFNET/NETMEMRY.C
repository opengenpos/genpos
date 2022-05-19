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

#include	<windows.h>							/* Windows header			*/
#include	<memory.h>						/* memxxx() C-library header	*/
#include	<malloc.h>						/* memory allocation header		*/

/* --- our common headers --- */
# include   "pif.h"
#include	"piftypes.h"					/* type modifier header			*/
#define		NET_MEMORY
#include	"netmodul.h"					/* our module header			*/

/*==========================================================================*\
;+																			+
;+					L O C A L     D E F I N I T I O N s						+
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
	return;
}

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
HMEM	SysCreateMemory(ULONG ulBytes)
{
    DWORD   dwBytes;
    HANDLE  hMemory;

    /* --- initialize --- */
    dwBytes = ulBytes ? (DWORD)ulBytes : 0x100000L;  /* adjust size          */

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

// ***   NOTE  ******
// If you want to enable the SysInfoMemory function to actually walk the
// heap, then you need to uncomment out the following #define.  For some
// reason, walking the heap is not enabled, presumably because it takes
// a lot of time and requires the heap to be locked for safety
// (see HeapLock() function in MSDN).  However if debugging network layer,
// you may want to see how the heap is being used.  An easy way to use this
// function is to insert lines of code to call this function and set a
// debugger break point to watch it.  Code will look something like the
// following and a good place to put it is in the NetManager () main loop
// just before the switch (pMessage->usCommand) statement.
//		{
//		    MEMINF      infMemory;
//			SysInfoMemory(infPacket.hMemory, &infMemory);    /* get heap info.       */
//		}

// #define ENABLE_SYSINFOMEMORY

BOOL	SysInfoMemory(HMEM hMemory, PMEMINF pInfo)
{
#if !defined(ENABLE_SYSINFOMEMORY)
    return TRUE;

    hMemory = hMemory;
    pInfo = pInfo;

#else
#pragma message("  **** WARNING:   SysInfoMemory () in netmemry.c is enabled to walk the heap!!")
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
/*==========================================================================*\
;+						E n d   O f   P r o g r a m							+
\*==========================================================================*/
