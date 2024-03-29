
#include "StdAfx.h"
#include <malloc.h>
#include <mbctype.h>
#include <mbstring.h>
#include <stdlib.h>
#include <shellapi.h>
#include <wchar.h>

#include "pepcomm.h"


static UINT myCodePage = 0;               // contains the current code page, _getmbcp and _setmbcp are unreliable
static WCHAR szFileCaption[PEP_STD_DESC_LEN];

BOOL	bWeAreWindowsNT = TRUE;  /* indicates whether we think we are Windows NT or not */

BOOL IsWindowsNT(void)
{
#if 0
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
	  // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

	   osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	  if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
		 return FALSE;
   }

	bWeAreWindowsNT = FALSE;
    switch (osvi.dwPlatformId)
    {
		case VER_PLATFORM_WIN32_NT:
		// Windows NT/2000 so UNICODE allowed
		bWeAreWindowsNT = TRUE;
		break;

		case VER_PLATFORM_WIN32_WINDOWS:
			// Windows 95/98/ME so no UNICODE allowed
			bWeAreWindowsNT = FALSE;
		break;
	}
#endif
	bWeAreWindowsNT = TRUE;
	return bWeAreWindowsNT;
}

UINT setCodePage (UINT iCodePage)
{
	UINT cp = myCodePage;

	_setmbcp (iCodePage);

	myCodePage = iCodePage;

	return cp;
}

UINT getCodePage (void)
{
	return myCodePage;
}


HCURSOR LoadPepCursor(HINSTANCE hInstance, LPCWSTR lpCursorName)
{
	return LoadCursor(hInstance, lpCursorName);
}

HICON LoadPepIcon(HINSTANCE hInstance, LPCWSTR lpIconName)
{
	return LoadIcon(hInstance, lpIconName);
}

HMENU LoadPepMenu(HINSTANCE hInstance, LPCWSTR lpMenuName)
{
	return LoadMenu(hInstance, lpMenuName);
}

HMODULE LoadPepLibrary(LPCWSTR lpFileName)
{
	return LoadLibrary(lpFileName);
}

HDC CreateDCPep(LPCWSTR lpszDriver, LPCWSTR lpszDevice, LPCWSTR lpszOutput, CONST DEVMODEW * lpInitData)
{
	return CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);
}

int DrawTextPep(HDC hDC, LPCWSTR lpString, int nCount, LPRECT lpRect, UINT uFormat)
{
	return DrawText(hDC, lpString, nCount, lpRect, uFormat);
}


int LoadFileCaption(HINSTANCE hResourceDll, UINT uID)
{
	return LoadString(hResourceDll, uID, szFileCaption, PEP_STRING_LEN_MAC(szFileCaption));
}


LRESULT SendThatUnicodeString (HWND hDlg, int nCtlID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
      return SendDlgItemMessage(hDlg, nCtlID, uMsg, wParam, lParam);
}



LRESULT SendUnicodeStringMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return SendMessage(hDlg, uMsg, wParam, lParam);
}


//  The following code is designed to work around the limitations of the edit box
//  control.  It allows the use of an IME with PEP by chaining our own special
//  edit box control window procedure in front of the standard edit box control
//  procedure.  We do this to allow us to handle the WM_IME_COMPOSITION message
//  from an IME.
//  The function ImmGetCompositionStringW is part of Imm32.lib and is supported
//  as a Unicode function under both Windows NT/2000 and Windows 98.

LRESULT CALLBACK myEditBoxProcMem (EDITBOXPROC *mem, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HIMC hIMC;
	DWORD dwSize;
	WCHAR *szMyBuffer;
	char *szMyTbuf;

	if (msg == WM_IME_COMPOSITION) 
			if (lParam & GCS_RESULTSTR) 
			{
				hIMC = ImmGetContext(mem->hWndEditBox);

				// Find out the size of the result string (len specified as zero)
				// does not include a terminating zero character
				dwSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);

				// increase buffer size for NULL terminator, 
				//   maybe it is in UNICODE
				// ImmGetCompositionStringW provides the number of bytes required to
				// store the string.  This byte count will work whether UNICODE or
				// not.  Since MBCS may be multi-byte, we assume a wide character
				// for the end of string as well.
				dwSize += sizeof(WCHAR);

				szMyBuffer = (WCHAR *) _alloca (dwSize);
				szMyTbuf = (char *) _alloca (dwSize);

				// Get the result strings that is generated by IME into lpstr.
				// We force the use of the wide version of ImmGetCompositionString
				// because it doesn't work if we dont when compiled under _MBCS.
				ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, szMyBuffer, dwSize);

				ImmReleaseContext(mem->hWndEditBox, hIMC);

				// add this string into text buffer of application
				SetWindowText (mem->hWndEditBox, szMyBuffer);
				InvalidateRect (mem->hWndEditBox, NULL, TRUE);
				return FALSE;
			}
	return CallWindowProc (mem->oldEditBoxProc, mem->hWndEditBox, msg, wParam, lParam);
}

static WNDPROC oldEditBoxProc = NULL;

static LRESULT CALLBACK myEditBoxProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	EDITBOXPROC mem;

	mem.hDlg = NULL;
	mem.wID = 0;
	mem.hWndEditBox = hWnd;
	mem.myEditBoxProc = (WNDPROC)myEditBoxProcMem;
	mem.oldEditBoxProc = oldEditBoxProc;

	return myEditBoxProcMem (&mem, msg, wParam, lParam);
}


EDITBOXPROC * EditBoxProcFactory (EDITBOXPROC *mem, HWND hdlg, WORD wID)
{
	WNDPROC savEditBoxProc = NULL;
	WNDPROC tempEditBoxProc = myEditBoxProc;

	//do nothing empty statement
	return mem;
}




UINT DlgItemGetText (HWND hdlg, int wID, LPWSTR szString, int nMax)
{
    /* ----- Set Loaded Strings to StaticText ----- */
	return GetDlgItemText(hdlg, wID, szString, nMax);
}

BOOL DlgItemRedrawText (HWND hdlg, int wID, LPCWSTR szString)
{
    /* ----- Set Loaded Strings to StaticText ----- */
	return SetDlgItemText(hdlg, wID, szString/*szMyBuffer*/);
}

int MessageBoxPopUp (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{

//	int nMsgMax = _mbstrlen (lpText);
//	WCHAR *szMyMessage = (WCHAR *) _alloca ((nMsgMax + 2) * sizeof (WCHAR));
//	memset(szMyMessage,0,(nMsgMax + 2) * sizeof (WCHAR));
//	MultiByteToWideChar (myCodePage, 0, lpText, -1, szMyMessage, nMsgMax);

//	int nCaptionMax = _mbstrlen (lpCaption);
//	WCHAR *szMyCaption = (WCHAR *) _alloca ((nCaptionMax + 2) * sizeof (WCHAR));
//	memset(szMyCaption,0,(nCaptionMax + 2) * sizeof (WCHAR));
//	MultiByteToWideChar (myCodePage, 0, lpCaption, -1, szMyCaption, nCaptionMax);

    /* ----- Set Loaded Strings to StaticText ----- */
	return MessageBox(hWnd, lpText/*szMyMessage*/, lpCaption/*szMyCaption*/, uType);
}

int MessageBoxPopUpFileCaption(HWND hWnd, LPCWSTR lpText, UINT uType)
{
	return MessageBoxPopUp(hWnd, lpText, szFileCaption, uType);
}


INT_PTR DialogBoxPopupParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return DialogBoxParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
}

BOOL WindowRedrawText (HWND hdlg, LPCWSTR szString)
{
    /* ----- Set Loaded Strings to StaticText ----- */
	return SetWindowText(hdlg, szString);
}

HWND DialogCreation(HINSTANCE hInstance, LPCWSTR lpName, HWND hWndParent, DLGPROC lpDialogFunc)
{
	return CreateDialog(hInstance, lpName, hWndParent, lpDialogFunc);
}

DWORD GetPepModuleFileName(HMODULE hModule, LPWSTR lpFileName, DWORD nSize)
{
	return GetModuleFileName(hModule, lpFileName, nSize);
}

DWORD GetPepTemporaryFolder (HMODULE hModule, LPWSTR lpFileName, DWORD nSize)
{
	const char *pTempFolderDefault = "/temp";
	const char *pUserName = getenv ("username");
	const char *pTempFolder = (pTempFolder = getenv ("temp")) ? pTempFolder : pTempFolderDefault;
	int  i;

	i = 0;
	if (pTempFolder) {
		int j = 0;
		for (j = 0; i < nSize - 2 && pTempFolder[j]; i++, j++) {
			lpFileName[i] = pTempFolder[j];
		}
		lpFileName[i] = '\\';  i++;
	}
	if (pTempFolder == pTempFolderDefault && pUserName) {
		int j = 0;
		for (j = 0; i < nSize - 2 && pUserName[j]; i++, j++) {
			lpFileName[i] = pUserName[j];
		}
		lpFileName[i] = '\\';  i++;
		_wmkdir (lpFileName);
	}
	lpFileName[i] = 0;

	return 0;
}

//This will need work for Win98
//we could take all the string identifiers in the format string
//and make them the opposite(NOT RECOMMENDED)
//For example
//when unicode is defined a %s is a LPWSTR and %S is a LPSTR
//when unicode is NOT defined a %s is a LPSTR and %S is a LPWSTR
//we could modify all format strings so that
//we are using the types %ls %lS %lc %lC
//this will need to be done anywhere wsPepf is called
int wsPepf(LPWSTR lpOut, LPCWSTR lpFormat, ...)
{
   va_list marker;

   va_start( marker, lpFormat );            /* Initialize variable arguments. */

	return wvsprintf(lpOut, lpFormat, marker);
}

BOOL GetPepOpenFileName(LPOPENFILENAMEW ofW)
{
	return GetOpenFileName(ofW);
}

BOOL GetPepSaveFileName(LPOPENFILENAMEW ofW)
{
	return GetSaveFileName(ofW);
}

UINT GetWindowsDirectoryPep(LPWSTR lpBuffer, UINT uSize)
{
	return GetWindowsDirectory(lpBuffer, uSize);
}

UINT GetSystemDirectoryPep(LPWSTR lpBuffer, UINT uSize)
{
	return GetSystemDirectory(lpBuffer, uSize);
}


HANDLE CreateFilePep(LPCWSTR lpFileName, DWORD dwDesiredAccess,
  DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
  HANDLE hTemplateFile)
{
	return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
				lpSecurityAttributes, dwCreationDisposition,
				dwFlagsAndAttributes, hTemplateFile);
}

BOOL DeleteFilePep(LPCWSTR lpFileName)
{
	return DeleteFile(lpFileName);
}

DWORD GetProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize)
{
	return GetProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize);
}

BOOL WritePrivateProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName)
{
	return WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
}

DWORD GetPrivateProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault,
								 LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
	return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

HINSTANCE ShellExecutePep(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters,
						  LPCWSTR lpDirectory, INT nShowCmd)
{
	return ShellExecute(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL WinHelpPep(HWND hWndMain, LPCWSTR lpszHelp, UINT uCommand, DWORD dwData)
{
	return WinHelp(hWndMain, lpszHelp, uCommand, dwData);
}

BOOL IsPepCharAlphaNumeric( WCHAR ch)
{
	return IsCharAlphaNumeric(ch);
}
/*END OF unicode_utilities.cpp*/