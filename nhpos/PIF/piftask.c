/* piftask.c : PIF thread control functions */

#ifndef _MT
#define _MT
#endif

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include <crtdbg.h>
#include <tchar.h>
#include <process.h>
#include <intrin.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"
#include "regstrct.h"
//#include "item.h"
#include "paraequ.h"
#include "transact.h" //SR 353 necessary for using TrnInformation Structure
#include "trans.h"    //SR 353 necessary for using TrnInformation Structure
#include "appllog.h"
#include "BlFWif.h"
#include "evs.h"
/* manipulation for segment and offset */
//#define FP_SEG(fp) (*((ULONG *)&(fp) + 1))
//#define FP_OFF(fp) (*((ULONG *)&(fp)))
//#define FP_SEG(fp) (*((USHORT *)&(fp) + 1))
//#define FP_OFF(fp) (*((USHORT *)&(fp)))

static int nShutdownStarted = 0;

#define  PIF_TASK_FIRST_THREAD_INDEX     2
#define  PIF_TASK_TEMP_THREAD            1

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef struct {
    DWORD dwThreadId;
    HANDLE hHandle;
	int   ulOrigLineNo;
    BOOL fThread;
    CHAR szThreadName[PIF_TASK_LEN+1];
	CHAR  szOrigFile[32];
} PIFTASKTABLE;

typedef struct {
	USHORT	usNumofStructures;
	ULONG	ulFileSize;
} PIFDEBUGFILEHEAD;

typedef struct {
	CHAR	fileName[40];
	CHAR	structureName[64];
	CHAR	dateOfCapture[30];
	INT		iLineNumber;
	INT		iSizeofData;
} PIFDEBUGDUMP;

typedef struct STACK { /* stack structure at entry point of Pifxxxx() */
    struct STACK NEAR *usBP; /* CPU BP register indicates the segment */
                                 /* of the caller's base stack */
    PIFDEF pRetAddr;         /* return address to caller */
} STACK;

typedef union {
    PIFDEF  pRetAddr;
    UCHAR   uchByte[4];
} RETADDR;
#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

#define PIF_ABORT_DISPLAY_LEN   40
#define PIF_ASSERT_FILE_MAX		2500000  //39(length of time,date,thread length + 257(length of 
										  // Condition, Filename, Functionname, Line No) * 20 (number of entries


PIFTASKTABLE aPifTaskTable[PIF_MAX_TASK];

CRITICAL_SECTION    g_TaskCriticalSection;

SHORT sPifAbortReqSem = -1;
SHORT sPifAbortRelSem = -1;
TCHAR szPifAbortDisplay[PIF_ABORT_DISPLAY_LEN+1];

//each element in the array corresponds to a month, starting with january
TCHAR szDebugFileMonth[] = _T("ABCDEFGHIJKL");

//each element in the array corresponds to a year, starting with 2005
TCHAR szDebugFileYear[]  = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

//each element in the array correseponds to all 24 hours  starting with 0
TCHAR szDebugFileHour[]	 = _T("ABCDEFGHIJKLMNOPQRSTUVWXY");

//each element in the array correspons to a day of the mont starting with 1
TCHAR szDebugFileDay[]	 = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ12345");


extern TCHAR wszPifLogPathName[];

BOOL    fsFault = FALSE;
BOOL    fsFaultDisp = FALSE;

VOID   PifSubPrint(UCHAR *pauchPrtMsg, USHORT usLen);
VOID   PifSubFtoA(PIFDEF pRetAddr, UCHAR *pauchMsg);
VOID   PifSubPtoA(UCHAR *pauchPPacked, UCHAR *pauchMsg, USHORT usSizePacked);

VOID PifSubSendAbortEvent(TCHAR *pszDisplay);
VOID PifSubWaitForAbortAck(VOID);

/* --- Task --- */

void PifSnapShotThreadTable (SHORT  shSnap)
{
	int i;
	char aszWriteLine [256];

	sprintf (aszWriteLine, "\nThread Table Snap\n");
	PifWriteSnapShotFile (shSnap, aszWriteLine);
	for (i = 0; i < PIF_MAX_TASK; i++) {
		aPifTaskTable[i].szThreadName[PIF_TASK_LEN] = 0;  // ensure end of string terminator.
		sprintf (aszWriteLine, "  i = %d  dwThreadId 0x%x Name %s Orig %s %d\n", i, aPifTaskTable[i].dwThreadId, aPifTaskTable[i].szThreadName, aPifTaskTable[i].szOrigFile, aPifTaskTable[i].ulOrigLineNo);
		PifWriteSnapShotFile (shSnap, aszWriteLine);
	}
}

/*
	Following code is used to provide to the MessageBox() function the
	main window handle.  This makes the MessageBox () dialog pop up on
	top of the NHPOS main window so that it is clearly visible to the
	user.
 */
static void *pNhposMainHwnd = 0;

VOID PIFENTRY PifSetWindowHandle (void *pHwnd)
{
	pNhposMainHwnd = pHwnd;
	NHPOS_ASSERT_TEXT(pHwnd, "PifSetWindowHandle");
}

VOID *PIFENTRY PifGetWindowHandle (void **ppHwnd)
{
	if (ppHwnd) {
		*ppHwnd = pNhposMainHwnd;
	}

	NHPOS_ASSERT_TEXT(pNhposMainHwnd, "PifGetWindowHandle");
	return pNhposMainHwnd;
}

VOID PIFENTRY PifSnapShotTables (VOID)
{
	SHORT shSnap = PifOpenSnapShotFile(_T("SNAP0003"), __FILE__, __LINE__);

	if (shSnap) {
		char aszWriteLine [256];

		sprintf (aszWriteLine, "PifSnapShotTables  \n");
		PifWriteSnapShotFile (shSnap, aszWriteLine);
		PifSnapShotThreadTable (shSnap);
		PifSnapShotFileTableFH (shSnap);
		PifSnapShotSemaphoreTable (shSnap);
		PifCloseSnapShotFile (shSnap);
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifAbort(USHORT usFaultModule         **
**                                      USHORT usFaultCode)         **
**              usFaultModule:    faulted moudle code               **
**              usFaultCode:      fault code
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description: stoping application, logging error code, and restart **     
**                                                                  **
**********************************************************************
fhfh*/
//
static VOID   PifAbortBase (USHORT usFaultModule, USHORT usFaultCode)
{
    DWORD    dwThreadId;
    TCHAR    wszDisplay[80+1];
    CHAR     szDisplay[PIF_ABORT_DISPLAY_LEN+1];
    USHORT   i;
	char     aszErrorBuffer[128+MAX_PATH];

	PifSnapShotTables ();

	PifLog(MODULE_PIF_ABORT, MODULE_PIF_ABORT);
    PifLog(usFaultModule, usFaultCode);

    if (fsFaultDisp == TRUE) return;	/* just handle first fault call */
    fsFaultDisp = TRUE;

    PifCloseFileHandleExec(FALSE);
    PifSaveRegistry();
	PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE);

    dwThreadId = GetCurrentThreadId();
    for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].dwThreadId == dwThreadId) break;
    }

    if (i >= PIF_MAX_TASK) {
        i = PIF_TASK_TEMP_THREAD;    /* default thread name */
        aPifTaskTable[i].dwThreadId = GetCurrentThreadId();
    }

    wsprintf(wszDisplay, TEXT("FAULT%S %d %d TURN OFF AND ON"), aPifTaskTable[i].szThreadName, usFaultModule, usFaultCode);
	sprintf (aszErrorBuffer,"FAULT%s %d %d", aPifTaskTable[i].szThreadName, usFaultModule, usFaultCode);
	NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
	// RPH memset(szDisplay, 0, sizeof(szDisplay));	/* 09/04/01 */
    // RPH wcstombs(szDisplay, wszDisplay, sizeof(szDisplay));

    PifSubSendAbortEvent((TCHAR*)wszDisplay);
    PifSubWaitForAbortAck();

    /* print "======================" */
    memset(szDisplay, '=', sizeof(szDisplay));
    PifSubPrint((UCHAR*)szDisplay, sizeof(szDisplay));

    /* print "FAULTnnnnn ???????? " */
    wsprintf(wszDisplay, TEXT("FAULT%S %d %d"), aPifTaskTable[i].szThreadName, usFaultModule, usFaultCode);
	memset(szDisplay, 0, sizeof(szDisplay));	/* 09/04/01 */
    wcstombs(szDisplay, wszDisplay, 24);
    PifSubPrint((UCHAR*)szDisplay, 24);

	if (fsFault == FALSE) {
		TCHAR *tcsMsg = 0;

		//we have implemented a way for pif aborts to show information to the user
		//so that they will have a better idea of ways to alleviate problems them
		//selves. JHHJ

		switch (usFaultCode)
		{
		case FAULT_ERROR_MSDE_OPEN_DB:
			tcsMsg = L"Please check MSDE connection";
			break;
		case FAULT_ERROR_TTLUPD_WAI_FILE:
			tcsMsg = L"Updating Waiter Totals Failed";
			break;
		case FAULT_ERROR_TTLUPD_PLU_FILE:
			tcsMsg = L"Updating PLU Totals Failed";
			break;
		case FAULT_ERROR_TTLUPD_DEP_FILE:
			tcsMsg = L"Updating Department Totals Failed";
			break;
		case FAULT_ERROR_TTLUPD_CAS_FILE:
			tcsMsg = L"Updating Cashier Totals Failed";
			break;
		case FAULT_ERROR_TTLUPD_CPN_FILE:
			tcsMsg = L"Updating Coupon Totals Failed";
			break;
		case FAULT_ERROR_AFFECTION:
			tcsMsg = L"Updating Totals Failed. Multiple Errors";
			break;
		default:
			tcsMsg = L"TURN OFF AND ON";
			break;
		}
		wsprintf(wszDisplay, TEXT("FAULT %S Module=%d Code=%d\nTURN OFF AND ON\n\n%s"), aPifTaskTable[i].szThreadName, usFaultModule, usFaultCode, tcsMsg);
        MessageBox(pNhposMainHwnd, wszDisplay, TEXT("PIFABORT"), MB_OK);
    }


#if 0
    /* log the current base stack pointer */
    _asm {
       mov     WORD PTR pStack+4,ss
       mov     WORD PTR pStack,bp
    }
    pLog->pCurrentStack = pStack;
    /* log the tracking of the return address of the caller */
    pStackWork = pStack;
    j = TRUE;
    for (i=0; i<pLog->usMaxCount; i++) {
        if (j) {
            pLog->apRetAddr[i] = pStackWork->pRetAddr;
        } else {
            pLog->apRetAddr[i] = NULL;
        }
        if (pStackWork->pRetAddr!=NULL) {
//        if (pStackWork->usBP!=0) {
            pStackWork = pStackWork->usBP;
            FP_SEG(pStackWork) = FP_SEG(pStack);
        } else {
            j = FALSE;
        }
    }

    /* print "======================" */
    memset(szPrint, '=', sizeof(szPrint));
    PifSubPrint(szPrint, sizeof(szPrint));

    /* print "FAULTnnnnn ???????? " */
    wsprintf(wszPrint, TEXT("FAULT%S %d"), aPifTaskTable[i].szThreadName, usFaultCode);
	memset(szPrint, 0, sizeof(szPrint));	/* 09/04/01 */
    wcstombs(szPrint, wszPrint, 24);
    PifSubPrint(szPrint, sizeof(szPrint));

    /* print "mm/dd/yyyy, hh:mm:ss " */
    PifGetDateTime(&DateTime);
    wsprintf(wszPrint, TEXT("%d/%d/%d %d:%d:%d"),
        DateTime.usMonth, DateTime.usMDay, DateTime.usYear, DateTime.usHour, DateTime.usMin, DateTime.usSec);
	memset(szPrint, 0, sizeof(szPrint));	/* 09/04/01 */
    wcstombs(szPrint, wszPrint, 24);
    PifSubPrint(szPrint, sizeof(szPrint));

    /* print "xxxx   ssss:oooo     " (XINU status, current stack) */
    if (pLog != NULL) {
        memset(szPrint, ' ', sizeof(szPrint));
        PifSubFtoA((PIFDEF)pLog->pCurrentStack, &szPrint[6]);
        PifSubPrint(szPrint, sizeof(szPrint));

        /* print 5 lines as "ssss:oooo  ssss:oooo " */
        /* (return address to caller) */
        for (i=0; i<10; i++) {
            memset(szPrint, ' ', sizeof(szPrint));
            if (pLog->apRetAddr[i]==NULL) break;
            PifSubFtoA(pLog->apRetAddr[i], &szPrint[0]);
            if (pLog->apRetAddr[i+1]==NULL) {
                PifSubPrint(szPrint, sizeof(szPrint));
                break;
            }
            PifSubFtoA(pLog->apRetAddr[++i], &szPrint[11]);
            PifSubPrint(szPrint, sizeof(szPrint));
        }
    }

    /* print "======================" */
    memset(szPrint, '=', sizeof(szPrint));
    PifSubPrint(szPrint, sizeof(szPrint));

    /* receipt feed */
    memset(szPrint, ' ', sizeof(szPrint));
    PifSubPrint(szPrint, sizeof(szPrint));
#endif

    /* terminate 2171 application */

//    PifRestart(1);

    if (fsFault == FALSE) {
		void * NhposMainHwnd = 0;

		PifGetWindowHandle(&NhposMainHwnd);
		// there appears to be a problem with doing this orderly shutdown.
		// US Customs has reported a problem with Rel 2.2.1.146 displaying a
		// Windows Error Dialog when GenPOS is shutting down.
		// there also seems to be a deadlock issue of some kind as well as
		// an exception error that can be thrown about a bad memory read address.
		// however it also seems that after rebooting PC, issues went away.
		// readings indicate that WM_CLOSE to the main window.should be used instead of WM_QUIT.
		//    Richard Chambers, May 5, 2015
		// we have since changed the architecture to use an event to drive the shutdown thread
		// from function FrameworkShutdownThread() to do shutdown.
		//    Richard Chambers, May 8, 2015
		if (NhposMainHwnd)
			PostMessage(NhposMainHwnd, WM_APP_SHUTDOWN_MSG, 0, 0);

		fsFault = TRUE;
    }
    return;

}

VOID   PIFENTRY PifAbortExtended (USHORT usModuleId, USHORT usExceptionCode, char *szFilePath, ULONG ulLineNo)
{
#if defined (_DEBUG)
	__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
#else
	PifAbortBase (usModuleId, usExceptionCode);
#endif

	if (usModuleId < 1000) {
		char  xBuff[128];

		sprintf (xBuff, "==PIFABORT:  Module %4d  Code %4d",  usModuleId, usExceptionCode);
		PifLogNoAbort( (UCHAR *)  "PifAbortExtended()" , (UCHAR *) szFilePath , (UCHAR *) xBuff, ulLineNo);
	}
}

#if defined(PifAbort)
#undef PifAbort
#endif
#if defined (_DEBUG)
VOID   PIFENTRY PifAbort(USHORT usFaultModule, USHORT usFaultCode)
{
	__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
}
#else
VOID   PIFENTRY PifAbort(USHORT usFaultModule, USHORT usFaultCode)
{
	PifAbortBase (usFaultModule, usFaultCode);
}
#endif

SHORT PIFENTRY PifShutdown(VOID)
{
    DWORD dwThreadId;
    USHORT  i;

	if (nShutdownStarted)
	{
		return -1;
	}
	nShutdownStarted = 1;
    PifLog(MODULE_PIF_TURNOFFPOWER, LOG_EVENT_POWER_DOWN);

    PifCloseFileHandleExec(FALSE);
    PifSaveRegistry();
//	PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE);

    dwThreadId = GetCurrentThreadId();
    for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].dwThreadId == dwThreadId) break;
    }
    if (i >= PIF_MAX_TASK) {
        i = PIF_TASK_TEMP_THREAD;    /* default thread name */
        aPifTaskTable[i].dwThreadId = GetCurrentThreadId();
    }

	PifShutDownApp();

    /* terminate application */
    return 1;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifBeginThread(VOID (THEADENTRY *pStart)(VOID), **
**                                      VOID FAR *pStack,
**                                      USHORT usStackSize,
**                                      USHORT usPriority,
**                                      CONST CHAR FAR *pszThreadName, ...)
**                                      USHORT usFaultCode)         **
**              pStart:    target thread function               **
**              pStack:     stack data, must be NULL
**              usStackSize: stack size of thread
**              usPriority: thread priority
**              pszThreadName: thread name
**                                                                  **
**  return:     thread id                                           **
**                                                                  **
**  Description: stoping application, logging error code, and restart **     
**                                                                  **
**********************************************************************
fhfh*/

/********************************************************************
**
**  Description: assign to a thread a thread name.
**               very useful when running GenPOS in a debugger!
*/
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).  copied into thread storage.
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void SetPifThreadName( DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info = {0};
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;

	__try
	{
		const DWORD MS_VC_EXCEPTION=0x406D1388;
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

ULONG PIFENTRY PifBeginThread(VOID (THREADENTRY *pStart)(VOID),
                               VOID *pStack, USHORT usStackSize,
                               USHORT usPriority,
                               CONST CHAR *pszThreadName, ...)
{
    HANDLE  hHandle;
    DWORD   dwThreadId;
    int     nPriority = THREAD_PRIORITY_NORMAL;
    DWORD   dwArg, dwError;
    LPVOID  lpArg = NULL;
    int     i;

    EnterCriticalSection(&g_TaskCriticalSection);
    for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].fThread == FALSE) break;
    }
    if (i >= PIF_MAX_TASK) {
        LeaveCriticalSection(&g_TaskCriticalSection);
        PifAbort(FAULT_AT_PIFBEGINTHREAD, FAULT_SHORT_RESOURCE);
        return (USHORT)PIF_ERROR_SYSTEM;
    }
    if (usPriority > PIF_TASK_TIME_CRITICAL) {
        LeaveCriticalSection(&g_TaskCriticalSection);

        PifAbort(FAULT_AT_PIFBEGINTHREAD, FAULT_INVALID_ARGUMENTS);
        return (USHORT)PIF_ERROR_SYSTEM;
    }

    /* set variable argumenet to LPVOID */
    dwArg = *(ULONG *)(&pszThreadName+1);
    lpArg = (LPVOID)dwArg;

	// replace use of CreateThread() with _beginthreadex() per recommendations
	// from various places including Microsoft since _beginthreadex()
	// will do C Run Time initialization for multi-threaded applications
	// while CreateThread() is a raw Windows API function that does not.
	// Supposedly the C Run Time will figure out that Thread Local Storage
	// is needed at some point and create it all.
	hHandle = (HANDLE)_beginthreadex(NULL, 0, (LPTHREAD_START_ROUTINE)pStart, lpArg, 0, &dwThreadId);
    if (hHandle == NULL) {
        dwError = GetLastError();
		PifLog(MODULE_PIF_BEGINTHREAD, LOG_ERROR_PIFTASK_CODE_01);
        PifLog(MODULE_PIF_BEGINTHREAD, (USHORT)dwError);
        LeaveCriticalSection(&g_TaskCriticalSection);
        PifAbort(FAULT_AT_PIFBEGINTHREAD, FAULT_SYSTEM);
        return (USHORT)PIF_ERROR_SYSTEM;
    }

    /* set thread priority */
    switch (usPriority)
	{
		case PIF_TASK_TIME_CRITICAL:
			nPriority = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		case PIF_TASK_HIGHEST:
			nPriority = THREAD_PRIORITY_HIGHEST;
			break;
		case PIF_TASK_ABOVE_NORMAL:
			nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case PIF_TASK_NORMAL:
			nPriority = THREAD_PRIORITY_NORMAL;
			break;
		case PIF_TASK_LOWEST:
			nPriority = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case PIF_TASK_ABOVE_IDLE:
			nPriority = THREAD_PRIORITY_LOWEST;
	//      nPriority = THREAD_PRIORITY_ABOVE_IDLE;
			break;
		case PIF_TASK_IDLE:
			nPriority = THREAD_PRIORITY_IDLE;
			break;
		default:
			nPriority = THREAD_PRIORITY_NORMAL;
			break;
    }

    if (SetThreadPriority(hHandle, nPriority) == 0)
	{
        dwError = GetLastError();
		PifLog(MODULE_PIF_BEGINTHREAD, LOG_ERROR_PIFTASK_CODE_02);
        PifLog(MODULE_PIF_BEGINTHREAD, (USHORT)dwError);
        PifAbort(FAULT_AT_PIFBEGINTHREAD, FAULT_SYSTEM);
    }


	//  CloseHandle(hHandle);
    memcpy(aPifTaskTable[i].szThreadName, pszThreadName, PIF_TASK_LEN);
    aPifTaskTable[i].dwThreadId = dwThreadId;
    aPifTaskTable[i].fThread = TRUE;
    aPifTaskTable[i].hHandle = hHandle;

	SetPifThreadName (dwThreadId, pszThreadName);
    LeaveCriticalSection(&g_TaskCriticalSection);

#if defined(RARELY_NEEDED_LOGS)
	// This log removed in order to reduce the number of rarely needed logs
	{
		char szText[256];
		sprintf (szText, "  PifBeginThread(): Name %s  dwThreadId 0x%x", pszThreadName, dwThreadId);
		NHPOS_NONASSERT_TEXT(szText);
	}
#endif

    return dwThreadId;

    pStack = pStack;
	usStackSize = usStackSize;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifEndThread(VOID)                   **
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description: exit thread                                        **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifEndThread(VOID)
{
    int i;
    DWORD dwThreadId = GetCurrentThreadId();

    EnterCriticalSection(&g_TaskCriticalSection);
    for (i = 1; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].dwThreadId == dwThreadId) {
	        aPifTaskTable[i].fThread = FALSE;
	    }
    }
    LeaveCriticalSection(&g_TaskCriticalSection);

	{
		char xBuff[128];

		sprintf (xBuff, "==WARNING: PifEndThread() id %d 0x%x", dwThreadId, dwThreadId);
		NHPOS_NONASSERT_NOTE("==WARNING", xBuff);
	}
	_endthreadex(0);
    return;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT CONST FAR *  PIFENTRY PifGetProcessId(VOID)  **
**                                                                  **
**  return:     process id                                               **
**                                                                  **
**  Description: return process id                                        **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT CONST FAR * PIFENTRY PifGetProcessId(VOID)
{
    return 0;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   ULONG PIFENTRY PifGetThreadId(VOID)                 **
**                                                                  **
**  return:     thread id                                           **
**                                                                  **
**  Description: return thread id                                   **     
**                                                                  **
**********************************************************************
fhfh*/
ULONG PIFENTRY PifGetThreadId(VOID)
{

    return GetCurrentThreadId();
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifDisable(VOID)                    **
**                                                                  **
**  Description:This function suspends every thread in the current  **
**              process, expect for the calling thread              **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifDisable(VOID) /* for the Recovery Manager */
{
    HANDLE hHandle;
    int iPriority;
    DWORD   dwError;

    hHandle = GetCurrentThread();

    iPriority = GetThreadPriority(hHandle);

    if (iPriority == THREAD_PRIORITY_ERROR_RETURN) {
        dwError = GetLastError();
		PifLog(MODULE_PIF_DISABLE, LOG_ERROR_PIFTASK_CODE_03);
        PifLog(MODULE_PIF_DISABLE, (USHORT)dwError);
        PifAbort(FAULT_AT_PIFDISABLE, FAULT_SYSTEM);
        return (USHORT)PIF_ERROR_SYSTEM;
    }

    /* set thread priority as critical */
    if (SetThreadPriority(hHandle, THREAD_PRIORITY_TIME_CRITICAL) == 0) {
        dwError = GetLastError();
		PifLog(MODULE_PIF_DISABLE, LOG_ERROR_PIFTASK_CODE_04);
        PifLog(MODULE_PIF_DISABLE, (USHORT)dwError);
        PifAbort(FAULT_AT_PIFDISABLE, FAULT_SYSTEM);
        return (USHORT)PIF_ERROR_SYSTEM;
    }
    return (USHORT)iPriority;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRestore(usFlags)                    **
**                                                                  **
**  Description:This function restore execution of calling threads  **
**              in the current                                      **
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifRestore(USHORT usFlags) /* for the Recovery Manager */
{
    HANDLE hHandle;
    DWORD   dwError;

    /* reset thread priority */
    hHandle = GetCurrentThread();

    if (SetThreadPriority(hHandle, THREAD_PRIORITY_TIME_CRITICAL) == 0) {
        dwError = GetLastError();
		PifLog(MODULE_PIF_DISABLE, LOG_ERROR_PIFTASK_CODE_05);
        PifLog(MODULE_PIF_DISABLE, (USHORT)dwError);
        PifAbort(FAULT_AT_PIFRESTORE, FAULT_SYSTEM);
    }
    UNREFERENCED_PARAMETER(usFlags);
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   ULONG PIFENTRY PifGetThreadHandle(DWORD dwThradId)  **
**                                                                  **
**  return:     thread id                                           **
**                                                                  **
**  Description: return thread id                                   **     
**                                                                  **
**********************************************************************
fhfh*/
ULONG PIFENTRY PifGetThreadHandle(ULONG ulThreadId)
{
    HANDLE hHandle;
    int i;

    EnterCriticalSection(&g_TaskCriticalSection);
    for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].dwThreadId == ulThreadId) break;
    }

    hHandle = aPifTaskTable[i].hHandle;
    LeaveCriticalSection(&g_TaskCriticalSection);

    return (ULONG)hHandle;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifEndThread(VOID)                   **
**                                                                  **
**  return:     none                                               **
**                                                                  **
**  Description: exit thread                                        **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY  PifClearTaskTable(ULONG ulThreadId)
{
    int i;

    EnterCriticalSection(&g_TaskCriticalSection);
    for (i = 1; i < PIF_MAX_TASK; i++) {
        if (aPifTaskTable[i].dwThreadId == ulThreadId) {
	        aPifTaskTable[i].fThread = FALSE;
	    }
    }
    LeaveCriticalSection(&g_TaskCriticalSection);

    return;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifTaskTableInit(VOID)                         **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description: initialize task table                              **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifTaskTableInit(VOID)
{
    int i;

    for (i = 0; i < PIF_MAX_TASK; i++) {

        aPifTaskTable[i].fThread = FALSE;
    }

    /* set default thread name, for main window */
    aPifTaskTable[0].dwThreadId = GetCurrentThreadId();
    aPifTaskTable[0].fThread = TRUE;
    aPifTaskTable[0].szThreadName[0] = _T('N');
    aPifTaskTable[0].szThreadName[1] = _T('O');
    aPifTaskTable[0].szThreadName[2] = _T(' ');
    aPifTaskTable[0].szThreadName[3] = _T('N');
    aPifTaskTable[0].szThreadName[4] = _T('A');
    aPifTaskTable[0].szThreadName[5] = _T('M');
    aPifTaskTable[0].szThreadName[6] = _T('E');
    aPifTaskTable[0].szThreadName[7] = 0;

    aPifTaskTable[1].szThreadName[0] = _T('T');
    aPifTaskTable[1].szThreadName[1] = _T('E');
    aPifTaskTable[1].szThreadName[2] = _T('M');
    aPifTaskTable[1].szThreadName[3] = _T('P');
    aPifTaskTable[1].szThreadName[4] = 0;
}

/*fhfh
*****************************************************************************
*
**  Synopsis:   VOID PifSubFtoA(PIFDEF pRetAddr, UCHAR *pauchMsg);
*               pRetAddr:   far pointer
*               pauchMsg:   pointer to the top of ASCII area
*
**  Return:     ASCII returned into pauchMsg
*
** Description: This function converts the far pointer to the ASCII string
*               as follows:
*
*                   'cscs:ipip'     cscs: segment, ipip: offset
*
*****************************************************************************
fhfh*/
VOID   PifSubFtoA(PIFDEF pRetAddr, UCHAR *pauchMsg)
{
    RETADDR uRetAddr;

    uRetAddr.pRetAddr = pRetAddr;
    PifSubPtoA(&uRetAddr.uchByte[3], &pauchMsg[0], 1); /* set high segment */
    PifSubPtoA(&uRetAddr.uchByte[2], &pauchMsg[2], 1); /* set low segment */
    pauchMsg[4] = ':';
    PifSubPtoA(&uRetAddr.uchByte[1], &pauchMsg[5], 1); /* set high offset */
    PifSubPtoA(&uRetAddr.uchByte[0], &pauchMsg[7], 1); /* set low offset */
}

/*fhfh
*****************************************************************************
*
**  Synopsis:   VOID PifSubPtoA(UCHAR *pauchPPacked, UCHAR *pauchMsg,
*                               USHORT usSizePacked);
*               pauchPPacked:   pointer to the top of packed decimal
*               pauchMsg:       pointer to the top of ASCII area
*               usSizePacked:   size of packed decimal (no. of byte)
*
**  Return:     ASCII returned into pauchMsg
*
** Description: This function converts the specified unsigned packed decimal
*               to the ASCII string. If pauchPPacked is binary, this function
*               also converts to ASCII string.
*
*****************************************************************************
fhfh*/
VOID   PifSubPtoA(UCHAR *pauchPPacked, UCHAR *pauchMsg, USHORT usSizePacked)
{
    USHORT  i, usWork;
    USHORT  j = 0;

    for (i=0; i<usSizePacked; i++) {
        usWork = (USHORT)(((USHORT)(pauchPPacked[i]) >> 4) + 0x30);
        if (usWork>0x39) usWork += 0x07;
        pauchMsg[j++] = (UCHAR)usWork;
        usWork = (USHORT)(((USHORT)(pauchPPacked[i]) & 0x0F) + 0x30);
        if (usWork>0x39) usWork += 0x07;
        pauchMsg[j++] = (UCHAR)usWork;
    }
}

/*fhfh
*****************************************************************************
*
**  Synopsis:   VOID PifSubPrint(UCHAR *pauchPrtMsg);
*               pauchPrtMsg:    pointer to the print message 
*
**  Return:     None
*
** Description: This function prints the specified message at the printer
*               connected the serial communication port #0.
*
*****************************************************************************
fhfh*/
VOID   PifSubPrint(UCHAR *pauchPrtMsg, USHORT usLen)
{
	extern SHORT  sPifPrintHandle;    /* for PifAbort.  set in PifOpenCom() under some circumstances */
    UCHAR         szCR[] = "\n";

    if (sPifPrintHandle >= 0) {
        PifWriteCom(sPifPrintHandle, pauchPrtMsg, usLen);
        PifWriteCom(sPifPrintHandle, szCR, sizeof(szCR));
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifCreatePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifCreateAbortSem(VOID)
{
    sPifAbortReqSem = PifCreateSem(0);
    sPifAbortRelSem = PifCreateSem(0);
    
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifRequestPowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifRequestAbortSem(TCHAR FAR *pszDisplay)
{
    if ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0)) {
		PifRequestSem(sPifAbortReqSem);
		_tcsncpy(pszDisplay, szPifAbortDisplay, PIF_ABORT_DISPLAY_LEN);
	}
	else {
		NHPOS_ASSERT(!"PifRequestAbortSem(): ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0))");
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifReleasePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifReleaseAbortSem(VOID)
{
    if ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0)) {
		/* send to Ack to power down */
		PifReleaseSem(sPifAbortRelSem);
	}
	else {
		NHPOS_ASSERT(!"PifReleaseAbortSem(): ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0))");
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleatePowerSem(VOID)
**                                                                  **
**  return:                                                        **
**                                                                  **
**  Description:                                                    **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifDeleteAbortSem(VOID)
{
    PifDeleteSem(sPifAbortReqSem);
    PifDeleteSem(sPifAbortRelSem);
    
    sPifAbortReqSem = -1;
    sPifAbortRelSem = -1;
}

VOID PifSubInitAbortSem(VOID)
{
    sPifAbortReqSem = -1;
    sPifAbortRelSem = -1;
}

VOID PifSubSendAbortEvent(TCHAR *pszDisplay)
{
    if ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0)) {
        _tcsncpy(szPifAbortDisplay, pszDisplay, PIF_ABORT_DISPLAY_LEN);
        PifReleaseSem(sPifAbortReqSem);
    }
	else {
		NHPOS_ASSERT(!"PifSubSendAbortEvent(): ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0))");
	}
}

VOID PifSubWaitForAbortAck(VOID)
{
    if ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0)) {
        PifRequestSem(sPifAbortRelSem);
    }
	else {
		NHPOS_ASSERT(!"PifSubWaitForAbortAck(): ((sPifAbortReqSem >= 0) && (sPifAbortRelSem >= 0))");
	}
}

/*fhfh
*****************************************************************************
*
**  Synopsis:   VOID PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode);
*
**  Return:     None
*
** Description: 
*
*****************************************************************************
fhfh*/

static VOID  PifLogBase (USHORT usModuleId, USHORT usExceptionCode)
{
    TCHAR wszFilePathName[MAX_PATH];
    DWORD dwThreadId, dwReturn, dwNumberOfBytes = 0;
    TCHAR  wszLog[64+1];
    CHAR    szLog[64+1];
    USHORT  i;
    DATE_TIME DateTime;
    HANDLE  hHandle;
    LONG   ulPosition;
    UINT   uiLine = 0;
    UINT   uiMaxLine;

    if (fsFaultDisp == TRUE) return;	/* not log after fault */

    wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifLogPathName, LOG_FILE);

    hHandle = CreateFile(wszFilePathName,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (hHandle != INVALID_HANDLE_VALUE) {
        /* read log file header */
        dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_BEGIN);
        if (dwReturn != 0xFFFFFFFF) { 
            ReadFile(hHandle, &szLog, (DWORD)LOG_HEADLINE, &dwNumberOfBytes, NULL);
        }

        if (dwNumberOfBytes) {
			memset(wszLog, 0, sizeof(wszLog));	/* 09/04/01 */
            mbstowcs(wszLog, szLog, sizeof(wszLog)/2);

            if (swscanf(wszLog, TEXT("%04u,%04u"), &uiLine, &uiMaxLine) == 0) {
                uiLine = 0;
            }
        }

        /* write log file header */
        uiLine++;
        if (uiLine > LOG_MAX_LINE) uiLine = 1;

        wsprintf(wszLog, TEXT("%04u,%04u,\n"), uiLine, LOG_MAX_LINE);
		memset(szLog, 0, sizeof(szLog));	/* 09/04/01 */
        wcstombs(szLog, wszLog, sizeof(szLog));

        dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_BEGIN);

        if (dwReturn != 0xFFFFFFFF) { 
            WriteFile(hHandle, szLog, (DWORD)LOG_HEADLINE, &dwNumberOfBytes, NULL);
        }
        
        /* write log information */
        dwThreadId = GetCurrentThreadId();
        for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {
            if (aPifTaskTable[i].dwThreadId == dwThreadId) break;
        }

        if (i >= PIF_MAX_TASK) {
            i = PIF_TASK_TEMP_THREAD;    /* default thread name */
            aPifTaskTable[i].dwThreadId = GetCurrentThreadId();
        }

        PifGetDateTime(&DateTime);
    
        wsprintf(wszLog, TEXT("%8S,%04d,%04d,%02d/%02d/%02d,%02d:%02d:%02d,\n"), aPifTaskTable[i].szThreadName, usModuleId, (usExceptionCode%10000),
            DateTime.usMonth, DateTime.usMDay, DateTime.usYear%100, DateTime.usHour, DateTime.usMin, DateTime.usSec);
		memset(szLog, 0, sizeof(szLog));	/* 09/04/01 */
        wcstombs(szLog, wszLog, sizeof(szLog));

        ulPosition = LOG_HEADLINE + ((uiLine -1)* LOG_LOGLINE);

        dwReturn = SetFilePointer(hHandle, ulPosition, NULL, FILE_BEGIN);

        if (dwReturn != 0xFFFFFFFF) { 
            WriteFile(hHandle, szLog, (DWORD)LOG_LOGLINE, &dwNumberOfBytes, NULL);
			_RPT1(_CRT_WARN, "PIFLOG: %s", szLog);
        }

        CloseHandle(hHandle);
    }
}

VOID   PIFENTRY PifLogExtended (USHORT usModuleId, USHORT usExceptionCode, char *szFilePath, ULONG ulLineNo)
{

	PifLogBase (usModuleId, usExceptionCode);

	if (usModuleId < 1000) {
		char  xBuff[128];

		sprintf (xBuff, "==PIFLOG:  Module %4d  Code %4d",  usModuleId, usExceptionCode);
		PifLogNoAbort( (UCHAR *)  "PifLogExtended()" , (UCHAR *) szFilePath , (UCHAR *) xBuff, ulLineNo);
	}
}

#if defined(PifLog)
#undef PifLog
#endif
VOID   PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode)
{
	PifLogBase (usModuleId, usExceptionCode);

	if (usModuleId < 1000) {
		char  xBuff[128];

		sprintf (xBuff, "==PIFLOG:  Module %4d  Code %4d",  usModuleId, usExceptionCode);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
}

// 
// 
/*fhfh
***********************************************************************
**                                                                   **
**  Synopsis:   VOID PIFENTRY PifLogAbort(UCHAR *lpCondition,		 **
**		 UCHAR *lpFilename, UCHAR *lpFunctionname,					 **
**				 ULONG ulLineNo)             						 **
**																	 **
**			lpCondition:	Reason for Assert to Fail				 **
**			lpFilename:		File name where the Assert Failed		 **
**			lpFunctionName: Function Name of Failed Assert (Not Yet  **
**							Implemented)							 **
**			ulLineNo		Line Number in File to find Failed		 **
**							Assert									 **
**																	 **
**  return:     none                                                 **
**                                                                   **
**  Description: Used by the NHPOS_ASSERT macros as defined in PIF.H **
**               to provide a log of an abort condition.			 **
**																	 **
**	updated: 07-01-04		JHH										 **
***********************************************************************
fhfh*/
ULONG   PIFENTRY PifLogNoAbort(UCHAR *lpCondition, UCHAR *lpFilename, UCHAR *lpFunctionname, ULONG ulLineNo)
{
    DWORD      dwThreadId, dwReturn, dwNumberOfBytes = 0;
    CHAR       szLog1[512];
    USHORT     i, usRetryCount = 5, usLen;
	SYSTEMTIME  DateTime;
    HANDLE     hHandle;
    static DWORD  dwFileSeek = 0;  //Static information for location to write the file

	if (lpCondition == 0 && lpFilename == 0 && lpFunctionname == 0) {
		DWORD  dwFileSeekSave = dwFileSeek;

		// provide a mechanism to initialize the file offset without adding to the
		// PIF.DLL list of entry points.  We do this so that we can add to the file
		// over restarts of the terminal application.
		if (ulLineNo != 0xffffffff)
			dwFileSeek = ulLineNo;
		return dwFileSeekSave;
	}

	do {
		hHandle = CreateFile(_T("\\FlashDisk\\NCR\\Saratoga\\Database\\ASSRTLOG"),
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ, 
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);

		if (hHandle == INVALID_HANDLE_VALUE) {
			Sleep (50);
		}
		usRetryCount--;
	} while ( (hHandle == INVALID_HANDLE_VALUE) && (usRetryCount > 0));

	if (hHandle != INVALID_HANDLE_VALUE) {
	    /* write log information */

        dwThreadId = GetCurrentThreadId();
        for (i = PIF_TASK_FIRST_THREAD_INDEX; i < PIF_MAX_TASK; i++) {

            if (aPifTaskTable[i].dwThreadId == dwThreadId) break;
        }
        if (i >= PIF_MAX_TASK) {
            i = PIF_TASK_TEMP_THREAD;    /* default thread name */
            aPifTaskTable[i].dwThreadId = GetCurrentThreadId();
        }

		if (lpFilename) {
			usLen = strlen (lpFilename);
			if (usLen > 28) {
				usLen -= 28;
			} else {
				usLen = 0;
			}
		} else {
			usLen = 0;
			lpFilename = "";
		}

		GetLocalTime(&DateTime);
		//format the string to tell the time, date, thread name
		//format the string so that it shows the condition that caused the fault, the filename
		//that it cane be found in, and the line number
        sprintf(szLog1, "%8.8s -> 0x%4.4x, %04d,%04d,%02d/%02d/%02d,%02d:%02d:%02d.%4.4d, %-50.50s, %28.28s, %5d, %-120.120s\n", aPifTaskTable[i].szThreadName, dwThreadId, 0, 0,
            DateTime.wMonth, DateTime.wDay, DateTime.wYear%100, DateTime.wHour, DateTime.wMinute, DateTime.wSecond, DateTime.wMilliseconds,
			lpCondition, lpFilename + usLen, ulLineNo, lpFunctionname);

		{
			DWORD dwFileSizeLow, dwFileSizeHigh;
			dwFileSizeLow = GetFileSize (hHandle, &dwFileSizeHigh);

			if (dwFileSeek > dwFileSizeLow) dwFileSeek = dwFileSizeLow;

			if( dwFileSeek >= PIF_ASSERT_FILE_MAX)
			{
				//if the file seek pointer is past the allotted size for writing, we will restart
				dwFileSeek = 0;
			}

			dwReturn = SetFilePointer(hHandle, dwFileSeek, NULL, FILE_BEGIN);
		}

		//if there is not an error
        if (dwReturn != 0xFFFFFFFF) { 
			//write the formatted string
            WriteFile(hHandle, szLog1, strlen(szLog1), &dwNumberOfBytes, NULL);
			//add the number of bytes written to the Seek pointer so that it will find the appropriate place 
			//to write next.
			dwFileSeek += dwNumberOfBytes;
			_RPT1(_CRT_WARN, "ASSRTLOG: %s", szLog1);
        }

        CloseHandle(hHandle);
	}

	return dwFileSeek;
}

VOID   PIFENTRY PifTransactionLog(UCHAR *lpCondition, UCHAR *lpTransactionLog, UCHAR *lpFilename, ULONG ulLineNo)
{
    TCHAR wszFilePathName[MAX_PATH];
    DWORD dwThreadId, dwReturn, dwNumberOfBytes = 0;
    TCHAR  wszLog[256];
    CHAR    szLog[LOGTRANS_LOGLINE+1];
    USHORT  usRetryCount = 5;
    SYSTEMTIME  DateTime;
    HANDLE  hHandle;
    LONG   ulPosition;
    UINT   uiLine = 0;
    UINT   uiMaxLine;

    wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifLogPathName, _T("TRANSLOG"));
	do {
		hHandle = CreateFile(wszFilePathName,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);

		if (hHandle == INVALID_HANDLE_VALUE) {
			Sleep (50);
		}
		usRetryCount--;
	} while ( (hHandle == INVALID_HANDLE_VALUE) && (usRetryCount > 0));

    if (hHandle != INVALID_HANDLE_VALUE) {
		int   iLen = 0;  // __FILE__ pathname index to shorten pathname to 30 characters

        /* read log file header */
        dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_BEGIN);
        if (dwReturn != 0xFFFFFFFF) { 
            ReadFile(hHandle, &szLog, (DWORD)LOG_HEADLINE, &dwNumberOfBytes, NULL);
        }

        if (dwNumberOfBytes) {
			memset(wszLog, 0, sizeof(wszLog));	/* 09/04/01 */
            mbstowcs(wszLog, szLog, sizeof(wszLog)/2);

            if (swscanf(wszLog, TEXT("%05u,%05u"), &uiLine, &uiMaxLine) == 0) {
                uiLine = 0;
            }
        }

        /* write log file header */
        uiLine++;
        if (uiLine > LOGTRANS_MAX_LINE) uiLine = 1;

        sprintf(szLog, "%05u,%05u,\n", uiLine, LOGTRANS_MAX_LINE);
        dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_BEGIN);

        if (dwReturn != 0xFFFFFFFF) { 
            WriteFile(hHandle, szLog, (DWORD)LOGTRANS_HEADLINE, &dwNumberOfBytes, NULL);
        }
        
        /* write log information */
        dwThreadId = GetCurrentThreadId();

        GetLocalTime (&DateTime);
    
		iLen = strlen (lpFilename);
		if (iLen > 20) { iLen -= 20; } else { iLen = 0; }
        sprintf(szLog, "%02d:%02d:%02d|%02d:%02d:%02d.%04d|%-20.20s|%06d|%-120.120s",
            DateTime.wMonth, DateTime.wDay, DateTime.wYear%100, DateTime.wHour, DateTime.wMinute, DateTime.wSecond, DateTime.wMilliseconds,
			lpFilename + iLen, ulLineNo, lpTransactionLog);

		iLen = strlen(szLog);
		if (iLen < LOGTRANS_LOGLINE) {
			for (iLen--; iLen < LOGTRANS_LOGLINE; iLen++) {
				szLog[iLen] = ' ';
			}
		}
		szLog[LOGTRANS_LOGLINE - 1] = '\n';
		szLog[LOGTRANS_LOGLINE] = 0;
        ulPosition = LOGTRANS_HEADLINE + ((uiLine - 1)* LOGTRANS_LOGLINE);

        dwReturn = SetFilePointer(hHandle, ulPosition, NULL, FILE_BEGIN);

        if (dwReturn != 0xFFFFFFFF) { 
            WriteFile(hHandle, szLog, (DWORD)iLen, &dwNumberOfBytes, NULL);
			_RPT1(_CRT_WARN, "TRANSLOG %s", szLog);
        }

        CloseHandle(hHandle);
    }
}


VOID   PIFENTRY PifLogAbort(UCHAR *lpCondition, UCHAR *lpFilename, UCHAR *lpFunctionname, ULONG ulLineNo)
{
	PifLogNoAbort (lpCondition, lpFilename, lpFunctionname, ulLineNo);
#if  defined(_DEBUG) || defined(DEBUG)
	__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
#endif
}

// 
// 
/*fhfh
***********************************************************************
**                                                                   **
**  Synopsis:   VOID   PIFENTRY PifDebugDump (UCHAR *aszTag,         **
**								UCHAR *aszFileName, INT nLineNo,     **
**								VOID *pStruct, INT nSizeOf);         **
**																	 **
**			lpCondition:	Reason for Assert to Fail				 **
**			lpFilename:		File name where the Assert Failed		 **
**			lpFunctionName: Function Name of Failed Assert (Not Yet  **
**							Implemented)							 **
**			ulLineNo		Line Number in File to find Failed		 **
**							Assert									 **
**																	 **
**  return:     none                                                 **
**                                                                   **
**  Description: Used by the NHPOS_ASSERT macros as defined in PIF.H **
**               to provide a log of an abort condition.			 **
**																	 **
**	updated: 07-01-04		JHH										 **
***********************************************************************
fhfh*/
VOID   PIFENTRY PifDebugDump (UCHAR *aszTag, UCHAR *aszFileName, int nLineNo, VOID *pStruct, int nSizeOf)
{
    DWORD dwReturn, dwNumberOfBytes = 0;
	//CHAR	fileName[40], structureName[64];
    USHORT  lengthOfFileName = 0;
	PIFDEBUGDUMP debugDump, readDump;
	PIFDEBUGFILEHEAD fileHead;
    DATE_TIME DateTime;
    HANDLE  hHandle;
	UCHAR  *charCounter;
    static DWORD  dwFileSeek = 0;  //Static information for location to write the file
	ULONG	ulBytesRead, ulBytesSeek = 0;
	SHORT sRet = 1;

	memset(&fileHead, 0x00, sizeof(PIFDEBUGFILEHEAD));
	memset(&debugDump,0x00, sizeof(PIFDEBUGDUMP));

    hHandle = CreateFile(_T("\\FlashDisk\\NCR\\Saratoga\\Database\\DEBUGDMP"),
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ, 
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0);

	if (hHandle) {
	    /* write log information */

        PifGetDateTime(&DateTime);
		//format the string to tell the time, date, thread name
        sprintf(debugDump.dateOfCapture, "%02d/%02d/%02d,%02d:%02d:%02d",    DateTime.usMonth, DateTime.usMDay, DateTime.usYear%100, DateTime.usHour, DateTime.usMin, DateTime.usSec);
//		memset(szLog, 0, sizeof(szLog));	/* 09/04/01 */
  //      wcstombs(szLog, wszLog, sizeof(szLog));

		strncpy(debugDump.structureName, aszTag, sizeof(debugDump.structureName));

		charCounter = aszFileName;

		while(*charCounter != 0x00)
		{
			*charCounter++;
			lengthOfFileName++;
		}

		if( lengthOfFileName > 39)
		{
			aszFileName += (lengthOfFileName - 39);
		}

		strncpy(debugDump.fileName, aszFileName, sizeof(debugDump.fileName)-1);

		debugDump.iLineNumber = nLineNo;

		debugDump.iSizeofData = nSizeOf;


		//if the file seek pointer is past the allotted size for writing, we will restart
		if( dwFileSeek >= PIF_ASSERT_FILE_MAX)
		{
			dwFileSeek = 0;
		}
		
		//if dwFileSeek is 0, then we will write at the beginning of the file else
		if (! dwFileSeek)
		{
			dwReturn = SetFilePointer(hHandle, 0, NULL, FILE_BEGIN);
		} else
		// we write to the designated section of the file
		{

			dwReturn = SetFilePointer(hHandle, dwFileSeek, NULL, FILE_BEGIN);
		}

		while (sRet != SUCCESS)
		{
			memset(&readDump,0x00, sizeof(PIFDEBUGDUMP));
			dwReturn = ReadFile(hHandle, &readDump, sizeof(PIFDEBUGDUMP), &ulBytesRead, NULL);

			if(!readDump.iSizeofData)
			{
				dwReturn = WriteFile(hHandle, &debugDump, sizeof(PIFDEBUGDUMP), &dwNumberOfBytes, NULL);
				dwReturn = WriteFile(hHandle, pStruct, nSizeOf, &dwNumberOfBytes, NULL);
				sRet = SUCCESS;
			}else
			{
				SetFilePointer(hHandle, readDump.iSizeofData, NULL, FILE_CURRENT);
				
			}

		}

        CloseHandle(hHandle);
	}

#if  defined(_DEBUG) || defined(DEBUG)

	__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm

#endif
}
/*fhfh
***********************************************************************
**                                                                   **
**  Synopsis:   VOID   PIFENTRY PIFENTRY PifDebugFileCleanUp(VOID)   **
**																	 **
**																	 **
**																	 **
**  return:     none                                                 **
**                                                                   **
**  Description: Used to clean up the debug file		 			 **
**																	 **
**	updated: 10-14-05		JHH										 **
***********************************************************************
fhfh*/
SHORT  PIFENTRY PifDebugFileCleanUp(VOID)
{

	TCHAR *myFileName = _T("DD*"); // find all DD files
	DWORD dwHandle;
	DWORD viSize = 0;
	TCHAR			backupName[MAX_PATH],backupName2[MAX_PATH], infoBuffer2[2][MAX_PATH];
	WIN32_FIND_DATA myFoundFile;
	HANDLE fileSearch;
	BOOL doit = TRUE;
	USHORT	myCounter = 2;
	TCHAR		versionNumber[40];
	INT		dateCheck, check = 0;
	DATE_TIME   dtCur;
	TCHAR		name[5];
	HANDLE  hHandle;
	UCHAR	dataArea[1024];
	SHORT       hsFileHandle;
	ULONG		ulBytesRead = sizeof(dataArea);
	ULONG		ulBytesWritten = 0;

	memset(dataArea, 0x00, sizeof(dataArea));

	hsFileHandle = PifOpenFile(_T("DEBUGDMP"), "or");

	if(hsFileHandle <= 0)
	{
		return SUCCESS;
	}

	memset(backupName, 0x00, sizeof(backupName));
	_tchdir(STD_FOLDER_DATABASEFILES);
	
	doit = TRUE;

	fileSearch = FindFirstFile (myFileName, &myFoundFile);
	
	//this while loop will continue as long as there are .exe in the directory
	while (myCounter && doit && fileSearch != INVALID_HANDLE_VALUE) 
	{
		//clean the buffer so we dont get erroneous data 
		memset(&versionNumber, 0x00, sizeof(versionNumber));
		//the next two functions get the file name, we must call the first one
		// in order to get the name, i dunno why, ask MSDN.
		viSize = GetFileVersionInfoSize (myFoundFile.cFileName, &dwHandle);
		memcpy(&infoBuffer2[2 - myCounter], myFoundFile.cFileName, sizeof(infoBuffer2[2 - myCounter]));

		//find the next file, if there is not one, doit will be FALSE
		doit = FindNextFile (fileSearch, &myFoundFile);
		myCounter --;
	}
	
	FindClose (fileSearch);

	//check to see which one is the newest file
	if(!myCounter)
	{
		dateCheck = _tcsncmp(infoBuffer2[0], infoBuffer2[1], 8);

		if( dateCheck > 0)
		{
			PifDeleteFile(infoBuffer2[1]);

		}else if (dateCheck < 0)
		{
			PifDeleteFile(infoBuffer2[0]);
		}
	}

	/*set up the new file name*/
    //Get the current time according to the terminal
	PifGetDateTime(&dtCur);
	//set up our backup debug dump file name
	name[0] = szDebugFileYear[dtCur.usYear - 2005];
	name[1] = szDebugFileMonth[dtCur.usMonth - 1];
	name[2] = szDebugFileDay[dtCur.usMDay - 1];
	name[3] = szDebugFileHour[dtCur.usHour];
	name[4] = 0;

	wsprintf(backupName, TEXT("DD%1c%1c%1c%1c%2d"),    
		name[0],name[1],name[2],name[3], dtCur.usMin);
	
	/*end setting up file name*/

    wsprintf(backupName2, TEXT("\\FlashDisk\\NCR\\Saratoga\\Database\\%s"), backupName);

    hHandle = CreateFile(backupName2, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	SetFilePointer(hHandle, 0L, NULL, FILE_BEGIN);

	while(ulBytesRead)
	{
		PifReadFile(hsFileHandle, &dataArea, sizeof(dataArea), &ulBytesRead);
		check += sizeof(dataArea);
		//PifSeekFile(hsFileHandle, sizeof(dataArea), &ulBytesRead);
		WriteFile(hHandle, dataArea, ulBytesRead, &ulBytesWritten, NULL);
	}

	CloseHandle(hHandle);
	PifCloseFile(hsFileHandle);

	PifDeleteFile(_T("DEBUGDMP"));
	return 1;
}
//Family of functions that keep track of time differentials for 
//determining the length of time it takes for certain things 
//to finish 
typedef struct
{
	SYSTEMTIME  stBeginTime;
	SYSTEMTIME	stEndTime;
	BOOL		inUse;
} DateTimeTracker, *pDateTimeTracker;


static DateTimeTracker	myTimeTracker[20] = {0,0,0,0,0,
											0,0,0,0,0,
											0,0,0,0,0,
											0,0,0,0,0};
/*fhfh
*****************************************************************************
*
**  Synopsis:   USHORT PIFENTRY PifOpenTimeTracker(VOID)
*
**  Return:     None
*
** Description: This function finds the next available array location to keep
*				track of time JHHJ
*
*****************************************************************************
fhfh*/

SHORT   PIFENTRY PifOpenTimeTracker(VOID)
{
	USHORT i = 0;

	while((myTimeTracker[i].inUse != 0) && (i < 20))
	{
		i++;
	}
	if(i >= 20)
	{
		return -1;		
	}
	myTimeTracker[i].inUse = TRUE;
	return i;

}
/*fhfh
*****************************************************************************
*
**  Synopsis:   USHORT PIFENTRY PifStartTimeTracker(USHORT usTimeIndex)
*
**  Return:     None
*
** Description: 
*
*****************************************************************************
fhfh*/

USHORT   PIFENTRY PifStartTimeTracker(USHORT usTimeIndex)
{

	if(usTimeIndex <= 20)
	{
		GetLocalTime(&myTimeTracker[usTimeIndex].stBeginTime);
	}
	return 1;

}
/*fhfh
*****************************************************************************
*
**  Synopsis:   USHORT PIFENTRY PifEndTimeTracker(USHORT usTimeIndex)
*
**  Return:     None
*
** Description: 
*
*****************************************************************************
fhfh*/

USHORT   PIFENTRY PifEndTimeTracker(USHORT usTimeIndex)
{
	if(usTimeIndex <= 20)
	{
		GetLocalTime(&myTimeTracker[usTimeIndex].stEndTime);
	}else
	{
		return 1;
	}
	return 1;

}
/*fhfh
*****************************************************************************
*
**  Synopsis:   USHORT PIFENTRY PifTimeTrackerGetDiff(USHORT usTimeIndex)
*
**  Return:     None
*
** Description: 
*
*****************************************************************************
fhfh*/
ULONG   PIFENTRY PifTimeTrackerGetDiff(USHORT usTimeIndex)
{
    FILETIME fileTime1;
       // Stores second instance of time
    FILETIME fileTime2;
       // Stores results of time arithmetic
    LARGE_INTEGER uLargeIntegerTimeRESULT;
// Used for testing the result of conversions
    ULONG usDiff;
	BOOL	result;

	
	//myTimeTracker[usTimeIndex]

  // Conversion from SYSTEMTIME structure to FILETIME structure
	  result = SystemTimeToFileTime(&myTimeTracker[usTimeIndex].stBeginTime, &fileTime1);
	  result = SystemTimeToFileTime(&myTimeTracker[usTimeIndex].stEndTime, &fileTime2);

	  uLargeIntegerTimeRESULT = *(LARGE_INTEGER*)&fileTime2;
	  uLargeIntegerTimeRESULT.QuadPart -= ((LARGE_INTEGER *)&fileTime1)->QuadPart;
	  usDiff = (ULONG)(uLargeIntegerTimeRESULT.QuadPart/10000);
	  
	  return usDiff;

}
/*fhfh
*****************************************************************************
*
**  Synopsis:   USHORT PIFENTRY PifTimeTrackerGetDiff(USHORT usTimeIndex)
*
**  Return:     None
*
** Description: 
*
*****************************************************************************
fhfh*/

USHORT   PIFENTRY PifCloseTimeTracker(USHORT usTimeIndex)
{

	memset(&myTimeTracker[usTimeIndex], 0x00, sizeof(myTimeTracker[usTimeIndex]));

	return 1;

}
/* end of piftask.c */
