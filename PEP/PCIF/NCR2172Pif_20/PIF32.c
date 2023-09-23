/////////////////////////////////////////////////////////////////////////////
// PIF32.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Aug-26-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "pif32.h"
#include "net32.h"
#include "debug.h"
#include "AW_Interfaces.h"

#pragma	pack(push, 1)
#include    "XINU.H"    /* XINU                         */
#define     PIFXINU     /* switch for "PIF.H"           */
#include    "R20_PIF.H"     /* Portable Platform Interface  */
#include    "LOG.H"     /* Fault Codes Definition       */
#include    "PIFXINU.H" /* Platform Interface           */
#pragma	pack(pop)

#define PEP_STRING_LEN_MAC(x) (sizeof(x)/sizeof((x)[0]))

/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////

#define PIF32_LEN_MODE			4
#define	PIF32_NSEM				32			// number of semaphore
#define	PIF32_NFDES				31			// number of files / directory
#define	PIF32_FDNLEN			10			// length of file name + 1


// global data local to this file

static CRITICAL_SECTION	g_csPif32Sem;			// for semaphore
static CRITICAL_SECTION	g_csPif32File;			// for file

static struct {									// file information table
	HANDLE	hHandle;
	BOOL	fState;
} aSemTbl[PIF32_NSEM];

static struct {									// file information table
	HANDLE	hHandle;
	WCHAR   wchFileName[48];
	DWORD	dwAccess;
	DWORD	dwCreate;
	BOOL	fState;
} aFileTbl[PIF32_NFDES];

// function prototypes

BOOL	PifIsValidSem(USHORT usSem);
BOOL	PifIsValidFile(USHORT usFile);
extern 	HANDLE      hPepInst;       /* Instance Handle of PEP Window        */

//SHORT PifOpenFileExec(TCHAR *pwszFileName, SHORT fsMode, HANDLE *hHandle);
SHORT PifReadFileExec(HANDLE hFile, VOID FAR *pBuffer, ULONG usBytes, ULONG *pulReadBytes, WCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);
SHORT PifWriteFileExec(HANDLE hFile, CONST VOID FAR *pBuffer, ULONG usBytes, ULONG *pulWriteBytes, WCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);
//SHORT PifSeekFileExec(HANDLE hFile, ULONG ulPosition, ULONG FAR *pulActualPosition, SHORT fsMode, WCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifLog(USHORT usModuleId,             **
**                                   USHORT usExceptionCode);       **
**              usModuleId:         module ID                       **
**              usExceptionCode:    exception code                  **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: logs the specified module ID and exception code     **
**              with time stamp.                                    **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode)
{
    return;

    usModuleId      = usModuleId;
    usExceptionCode = usExceptionCode;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSleep(USHORT usMsec);              **
**              usMsec:     sleep time (unit: mS)                   **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: stops the current thread for the specified time.    **
**                                                                  **
**********************************************************************
fhfh*/
VOID  PIFENTRY PifSleep(USHORT usMsec)
{
	DWORD dwMsec = (DWORD)usMsec;
    Sleep(dwMsec);

    return;    
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

	memset(pDateTime, 0, sizeof(DATE_TIME));

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
		_tcsncpy(pDateTime->wszTimeZoneName, TimeZoneInformation.StandardName, 30);
	}
	else if (dwTimeZoneType == TIME_ZONE_ID_DAYLIGHT) {
		_tcsncpy(pDateTime->wszTimeZoneName, TimeZoneInformation.DaylightName, 30);
	}
	else {
	}

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32Init();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PIFENTRY PifInit()
{
	// allocate critical section

	InitializeCriticalSection(&g_csPif32Sem);
	InitializeCriticalSection(&g_csPif32File);

	// initialzie
	memset(&aSemTbl, 0,  sizeof(aSemTbl));
	memset(&aFileTbl, 0, sizeof(aFileTbl));
	/*ZeroMemory(&aSemTbl,  sizeof(aSemTbl));
	ZeroMemory(&aFileTbl, sizeof(aFileTbl));*/

	NetInit32();

	// exit ...

	return PIF_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32Uninit();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PIFENTRY PifUninit()
{
	int			nIndex;

	NetUninit32();

	// free the file handle

    for (nIndex = 0; nIndex < PIF32_NFDES; nIndex++)
	{
        if (aFileTbl[nIndex].fState)
		{
			CloseHandle(aFileTbl[nIndex].hHandle);
        }
	}

	// free the semaphore handle

    for (nIndex = 0; nIndex < PIF32_NSEM; nIndex++)
	{
        if (aSemTbl[nIndex].fState)
		{
			CloseHandle(aSemTbl[nIndex].hHandle);
        }
	}

	// initialzie
	memset(&aFileTbl, 0, sizeof(aFileTbl));
	//ZeroMemory(&aFileTbl, sizeof(aFileTbl));

	// free critical section

	DeleteCriticalSection(&g_csPif32File);
	DeleteCriticalSection(&g_csPif32Sem);

	// exit ...

	return PIF_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32CreateSem();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

USHORT PIFENTRY PifCreateSem(USHORT usCount)
{
	int		nIndex;
	HANDLE	hHandle;

	EnterCriticalSection(&g_csPif32Sem);

	// get free table

    for (nIndex = 0; nIndex < PIF32_NSEM; nIndex++)
	{
        if (! aSemTbl[nIndex].fState)
		{
            break;
        }
        if (nIndex == PIF32_NSEM)					// short resource
		{
			LeaveCriticalSection(&g_csPif32Sem);
            PifAbortEx(MODULE_PIF_CREATESEM, FAULT_SHORT_RESOURCE, TRUE);
        }
    }

	hHandle = CreateSemaphore(NULL, usCount, 99999, NULL);

	if (hHandle == NULL)
	{
		LeaveCriticalSection(&g_csPif32Sem);
		PifAbortEx(MODULE_PIF_CREATESEM, FAULT_GENERAL, TRUE);
	}

	// save file information

	aSemTbl[nIndex].hHandle = hHandle;
	aSemTbl[nIndex].fState  = TRUE;

	LeaveCriticalSection(&g_csPif32Sem);

	// exit ...

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32DeleteSem();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifDeleteSem(USHORT usSem)
{
	BOOL	fState;

	EnterCriticalSection(&g_csPif32Sem);

	if (usSem < PIF32_NSEM)
	{
		fState = aSemTbl[usSem].fState;		// get handle state
	}
	else
	{
		fState = FALSE;
	}

	// valid handle ?

	if (! fState)
	{
		LeaveCriticalSection(&g_csPif32Sem);
		PifAbortEx(MODULE_PIF_DELETESEM, FAULT_INVALID_HANDLE, TRUE);
	}

	CloseHandle(aSemTbl[usSem].hHandle);

	aSemTbl[usSem].hHandle = NULL;
	aSemTbl[usSem].fState  = FALSE;

	LeaveCriticalSection(&g_csPif32Sem);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32RequestSem();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifRequestSem(USHORT usSem)
{
	// valid handle ?

	if (! PifIsValidSem(usSem))
	{
		PifAbortEx(MODULE_PIF_REQUESTSEM, FAULT_INVALID_HANDLE, TRUE);
	}

	WaitForSingleObject(aSemTbl[usSem].hHandle, INFINITE);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32ReleaseSem();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifReleaseSem(USHORT usSem)
{
	// valid handle ?

	if (! PifIsValidSem(usSem))
	{
		PifAbortEx(MODULE_PIF_RELEASESEM, FAULT_INVALID_HANDLE, TRUE);
	}

	ReleaseSemaphore(aSemTbl[usSem].hHandle, 1, NULL);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		PifIsValidSem();
//
// Inputs:		nothing
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL PifIsValidSem(USHORT usSem)
{
	BOOL	fState;

	EnterCriticalSection(&g_csPif32Sem);

	if (usSem < PIF32_NSEM)
	{
		fState = aSemTbl[usSem].fState;		// get handle state
	}
	else
	{
		fState = FALSE;
	}

	LeaveCriticalSection(&g_csPif32Sem);

	// exit ...

	return fState;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32OpenFile();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PIFENTRY PifOpenFile(CONST WCHAR FAR * pszFileName, CONST UCHAR FAR * pszMode)
{
	DWORD		dwRC;
	DWORD		dwAccess, dwCreate, dwDir = 0;
	HANDLE		hFile;
	int			nIndex;
	WCHAR		szPathName[MAX_PATH];

	EnterCriticalSection(&g_csPif32File);
	DebugPrintf(L"+Pif32OpenFile: File=%s, Mode=%s\r\n", pszFileName, pszMode);

	// get free table

    for (nIndex = 0; nIndex < PIF32_NFDES; nIndex++)
	{
        if (! aFileTbl[nIndex].fState)
		{
            break;
        }
        if (nIndex == PIF32_NFDES)						// short resource
		{
			LeaveCriticalSection(&g_csPif32File);
            PifAbortEx(MODULE_PIF_OPENFILE, FAULT_SHORT_RESOURCE, TRUE);
        }
    }

	// valide mode ?

    if (! PifFileMode(pszMode, &dwAccess, &dwCreate, &dwDir))	// error
	{
		LeaveCriticalSection(&g_csPif32File);
        PifAbortEx(MODULE_PIF_OPENFILE, FAULT_INVALID_MODE, TRUE);
    }

	// make up full path name
		szPathName[0] = L'\0';

#if 1
		GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
		GetPepModuleFileName(NULL, szPathName, MAX_PATH);
		szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
	   /* if (GetWindowsDirectoryPep(szPathName, MAX_PATH) > MAX_PATH - PIF32_FDNLEN)
		{
			LeaveCriticalSection(&g_csPif32File);
			PifAbortEx(MODULE_PIF_OPENFILE, FAULT_GENERAL, TRUE);
		}*/
		wcscat(szPathName, WIDE("\\"));
#endif

		wcsncat(szPathName, pszFileName, PIF32_FDNLEN - 1);

	// create or open file

	hFile = CreateFilePep(szPathName,
					   dwAccess,
					   FILE_SHARE_READ,
					   NULL,
					   dwCreate,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		dwRC = GetLastError();
		LeaveCriticalSection(&g_csPif32File);

		if (dwRC == ERROR_FILE_EXISTS)
		{
			DebugPrintf(L"-Pif32OpenFile: File=%s, PIF_ERROR_FILE_EXIST\r\n", pszFileName);
		    return(PIF_ERROR_FILE_EXIST);
		}
		else if (dwRC == ERROR_FILE_NOT_FOUND)
		{
			DebugPrintf(L"-Pif32OpenFile: File=%s, PIF_ERROR_FILE_NOT_FOUND\r\n", pszFileName);
 		    return(PIF_ERROR_FILE_NOT_FOUND);
		}		
		else
		{
			PifAbortEx(MODULE_PIF_OPENFILE, FAULT_GENERAL, TRUE);
		}
	}

	// save file information

	aFileTbl[nIndex].hHandle  = hFile;
	aFileTbl[nIndex].dwAccess = dwAccess;
	aFileTbl[nIndex].dwCreate = dwCreate;
	aFileTbl[nIndex].fState   = TRUE;
	wcsncpy (aFileTbl[nIndex].wchFileName, pszFileName, sizeof(aFileTbl[0].wchFileName)/sizeof(aFileTbl[0].wchFileName[0]));
	aFileTbl[nIndex].wchFileName[sizeof(aFileTbl[0].wchFileName)/sizeof(aFileTbl[0].wchFileName[0]) - 1] = 0;

	DebugPrintf(L"-Pif32OpenFile: File=%s, Handle=%d\r\n", pszFileName, nIndex);
	LeaveCriticalSection(&g_csPif32File);

	// exit ...

	return nIndex;
}

SHORT  PIFENTRY PifOpenFileNew(CONST WCHAR *pszFileName, CONST CHAR *pszMode, CHAR *pPathName, int iLineNo){
	//took the lazy man's way out and just forward to the old functionality
	return PifOpenFile(pszFileName,(UCHAR*)pszMode);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		PifFileMode();
//
// Inputs:		nothing
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL PIFENTRY PifFileMode(CONST UCHAR FAR *pszMode, DWORD* pdwAccess, DWORD* pdwCreate, DWORD *pdwDir)
{
	CHAR	ch;

	// initialize

	*pdwAccess = 0;
	*pdwCreate = 0;

	for (ch = *pszMode; ch > 0; ch = (*(++pszMode)))
	{
		switch (ch)
		{
		case 'r':
			if (*pdwAccess & GENERIC_READ)
			{
				return FALSE;
			}
			*pdwAccess |= GENERIC_READ;
			break;
		case 'w':
			if (*pdwAccess & GENERIC_WRITE)
			{
				return FALSE;
			}
			*pdwAccess |= GENERIC_WRITE;
			break;
		case 'o':
			if (*pdwCreate & OPEN_EXISTING || *pdwCreate & CREATE_NEW)
			{
				return FALSE;
			}
			*pdwCreate = OPEN_EXISTING;
			break;
		case 'n':
			if (*pdwCreate & OPEN_EXISTING || *pdwCreate & CREATE_NEW)
			{
				return FALSE;
			}
			*pdwCreate = CREATE_NEW;
			break;
		case 'i':
			*pdwDir = ICON_DIR;
			break;
		case 'e':
			if (*pdwCreate & OPEN_EXISTING || *pdwCreate & CREATE_NEW)
			{
				return FALSE;
			}
			*pdwCreate  = OPEN_ALWAYS;
 			break;
		default:
			 return FALSE;
		}
	}

	// default ?  allow R + W

	if (*pdwAccess == 0)
	{
		*pdwAccess |= (GENERIC_READ | GENERIC_WRITE);
	}

	if (*pdwCreate == 0)
	{
		*pdwCreate |= (CREATE_NEW);
	}

	// exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	USHORT		Pif32ReadFile();
//
// Inputs:		nothing
//
// Outputs:		USHORT		usResult;	-
//
/////////////////////////////////////////////////////////////////////////////

USHORT PIFENTRY PifReadFile(USHORT usFile, VOID* pvBuffer, ULONG usBytes)
{
	BOOL		fResult;
	HANDLE		hFile;
	DWORD		dwBytesToRead, dwBytesRead;

	DebugPrintf(L"+Pif32ReadFile: Handle=%hu, Bytes=%hu\r\n", usFile, usBytes);

	// valid handle ?

	if (! PifIsValidFile(usFile))
	{
		PifAbortEx(MODULE_PIF_READFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	hFile         = aFileTbl[usFile].hHandle;
	dwBytesToRead = (DWORD)usBytes;

	fResult = ReadFile(hFile,
					   pvBuffer,
					   dwBytesToRead,
					   &dwBytesRead,
					   NULL);

	// inspect execution status

	if (! fResult)
	{
		DebugPrintf(L"-Pif32ReadFile: Handle=%hu, GetLastError=%d\r\n", usFile, GetLastError());
		return 0;							// error
	}

	// exit ...

	DebugPrintf(L"-Pif32ReadFile: Handle=%hu, BytesRead=%d\r\n", usFile, dwBytesRead);
	return (USHORT)dwBytesRead;
}

SHORT PIFENTRY PifReadFileNew(USHORT usFile, VOID *pBuffer, ULONG ulBytes, ULONG *pulBytesRead, CHAR *pcFilePath, int iLineNo)
{
    HANDLE  hFile;
    SHORT   sReturn = PIF_ERROR_SYSTEM, sStatus = 0;
	ULONG	ulActualBytesRead;
//	char aszErrorBuffer[128];

	if(!pulBytesRead){
		pulBytesRead = &ulActualBytesRead;
	}

	*pulBytesRead = 0;

    EnterCriticalSection(&g_csPif32File);

	if (! PifIsValidFile(usFile)) {
//		sprintf (aszErrorBuffer, "PifReadFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
//		sprintf (aszErrorBuffer, "PifReadFile (): Source File: %s,  Line No: %d", pcFilePath, iLineNo);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_csPif32File);
        PifLog(FAULT_AT_PIFREADFILE, FAULT_INVALID_HANDLE);
        PifLog(FAULT_AT_PIFREADFILE, usFile);
        PifLog(FAULT_AT_PIFREADFILE, (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFREADFILE, FAULT_INVALID_HANDLE);
    } else {
		hFile = aFileTbl[usFile].hHandle;

		sReturn = PifReadFileExec(hFile, pBuffer, ulBytes, pulBytesRead, L"UNKNOWN", pcFilePath, iLineNo);

//not utilized in pep
//		if (*pulBytesRead > 0) {
//			aFileTbl[usFile].ulFilePointer += *pulBytesRead;
//		}
	
		LeaveCriticalSection(&g_csPif32File);
	}
    return sReturn;
}

/* PifReadFile */

SHORT PifReadFileExec(HANDLE hFile, VOID FAR *pBuffer, ULONG usBytes, ULONG *pulBytesRead, WCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo)
{

    DWORD   dwNumberOfBytesRead = 0;
    BOOL    fReturn;
    DWORD   dwError;
//	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifReadFile, %s, %d bytes read\r\n"), pwszFileName, usBytes);
    OutputDebugString(wszDisplay);
#endif

    UNREFERENCED_PARAMETER(pwszFileName);
    fReturn = ReadFile(hFile, pBuffer, (DWORD)usBytes, &dwNumberOfBytesRead, NULL);

    if (fReturn == 0) {
        dwError = GetLastError();
		wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
//		sprintf (aszErrorBuffer, "PifReadFileExec (): File: %s,  hFile: %d,  Error: %d, pBuffer = %p, usBytes = %d ",aszFilePathName, hFile, dwError, pBuffer, usBytes);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
//		sprintf (aszErrorBuffer, "PifReadFileExec (): Source File: %s,  Line No.: %d", pcFilePath, iLineNo);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
//		PifLog(MODULE_PIF_READFILE, LOG_ERROR_CODE_FILE_01);	//no such error in PEP
//		PifLog(MODULE_ERROR_NO(MODULE_PIF_READFILE), (USHORT)dwError);
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("PifReadFile, %s, %d bytes read,  Error = %d\r\n"), pwszFileName, dwNumberOfBytesRead, dwError);
        OutputDebugString(wszDisplay);
#endif
        dwNumberOfBytesRead = 0;
#if 0
#pragma message("---------------    Unconditional file closing activated in pif/piffile.c function PifReadFileExec() \z")
		{
			SHORT   i;

			for (i = 0; i < PIF_MAX_FILE; i++) {
				if (aPifFileTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {
					/* unconditional closing */
					CloseHandle(aPifFileTable[i].hWin32Handle);
					aPifFileTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
				}
			}
			dwNumberOfBytesRead = usBytes; /* for demo */
			PifLog(FAULT_AT_PIFREADFILE, FAULT_SYSTEM);
			PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREADFILE), (USHORT) dwError);
			PifAbort(FAULT_AT_PIFREADFILE, FAULT_SYSTEM);
			*pulBytesRead = dwNumberOfBytesRead;
		}
        return PIF_ERROR_SYSTEM;
#else
        PifLog(FAULT_AT_PIFREADFILE, FAULT_SYSTEM);
//		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREADFILE), (USHORT) dwError);
        PifAbort(FAULT_AT_PIFREADFILE, FAULT_SYSTEM);
		*pulBytesRead = dwNumberOfBytesRead;
        return PIF_ERROR_SYSTEM;
#endif
	}

    *pulBytesRead = dwNumberOfBytesRead;

    return PIF_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		Pif32WriteFile();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifWriteFile(USHORT usFile, CONST VOID* pvBuffer, ULONG usBytes)
{
	BOOL		fResult;
	HANDLE		hFile;
	DWORD		dwBytesToWrite, dwBytesWritten;

	DebugPrintf(L"+Pif32WriteFile: Handle=%hu, Bytes=%hu\r\n", usFile, usBytes);

	// valid handle ?

	if (! PifIsValidFile(usFile))
	{
 		PifAbortEx(MODULE_PIF_READFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	hFile          = aFileTbl[usFile].hHandle;
	dwBytesToWrite = (DWORD)usBytes;

	fResult = WriteFile(hFile,
						pvBuffer,
						dwBytesToWrite,
						&dwBytesWritten,
						NULL);

	// inspect execution status

	if (! fResult || dwBytesToWrite != dwBytesWritten)
	{
		DebugPrintf(L"-Pif32WriteFile: Handle=%hu, GetLastError=%d\r\n", usFile, GetLastError());
		return ;							// error
	}

	DebugPrintf(L"-Pif32WriteFile: Handle=%hu, BytesWritten=%d\r\n", usFile, dwBytesWritten);
}

VOID   PIFENTRY PifWriteFileNew(USHORT usFile, CONST VOID *pBuffer, ULONG ulBytes, CHAR *pcFilePath, int iLineNo)
{
    HANDLE  hFile;
    ULONG   ulBytesWrite = 0;
    SHORT   sReturn, sStatus = 0;
//	char aszErrorBuffer[128];

    EnterCriticalSection(&g_csPif32File);

    if (! PifIsValidFile(usFile)) {
//		sprintf (aszErrorBuffer, "PifWriteFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
//		sprintf (aszErrorBuffer, "PifWriteFile (): Source File: %s,  Line No: %d", pcFilePath, iLineNo);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_csPif32File);
        PifLog(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        PifLog(FAULT_AT_PIFWRITEFILE, usFile);
        PifLog(FAULT_AT_PIFWRITEFILE, (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        return;
    }
    
	hFile = aFileTbl[usFile].hHandle;

    sReturn =  PifWriteFileExec(hFile, pBuffer, ulBytes, &ulBytesWrite, L"UNKNOWN", pcFilePath, iLineNo);

//	pep doesn't have this functionality
//	if (ulBytesWrite > 0) {
//		aPifFileTable[usFile].ulFilePointer += ulBytesWrite;
//	}

    LeaveCriticalSection(&g_csPif32File);
}

/* PifWriteFile */

SHORT   PifWriteFileExec(HANDLE hFile, CONST VOID FAR *pBuffer, ULONG usBytes, ULONG *pulBytesWrite, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo)
{
    BOOL    fReturn;
    DWORD   dwNumberOfBytesWrite = 0;
    DWORD   dwError;
//	char aszErrorBuffer[128+MAX_PATH];  // max path name length plus extra 128 for description text
//	char aszFilePathName[MAX_PATH];
#if DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifWriteFile, %s, %d bytes write\r\n"), pwszFileName, usBytes);
    OutputDebugString(wszDisplay);
#endif
    UNREFERENCED_PARAMETER(pwszFileName);

    fReturn = WriteFile(hFile, pBuffer, (DWORD)usBytes, &dwNumberOfBytesWrite, NULL);

    if (!fReturn) {
        dwError = GetLastError();
//		sprintf (aszErrorBuffer, "PifWriteFileExec (): Source File: %s,  Line No: %d", pcFilePath, iLineNo);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
//		wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
//		sprintf (aszErrorBuffer, "PifWriteFileExec (): File: %s,  hFile: %d, dwError: %d, pBuffer = %p, usBytes = %d ",aszFilePathName, hFile, dwError, pBuffer, usBytes);
//		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
//		PifLog(MODULE_PIF_WRITEFILE, LOG_ERROR_CODE_FILE_01);	//error code not in PEP
//		PifLog(MODULE_ERROR_NO(MODULE_PIF_WRITEFILE), (USHORT)dwError);
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("WriteFile, %s, %d bytes write, dwError = %d\r\n"), pwszFileName, dwNumberOfBytesWrite, dwError);
        OutputDebugString(wszDisplay);
#endif

        dwNumberOfBytesWrite = 0;
#if 0
#pragma message("---------------    Unconditional file closing activated in pif/piffile.c function PifWriteFileExec() \z")
		{
			SHORT   i;

			for (i = 0; i < PIF_MAX_FILE; i++) {
				if (aPifFileTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {
					/* unconditional closing */
					CloseHandle(aPifFileTable[i].hWin32Handle);
					aPifFileTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
				}
			}
			dwNumberOfBytesWrite = usBytes; /* for demo */
			PifLog(FAULT_AT_PIFWRITEFILE, FAULT_SYSTEM);
			PifLog(MODULE_ERROR_NO(FAULT_AT_PIFWRITEFILE), (USHORT) dwError);
			PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_SYSTEM);
			*pulBytesWrite = dwNumberOfBytesWrite;
		}
        return PIF_ERROR_SYSTEM;
#else
        PifLog(FAULT_AT_PIFWRITEFILE, FAULT_SYSTEM);
//		PifLog(MODULE_ERROR_NO(FAULT_AT_PIFWRITEFILE), (USHORT) dwError);
        PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_SYSTEM);
		*pulBytesWrite = dwNumberOfBytesWrite;
        return PIF_ERROR_SYSTEM;
#endif
    }

    *pulBytesWrite = dwNumberOfBytesWrite;

    return PIF_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32SeekFile();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PIFENTRY PifSeekFile(USHORT usFile, ULONG ulPosition, ULONG* pulActualPosition)
{
	HANDLE		hFile;
	DWORD		dwRet, dwNewSize, dwCurSize;

	DebugPrintf(L"+Pif32SeekFile: Handle=%hu, Pos=%u\r\n", usFile, ulPosition);

	// valid handle ?

	if (! PifIsValidFile(usFile))
	{
		PifAbortEx(MODULE_PIF_READFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	hFile     = aFileTbl[usFile].hHandle;
	dwNewSize = (DWORD)ulPosition;
	dwCurSize = GetFileSize(hFile, NULL);

	if (dwNewSize <= dwCurSize)
	{
		dwRet = SetFilePointer(hFile, dwNewSize, NULL, FILE_BEGIN);

		if (dwRet == -1)
		{
			PifAbortEx(MODULE_PIF_SEEKFILE, FAULT_GENERAL, TRUE);
		}

		*pulActualPosition = (ULONG)dwRet;

		DebugPrintf(L"-Pif32SeekFile: Handle=%hu, PIF_OK\r\n", usFile);
		return PIF_OK;						// exit ...
	}

	// is write attribute ?

	else if (aFileTbl[usFile].dwAccess & GENERIC_WRITE)
	{
		// expand the file size

		dwRet = SetFilePointer(hFile, dwNewSize, NULL, FILE_BEGIN);

		if (dwRet == -1)
		{
			if (GetLastError() == ERROR_DISK_FULL)
			{
				*pulActualPosition = 0L;

				DebugPrintf(L"-Pif32SeekFile: Handle=%hu, PIF_ERROR_FILE_DISK_FULL\r\n", usFile);
				return PIF_ERROR_FILE_DISK_FULL;
			}
			else
			{
				PifAbortEx(MODULE_PIF_SEEKFILE, FAULT_GENERAL, TRUE);
			}
		}

		if (! SetEndOfFile(hFile))
		{
			PifAbortEx(MODULE_PIF_SEEKFILE, FAULT_GENERAL, TRUE);
		}

		*pulActualPosition = (ULONG)dwRet;

		DebugPrintf(L"-Pif32SeekFile: Handle=%hu, PIF_OK\r\n", usFile);
		return PIF_OK;
	}

	// is read only ?

	else
	{
		dwRet = SetFilePointer(hFile, dwCurSize, NULL, FILE_BEGIN);

		if (dwRet == -1)
		{
			PifAbortEx(MODULE_PIF_SEEKFILE, FAULT_GENERAL, TRUE);
		}

		*pulActualPosition = (ULONG)dwRet;

		DebugPrintf(L"-Pif32SeekFile: Handle=%hu, PIF_ERROR_FILE_EOF\r\n", usFile);
		return PIF_ERROR_FILE_EOF;			// exit ...
	}
}

SHORT  PIFENTRY PifSeekFileNew(USHORT usFile, ULONG ulPosition, ULONG *pulActualPosition, CHAR *pcFilePath, int iLineNo)
{
	//took the lazy man's way out and just forward to the old functionality
    return PifSeekFile(usFile,ulPosition,pulActualPosition);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   ULONG PIFENTRY PifGetFileSize( USHORT usFile        **
**                            ULONG FAR *pulFileSizeHigh)           **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**********************************************************************
fhfh*/
ULONG   PIFENTRY PifGetFileSize(USHORT usFile, ULONG FAR* pulFileSizeHigh)
{
    HANDLE  hFile;
    ULONG   ulFileSize;

    EnterCriticalSection(&g_csPif32File);

	if (! PifIsValidFile(usFile))
	{
		PifAbortEx(MODULE_PIF_READFILE, FAULT_INVALID_HANDLE, TRUE);
	}
    
    hFile = aFileTbl[usFile].hHandle;

    ulFileSize = GetFileSize(hFile, pulFileSizeHigh);

    LeaveCriticalSection(&g_csPif32File);

    return ulFileSize;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32ControlFile();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PifControlFile(USHORT usFile, USHORT usFunc, ...)
{
	HANDLE		hFile;
	va_list		marker;
	LONG		ulPosition;
	DWORD		dwRet, dwNewSize;

	// valid handle ?

	if (! PifIsValidFile(usFile))
	{
		PifAbortEx(MODULE_PIF_CONTROLFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	switch (usFunc)
	{
	case PIF_FILE_CHANGE_SIZE:				// change file size
		va_start(marker, usFunc);
		ulPosition = va_arg(marker, LONG);
		va_end(marker);

		DebugPrintf(L"+Pif32ControlFile: Handle=%hu, Pos=%u\r\n", usFile, ulPosition);

		hFile     = aFileTbl[usFile].hHandle;
		dwNewSize = (DWORD)ulPosition;
		dwRet     = SetFilePointer(hFile, dwNewSize, NULL, FILE_BEGIN);

		if (dwRet == -1)
		{
			if (GetLastError() == ERROR_DISK_FULL)
			{
				DebugPrintf(L"-Pif32ControlFile: Handle=%hu, PIF_ERROR_FILE_DISK_FULL\r\n", usFile);
				return PIF_ERROR_FILE_DISK_FULL;
			}
			else
			{
				PifAbortEx(MODULE_PIF_CONTROLFILE, FAULT_GENERAL, TRUE);
			}
		}

		if (! SetEndOfFile(hFile))
		{
			PifAbortEx(MODULE_PIF_CONTROLFILE, FAULT_GENERAL, TRUE);
		}

		break;
	default:
		PifAbortEx(MODULE_PIF_CONTROLFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	// exit ...

	DebugPrintf(L"-Pif32ControlFile: Handle=%hu, PIF_OK\r\n", usFile);
	return PIF_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		Pif32CloseFile();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifCloseFile(USHORT usFile)
{
	BOOL	fState;

	EnterCriticalSection(&g_csPif32File);
	DebugPrintf(L"+Pif32CloseFile: Handle=%hu\r\n", usFile);

	if (usFile < PIF32_NFDES)
	{
		fState = aFileTbl[usFile].fState;	// get handle state
	}
	else
	{
		fState = FALSE;
	}

	// valid handle ?

	if (! fState)
	{
		LeaveCriticalSection(&g_csPif32File);
		PifAbortEx(MODULE_PIF_CLOSEFILE, FAULT_INVALID_HANDLE, TRUE);
	}

	CloseHandle(aFileTbl[usFile].hHandle);

	aFileTbl[usFile].hHandle  = NULL;
	aFileTbl[usFile].dwAccess = 0;
	aFileTbl[usFile].dwCreate = 0;
	aFileTbl[usFile].fState   = FALSE;

	DebugPrintf(L"-Pif32CloseFile: Handle=%hu\r\n", usFile);
	LeaveCriticalSection(&g_csPif32File);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		Pif32DeleteFile();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifDeleteFile(LPCWSTR lpszFileName)
{
	BOOL		fResult;
	WCHAR		szPathName[MAX_PATH];
	DWORD       dwError;

	DebugPrintf(L"+Pif32DeleteFile: File=%s\r\n", lpszFileName);

	// make up full path name

	PifIsWindowsNT();
	szPathName[0] = '\0';

#if 1
	GetPepTemporaryFolder(NULL, szPathName, PEP_STRING_LEN_MAC(szPathName));
#else
	GetPepModuleFileName(NULL, szPathName, MAX_PATH);
	szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
    /*if (GetWindowsDirectoryPep(szPathName, MAX_PATH) > MAX_PATH - PIF32_FDNLEN)
	{
		PifAbortEx(MODULE_PIF_DELETEFILE, FAULT_GENERAL, TRUE);
    }*/
    wcscat(szPathName, WIDE("\\"));
#endif

    wcsncat(szPathName, lpszFileName, PIF32_FDNLEN - 1);

	fResult = DeleteFilePep(szPathName);

	dwError = GetLastError();  // if dwError == 32  ERROR_SHARING_VIOLATION
	if ((! fResult) && (dwError != ERROR_FILE_NOT_FOUND))
	{
		PifAbort(MODULE_PIF_DELETEFILE, FAULT_GENERAL);
	}

	DebugPrintf(L"-Pif32DeleteFile: File=%s\r\n", lpszFileName);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL		Pif32IsValidFile();
//
// Inputs:		nothing
//
// Outputs:		BOOL		fResult;	-
//
/////////////////////////////////////////////////////////////////////////////

BOOL PifIsValidFile(USHORT usFile)
{
	BOOL	fState;

	EnterCriticalSection(&g_csPif32File);

	if (usFile < PIF32_NFDES)
	{
		fState = aFileTbl[usFile].fState;	// get handle state
	}
	else
	{
		fState = FALSE;
	}

	LeaveCriticalSection(&g_csPif32File);

	// exit ...

	return fState;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	VOID		PifAbortEx();
//
// Inputs:		nothing
//
// Outputs:		nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID PIFENTRY PifAbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl)
{
    static wchar_t achGeneral[]       = L"General failure";
    static wchar_t achTimeout[]       = L"Timeout occured";
    static wchar_t achInvalidData[]   = L"Unrecognized data detected";
    static wchar_t achInvalidFunc[]   = L"This function is not provided";
    static wchar_t achInvalidArg[]    = L"Invalid argument detected";
    static wchar_t achInvalidHandle[] = L"Invalid handle detected";
    static wchar_t achInvalidMode[]   = L"This function detected in invalid mode";
    static wchar_t achBufferOver[]    = L"Buffer overflow occuered";
    static wchar_t achShortResource[] = L"Short resource";
    static wchar_t achBadEnv[]        = L"This function is not provided in the environment";
    static wchar_t achInvalidCause[]  = L"Invalid fault cause code detect";
    static wchar_t achUsedHandle[]    = L"This function is not closed";
    static wchar_t achNotFound[]      = L"Not found the file";
    static wchar_t achAlreadyExist[]  = L"Already exists the file";
    static wchar_t achEof[]           = L"End of the file";
    static wchar_t achDiskFull[]      = L"Disk is full";
    static wchar_t achAccessDenied[]  = L"Access is denied";
    static wchar_t achReadError[]     = L"Receive or read error";
    static wchar_t achWriteError[]    = L"Send or write error";
    static wchar_t achOffline[]       = L"Network is not ready";
    static wchar_t achUnreachable[]   = L"Network address unreachable";
    static wchar_t achMsgErr[]        = L"Invalid message";
	wchar_t    achBuf[64] = {0};
    wchar_t	   *pchErr;

	DebugPrintf(L"*PifAbortEx: GetLastError=%d\r\n", GetLastError());

    if (usFaultModule > MODULE_PIF_SWITCHSTACK || fAppl)
	{
        switch(usFaultCode)
		{
        case FAULT_GENERAL:				pchErr = achGeneral;		break;
        case FAULT_TIMEOUT:				pchErr = achTimeout;		break;
        case FAULT_INVALID_DATA:        pchErr = achInvalidData;	break;
        case FAULT_INVALID_FUNCTION:	pchErr = achInvalidFunc;	break;
        case FAULT_INVALID_ARGUMENTS:	pchErr = achInvalidArg;		break;
        case FAULT_INVALID_HANDLE:		pchErr = achInvalidHandle;	break;
        case FAULT_INVALID_MODE:		pchErr = achInvalidMode;	break;
        case FAULT_BUFFER_OVERFLOW:		pchErr = achBufferOver;		break;
        case FAULT_SHORT_RESOURCE:		pchErr = achShortResource;	break;
        case FAULT_BAD_ENVIRONMENT:		pchErr = achBadEnv;			break;
        case FAULT_USED_HANDLE:			pchErr = achUsedHandle;		break;
        default:						pchErr = achInvalidCause;	break;
        }

    }
	else										// kernel error ?
	{
        if (usFaultCode == (USHORT)abs(SYSERR))
		{
            pchErr = achGeneral;
        }
		else if (usFaultCode == (USHORT)abs(TIMEOUT))
		{
            pchErr = achTimeout;
        }
		else if (usFaultCode == (USHORT)abs(XINU_NOT_FOUND))
		{
            pchErr = achNotFound;
        }
		else if (usFaultCode == (USHORT)abs(XINU_ALREADY_EXIST))
		{
            pchErr = achAlreadyExist;
        }
		else if (usFaultCode == (USHORT)abs(XINU_EOF))
		{
            pchErr = achEof;
        }
		else if (usFaultCode == (USHORT)abs(XINU_DISK_FULL))
		{
            pchErr = achDiskFull;
        }
		else if (usFaultCode == (USHORT)abs(XINU_ACCESS_DENIED))
		{
            pchErr = achAccessDenied;
        }
		else if (usFaultCode == (USHORT)abs(XINU_READ_ERROR))
		{
            pchErr = achReadError;
        }
		else if (usFaultCode == (USHORT)abs(XINU_WRITE_ERROR))
		{
            pchErr = achWriteError;
        }
		else if (usFaultCode == (USHORT)abs(XINU_OFFLINE))
		{
            pchErr = achOffline;
        }
		else if (usFaultCode == (USHORT)abs(XINU_UNREACHABLE))
		{
            pchErr = achUnreachable;
        }
		else if (usFaultCode == (USHORT)abs(XINU_MESSAGE_ERROR))
		{
            pchErr = achMsgErr;
        }
		else
		{
            pchErr = achInvalidCause;
        }
    }

    // check the module ID and the cause code

    if (usFaultModule > 999)	usFaultModule = 0;
    if (usFaultCode > 99)       usFaultCode   = 0;

	// edit error message

    wsprintf(achBuf, L"E%03d%02d : ", usFaultModule, usFaultCode);
    wcscat(achBuf, pchErr);

	// show message box

	MessageBox(NULL, achBuf, L" ", MB_OK | MB_ICONERROR);

	// terminate

#ifdef _DEBUG
	__debugbreak();  // prep for ARM build _asm int 3;
#endif

	FatalExit(0);

	return;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifAbort(USHORT usFaultModule,        **
**                                     USHORT usFaultCode);         **
**              usFaultModule: module ID or function ID which       **
**                             detects fatal error                  **
**              usFaultCode:   cause code of fatal error            **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: Stop the application and output the the error popup.**
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifAbort(USHORT usFaultModule, USHORT usFaultCode)
{
	PifAbortEx(usFaultModule, usFaultCode, TRUE);
}

/*fhfh

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifBeginThread                      **
**                               (VOID (THREADENTRY *pStart)(VOID), **
**                               VOID FAR *pStack,                  **
**                               USHORT usStackSize,                **
**                               USHORT usPriority,                 **
**                               CONST CHAR FAR *pszThreadName, ...)**
**                                                                  **
**          pStart:         pointer to entry point of new thread    **
**          pStack:         pointer to bottom of stack area         **
**          usStackSize:    stack size                              **
**          usPriority:     new thread priority (1-30, 1:low)       **
**          pszThreadName:  new thread name (up to 8 ASCII + null)  **
**          ...:            up to 4 bytes arguments for new thread  **
**                                                                  **
**  Return:     0 (Thread ID)                                       **
**                                                                  **
** Description: create new thread                                   **
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifBeginThread(VOID (THREADENTRY *pStart)(VOID), 
                               VOID FAR *pStack,  USHORT usStackSize,
                               USHORT usPriority, 
                               LPCWSTR *pszThreadName, ...)
{
    PifAbort(MODULE_PIF_BEGINTHREAD, FAULT_BAD_ENVIRONMENT);

    pStart        = pStart;
    pStack        = pStack;
    usStackSize   = usStackSize;
    usPriority    = usPriority;
    pszThreadName = pszThreadName;

    return(0);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifEndThread(VOID);                   **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: end the thread                                      **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifEndThread(VOID)
{
    return;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   UCHAR CONST FAR *PIFENTRY PifGetProcessId(VOID);    **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: get the process ID.                                 **
**                                                                  **
**********************************************************************
fhfh*/
UCHAR CONST FAR * PIFENTRY PifGetProcessId(VOID)
{
    return((UCHAR *)NULL);
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   BOOL PifIsWindowsNT(VOID);    **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: get the process ID.                                 **
**                                                                  **
**********************************************************************
fhfh*/
BOOL PifIsWindowsNT(VOID)
{
	return(IsWindowsNT());
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	SHORT		Pif32OpenFile();
//
// Inputs:		nothing
//
// Outputs:		SHORT		sResult;	-
//
/////////////////////////////////////////////////////////////////////////////

SHORT PIFENTRY PifOpenDirFile(CONST WCHAR FAR * pszPath, CONST WCHAR FAR * pszFileName, CONST UCHAR FAR * pszMode)
{
	DWORD		dwRC;
	DWORD		dwAccess, dwCreate, dwDir = 0;
	HANDLE		hFile;
	int			nIndex;
	WCHAR		szPathName[MAX_PATH];

	EnterCriticalSection(&g_csPif32File);
	DebugPrintf(L"+Pif32OpenFile: File=%s, Mode=%s\r\n", pszFileName, pszMode);

	// get free table

    for (nIndex = 0; nIndex < PIF32_NFDES; nIndex++)
	{
        if (! aFileTbl[nIndex].fState)
		{
            break;
        }
        if (nIndex == PIF32_NFDES)						// short resource
		{
			LeaveCriticalSection(&g_csPif32File);
            PifAbortEx(MODULE_PIF_OPENFILE, FAULT_SHORT_RESOURCE, TRUE);
        }
    }

	// valide mode ?

    if (! PifFileMode(pszMode, &dwAccess, &dwCreate, &dwDir))	// error
	{
		LeaveCriticalSection(&g_csPif32File);
        PifAbortEx(MODULE_PIF_OPENFILE, FAULT_INVALID_MODE, TRUE);
    }

	// make up full path name

	szPathName[0] = L'\0';

	wcscat(szPathName, pszPath);
	//szPathName[wcslen(szPathName)-8] = '\0';  //remove pep.exe from path
   /* if (GetWindowsDirectoryPep(szPathName, MAX_PATH) > MAX_PATH - PIF32_FDNLEN)
	{
		LeaveCriticalSection(&g_csPif32File);
		PifAbortEx(MODULE_PIF_OPENFILE, FAULT_GENERAL, TRUE);
	}*/
	wcscat(szPathName, WIDE("\\"));
	wcsncat(szPathName, pszFileName, wcslen(pszFileName));


	// create or open file

	hFile = CreateFilePep(szPathName,
					   dwAccess,
					   FILE_SHARE_READ,
					   NULL,
					   dwCreate,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		dwRC = GetLastError();
		LeaveCriticalSection(&g_csPif32File);

		if (dwRC == ERROR_FILE_EXISTS)
		{
			DebugPrintf(L"-Pif32OpenFile: File=%s, PIF_ERROR_FILE_EXIST\r\n", pszFileName);
		    return(PIF_ERROR_FILE_EXIST);
		}
		else if (dwRC == ERROR_FILE_NOT_FOUND)
		{
			DebugPrintf(L"-Pif32OpenFile: File=%s, PIF_ERROR_FILE_NOT_FOUND\r\n", pszFileName);
 		    return(PIF_ERROR_FILE_NOT_FOUND);
		}		
		else
		{
			PifAbortEx(MODULE_PIF_OPENFILE, FAULT_GENERAL, TRUE);
		}
	}

	// save file information

	aFileTbl[nIndex].hHandle  = hFile;
	aFileTbl[nIndex].dwAccess = dwAccess;
	aFileTbl[nIndex].dwCreate = dwCreate;
	aFileTbl[nIndex].fState   = TRUE;

	DebugPrintf(L"-Pif32OpenFile: File=%s, Handle=%d\r\n", pszFileName, nIndex);
	LeaveCriticalSection(&g_csPif32File);

	// exit ...

	return nIndex;
}

///////////////////////////////	 End of File  ///////////////////////////////
