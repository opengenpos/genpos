/* pif.c : Defines the entry point for the DLL application. */

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "scf.h"
#include "piflocal.h"
// #include "notify.h"

//CRITICAL_SECTION    g_HandleCriticalSection;

//PIFHANDLETABLE  aPifHandleTable[PIF_MAX_HANDLE];

SHORT       sPifPrintHandle = -1;    /* for PifAbort.  set in PifOpenCom() under some circumstances */

#define  PIFTABLESIGNATURE  0x1F2E3D4C

extern CRITICAL_SECTION g_TaskCriticalSection;
extern CRITICAL_SECTION g_SemCriticalSection;
extern CRITICAL_SECTION g_QueCriticalSection;

extern  PIFHANDLETABLE   aPifFileTable[PIF_MAX_FILE];
extern  PIFHANDLETABLE   aPifSioTable[PIF_MAX_SIO];
extern CRITICAL_SECTION g_FileCriticalSection;
extern CRITICAL_SECTION g_SioCriticalSection;

//---------------------------------------------------------------------------------------
//
//    WHEN CREATING A NEW PIF FUNCTION, MAKE SURE TO ADD TO PIF.DEF TO AVOID UNRESEOLVED EXTERNALS JHHJ
//
//---------------------------------------------------------------------------------------

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   BOOL APIENTRY DllMain(HANDLE hHandle,              **
**                                          DWORD ul_reason_for_call, **
**                                          LPVOID lpReserved)      **
**              hModule:                                            **
**              ul_reason_for_call:                                 **
**              lpReserved:                                         **
**                                                                  **
**  return:     number of bytes to be read                          **
**                                                                  **
**  Description:reading file                                        **     
**                                                                  **
**********************************************************************
fhfh*/
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
#ifdef DEBUG_PIF_OUTPUT
			wsprintf(wszDisplay, TEXT("Pif.c  DllMain DLL_PROCESS_ATTACH hit.\n"));
			OutputDebugString(wszDisplay);
#endif
            PifInitialize();
            break;

        case DLL_PROCESS_DETACH:
#ifdef DEBUG_PIF_OUTPUT
			wsprintf(wszDisplay, TEXT("Pifnet.c  DllMain DLL_PROCESS_DETACH hit.\n"));
			OutputDebugString(wszDisplay);
#endif
			PifFinalize();
            break;

        case DLL_THREAD_ATTACH:
#ifdef DEBUG_PIF_OUTPUT
			wsprintf(wszDisplay, TEXT("Pif.c  DllMain DLL_THREAD_ATTACH hit. Thread:%x\n"), GetCurrentThreadId ());
			OutputDebugString(wszDisplay);
#endif
			break;

        case DLL_THREAD_DETACH:
#ifdef DEBUG_PIF_OUTPUT
			wsprintf(wszDisplay, TEXT("Pif.c  DllMain DLL_THREAD_DETACH hit. Thread:%x\n"), GetCurrentThreadId ());
			OutputDebugString(wszDisplay);
#endif
			break;
    }
    return TRUE;
}


/* Functions of Potable Platform Interface */

/* --- Time --- */
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSleep(USHORT usMsec)              **
**              usMsec         system suspend time                  **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:suspend system by time                              **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifSleep(USHORT usMsec)
{

    Sleep((DWORD)usMsec);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifGetDateTime(DATE_TIME  *pDateTime) **
**              pDateTime:         date/time buffer                 **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:read local date/time                                **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifGetDateTime(DATE_TIME *pDateTime)
{
	TIME_ZONE_INFORMATION TimeZoneInformation;
    SYSTEMTIME  SystemTime;
	DWORD       dwTimeZoneType = 0;

    GetLocalTime(&SystemTime);

	memset (pDateTime, 0, sizeof(DATE_TIME));

    pDateTime->usYear = SystemTime.wYear;
    pDateTime->usMonth = SystemTime.wMonth;
    pDateTime->usWDay = SystemTime.wDayOfWeek;
    pDateTime->usMDay = SystemTime.wDay;
    pDateTime->usHour = SystemTime.wHour;
    pDateTime->usMin = SystemTime.wMinute;
    pDateTime->usSec = SystemTime.wSecond;
    /* SystemTime.wMilliseconds */

	// Get the timezone information.  Used with Amtrak.
	dwTimeZoneType = GetTimeZoneInformation(&TimeZoneInformation);

	if (dwTimeZoneType == TIME_ZONE_ID_STANDARD) {
		_tcsncpy (pDateTime->wszTimeZoneName, TimeZoneInformation.StandardName, 30);
	}
	else if (dwTimeZoneType == TIME_ZONE_ID_DAYLIGHT) {
		_tcsncpy (pDateTime->wszTimeZoneName, TimeZoneInformation.DaylightName, 30);
	}
	else {
	}

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetDateTime(DATE_TIME FAR *pDateTime) **
**              pDateTime:         date/time buffer                 **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:set local date/time                                **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifSetDateTime(CONST DATE_TIME FAR *pDateTime)
{
    DWORD dwError;
    SYSTEMTIME  SystemTime;

    SystemTime.wYear = pDateTime->usYear;
    SystemTime.wMonth = pDateTime->usMonth;
    SystemTime.wDayOfWeek = pDateTime->usWDay;
    SystemTime.wDay = pDateTime->usMDay;
    SystemTime.wHour = pDateTime->usHour;
    SystemTime.wMinute = pDateTime->usMin;
    SystemTime.wSecond = pDateTime->usSec;
    SystemTime.wMilliseconds = 0;

	// The system uses UTC internally. Therefore, when you call 
	// SetLocalTime, the system uses the current time zone information 
	// to perform the conversion, including the daylight saving time 
	// setting. Note that the system uses the daylight saving time setting 
	// of the current time, not the new time you are setting. Therefore, 
	// to ensure the correct result, call SetLocalTime a second time, now 
	// that the first call has updated the daylight saving time setting. 
    if ((SetLocalTime(&SystemTime) == 0) ||
		(SetLocalTime(&SystemTime) == 0)) {

        dwError = GetLastError();
    }

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifGetTickCount(pulElapsedTimeInMsec) **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:                                                    **
**    This function retrieves the number of milliseconds that have  **
**    elapsed since the system (WinCE) was started.                 **
**    Added by DollerTree SCER (2000-11-17)                         **
**********************************************************************
fhfh*/
VOID PIFENTRY   PifGetTickCount( ULONG FAR* pulElapsedTimeInMsec )
{
    /* --- determine pointer passed by user is valid or not --- */

    if ( pulElapsedTimeInMsec != NULL )
    {
        /* --- OK, I will tell you how long time has elapsed --- */

        *pulElapsedTimeInMsec = GetTickCount();

        /* NOTE:
            The elapsed time is stored as a DWORD value. Therefore,
            the time will wrap around to zero if the system is run
            continuously for 49.7 days.
        */
    }
}

SYSCONFIG CONST FAR * PIFENTRY PifSysConfig(VOID)
{

    return &SysConfig;
}

VOID   PIFENTRY PifTrackKey(UCHAR uchData)
{
    return;

    uchData = uchData;
}


/*** PIF Sub Functions ***/


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSubHandleTableInit(PIFHANDLETABLE *pPifHandleTable,
**                                          USHORT usMaxNumber)      **
**              pPifHandleTable         PIF Handle Table Buffer     **
**              usMaxNumber             size of pPifHandleTable     **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:initialize pif handle table                          **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifSubHandleTableInit(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
    USHORT i;

    for (i = 0; i < usMaxNumber; i++) {
		pPifHandleTable->ulSignature = PIFTABLESIGNATURE;
        pPifHandleTable->sPifHandle = -1;
        pPifHandleTable->hWin32Handle = INVALID_HANDLE_VALUE;
        pPifHandleTable->hWin32Socket = INVALID_SOCKET;
        pPifHandleTable++;
    }
}

VOID PifSnapShotFileTableFH (SHORT  shFile)
{
	if (shFile >= 0) {
		PIFHANDLETABLE *pPifHandleTableSave = aPifFileTable;
		USHORT usMaxNumber = sizeof(aPifFileTable)/sizeof(aPifFileTable[0]);
		USHORT i;
		CHAR aszWriteLine[256];

		for (i = 0; i < usMaxNumber; i++) {
			if (pPifHandleTableSave->sPifHandle != -1) {
				sprintf (aszWriteLine, "i = %3.3d 0x%8.8x %S %d\n", i, pPifHandleTableSave->ulSignature, pPifHandleTableSave->wszDeviceName, pPifHandleTableSave->sPifHandle);
				PifWriteSnapShotFile (shFile, aszWriteLine);
			}
			pPifHandleTableSave++;
		}
	}
}

VOID PifSnapShotFileTable (TCHAR *pSnapFileName, CHAR *pcSourcePath, int iLineNo, CHAR *aszHeader)
{
	SHORT  shFile = 0;

	shFile = PifOpenSnapShotFile(pSnapFileName, pcSourcePath, iLineNo);

	if (shFile >= 0) {
		if (aszHeader) {
			PifWriteSnapShotFile (shFile, aszHeader);
		}
		PifSnapShotFileTableFH (shFile);
		PifCloseSnapShotFile (shFile);
	}
}

VOID PifSnapShotCommTableFH (SHORT  shFile)
{
	if (shFile >= 0) {
		PIFHANDLETABLE *pPifHandleTableSave = aPifSioTable;
		USHORT usMaxNumber = sizeof(aPifFileTable)/sizeof(aPifFileTable[0]);
		USHORT i;
		CHAR aszWriteLine[256];

		for (i = 0; i < usMaxNumber; i++) {
			if (pPifHandleTableSave->sPifHandle != -1) {
				sprintf (aszWriteLine, "i = %3.3d 0x%8.8x %S %d\n", i, pPifHandleTableSave->ulSignature, pPifHandleTableSave->wszDeviceName, pPifHandleTableSave->sPifHandle);
				PifWriteSnapShotFile (shFile, aszWriteLine);
			}
			pPifHandleTableSave++;
		}
	}
}

VOID PifSnapShotCommTable (TCHAR *pSnapFileName, CHAR *pcSourcePath, int iLineNo, CHAR *aszHeader)
{
	SHORT  shFile = 0;

	shFile = PifOpenSnapShotFile(pSnapFileName, pcSourcePath, iLineNo);

	if (shFile >= 0) {
		if (aszHeader) {
			PifWriteSnapShotFile (shFile, aszHeader);
		}
		PifSnapShotCommTableFH (shFile);
		PifCloseSnapShotFile (shFile);
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSubGetNewId(PIFHANDLETABLE *pPifHandleTable,
**                                          USHORT usMaxNumber)      **
**              pPifHandleTable         PIF Handle Table Buffer     **
**              usMaxNumber             size of pPifHandleTable     **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:search empty handle value                           **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PifSubGetNewId(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	SHORT  sIndex = 0;
    USHORT i;
	PIFHANDLETABLE *pPifHandleTableSave = pPifHandleTable;

    for (i = 0; i < usMaxNumber; i++) {
        if (pPifHandleTable->sPifHandle == -1) {
			if (i == (usMaxNumber * 3) / 4) {
				PifLog (FAULT_AT_PIFOPENFILE, FAULT_INVALID_ENVIRON_2);
			}
            return i;
        }
        pPifHandleTable++;
    }
	// We have run out of resources.  Lets do a dump of the table to capture what has happened.
	PifLog (FAULT_AT_PIFOPENFILE, FAULT_INVALID_ENVIRON_1);

	sIndex = PifOpenSnapShotFile(_T("SNAP0001"), __FILE__, __LINE__);

	if (sIndex >= 0) {
		CHAR aszWriteLine[256];

		for (i = 0; i < usMaxNumber; i++) {
			if (pPifHandleTableSave->sPifHandle != -1) {
				sprintf (aszWriteLine, "i = %3.3d %S\n", i, pPifHandleTableSave->wszDeviceName);
				PifWriteSnapShotFile (sIndex, aszWriteLine);
			}
			pPifHandleTableSave++;
		}

		PifCloseSnapShotFile (sIndex);
	}

    return (USHORT)PIF_ERROR_HANDLE_OVERFLOW;
}

// The following function performs an extra step of setting the sPifHandle to a special
// value indicating that it is reserved. We are using this reservation step for those
// PIF functions that need to get a resource id and reserve it because they enter a
// critical region, call this function to get a resource id and reserve it, and then
// exit the critical region in order to use this table entry with some action which
// may result in a delay.
// An example of such usage is in PifNetOpenEx() in which a TCP connect request may
// be to a device, e.g. LAN kitchen printer, which is not operational so the TCP connect
// request will time out however that takes a number of seconds and during that time
// we sit in the critical region blocking any other threads from network activity.
//      Richard Chambers, Jul-09-2020
USHORT PifSubGetNewIdReserve(PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	SHORT  sHandle = PifSubGetNewId(pPifHandleTable, usMaxNumber);

	if (sHandle != PIF_ERROR_HANDLE_OVERFLOW) {
		pPifHandleTable[sHandle].sPifHandle = -2;    // reserve this entry
	}

	return sHandle;
}

VOID PifSubGetNewIdReserveClear(USHORT usHandle, PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
	if (usHandle != PIF_ERROR_HANDLE_OVERFLOW && pPifHandleTable[usHandle].sPifHandle == -2) {
		pPifHandleTable[usHandle].sPifHandle = -1;    // unreserve this entry
	}
}
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSubSearchId(USHORT usHandle
**                                      PIFHANDLETABLE *pPifHandleTable,
**                                          USHORT usMaxNumber)      **
**              usHandle                PIF Handle value
**              pPifHandleTable         PIF Handle Table Buffer     **
**              usMaxNumber             size of pPifHandleTable     **
**                                                                  **
**  return:     PIF handle value                                    **
**                                                                  **
**  Description:search handle value whether already opened or not   **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT PifSubSearchId(USHORT usHandle, PIFHANDLETABLE *pPifHandleTable, USHORT usMaxNumber)
{
    if (usHandle >= usMaxNumber) {
        return -1;
    }

    if (pPifHandleTable[usHandle].sPifHandle != usHandle) {
        return -2;
    }

    return usHandle;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifInitialize(VOID)
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:initialize routine of DLL_PROCESS_ATTACH           **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifInitialize(VOID)
{
    PifTableInitialize();
    PifSubInitAbortSem();
    PifSetDiskNamePath();
    PifCheckAndCreateDirectory();
    /* --- Delete *.chk files to save disk space (01/24/2001) --- */
    PifDeleteChkFile();
    PifNetStartup();
    PifSetSysConfig();
    PifOpenSpeaker();
    PifStartRecoveryThread();

    sPifPrintHandle = -1;   /* for PifAbort. set in PifOpenCom() under some circumstances */
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifTableInitialize(VOID)
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:initialize each pif table                           **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifTableInitialize(VOID)
{

    InitializeCriticalSection(&g_SemCriticalSection);
    PifSemInitialize();

    InitializeCriticalSection(&g_QueCriticalSection);
    PifQueueInitialize();

    InitializeCriticalSection(&g_TaskCriticalSection);
    PifTaskTableInit();

    InitializeCriticalSection(&g_SioCriticalSection);
    PifSubHandleTableInit(aPifSioTable, PIF_MAX_SIO);

    InitializeCriticalSection(&g_FileCriticalSection);
    PifSubHandleTableInit(aPifFileTable, PIF_MAX_FILE);

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifFinalize(VOID)
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:finalize routine of DLL_PROCESS_DETTACH             **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifFinalize(VOID)
{
	// The Windows API function ::GetModuleHandleEx () may be needed to pin
	// the Pif.dll module into memory and prevent calls to FreeLibrary() to
	// unloading the dll.
	// See ::GetModuleHandleEx (GET_MODULE_HANDLE_EX_FLAG_PIN, L"pif.dll", &hMod);
	//
	// Do not do any kind of a cleanup with PifFinalize() and processing a
	// DLL_PROCESS_DETACH message in the DllMain(). We will let Windows clean up
	// for us. We want the PIF services and functionality to be avalailable until
	// the final, bitter end of GenPOS exiting.
	//    Richard Chambers, Mar-12-2018
	//
	return;
    PifCloseSpeaker();
    PifStopRecoveryThread();
//    PifCloseHandle(FALSE);
    DeleteCriticalSection(&g_FileCriticalSection);
    DeleteCriticalSection(&g_SioCriticalSection);
    PifNetFinalize();
    DeleteCriticalSection(&g_QueCriticalSection);
//  PifDeleteSemAll();
    DeleteCriticalSection(&g_SemCriticalSection);
}
#if 0
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifCloseHandle(USHORT usPowerDown)
**              usPowerDown:    0: finalize process
**                              1: power down recovery process
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:close all device handle at finalize  **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifCloseFileAll(VOID)
{
    int i;

    EnterCriticalSection(&g_FileCriticalSection);

    for (i=0; i<PIF_MAX_FILE; i++) {

        if (aPifFileTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {

            /* unconditional closing */

            CloseHandle(aPifFileTable[i].hWin32Handle);
            aPifFileTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
        }
    }

    LeaveCriticalSection(&g_FileCriticalSection);
}
#endif
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifCloseHandle(USHORT usPowerDown)
**              usPowerDown:    0: finalize process
**                              1: power down recovery process
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:close each device handle at finalize or power down recovery **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifCloseHandle(USHORT usPowerDown)
{

    PifCloseFileHandle(usPowerDown);
    PifCloseSioHandle(usPowerDown);
    PifNetCloseHandle(usPowerDown);
    
}

/* end of pif.c */
