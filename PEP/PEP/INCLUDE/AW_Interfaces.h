

#if !defined(AW_INTERFACES_H_INCLUDE)
#include <windows.h>
#include <commdlg.h>
#include <TCHAR.H>

#define AW_INTERFACES_H_INCLUDE

#define WIDE(s) L##s

// following two routines are utility routines used to convert between UCHAR
// and TCHAR arrays.  These routines are required when moving data between
// a part of the PEP database file, which uses UCHAR variables for text strings,
// and the Unicode compatible rest of the application.
// While these routines are current stored here, they need to be placed in
// a standard library for use by all routines.

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL	bWeAreWindowsNT;  /* indicates whether we think we are Windows NT or not */

typedef struct tagEditBoxProc {
	HWND     hDlg;
	WORD     wID;
	HWND     hWndEditBox;
	WNDPROC  oldEditBoxProc;
	WNDPROC  myEditBoxProc;
} EDITBOXPROC, *PEDITBOXPROC;

LRESULT CALLBACK myEditBoxProcMem (EDITBOXPROC *mem, UINT msg, WPARAM wParam, LPARAM lParam);
EDITBOXPROC *EditBoxProcFactory (EDITBOXPROC *mem, HWND hdlg, WORD wID);

BOOL IsWindowsNT(void);

UINT setCodePage (UINT iCodePage);
UINT getCodePage (void);

HCURSOR LoadPepCursor(HINSTANCE hInstance, LPCWSTR lpCursorName);
HICON LoadPepIcon(HINSTANCE hInstance, LPCWSTR lpIconName);
HMENU LoadPepMenu(HINSTANCE hInstance, LPCWSTR lpMenuName);
HMODULE LoadPepLibrary(LPCWSTR lpFileName);

HDC CreateDCPep(LPCWSTR lpszDriver, LPCWSTR lpszDevice, LPCWSTR lpszOutput, CONST DEVMODEW * lpInitData);

int DrawTextPep(HDC hDC, LPCWSTR lpString, int nCount, LPRECT lpRect, UINT uFormat);

int LoadFileCaption(HINSTANCE hResourceDll, UINT uID);

LRESULT SendThatUnicodeString (HWND hDlg, int nCtlID, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT SendUnicodeStringMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//BOOL StaticBoxRedrawText (HWND hdlg, int wID, UINT idString, size_t stLen);
UINT DlgItemGetText (HWND hdlg, int wID, LPWSTR szString, int nMax);
BOOL DlgItemRedrawText (HWND hdlg, int wID, LPCWSTR idString);
BOOL WindowRedrawText (HWND hdlg, LPCWSTR idString);
int MessageBoxPopUp (HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
int MessageBoxPopUpFileCaption(HWND hWnd, LPCWSTR lpText, UINT uType);

int SetGlobalFont(HINSTANCE hInst, UINT wID);

DWORD GetPepModuleFileName(HMODULE hModule, LPWSTR lpFileName, DWORD nSize);

INT_PTR DialogBoxPopupParam(HINSTANCE hInstance, LPCWSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND DialogCreation(HINSTANCE hInstance, LPCWSTR lpName, HWND hWndParent, DLGPROC lpDialogFunc);
HWND CreateMainPep(LPCTSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
				   int X, int Y, int nWidth, int nHeight,
				   HWND hWndParent , HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

int wsPepf(LPWSTR lpOut, LPCWSTR lpFormat, ...);

BOOL GetPepOpenFileName(LPOPENFILENAMEW ofW);
BOOL GetPepSaveFileName(LPOPENFILENAMEW ofW);

UINT GetWindowsDirectoryPep(LPWSTR lpBuffer, UINT uSize);
UINT GetSystemDirectoryPep(LPWSTR lpBuffer, UINT uSize);

HANDLE CreateFilePep(LPCWSTR lpFileName, DWORD dwDesiredAccess,
  DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
  HANDLE hTemplateFile);
BOOL DeleteFilePep(LPCWSTR lpFileName);

DWORD GetProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);

BOOL WritePrivateProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName);
DWORD GetPrivateProfileStringPep(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);

HINSTANCE ShellExecutePep(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);

BOOL WinHelpPep(HWND hWndMain, LPCWSTR lpszHelp, UINT uCommand, DWORD dwData);
BOOL IsPepCharAlphaNumeric( WCHAR ch);

#define DlgItemSendTextMessage  SendThatUnicodeString

#define WindowSendTextMessage  SendUnicodeStringMsg

#define DialogBoxPopup(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxPopupParam(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

#ifdef __cplusplus
};
#endif

#endif
