/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 7450 POS *             AT&T GIS, Japan, E&M OISO			||
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

/* --- global referrence header --- */
#include <windows.h>							/* Windows header			*/
#include <TCHAR.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

/* --- our common headers --- */
# include   <ecr.h>
#include	<pif.h>								/* PIF common header		*/
#include    "netsem.h"
#include	"piftypes.h"						/* type modifier header		*/
#include	"pifnet.h"							/* PifNet common header		*/

#define		NET_SYSTEM
#include	"netmodul.h"						/* common header			*/

/*==========================================================================*\
;+																			+
;+					L O C A L     D E F I N I T I O N s						+
;+																			+
\*==========================================================================*/

/* --- binary to ASCII conversion --- */

#define	BIN2ASC(x)	(UCHAR)(((x) & 0x0F) + ((((x) & 0x0F) > 9) ? ('A' - 10)	\
															   : '0'))
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


/*==========================================================================*\
;+																			+
;+				C O N S T A N T    D E F I N I T I O N s					+
;+																			+
\*==========================================================================*/

/* --- log function address --- */

static	PFUNLOG		pfnErrorLog = NULL;			/* ptr. to error log. func	*/

/*==========================================================================*\
;+																			+
;+					T H R E A D    H A N D L I N G							+
;+																			+
\*==========================================================================*/
VOID	debugOut(int lineNumber, DWORD dwEvent)
{
#ifdef DEBUG_PIFNET_OUTPUT
		TCHAR  wszDisplay[128];

		wsprintf(wszDisplay, TEXT("FILE: NETSYSTM.C  LINE: %i  SysEventWait WaitForSingleObject dwEvent:%x  Thread: %x\n"),
			lineNumber, dwEvent, GetCurrentThreadId ());
		OutputDebugString(wszDisplay);
#endif
}

VOID  debugDump (UCHAR *filename, int lineNumber, void *p, int nBytes)
{
#ifdef DEBUG_PIFNET_OUTPUT
	TCHAR  wszDisplay[1000];
	int iLen = 0;
	unsigned short *iP = (unsigned short *)p;

	wsprintf (wszDisplay, TEXT(" File %S Line number %i  ThreadId:  %x \n  iP = %8.8x   "), filename, lineNumber, GetCurrentThreadId (), iP);
	if (iP) {
		for (iLen = 1; iLen <= nBytes; iLen++, iP++) {
			wsprintf (wszDisplay + _tcslen (wszDisplay), TEXT("%4.4x "), *iP);
			if ( ! (iLen % 8)) {
				_tcscat (wszDisplay, TEXT("\n"));
				OutputDebugString (wszDisplay);
				wsprintf (wszDisplay, TEXT("  iP = %8.8x   "), iP);
			}
		}
	}
	_tcscat (wszDisplay, TEXT("\n"));
	OutputDebugString (wszDisplay);
#endif
}
/**
;========================================================================
;
;   function : Create a thread
;
;   format : ULONG		SysBeginThread(pEntry,
;									   pvStack,
;									   usSize,
;									   usPriority,
;									   pvArg);
;
;   input  : 
;
;   output : ULONG		ulThread;	- thread ID
;
;========================================================================
**/

ULONG	SysBeginThread(PFNTHREAD pEntry,
					   LPVOID    pvStack,
					   USHORT    usSize,
					   ULONG     ulPriority,
					   LPVOID    pvArg)
{
	LONG	lRet;
    DWORD   dwThreadId;
#ifdef DEBUG_PIFNET_OUTPUT
	TCHAR wszDisplay[128];
#endif
	/* --- create a thread --- */
	// replace use of CreateThread() with _beginthreadex() per recommendations
	// from various places including Microsoft since _beginthreadex()
	// will do C Run Time initialization for multi-threaded applications
	// while CreateThread() is a raw Windows API function that does not.
	// Supposedly the C Run Time will figure out that Thread Local Storage
	// is needed at some point and create it all.
	lRet = (LONG)_beginthreadex (NULL,
		0,
		(LPTHREAD_START_ROUTINE)pEntry,
		pvArg,
		0,
		&dwThreadId);

#ifdef DEBUG_PIFNET_OUTPUT
	wsprintf(wszDisplay, TEXT("NETSYSTM.C  LINE:%i SysBeginThread stack size %d. Thread:%x lRet:%i\n"), __LINE__, usSize, dwThreadId, lRet);
	OutputDebugString(wszDisplay);
#endif

	/* --- adjust thread's priority --- */
	if ((HANDLE)lRet != NULL) {							/* completed ?				*/
		SetThreadPriority((HANDLE)lRet, (INT)ulPriority);
    } else {
        lRet = -1L;
    }
	return ((ULONG)lRet);
}

/**
;========================================================================
;
;   function : Terminate a thread
;
;   format : VOID		SysEndThread();
;
;   input  : nothing
;
;   output : nothing
;
;========================================================================
**/

VOID	SysEndThread(VOID)
{
	/* --- terminate a thread --- */
#ifdef DEBUG_PIFNET_OUTPUT
    TCHAR  wszDisplay[128+1];
	wsprintf(wszDisplay, TEXT("FILE: %s  LINE: %n  SysEndThread thread ID %x\n"), __FILE__, __LINE__, GetCurrentThreadId());
	OutputDebugString(wszDisplay);
#endif
	_endthreadex(0);
}

/**
;========================================================================
;
;   function : Sleep for a while
;
;   format : VOID		SysSleep(ulMilliSec);
;
;   input  : ULONG		ulMilliSec;
;
;   output : nothing
;
;========================================================================
**/

VOID	SysSleep(ULONG ulMilliSec)
{
	Sleep(ulMilliSec);
}

/*==========================================================================*\
;+																			+
;+				S E M A P H O R E    H A N D L I N G						+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Create a sempahore
;
;   format : BOOL		SysSemCreate(pObj, fBlocked);
;
;   input  : PSEMOBJ	pObj;		- ptr. to an object
;			 BOOL		fBlocked;	- blocked state or free state
;
;   output : BOOL		fDone;		- done or error
;
;========================================================================
**/
BOOL	SysSemCreate(PSEMOBJ pObj, BOOL fBlocked)
{
	DWORD	dwLastError;
	LONG    lInit = 1;
	LONG    lMax = 1;

	if (fBlocked) lInit = 0;

	pObj->hSemaphoreHandle = CreateSemaphore(NULL, lInit, lMax, NULL);
	dwLastError = GetLastError();

	if (pObj->hSemaphoreHandle == NULL) {
		char  xBuff[128];
		sprintf (xBuff, " SysSemCreate(): hSemaphoreHandle 0x%p dwLastError = %d", pObj->hSemaphoreHandle, dwLastError);
		NHPOS_NONASSERT_NOTE("==TEST", xBuff);
	}
	return (TRUE);
}


/**
;========================================================================
;
;   function : Clear a semaphore
;
;   format : BOOL		SysSemClear(pObj);
;
;   input  : PSEMOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;	- done or error
;
;========================================================================
**/
BOOL	SysSemClear(PSEMOBJ pObj)
{
	DWORD	dwLastError;
	LONG    lPrevCount;
	BOOL    fReturn;

	fReturn = ReleaseSemaphore(pObj->hSemaphoreHandle, 1, &lPrevCount);
	dwLastError = GetLastError();

	if (!fReturn) {
		char  xBuff[128];
		sprintf (xBuff, " SysSemClear(): hSemaphoreHandle 0x%p fReturn = %d  dwLastError = %d, lPrevCount %d", pObj->hSemaphoreHandle, fReturn, dwLastError, lPrevCount);
		NHPOS_NONASSERT_NOTE("==TEST", xBuff);
	}
	return fReturn;
}

/**
;========================================================================
;
;   function : Request to a semaphore
;
;   format : BOOL	SysSemRequest(pObj, ulTime);
;
;   input  : PSEMOBJ	pObj;	- ptr. to an object
;			 ULONG		ulTime;	- max. time to wait
;
;   output : BOOL	fDone;	- done or error
;
;========================================================================
**/
BOOL	SysSemRequest(PSEMOBJ pObj, ULONG ulTime)
{
	DWORD	dwReturn, dwLastError;

	dwReturn = WaitForSingleObject(pObj->hSemaphoreHandle, INFINITE);
	dwLastError = GetLastError();
	if (dwReturn != 0) {
		char  xBuff[128];
		sprintf (xBuff, " SysSemRequest(): hSemaphoreHandle 0x%p dwReturn = %d, 0x%x  dwLastError = %d", pObj->hSemaphoreHandle, dwReturn, dwReturn, dwLastError);
		NHPOS_NONASSERT_NOTE("==TEST", xBuff);
	}
	return TRUE;
}

/**
;========================================================================
;
;   function : Close a semaphore
;
;   format : BOOL		SysSemClose(pObj);
;
;   input  : PSEMOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;	- done or error
;
;========================================================================
**/
BOOL	SysSemClose(PSEMOBJ pObj)
{
	DWORD	dwLastError;
	BOOL  fReturn = CloseHandle(pObj->hSemaphoreHandle);
	dwLastError = GetLastError();

	if (!fReturn) {
		char  xBuff[128];
		sprintf (xBuff, " SysSemClose(): hSemaphoreHandle 0x%p fReturn = %d  dwLastError = %d", pObj->hSemaphoreHandle, fReturn, dwLastError);
		NHPOS_NONASSERT_NOTE("==TEST", xBuff);
	}
	if (fReturn) pObj->hSemaphoreHandle = 0;

	return fReturn;

}

/*==========================================================================*\
;+																			+
;+				E V E N T    S I G N A L    H A N D L I N G					+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Create an event signal
;
;   format : BOOL		SysEventCreate(pObj, fBlocked);
;
;   input  : PEVTOBJ	pObj;		- ptr. to an object
;			 BOOL		fBlocked;	- blocked state or not
;
;   output : BOOL		fDone;		- done or error
;
;========================================================================
**/
BOOL	SysEventCreate(PEVTOBJ pObj, BOOL fBlocked)
{
	pObj->hEvent = CreateEvent(NULL, fBlocked ? FALSE : TRUE, FALSE, NULL);
	return ((BOOL)(pObj->hEvent ? TRUE : FALSE));
}

/**
;========================================================================
;
;   function : Set an event signal blocking state
;
;   format : BOOL	SysEventBlock(pObj);
;
;   input  : PEVTOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;		- done or error
;
;========================================================================
**/
BOOL	SysEventBlock(PEVTOBJ pObj)
{
	return (ResetEvent(pObj->hEvent));
}

/**
;========================================================================
;
;   function : Wait for a signal event
;
;   format : BOOL	SysEventWait(pObj);
;
;   input  : PEVTOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;	- done or error
;
;========================================================================
**/
BOOL	SysEventWait(PEVTOBJ pObj, ULONG ulTime)
{
	DWORD	dwEvent;
	int lineNumber;
	
	lineNumber = __LINE__;
	debugOut(lineNumber, 0);

	debugDump (__FILE__, __LINE__, ((char *)(&pObj)) - 4, 30);
	debugDump (__FILE__, __LINE__, pObj, sizeof(EVTOBJ));

	dwEvent = WaitForSingleObject(pObj->hEvent, ulTime);
	
	lineNumber = __LINE__;
	debugOut(lineNumber, dwEvent);

	debugDump (__FILE__, __LINE__, ((char *)(&pObj)) - 4, 20);  
	return ((dwEvent == WAIT_FAILED) ? FALSE : TRUE);
}

/**
;========================================================================
;
;   function : Signalize a signal event
;
;   format : BOOL	SysEventSignal(pObj);
;
;   input  : PEVTOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;	- done or error
;
;========================================================================
**/
BOOL	SysEventSignal(PEVTOBJ pObj)
{
	return (SetEvent(pObj->hEvent));
}

/**
;========================================================================
;
;   function : Close a signal event
;
;   format : BOOL	SysEventClose(pObj);
;
;   input  : PEVTOBJ	pObj;	- ptr. to an object
;
;   output : BOOL		fDone;	- done or error
;
;========================================================================
**/
BOOL	SysEventClose(PEVTOBJ pObj)
{
	return (CloseHandle(pObj->hEvent));
}

/*==========================================================================*\
;+																			+
;+					E R R O R    L O G    F U N C T I O N					+
;+																			+
\*==========================================================================*/

/**
;========================================================================
;
;   function : Set error log function address
;
;   format : VOID		SysSetErrorLog(pfnAddress);
;
;   input  : PFUNLOG	pfnAddress;		- ptr. to error log function
;
;   output : nothing
;
;========================================================================
**/

VOID	SysSetErrorLog(PFUNLOG pfnAddress)
{
	/* --- just save the function addres --- */
	pfnErrorLog = pfnAddress;					/* just save the address	*/
}

/**
;========================================================================
;
;   function : Log error
;
;   format : VOID		SysErrorLog(usModule, usCode, usSeverity,
;									fOption, sStatus, pszString);
;
;   input  :
;
;   output : nothing
;
;========================================================================
**/

VOID	SysErrorLog(USHORT usModule,
					USHORT usCode,
					USHORT usSeverity,
					BOOL   fOption,
					SHORT  sStatus,
					PSZ    pszString)
{
	/* --- pass a control to the error log function --- */
}

/*==========================================================================*\
;+						E n d   O f   P r o g r a m							+
\*==========================================================================*/
