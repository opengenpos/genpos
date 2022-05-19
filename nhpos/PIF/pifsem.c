/* pifsem.c : PIF semaphore control function under Windows CE */

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"


#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

#undef PifRequestSem
#undef PifReleaseSem

// The following variables are used to issue a PifLog () if the count associated
// with a semaphore is not within the expected range for the semaphore.
// Highwater mark is the maximum expected count of threads waiting on the
// semaphore to become available.  Highwater mark is a negative number
// because a semaphore count goes negative if threads requesting the semaphore
// have to wait until the semaphore becomes available.
// Lowwater mark is the maximum expected value of the semaphore count if
// no threads have requested the semaphore.  Lowwater mark is a positive
// non-zero number indicated the maximum number of PifReleaseSem () calls
// that are expected.

// ported changes made in Rel 2.2.0 to use Windows Semaphore API to
// Relk 2.2.1 in order to address the same issues seen at VCS with
// Rel 2.2.0 in which it appears that some Electronic Journal and
// total update issues originate from semaphore behavior that result
// in semaphores as previously implemented not being reliable.
//    Richard Chambers, Aug-21-2014
#define  PIF_SEM_STATE_FLAGS_REQ_REL     0x00000001      // indicates if last action was a Request (set) or Release (unset or 0)
#define  PIF_SEM_STATE_FLAGS_INIT_ZERO   0x00000002      // indicates if when created a zero initial value was set.
#define  PIF_SEM_STATE_FLAGS_BINARY      0x00000004      // indicates if when created a max count of 1 indicating binary semaphore.

#define  PIF_SEM_STATE_FLAGS_LOG_ON_REL   0x00000100      // indicates log on release.  indicates last request blocked.

typedef struct {
	HANDLE  hSemaphoreHandle;   // actual Windows OS Semaphore handle using Windows API
	HANDLE	hMutexHandle;
	HANDLE	hEventHandle;
    DWORD   dwThreadId;         // id of thread that created the semaphore
    DWORD   dwCurrentOwnerId;   // id of thread that has the semaphore
    DWORD   dwLastOwnerId;      // id of thread that last ad the semaphore
	ULONG   ulRequestCount;
	ULONG   ulRequestCountSig;
	ULONG   ulReleaseCount;
	ULONG   ulReleaseCountSig;
	ULONG   ulWaitFailCount;       // count of WaitForSingleObject() returned WAIT_FAILED
	ULONG   ulWaitTimeOutCount;    // count of WaitForSingleObject() returned WAIT_TIMEOUT
	ULONG   ulWaitErrorCount;      // count of WaitForSingleObject() did NOT return WAIT_OBJECT_0
	ULONG   ulReleaseErrorCount;   // count of ReleaseSemaphore() that failed
	ULONG   ulStateFlags;          // state flags to indicate particular states.  See PIF_SEM_STATE_FLAGS_ above 
	ULONG   ulSemFlags;            // flags indicating actions such as turning on logging. 
	ULONG   ulActionFlags;         // action flags to indicate particular actions.  See PIF_SEM_ACTION_FLAGS_ above 
	DWORD   dwWaitTimeout;
	LONG    lPrevCount;            // lPrevCount from last release of this semaphore
	ULONG   ulOrigLineNo;
	ULONG   ulLastReqLineNo;
	ULONG   ulLastRelLineNo;
	SHORT	sCount;             // count of available concurrent 
	SHORT	sMaxCount;          // max count specified when created 
	SHORT	sPifHandle;
	SHORT   sMaxWaitingThread;
	SHORT   sMaxUnusedCount;
	char    szOrigFile[32];
	char    szLastReqFile[32];
	char    szLastRelFile[32];
} PIFSEMHANDLETABLE;


#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */
CRITICAL_SECTION	g_SemCriticalSection;

static PIFSEMHANDLETABLE	aPifSemTable[PIF_MAX_SEM];

SHORT PifSemSearchId(USHORT usHandle, PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);
USHORT PifSemGetNewId(PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber);

void PifSnapShotSemaphoreTable (SHORT  shSnap)
{
	int i;
	char aszWriteLine [384] = {0};

	sprintf_s (aszWriteLine, 254, "\nSemaphore Table Snap\n");
	PifWriteSnapShotFile (shSnap, aszWriteLine);
	for (i = 0; i < PIF_MAX_SEM; i++) {
		int iLen = strlen (aPifSemTable[i].szOrigFile);
		if (iLen > 20) iLen -= 20; else iLen = 0;
		sprintf_s(aszWriteLine, 380, "  i = %d sPifHandle %d dwCurrentOwnerId 0x%x dwLastOwnerId 0x%x sCount %d, ulStateFlags 0x%8.8x, wait errors %d, release errors %d, ulRequestCount %d, ulReleaseCount %d, Orig %s %d\n", i,
			aPifSemTable[i].sPifHandle, aPifSemTable[i].dwCurrentOwnerId, aPifSemTable[i].dwLastOwnerId, aPifSemTable[i].sCount, aPifSemTable[i].ulStateFlags, aPifSemTable[i].ulWaitErrorCount, aPifSemTable[i].ulReleaseErrorCount,
			aPifSemTable[i].ulRequestCount, aPifSemTable[i].ulReleaseCount,
			aPifSemTable[i].szOrigFile + iLen, aPifSemTable[i].ulOrigLineNo);
		PifWriteSnapShotFile (shSnap, aszWriteLine);
	}
}

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
USHORT PIFENTRY PifCreateSemNew(USHORT usCount, USHORT usInit, CHAR *pcFilePath, int iLineNo)
{
	HANDLE	hMutexHandle = 0;
	HANDLE	hEventHandle = 0;
	HANDLE	hSemaphoreHandle = 0;
	LONG    lInit = usInit, lMax = usCount;
	DWORD	dwError;
	int     iLen = 0;     // used to trim pcFilePath to last 20 characters
	TCHAR   tcsName[32];
	SHORT	sSem;

	EnterCriticalSection(&g_SemCriticalSection);

    /* search free ID */
    sSem = PifSemGetNewId(aPifSemTable, PIF_MAX_SEM);
    if (sSem == PIF_ERROR_HANDLE_OVERFLOW) {
		LeaveCriticalSection(&g_SemCriticalSection);
		PifAbort(FAULT_AT_PIFCREATESEM, FAULT_SHORT_RESOURCE);
		return (USHORT)PIF_ERROR_SYSTEM;
	}

	tcsName[0] = _T('S');
	tcsName[1] = _T('E');
	tcsName[2] = _T('M');
	tcsName[3] = _T(' ');
	tcsName[4] = (sSem / 100) + _T('0');
	tcsName[5] = ((sSem / 10) % 10) + _T('0');
	tcsName[6] = (sSem % 10) + _T('0');
	tcsName[7] = 0;

	if (usCount < 2) lMax = 1;
	hSemaphoreHandle = CreateSemaphore(NULL, lInit, lMax, NULL);
	if (hSemaphoreHandle == 0) {
		dwError = GetLastError();
		PifLog(MODULE_PIF_CREATESEM, LOG_ERROR_PIFSEM_CODE_01);
		PifLog(MODULE_PIF_CREATESEM, (USHORT)dwError);
		LeaveCriticalSection(&g_SemCriticalSection);
		PifAbort(FAULT_AT_PIFCREATESEM, FAULT_SHORT_RESOURCE);
		return (USHORT)PIF_ERROR_SYSTEM;
	}

	aPifSemTable[sSem].sPifHandle = sSem;
	aPifSemTable[sSem].hSemaphoreHandle = hSemaphoreHandle;
	aPifSemTable[sSem].hMutexHandle = hMutexHandle;
	aPifSemTable[sSem].hEventHandle = hEventHandle;
	aPifSemTable[sSem].sCount = usCount;
	aPifSemTable[sSem].sMaxCount = (USHORT)lMax;
	aPifSemTable[sSem].ulRequestCount = 0;
	aPifSemTable[sSem].ulRequestCountSig = 0;
	aPifSemTable[sSem].ulReleaseCount = 0;
	aPifSemTable[sSem].ulReleaseCountSig = 0;
	aPifSemTable[sSem].ulSemFlags = 0;
	aPifSemTable[sSem].dwThreadId = GetCurrentThreadId();
	aPifSemTable[sSem].dwLastOwnerId = aPifSemTable[sSem].dwCurrentOwnerId = 0;
	if (usCount < 1) {
		aPifSemTable[sSem].dwCurrentOwnerId = GetCurrentThreadId();
	}
	aPifSemTable[sSem].dwWaitTimeout = INFINITE;
	aPifSemTable[sSem].ulWaitFailCount = 0;
	aPifSemTable[sSem].ulWaitErrorCount = 0;
	aPifSemTable[sSem].ulWaitTimeOutCount = 0;
	aPifSemTable[sSem].ulReleaseErrorCount = 0;
	aPifSemTable[sSem].ulStateFlags = 0;
	if (usCount == 0) {
		aPifSemTable[sSem].ulStateFlags |= PIF_SEM_STATE_FLAGS_INIT_ZERO;  // indicate a zero initial value was set.
	}
	if (lMax == 1) {
		aPifSemTable[sSem].ulStateFlags |= PIF_SEM_STATE_FLAGS_BINARY;  // indicate a non-zero initial value was set.
	}

	// Initialize high water and low water marks to reasonable values.
	// If usCount is zero then semaphore being created as blocking.
	aPifSemTable[sSem].sMaxWaitingThread = 0;
	aPifSemTable[sSem].sMaxUnusedCount = 0;

	iLen = strlen (pcFilePath);
	if (iLen > 20) {
		iLen -= 20;
	} else {
		iLen = 0;
	}
	strcpy_s (aPifSemTable[sSem].szOrigFile, sizeof(aPifSemTable[sSem].szOrigFile) - 1, pcFilePath + iLen);
	aPifSemTable[sSem].ulOrigLineNo = iLineNo;
	aPifSemTable[sSem].szLastReqFile[0] = 0;
	aPifSemTable[sSem].ulLastReqLineNo = 0;
	aPifSemTable[sSem].szLastRelFile[0] = 0;
	aPifSemTable[sSem].ulLastRelLineNo = 0;
	LeaveCriticalSection(&g_SemCriticalSection);

#if defined(RARELY_NEEDED_LOGS)
	{
		char szText[256];

		sprintf_s (szText, 254, "  PifCreateSem (): Source File: %s   Line No: %d", pcFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT (szText);			
		sprintf_s (szText, 254, "     PifCreateSem(): Semaphore %d  dwThreadId 0x%x lInit %d lMax %d", sSem, aPifSemTable[sSem].dwThreadId, lInit, lMax);
		NHPOS_NONASSERT_TEXT(szText);
	}
#endif
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
VOID   PIFENTRY PifDeleteSem(USHORT usSem)
{
	HANDLE	hMutexHandle = 0;
	HANDLE	hEventHandle = 0;
	HANDLE	hSemaphoreHandle = 0;
	BOOL	fReturn;
	DWORD	dwError;
	SHORT   sSearchStatus = 0;

	EnterCriticalSection(&g_SemCriticalSection);

	sSearchStatus = PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM);
	if (sSearchStatus != usSem) {
		LeaveCriticalSection(&g_SemCriticalSection);
		PifLog(MODULE_PIF_DELETESEM, LOG_ERROR_PIFSEM_CODE_05);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_DELETESEM), (USHORT)abs(sSearchStatus));
		return;
	}

	hSemaphoreHandle = aPifSemTable[usSem].hSemaphoreHandle;
	fReturn = CloseHandle(hSemaphoreHandle);
	if (fReturn == 0) {
		dwError = GetLastError();
		PifLog(MODULE_PIF_DELETESEM, LOG_ERROR_PIFSEM_CODE_03);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_DELETESEM), (USHORT)dwError);
	}

	aPifSemTable[usSem].sPifHandle = -1;

	LeaveCriticalSection(&g_SemCriticalSection);

}

/*fhfh 
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifHighLowSem(                        **
**                               USHORT usSem,                      **
**                               USHORT usMaxWaitingThreads,        **
**  							 USHORT usMaxUnusedCount))          **
**                                                                  **
**       usSem:                semaphore handle                     **
**       usMaxWaitingThreads:  maximum negative count value         **
**       usMaxUnusedCount:     maximum positive count value         **
**                                                                  **
**  Description:Set semaphore's range of expected values.           **     
**                                                                  **
**     Highwater mark is the maximum expected count of threads      ** 
**     waiting on the semaphore to become available.  Highwater     ** 
**     mark is a negative number because a semaphore count goes     ** 
**     negative if threads requesting the semaphore have to wait    ** 
**     until the semaphore becomes available.  The Highwater mark   **
**     is a negative number indicating the maximum number of        **
**     PifRequestSem () calls that are expected to be outstanding   **
**     before a call is made to PifReleaseSem () to release the     **
**     semaphore to a waiting thread.                               ** 
**                                                                  **
**     Lowwater mark is the maximum expected value of the semaphore ** 
**     count if no threads have requested the semaphore.  Lowwater  ** 
**     mark is a positive non-zero number indicated the maximum     ** 
**     number of PifReleaseSem () calls that are expected before a  **
**     call is made to PifRequestSem () to request the semaphore.   ** 
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifHighLowSem(USHORT usSem, ULONG ulSemFlags, USHORT usMaxWaitingThreads, USHORT usMaxUnusedCount)
{
	SHORT  sSearchStatus = 0;
	EnterCriticalSection(&g_SemCriticalSection);

	sSearchStatus = PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM);
	if (sSearchStatus != usSem) {
		LeaveCriticalSection(&g_SemCriticalSection);
		PifLog(FAULT_AT_PIFREQUESTSEM, LOG_ERROR_PIFSEM_CODE_05);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREQUESTSEM), (USHORT)abs(sSearchStatus));
		PifAbort(FAULT_AT_PIFREQUESTSEM, FAULT_INVALID_HANDLE);
		return;
	}

	aPifSemTable[usSem].sMaxWaitingThread = usMaxWaitingThreads;
	aPifSemTable[usSem].sMaxWaitingThread *= (-1);
	aPifSemTable[usSem].sMaxUnusedCount = usMaxUnusedCount;
	aPifSemTable[usSem].ulActionFlags = ulSemFlags;

	LeaveCriticalSection(&g_SemCriticalSection);
}


/*fhfh 
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetTimeoutSem(                     **
**                               USHORT usSem,                      **
**  							 USHORT usTimeoutCount)             **
**                                                                  **
**       usSem:                semaphore handle                     **
**       usTimeoutCount:       time out count in milliseconds       **
**                                                                  **
**  Description: Set semaphore's time out by specifying the number  **     
**               of milliseconds to wait for a time out.  If the    **
**               time out value is zero then set to INFINITE        **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifSetTimeoutSem(USHORT usSem, USHORT usTimeoutCount)
{
	EnterCriticalSection(&g_SemCriticalSection);

	if (PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM) != usSem) {

		LeaveCriticalSection(&g_SemCriticalSection);
		PifAbort(FAULT_AT_PIFREQUESTSEM, FAULT_INVALID_HANDLE);
		return;
	}

	if (usTimeoutCount < 1)
		aPifSemTable[usSem].dwWaitTimeout = INFINITE;
	else
		aPifSemTable[usSem].dwWaitTimeout = usTimeoutCount;

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
SHORT  PIFENTRY PifRequestSemNew(USHORT usSem, char *aszFilePath, int iLineNo)
{
	int    iLen = 0;
	SHORT  sRetStatus = 0;

	iLen = strlen (aszFilePath);
	if (iLen > 20) {
		iLen = iLen - 20;
	}
	else {
		iLen = 0;
	}

	if ( (sRetStatus = PifRequestSem(usSem)) < 0 || sRetStatus > 0) {
		char  xBuff[256];

		sprintf_s (xBuff, 254, "==LOG: PifRequestSemNew(): usSem %d  %s  %d - last rel 0x%x %s  %d", usSem, aszFilePath + iLen, iLineNo, aPifSemTable[usSem].dwLastOwnerId, aPifSemTable[usSem].szLastRelFile, aPifSemTable[usSem].ulLastRelLineNo);
		NHPOS_NONASSERT_TEXT(xBuff);
		sprintf_s (xBuff, 254, "                           last req %s  %d", aPifSemTable[usSem].szLastReqFile, aPifSemTable[usSem].ulLastReqLineNo);
		NHPOS_NONASSERT_TEXT(xBuff);
		if (sRetStatus < 0)
			PifAbort(FAULT_AT_PIFREQUESTSEM, FAULT_INVALID_HANDLE);
	}
	
	if (sRetStatus >= 0) {
		strncpy_s (aPifSemTable[usSem].szLastReqFile, sizeof(aPifSemTable[usSem].szLastReqFile) - 1, aszFilePath + iLen, 20);
		aPifSemTable[usSem].ulLastReqLineNo = iLineNo;
	}
	return sRetStatus;
}

SHORT   PIFENTRY PifRequestSem(USHORT usSem)
{
	HANDLE	hMutexHandle = 0;
	HANDLE	hEventHandle = 0;
	HANDLE	hSemaphoreHandle = 0;
	DWORD	dwReturn, dwError;
	SHORT   sSearchStatus = 0;
	SHORT   sRetStatus = 0;

	EnterCriticalSection(&g_SemCriticalSection);

	sSearchStatus = PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM);
	if (sSearchStatus != usSem) {
		LeaveCriticalSection(&g_SemCriticalSection);
		PifLog(FAULT_AT_PIFREQUESTSEM, FAULT_INVALID_HANDLE);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREQUESTSEM), (USHORT)abs(sSearchStatus));
#if PifRequestSemNew_debug
#else
		PifAbort(FAULT_AT_PIFREQUESTSEM, FAULT_INVALID_HANDLE);
#endif
		return -1;
	}

	hSemaphoreHandle = aPifSemTable[usSem].hSemaphoreHandle;
#if 0
	if (aPifSemTable[usSem].sMaxWaitingThread < 0 && aPifSemTable[usSem].sMaxWaitingThread > aPifSemTable[usSem].sCount) {
		PifLog(MODULE_PIF_REQUESTSEM, LOG_ERROR_PIFSEM_WAIT_TIMEOUT_01);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), usSem);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), aPifSemTable[usSem].sMaxWaitingThread);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), aPifSemTable[usSem].sCount);
		PifLog(MODULE_LINE_NO(MODULE_PIF_REQUESTSEM), (USHORT)__LINE__);
	}
#endif
	aPifSemTable[usSem].ulStateFlags |= PIF_SEM_STATE_FLAGS_REQ_REL;  // set flag indicating last action was Request
	if (aPifSemTable[usSem].ulActionFlags & PIF_SEM_ACTION_FLAGS_REQ_ZERO) {
		if (aPifSemTable[usSem].sCount <= 0) {
			char  xBuff[256];

			sprintf_s (xBuff, 254, "==LOG: PIF_SEM_ACTION_FLAGS_REQ_ZERO usSem %d  sCount %d  dwCurrentOwnerId 0x%x  %s %d", usSem, aPifSemTable[usSem].sCount, aPifSemTable[usSem].dwCurrentOwnerId, aPifSemTable[usSem].szLastReqFile, aPifSemTable[usSem].ulLastReqLineNo);
			NHPOS_NONASSERT_TEXT(xBuff);
			sRetStatus = 3;
			aPifSemTable[usSem].ulActionFlags |= PIF_SEM_STATE_FLAGS_LOG_ON_REL;
		}
	}
	LeaveCriticalSection(&g_SemCriticalSection);

	dwReturn = WaitForSingleObject(hSemaphoreHandle, INFINITE);

	EnterCriticalSection(&g_SemCriticalSection);
	if (dwReturn == WAIT_FAILED) {
		dwError = GetLastError();
		PifLog(MODULE_PIF_REQUESTSEM, LOG_ERROR_PIFSEM_CODE_05);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_REQUESTSEM), (USHORT)dwError);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), usSem);
		aPifSemTable[usSem].ulWaitFailCount++;
	}
	if (dwReturn == WAIT_TIMEOUT) {
		PifLog(MODULE_PIF_REQUESTSEM, LOG_ERROR_PIFSEM_WAIT_TIMEOUT_01);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), usSem);
		aPifSemTable[usSem].ulWaitTimeOutCount++;
	}
	if (dwReturn != WAIT_OBJECT_0) {
		PifLog(MODULE_PIF_REQUESTSEM, LOG_ERROR_PIFSEM_WAIT_TIMEOUT_01);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), usSem);
		aPifSemTable[usSem].ulWaitErrorCount++;
	}
	aPifSemTable[usSem].dwCurrentOwnerId = GetCurrentThreadId();
	aPifSemTable[usSem].sCount--;
	aPifSemTable[usSem].ulRequestCount++;
	if (aPifSemTable[usSem].ulRequestCount == 0) {
		aPifSemTable[usSem].ulRequestCountSig++;
	}
	LeaveCriticalSection(&g_SemCriticalSection);

	return sRetStatus;
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
SHORT  PIFENTRY PifReleaseSemNew(USHORT usSem, char *aszFilePath, int iLineNo)
{
	int    iLen = 0;
	SHORT  sRetStatus = 0;

	iLen = strlen (aszFilePath);
	if (iLen > 20) {
		iLen = iLen - 20;
	}
	else {
		iLen = 0;
	}

	if ( (sRetStatus = PifReleaseSem(usSem)) < 0 || sRetStatus > 0) {
		char  xBuff[256];

		sprintf_s (xBuff, 254, "==LOG: PifReleaseSemNew(): usSem %d  %s  %d - last %s  %d", usSem, aszFilePath + iLen, iLineNo, aPifSemTable[usSem].szLastRelFile, aPifSemTable[usSem].ulLastRelLineNo);
		NHPOS_NONASSERT_TEXT(xBuff);
		if (sRetStatus < 0)
			PifAbort(FAULT_AT_PIFRELEASESEM, FAULT_INVALID_HANDLE);
	}

	if (aPifSemTable[usSem].ulActionFlags & PIF_SEM_STATE_FLAGS_LOG_ON_REL) {
		char  xBuff[256];

		aPifSemTable[usSem].ulActionFlags &= ~PIF_SEM_STATE_FLAGS_LOG_ON_REL;
		sprintf_s (xBuff, 254, "==LOG: PIF_SEM_STATE_FLAGS_LOG_ON_REL usSem %d  %s  %d - last 0x%x  %s  %d", usSem, aszFilePath + iLen, iLineNo, aPifSemTable[usSem].dwLastOwnerId, aPifSemTable[usSem].szLastRelFile, aPifSemTable[usSem].ulLastRelLineNo);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	if (sRetStatus >= 0) {
		strncpy_s (aPifSemTable[usSem].szLastRelFile, sizeof(aPifSemTable[usSem].szLastRelFile) - 1, aszFilePath + iLen, 20);
		aPifSemTable[usSem].ulLastRelLineNo = iLineNo;
	}
	return sRetStatus;
}

SHORT   PIFENTRY PifReleaseSem(USHORT usSem)
{
	HANDLE	hEventHandle = 0;
	HANDLE	hSemaphoreHandle = 0;
	LONG    lPrevCount = 0;
	BOOL	fReturn;
	DWORD	dwError;
	SHORT   sSearchStatus = 0;
	SHORT   sLowWaterMark = 0, sCount = 0;
	SHORT   sRetStatus = 0;

	EnterCriticalSection(&g_SemCriticalSection);

	sSearchStatus = PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM);
	if (sSearchStatus != usSem) {
		LeaveCriticalSection(&g_SemCriticalSection);
		PifLog(FAULT_AT_PIFRELEASESEM, FAULT_INVALID_HANDLE);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_RELEASESEM), usSem);
		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFRELEASESEM), (USHORT)abs(sSearchStatus));
#if PifReleaseSemNew_debug
#else
		PifAbort(FAULT_AT_PIFRELEASESEM, FAULT_INVALID_HANDLE);
#endif
		return -1;
	}

	hSemaphoreHandle = aPifSemTable[usSem].hSemaphoreHandle;
	aPifSemTable[usSem].sCount++;
	aPifSemTable[usSem].ulReleaseCount++;
	if (aPifSemTable[usSem].ulReleaseCount == 0) {
		aPifSemTable[usSem].ulReleaseCountSig++;
	}
	aPifSemTable[usSem].dwLastOwnerId = aPifSemTable[usSem].dwCurrentOwnerId;
	aPifSemTable[usSem].dwCurrentOwnerId = 0;
#if 0
	if (aPifSemTable[usSem].sMaxUnusedCount > 0 && aPifSemTable[usSem].sMaxUnusedCount < aPifSemTable[usSem].sCount) {
		PifLog(MODULE_PIF_REQUESTSEM, LOG_ERROR_PIFSEM_CODE_08);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), usSem);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), aPifSemTable[usSem].sMaxUnusedCount);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_REQUESTSEM), aPifSemTable[usSem].sCount);
		PifLog(MODULE_LINE_NO(MODULE_PIF_REQUESTSEM), (USHORT)__LINE__);
	}
#endif
	aPifSemTable[usSem].ulStateFlags &= ~PIF_SEM_STATE_FLAGS_REQ_REL;  // clear flag indicating last action was Release
	LeaveCriticalSection(&g_SemCriticalSection);

	fReturn = ReleaseSemaphore(hSemaphoreHandle, 1, &lPrevCount);
	aPifSemTable[usSem].lPrevCount = lPrevCount;
	if (fReturn == 0) {
		dwError = GetLastError();
		PifLog(MODULE_PIF_RELEASESEM, LOG_ERROR_PIFSEM_CODE_08);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_RELEASESEM), usSem);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_RELEASESEM), (USHORT)lPrevCount);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_RELEASESEM), (USHORT)dwError);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_RELEASESEM), aPifSemTable[usSem].sCount);
		PifLog(MODULE_DATA_VALUE(MODULE_PIF_RELEASESEM), (USHORT)GetCurrentThreadId());
		aPifSemTable[usSem].ulReleaseErrorCount++;
		sRetStatus = 2 * 1000 + usSem;
	}

	if (aPifSemTable[usSem].ulActionFlags & PIF_SEM_ACTION_FLAGS_REL_NONZERO) {
		if (lPrevCount > 0) {
			char  xBuff[128];

			sprintf_s (xBuff, 126, "==LOG: PIF_SEM_ACTION_FLAGS_REL_NONZERO usSem %d  sCount %d  dwLastOwnerId 0x%x", usSem, aPifSemTable[usSem].sCount, aPifSemTable[usSem].dwLastOwnerId);
			NHPOS_NONASSERT_TEXT(xBuff);
			sRetStatus = 3;
		}
	}

	return sRetStatus;
}


SHORT   PIFENTRY PifRetrieveValueSem(USHORT usSem)
{
	SHORT   sCount = -1;

	EnterCriticalSection(&g_SemCriticalSection);

	if (PifSemSearchId(usSem, aPifSemTable, PIF_MAX_SEM) == usSem) {
		sCount = aPifSemTable[usSem].sCount;
	}

	LeaveCriticalSection(&g_SemCriticalSection);

	return sCount;
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
VOID PifDeleteSemAll(VOID)
{
	int i;

	for (i = 0; i < PIF_MAX_SEM; i++) {
		if (aPifSemTable[i].sPifHandle != -1) {
			PifDeleteSem(aPifSemTable[i].sPifHandle);
		}
	}
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
VOID PifSemInitialize(VOID)
{
	USHORT i;

	for (i = 0; i < PIF_MAX_SEM; i++) {
		aPifSemTable[i].sPifHandle = -1;
		aPifSemTable[i].hMutexHandle = INVALID_HANDLE_VALUE;
		aPifSemTable[i].hEventHandle = INVALID_HANDLE_VALUE;
		memset (aPifSemTable[i].szLastRelFile, 0, sizeof(aPifSemTable[i].szLastRelFile));
		memset (aPifSemTable[i].szLastReqFile, 0, sizeof(aPifSemTable[i].szLastReqFile));
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
USHORT PifSemGetNewId(PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	SHORT              sIndex;
    USHORT             i;
	PIFSEMHANDLETABLE *pPifHandleTableSave = pPifHandleTable;

	// We start with a handle number of 1 to allow for handle value of zero to be an error
    for (i = 1; i < usMaxNumber; i++) {
		if (pPifHandleTable[i].sPifHandle == -1) {
			return i;
		}
    }

	// We have run out of resources.  Lets do a dump of the table to capture what has happened.
	PifLog (FAULT_AT_PIFCREATESEM, FAULT_INVALID_ENVIRON_1);

	sIndex = PifOpenSnapShotFile(_T("SNAP0002"), __FILE__, __LINE__);

	if (sIndex >= 0) {
		CHAR aszWriteLine[256];

		for (i = 0; i < usMaxNumber; i++) {
			if (pPifHandleTableSave->sPifHandle != -1) {
				sprintf_s (aszWriteLine, 254, "i = %3.3d, Orig = 0x%x,  Owner = 0x%x,  Count = %d\n", i, pPifHandleTableSave->dwThreadId, pPifHandleTableSave->dwCurrentOwnerId, pPifHandleTableSave->sCount);
				PifWriteSnapShotFile (sIndex, aszWriteLine);
			}
			pPifHandleTableSave++;
		}

		PifCloseSnapShotFile (sIndex);
	}

    return (USHORT)PIF_ERROR_HANDLE_OVERFLOW;
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
SHORT PifSemSearchId(USHORT usHandle, PIFSEMHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	if (usHandle > usMaxNumber) {
		return -1;
	}

	if (pPifHandleTable[usHandle].sPifHandle != usHandle) {
		return -2;
	}

	return usHandle;
}

/* end of pifsem.c */

