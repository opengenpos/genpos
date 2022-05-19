#include "stdafx.h"
#include "PluUtilities.h"
#include <atlbase.h>
#include <winreg.h>

CString	m_strCurrentDir; /* Current Directory */

BOOL	OpenDir(const SHORT nType)
{
	TCHAR	cBuf[MAX_PATH];
	BOOL	bSts;

	switch(nType)
	{
		case s_DataBase:
				bSts = GetPathString(PATH, DATABASE, cBuf);
				if (! bSts) bSts = GetPathString(PATH, DATABASE, cBuf);
		break;

		case s_TempBase:
				bSts = GetPathString(TEMPPATH, DATABASE, cBuf);
				if (! bSts) bSts = GetPathString(TEMPPATH, DATABASE, cBuf);
		break;

		case s_SavedTotals:
				bSts = GetPathString(PATH, DATABASE, cBuf);
				if (! bSts) bSts = GetPathString(PATH, DATABASE, cBuf);
				_tcscat (cBuf, _T("SavedTotal\\"));
		break;

		default:
				bSts = FALSE;
	}

	if(bSts)
	{
		m_strCurrentDir = cBuf;
	}
	else
	{
		m_strCurrentDir.Empty();
	}

	return	bSts;
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

const TCHAR wszFilePathFlashDisk[] = TEXT("\\FlashDisk\\NCR\\Saratoga");
const TCHAR wszFilePathTempDisk[] =  TEXT("\\TempDisk\\NCR\\Saratoga");
const TCHAR wszFilePathDataBase[] = TEXT("Database");
const TCHAR wszFilePathLog[] = TEXT("Log");
const TCHAR wszFilePathProgram[] = TEXT("Program");
const TCHAR wszFilePathSavedTotal[] = TEXT("SavedTotal");
const TCHAR wszFilePathPrintFiles[] = TEXT("PrintFiles");
const TCHAR wszFilePathIcon[] = TEXT("Icons");
const TCHAR wszFilePathImages[] = TEXT("Images");
const TCHAR wszFilePathConfig[] = TEXT("\\POSNAME");     // standard path for FLCONFIG as used for Amtrak

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

VOID PifSetDiskNamePath(VOID)
{
    
    TCHAR   wszKey[128];
    TCHAR  wszPath[MAX_PATH];
    TCHAR  wszBase[MAX_PATH];
    TCHAR  wszLog[MAX_PATH];
    TCHAR  wszData[MAX_PATH];
    LONG   lRet;
    HKEY hKey;
//    HANDLE hKey;
    DWORD   dwDisposition, dwType, dwBytes;
    
    // attempt to create the key
    // for future hard disk drive

    memset(wszPath, 0, sizeof(wszPath));
    memcpy(wszPath, wszFilePathFlashDisk, sizeof(wszFilePathFlashDisk));
    memset(wszBase, 0, sizeof(wszBase));
    memcpy(wszBase, wszFilePathDataBase, sizeof(wszFilePathDataBase));
    memset(wszLog, 0, sizeof(wszLog));
    memcpy(wszLog, wszFilePathLog, sizeof(wszFilePathLog));

    wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, FILEKEY);

    lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

    if (lRet == ERROR_SUCCESS) {

        memset(wszData, 0, sizeof(wszData));
        dwBytes = sizeof(wszData);

        lRet = RegQueryValueEx(hKey, PATH, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);

        if (lRet == ERROR_SUCCESS) {
            memset(wszPath, 0, sizeof(wszPath));
            if (wszData[0] != TEXT('\\')) {                                 /* \Flash Disk */
                wsprintf(wszPath, TEXT("\\%s"), wszData);
            } else if ((wszData[0] == TEXT('\\')) && (wszData[1] == TEXT('\\'))) { /* \\Flash Disk */
                wsprintf(wszPath, TEXT("%s"), &wszData[1]);
            } else {                                                        /* Flash Disk */
                wsprintf(wszPath, TEXT("%s"), wszData);
            }
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

    lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

    if (lRet == ERROR_SUCCESS) {
        memset(wszData, 0, sizeof(wszData));
        dwBytes = sizeof(wszData);

        lRet = RegQueryValueEx(hKey, TEMPPATH, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);

        if (lRet == ERROR_SUCCESS) {
            memset(wszPath, 0, sizeof(wszPath));
            if (wszData[0] != TEXT('\\')) {                                 /* \Flash Disk */
                wsprintf(wszPath, TEXT("\\%s"), wszData);
            } else if ((wszData[0] == TEXT('\\')) && (wszData[1] == TEXT('\\'))) { /* \\Flash Disk */
                wsprintf(wszPath, TEXT("%s"), &wszData[1]);
            } else {                                                        /* Flash Disk */
                wsprintf(wszPath, TEXT("%s"), wszData);
            }
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

BOOL GetPathString(LPCTSTR lpPathKey,LPCTSTR lpDirKey,LPTSTR lpszPath)
{
	TCHAR	szDirBuf[MAX_DIRECTORY_LENGTH];	
	TCHAR	szKeyNameBuf[MAX_REGKEY_LENGTH];
	LONG	lSts1,lSts2 = ERROR_SUCCESS;
	DWORD	dwSize1 = MAX_DIRECTORY_LENGTH,dwSize2 = MAX_DIRECTORY_LENGTH;
	BOOL	bRet = FALSE;
	CRegKey regKey;
	LONG    lResult;

	/* ---- Open registry ---- */
	wsprintf(szKeyNameBuf,_T("%s\\%s"),PIFROOTKEY,FILEKEY);
	lResult = regKey.Open(HKEY_LOCAL_MACHINE, szKeyNameBuf, KEY_READ);

	if (lResult == ERROR_SUCCESS) {
		/* ----  read regs ---- */
//		lSts1 = regKey.QueryValue(lpszPath,lpPathKey,&dwSize1);
		lSts1 = regKey.QueryStringValue(lpPathKey, lpszPath, &dwSize1);		//VS2k5 compat

		if(lpDirKey != NULL){
//			lSts2 = regKey.QueryValue(szDirBuf,lpDirKey,&dwSize2);
			lSts2 = regKey.QueryStringValue(lpDirKey, szDirBuf, &dwSize2);
		}

		/* ----  make path string ---- */
		if(lSts1 == ERROR_SUCCESS && lSts2 == ERROR_SUCCESS)
		{
			if(lpDirKey != NULL)
				MakePathString(lpszPath,szDirBuf);
			else
				MakePathString(lpszPath,NULL);
			bRet = TRUE;
		}

		regKey.Close();
	} else {
		PifSetDiskNamePath ();
	}

	return	bRet;
}

void MakePathString(LPTSTR szPath,LPCTSTR szDir)
{
	CString	strPath(szPath);
	CString	strDir;

	if(szDir != NULL)
	{
		if(szDir[0] == _T('\\'))
			strDir = &szDir[1];
		else
			strDir = szDir;

		if(strPath.Right(1) != _T("\\"))
			strPath += _T("\\");
		strPath += strDir;
	}

	if(strPath.Right(1) != _T("\\"))
		wsprintf(szPath,_T("%s\\"),(LPCTSTR)strPath);
	else
		wsprintf(szPath,_T("%s"),(LPCTSTR)strPath);
}

BOOL	DelFiles(LPCTSTR lpcFname)
{
	if(m_strCurrentDir.IsEmpty())
	{
		return	FALSE;
	}

	CString	strPath(m_strCurrentDir);

	if(lpcFname == NULL)
	{
		strPath += _T("*.*");
	}
	else
	{
		strPath += lpcFname;
	}

	WIN32_FIND_DATA	w32fdInfo;
	HANDLE	hd = ::FindFirstFile((LPCTSTR)strPath,&w32fdInfo);
	BOOL	bFindSts;

	if(hd == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	do
	{
		CString	strPath;
		if(!(w32fdInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			strPath = m_strCurrentDir;
			strPath += w32fdInfo.cFileName;

			/* ---- Remove read only attribute ---- */
			::SetFileAttributes((LPCTSTR)strPath, w32fdInfo.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);			
			::DeleteFile((LPCTSTR)strPath);
		}

		bFindSts = ::FindNextFile(hd,&w32fdInfo);

	}while(bFindSts);

	::FindClose(hd);

	return	TRUE;
}

// For PCI-DSS 2.0 standards for cardholder data security we
// provide a function that will overwrite an existing file with
// random numbers before deleting it.  We do this so that even if the
// deleted file is recovered, it will still be useless.
BOOL	OverWriteDelFiles(LPCTSTR lpcFname)
{
	if(m_strCurrentDir.IsEmpty())
	{
		return	FALSE;
	}

	CString	strPath(m_strCurrentDir);

	if(lpcFname == NULL)
	{
		strPath += _T("*.*");
	}
	else
	{
		strPath += lpcFname;
	}

	// Create a buffer of random digits which we will use to overwrite the
	// file contents before we delete it.
	unsigned char OverWriteBuffer[1028 * sizeof(unsigned long)];
	for (int i = 0; i < 1024; i++) {
		OverWriteBuffer[i] = rand();
	}

	WIN32_FIND_DATA	w32fdInfo;
	HANDLE	hd = ::FindFirstFile((LPCTSTR)strPath,&w32fdInfo);
	BOOL	bFindSts;

	if(hd == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	do
	{
		CString	strPath;
		if(!(w32fdInfo.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM)))
		{
			HANDLE  hHandle;

			strPath = m_strCurrentDir;
			strPath += w32fdInfo.cFileName;

			/* ---- Remove read only attribute ---- */
			::SetFileAttributes((LPCTSTR)strPath, w32fdInfo.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);			

			hHandle = CreateFile(strPath,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ, 
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				0);

			if (hHandle != INVALID_HANDLE_VALUE) {
				DWORD   dwFileSizeLow, dwFileSizeHigh, dwNumberOfBytes;

				dwFileSizeLow = GetFileSize (hHandle, &dwFileSizeHigh);
				long long llFileSize = dwFileSizeLow;
				do {
					WriteFile(hHandle, OverWriteBuffer, sizeof(OverWriteBuffer), &dwNumberOfBytes, NULL);
					llFileSize -= dwNumberOfBytes;
				} while (llFileSize > 0);
				CloseHandle(hHandle);
			}
			::DeleteFile(strPath);
		}

		bFindSts = ::FindNextFile(hd,&w32fdInfo);

	}while(bFindSts);

	::FindClose(hd);

	return	TRUE;
}

void CloseDir(void)
{
	m_strCurrentDir.Empty();
}