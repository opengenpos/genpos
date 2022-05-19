// NHPOS_filetool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NHPOS_filetool.h"
#include "ej_header.h"

#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

EJF_HEADER  headerFile1 = {0};    // file #1 so that we can allow file comparisons later.

wchar_t  szOpenAsType[128] = {0};
wchar_t  szExportStatus[128] = {0};
wchar_t  szFileOpenPath[512] = {0};


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NHPOS_FILETOOL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NHPOS_FILETOOL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NHPOS_FILETOOL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NHPOS_FILETOOL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExportAsText (EJF_HEADER fileHeader, wchar_t *pBuffer)
{
	EJT_HEADER  myRecPrev = {0};
	EJ_RECORD   myRec;
	wchar_t     outFile[2048];
	HANDLE      hFile = CreateFile(pBuffer, GENERIC_READ, 0,
                    (LPSECURITY_ATTRIBUTES) NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,  (HANDLE) NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwBytesWrite = 0;
		DWORD dwBytesRead = 0;
		long  lReadPoint = fileHeader.ulBeginPoint;
		long  lReadPointLast = fileHeader.ulBeginPoint;
		bool  bProcessFile = true;

		wcscpy_s (outFile, 1000, pBuffer);
		wcscat_s (outFile, 1000, L".txt");

		HANDLE      hFileText = CreateFile(outFile, GENERIC_WRITE, 0,
						(LPSECURITY_ATTRIBUTES) NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,  (HANDLE) NULL);

		SetFilePointer (hFile, lReadPoint, 0, FILE_BEGIN);
		wchar_t  badRecIndic[4] = L"  ";
		while (bProcessFile) {

			memset (&myRec.header, 0, sizeof(myRec.header));
			if (! ReadFile(hFile, &myRec, sizeof(myRec), &dwBytesRead, NULL)) {
				break;
			}
			if (dwBytesRead < sizeof(myRec.header))
				break;
			if (myRec.header.usEjSignature != EJT_HEADER_SIGNATURE) {
				int i;

				if (lReadPoint < 20 || dwBytesRead < 20)
					break;
				// back up to the previous record and re-read the record.
				lReadPoint -= myRecPrev.usCVLI;
				lReadPointLast = lReadPoint;
				SetFilePointer (hFile, lReadPoint, 0, FILE_BEGIN);
				ReadFile(hFile, &myRec, sizeof(myRec), &dwBytesRead, NULL);
				// now look for the signature value so that we can determine where the next record header starts.
				for (i = 0; i < dwBytesRead; i++) {
					if (myRec.text[i] == EJT_HEADER_SIGNATURE) {
						// Found the record header signature so set our file pointer to the proper read loacation
						// so that our next read will be at the beginning of the record header.
						EJT_HEADER  *pRecTemp;

						pRecTemp = (EJT_HEADER  *)&myRec.text[i +1];  // signature is in last USHORT of header so add in its length
						pRecTemp--;
						unsigned long ulDist = (char  *)pRecTemp - (char  *)&myRec;
						lReadPoint += ulDist;
						SetFilePointer (hFile, lReadPoint, 0, FILE_BEGIN);
						wcscpy_s (badRecIndic, 4, L"!!");
						break;
					}
				}
				if (i >= dwBytesRead)
					break;
			} else {
				swprintf_s (outFile, 2000, L"%-2.2s %4.4d (%4.4d), %4.4d, %4.4d on %2.2d/%2.2d @ %2.2d:%2.2d - %s\r\n", badRecIndic, myRec.header.usPVLI, lReadPoint - lReadPointLast, myRec.header.usCVLI, myRec.header.usConsecutive,
					myRec.header.usDate/100, myRec.header.usDate % 100, myRec.header.usTime/100, myRec.header.usTime%100,
					myRec.text);
				WriteFile (hFileText, outFile, wcslen (outFile) * sizeof(outFile[0]), &dwBytesWrite, NULL);
				lReadPointLast = lReadPoint;
				lReadPoint += myRec.header.usCVLI;
				SetFilePointer (hFile, lReadPoint, 0, FILE_BEGIN);
				myRecPrev = myRec.header;
				wcscpy_s (badRecIndic, 4, L"  ");
			}
		}
		CloseHandle (hFile);
		CloseHandle (hFileText);
	}
}

void OpenFileDisplayData (HWND hWnd, wchar_t *pBuffer)
{
	HANDLE hFile = CreateFile(pBuffer, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  (HANDLE) NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD  dwBytesRead = 0;

		ReadFile(hFile, &headerFile1, sizeof(headerFile1), &dwBytesRead, NULL);
		CloseHandle (hFile);
	} else {
		memset (&headerFile1, 0, sizeof(headerFile1));
	}

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

BOOL GetOpenFileDialog (HWND hWnd, wchar_t *pBuffer, int nLen)
{
	OPENFILENAME ofn = {0};       // common dialog box structure

	// Initialize OPENFILENAME
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pBuffer;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = nLen;
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	return GetOpenFileName(&ofn);
}

BOOL  ProcessPaintMessage (HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	/* -- Default font is the current font but below we will want to change point size ----- */
	SelectObject(hdc, hInst);

    /* ----- Set the Coodinates of the BackGround and ForeGround Rects ----- */
    RECT   rTitle, rClientWnd;

	GetClientRect(hWnd, &rClientWnd);

	rTitle.left   = rClientWnd.left + 25;
    rTitle.top    = rClientWnd.top + 25;
    rTitle.right  = 425;
    rTitle.bottom = rClientWnd.bottom;

	/* ----- Set the Current Text Color to BLACK ----- */
    SetTextColor(hdc, RGB(0, 0, 0));

    /* ----- Set the Text-Alignment Flags for the Device Context ----- */
    SetTextAlign(hdc, TA_LEFT | TA_TOP);

	/*-----------------------------------------------------------------------*/
	/*-------create the font we will use for our file description.-----------*/
	/*---- we want to use a 14 point font size with the ANSI character set---*/
	/*---- Title will use 14 point, underline, will change for description---*/
	LOGFONT		logfont = {0};

	logfont.lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	logfont.lfCharSet = ANSI_CHARSET ;
	logfont.lfUnderline = FALSE;

	HFONT		hDescFont = 0, oldFont = 0, tmpFont = 0;
	hDescFont = CreateFontIndirect (&logfont);
	if (hDescFont)
		oldFont = static_cast<HFONT>(SelectObject (hdc, static_cast<HGDIOBJ>(hDescFont)));

	wchar_t textBuffer[1024];

	swprintf_s (textBuffer, 1024, L"File: %s\n\tType: %s\n\tStatus: %s\n\nFlags 0x%4.4x\nSize in Bytes %d\nNo. Lines %d\nStart Time Stamp  %2.2d/%2.2d %2.2d:%2.2d", 
		szFileOpenPath,
		szOpenAsType,
		szExportStatus,
		headerFile1.fchEJFlag,
		headerFile1.ulEJFSize,
		headerFile1.usTtlLine,
		headerFile1.usMonth,
		headerFile1.usDay,
		headerFile1.usHour,
		headerFile1.usMin);

    /* ---------------- Write the String within the Rectangular Region ------- */
	DrawTextW(hdc, textBuffer, wcslen(textBuffer), &rTitle, DT_LEFT | DT_EXPANDTABS |DT_NOCLIP);

	// Finally, we clean up the created font so as to be a good Windows app.
	if (hDescFont) {
		tmpFont = static_cast<HFONT>(SelectObject (hdc, static_cast<HGDIOBJ>(oldFont)));
		DeleteObject (tmpFont);
	}

	EndPaint(hWnd, &ps);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_FILE_OPEN:
			wcscpy_s (szOpenAsType, 48, L"GenPOS TOTALEJF Format");
			wcscpy_s (szExportStatus, 48, L"");
			GetOpenFileDialog (hWnd, szFileOpenPath, 510);
			OpenFileDisplayData (hWnd, szFileOpenPath);
			break;
		case ID_FILE_OPENRIS:
			wcscpy_s (szOpenAsType, 48, L"RIS Data Format");
			wcscpy_s (szExportStatus, 48, L"");
			GetOpenFileDialog (hWnd, szFileOpenPath, 510);
			memset (&headerFile1, 0, sizeof(headerFile1));
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case ID_FILE_EXPORTASTEXT:
			ExportAsText (headerFile1, szFileOpenPath);
			wcscpy_s (szExportStatus, 48, L"Export Complete");
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		 ProcessPaintMessage (hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
