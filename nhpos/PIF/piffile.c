/* piffile.c : PIF file control functions */

#pragma warning( disable : 4201 4214 4514)
#include <math.h>

#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"
#include "file.h"
#include "uie.h"

/* --- File --- */

/* for File */
/* internal error code */

#define LEN_FILE_MODE       4

#define F_FILE_OPENING      1
#define F_FILE_OPENED       2
#define F_FILE_NEW          3

CRITICAL_SECTION g_FileCriticalSection;

PIFHANDLETABLE   aPifFileTable[PIF_MAX_FILE];

/* default path */
const TCHAR wszFilePathFlashDisk[] = STD_FOLDER_FLASHROOT;
const TCHAR wszFilePathTempDisk[] =  STD_FOLDER_TEMPROOT;
const TCHAR wszFilePathDataBase[] = TEXT("Database");
const TCHAR wszFilePathLog[] = TEXT("Log");
const TCHAR wszFilePathProgram[] = TEXT("Program");
const TCHAR wszFilePathSavedTotal[] = TEXT("SavedTotal");
const TCHAR wszFilePathPrintFiles[] = TEXT("PrintFiles");
const TCHAR wszFilePathIcon[] = TEXT("Icons");
const TCHAR wszFilePathImages[] = TEXT("Images");
const TCHAR wszFilePathConfig[] = STD_FOLDER_AMTRAK_CONFIG;     // standard path for FLCONFIG as used for Amtrak

TCHAR wszPifDiskPathName[MAX_PATH];
TCHAR wszPifTempPathName[MAX_PATH];
TCHAR wszPifTotalPathName[MAX_PATH];
TCHAR wszPifLogPathName[MAX_PATH];
TCHAR wszPifPathName[MAX_PATH];
TCHAR wszPifSavTotalPathName[MAX_PATH];
TCHAR wszPifIconPathName[MAX_PATH];
TCHAR wszPifPrintPathName[MAX_PATH];
TCHAR wszPifConfigPathName[MAX_PATH];
TCHAR wszPifImagesPathName[MAX_PATH];

/*
 *  See also the defines in file rmt.h which are used by the function RmtCheckOpenMode()
 *  and the RmtFileServer() function. The bit usage is a bit different.
 *
 *  See defines starting with RMT_TEMP.
 *
 *  WARNING: Any changes to these defines should be reflected in a change in the
 *           function dfckmd() which decodes the char string for the file open mode
 *           and returns a bit map of these flags indicating the file open attributes
 *           to use when creating/opening the file.
 *
 *           See also function PifSetDiskNamePath() and function PifCheckAndCreateDirectory()
 *           which may also need changes and updating.
*/
typedef  USHORT FlMode;          /* fl_mode variable type. must be sized sufficient for all following flags */

#define FLREAD          0x0001   /* fl_mode bit for "read"               */
#define FLWRITE         0x0002   /* fl_mode bit for "write"              */
#define FLRW            0x0003   /* fl_mode bits for read+write          */
#define FLDENYSHARE     0x0008   /* fl_mode bits for no share mode       */
#define FLNEW           0x0010   /* fl_mode bit for "new file"           */
#define FLOLD           0x0020   /* fl_mode bit for "old file"           */
#define FLRECREATE      0x0040   /* fl_mode bit for CREATE_ALWAYS, truncate file */
#define FLTMP           0x0100   /* fl_mode bit for "temp file". See also RMT_TEMP  */
#define FLFLASH         0x0200   /* fl_mode bit for "flash file". See also RMT_FLASH  */
#define FLSAVTTL		0x0400	 /* fl_mode bit for "saved total file". See also RMT_SAVTTL  */
#define FLICON			0x0800	 /* fl_mode bit for "icon file". See also RMT_ICON  */
#define FLPRINT 		0x1000	 /* fl_mode bit for "print spool file". See also RMT_PRINTFILES  */
#define FLCONFIG 		0x2000	 /* fl_mode bit for "configuration file"  Amtrak. See also RMT_CONFIG  */
#define FLHISFOLDER		0x4000	 /* fl_mode bit for "historial and reports file"  Rel 2.3.0 cloud. See also RMT_HISFOLDER  */
#define FLIMAGESFOLDER	0x8000	 /* fl_mode bit for "suggestive selling images file". See also RMT_IMAGESFOLDER  */

#define OK              1       /* returned when system call ok         */
#define SYSERR         -1       /* returned if there is an error such as bad arguments */

static FlMode dfckmd(CONST CHAR *mode);

VOID *pPifDatAddress = 0;
ULONG usPifDatSize = 0;

VOID PifSetDiskFreeSpace(VOID);
SHORT  PifOpenFileExec(CONST TCHAR *pwszFileName, FlMode fsMode, HANDLE *hHandle);
/*SHORT  PifOpenFileExec(TCHAR *pwszFileName, CHAR chMode, HANDLE *hHandle);*/
SHORT PifReadFileExec(HANDLE hFile, VOID FAR *pBuffer, ULONG usBytes, ULONG *pulReadBytes, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);
SHORT PifWriteFileExec(HANDLE hFile, CONST VOID  *pBuffer, ULONG usBytes, ULONG *pulWriteBytes, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);
SHORT PifSeekFileExec(HANDLE hFile, ULONG ulPosition, ULONG  *pulActualPosition, SHORT fsMode, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo);
VOID  PifCloseFileExec(HANDLE hFile, TCHAR *pwszFileName);

static CONST TCHAR *  PifGetFilePathLocal (CONST TCHAR *pwszFileName, FlMode fsMode, TCHAR *pwszFilePath)
{
	static  TCHAR  wszFilePathNameStatic[MAX_PATH];

	TCHAR   wszFilePathName[MAX_PATH] = {0};

	if (fsMode & FLTMP) {
		if (pwszFileName)
			wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifTempPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifTempPathName);
    } else if(fsMode & FLSAVTTL) {
		if (pwszFileName)
			wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifSavTotalPathName, pwszFileName);	
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifSavTotalPathName);
	} else if(fsMode & FLICON) {
		if (pwszFileName)
			 wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifIconPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifIconPathName);
	} else if(fsMode & FLPRINT) {
		if (pwszFileName)
			 wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifPrintPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifPrintPathName);
	} else if(fsMode & FLCONFIG) {
		if (pwszFileName)
			 wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifConfigPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifConfigPathName);
	} else if(fsMode & FLIMAGESFOLDER) {
		if (pwszFileName)
			 wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifImagesPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifImagesPathName);
	} else if(fsMode & FLHISFOLDER) {
		CONST SYSCONFIG    *pSysConfig = PifSysConfig();       /* get system config */
		if (pSysConfig->tcsReportsHistoricalFolder[0]) {
			if (pwszFileName)
				wsprintf(wszFilePathName, TEXT("%s\\%s"), pSysConfig->tcsReportsHistoricalFolder, pwszFileName);
			else
				wsprintf(wszFilePathName, TEXT("%s\\"), pSysConfig->tcsReportsHistoricalFolder);
		}
	} else {
		if (pwszFileName)
			wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifDiskPathName, pwszFileName);
		else
			wsprintf(wszFilePathName, TEXT("%s\\"), wszPifDiskPathName);
    }

	if (pwszFilePath)
		return wcscpy (pwszFilePath, wszFilePathName);
	else
		return wcscpy (wszFilePathNameStatic, wszFilePathName);
}

CONST TCHAR *  PIFENTRY PifGetFilePath(CONST TCHAR *pwszFileName, CONST CHAR *pszMode, TCHAR *pwszFilePath)
{
		FlMode  fsMode = dfckmd(pszMode);

		return PifGetFilePathLocal (pwszFileName, fsMode, pwszFilePath);
}

SHORT  PIFENTRY PifGetFileAttribs(CONST TCHAR *pwszFileName, CONST CHAR *pszMode, VOID *pFileAttribs)
{
	WIN32_FILE_ATTRIBUTE_DATA  fileAttribs = {0};
	TCHAR wszFilePathName[MAX_PATH] = {0};
	FlMode  fsMode = dfckmd(pszMode);
	char   xBuff[256];

	PifGetFilePath (pwszFileName, pszMode, wszFilePathName);

	if (!GetFileAttributesEx (wszFilePathName, GetFileExInfoStandard, &fileAttribs)) {
		DWORD dwLastError = GetLastError();

		sprintf (xBuff, "==NOTE: PifGetFileAttribs() %S failed. GetLastError() %d", pwszFileName, dwLastError);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	} else {
		if (pFileAttribs) {
			*((WIN32_FILE_ATTRIBUTE_DATA *)pFileAttribs) = fileAttribs;
		} else {
			// Generate an ASSRTLOG log with the data.
			SYSTEMTIME  stUTC;
			SYSTEMTIME  stLocal;

			sprintf (xBuff, "==NOTE: PifGetFileAttribs() %S", pwszFileName);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

			sprintf (xBuff, "        Size     - %u  Attributes 0x%8.8x", fileAttribs.nFileSizeLow, fileAttribs.dwFileAttributes);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

			FileTimeToSystemTime (&(fileAttribs.ftCreationTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			sprintf (xBuff, "        Created  - %2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%4.4d",
				stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute, stLocal.wSecond, stLocal.wMilliseconds);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

			FileTimeToSystemTime (&(fileAttribs.ftLastWriteTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			sprintf (xBuff, "        Modified - %2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%4.4d",
				stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute, stLocal.wSecond, stLocal.wMilliseconds);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

			FileTimeToSystemTime (&(fileAttribs.ftLastAccessTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			sprintf (xBuff, "        Accessed - %2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%4.4d",
				stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute, stLocal.wSecond, stLocal.wMilliseconds);
			NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

		}
	}

	return 0;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifOpenFile(CONST CHAR FAR *pszFileName **
**                                          CONST CHAR FAR *pszMode) **
**              pszFileName:    file name to open                   **
**              pszMode:        opening method                      **
**                                                                  **
**  return:     handle of file                                      **
**                                                                  **
**  Description:Opening file                                        **     
**                                                                  **
**********************************************************************
fhfh*/

SHORT  PIFENTRY PifOpenFileNew(CONST TCHAR *pszFileName,
                            CONST CHAR *pszMode, CHAR *pcFilePath, int iLineNo)
{
    SHORT   sFile = 0;

    EnterCriticalSection(&g_FileCriticalSection);

	{
		HANDLE  hHandle;
		SHORT   sReturn;
		FlMode  fsMode;

		/* search free ID */
		sFile = PifSubGetNewId(aPifFileTable, PIF_MAX_FILE);
		if (sFile == PIF_ERROR_HANDLE_OVERFLOW) {
			char    aszErrorBuffer[256];
			int  iLen = 0;

			NHPOS_ASSERT_TEXT (0, "PifOpenFile (): Handle Overflow in aPifFileTable");
			iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
			sprintf (aszErrorBuffer, "    File: %S Source File: %s,  Line No: %d", pszFileName, pcFilePath + iLen, iLineNo);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
			LeaveCriticalSection(&g_FileCriticalSection);
			// add new line to end of string for the snapshot file output.
			sprintf (aszErrorBuffer, "    File: %S Source File: %s,  Line No: %d\n", pszFileName, pcFilePath + iLen, iLineNo);
			PifSnapShotFileTable(_T("SNAP0010"), __FILE__, __LINE__, aszErrorBuffer);
			PifLog (FAULT_AT_PIFOPENFILE, FAULT_SHORT_RESOURCE);
			PifAbort (FAULT_AT_PIFOPENFILE, FAULT_SHORT_RESOURCE);
			return PIF_ERROR_SYSTEM;
		}

		fsMode = dfckmd(pszMode);

		if (fsMode == SYSERR) {
			LeaveCriticalSection(&g_FileCriticalSection);
			{
				char    aszErrorBuffer[256];
				int  iLen = 0;

				sprintf (aszErrorBuffer, "**WARNING: PifOpenFile (): Invalid mode arguments  Mode: %s", pszMode);
				NHPOS_ASSERT_TEXT ((fsMode != SYSERR), aszErrorBuffer);
				sprintf (aszErrorBuffer, "    Filename: %S", pszFileName);
				NHPOS_ASSERT_TEXT ((fsMode != SYSERR), aszErrorBuffer);
				iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
				sprintf (aszErrorBuffer, "    Source File: %s,  Line No: %d", pcFilePath + iLen, iLineNo);
				NHPOS_ASSERT_TEXT ((fsMode != SYSERR), aszErrorBuffer);
				PifAbort(FAULT_AT_PIFOPENFILE, FAULT_INVALID_ARGUMENTS);
			}
			return PIF_ERROR_SYSTEM;
		}

		sReturn = PifOpenFileExec(pszFileName, fsMode, &hHandle);
	    
		if (sReturn == PIF_OK) {
			/* set Pif Handle Table */
			aPifFileTable[sFile].sPifHandle = sFile;
			aPifFileTable[sFile].hWin32Handle = hHandle;
			aPifFileTable[sFile].fCompulsoryReset = FALSE;
			wcsncpy(aPifFileTable[sFile].wszDeviceName, pszFileName, PIF_DEVICE_LEN);
			aPifFileTable[sFile].fsMode = fsMode;
			aPifFileTable[sFile].ulFilePointer = 0;

			PifSetDiskFreeSpace();
		} else {
			sFile = sReturn;
		}
	}

    LeaveCriticalSection(&g_FileCriticalSection);

    return sFile;

}

static HANDLE  hSnapShotFileHandles[5] = {0, 0};
static SHORT   sSnapShotFileHandleIndex = 0;

SHORT  PIFENTRY PifOpenSnapShotFile(CONST TCHAR *pszFileName, CHAR *pcSourcePath, int iLineNo)
{
	CONST CHAR *pszMode = "xwr";
    HANDLE     hHandle;
    DATE_TIME  DateTime;
    SHORT      sReturn;
    FlMode     fsMode;
	CHAR       wszFileName[256];

	fsMode = dfckmd(pszMode);
	if (fsMode == SYSERR) {
		NHPOS_ASSERT_TEXT(0, "**ERROR: PifOpenSnapShotFile() open mode error.");
		return -1;
	}

	sReturn = PifOpenFileExec(pszFileName, fsMode, &hHandle);
	if (sReturn == PIF_OK) {
		sprintf (wszFileName, "==SNAP: %S created.", pszFileName);
		NHPOS_NONASSERT_NOTE("==SNAP", wszFileName);
		sSnapShotFileHandleIndex = ((sSnapShotFileHandleIndex + 1) % 5);  // toggle the value in the range of 0 to 4
		hSnapShotFileHandles[sSnapShotFileHandleIndex] = hHandle;

		PifGetDateTime(&DateTime);
		sprintf(wszFileName, "%S: %4.4d/%2.2d/%2.2d  %2.2d:%2.2d:%2.2d\n", pszFileName,
			DateTime.usYear, DateTime.usMonth, DateTime.usMDay, DateTime.usHour, DateTime.usMin, DateTime.usSec);
		PifWriteSnapShotFile(sSnapShotFileHandleIndex, (CHAR *)wszFileName);

		sprintf (wszFileName, "From: %s: Line %d\n", pcSourcePath, iLineNo);

		PifWriteSnapShotFile(sSnapShotFileHandleIndex, (CHAR *)wszFileName);
		return sSnapShotFileHandleIndex;
	}
	else {
		return -1;
	}
}

SHORT  PIFENTRY PifCloseSnapShotFile(SHORT sIndex)
{
    BOOL    fReturn = FALSE;

	if (sIndex >= 0 && hSnapShotFileHandles[sIndex]) {
		fReturn = CloseHandle (hSnapShotFileHandles[sIndex]);
		hSnapShotFileHandles[sIndex] = 0;
	}
	return 0;
}

SHORT  PIFENTRY PifWriteSnapShotFile(SHORT sIndex, CHAR *pszWriteLine)
{
    BOOL    fReturn = FALSE;

	if (sIndex >= 0 && hSnapShotFileHandles[sIndex]) {
		DWORD  dwBytes = strlen (pszWriteLine);
		DWORD  dwNumberOfBytesWrite = 0;

	    fReturn = WriteFile(hSnapShotFileHandles[sIndex], pszWriteLine, dwBytes, &dwNumberOfBytesWrite, NULL);
	}
	return 0;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifReadFile(USHORT usFile            **
**                                          VOID FAR *pBuffer       **
**                                          ULONG ulBytes           **
**											ULONG *pulBytesRead		**
**              usFile:         file handle                         **
**              pBuffer:        reading buffer                      **
**              ulBytes:        sizeof pBuffer                      **
**				pulBytesRead	actual bytes read					**
**                                                                  **
**  return:     PIF_ERROR_SYSTEM									**
**              PIF_OK                                              **
**																	**
**  Description:reading file                                        **     
**                                                                  **
	PifReadFile used to return the size of the Read, but now only 
	returns a condition/error.
	The size of the Read is now passed back through a pointer in
	the argument list (pulBytesRead)
	This changes the implementation	of PifReadFile.
	RPH 11-7-3
**********************************************************************
fhfh*/
SHORT PIFENTRY PifReadFileNew(USHORT usFile, VOID *pBuffer, ULONG ulBytes, ULONG *pulBytesRead, CHAR *pcFilePath, int iLineNo)
{
    SHORT   sReturn = PIF_ERROR_SYSTEM, sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];
		int  iLen = 0;

		sprintf (aszErrorBuffer, "PifReadFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "    Handle %d, Source File: %s,  Line No: %d", usFile, pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_FileCriticalSection);
		// add new line to end of string for the snapshot file output.
		sprintf (aszErrorBuffer, "    Handle %d, Source File: %s,  Line No: %d\n", usFile, pcFilePath + iLen, iLineNo);
		PifSnapShotFileTable(_T("SNAP0011"), __FILE__, __LINE__, aszErrorBuffer);
        PifLog(FAULT_AT_PIFREADFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFREADFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREADFILE), (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFREADFILE, FAULT_INVALID_HANDLE);
    } else {
		HANDLE  hFile;
		ULONG	ulActualBytesRead;

		if(!pulBytesRead){
			pulBytesRead = &ulActualBytesRead;
		}
		*pulBytesRead = 0;

		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn = PifReadFileExec(hFile, pBuffer, ulBytes, pulBytesRead, aPifFileTable[usFile].wszDeviceName, pcFilePath, iLineNo);

		if (*pulBytesRead > 0) {
			aPifFileTable[usFile].ulFilePointer += *pulBytesRead;
		}
	
		LeaveCriticalSection(&g_FileCriticalSection);
	}
    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifWriteFile(USHORT usFile           **
**                                          VOID FAR *pBuffer       **
**                                          USHORT usBytes)          **
**              usFile:         file handle                         **
**              pBuffer:        writing buffer                      **
**              usBytes:        sizeof pBuffer                      **
**                                                                  **
**  return:     number of bytes to be written                       **
**                                                                  **
**  Description:writing file                                        **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifWriteFileNew(USHORT usFile, CONST VOID *pBuffer,
                             ULONG ulBytes, CHAR *pcFilePath, int iLineNo)
{
    SHORT   sReturn, sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];
		int  iLen = 0;

		sprintf (aszErrorBuffer, "PifWriteFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d", usFile, pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_FileCriticalSection);
		// add new line to end of string for the snapshot file output.
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d\n", usFile, pcFilePath + iLen, iLineNo);
		PifSnapShotFileTable(_T("SNAP0012"), __FILE__, __LINE__, aszErrorBuffer);
        PifLog(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFWRITEFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFWRITEFILE), (USHORT)abs(sStatus));
        PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        return;
	} else {
		HANDLE  hFile;
		ULONG   ulBytesWrite = 0;
    
		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn =  PifWriteFileExec(hFile, pBuffer, ulBytes, &ulBytesWrite, aPifFileTable[usFile].wszDeviceName, pcFilePath, iLineNo);

		if (ulBytesWrite > 0) {
			aPifFileTable[usFile].ulFilePointer += ulBytesWrite;
		}

		LeaveCriticalSection(&g_FileCriticalSection);
	}
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifSeekFile(USHORT usFile           **
**                                          ULONG ulPosition,       **
**                                          ULONG FAR *pulActualPosition) **
**              usFile:         file handle                         **
**              ulPosition:     moving position from file head      **
**              pulActualPosition: moved position actually          **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_SYSTEM                                    **
**                                                                  **
**  Description:move file pointer position to read/write            **     
**              and extend file size
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifSeekFileNew(USHORT usFile, ULONG ulPosition,
                            ULONG *pulActualPosition, CHAR *pcFilePath, int iLineNo)
{
    SHORT   sReturn = PIF_OK, sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];
		int  iLen = 0;

		sprintf (aszErrorBuffer, "PifSeekFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d", usFile, pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_FileCriticalSection);
		// add new line to end of string for the snapshot file output.
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d\n", usFile, pcFilePath + iLen, iLineNo);
		PifSnapShotFileTable(_T("SNAP0013"), __FILE__, __LINE__, aszErrorBuffer);
        PifLog(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFSEEKFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFSEEKFILE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFSEEKFILE), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        return PIF_ERROR_SYSTEM;
	} else {
		HANDLE  hFile;
		ULONG	ulActualPosition;

		if(!pulActualPosition){
			pulActualPosition = &ulActualPosition;
		}
		*pulActualPosition = 0;
    
		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn = PifSeekFileExec(hFile, ulPosition, pulActualPosition, aPifFileTable[usFile].fsMode, aPifFileTable[usFile].wszDeviceName, pcFilePath, iLineNo);
	    
		if (sReturn == PIF_OK) {
			aPifFileTable[usFile].ulFilePointer = *pulActualPosition;
		}

		LeaveCriticalSection(&g_FileCriticalSection);
	}

    return sReturn;
}

/*
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifSetEndOfFile(USHORT usFile)		**
**              usFile:         file handle                         **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_SYSTEM                                    **
**                                                                  **
**  Description: Typically called after PifSeekFile. Moves the		**
**				 end-of-file position for the specified file to the	**
**				 current position of the file pointer. Can be used	**
**				 to truncate a file.								**
**                                                                  **
**********************************************************************
*/
SHORT PIFENTRY PifSetEndOfFile(USHORT usFile)
{
	SHORT   sReturn = PIF_OK;
	SHORT   sStatus;

	EnterCriticalSection(&g_FileCriticalSection);

	if ((sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE)) != usFile) {
		char  aszErrorBuffer[128];

		sprintf (aszErrorBuffer, "PifSetEndOfFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        LeaveCriticalSection(&g_FileCriticalSection);
		PifSnapShotFileTable(_T("SNAP0014"), __FILE__, __LINE__, 0);
        PifLog(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFSEEKFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFSEEKFILE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFSEEKFILE), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        return PIF_ERROR_SYSTEM;
	} else {
		HANDLE hFile;

		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn = SetEndOfFile(hFile);

		LeaveCriticalSection(&g_FileCriticalSection);
	}


	return sReturn;
}


SHORT  PIFENTRY PifSeekWriteFileNew(USHORT usFile, ULONG ulPosition,
                            CONST VOID *pBuffer, ULONG ulBytes, CHAR *pcFilePath, int iLineNo)
{
    SHORT   sReturn = PIF_OK, sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];
		int  iLen = 0;

		sprintf (aszErrorBuffer, "PifSeekWriteFile (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d", usFile, pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
        LeaveCriticalSection(&g_FileCriticalSection);
		// add new line to end of string for the snapshot file output.
		sprintf (aszErrorBuffer, "    Handle %d Source File: %s,  Line No: %d\n", usFile, pcFilePath + iLen, iLineNo);
		PifSnapShotFileTable(_T("SNAP0015"), __FILE__, __LINE__, aszErrorBuffer);
        PifLog(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFSEEKFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFSEEKFILE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFSEEKFILE), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        return PIF_ERROR_SYSTEM;
	} else {
		HANDLE  hFile;
		ULONG   ulActualPosition, ulBytesWrite;

		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn = PifSeekFileExec(hFile, ulPosition, &ulActualPosition, aPifFileTable[usFile].fsMode, aPifFileTable[usFile].wszDeviceName, pcFilePath, iLineNo);
	    
		if (sReturn == PIF_OK) {
			aPifFileTable[usFile].ulFilePointer = ulActualPosition;
			sReturn =  PifWriteFileExec(hFile, pBuffer, ulBytes, &ulBytesWrite, aPifFileTable[usFile].wszDeviceName, pcFilePath, iLineNo);
			if (ulBytesWrite > 0) {
				aPifFileTable[usFile].ulFilePointer += ulBytesWrite;
			}
		}

		LeaveCriticalSection(&g_FileCriticalSection);
	}

    return sReturn;
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifCloseFile(USHORT usFile)           **
**              usFile:         file handle                         **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:close file handle                                   **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifCloseFile(USHORT usFile)
{
	short   sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];

        LeaveCriticalSection(&g_FileCriticalSection);
		sprintf (aszErrorBuffer, "PifCloseFile (): Invalid close file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        PifLog(FAULT_AT_PIFCLOSEFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFCLOSEFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFCLOSEFILE), (USHORT)abs(sStatus));
        return;
	} else {
		HANDLE  hFile;

		hFile = aPifFileTable[usFile].hWin32Handle;
		PifCloseFileExec(hFile, aPifFileTable[usFile].wszDeviceName);

		aPifFileTable[usFile].sPifHandle = -1;
		aPifFileTable[usFile].hWin32Handle = INVALID_HANDLE_VALUE;
		aPifFileTable[usFile].hMutexHandle = INVALID_HANDLE_VALUE;
		aPifFileTable[usFile].ulFilePointer = 0;

		LeaveCriticalSection(&g_FileCriticalSection);
	}
}


/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleteFile(CONST CHAR FAR *pszFileName) **
**              pszFileName:         file name to delete            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:delete file                                         **     
**                                                                  **
**********************************************************************
fhfh*/
VOID  PIFENTRY PifDeleteFileNew(CONST TCHAR *pszFileName, CONST CHAR *pszMode)
{
	CONST TCHAR  *wszPifPathName = 0;
    DWORD   dwError;
    BOOL    fReturn;
    FlMode  fsMode;
    TCHAR   wszFilePathName[MAX_PATH];
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];

    fsMode = dfckmd(pszMode);

    if (fsMode == SYSERR) {
		NHPOS_ASSERT_TEXT(0, "**ERROR: PifDeleteFileNew() open mode error.");
        return;
    }

    if ((fsMode & FLICON) != 0) {
		wszPifPathName = wszFilePathIcon;
    } else if ((fsMode & FLPRINT) != 0) {
		wszPifPathName = wszFilePathPrintFiles;
	} else {
		return;
	}

	if (wszPifPathName != 0) {
		wsprintf(wszFilePathName, TEXT("%s\\%s\\%s"), wszFilePathFlashDisk, wszPifPathName, pszFileName);

		fReturn = DeleteFile(wszFilePathName);
		if (fReturn == 0) {
			dwError = GetLastError();
			if (dwError != ERROR_FILE_NOT_FOUND) {
				wcstombs(aszFilePathName,wszFilePathName,sizeof(aszFilePathName));
				sprintf (aszErrorBuffer, "PifDeleteFileNew (): Error Deleting File  File Name: %s,  Error: %d", aszFilePathName, dwError);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);				
				PifLog (MODULE_PIF_DELETEFILE, LOG_ERROR_CODE_FILE_02);
				PifLog (MODULE_ERROR_NO(MODULE_PIF_DELETEFILE), (USHORT)dwError);
				PifLog (MODULE_LINE_NO(MODULE_PIF_DELETEFILE), (USHORT)__LINE__);
			}
		}

		PifSetDiskFreeSpace();
	}
}


VOID   PIFENTRY PifDeleteFile(CONST TCHAR  *pszFileName)
{
    BOOL    fReturn;
    TCHAR   wszFilePathName[MAX_PATH];
    DWORD   dwError;
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif

    wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifDiskPathName, pszFileName);
#ifdef DEBUG_PIF_OUTPUT
    wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName);
    OutputDebugString(wszDisplay);
#endif

    fReturn = DeleteFile(wszFilePathName);
    if (fReturn == 0) {
        dwError = GetLastError();
        if (dwError != ERROR_FILE_NOT_FOUND) {
			wcstombs(aszFilePathName,wszFilePathName,sizeof(aszFilePathName));
			sprintf (aszErrorBuffer, "PifDeleteFile (): Error Deleting File  File Name: %s,  Error: %d",aszFilePathName, dwError);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
			PifLog (MODULE_PIF_DELETEFILE, LOG_ERROR_CODE_FILE_01);
			PifLog (MODULE_ERROR_NO(MODULE_PIF_DELETEFILE), (USHORT)dwError);
			PifLog (MODULE_LINE_NO(MODULE_PIF_DELETEFILE), (USHORT)__LINE__);
		}
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName, dwError);
        OutputDebugString(wszDisplay);
#endif
    }

    wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifTempPathName, pszFileName);
#ifdef DEBUG_PIF_OUTPUT
    wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName);
    OutputDebugString(wszDisplay);
#endif
    fReturn = DeleteFile(wszFilePathName);
    if (fReturn == 0) {
    
        dwError = GetLastError();
        if (dwError != ERROR_FILE_NOT_FOUND) {
			wcstombs(aszFilePathName,wszFilePathName,sizeof(aszFilePathName));
			sprintf (aszErrorBuffer, "PifDeleteFile (): Error Deleting Temporary File  File Name: %s,  Error: %d",aszFilePathName, dwError);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);				
			PifLog (MODULE_PIF_DELETEFILE, LOG_ERROR_CODE_FILE_02);
			PifLog (MODULE_ERROR_NO(MODULE_PIF_DELETEFILE), (USHORT)dwError);
			PifLog (MODULE_LINE_NO(MODULE_PIF_DELETEFILE), (USHORT)__LINE__);
		}
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName, dwError);
        OutputDebugString(wszDisplay);
#endif
    }

    PifSetDiskFreeSpace();

    return;
}

/************* BEGIN: ADD FOR DOLLAR TREE SCER 12/05/2000 *******************/
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleteFileEx(CONST CHAR FAR *pszFileName,
**                                          BOOL fTempDisk)         **
**              pszFileName:         file name to delete            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:delete file                                         **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifDeleteFileEx(CONST TCHAR  *pszFileName, BOOL fTempDisk)
{
    BOOL    fReturn;
    TCHAR   wszFilePathName[MAX_PATH];
    DWORD   dwError;
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif

    EnterCriticalSection(&g_FileCriticalSection);

    if (fTempDisk)
    {
        wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifTempPathName, pszFileName);
    }
    else
    {
        wsprintf(wszFilePathName, TEXT("%s\\%s"), wszPifDiskPathName, pszFileName);
    }    
#ifdef DEBUG_PIF_OUTPUT
    wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName);
    OutputDebugString(wszDisplay);
#endif

    fReturn = DeleteFile(wszFilePathName);
    if (fReturn == 0) {
        dwError = GetLastError();
        if (dwError != ERROR_FILE_NOT_FOUND) {
			wcstombs(aszFilePathName,wszFilePathName,sizeof(aszFilePathName));
			sprintf (aszErrorBuffer, "PifDeleteFileEx (): Error Deleting File  File: %s,  Error: %d",aszFilePathName, dwError);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
			PifLog (MODULE_PIF_DELETEFILEEX, LOG_ERROR_CODE_FILE_01);
			PifLog (MODULE_ERROR_NO(MODULE_PIF_DELETEFILEEX), (USHORT)dwError);
			PifLog(MODULE_LINE_NO(MODULE_PIF_DELETEFILEEX), (USHORT)__LINE__);
		}
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("PifDeleteFile, %s\r\n"),  pszFileName, dwError);
        OutputDebugString(wszDisplay);
#endif
    }

    PifSetDiskFreeSpace();
    LeaveCriticalSection(&g_FileCriticalSection);

    return;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   ULONG PIFENTRY PifMoveFile(CONST CHAR FAR *szSrcFileName,
**                      BOOL fSrcTempDisk, CONST CHAR FAR *szDstFileName,
**                      BOOL fDstTempDisk )                         **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:delete file             Dec/5/2000                  **     
**                                                                  **
**********************************************************************
fhfh*/
ULONG   PIFENTRY PifMoveFile(CONST TCHAR  *szSrcFileName, BOOL fSrcTempDisk,
                             CONST TCHAR  *szDstFileName, BOOL fDstTempDisk)
{
    BOOL    fReturn;
    TCHAR   wszSrcFilePathName[MAX_PATH];
    TCHAR   wszDstFilePathName[MAX_PATH];
    DWORD   dwError = ERROR_SUCCESS;
	char aszErrorBuffer[128+MAX_PATH+MAX_PATH];
	char aszSrcFilePathName[MAX_PATH], aszDestFilePathName[MAX_PATH];

    EnterCriticalSection(&g_FileCriticalSection);

    if ( fSrcTempDisk )
    {
        wsprintf(wszSrcFilePathName, TEXT("%s\\%s"), wszPifTempPathName, szSrcFileName);
    }
    else
    {
        wsprintf(wszSrcFilePathName, TEXT("%s\\%s"), wszPifDiskPathName, szSrcFileName);
    }

    if ( fDstTempDisk )
    {
        wsprintf(wszDstFilePathName, TEXT("%s\\%s"), wszPifTempPathName, szDstFileName);
    }
    else
    {
        wsprintf(wszDstFilePathName, TEXT("%s\\%s"), wszPifDiskPathName, szDstFileName);
    }

    fReturn = MoveFile(wszSrcFilePathName, wszDstFilePathName);
    if (! fReturn) {
        dwError = GetLastError();
		wcstombs(aszSrcFilePathName,wszSrcFilePathName,sizeof(aszSrcFilePathName));
		wcstombs(aszDestFilePathName,wszDstFilePathName,sizeof(aszDestFilePathName));
		sprintf (aszErrorBuffer, "PifMoveFile (): Error Moving File  Source: %s  Destination: %s,  Error: %d",aszSrcFilePathName, aszDestFilePathName, dwError);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
		PifLog( MODULE_PIF_MOVEFILE, LOG_ERROR_CODE_FILE_01);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_MOVEFILE), (USHORT)dwError);
    }

    PifSetDiskFreeSpace();
    LeaveCriticalSection(&g_FileCriticalSection);

    return dwError;
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
ULONG   PIFENTRY PifGetFileSize(USHORT usFile, ULONG * pulFileSizeHigh)
{
    ULONG   ulFileSize;
	SHORT   sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];

		sprintf (aszErrorBuffer, "PifGetFileSize (): Invalid open file search id  ID: %d, sStatus = %d", usFile, sStatus);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        LeaveCriticalSection(&g_FileCriticalSection);
        PifLog(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFSEEKFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFSEEKFILE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFSEEKFILE), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_INVALID_HANDLE);
        return (ULONG)PIF_ERROR_SYSTEM;
	} else {
		HANDLE  hFile;

		hFile = aPifFileTable[usFile].hWin32Handle;
		ulFileSize = GetFileSize(hFile, pulFileSizeHigh);

		LeaveCriticalSection(&g_FileCriticalSection);
	}

    return ulFileSize;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT  PIFENTRY PifWriteFileEx(USHORT usFile        **
**                                          CONST VOID FAR *pBuffer **
**                                          USHORT usBytes)         **
**              usFile:         file handle                         **
**              pBuffer:        writing buffer                      **
**              usBytes:        sizeof pBuffer                      **
**                                                                  **
**  return:     number of bytes to be written                       **
**                                                                  **
**  Description:writing file                                        **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT  PIFENTRY PifWriteFileEx(USHORT usFile, CONST VOID  *pBuffer, USHORT usBytes)
{
	ULONG   ulBytesWrite = 0;
    SHORT   sReturn, sStatus = 0;

    EnterCriticalSection(&g_FileCriticalSection);

    sStatus = PifSubSearchId(usFile, aPifFileTable, PIF_MAX_FILE);
    if (sStatus != usFile) {
		char aszErrorBuffer[128];

		sprintf (aszErrorBuffer, "PifWriteFileEx (): Invalid open file search id  ID: %d", usFile);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
        LeaveCriticalSection(&g_FileCriticalSection);
        PifLog(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        PifLog(MODULE_DATA_VALUE(FAULT_AT_PIFWRITEFILE), usFile);
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFWRITEFILE), (USHORT)abs(sStatus));
        PifLog(MODULE_LINE_NO(FAULT_AT_PIFWRITEFILE), (USHORT)__LINE__);
        PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_INVALID_HANDLE);
        return (USHORT)PIF_ERROR_SYSTEM;
	} else {
		HANDLE  hFile;

		hFile = aPifFileTable[usFile].hWin32Handle;
		sReturn = PifWriteFileExec(hFile, pBuffer, usBytes, &ulBytesWrite, aPifFileTable[usFile].wszDeviceName, __FILE__, __LINE__);
		if (ulBytesWrite > 0) {
			aPifFileTable[usFile].ulFilePointer += ulBytesWrite;
		}

		LeaveCriticalSection(&g_FileCriticalSection);
	}

    return (USHORT)ulBytesWrite;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifGetFileSizeEx(CONST CHAR FAR * szFileName,
**                            ULONG FAR *pulFileSizeHigh,           **
**                            ULONG FAR *pulFileSizeLow)            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifGetFileSizeEx(CONST TCHAR  *szFileName, BOOL fTempDisk,
                ULONG  *pulFileSizeHigh, ULONG  *pulFileSizeLow)
{
    //TCHAR   wszSrcFileName[MAX_PATH];
    TCHAR   wszFullPathName[MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE  hFile;
    SHORT   sReturn = PIF_ERROR_SYSTEM;

    *pulFileSizeHigh = 0UL;
    *pulFileSizeLow  = 0UL;

    //memset(wszSrcFileName, 0, sizeof(wszSrcFileName));	/* 09/04/01 */
    //mbstowcs(wszSrcFileName, szFileName, MAX_PATH);

    if (fTempDisk)
    {
        wsprintf(wszFullPathName, TEXT("%s\\%s"), wszPifTempPathName, szFileName);
    }
    else
    {
        wsprintf(wszFullPathName, TEXT("%s\\%s"), wszPifDiskPathName, szFileName);
    }

    EnterCriticalSection(&g_FileCriticalSection);

    hFile = FindFirstFile( szFileName, &findData );

    if ( hFile != INVALID_HANDLE_VALUE )
    {
        sReturn = PIF_OK;
        
        *pulFileSizeHigh += findData.nFileSizeHigh;
        *pulFileSizeLow  += findData.nFileSizeLow;

        /* --- look for next file --- */

        while ( FindNextFile( hFile, &findData ) )
        {
            *pulFileSizeHigh += findData.nFileSizeHigh;
            *pulFileSizeLow  += findData.nFileSizeLow;
        }

        FindClose( hFile );
    }

    LeaveCriticalSection(&g_FileCriticalSection);

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifGetDiskFreeSpace(BOOL fTempDisk,
**                            ULONG FAR *pulFreeSizeHigh,           **
**                            ULONG FAR *pulFreeSizeLow,            **
**                            ULONG FAR *pulTotalDiskHigh           **
**                            ULONG FAR *pulTotalDiskLow,           **
**                            ULONG FAR *pulFreeDiskHigh,           **
**                            ULONG FAR *pulFreeDiskLow)            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  PIFENTRY PifGetDiskFreeSpace(BOOL fTempDisk,
                ULONG  *pulFreeSizeHigh,  ULONG  *pulFreeSizeLow,
                ULONG  *pulTotalDiskHigh, ULONG  *pulTotalDiskLow,
                ULONG  *pulFreeDiskHigh,  ULONG  *pulFreeDiskLow )
{
    TCHAR   *pszDiskName;
    SHORT   sReturn = PIF_ERROR_SYSTEM;
    ULARGE_INTEGER  FreeCaller;
    ULARGE_INTEGER  TotalDisk;
    ULARGE_INTEGER  FreeDisk;

    pszDiskName = ( fTempDisk ) ? wszPifTempPathName : wszPifDiskPathName;

    EnterCriticalSection(&g_FileCriticalSection);

    if ( GetDiskFreeSpaceEx( pszDiskName, &FreeCaller, &TotalDisk, &FreeDisk ))
    {
        sReturn = PIF_OK;

        *pulFreeSizeHigh = FreeCaller.HighPart;
        *pulFreeSizeLow  = FreeCaller.LowPart;

        *pulTotalDiskHigh = TotalDisk.HighPart;
        *pulTotalDiskLow  = TotalDisk.LowPart;

        *pulFreeDiskHigh = FreeDisk.HighPart;
        *pulFreeDiskLow  = FreeDisk.LowPart;
    }

    LeaveCriticalSection(&g_FileCriticalSection);

    return sReturn;
}
/************* END: ADD FOR DOLLAR TREE SCER 12/05/2000 *******************/

/* PifOpenFile */

SHORT  PifOpenFileExec(CONST TCHAR *pwszFileName, FlMode fsMode, HANDLE *hHandle)
/*SHORT  PifOpenFileExec(TCHAR *pwszFileName, CHAR chMode, HANDLE *hHandle)*/
{

    TCHAR wszFilePathName[MAX_PATH];
    DWORD dwDesiredAccess = 0, dwShareMode = 0, dwCreationDesposition = 0;
    DWORD dwError;
    DWORD dwRetry;
    SHORT sReturn;
    HANDLE  hFile;
    DWORD dwFlagsAndAttributes = 0;
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif

    if ((fsMode & 0x0f) == FLREAD) {
        dwDesiredAccess |= GENERIC_READ;
        /* set always read/write share mode, to avoid sharing vioration, 05/28/01 */
        dwShareMode     |= FILE_SHARE_READ|FILE_SHARE_WRITE;  /* only 'r'             */
        /* dwShareMode     |= FILE_SHARE_READ; */
    }
    else if ((fsMode & 0x0f) == FLWRITE) {
        dwDesiredAccess |= GENERIC_WRITE;
        /* set always read/write share mode, to avoid sharing vioration, 05/28/01 */
        dwShareMode     |= FILE_SHARE_READ|FILE_SHARE_WRITE;  /* only 'w'             */
        /* dwShareMode     |= FILE_SHARE_WRITE; */
    }
    else if ((fsMode & 0x0f) == (FLREAD | FLDENYSHARE)) {
        dwDesiredAccess |= GENERIC_READ;                      /* only 'r'             */
    }
    else if ((fsMode & 0x0f) == (FLWRITE | FLDENYSHARE)) {
        dwDesiredAccess |= GENERIC_WRITE;                     /* only 'w'             */
    }
    else if ((fsMode & 0x0f) == (FLREAD | FLWRITE | FLDENYSHARE)) {
        dwDesiredAccess |= GENERIC_READ|GENERIC_WRITE;        /* 'w' & 'r' or default */
    }
    else {
        dwDesiredAccess |= GENERIC_READ|GENERIC_WRITE;
        dwShareMode     |= FILE_SHARE_READ|FILE_SHARE_WRITE;  /* 'w' & 'r' or default */
    }

    if ((fsMode & 0xf0) == FLNEW) {
        dwCreationDesposition = CREATE_NEW;
    }
    else if ((fsMode & 0xf0) == FLOLD) {
        dwCreationDesposition = OPEN_EXISTING;
    }
    else if ((fsMode & 0xf0) == FLRECREATE) {
        dwCreationDesposition = CREATE_ALWAYS;
    }
    else {
        dwCreationDesposition = OPEN_ALWAYS;
    }

    if (fsMode & FLFLASH) {
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_RANDOM_ACCESS;
	} else {
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS;
    }
#ifdef DEBUG_PIF_OUTPUT
    wsprintf(wszDisplay, TEXT("PifOpenFile, %s, Mode = %04x\r\n"),  pwszFileName, fsMode);
    OutputDebugString(wszDisplay);
#endif

	PifGetFilePathLocal (pwszFileName, fsMode, wszFilePathName);
	if (_tcslen(wszFilePathName) < 2) {
		sReturn = PIF_ERROR_SYSTEM;
		PifLog( MODULE_PIF_OPENFILE, LOG_ERROR_CODE_FILE_01);
		PifLog( MODULE_ERROR_NO(MODULE_PIF_OPENFILE), 1 );
		return sReturn;
	}

    /* --- open specified file with required mode --- */
    dwError = ERROR_FILE_NOT_FOUND;
    dwRetry = 0;

    while (( dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_SHARING_VIOLATION ) && ( dwRetry < 3 ))
    {
        hFile = CreateFile( wszFilePathName, dwDesiredAccess, dwShareMode, NULL, dwCreationDesposition, dwFlagsAndAttributes, 0 );

        if ( hFile != INVALID_HANDLE_VALUE )
        {
            dwError  = ERROR_SUCCESS;
            *hHandle = hFile;
        }
        else
        {
            dwError = GetLastError();
//            PifLog( MODULE_PIF_OPENFILE, ( SHORT )dwError );
			if (dwError == ERROR_FILE_NOT_FOUND)
				PifSleep (50);
			else
				PifSleep( 100 );
        }
        dwRetry++;
    }

    switch ( dwError )
    {
    case ERROR_SUCCESS:
        sReturn = PIF_OK;
        break;
    case ERROR_FILE_EXISTS:
        sReturn = PIF_ERROR_FILE_EXIST;
        break;
    case ERROR_FILE_NOT_FOUND:
        sReturn = PIF_ERROR_FILE_NOT_FOUND;
        break;
    default:
        sReturn = PIF_ERROR_SYSTEM;
		PifLog( MODULE_PIF_OPENFILE, LOG_ERROR_CODE_FILE_01);
        PifLog( MODULE_ERROR_NO(MODULE_PIF_OPENFILE), ( SHORT )dwError );
		{
			char xBuff[128];
			int  iLen = _tcslen(pwszFileName);
			if (iLen > 30) {
				iLen -= 30;
			}
			else {
				iLen = 0;
			}
			sprintf (xBuff, "--WARNING: PifOpenFileExec(): CreateFile() %S error %d", pwszFileName + iLen, dwError);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        break;
    }
    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT dfckmd (CHAR *pchMode)                        **
**                                                                  **
**  Return:     TRUE    OK                                          **
**              FALSE   invalid open mode                           **
**                                                                  **
** Description: This function checks the open mode of the file.     **
**              See also function RmtCheckOpenMode() in isprmt.c    **
**              Changes to dfckmd() may require changes to the      **
**              corresponding remote function RmtCheckOpenMode().   **
**********************************************************************
fhfh*/
static FlMode dfckmd(CONST CHAR *mode)
{
    short    ch;
    FlMode   mbits;
	// following list of all folders used to determine if any folder other than specified folder has already been indicated.
	// typical usage: if (mbits & (mBitsFolders & ~RMT_PRINTFILES)) mbits = (SYSERR); // folder other than RMT_PRINTFILES already specified.
	FlMode   mBitsFolders = (FLTMP | FLFLASH | FLSAVTTL | FLICON | FLPRINT | FLCONFIG | FLHISFOLDER | FLIMAGESFOLDER);

    mbits = 0;
    for (ch = *mode; ch > 0; ch = (*(++mode))) {
        switch (ch) {
        case 'r':
            mbits |= FLREAD;                // means same as RMT_READ for remote file access through ISP subsystem.
            break;

        case 'w':
            mbits |= FLWRITE;                // means same as RMT_WRITE for remote file access through ISP subsystem.
            break;

        case 'o':
            if (mbits & FLNEW)
                return(SYSERR);
            mbits |= FLOLD;                // means same as RMT_OLD for remote file access through ISP subsystem.
            break;

        case 'n':
            if (mbits & FLOLD)
                return(SYSERR);
            mbits |= FLNEW;                // means same as RMT_NEW for remote file access through ISP subsystem.
            break;

        case 'x':
            if (mbits & FLOLD || mbits & FLNEW)
                return(SYSERR);
            mbits |= FLRECREATE;
            break;

        case '-':
            mbits |= FLDENYSHARE;
            break;

        case 't':                 // FLTMP - temporary files folder.  RMT_TEMP for remote file access through ISP subsystem.
			if (mbits & (mBitsFolders & ~FLTMP))
				return(SYSERR);
			else
				mbits |= FLTMP;
            break;
        case 'f':
			if (mbits & (mBitsFolders & ~FLFLASH))
				return(SYSERR);
			else
				mbits |= FLFLASH;
            break;
		case 's':
			if (mbits & (mBitsFolders & ~FLSAVTTL))
				return(SYSERR);
			else
				mbits |= FLSAVTTL;
			break;
		case 'i':                 // FLICON - icon files folder.  RMT_ICON for remote file access through ISP subsystem.
			if (mbits & (mBitsFolders & ~FLICON))
				return(SYSERR);
			else
				mbits |= FLICON;
			break;
		case 'b':                 // FLIMAGESFOLDER - image files folder, images (bitmap, png, jpeg, etc.) used for purposes such as Suggestive Selling.
			if (mbits & (mBitsFolders & ~FLIMAGESFOLDER))
				return(SYSERR);
			else
				mbits |= FLIMAGESFOLDER;
			break;
		case 'p':                 // FLPRINT - print files folder, Amtrak.  RMT_PRINTFILES for remote file access through ISP subsystem.
			if (mbits & (mBitsFolders & ~FLPRINT))
				return(SYSERR);
			else
				mbits |= FLPRINT;
			break;
		case 'c':                 // FLCONFIG - configuration files folder, Amtrak.  RMT_CONFIG for remote file access through ISP subsystem.
			if (mbits & (mBitsFolders & ~FLCONFIG))
				return(SYSERR);
			else
				mbits |= FLCONFIG;
			break;
		case 'h':                 // FLHISFOLDER - historial and reports files folder.  RMT_HISFOLDER for remote file access through ISP subsystem.
			if (mbits & (mBitsFolders & ~FLHISFOLDER))
				return(SYSERR);
			else
				mbits |= FLHISFOLDER;
			break;
        default:
            return(SYSERR);
        }
    }
    if (!(mbits & FLREAD) && !(mbits & FLWRITE))      /* default: allow R + W     */
        mbits |= (FLREAD | FLWRITE);
    return(mbits);
}

/* PifReadFile */

SHORT PifReadFileExec(HANDLE hFile, VOID  *pBuffer, ULONG usBytes, ULONG *pulBytesRead, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo)
{

    DWORD   dwNumberOfBytesRead = 0;
    BOOL    fReturn;
    DWORD   dwError;
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifReadFile, %s, %d bytes read\r\n"), pwszFileName, usBytes);
    OutputDebugString(wszDisplay);
#endif

    UNREFERENCED_PARAMETER(pwszFileName);
    fReturn = ReadFile(hFile, pBuffer, (DWORD)usBytes, &dwNumberOfBytesRead, NULL);

    if (fReturn == 0) {
		int  iLen = 0;

        dwError = GetLastError();
		wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
		sprintf (aszErrorBuffer, "PifReadFileExec (): File: %s,  hFile: %p,  Error: %d, pBuffer = %p, usBytes = %d ",aszFilePathName, hFile, dwError, pBuffer, usBytes);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "    Source File: %s,  Line No.: %d", pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
		PifLog(MODULE_PIF_READFILE, LOG_ERROR_CODE_FILE_01);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_READFILE), (USHORT)dwError);
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
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFREADFILE), (USHORT) dwError);
        PifAbort(FAULT_AT_PIFREADFILE, FAULT_SYSTEM);
		*pulBytesRead = dwNumberOfBytesRead;
        return PIF_ERROR_SYSTEM;
#endif
	}

    *pulBytesRead = dwNumberOfBytesRead;

    return PIF_OK;
}

/* PifWriteFile */

SHORT   PifWriteFileExec(HANDLE hFile, CONST VOID FAR *pBuffer,
                             ULONG usBytes, ULONG *pulBytesWrite, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo)
{
    BOOL    fReturn;
    DWORD   dwNumberOfBytesWrite = 0;
    DWORD   dwError;
	char aszErrorBuffer[128+MAX_PATH];  // max path name length plus extra 128 for description text
	char aszFilePathName[MAX_PATH];
#if DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifWriteFile, %s, %d bytes write\r\n"), pwszFileName, usBytes);
    OutputDebugString(wszDisplay);
#endif
    UNREFERENCED_PARAMETER(pwszFileName);

    fReturn = WriteFile(hFile, pBuffer, (DWORD)usBytes, &dwNumberOfBytesWrite, NULL);

    if (!fReturn) {
		int  iLen = 0;

        dwError = GetLastError();
		iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
		sprintf (aszErrorBuffer, "PifWriteFileExec (): Source File: %s,  Line No: %d", pcFilePath + iLen, iLineNo);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
		wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
		sprintf (aszErrorBuffer, "    File: %s,  hFile: %p, dwError: %d, pBuffer = %p, usBytes = %d ",aszFilePathName, hFile, dwError, pBuffer, usBytes);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
		PifLog(MODULE_PIF_WRITEFILE, LOG_ERROR_CODE_FILE_01);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_WRITEFILE), (USHORT)dwError);
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
        PifLog(MODULE_ERROR_NO(FAULT_AT_PIFWRITEFILE), (USHORT) dwError);
        PifAbort(FAULT_AT_PIFWRITEFILE, FAULT_SYSTEM);
		*pulBytesWrite = dwNumberOfBytesWrite;
        return PIF_ERROR_SYSTEM;
#endif
    }

    *pulBytesWrite = dwNumberOfBytesWrite;

    return PIF_OK;
}

/* PifSeekFile */

SHORT  PifSeekFileExec(HANDLE hFile, ULONG ulPosition,
                            ULONG *pulActualPosition, SHORT fsMode, TCHAR *pwszFileName, CHAR *pcFilePath, int iLineNo)
{
    DWORD   dwReturn, dwFileSize;
    DWORD   dwError;
    SHORT   sReturn = PIF_OK;
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifSeekFile, %s, %d bytes seek\r\n"), pwszFileName, ulPosition);
    OutputDebugString(wszDisplay);
#endif
    UNREFERENCED_PARAMETER(pwszFileName);

    dwReturn = GetFileSize(hFile, &dwFileSize);

    if (ulPosition > dwReturn) {

        /* current size is smaller than desired size */

        if ((fsMode & 0x0f) == FLREAD) {
        /* if (chMode & 0x0f == FLREAD) { */
        /* if (aPifHandleTable[usFile].chMode & 0x0f == FLREAD) { */

            /* read only mode */
            dwReturn = SetFilePointer(hFile, 0, NULL, FILE_END);

            if (dwReturn == 0xFFFFFFFF) {
				int  iLen = 0;

                dwError = GetLastError();
				wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
				sprintf (aszErrorBuffer, "PifSeekFileExec (): Error Setting File Pointer Read  File Name: %s, %d bytes seek,  Error: %d",aszFilePathName, ulPosition, dwError);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
				iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
				sprintf (aszErrorBuffer, "    Source File: %s,  Line No: %d", pcFilePath + iLen, iLineNo);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
				PifLog(MODULE_PIF_SEEKFILE, LOG_ERROR_CODE_FILE_01);
				PifLog(MODULE_ERROR_NO(MODULE_PIF_SEEKFILE), (USHORT)dwError);
#ifdef DEBUG_PIF_OUTPUT
                wsprintf(wszDisplay, TEXT("PifSeekFile, %s, %d bytes seek, Error=%d\r\n"), pwszFileName, ulPosition, dwError);
                OutputDebugString(wszDisplay);
#endif
                sReturn = PIF_ERROR_SYSTEM;
                // PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_SYSTEM);
            } else {
                *pulActualPosition = dwFileSize;
                sReturn = PIF_ERROR_FILE_EOF;
            }
        } else {
            /* write mode */
            dwReturn = SetFilePointer(hFile, ulPosition, NULL, FILE_BEGIN);

            if (dwReturn == 0xFFFFFFFF) {
				int  iLen = 0;

                dwError = GetLastError();
				wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
				sprintf (aszErrorBuffer, "PifSeekFileExec (): Error Setting File Pointer Write  File Name: %s, %d bytes seek,  dwError: %d", aszFilePathName, ulPosition, dwError);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
				iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
				sprintf (aszErrorBuffer, "    Source File: %s,  Line No: %d", pcFilePath + iLen, iLineNo);
				NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
				PifLog(MODULE_PIF_SEEKFILE, LOG_ERROR_CODE_FILE_02);
				PifLog(MODULE_ERROR_NO(MODULE_PIF_SEEKFILE), (USHORT)dwError);
                sReturn = PIF_ERROR_FILE_DISK_FULL;
                /* PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_SYSTEM); */
            } else {
                /* increase file size */
                *pulActualPosition = dwReturn;
                dwReturn = SetEndOfFile(hFile);

                if (dwReturn) {
                    sReturn = PIF_OK;
                    PifSetDiskFreeSpace();
                } else {
                    dwError = GetLastError();
					PifLog(MODULE_PIF_SEEKFILE, LOG_ERROR_CODE_FILE_03);
					PifLog(MODULE_ERROR_NO(MODULE_PIF_SEEKFILE), (USHORT)dwError);
                    sReturn = PIF_ERROR_FILE_DISK_FULL;
                }
            }
        }
    } else {
        dwReturn = SetFilePointer(hFile, ulPosition, NULL, FILE_BEGIN);

        if (dwReturn == 0xFFFFFFFF) {
			int iLen = 0;

            dwError = GetLastError();
			wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));

			iLen = strlen(aszFilePathName);
			if (iLen > 30) {
				iLen -= 30;
			}
			else {
				iLen = 0;
			}
			sprintf (aszErrorBuffer, "PifSeekFileExec (): Error Setting Pointer  File: %s, %d bytes seek,  Error: %d",aszFilePathName + iLen, ulPosition, dwError);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);

			iLen = strlen (pcFilePath); if (iLen < 30) iLen = 0; else iLen -= 30;
			sprintf (aszErrorBuffer, "    Source File: %s,  Line No: %d", pcFilePath + iLen, iLineNo);
			NHPOS_ASSERT_TEXT (0, aszErrorBuffer);			
			PifLog(MODULE_PIF_SEEKFILE, LOG_ERROR_CODE_FILE_04);
			PifLog(MODULE_ERROR_NO(MODULE_PIF_SEEKFILE), (USHORT)dwError);
#ifdef DEBUG_PIF_OUTPUT
            wsprintf(wszDisplay, TEXT("PifSeekFile, %s, %d bytes seek, Error=%d\r\n"), pwszFileName, ulPosition, dwError);
            OutputDebugString(wszDisplay);
#endif
            sReturn = PIF_ERROR_SYSTEM;
            // PifAbort(FAULT_AT_PIFSEEKFILE, FAULT_SYSTEM);
        } else {
            *pulActualPosition = dwReturn;
            sReturn = PIF_OK;
        }
    }
    return sReturn;
}

/* PifCloseFile */

VOID   PifCloseFileExec(HANDLE hFile, TCHAR *pwszFileName)
{
    BOOL    fReturn;
    DWORD   dwError;
	char aszErrorBuffer[128+MAX_PATH];
	char aszFilePathName[MAX_PATH];
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];

    wsprintf(wszDisplay, TEXT("PifCloseFile, %s\r\n"), pwszFileName);
    OutputDebugString(wszDisplay);
#endif
	UNREFERENCED_PARAMETER(pwszFileName);

    fReturn = CloseHandle(hFile);

    if (fReturn == 0) {
        dwError = GetLastError();
		wcstombs(aszFilePathName,pwszFileName,sizeof(aszFilePathName));
		sprintf (aszErrorBuffer, "PifCloseFileExec (): Error Setting File Pointer  File Name: %s,  Error: %d",aszFilePathName, dwError);
		NHPOS_ASSERT_TEXT (0, aszErrorBuffer);
		PifLog(MODULE_PIF_CLOSEFILE, LOG_ERROR_CODE_FILE_11);
		PifLog(MODULE_ERROR_NO(MODULE_PIF_CLOSEFILE), (USHORT)dwError);
#ifdef DEBUG_PIF_OUTPUT
        wsprintf(wszDisplay, TEXT("PifCloseFile, %s, Error=%d\r\n"), pwszFileName, dwError);
        OutputDebugString(wszDisplay);
#endif

    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifLoadFarData(VOID *pAddress, USHORT usSize) **
**              pAddress:         RAM area start address            **
**              usSize              size of RAM area                **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:load default parameter from parameter file to ram area  **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT   PIFENTRY PifLoadFarData(VOID *pAddress, ULONG usSize, TCHAR *pVersion)
{
    SHORT sFile;
	SHORT sRetStatus = 0;

    sFile = PifOpenFile(_T("NHPOSPara.dat"), "ro");
    if (sFile >= 0) {
		ULONG  usActualPosition;
		ULONG  ulActualBytesRead;	//RPH 11-10-3

        PifSeekFile(sFile, 0, &usActualPosition);
        //RPH 11-10-3 Change for PifReadFile
		PifReadFile(sFile, pAddress, usSize, &ulActualBytesRead);
		if( ulActualBytesRead < usSize)
		{
			SHORT  sFile2 = PifOpenFile(_T("NHPOSParaBKUP.dat"), "rw");
			PifSeekFile(sFile2, 0, &usActualPosition);
			PifWriteFile(sFile2, pAddress, ulActualBytesRead);
			PifCloseFile(sFile2);
			PifCloseFile(sFile);
			PifFileMigration(pAddress, ulActualBytesRead, pVersion);
			PifDeleteFile(_T("NHPOSParaBKUP.dat"));			
			SysConfig.uchPowerUpMode |= POWER_UP_CLEAR_WHOLE_MEMORY;    // PifLoadFarData()
			sRetStatus = -1;
			NHPOS_NONASSERT_TEXT("==NOTE: PifLoadFarData() - size mismatch.");
		}
		else {
			PifCloseFile(sFile);
		}
    } else {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: PifLoadFarData() - NHPOSPara.dat open error %d", sFile);
		NHPOS_NONASSERT_TEXT(xBuff);
		sRetStatus = -1;
        /* initial reset flag */
        SysConfig.uchPowerUpMode |= POWER_UP_CLEAR_WHOLE_MEMORY;      // PifLoadFarData()
    }

    pPifDatAddress = pAddress;
    usPifDatSize = usSize;
    return sRetStatus;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSaveFarData(VOID)                  **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:save current parameter to parameter file from ram area  **     
**                                                                  **
**********************************************************************
fhfh*/
VOID   PIFENTRY PifSaveFarData(VOID)
{
    ULONG usActualPosition = 0;

    if (pPifDatAddress) {
		SHORT sFile = PifOpenFile(_T("NHPOSPara.dat"), "w");
        if (sFile >= 0) {
            PifSeekFile(sFile, usPifDatSize, &usActualPosition);
			if (usPifDatSize != usActualPosition) {
				NHPOS_ASSERT(!"PifSaveFarData usPifDatSize != usActualPosition");
			}
            PifSeekFile(sFile, 0, &usActualPosition);
            PifWriteFile(sFile, pPifDatAddress, usPifDatSize);
            PifCloseFile(sFile);
		} else {
			char  xBuff[128];

			sprintf (xBuff, "==NOTE: PifSaveFarData() - NHPOSPara.dat open error %d", sFile);
			NHPOS_NONASSERT_TEXT(xBuff);
        }
    }

    return;
}

SHORT PifCheckInitialReset()
{
    SHORT sFile = PifOpenFile(_T("NHPOSPara.dat"), "ro");
    if (sFile >= 0) {
        PifCloseFile(sFile);
        return FALSE;
    } else {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: PifCheckInitialReset() - NHPOSPara.dat open error %d", sFile);
		NHPOS_NONASSERT_TEXT(xBuff);

		/* initial reset flag */
        SysConfig.uchPowerUpMode |= POWER_UP_CLEAR_WHOLE_MEMORY;
        return TRUE;
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetDiskNamePath(VOID)              **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:set disk name path from registry                    **     
**                                                                  **
**********************************************************************
fhfh*/

VOID PifSetDiskNamePath(VOID)
{
	TCHAR   wszKey[128] = {0};
	TCHAR   wszPath[MAX_PATH] = {0};
	TCHAR   wszBase[MAX_PATH] = {0};
	TCHAR   wszLog[MAX_PATH] = {0};
    LONG    lRet;
    HKEY hKey;
	DWORD   dwDisposition;


    // attempt to create the key
    // for future hard disk drive
    memcpy(wszPath, wszFilePathFlashDisk, sizeof(wszFilePathFlashDisk));
    memcpy(wszBase, wszFilePathDataBase, sizeof(wszFilePathDataBase));
    memcpy(wszLog, wszFilePathLog, sizeof(wszFilePathLog));

    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, FILEKEY);

    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
		TCHAR  wszData[MAX_PATH] = {0};
		DWORD   dwBytes = sizeof(wszData);
		DWORD   dwType;

        lRet = RegQueryValueEx(hKey, PATH, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            memset(wszPath, 0, sizeof(wszPath));
#if 1
			// Just use the old style C:\DirA\DirB pathname which works just fine
			// on all Windows platforms.
			// This is to address a problem in which the pathname was ill formed by
			// the previous code which ended up being \C:\DirA\DirB. There is an
			// alternative form of Windows pathname that looks like \\.\C:\DirA\DirB
			// however it suggested for physical devices such as Serial Port.
			// The previous code seemed to work with Windows 7
			// but with Windows 10 it fails and GenPOS would start up and not create
			// any of the files in the FlashDisk nor TempDisk folder hierarchy.
			//    Richard Chambers, Jan-24-2019
            wsprintf(wszPath, TEXT("%s"), wszData);
			NHPOS_NONASSERT_NOTE ("==NOTE", "==NOTE: using FlashDisk Database path from Registry.");
#else
            if (wszData[0] != TEXT('\\')) {                                 /* \Flash Disk */
                wsprintf(wszPath, TEXT("\\%s"), wszData);
            } else if ((wszData[0] == TEXT('\\')) && (wszData[1] == TEXT('\\'))) { /* \\Flash Disk */
                wsprintf(wszPath, TEXT("%s"), &wszData[1]);
            } else {                                                        /* Flash Disk */
                wsprintf(wszPath, TEXT("%s"), wszData);
            }
#endif
            if (wszPath[wcslen(wszPath)-1] == TEXT('\\')) {                 /* remove last \ */
                wszPath[wcslen(wszPath)-1] = TEXT('\0');
            }
        } else {
            /* set default path data for other users */
            dwBytes = (wcslen(wszPath)+1) * 2;
            /* dwBytes = sizeof(wszData); */

            lRet = RegSetValueEx(hKey, PATH, 0, REG_SZ, (const UCHAR *)&wszPath[0], dwBytes);
        }

        memcpy(wszPifPathName, wszPath, sizeof(wszPifPathName));

        /* read pif file directory name */
        memset(wszData, 0, sizeof(wszData));
        dwBytes = sizeof(wszData);

        lRet = RegQueryValueEx(hKey, DATABASE, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            memset(wszBase, 0, sizeof(wszBase));
            if (wszData[0] == TEXT('\\')) {
                memcpy(wszBase, &wszData[1], sizeof(wszBase)-2);
            } else {
                memcpy(wszBase, wszData, sizeof(wszBase));
            }
            if (wszBase[wcslen(wszBase)-1] == TEXT('\\')) {                 /* remove last \ */
                wszBase[wcslen(wszBase)-1] = TEXT('\0');
            }
        } else {
            /* set default data for other users */
            dwBytes = (wcslen(wszBase)+1) * 2;
            //dwBytes = sizeof(wszBase);
            lRet = RegSetValueEx(hKey, DATABASE, 0, REG_SZ, (const UCHAR *)&wszBase[0], dwBytes);
        }

        /* read log directory name */
        memset(wszData, 0, sizeof(wszData));
        dwBytes = sizeof(wszData);

        lRet = RegQueryValueEx(hKey, PIFLOG, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            memset(wszLog, 0, sizeof(wszLog));
            if (wszData[0] == TEXT('\\')) {
                memcpy(wszLog, &wszData[1], sizeof(wszLog)-2);
            } else {
                memcpy(wszLog, wszData, sizeof(wszLog));
            }
            if (wszLog[wcslen(wszLog)-1] == TEXT('\\')) {                 /* remove last \ */
                wszLog[wcslen(wszLog)-1] = TEXT('\0');
            }
        } else {
            /* set default data for other users */
            dwBytes = (wcslen(wszLog)+1) * 2;
            //dwBytes = sizeof(wszLog);

            lRet = RegSetValueEx(hKey, PIFLOG, 0, REG_SZ, (const UCHAR *)&wszLog[0], dwBytes);
        }
    }

    wsprintf(wszPifDiskPathName, TEXT("%s\\%s"), wszPath, wszBase);
    wsprintf(wszPifLogPathName, TEXT("%s\\%s"), wszPath, wszBase);
	wsprintf(wszPifSavTotalPathName, TEXT("%s\\%s\\%s"), wszPath, wszBase, wszFilePathSavedTotal);
	wsprintf(wszPifIconPathName, TEXT("%s\\%s"), wszPath, wszFilePathIcon);
	wsprintf(wszPifImagesPathName, TEXT("%s\\%s\\%s"), wszPath, wszBase, wszFilePathImages);
	wsprintf(wszPifPrintPathName, TEXT("%s\\%s"), wszPath, wszFilePathPrintFiles);
	wsprintf(wszPifConfigPathName, TEXT("%s"), wszFilePathConfig);

    /* set temporary directory in ram disk */
    memset(wszKey, 0, sizeof(wszKey));
    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, FILEKEY);
    memset(wszPath, 0, sizeof(wszPath));
    memcpy(wszPath, wszFilePathTempDisk, sizeof(wszFilePathTempDisk));

    lRet = RegCreateKeyEx(HKEY_CURRENT_USER, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (lRet == ERROR_SUCCESS) {
		TCHAR  wszData[MAX_PATH] = {0};
		DWORD   dwBytes = sizeof(wszData);
		DWORD   dwType;

        lRet = RegQueryValueEx(hKey, TEMPPATH, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            memset(wszPath, 0, sizeof(wszPath));
#if 1
			// Just use the old style C:\DirA\DirB pathname which works just fine
			// on all Windows platforms.
			// This is to address a problem in which the pathname was ill formed by
			// the previous code which ended up being \C:\DirA\DirB. There is an
			// alternative form of Windows pathname that looks like \\.\C:\DirA\DirB
			// however it suggested for physical devices such as Serial Port.
			// The previous code seemed to work with Windows 7
			// but with Windows 10 it fails and GenPOS would start up and not create
			// any of the files in the FlashDisk nor TempDisk folder hierarchy.
			//    Richard Chambers, Jan-24-2019
            wsprintf(wszPath, TEXT("%s"), wszData);
			NHPOS_NONASSERT_NOTE ("==NOTE", "==NOTE: using TempDisk Database path from Registry.");
#else
            if (wszData[0] != TEXT('\\')) {                                 /* \Flash Disk */
                wsprintf(wszPath, TEXT("\\%s"), wszData);
            } else if ((wszData[0] == TEXT('\\')) && (wszData[1] == TEXT('\\'))) { /* \\Flash Disk */
                wsprintf(wszPath, TEXT("%s"), &wszData[1]);
            } else {                                                        /* Flash Disk */
                wsprintf(wszPath, TEXT("%s"), wszData);
            }
#endif
            if (wszPath[wcslen(wszPath)-1] == TEXT('\\')) {                 /* remove last \ */
                wszPath[wcslen(wszPath)-1] = TEXT('\0');
            }
        } else {
            /* set default path data for other users */
            dwBytes = (wcslen(wszPath)+1) * 2;
            /* dwBytes = sizeof(wszData); */
            lRet = RegSetValueEx(hKey, TEMPPATH, 0, REG_SZ, (const UCHAR *)&wszPath[0], dwBytes);
        }

        /* read pif file directory name */
        memset(wszData, 0, sizeof(wszData));
        dwBytes = sizeof(wszData);

        lRet = RegQueryValueEx(hKey, DATABASE, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
        if (lRet == ERROR_SUCCESS) {
            memset(wszBase, 0, sizeof(wszBase));
            if (wszData[0] == TEXT('\\')) {
                memcpy(wszBase, &wszData[1], sizeof(wszBase)-2);
            } else {
                memcpy(wszBase, wszData, sizeof(wszBase));
            }
            if (wszBase[wcslen(wszBase)-1] == TEXT('\\')) {                 /* remove last \ */
                wszBase[wcslen(wszBase)-1] = TEXT('\0');
            }
        } else {
            /* set default data for other users */
            dwBytes = (wcslen(wszBase)+1) * 2;
            //dwBytes = sizeof(wszBase);

            lRet = RegSetValueEx(hKey, DATABASE, 0, REG_SZ, (const UCHAR *)&wszBase[0], dwBytes);
        }
    }
    wsprintf(wszPifTempPathName, TEXT("%s\\%s"), wszPath, wszBase);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PifCheckAndCreateDirectory(VOID)             **
**                                                                  **
**  return:     PIF_OK                                              **
*               PIF_ERROR_SYSTEM                                    **
**                                                                  **
**  Description:create directory to administrate file by PIF        **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PifCheckAndCreateDirectory(VOID)
{
    DWORD   dwError;
    wchar_t wszFilePath[MAX_PATH];
    wchar_t *pwszStart, *pwszFilePath;
    int     i;
	TCHAR   *pwszPathName[] = {
		wszPifDiskPathName,           // folder for the database folder
		wszPifTempPathName,           // folder for temporary files used during transactions, etc.
		wszPifSavTotalPathName,       // folder for saving the totals when doing an end of day
		wszPifIconPathName,           // folder for containing the icon files
		wszPifPrintPathName,          // folder for other applications to put text files to be printed
		wszPifConfigPathName,         // folder for configuration files, Amtrak specific
		wszPifImagesPathName,         // folder for images files, such as Suggestive Selling.
		0
	};

    for (i=0; pwszPathName[i]; i++) {
        memset(wszFilePath, 0, sizeof(wszFilePath));
        memcpy(wszFilePath, pwszPathName[i], MAX_PATH);
        pwszFilePath = &wszFilePath[0];
        pwszStart = pwszFilePath;

		if (pwszStart[0] == 0)
			continue;

        do {
            pwszFilePath++;

            if (*pwszFilePath == TEXT('\\')) {  /* create each directoy path */
                *pwszFilePath = TEXT('\0');
                if (CreateDirectory(pwszStart, NULL)) {
                    ;
                } else {
                    dwError = GetLastError();
                    if (dwError == ERROR_ALREADY_EXISTS) {
                        ;
                    } else {
                        return (USHORT)PIF_ERROR_SYSTEM;
                    }
                }
                *pwszFilePath = TEXT('\\');
                continue;
            }
            if (*pwszFilePath == TEXT('\0')) {  /* last directroy */
                if (CreateDirectory(pwszStart, NULL)) {
                    ;
                } else {
                    dwError = GetLastError();
                    if (dwError == ERROR_ALREADY_EXISTS) {
                        ;
                    } else {
                        return (USHORT)PIF_ERROR_SYSTEM;
                    }
                }
                break;
            }

        } while(1);
    }
    return (PIF_OK);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifSetDiskFreeSpace(VOID)                      **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description: Get flash disk free space                          **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifSetDiskFreeSpace(VOID)
{
    ULARGE_INTEGER FreeBytesAvailableToCaller;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    BOOL    fReturn;
    DWORD   dwError;

    fReturn = GetDiskFreeSpaceEx(wszPifDiskPathName, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);

    if (fReturn) {
        SysConfig.ulFreeBytesAvailableToCaller = FreeBytesAvailableToCaller.LowPart;
        SysConfig.ulTotalNumberOfBytes = TotalNumberOfBytes.LowPart;
        SysConfig.ulTotalNumberOfFreeBytes = TotalNumberOfFreeBytes.LowPart;
    } else {
        dwError = GetLastError();
    }
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifCloseFileHanle(VOID)                            **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description:close each device handle at finalize or power down recovery **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifCloseFileHandle(USHORT usPowerDown)
{
    /* close file handle */
    EnterCriticalSection(&g_FileCriticalSection);
    PifCloseFileHandleExec(usPowerDown);
    LeaveCriticalSection(&g_FileCriticalSection);
}

VOID PifCloseFileHandleExec(USHORT usPowerDown)
{
    int i;

    /* close file handle */
    for (i=0; i<PIF_MAX_FILE; i++) {
        if (aPifFileTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {
            if (usPowerDown == TRUE) {
                /* close handle by power down recovery */
                if (aPifFileTable[i].fCompulsoryReset == TRUE) {
                    CloseHandle(aPifFileTable[i].hWin32Handle);
                    aPifFileTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
                }
            } else {
                /* unconditional closing */
                CloseHandle(aPifFileTable[i].hWin32Handle);
                aPifFileTable[i].hWin32Handle = INVALID_HANDLE_VALUE;
            }
        }
    }
}

VOID   PifPowerLog(TCHAR *pszThread, TCHAR *pwszString);
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifReopenFile(VOID)                            **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description: reoepn file after power down recovery              **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifReopenFile(VOID)
{
    USHORT  i;
    SHORT   sReturn;
#ifdef DEBUG_PIF_OUTPUT
    TCHAR  wszDisplay[128];
#endif
//    EnterCriticalSection(&g_FileCriticalSection);

    for (i=0; i<PIF_MAX_FILE; i++) {
        if (aPifFileTable[i].sPifHandle >= 0) {
			FlMode  fsMode;
			HANDLE  hHandle;

            if (aPifFileTable[i].hWin32Handle != INVALID_HANDLE_VALUE) {
                continue;
            }

            /* reopen each file */
            fsMode = aPifFileTable[i].fsMode;
            fsMode &= ~FLNEW;

            sReturn = PifOpenFileExec(aPifFileTable[i].wszDeviceName, fsMode, &hHandle);

            if (sReturn == PIF_OK) {
				ULONG   ulActualPosition;

                sReturn = PifSeekFileExec(hHandle, aPifFileTable[i].ulFilePointer, &ulActualPosition, aPifFileTable[i].fsMode, aPifFileTable[i].wszDeviceName, __FILE__, __LINE__);
                if (sReturn == PIF_OK) {
                    aPifFileTable[i].hWin32Handle = hHandle;
                    aPifFileTable[i].ulFilePointer = ulActualPosition;
                } else {
#ifdef DEBUG_PIF_OUTPUT
                    wsprintf(wszDisplay, TEXT("failed by seek file at %s"),  aPifFileTable[i].wszDeviceName);
                    OutputDebugString(wszDisplay);
                    PifPowerLog(_T("RECOVER THREAD"), wszDisplay);
#endif
					PifLog(MODULE_POWER, LOG_ERROR_CODE_FILE_14);
                    aPifFileTable[i].sPifHandle = -1;
                }
            } else {
#if DEBUG_PIF_OUTPUT
                wsprintf(wszDisplay, TEXT("failed by open at %s"),  aPifFileTable[i].wszDeviceName);
                OutputDebugString(wszDisplay);
                PifPowerLog(_T("RECOVER THREAD"), wszDisplay);
#endif
				PifLog(MODULE_POWER, LOG_ERROR_CODE_FILE_15);
                aPifFileTable[i].sPifHandle = -1;
            }
        }
    }
//    LeaveCriticalSection(&g_FileCriticalSection);

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifDeleteChkFile(VOID)                         **
**                                                                  **
**  return:                                                         **
**                                                                  **
**  Description: delete *.chk files in FlashDisk to save disk space **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifDeleteChkFile(VOID)
{
    CONST TCHAR szDirectory[] = TEXT("\\FlashDisk");
    CONST TCHAR szWildCard[]  = TEXT("FILE????.CHK");
    WIN32_FIND_DATA FindFileData;
    HANDLE          hFind;
    TCHAR           szTmp[MAX_PATH];

    /* --- Determine whether *.chk files exist or not --- */
    wcscpy(szTmp, szDirectory);
    wcscat(szTmp, TEXT("\\"));
    wcscat(szTmp, szWildCard);

    hFind = FindFirstFile(szTmp, &FindFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        /* --- Yes, *.chk files are existing in FlashDisk --- */
        do
        {
            /* --- To delete file, make full path of target --- */
            wcscpy(szTmp, szDirectory);
            wcscat(szTmp, TEXT("\\"));
            wcscat(szTmp, FindFileData.cFileName);

            /* --- Delete the CHK file found by Win32 OS --- */
            DeleteFile(szTmp);
        }
        /* --- Determine more *.CHK files exist or not? --- */
        while (FindNextFile(hFind, &FindFileData));

        /* --- Close "Find Handle" before exit this routine --- */
        FindClose(hFind);
        
        PifLog(MODULE_SYS_STARTUP, LOG_EVENT_POWER_UP_CLEAR_CHK);   /* CHK file delete log */
    }
    /* --- All *.chk files are removed from flash fisk... --- */
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT  PIFENTRY PifFileToFile(USHORT usFile        **
**                                          CONST VOID FAR *pBuffer **
**                                          USHORT usBytes)         **
**              usFile:         file handle                         **
**              pBuffer:        writing buffer                      **
**              usBytes:        sizeof pBuffer                      **
**                                                                  **
**  return:     number of bytes to be written                       **
**                                                                  **
**  Description:writing file                                        **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifFileToFile(SHORT sDestFileHandle, SHORT sSrcFileHandle,
								ULONG ulDestOffset, ULONG ulSrcOffset,
								ULONG ulSrcFileLen)
{
	UCHAR	auchStoreBuffer[1024];
	ULONG	pulActualPosition, ulTotalWriteBytes, ulBytesWritten= 0;
	ULONG	ulActualBytesRead, ulTotalOffset;

	ulTotalWriteBytes = ulSrcFileLen;
	ulTotalOffset = ulDestOffset;

	while(ulBytesWritten < ulTotalWriteBytes)
	{
		PifSeekFile(sSrcFileHandle, ulSrcOffset, &pulActualPosition);		
		PifReadFile(sSrcFileHandle, auchStoreBuffer, sizeof(auchStoreBuffer), &ulActualBytesRead);

		PifSeekFile(sDestFileHandle, ulTotalOffset, &pulActualPosition);
		PifWriteFile(sDestFileHandle, auchStoreBuffer, ulActualBytesRead);

		ulBytesWritten += (ulActualBytesRead);
		ulTotalOffset += (ulActualBytesRead);
		ulSrcOffset += (ulActualBytesRead) ;

		if(!ulActualBytesRead)
		{
			return SUCCESS;
		}
	}

	return SUCCESS;
	
}
/* end of piffile.c */