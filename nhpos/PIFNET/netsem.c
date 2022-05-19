#if !defined(POSSIBLE_DEAD_CODE)
// This legacy semphore functionality has been replaced with using the Windows Semaphore API
// directly as part of correcting problems with user interface lagging seen with Windows 7 at US Customs.
// There were a number of issues and we have replaced this use of mutexes and other synchronization
// primitives with Windows Semphores as part of the changes to address the user interface lagging issue.
//    R. Chambers  Oct-16-2015

# include	<windows.h>							/* Windows header			*/
#include	<pif.h>								/* PIF common header		*/

VOID NetSemInitialize(VOID)
{
}

VOID NetDeleteSemAll(VOID)
{
}

#else
/*
	netsem.c - contains the semaphore routines used within the network layer
	of the PifNet subsystem.

	This is basically a copy of the routines in PifSem.c of the Pif subsystem.

	This functions are here because pifnet.dll is a dll and rather than linking
	in the Pif.dll, the semaphore routines were copied here and included in pifnet.dll
	as source code instead.

 */

#if defined(SARATOGA) 
# include	<windows.h>							/* Windows header			*/
#include	<pif.h>								/* PIF common header		*/
#elif defined(SARATOGA_PC)
# include	<windows.h>							/* Windows header			*/
# include   "pif.h"
#elif	defined (POS7450)						/* 7450 POS model ?				*/
# include	<phapi.h>							/* PharLap header			*/
#elif defined (POS2170)						/* 2170 POS model ?				*/
# define	NET_DATAGRAM_ACK
# include	<ecr.h>								/* ECR common header		*/
//include	<pif.h>								/* PIF common header		*/
#elif defined (SERVER_NT)					/* WindowsNT model ?			*/
# include	<windows.h>							/* Windows header			*/
#endif

#define PIF_MAX_SEM 32
#define PIF_ERROR_HANDLE_OVERFLOW   0xffff

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef struct {
	SHORT	sPifHandle;
	HANDLE	hMutexHandle;
	HANDLE	hEventHandle;
	SHORT	sCount;
} PIFSEMHANDLETABLE;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

static CRITICAL_SECTION	g_SemCriticalSection;

static PIFSEMHANDLETABLE	aPifSemTable[PIF_MAX_SEM];

SHORT NetSemSearchId(USHORT usHandle, PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
USHORT NetSemGetNewId(PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHOT PIFENTRY PifCreateSem(USHORT usCount)         **
**              usCount:    max number of semaphore                 **
**                                                                  **
**  return:     handle of semaphore                                 **
**                                                                  **
**  Description:Create semaphore                                    **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT NetCreateSem(USHORT usCount)
{
	SHORT	sSem;
	HANDLE	hMutexHandle;
	HANDLE	hEventHandle;
	DWORD	dwError;

	EnterCriticalSection(&g_SemCriticalSection);

    /*
		search for a free slot in the semaphore data table to use for
		this semaphore create request.  If a slot is not found, then
		inform the caller.

		Otherwise, create the resources needed for handling a semaphore
		and populate the table slot with the information.
	 */
    sSem = NetSemGetNewId(aPifSemTable, PIF_MAX_SEM);
    if (sSem == PIF_ERROR_HANDLE_OVERFLOW) {
		LeaveCriticalSection(&g_SemCriticalSection);
		return PIF_ERROR_SYSTEM;
	}

	// CreateMutex (LPSECURITY_ATTRIBUTES lpAttributes, BOOL bInitialOwner, LPCTSTR lpName)
	// Create an unnamed event object.  The event object is
	// initialized to auto-reset (becomes non-signaled when any
	// of the threads waiting on this event are released) and
	// the starting states is non-signaled.
	//
	// Use the SetEvent function to set the event object to signaled
	// and use the ResetEvent function to set the event object to non-signaled.
	// Since this is an auto-reset event object, we don't use ResetEvent.
	//
	// Threads waiting on an event using a Wait function, will be allowed
	// to continue if the event object state is signaled.  If the event object
	// state is non-signaled then any threads waiting on the event wait until
	// the event object goes to the signaled state.
	//
	// By using auto-reset, we ensure that only one waiting thread at a time
	// is released when some other thread issues a SetEvent call.
	hMutexHandle = CreateMutex(NULL, FALSE, NULL);
	if (hMutexHandle == NULL) {

		dwError = GetLastError();
		LeaveCriticalSection(&g_SemCriticalSection);
		return PIF_ERROR_SYSTEM;
	}

	// CreateEvent (LPSECURITY_ATTRIBUTES lpAttributes, BOOL bManualReset, BOOL bInitialState, LPTSTR lpName);
	// Create an unnamed event object.  The event object is
	// initialized to auto-reset (becomes non-signaled when any
	// of the threads waiting on this event are released) and
	// the starting states is non-signaled.
	//
	// Use the SetEvent function to set the event object to signaled
	// and use the ResetEvent function to set the event object to non-signaled.
	// Since this is an auto-reset event object, we don't use ResetEvent.
	//
	// Threads waiting on an event using a Wait function, will be allowed
	// to continue if the event object state is signaled.  If the event object
	// state is non-signaled then any threads waiting on the event wait until
	// the event object goes to the signaled state.
	//
	// By using auto-reset, we ensure that only one waiting thread at a time
	// is released when some other thread issues a SetEvent call.
	hEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEventHandle == NULL) {

		CloseHandle(hMutexHandle);
		dwError = GetLastError();
		LeaveCriticalSection(&g_SemCriticalSection);
		return PIF_ERROR_SYSTEM;
	}

	aPifSemTable[sSem].sPifHandle = sSem;
	aPifSemTable[sSem].hMutexHandle = hMutexHandle;
	aPifSemTable[sSem].hEventHandle = hEventHandle;
	aPifSemTable[sSem].sCount = usCount;

	LeaveCriticalSection(&g_SemCriticalSection);

	return sSem;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleteSem(USHORT usSem)            **
**              usSem:  semaphore handle                            **
**                                                                  **
**  Description:Delete semaphore                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   NetDeleteSem(USHORT usSem)
{
	HANDLE	hMutexHandle;
	HANDLE	hEventHandle;
	BOOL	fReturn;
	DWORD	dwError;

	EnterCriticalSection(&g_SemCriticalSection);

	if (NetSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM) != usSem) {

		LeaveCriticalSection(&g_SemCriticalSection);
		return;
	}
	hMutexHandle = aPifSemTable[usSem].hMutexHandle;
	hEventHandle = aPifSemTable[usSem].hEventHandle;

	while(aPifSemTable[usSem].sCount < 0) {

		SetEvent(hEventHandle);
		aPifSemTable[usSem].sCount++;
	}

	fReturn = CloseHandle(hEventHandle);
	fReturn = CloseHandle(hMutexHandle);
	if (fReturn == 0) {

		dwError = GetLastError();
	}

	aPifSemTable[usSem].sPifHandle = -1;

	LeaveCriticalSection(&g_SemCriticalSection);

}

/*fhfh 
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRequestSem(USHORT usSem)           **
**              usSem:  semaphore handle                            **
**                                                                  **
**  Description:Request semaphore                                   **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   NetRequestSem(USHORT usSem)
{
	HANDLE	hMutexHandle;
	HANDLE	hEventHandle;
	DWORD	dwReturn, dwError;

	// Get exclusive access to the semaphore table and look up the
	// specified semaphore handle in the table.  Once we find the
	// semaphore table entry for this semaphore handle, we will
	// then count down the semaphore value.
	// If the value is less than 0, this thread is blocked until one
	// or more of the current owners of the semaphore, release the
	// semaphore.

	EnterCriticalSection(&g_SemCriticalSection);

	if (NetSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM) != usSem) {

		LeaveCriticalSection(&g_SemCriticalSection);
		return;
	}
	hMutexHandle = aPifSemTable[usSem].hMutexHandle;
	hEventHandle = aPifSemTable[usSem].hEventHandle;

	// Notice one very important thing about this semaphore implementation.
	// If a thread requests this semaphore and then requests it again, if the
	// semaphore count goes negative, the thread will be blocked until some
	// other thread releases the semaphore.  This is a bit different from
	// Windows Critical Section semantics because a thread which uses the
	// EnterCriticalSection function to enter a critical section it already
	// owns, will succeed with the EnterCriticalSection and will not be blocked.
	aPifSemTable[usSem].sCount--;
	if (aPifSemTable[usSem].sCount < 0) {

		LeaveCriticalSection(&g_SemCriticalSection);

		/* wait for other task leave from PifRequestSem() */
		dwReturn = WaitForSingleObject(hMutexHandle, INFINITE);
		if (dwReturn == WAIT_FAILED) {

			dwError = GetLastError();
		}

		/* wait for other task releasing semaphore */
		dwReturn = WaitForSingleObject(hEventHandle, INFINITE);
		if (dwReturn == WAIT_FAILED) {

			dwError = GetLastError();
		}

		/* release mutex for other task */
		dwReturn = ReleaseMutex(hMutexHandle);
		if (dwReturn == 0) {

			dwError = GetLastError();
		}
		
	} else {

		LeaveCriticalSection(&g_SemCriticalSection);

	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifReleaseSem(USHORT usSem)           **
**              usSem:  semaphore handle                            **
**                                                                  **
**  Description:Release semaphore                                   **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   NetReleaseSem(USHORT usSem)
{

	HANDLE	hEventHandle;
	BOOL	fReturn;
	DWORD	dwError;

	EnterCriticalSection(&g_SemCriticalSection);

	if (NetSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM) != usSem) {

		LeaveCriticalSection(&g_SemCriticalSection);
		return;
	}
	hEventHandle = aPifSemTable[usSem].hEventHandle;

	if (aPifSemTable[usSem].sCount < 0) {

		fReturn = SetEvent(hEventHandle);
		if (fReturn == 0) {

			dwError = GetLastError();
		}
	}

	aPifSemTable[usSem].sCount++;

	LeaveCriticalSection(&g_SemCriticalSection);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifDeleteSemAll(VOID)                        **
**                                                                  **
**  Description:delete all semaphore at finalize                    **
**                                                                  **
**********************************************************************
fhfh*/
VOID NetDeleteSemAll(VOID)
{
	int i;

	for (i=0; i<PIF_MAX_SEM; i++) {

		if (aPifSemTable[i].sPifHandle != -1) {

			NetDeleteSem(aPifSemTable[i].sPifHandle);
		}
	}
    DeleteCriticalSection(&g_SemCriticalSection);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSemInititalize(VOID)                        **
**                                                                  **
**  Description:initialize semaphore buffer                         **
**                                                                  **
**********************************************************************
fhfh*/
VOID NetSemInitialize(VOID)
{
	USHORT i;

    InitializeCriticalSection(&g_SemCriticalSection);

    for (i=0; i<PIF_MAX_SEM; i++) {

		aPifSemTable[i].sPifHandle = -1;
		aPifSemTable[i].hMutexHandle = INVALID_HANDLE_VALUE;
		aPifSemTable[i].hEventHandle = INVALID_HANDLE_VALUE;
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSemGetNewId(PIFHANDLETABLE *pPifHandleTable,
**											USHORT usMaxNumber)      **
**              pPifHandleTable         PIF Handle Table Buffer     **
**				usMaxNumber				size of pPifHandleTable     **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:search empty handle value                           **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT NetSemGetNewId(PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
    USHORT i;

    for (i=0; i<usMaxNumber; i++) {
		if (pPifHandleTable->sPifHandle == -1) {
			return i;
		}
		pPifHandleTable++;
    }
    return(PIF_ERROR_HANDLE_OVERFLOW);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSsmSearchId(USHORT usHandle
**										PIFSEMHANDLETABLE *pPifHandleTable,
**											USHORT usMaxNumber)      **
**				usHandle				PIF Handle value
**              pPifHandleTable         PIF Handle Table Buffer     **
**				usMaxNumber				size of pPifHandleTable     **
**                                                                  **
**  return:     PIF handle value                                    **
**                                                                  **
**  Description:search handle value whether already opened or not   **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT NetSemSearchId(USHORT usHandle, PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	if (usHandle > usMaxNumber) {
		return -1;
	}

	if (pPifHandleTable[usHandle].sPifHandle != usHandle) {
		return -1;
	}

	return usHandle;
}

/* end of pifsem.c */

#endif