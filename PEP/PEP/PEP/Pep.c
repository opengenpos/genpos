/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP (Parameter Entry Program) Main Procedure
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEP.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* ---------------------------------------------------------------------------
* Abstract :	This file contains the main entry point for the PEP application.
*				This file also contains the following major functionality:
*				  - language selection table and dialog handler
*				  - main screen display repaint logic displaying PEP file statistics
*				  - main menu processing and dialog display initiation
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add AC 33, 160, 161
* Jan-30-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
* Jun-17-99 : 03.04.01 : hrkato     : Euro Enhancements & Windows 98
* Nov-25-02 :          : P.Griffis  : command line parsing for SR064
* Jan-07-03 :          : R.Chambers : SR100, removed use of GradientFill, using Rectangle
* Jan-07-03				 R.Herrington SR101, Show Title In Tile Bar
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include	<Windows.h>
#include    <stdlib.h>
#include    <string.h>
#include	<malloc.h>
#include	<memory.h>
#include	<tchar.h>
#include	<locale.h>
#include	<time.h>
#include	<stdio.h>

//#include	"stringResource.h" //RPH 4-2-03 Mulilingual PEP
#include    <ecr.h>             /* 2170 common header file              */
#include    <plu.h>
#include    <paraequ.h>         /* PARAMETER EQU header file            */
#include    <para.h>            /* PARAMETER common header file         */

/* NCR2172 */
#include    <pararam.h>         /* RAM Definition                       */


#include    "pepcomm.h"         /* PEP common function header file      */
#include    "pepflag.h"         /* PEP flag control header file         */
#include    "pep.h"             /* PEP main header file                 */
#include    "prog.h"            /* Pragram Mode function header file    */
#include    "action.h"          /* Action Code function header file     */
#include    "file.h"            /* PEP file procedure header file       */
#include    "trans.h"           /* PEP transfer procedure header file   */
#include    "pepprn.h"          /* PEP print procedure header file      */
#include    "report.h"          /* PEP report procedure header file      */

#include    "csop.h"
#include    "plu.h"
#include    "pifwin.hi"         /* PIFWIN.DLL for Win32 check header file */
#include    "stubwin.hi"        /* STUBWIN.DLL for Win32 check header file */

#include	"FileMgGp.h"

#include	"Controls.h"		//Controls used for ActiveX controls

#include	"layout.h"			/*PEP Layout Manager Procedure header file*/

#include	<crtdbg.h>             /* contains the _ASSERT macro stuff */
#include	"RES/resource.h"

// include necessary OLE headers
// and lets make sure that we are linking with the
// OLE libraries.
#ifndef _OBJBASE_H_
	#include <objbase.h>
#endif
#ifndef _OLEAUTO_H_
	#include <oleauto.h>
#endif
#ifndef _OLECTL_H_
	#include <olectl.h>
#endif
#ifndef __ocidl_h__
	#include <ocidl.h>
#endif

#pragma comment(lib, "oledlg.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "olepro32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "urlmon.lib")

/*
	ADDED LIB FILES
*/
#pragma comment (lib, "pepcomm.lib")




/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/

enum selectedLanguage{chineseSimplified = 0, english, french, german, /*greek, italian, japanese, russian,*/ spanish};

typedef struct tagCodePageTable {
	WCHAR  *tcCountry;
	UINT   unFontSize;
	UINT   unCodePage;
	WORD   wLanguage;
	WORD   wSubLanguage;
	UINT   uiCharSet;
	BYTE   ucQuality;
	BYTE   ucPitch;
} CodePageTable, *PCodePageTable;

//  Typically, Asian languages such as Chinese require a 10 pt font for legibility.
//  The following define, ISEL_ENGLISH, is used for the default language.
#define ILISEL_ENGLISH  1
CodePageTable myCodePageTable [] = {
	{WIDE("chinese-simplified"), 10, 936, LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED, GB2312_CHARSET, DEFAULT_QUALITY, FIXED_PITCH},
	{WIDE("english"), 8, 1252, LANG_ENGLISH, SUBLANG_ENGLISH_US, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("french"), 8, 1252, LANG_FRENCH, SUBLANG_FRENCH, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("german"), 8, 1252, LANG_GERMAN, SUBLANG_GERMAN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
/*	{WIDE("greek"), 8, 1253, LANG_GREEK, 0x01, GREEK_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("italian"), 8, 1252, LANG_ITALIAN, SUBLANG_ITALIAN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{WIDE("japanese"), 10, 932, LANG_JAPANESE, 0x01, SHIFTJIS_CHARSET, PROOF_QUALITY, FIXED_PITCH},*/
//	{WIDE("Korean"), 10, 949, LANG_KOREAN, SUBLANG_KOREAN, HANGUL_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH}, /* ESMITH */
/*	{WIDE("russian"), 8, 1251, LANG_RUSSIAN, 0x01, RUSSIAN_CHARSET, PROOF_QUALITY, FIXED_PITCH},*/
	{WIDE("spanish"), 8, 1252, LANG_SPANISH, SUBLANG_SPANISH_MODERN, ANSI_CHARSET, DEFAULT_QUALITY, DEFAULT_PITCH},
	{NULL, 10, 1252, LANG_NEUTRAL, SUBLANG_SYS_DEFAULT, ANSI_CHARSET, PROOF_QUALITY, FIXED_PITCH}
};


void	SaratogaPasswdGetDateStrings(char * szY,char * szM,char * szD){
	SYSTEMTIME timeCur;
	TCHAR	tBuf[10];

	GetSystemTime (&timeCur);

	wsprintf(tBuf,TEXT("%4ld"),(LONG)timeCur.wYear);
	wcstombs(szY,tBuf,4);
	szY[4] = '\0';

	wsprintf(tBuf,TEXT("%02ld"),(LONG)timeCur.wMonth);
	wcstombs(szM,tBuf,2);
	szM[2] = '\0';

	wsprintf(tBuf,TEXT("%02ld"),(LONG)timeCur.wDay);
	wcstombs(szD,tBuf,2);
	szD[2] = '\0';
}


/***************
	Password 	X1 X2 X3 X4 X5 (5 Digits)
	X1	Right digit of Year + 5 right justified digit 
		(1999 would mean: X1 = (9 + 5 = 14) = 4	
	X2	Right digit of Month x 9 right justified digit 
		(Oct = 10) would mean: X2 = (0 * 9 = 0) = 0	
	X3	Last (rightmost) digit of Day of month + 1 right justified digit 
		(Day of month = 12) would mean: X3= (12 + 1 = 13) = 3	
	X4	Right digit of Year + 8 right justified digit 
		(1999 would mean X4: = (9 + 8 = 17) = 7	
	X5	Right digit of Month + 3 right justified digit 
		(Feb = 2) would mean X5: = (2 + 3 = 5) = 5
****************/
BOOL	SaratogaPasswdGenerate(TCHAR	cPassWd[6]){
	char	aYear[10],aMonth[10],aDay[10];
	char	aPassWd[6];
	int	    nX;

	SaratogaPasswdGetDateStrings(aYear,aMonth,aDay);

	// X1	Right digit of Year + 5 right justified digit
	nX = (int)(aYear[3] - '0') + 5;
	aPassWd[0] = (char)(nX % 10) + '0';

	// X2	Right digit of Month x 9 right justified digit
	nX = (int)(aMonth[1] - '0') * 9;
	aPassWd[1] = (char)(nX % 10) + '0';

	// X3	Last (rightmost) digit of Day of month + 1 right justified digit
	nX = (int)(aDay[1] - '0') + 1;
	aPassWd[2] = (char)(nX % 10) + '0';

	// X4	Right digit of Year + 8 right justified digit
	nX = (int)(aYear[3] - '0') + 8;
	aPassWd[3] = (char)(nX % 10) + '0';

	// X5	Right digit of Month + 3 right justified digit
	nX = (int)(aMonth[1] - '0') +3;
	aPassWd[4] = (char)(nX % 10) + '0';
	aPassWd[5] = '\0';

	mbstowcs(cPassWd,aPassWd,5);
	cPassWd[5] = (TCHAR)0;

	// DEBUG m_strPassWd = TEXT("12345");

	return	TRUE;
}

/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
HANDLE      hPepInst;       /* Instance Handle of PEP Window            */
#if defined(POSSIBLE_DEAD_CODE)
// never used
HANDLE      hPepProgInst;   /* Instance Handle of PEPPROG.DLL           */
HANDLE      hPepActInst;    /* Instance Handle of PEPACT.DLL            */
#endif
HWND        hwndPepMain;    /* Window Handle of PEP Window              */
VOID       *lpPepParam;     /* address of default parameter set area    */

HANDLE		hResourceDll;   /* Instance Handle of Resource.DLL			*/
HFONT		hResourceFont;	/* font handle for the standard font to use */
LANGID      xLangId;		/* current language							*/
HANDLE      hProgPepInst;   /* Instance Handle of PEP.EXE               */

/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static HWND		hwndPepDlgChk1; /* Window Handle of Modeless DialogBox		*/
static HWND		hwndPepDlgChk2; /* Window Handle of Modeless DialogBox		*/
static HANDLE	hPepCommInst;       /* Instance Handle of PEPCOMM.DLL       */
static HWND		hPepStat;           /* Window Handle of Status Window       */
static char 	*szModCmmndLine = NULL;

int HandleKeyBoardChange (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis    :   int PASCAL  WinMain()
*
**  Input       :   HANDLE  hInstance   -   Handle of Current Instance
*                   HANDLE  hPrevInst   -   Handle of Previous Instatnce
*                   LPSTR   lpszCmdLine -   Address of Command Line
*                   int     nCmdShow    -   Show-Window Type (Open/Icon)
*
**  Return      :   TRUE                -   User Process is Executed
*                   FALSE               -   Default Process is Expected
*
**  Description :
*       This function is called by the system as the initial entry point for
*      a Windows application.
* ===========================================================================
*/



int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;
    int     nReturn = FALSE;
    BOOL    fChk = FALSE;
	HANDLE hMutex;
	COPYDATASTRUCT cds;
	TCHAR   szMainClass[PEP_CLASSNAME_LEN + 1];


    /* ----- Store instance handle ----- */
#if defined(POSSIBLE_DEAD_CODE)
// never used
//    hPepProgInst = GetDllInst(hInstance, IDS_PEP_NAME_DLL_PROG);
//    hPepActInst  = GetDllInst(hInstance, IDS_PEP_NAME_DLL_ACT);
//    hPepCommInst = GetDllInst(hInstance, IDS_PEP_NAME_DLL_COMM);
    hPepProgInst = hInstance;
	hPepActInst  = hInstance;
#endif
    hPepCommInst = hInstance;
	xLangId      = 0x0409;			/*initialize language identifier to English*/


	IsWindowsNT(); //Call to setup AW_Interfaces to choose which OS

	//Get font with GetLangFont instead of using GetStockObject
	GetLangFont(english);

	hResourceDll = hInstance;  // set the resource DLL to be us for now.  Will change later.
//	hResourceFont = GetStockObject (SYSTEM_FONT);


#ifdef _DEBUG
//	__debugbreak();  // prep for ARM build _asm int 3;  //  temp break when testing
#endif

    /* ----- Initialize DLL ----- */

    ProgInitialize(hInstance);
#if defined(POSSIBLE_DEAD_CODE)
    ActInitialize(hInstance, hPepActInst, hPepCommInst);
#endif
    PepCommInitialize(hInstance, hPepCommInst);
	OpInit ();  //RJC to init semaphore,


	//SR 448, and 487, When the user was double clicking on a Pep file, and pep was
	//already open, Pep would crash.  To alleviate this problem, we now send this
	//file to the already open application.  Below, we try to open a mutex called "PepMain".
	//By doing this we are checking to see if it exists. JHHJ
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, L"PepMain");

	//If the mutex does not exist, we will create it, because this is the first time that
	//the application has been created.
	if (!hMutex)
	{
      // Mutex doesn’t exist. This is
      // the first instance so create
      // the mutex.
      hMutex = CreateMutex(0, 0, L"PepMain");

	  //We create the pep window, because it has not been created.
	  hwndPepMain = InitPepWnd(hInstance, hPrevInst, nCmdShow);
	} else
	{
		//We load the string of the name of the class that PEP is using for its window
		//we will find the window of pep by searching for this class name.
		LoadString(hInstance, IDS_PEP_WINCLASS, szMainClass, PEP_CLASSNAME_LEN);

		hwndPepMain = FindWindow(szMainClass, NULL);

		SetForegroundWindow(hwndPepMain);

		// Command line is not empty. Send the
		// command line in a WM_COPYDATA message.
		if (strlen(lpszCmdLine) != 0) {
			cds.cbData = strlen(lpszCmdLine);
			cds.lpData = lpszCmdLine;
			SendMessage(hwndPepMain, WM_COPYDATA, (WPARAM)cds.cbData, (LPARAM)&cds);
		}

		return 0;
	}
    /* ----- Initialize PEP Window ----- */

	/*
		SR64 - command line parsing to implement double click of PEP file
		Look on the command line to determine if a file was specified.
		We expect to see a pathname if this application was started by
		doubleclicking a .pep file.

		The pathname will be enclosed in doublequotes (").  What we must
		do is to find the first doublequote which indicates the beginning of
		the pathname and then run along the character string until we find the
		closing doublequote we replace with a zero to indicate the end of string.

		We then store the pointer to the beginning of the pathname in the global
		variable szModCmmndLine for use when processing the PM_CMNDLINE_OPEN message.

		Finally, we send ourselves the PM_CMNDLINE_OPEN message which will be processed
		after the window is up and running.
	*/
    if (hwndPepMain) {
		BOOL  bExitApp = FALSE;
		LPSTR lpszFirstCmdLine = NULL;
		for( ; *lpszCmdLine && *lpszCmdLine != '\"' && *lpszCmdLine != '-' && *lpszCmdLine != '/'; lpszCmdLine++) ;
		for ( ; *lpszCmdLine; lpszCmdLine++) {
			switch (*lpszCmdLine) {
				case '\"':
						lpszCmdLine++;
						for(lpszFirstCmdLine = lpszCmdLine; *lpszCmdLine && *lpszCmdLine != L'\"'; lpszCmdLine++) ;
						*lpszCmdLine = '\0';
						if (lpszCmdLine - lpszFirstCmdLine > 0) {
							szModCmmndLine = lpszFirstCmdLine;
							// we now have our pathname.  Send a message to ourselves to open the file
							// and read it in once our window is up and running.
							SendMessage(hwndPepMain, PM_CMNDLINE_OPEN, 0, (LPARAM)szModCmmndLine);
						}
					break;
				case '-':
				case '/':
					// handle additional arguments and options to allow for PEP to export the PLU data into a
					// output file, pep_pluoutput.csv, and then exit.
					{
						FILE *pFile;
						char  aszFileName[512] = "pep_pluoutput.csv";
						extern LONG  A068ExportPluDataFile (FILE *pFile);    // A068.c - export PLU data to a file.

						if ((pFile = fopen (aszFileName, "w")) != NULL) {
							A068ExportPluDataFile (pFile);
							fclose (pFile);
							bExitApp = TRUE;
						}
					}
					break;
			}
		}

		if (bExitApp) SendMessage(hwndPepMain, WM_CLOSE, 0, 0);

        /* ----- Retrieve Message from Application's Message Queue ----- */
         while (GetMessage((MSG     *)&msg, (HWND)NULL, 0, 0)) {
            if (hwndPepDlgChk1 || hwndPepDlgChk2) {
                /* ----- Determine Processes the Message ----- */
                if (IsDialogMessage(hwndPepDlgChk1, (MSG     *)&msg)) {
                    continue;
                }
                if (IsDialogMessage(hwndPepDlgChk2, (MSG     *)&msg)) {
                    continue;
                }
            }

            /* ----- Translate Virtual-Key Messages into Character Messages ----- */
            TranslateMessage((const MSG     *)&msg);

            /* ----- Dispatch a Message to a Window ----- */
            DispatchMessage((const MSG     *)&msg);
        }
        nReturn = (int)msg.wParam;
    }

    /* ----- Finalize DLL ----- */

#if defined(POSSIBLE_DEAD_CODE)
    ProgFinalize();
    ActFinalize();
    PepCommFinalize();

    /* ----- Free library ----- */

//    FreeLibrary(hPepProgInst);
//    FreeLibrary(hPepActInst);
//    FreeLibrary(hPepCommInst);
#endif


    return (nReturn);
    PEP_REFER(lpszCmdLine);


}

/*
* ===========================================================================
**  Synopsis    :   LONG    WINAPI  PepWndProc()
*
**  Input       :   HWND    hWnd    -   Handle of PEP Window
*                   WORD    wMsg    -   Dispatched Message
*                   WORD    wParam  -   16 Bits Message Parameter
*                   LONG    lParam  -   32 Bits Message Parameter
*
**  Return      :   The Result of the Message Processing and depends on the
*                   Message being Processed.
*
**  Description :
*       This Function is an application-defined callback function that processes
*      messages sent to a window.
* ===========================================================================
*/
LONG    WINAPI  PepWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  int         nCyStatDef; /* Default Height of Status Window  */
    static  HGLOBAL     hMemParam;  /* handle of memory for parameter   */

	PCOPYDATASTRUCT		CopyData;
	COPYDATASTRUCT		CopyData2;
    FCONFIG const       *lpFile;    /* Address of File Config Structure */
    TEXTMETRIC          TextMtrc;   /* Structure for Text Metrics       */
    PAINTSTRUCT         Paint;      /* Structure with Paint Information */
    HDC                 hdcPepMain; /* Handle of Device Context         */
    UINT                unEnable;   /* Enable/Disable Menu Action Flag  */
    HANDLE				iLogHandle; /* Log File Handle					*/
    int                 nCyMain;    /* Height of PEP Window Flame       */
//                        nCxStat,    /* Width of Status Window           */
//                        nCyStat;    /* Height of Status Window          */
    WCHAR               szWork[PEP_FILENAME_LEN + 1],
                        szTitle[PEP_CAPTION_LEN + 1],
                        szTimeStamp[PEP_FILENAME_LEN + 1],
						szLogPath[PEP_FILENAME_LEN];
	UCHAR				*uchPath, szPath[PEP_FILENAME_LEN + 1];

    void            *lpBufStart;
    time_t				sysTime;
    DWORD				bytesWritten, bytesToWrite;
    HCURSOR             hCursor;
	USHORT				i, j;
	WCHAR *szMyBuffer = (WCHAR *) _alloca ((PEP_FILENAME_LEN + 2) * sizeof (WCHAR));
	CopyData = &CopyData2;

    switch (wMsg) {

    case    WM_CREATE:
        /* ----- V3.3 for use New STUBWIN.DLL and PIFWIN.DLL (PCI/F for Win32)----- */
        /* ----- Allocate global memory for default parameter set area ----- */

		/*JHHJ, changed GlobalAlloc(GHND, PEP_PARAM_LEN * sizeof(WCHAR))), we now use the actual
		size of Uniniram structure pluse a buffer of 1000 bytesfor safety*/
		if ((hMemParam = GlobalAlloc(GHND, sizeof(Para))) == 0) {
			break;
		}
		if ((lpPepParam = GlobalLock(hMemParam)) == NULL) {
			GlobalFree(hMemParam);
			break;
		}

		lpBufStart = &Para;
        memcpy(lpPepParam, lpBufStart, sizeof(Para));


        /* ----- Retrieve Handle of DC for Client Area of PEP Window ----- */
        hdcPepMain = GetDC(hWnd);

        /* ----- Retrieve Metrics for Current Font ----- */
        GetTextMetrics(hdcPepMain, (TEXTMETRIC     *)&TextMtrc);

        /* ----- Release Handle of DC of PEP Window ----- */
        ReleaseDC(hWnd, hdcPepMain);

        /* ----- Get Height of PEP Window Flame that Cannot be Sized ----- */

        nCyMain = GetSystemMetrics(SM_CYBORDER);

        /* ----- Set Height of Status Window to Static Area ----- */
        nCyStatDef = nCyMain * PEP_STAT_CY + TextMtrc.tmHeight;

        /* ----- Reset Modify Flag ----- */
        PepResetModFlag(PEP_MF_ALL, 0);
        PepResetModFlag(PEP_MF_OPEN, 0);

        /* ----- Delete Temporary File ----- */
        FileTempDelete(hPepInst);
		/* ----- Disable Menu without FILE and HELP ----- */

		//JHHJ, Now when PEP opens, it opens into a new file,
		//Previously, the user had to choose New from the file menu
		//to receive a file or to start working, now they can just start
		//working when PEP opens 12-04-03
		if (FileNew(hPepInst, hWnd) == TRUE) {
            SendMessage(hWnd, PM_PEPMENU, (WPARAM)PEP_MENU_ON, 0L);
         }
	//	PepMenuControl(hWnd, (WPARAM)PEP_MENU_OFF, 0L);
		//Removed by request of RJC

		lpBufStart = &Para;
        memcpy(lpPepParam, lpBufStart, sizeof(Para));

		/* --- Saratoga --- */
		PluInitialize();
		PluInitialize_GP();

		/*------------ Log File Implementation ------------- */
		//Creation of log file
		//Log file will be created in the same folder as PEP.exe
#if 1
		GetPepTemporaryFolder(NULL, szLogPath, PEP_STRING_LEN_MAC(szLogPath));
#else
		GetPepModuleFileName(NULL, szLogPath, sizeof(szLogPath));
		szLogPath[wcslen(szLogPath)-8] = '\0';  //remove pep.exe from path
		wcscat(szLogPath, WIDE("\\"));
#endif
		wcsncat(szLogPath, PEPLOGPATH, 10);
		iLogHandle = CreateFileW(szLogPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
						CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		//Time Stamp
		time(&sysTime);
		memset(szTimeStamp,0,PEP_FILENAME_LEN+1);
		memcpy(szTimeStamp, ctime(&sysTime), 26);
		bytesToWrite = 26;
		WriteFile(iLogHandle,szTimeStamp,bytesToWrite,&bytesWritten,NULL);
		CloseHandle(iLogHandle);
        break;

    case WM_INITMENUPOPUP:
        if (LOWORD(lParam) == PEP_MENU_FILE) {
            unEnable = ((PepQueryModFlag(PEP_MF_OPEN, 0) == FALSE) ? MF_GRAYED : MF_ENABLED);
            EnableMenuItem((HMENU)wParam, IDM_SAVE,   unEnable);
            EnableMenuItem((HMENU)wParam, IDM_SAVEAS, unEnable);
            EnableMenuItem((HMENU)wParam, IDM_CONFIG, unEnable);
            EnableMenuItem((HMENU)wParam, IDM_PRN,    unEnable);
        }
        break;

    case    WM_QUERYENDSESSION:
        if (!(GetMenuState(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND) & PEP_MENU_NOT_ENABLED)) {
            if (FileExit(hPepCommInst, hWnd) == TRUE) {
                /* ----- Destroy PEP Window ----- */
                DestroyWindow(hWnd);

                /* ----- Free memory area ----- */
                GlobalUnlock(hMemParam);
                GlobalFree(hMemParam);
                return ( TRUE );
            }
        }
        break;

    case    WM_CLOSE:
        if (!(GetMenuState(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND) & PEP_MENU_NOT_ENABLED)) {
            if (FileExit(hPepCommInst, hWnd) == TRUE) {
                /* --- Saratoga --- */
                PluFinalize();
                /* ----- Destroy PEP Window ----- */
                DestroyWindow(hWnd);

                /* ----- Free memory area ----- */
                GlobalUnlock(hMemParam);
                GlobalFree(hMemParam);
            }
        }
        break;

    case    WM_DESTROY:
		WinHelp(hWnd, L"pephelp.hlp", HELP_QUIT, 0L);  //make sure Help shutsdown RPH 01/06/03
        PostQuitMessage(0);
        break;

    case    WM_PAINT:
        hdcPepMain = BeginPaint(hWnd, (PAINTSTRUCT     *)&Paint);

		SaveDC (hdcPepMain);
        DrawTitle(hdcPepMain, hWnd);
		RestoreDC (hdcPepMain, -1);

        EndPaint(hWnd, (const PAINTSTRUCT     *)&Paint);
        break;

    case    WM_SIZE:
/*
        // ----- Get Width and Height of Client Area -----

        nCxStat = LOWORD(lParam);
        nCyStat = HIWORD(lParam);

        //----- Change the Position and Dimensions of the Status Window -----

        MoveWindow(hPepStat,
                   0,
                   nCyStat - nCyStatDef + PEP_STAT_Y,
                   nCxStat,
                   nCyStatDef,
                   TRUE);
*/
        break;

    case    PM_TRA_ACCESS:
        /* ----- Set Transaction Flag ----- */
        TransAccessData(wParam);
        break;

    case    PM_GET_FCONF:
        /* ----- Get Address of the File Configulation Structure ----- */
        lpFile = FileConfigGet();

        /* ----- Send the Address of File Configlation to Called Window ----- */
        SendMessage((HWND)lParam, wMsg, 0, (LPARAM)(lpFile));
        break;

    case    PM_SET_TRAFLAG:
        /* ----- Set Transaction Flag ----- */
        TransAccessFlexMem(wParam);
        break;

		// SR64 - open file specified on PEP command line
		// handle a PM_CMMNDLINE_OPEN message from the main.  This message
		// indicates we have a .pep file specified on the command line and
		// we are to load it.  The command line is already modified and all
		// we have to do is use szModCmmndLine as the pointer to the pathname
		// of the file to open.
    case    PM_CMNDLINE_OPEN:
		FileTempDelete(hPepInst);

		MultiByteToWideChar(getCodePage(),0, szModCmmndLine, -1, szMyBuffer, (PEP_FILENAME_LEN + 2) * sizeof (WCHAR));

		if (szModCmmndLine && FileReadInFile(hPepInst, hWnd, szMyBuffer)) {
			PepMenuControl(hWnd, (WPARAM)PEP_MENU_ON, 0L);
		}
		else {
			/* ----- Disable Menu without FILE and HELP ----- */
			PepMenuControl(hWnd, (WPARAM)PEP_MENU_OFF, 0L);
			/* --- Saratoga --- */
			PluInitialize();
			PluInitialize_GP();
		}

		InvalidateRect(hWnd, NULL, TRUE);
        break;


		//SR 448, and 487, handle the WM_COPYDATA event from main.  This
		//message is sent when the user has double clicked a pep file with an instance
		//of pep already running. We pull the path name from lParam then pull the path
		//of the file to be read and send it in. JHHJ
	case WM_COPYDATA:
		//clear my work area
		memset(&CopyData2, 0x00, sizeof(CopyData2));
		memset(&szPath, 0x00, sizeof(szPath));

		//the CopyDataStructure is send via the lParam, so we set
		//it to a Structure
		CopyData = (PCOPYDATASTRUCT)lParam;

		uchPath = (UCHAR *)CopyData->lpData;

		//search through the path and remove the quotation marks from the path
		for(i = 0, j = 0; i < CopyData->cbData;i++)
		{
			switch(*uchPath)
			{
			case '"':
				uchPath++;
				continue;
			default:
				szPath[j] = *uchPath;
				uchPath++;
				j++;
			}
		}

		MultiByteToWideChar(getCodePage(),0, szPath, -1, szMyBuffer, (PEP_FILENAME_LEN + 2) * sizeof (WCHAR));

		if (FileReadInFile(hPepInst, hWnd, szMyBuffer)) {
			PepMenuControl(hWnd, (WPARAM)PEP_MENU_ON, 0L);
		}
;
		break;


    case    PM_PEPMENU:
        /* ----- Enable / Disable a Specified Menu ----- */
        PepMenuControl(hWnd, wParam, lParam);
        break;

	case WM_INPUTLANGCHANGE:
		HandleKeyBoardChange (hWnd, wMsg, wParam, lParam);
		InvalidateRect(NULL,NULL,TRUE);
		break;

	case	WM_HELP:
		GetPepModuleFileName(hPepInst, szWork, PEP_FILENAME_LEN);
		szWork[wcslen(szWork) - 4] = '\0';
		wcscat((szWork), WIDE("help.hlp"));
		WinHelpPep(hWnd, szWork, HELP_FINDER, 0L);  //start help RPH 1/6/03
		break;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
        /* ----- File Functions ----- */
        case    IDM_NEW:        /* New File         */
            /* ----- Change Cursor (Arrow => Hour-Glass) ----- */
            hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(TRUE);
            if (FileNew(hPepInst, hWnd) == TRUE) {
                SendMessage(hWnd, PM_PEPMENU, (WPARAM)PEP_MENU_ON, 0L);
            }

            /* ----- Restore Cursor (Hour-Glass => Arrow) ----- */
			InvalidateRect(hWnd, NULL, TRUE);
			ShowCursor(FALSE);
            SetCursor(hCursor);
			SendMessage(hWnd, WM_PAINT, 0L, 0L);
            break;

        case    IDM_OPEN:       /* Open File        */
            if (FileOpen(hPepInst, hWnd) == TRUE) {
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;

        case    IDM_SAVE:       /* Save File        */
            FileSave(hPepInst, hWnd);
            break;

        case    IDM_SAVEAS:     /* Save File As     */
            FileSaveAs(hPepInst, hWnd);
            break;

        case    IDM_DELETE:     /* Delete File      */
            FileDelete(hPepInst, hWnd);
            break;

        case    IDM_CONFIG:     /* Configuration    */
            FileConfig(hPepInst, hWnd);
            break;

        case    IDM_PRN:        /* Print */
            PepPrn(hWnd);
            break;

        case    IDM_PRN_PRINTER:    /* Printer Setup */
            PepPrnSetup(hWnd);
            break;

		case	IDM_CHANGE_LANGUAGE:
			PepChangeLanguage (hWnd);
			InvalidateRect(NULL,NULL,TRUE);
			break;

		case	IDM_SECRET_CODE:
			{
				TCHAR xBuf[128];
				SaratogaPasswdGenerate(xBuf);
				wcscpy (szWork, L"PW = ");
				wcscat (szWork, xBuf);
				wcscpy (szTitle, L"Secret Code");
				MessageBoxPopUp(hWnd, szWork, szTitle, MB_OK);
			}
			break;

        case    IDM_EXIT:       /* Exit PEP.EXE     */
            SendMessage(hWnd, WM_CLOSE, 0, 0L);
            break;

        /* ----- Setup functions ----- */

        case    IDM_P01:        /* Machine Definition Code (MDC)            */
        case    IDM_P02:        /* Flexible Memory Assignment               */
        case    IDM_P03:        /* Function Selction Code (Key Assignment)  */
        case    IDM_P06:        /* Program Mode Security Code               */
        case    IDM_P08:        /* Supervisor Number/Level                  */
        case    IDM_P09:        /* Action Code Security                     */
        case    IDM_P10:        /* High Amount Lock-out Limit               */
        case    IDM_P15:        /* Preset Amount for Preset Cash Key        */
        case    IDM_P17:        /* Hourly Activity Block                    */
        case    IDM_P18:        /* Slip Print Control                       */
        case    IDD_P20:        /* Transaction Mnemonics                    */
        case    IDD_P21:        /* Lead Through Messages                    */
        case    IDM_P22:        /* Report Mnemonics                         */
        case    IDM_P23:        /* Special Mnemonics                        */
        case    IDM_P46:        /* Adjective Mnemonics                      */
        case    IDM_P47:        /* Print Modifier Mnemonics                 */
        case    IDM_P48:        /* Major Department Mnemonics               */
        case    IDM_P49:        /* Automatic Alternative Remote Printer     */
        case    IDM_P50:        /* Assignment Terminal # Installing Shared Printer */
        case    IDM_P51:        /* Assignment IP Address for KDS    NCR2172 */
        case    IDM_P54:        /* Set Hotel Id and SLd                     */
        case    IDM_P57:        /* Header/Special Messages                  */
        case    IDM_P60:        /* Total Key Type / Function                */

        case    IDM_P62:        /* Tender Key Parameter , V3.3              */
        case    IDM_P65:        /* Multi-Line Display Mnemonics , V3.3      */
        case	IDM_P67:		/* Auto Combinational Coupons				*/ //RLZ
        case	IDM_P72:		/* Reason Code mnemonics				*/ //RLZ
        case    IDM_P97:        /* Terminal information dialog box          */
        case    IDM_P98:        /* Tender Key setup for XEPT with DLL       */
        case    IDD_P223_RPT:   /* Financial Report Setup       */
        case    IDD_P223_1_RPT: /* Cashier Report Setup        */

            ExecuteProg(hWnd, LOWORD(wParam));
			InvalidateRect(hWnd, NULL, TRUE);
            break;

        /* ----- Maintenance functions ----- */

        case    IDM_A04:        /* Set Department Key           */
        case    IDM_A05:        /* Set Page Control             */
        case    IDM_A07:        /* Cashier A/B Key Assinment    */
        case    IDM_A20:        /* Cashier Maintenance          */
        case    IDM_A33:        /* Set Control String Table     */
        // case    IDM_A50:        /* Server Maintenance           */
        case    IDM_A68:        /* PLU Maintenance              */
        case    IDM_A71:        /* Set PPI Table                */
        case    IDM_A84:        /* Set Rounding Table           */
        case    IDM_A86:        /* Set Percent Rates            */
        case    IDM_A87:        /* Set Slip Promotion Header    */
        case    IDM_A88:        /* Set Receipt Promotion Header */
        case    IDM_A89:        /* Set Currency Conversion Rate */
        case    IDM_A111:       /* Set Tare Table               */
        case    IDM_A114:       /* Department(File) Maintenance */
/* DEL Saratoga */
/*        case    IDM_A115:     */  /* Set Department No.on Keyted  */
                                /*     Department Key  NCR2172  */
        case    IDM_A116:       /* Set Promotional PLU          */
        case    IDM_A124:       /* Set Tax Table                */
        case    IDM_A127:       /* Set Tax Rate                 */
        case    IDM_A130:       /* Set Pig Rule Table           */
        case    IDM_A133:       /* Service Time Parameter Maintenance */
        case    IDM_A137:       /* Set Beverage Dispenser Parameter */
        case    IDM_A152:       /* Employee Number Assignment   */
		case    IDM_A154:       /* Set Wage Rate for Job Code   */
		case    IDD_A156:       /* Set suggested tip amount   */
		case    IDM_A160:       /* Order Entry Prompt Table Maintenance */
        case    IDM_A161:       /* Combination Coupon Talbe Maintenance */
        case    IDM_A162:       /* Flexible Drivethru Parameter Maintenance */
        case    IDM_A170:       /* Set Sales Restriction Table of PLU NCR2172 */
        case    IDM_A208:       /* Set Boundary Age     NCR2172 */
		case	IDM_A209:		/* Set Color Palette			*/

        case    IDM_A128:       /* ### ADD Saratoga (051900) */

            ExecuteAct(hWnd, LOWORD(wParam));
            break;

        /* ----- Transfer Functions ----- */
        case    IDM_TERMINAL:   /* Send/Receive to/from Terminal        */
            TransTermExec(hPepInst, hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

        /** removed at saratoga
        case    IDM_LOADER:     / Send/Receive to/from Loader Board    /

            TransLoaderExec(hPepInst, hWnd);
            break;
        **/

        /* ----- Help functions ----- */
		case	IDM_HLP_CONTENTS:
			GetPepModuleFileName(hPepInst, szWork, PEP_FILENAME_LEN);
			szWork[wcslen(szWork) - 4] = '\0';
			wcscat((szWork), WIDE("help.hlp"));
			WinHelpPep(hWnd, szWork, HELP_FINDER, 0L);  //start help RPH 1/6/03
			break;

        case    IDM_HLP_ABOUT:  /* About PEP    */
            DoHelp(hWnd, AboutDlgProc, IDD_HLP_ABOUT);
            break;

        /* ----- Report functions ----- */
        case    IDM_PROGREPORT:     /* execute Programmable Report */
            ReportExec(hPepInst, hWnd);
            break;

        case    IDM_REPORTFILE:     /* Report File     */
            ReportFile(hPepInst, hWnd);
            break;

		/* Layout Files ESMITH LAYOUT */
		case	IDM_LAYOUTTOUCH_LMNGR:
			LayoutExec(hPepInst, hWnd, szCommTouchLayoutSaveName, xLangId);
			break;

		case	IDM_LAYOUTKEYBOARD_LMNGR:
			LayoutExec(hPepInst, hWnd, szCommKeyboardLayoutSaveName, xLangId);
			break;

		case	IDM_LAYOUTTOUCH_SET:
			LayoutFile(hPepInst, hWnd, szCommTouchLayoutSaveName,PEP_MF_LAYTOUCH);
			break;

		case	IDM_LAYOUTKEYBOARD_SET:
			LayoutFile(hPepInst, hWnd, szCommKeyboardLayoutSaveName,PEP_MF_LAYKEYBRD);
			break;

        case    IDM_CURRENTRPTFILE: /* Current Report File Name */
            CurrentReportFile(hPepInst, hWnd);
            break;

		case IDM_EDITLOGO:
			LogoExec(hPepInst, hWnd, szCommLogoSaveName, xLangId);
			break;
		case IDM_CHOOSELOGO:
			DoAction(hWnd,LogoDlgProc,IDD_LOGO, hPepInst);
//			LogoFile(hPepInst, hWnd, szCommLogoSaveName, PEP_MF_LOGO);
			break;

        default:
            LoadString(hResourceDll, IDS_PEP_NOT_IMPLE, szWork, PEP_FILENAME_LEN);
            LoadString(hResourceDll, IDS_PEP_TITLE, szTitle, PEP_CAPTION_LEN);
            MessageBeep(MB_ICONEXCLAMATION);
            MessageBoxPopUp(hWnd, szWork, szTitle, MB_OK);
            break;
        }


	default:
        return (DefWindowProc(hWnd, wMsg, wParam, lParam));
        break;
    }
    return 0L;
}

/*
* ===========================================================================
**  Synopsis    :   LONG    WINAPI  StatusWndProc()
*
**  Input       :   HWND    hWnd    -   Handle of PEP Window
*                   WORD    wMsg    -   Dispatched Message
*                   WORD    wParam  -   16 Bits Message Parameter
*                   LONG    lParam  -   32 Bits Message Parameter
*
**  Return      :   The Result of the Message Processing and depends on the
*                   Message being Processed.
*
**  Description :
*       This Function is an application-defined callback function that processes
*      messages sent to a window.
* ===========================================================================
*/
LONG    WINAPI  StatusWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    return (DefWindowProc(hWnd, wMsg, wParam, lParam));
}
#if defined(POSSIBLE_DEAD_CODE)
// never called

/*
* ===========================================================================
**  Synopsis    :   static  HANDLE  GetDllInst()
*
**  Input       :   HANDLE  hInstance   - handle of current instance
*                   UINT    unID        - DLL ID
*
**  Return      :   Window Handle of PEPCOMM.DLL, if LoadLibrary Function
*                   is Successful. Otherwise, it is NULL.
*
**  Description :
*       This Function loads the common library, and sets loaded instance handle
*      to the public valuable.
* ===========================================================================
*/
static  HANDLE  GetDllInst(HANDLE hInstance, UINT unID)
{
    WCHAR    szDllName[PEP_FILENAME_LEN + 1];
    HANDLE  hLoadInst;

    /* ----- Load DLL Name String from Resource ----- */

    LoadString(hResourceDll, unID, szDllName, PEP_FILENAME_LEN);

    /* ----- Load PEPCOMM.DLL File and Get One's Instance Handle ----- */

    hLoadInst = LoadLibrary((LPCSTR)szDllName);

    return hLoadInst;
}
#endif
//Experimental Section FIX THIS RPH
//#define WIDEPEP	//Start PEP with Wide functions to accomodate Chinese TitleBar
#ifndef WIDEPEP
/*
* ===========================================================================
**  Synopsis    :   static  HWND    InitPepWnd()
*
**  Input       :   HANDLE  hInstance   -   Handle of Current Instance
*                   HANDLE  hPrevInst   -   Handle of Previous Instatnce
*                   int     nCmdShow    -   Show-Window Type (Open/Icon)
*
**  Return      :   Window Handle of PEP Window, if CreateWindow Function
*                   is Successful. Otherwise, it is NULL.
*
**  Description :
*       This function registeres window class, and then creates the PEP Window
*      and  Status Window.
* ===========================================================================
*/
static  HWND InitPepWnd(HANDLE hInstance, HANDLE hPrevInst, int nCmdShow)
{
	ATOM        aRegRet;
    WNDCLASS    wc;
    HWND        hwnd;
    TCHAR       szCaption[PEP_CAPTION_LEN + 1],
                szMainClass[PEP_CLASSNAME_LEN + 1],
                szStatusClass[] = _T("PEP:STATUS");
	long		status;
	WCHAR		szWork[300], szTitle[128];

    /* ----- Load Class Name String of PEP Window ----- */
    LoadString(hInstance, IDS_PEP_WINCLASS, szMainClass, PEP_CLASSNAME_LEN);

    if (! hPrevInst) {

        wc.lpszClassName    = szMainClass,
        wc.hInstance        = hInstance;
        wc.lpfnWndProc      = (WNDPROC)PepWndProc;
        wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon            = LoadIcon(hResourceDll, MAKEINTRESOURCE(IDI_PEPICON1));
        wc.lpszMenuName     = _T("#1");
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.style            = CS_HREDRAW | CS_VREDRAW;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;

        aRegRet = RegisterClass((const WNDCLASS     *)&wc);
		_ASSERT(aRegRet);

        wc.lpszClassName    = szStatusClass;
        wc.hInstance        = hInstance;
        wc.lpfnWndProc      = (WNDPROC)StatusWndProc;
        wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon            = NULL;
        wc.lpszMenuName     = NULL;
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.style            = 0;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;

        aRegRet = RegisterClass((const WNDCLASS     *)&wc);
		_ASSERT(aRegRet);
    }

    hPepInst = hInstance;

    /* ----- Load Window Title String of PEP Window and create the window ----- */
    LoadString(hInstance, IDS_PEP_TITLE, szCaption, PEP_CAPTION_LEN);

    hwnd = CreateWindow(szMainClass,
                        szCaption,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        475/*CW_USEDEFAULT*/,
                        700/*CW_USEDEFAULT*/,
                        0,
                        LoadMenu(hResourceDll,_T("#1")),
                        hInstance,
                        NULL);

    hPepStat = CreateWindow(szStatusClass,
                            NULL,
                            WS_CHILD,// | WS_BORDER | WS_VISIBLE,//remove status window
                            0,
                            0,
                            0,
                            0,
                            hwnd,
                            0,
                            hInstance,
                            NULL);

	SelectObject (GetDC(hwnd), hResourceFont);


//#ifdef _DEBUG
#if 1
//When we are running in DEBUG mode the softlocx control will
//not load and we get our Failed to load message
//we are setting status to 1 in order to be able to be able to continue
#pragma message(" ***  WARNING - CheckLicense disabled due to _DEBUG")
	status = 1;
#else
	status = CheckLicense();  //Softlocx Security
#endif

	if(status == 0)
	{
		//Close Becuase License has expired
		LoadString(hResourceDll, IDS_PEP_LICENSE_CAPTION, szTitle, PEP_STRING_LEN_MAC(szTitle));
        LoadString(hResourceDll, IDS_PEP_LICENSE_EXPIRED,  szWork,  PEP_STRING_LEN_MAC(szWork));
		MessageBoxPopUp(hwnd, szWork, szTitle, MB_OK);

        SendMessage(hwnd, WM_CLOSE, 0, 0L);
	}
	if(status == -1)
	{
		//Close because the Softlocx Control did not load properly
		LoadString(hResourceDll, IDS_PEP_CAPTION_CONTROL, szTitle, PEP_STRING_LEN_MAC(szTitle));
        LoadString(hResourceDll, IDS_PEP_CONTROL_FAILED,  szWork,  PEP_STRING_LEN_MAC(szWork));
		MessageBoxPopUp(hwnd, szWork, szTitle, MB_OK);

        SendMessage(hwnd, WM_CLOSE, 0, 0L);
	}


    ShowWindow(hwnd, SW_SHOW);  //RPH 12/11/02 SR#78

    UpdateWindow(hwnd);

    return hwnd;

    /* ----- This Program Ignores nCmdShow Parameter ----- */

    nCmdShow = nCmdShow;
}
#else
/* ===========================================================================
**  Synopsis    :   static  HWND    InitPepWnd()
*
**  Input       :   HANDLE  hInstance   -   Handle of Current Instance
*                   HANDLE  hPrevInst   -   Handle of Previous Instatnce
*                   int     nCmdShow    -   Show-Window Type (Open/Icon)
*
**  Return      :   Window Handle of PEP Window, if CreateWindow Function
*                   is Successful. Otherwise, it is NULL.
*
**  Description :
*       This function registeres window class, and then creates the PEP Window
*      and  Status Window.
* ===========================================================================
*/
#define WIDE(s) L##s
static  HWND InitPepWnd(HANDLE hInstance, HANDLE hPrevInst, int nCmdShow)
{
	ATOM        aRegRet;
    WNDCLASSW   wc;
    HWND        hwnd;
    WCHAR       szCaption[PEP_CAPTION_LEN + 1],
                szMainClass[PEP_CLASSNAME_LEN + 1],
                szStatusClass[] = WIDE("PEP:STATUS");
	long		status;

	WCHAR		szWork[300],
				szTitle[128];
BOOL unicode;
    /* ----- Load Class Name String of PEP Window ----- */

    LoadString(hResourceDll, IDS_PEP_WINCLASS, szMainClass, PEP_CLASSNAME_LEN);

    if (! hPrevInst) {

        wc.lpszClassName    = szMainClass,
        wc.hInstance        = hInstance;
        wc.lpfnWndProc      = (WNDPROC)PepWndProc;
        wc.hCursor          = LoadPepCursor(NULL, MAKEINTRESOURCEW(32512)/*IDC_ARROW*/);
        wc.hIcon            = LoadPepIcon(hResourceDll, MAKEINTRESOURCEW(IDI_PEPICON1));
        wc.lpszMenuName     = WIDE("#1");
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.style            = CS_HREDRAW | CS_VREDRAW;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;

        aRegRet = RegisterClassW((const WNDCLASSW     *)&wc);
		_ASSERT(aRegRet);

        wc.lpszClassName    = szStatusClass;
        wc.hInstance        = hInstance;
        wc.lpfnWndProc      = (WNDPROC)StatusWndProc;
        wc.hCursor          = LoadPepCursor(NULL, MAKEINTRESOURCEW(32512)/*IDC_ARROW*/);
        wc.hIcon            = NULL;
        wc.lpszMenuName     = NULL;
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.style            = 0;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;

        aRegRet = RegisterClassW((const WNDCLASSW     *)&wc);
		_ASSERT(aRegRet);
    }

    hPepInst = hInstance;

    /* ----- Load Window Title String of PEP Window ----- */

    LoadString(hResourceDll, IDS_PEP_TITLE, szCaption, PEP_CAPTION_LEN);

    hwnd = CreateWindowW(szMainClass,
                        szCaption,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        0,
                        LoadMenuW(hResourceDll,WIDE("#1")),
                        hInstance,
                        NULL);
unicode = IsWindowUnicode(hwnd);
    hPepStat = CreateWindowW(szStatusClass,
                            NULL,
                            WS_CHILD,// | WS_BORDER | WS_VISIBLE,//remove status window
                            0,
                            0,
                            0,
                            0,
                            hwnd,
                            0,
                            hInstance,
                            NULL);

	SelectObject(GetDC(hwnd), hResourceFont);


//#ifdef _DEBUG
#if 1
//When we are running in DEBUG mode the softlocx control will
//not load and we get our Failed to load message
//we are setting status to 1 in order to be able to be able to continue
#pragma message(" ***  WARNING - CheckLicense disabled due to _DEBUG")
	status = 1;
#else
	status = CheckLicense();  //Softlocx Security
#endif
	if(status == 0)
	{
		//Close Becuase License has expired
		LoadString(hResourceDll, IDS_PEP_LICENSE_CAPTION, szTitle, PEP_STRING_LEN_MAC(szTitle));
        LoadString(hResourceDll, IDS_PEP_LICENSE_EXPIRED,  szWork,  PEP_STRING_LEN_MAC(szWork));
		MessageBoxPopUp(hwnd, szWork, szTitle, MB_OK);

        SendMessage(hwnd, WM_CLOSE, 0, 0L);
	}
	if(status == -1)
	{
		//Close because the Softlocx Control did not load properly
		LoadString(hResourceDll, IDS_PEP_CAPTION_CONTROL, szTitle, PEP_STRING_LEN_MAC(szTitle));
        LoadString(hResourceDll, IDS_PEP_CONTROL_FAILED,  szWork,  PEP_STRING_LEN_MAC(szTitle));
		MessageBoxPopUp(hwnd, szWork, szTitle, MB_OK);

        SendMessage(hwnd, WM_CLOSE, 0, 0L);
	}


    ShowWindow(hwnd, SW_SHOW);  //RPH 12/11/02 SR#78

    UpdateWindow(hwnd);

    return hwnd;

    /* ----- This Program Ignores nCmdShow Parameter ----- */

    nCmdShow = nCmdShow;
}
#endif
/*
* ===========================================================================
**  Synopsis    :   static  void    DrawTitle()
*
**  Input       :   HDC     hdcPep  -   Handle of the Device Context
*                   HWND    hWnd    -   Window Handle of PEP Window to Paint
*
**  Return      :   No Return Value.
*
**  Description:
*       This function draws the PEP title with GDI.
* ===========================================================================
*/

static  void    DrawTitle(HDC hdcPep, HWND hWnd)
{

    RECT        rTitle, rClientWnd;
    WCHAR       szTitle[PEP_TITLE_LEN + 1];
	WCHAR		szP2Info[1024];
	WCHAR		szBUSys[40];
	WCHAR		szTaxSys[10];
	WCHAR		szGCSys[40];
	WCHAR		szFormatter[1024];


	FLEXMEM		aFlexMemData[MAX_FLXMEM_SIZE];	/* Buffer for P02 data		*/
	BYTE		abMDCData[MAX_MDC_SIZE];		/* Buffer of MDC Data       */
	USHORT      usReturnLen, usRet;
	BYTE		bMDCWork, bWork, bWork2;
	USHORT      usBuffAddr;
	int         nAddress, nNumTerm;
	HFONT		hDescFont, oldFont, tmpFont;
	LOGFONT		logfont;
	int			iLiSel = 0;


    /* -- Default font is the current font but below we will want to change point size ----- */
	SelectObject(hdcPep, hResourceFont);

    /* ----- Set the Coodinates of the BackGround and ForeGround Rects ----- */
	GetClientRect(hWnd, (RECT     *)&rClientWnd);

	rTitle.left   = rClientWnd.left + 25;
    rTitle.top    = rClientWnd.top + 25;
    rTitle.right  = 425;
    rTitle.bottom = rClientWnd.bottom;

	/* ----- Set the Current Text Color to BLACK ----- */
    SetTextColor(hdcPep, PEP_TEXT_COLOR);

    /* ----- Set the Text-Alignment Flags for the Device Context ----- */
    SetTextAlign(hdcPep, TA_CENTER | TA_TOP);


    /* ----- Load String of the PEP Title ----- */
   LoadString(hResourceDll, IDS_PEP_TITLE, szTitle, PEP_STRING_LEN_MAC(szTitle));


	/*-----------------------------------------------------------------------*/
	/*-------create the font we will use for our file description.-----------*/
	/*---- we want to use a 14 point font size with the ANSI character set---*/
	/*---- Title will use 14 point, underline, will change for description---*/
	memset (&logfont, 0, sizeof (LOGFONT));

	logfont.lfHeight = -MulDiv(14, GetDeviceCaps(hdcPep, LOGPIXELSY), 72);
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	logfont.lfCharSet = ANSI_CHARSET ;
	logfont.lfUnderline = TRUE;

	for (iLiSel = 0; myCodePageTable[iLiSel].tcCountry; iLiSel++) {
		if ((UCHAR)xLangId == myCodePageTable[iLiSel].wLanguage)
			break;
	}

	if (myCodePageTable[iLiSel].tcCountry) {
		logfont.lfHeight = -MulDiv(14, GetDeviceCaps(GetDC (NULL), LOGPIXELSY), 72);
		logfont.lfCharSet = myCodePageTable[iLiSel].uiCharSet ;
		logfont.lfQuality = myCodePageTable[iLiSel].ucQuality;
		logfont.lfPitchAndFamily = myCodePageTable[iLiSel].ucPitch;
	}

	hDescFont = CreateFontIndirect (&logfont);
	if (hDescFont)
		oldFont = SelectObject (hdcPep, hDescFont);


    /* ----- Write the String within the Rectangular Region ----- */
	ExtTextOutW(hdcPep,
			   ((rTitle.left + rTitle.right)  / PEP_TITLE_CALC_STD),
			   rTitle.top,//((rTitle.top  + rTitle.bottom) / PEP_TITLE_CALC_STD),
			   ETO_OPAQUE,
			   (const RECT     *)&rTitle,
			   szTitle,//szMyBuffer,
			   wcslen(szTitle),
			   NULL);


///////////////gather file description data//////////////////////
/////////////////////////////////////////////////////////////////

	memset(aFlexMemData,0,sizeof(aFlexMemData));
	memset(szP2Info,0,sizeof(szP2Info));

	//get P2 data
    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)aFlexMemData, sizeof(aFlexMemData), 0, (USHORT *)&usReturnLen);

	//get MDC data
	ParaAllRead(CLASS_PARAMDC, (UCHAR *)abMDCData, sizeof(abMDCData), 0, &usRet);

	/*--------------------------Get System Type --------------------------*/
	nAddress = 368;
	usBuffAddr = ((nAddress - 1) / PEP_CHK_ADDR);

	bMDCWork = PEP_GET_HIBIT(abMDCData[usBuffAddr]);		//odd address uses hi-bit

	// ----- Check  MDC Addr 368 Bit D whether ON/OFF
	//       Then display whether we have a Backup Master or not
    bWork = (BYTE)((bMDCWork >> 0) & PEP_CHK_BIT);

	if(bWork) {
		LoadString(hResourceDll, IDS_PEP_NOBU_SYS,  szBUSys, PEP_STRING_LEN_MAC(szBUSys));
	}
	else {
		LoadString(hResourceDll, IDS_PEP_BU_SYS,  szBUSys, PEP_STRING_LEN_MAC(szBUSys));
	}

	/*-----------------------Get Number of Terminals-----------------------*/
	nAddress = 365;
	usBuffAddr = ((nAddress - 1) / PEP_CHK_ADDR);

	bMDCWork = PEP_GET_LOBIT(abMDCData[usBuffAddr]);		//odd address uses lo-bit

	// ----- Get # of Terminals via MDC Addr 365 bits ----- //
	bWork = (BYTE)(bMDCWork & PEP_CHK_4BITS);
	nNumTerm = bWork + 1;

	// ------       Get Tax System        ------------
	//    Check  MDC Addr 15 Bit D whether ON/OFF -----
	//    Determine what tax system has been selected.

	nAddress = 15;
	usBuffAddr = ((nAddress - 1) / PEP_CHK_ADDR);

	bMDCWork = PEP_GET_LOBIT(abMDCData[usBuffAddr]);		//odd address uses lo-bit

	bWork = (BYTE)((bMDCWork >> 0) & PEP_CHK_BIT);

	if( bWork ) {		//Canadian / VAT
		nAddress = 11;
		usBuffAddr = ((nAddress - 1) / PEP_CHK_ADDR);

		bMDCWork = PEP_GET_LOBIT(abMDCData[usBuffAddr]);		//odd address uses lo-bit

		// ----- Check  MDC Addr 11 Bit D whether ON/OFF ----- //
		bWork2 = (BYTE)((bMDCWork >> 0) & PEP_CHK_BIT);

		if( bWork2 ){
			LoadString(hResourceDll, IDS_PEP_TXSYS_VAT,  szTaxSys, PEP_STRING_LEN_MAC(szTaxSys));
		}
		else {
			LoadString(hResourceDll, IDS_PEP_TXSYS_CAN,  szTaxSys, PEP_STRING_LEN_MAC(szTaxSys));
		}
	}
	else {		//US Tax System
		LoadString(hResourceDll, IDS_PEP_TXSYS_US,  szTaxSys, PEP_STRING_LEN_MAC(szTaxSys));
	}

	//-----Format string for Guest Check system type-----//
	switch(aFlexMemData[8].uchPTDFlag) {
	case	0:
		LoadString(hResourceDll, 3599,  szGCSys, PEP_STRING_LEN_MAC(szGCSys));
		break;
	case	1:
		LoadString(hResourceDll, 3600,  szGCSys, PEP_STRING_LEN_MAC(szGCSys));
		break;
	case	2:
		LoadString(hResourceDll, 3601,  szGCSys, PEP_STRING_LEN_MAC(szGCSys));
		break;
	case	3:
		LoadString(hResourceDll, 3602,  szGCSys, PEP_STRING_LEN_MAC(szGCSys));
		break;
	default:
		wcscpy(szGCSys, WIDE(""));
	}

	/*-----------------------Format Output String---------------------------*/

    /* ----- Load String of the PEP Title ----- */

    LoadString(hResourceDll, IDS_PEP_FILEINFO_FRMT, szFormatter, PEP_STRING_LEN_MAC(szFormatter));

	wsPepf(szP2Info, szFormatter,
				  szBUSys,
				  nNumTerm,
			   	  szGCSys,
				  aFlexMemData[8].ulRecordNumber,
				  aFlexMemData[7].ulRecordNumber,
				  aFlexMemData[6].ulRecordNumber,
				  aFlexMemData[0].ulRecordNumber,
				  aFlexMemData[0].uchPTDFlag,
				  aFlexMemData[1].ulRecordNumber,
				  aFlexMemData[1].uchPTDFlag,
				  aFlexMemData[3].ulRecordNumber,
				  aFlexMemData[3].uchPTDFlag,
				  aFlexMemData[9].ulRecordNumber,
				  aFlexMemData[9].uchPTDFlag,
				  aFlexMemData[4].ulRecordNumber,
                  aFlexMemData[4].uchPTDFlag,
				  aFlexMemData[5].ulRecordNumber,
				  aFlexMemData[10].ulRecordNumber,
				  aFlexMemData[12].ulRecordNumber,
				  aFlexMemData[11].ulRecordNumber,
				  szTaxSys);

	// Now that we have written out the title, we will
	// change font to 12 point, not underline and write out some
	// of the basic statistics of this PEP file.

	// First thing is to get rid of the font we were previously using.
	// So select back into the DC the old font, if we were using a
	// different font, and delete the different font.
	// Next we create our new font using a 12 pt size and select it into the DC.

	if (hDescFont) {
		tmpFont = SelectObject (hdcPep, oldFont);
		DeleteObject (tmpFont);
	}

	logfont.lfHeight = -MulDiv(12, GetDeviceCaps(hdcPep, LOGPIXELSY), 72);
	logfont.lfUnderline = FALSE;
	hDescFont = CreateFontIndirect (&logfont);

	if (hDescFont)
		oldFont = SelectObject (hdcPep, hDescFont);


    /* -------- Set the Text-Alignment Flags for the Device Context --------- */

    SetTextAlign(hdcPep, TA_TOP | TA_LEFT);

	/*-------Adjust Foreground RECT top to adjust for title-------*/
    rTitle.top    += 50;


    /* ---------------- Write the String within the Rectangular Region ------- */
	DrawTextW(hdcPep, szP2Info, wcslen(szP2Info), &rTitle, DT_LEFT | DT_EXPANDTABS |DT_NOCLIP);

	// Finally, we clean up the created font so as to be a good Windows app.
	if (hDescFont) {
		tmpFont = SelectObject (hdcPep, oldFont);
		DeleteObject (tmpFont);
	}
}

/*
* ===========================================================================
**  Synopsis    :   static  void    ExecuteProg()
*
**  Input       :   HWND    hWnd    -   Window Handle of PEP Window
*                   WPARAM  wParam  -   Menu ID of Selected
*
**  Return      :   No Return Value.
*
**  Description:
*       This function executes Program Mode function.
* ===========================================================================
*/
static  void    ExecuteProg(HWND hWnd, WORD wEditID)
{
    FCONFIG const       *lpFile;    /* Address of File Config Structure */
    WORD                wID;        /* DialogBox ID of Current Keyboard */

    switch (wEditID) {

    case    IDM_P01:        /* Machine Definition Code (MDC) */
        DoProgram01(hWnd, (LPHANDLE)&hwndPepDlgChk1, (LPHANDLE)&hwndPepDlgChk2);
        break;

    case    IDM_P02:        /* Flexible Memory Assignment */
        DoProgram(hWnd, P002DlgProc, IDD_P02, hPepInst);
        break;

    case    IDM_P03:        /* Function Selction Code (Key Assignment) */
        /* ----- Get Address of the File Configulation Structure ----- */
        lpFile = FileConfigGet();

        /* ----- Determine DialogBox ID of Current Selected Keyboard ----- */
        if (lpFile->chKBType == FILE_KB_CONVENTION) {         /* NCR 7448 with Conventional Keyboard  */
            wID = IDD_P03_CONV;
        } else if (lpFile->chKBType == FILE_KB_MICRO) {  /* NCR 7448 with Micromotion Keyboard   */
            wID = IDD_P03_MICRO;
        } else if (lpFile->chKBType == FILE_KB_WEDGE_68) {    /* NCR Wedge 68 key keyboard  */
            wID = IDD_P03_WEDGE_68;
        } else if (lpFile->chKBType == FILE_KB_WEDGE_78) {    /* NCR Wedge 78 key keyboard  */
            wID = IDD_P03_WEDGE_78;
        }else if (lpFile->chKBType == FILE_KB_BIG_TICKET) { /* NCR Big Ticket Keyboard */
			wID = IDD_P03_TICKET ;
		}else if (lpFile->chKBType == FILE_KB_TOUCH){

			LayoutExec(hPepInst, hWnd, szCommTouchLayoutSaveName, xLangId); //ESMITH LAYOUT
			break;
		}   else {
			//This will be called if the terminal is set to PC 102 in the configuration dialog
			break;  // breakpoint out.
		}

        DoProgram03(hWnd, (LPHANDLE)&hwndPepDlgChk1, (LPHANDLE)&hwndPepDlgChk2, wID);
        break;

    case    IDM_P06:        /* Program Mode Security Code */
        DoProgram(hWnd, P006DlgProc, IDD_P06, hPepInst);
        break;

    case    IDM_P08:        /* Supervisor Number/Level */
        DoProgram(hWnd, P008DlgProc, IDD_P08, hPepInst);
        break;

    case    IDM_P09:        /* Action Code Security */
        DoProgram(hWnd, P009DlgProc, IDD_P09, hPepInst);
        break;

    case    IDM_P10:        /* High Amount Lock-out Limit */
        DoProgram(hWnd, P010DlgProc, IDD_P10, hPepInst);
        break;

    case    IDM_P15:        /* Preset Amount for Preset Cash Key */
        DoProgram(hWnd, P015DlgProc, IDD_P15, hPepInst);
        break;

    case    IDM_P17:        /* Hourly Activity Block */
        DoProgram(hWnd, P017DlgProc, IDD_P17, hPepInst);
        break;

    case    IDM_P18:        /* Slip Print Control */
        DoProgram(hWnd, P018DlgProc, IDD_P18, hPepInst);
        break;

    case    IDD_P20:        /* Transaction Mnemonics */
        DoProgram(hWnd, P020DlgProc, IDD_P20, hPepInst);
        break;

    case    IDD_P223_RPT:        /* Financial Report Setup */
        DoProgram(hWnd, P223DlgProc, IDD_P223_RPT, hPepInst);
        break;

    case    IDD_P223_1_RPT:        /* Cashier Report Setup */
        DoProgram(hWnd, P223DlgProc_1, IDD_P223_RPT, hPepInst);
        break;

    case    IDD_P21:        /* Lead Through Messages */
        DoProgram(hWnd, P021DlgProc, IDD_P21, hPepInst);
        break;

    case    IDM_P22:        /* Report Mnemonics */
        DoProgram(hWnd, P022DlgProc, IDD_P22, hPepInst);
        break;

    case    IDM_P23:        /* Special Mnemonics */
        DoProgram(hWnd, P023DlgProc, IDD_P23, hPepInst);
        break;

    case    IDM_P46:        /* Adjective Mnemonics */
        DoProgram(hWnd, P046DlgProc, IDD_P46, hPepInst);
        break;

    case    IDM_P47:        /* Print Modifier Mnemonics */
        DoProgram(hWnd, P047DlgProc, IDD_P47, hPepInst);
        break;

    case    IDM_P48:        /* Major Department Mnemonics */
        DoProgram(hWnd, P048DlgProc, IDD_P48, hPepInst);
        break;

    case    IDM_P49:        /* Automatic Alternative Remote Pronter */
        DoProgram(hWnd, P049DlgProc, IDD_P49, hPepInst);
        break;

    case    IDM_P50:        /* Assignment Terminal # Installing Shared Printer */
        DoProgram(hWnd, P050DlgProc, IDD_P50, hPepInst);
        break;

    case    IDM_P51:        /* Assignment IP Address for KDS   NCR2172  */
        DoProgram(hWnd, P051DlgProc, IDD_P51, hPepInst);
        break;

    case    IDM_P54:        /* Set Hotel Id and SLD */
        DoProgram(hWnd, P054DlgProc, IDD_P54, hPepInst);
        break;

    case    IDM_P57:        /* Header/Special Messages */
        DoProgram(hWnd, P057DlgProc, IDD_P57, hPepInst);
        break;

    case    IDM_P60:        /* Total Key Type / Function */
        DoProgram(hWnd, P060DlgProc, IDD_P60, hPepInst);
        break;

    case    IDM_P62:        /* Tender Key Parameter , V3.3 */
        DoProgram(hWnd, P062DlgProc, IDD_P62, hPepInst);
        break;

    case    IDM_P65:        /* Multi-Line Display Mnemonics , V3.3 */
        DoProgram(hWnd, P065DlgProc, IDD_P65, hPepInst);
        break;

	case	IDM_P67:		/* Auto Combinational Coupons	, */
	    DoProgram(hWnd, P067DlgProc, IDD_P67, hPepInst);
		break;

	case	IDM_P72:		/* Reason Code mnemonics	, */
	    DoProgram(hWnd, P072DlgProc, IDD_P72, hPepInst);
		break;

    case    IDM_P97:        /* Multi-Line Display Mnemonics , V3.3 */
        DoProgram(hWnd, P097DlgProc, IDD_P97, hPepInst);
        break;

    case    IDM_P98:        /* Multi-Line Display Mnemonics , V3.3 */
        DoProgram(hWnd, P098DlgProc, IDD_P98, hPepInst);
        break;
    }

    return;
}

/*
* ===========================================================================
**  Synopsis    :   static  void    ExecuteAct()
*
**  Input       :   HWND    hWnd    -   Window Handle of PEP Window
*                   WPARAM  wParam  -   Menu ID of Selected
*
**  Return      :   No Return Value.
*
**  Description:
*       This function executes Action Code function.
* ===========================================================================
*/
static  void    ExecuteAct(HWND hWnd, WORD wEditID)
{
    FCONFIG const       *lpFile;    /* Address of File Config Structure */
    WORD                wID;        /* DialogBox ID of Current Keyboard */

    switch (wEditID) {

    case    IDM_A04:        /* Set Department Key           */
        /* ----- Get Address of the File Configulation Structure ----- */
        lpFile = FileConfigGet();

        /* ----- Determine DialogBox ID of Current Selected Keyboard ----- */
        if (lpFile->chKBType == FILE_KB_CONVENTION) {       /* NCR 7448 with Conventional Keyboard  */
            wID = IDD_A04_CONV;
        } else if (lpFile->chKBType == FILE_KB_MICRO) {     /* NCR 7448 with Micromotion Keyboard   */
            wID = IDD_A04_MICR;
        } else if (lpFile->chKBType == FILE_KB_WEDGE_68) {  /* NCR Wedge 64 key keyboard  */
            wID = IDD_A04_WEDGE_68;
        } else if (lpFile->chKBType == FILE_KB_WEDGE_78) {  /* NCR Wedge 78 key keyboard  */
            wID = IDD_A04_WEDGE_78;
        } else {
			break;
			// __debugbreak();  // prep for ARM build _asm int 3;
		}   // breakpoint out.

        DoAction04(hWnd, (LPHANDLE)&hwndPepDlgChk1, (LPHANDLE)&hwndPepDlgChk2, wID);
        break;

    case    IDM_A05:        /* Set Page Control             */
        DoAction(hWnd, A005DlgProc, IDD_A05, hPepInst);
        break;

    case    IDM_A07:        /* Cashier A/B Key Assignment */
        DoAction(hWnd, A007DlgProc, IDD_A07, hPepInst);
        break;

    case    IDM_A20:        /* Cashier Maintenance          */
        DoAction(hWnd, A020DlgProc, IDD_A20, hPepInst);
        break;

    case    IDM_A33:        /* Set Control String Table     */
        DoAction(hWnd, A033DlgProc, IDD_A33, hPepInst);
        break;

#if defined(POSSIBLE_DEAD_CODE)
	case    IDM_A50:        /* Server Maintenance           */
        DoAction(hWnd, A050DlgProc, IDD_A50, hPepInst);
        break;
#endif

    case    IDM_A68:        /* PLU Maintenance              */
        DoAction(hWnd, A068MaintDlgProc, IDD_A68, hPepInst);
        break;

    case    IDM_A71:        /* Set PPI Table                */
        DoAction(hWnd, A071DlgProc, IDD_A71, hPepInst);
        break;

    case    IDM_A84:        /* Set Rounding Table           */
        DoAction(hWnd, A084DlgProc, IDD_A84, hPepInst);
        break;

    case    IDM_A86:        /* Set Percent Rates            */
        DoAction(hWnd, A086DlgProc, IDD_A86, hPepInst);
        break;

    case    IDM_A87:        /* Set Slip Promotion Header */
        DoAction(hWnd, A087DlgProc, IDD_A87, hPepInst);
        break;

    case    IDM_A88:        /* Set Receipt Promotion Header */
        DoAction(hWnd, A088DlgProc, IDD_A88, hPepInst);
        break;

    case    IDM_A89:        /* Set Currency Conversion Rate */
        DoAction(hWnd, A089DlgProc, IDD_A89, hPepInst);
        break;

    case    IDM_A111:       /* Set Tare Table               */
        DoAction(hWnd, A111DlgProc, IDD_A111, hPepInst);
        break;

    case    IDM_A114:       /* Department(File) Maintenance */
        DoAction(hWnd, A114DlgProc, IDD_A114, hPepInst);
        break;
/* DEL Saratoga */
/*    case    IDM_A115: */      /* Set Department No. on Keyed Department Key*/
/*        DoAction(hWnd, A115DlgProc, IDD_A115, hPepInst); */
/*        break;                                                    */
    case    IDM_A116:       /* Set Promotional PLU          */
        DoAction(hWnd, A116DlgProc, IDD_A116, hPepInst);
        break;

    case    IDM_A124:       /* Set Tax Table                */
        DoAction(hWnd, A124DlgProc, IDD_A124, hPepInst);
        break;

    case    IDM_A127:       /* Set Tax Rate                 */
        DoAction(hWnd, A127DlgProc, IDD_A127, hPepInst);
        break;

    case    IDM_A130:       /* Set Pig Rule Table           */
        DoAction(hWnd, A130DlgProc, IDD_A130, hPepInst);
        break;

    case    IDM_A133:       /* Service Time Parameter Maintenance */
        DoAction(hWnd, A133DlgProc, IDD_A133, hPepInst);
        break;

    case    IDM_A137:       /* Set Beverage Dispenser Parameter */
        DoAction(hWnd, A137DlgProc, IDD_A137, hPepInst);
        break;

    case    IDM_A152:       /* Employee Number Assignment   */
        DoAction(hWnd, A152DlgProc, IDD_A152, hPepInst);
        break;

    case    IDM_A154:       /* Set Wage Rate for Job Code   */
        DoAction(hWnd, A154DlgProc, IDD_A154, hPepInst);
        break;

	case    IDD_A156:       /* Set suggested tip amount   */
		DoAction(hWnd, A154DlgProc, IDD_A156, hPepInst);
		break;

    case    IDM_A160:       /* Order Entry Prompt Table Maintenance */
        DoAction(hWnd, A160DlgProc, IDD_A160, hPepInst);
        break;

    case    IDM_A161:       /* Combination Coupon Talbe Maintenance */
        DoAction(hWnd, A161DlgProc, IDD_A161, hPepInst);
        break;

    case    IDM_A162:       /* Flexible Drivethru Parameter Maintenance */
        DoAction(hWnd, A162DlgProc, IDD_A162, hPepInst);
        break;

    case    IDM_A170:       /* Set Sales Restriction Table of PLU */
        DoAction(hWnd, A170DlgProc, IDD_A170, hPepInst);
        break;

    case    IDM_A208:       /* Set Boumdary Age    NCR2172          */
        DoAction(hWnd, A208DlgProc, IDD_A208, hPepInst);
        break;

	case	IDM_A209:		/* Set Color Palette		*/
		DoAction(hWnd, A209DlgProc, IDD_A209, hPepInst);
		break;

    case    IDM_A128:       /* ### ADD Saratoga (051900) */
        DoAction(hWnd, A128DlgProc, IDD_A128, hPepInst);
        break;
    }
    return;
}


//-----------------------   Multi-lingual code

UINT uiCurrentCodePage = 0;

VOID GetLangFont(int iLiSel)
{
	LOGFONT logfont = {0};

	logfont.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC (NULL), LOGPIXELSY), 72);
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;


	logfont.lfHeight = -MulDiv(myCodePageTable[iLiSel].unFontSize, GetDeviceCaps(GetDC (NULL), LOGPIXELSY), 72);
	logfont.lfCharSet = myCodePageTable[iLiSel].uiCharSet ;
	logfont.lfQuality = myCodePageTable[iLiSel].ucQuality;
	logfont.lfPitchAndFamily = myCodePageTable[iLiSel].ucPitch;
	hResourceFont = CreateFontIndirect (&logfont);

	_ASSERT (hResourceFont);

	if (!hResourceFont) {
		hResourceFont = GetStockObject(SYSTEM_FONT);
	}
}

int HandleKeyBoardChange (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int nBuff = 30;
	int iLiSel = 0;
	WCHAR  AvailableLangName[100];
	HMENU  hMenu;
	CPINFO myCPInfo;
	UINT j = 0;
    WCHAR        szTitle[PEP_TITLE_LEN];

	for (iLiSel = 0; myCodePageTable[iLiSel].tcCountry; iLiSel++) {
		LANGID xlan = (LANGID)(LOWORD(lParam) & 0x00ff);
		if ((UCHAR)xlan == myCodePageTable[iLiSel].wLanguage)
			break;
	}
	if (! myCodePageTable[iLiSel].tcCountry) {
		MessageBoxPopUp(hwnd, WIDE("Selection not in suported list."), WIDE("Language Select"), MB_OK);
		return 0;
	}

	if (! GetCPInfo(myCodePageTable[iLiSel].unCodePage, &myCPInfo)) {
		MessageBoxPopUp(hwnd, WIDE("Code Page not found.  No changes made."), WIDE("Code Page Not Found."), MB_OK);
		return FALSE;
	}
	_wsetlocale (LC_ALL, myCodePageTable[iLiSel].tcCountry);
	uiCurrentCodePage = myCodePageTable[iLiSel].unCodePage;
	setCodePage (uiCurrentCodePage);
	wcscpy(AvailableLangName, WIDE(".\\Pep_Res\\"));
	wcscat(AvailableLangName, myCodePageTable[iLiSel].tcCountry);
	wcscat(AvailableLangName, WIDE(".dll"));
	if (wcscmp (WIDE("english"), myCodePageTable[iLiSel].tcCountry) != 0) {
		hResourceDll = LoadLibraryW(AvailableLangName);
		if (!hResourceDll) {
			hResourceDll = hPepInst;
			MessageBoxPopUp(hwnd, WIDE("Language DLL extension not found. Using default."), WIDE("Language Select"), MB_OK);
		}
	}
	else {
		hResourceDll = hPepInst;
	}
//				hAccelTable = LoadAccelerators(hResourceInst, _T("#1"));
	// Redraw the client area of the main window
	InvalidateRect(hwnd,NULL,TRUE);
	UpdateWindow(hwnd);
	// Update the caption of the main window
//				LoadString(hResourceDll, IDS_PEP_TITLE, szTitle, sizeof(szTitle));
//				WindowSendTextMessage(hwnd, WM_SETTEXT, NULL, (LPARAM)szTitle);
	// Load the localized menu
	DestroyMenu(GetMenu(hwnd));
	hMenu = LoadPepMenu(hResourceDll, WIDE("#1"));
	SetMenu(hwnd, hMenu);
	SendMessage (hwnd, PM_PEPMENU, 0, 0);
//				DrawMenuBar(hMainWindow);
	GetLangFont(iLiSel);

	memset(szTitle, 0x00, sizeof(szTitle));
	LoadString(hPepInst/*hResourceDll*/, IDS_PEP_TITLE, szTitle, PEP_STRING_LEN_MAC(szTitle));
	WindowRedrawText(hwnd, szTitle);


	return 1;
}

// Mesage handler for language box.
LRESULT LanguageChange (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int nBuff = 30;
	HKL lpList[30];
	HWND hItemWnd = NULL, hMainWindow = (HWND) GetWindowLong (hDlg, GWL_HWNDPARENT);
	PCodePageTable pp;
	int iLiSel = 0;
	WCHAR  AvailableLangName[256];
	HMENU  hMenu;
    WCHAR        szTitle[PEP_TITLE_LEN];

	switch (message)
	{
		case WM_INITDIALOG:
	//LangInitDlgStrings(hDlg);
 		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

		hItemWnd = GetDlgItem (hDlg, IDD_CHANGE_LANGUAGE_LIST);
			_ASSERT(hItemWnd);
			for (pp = myCodePageTable; pp->tcCountry; pp++) {
				WindowSendTextMessage(hItemWnd, LB_ADDSTRING, 0, (LONG) pp->tcCountry);
			}
			return TRUE;

		case WM_SETFONT:

		if (hResourceFont) {
			SendDlgItemMessage(hDlg, IDD_LANG_CHOOSE, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDD_CHANGE_LANGUAGE_LIST, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			else if (LOWORD(wParam) == IDOK)
			{
				CPINFO myCPInfo;
				UINT j = 0;
				UINT myN = GetKeyboardLayoutList(nBuff, lpList);
				hItemWnd = GetDlgItem (hDlg, IDD_CHANGE_LANGUAGE_LIST);
				_ASSERT(hItemWnd);

				iLiSel = SendMessage (hItemWnd, LB_GETCURSEL, 0, 0);
				if (iLiSel == LB_ERR) {
					EndDialog(hDlg, IDCANCEL);
					return TRUE;
				}

				if (! GetCPInfo(myCodePageTable[iLiSel].unCodePage, &myCPInfo)) {
					MessageBoxPopUp (hDlg, WIDE("Code Page not found.  No changes made."), WIDE("Code Page Not Found."), MB_OK);
					return FALSE;
				}

				for (j = 0; j < myN; j++) {
					LANGID xlan = (LANGID)lpList[j];
					if ((UCHAR)xlan == myCodePageTable[iLiSel].wLanguage)
						break;
				}
				if (j >= myN) {
					MessageBoxPopUp(hDlg, WIDE("Selection not in keyboard list."), WIDE("Language Select"), MB_OK);
				}

				/*
					Set the locale and code page information for the selected
					language.  The default language is English which is in the
					standard PEP.RC file which is compiled into PEP.EXE where as
					the other languages are in separate DLLs.
				 */
				wcscpy(AvailableLangName, WIDE(".\\Pep_Res\\"));
				wcscat(AvailableLangName, myCodePageTable[iLiSel].tcCountry);
				wcscat(AvailableLangName, WIDE(".dll"));
				if (wcscmp(WIDE("english"), myCodePageTable[iLiSel].tcCountry) != 0) {
					hResourceDll = LoadPepLibrary(AvailableLangName);
				}
				else {
					hResourceDll = hPepInst;
				}

				if (!hResourceDll) {
					hResourceDll = hPepInst;
					iLiSel = ILISEL_ENGLISH;
					MessageBoxPopUp(hDlg, WIDE("Language DLL extension not found. Using default."), WIDE("Language Select"), MB_OK);
				}

				// Set the global language identifier from the table value.
				// Then set up the locale and code page information.
				xLangId = MAKELANGID((USHORT)myCodePageTable[iLiSel].wLanguage,
								  (USHORT)myCodePageTable[iLiSel].wSubLanguage);
				_wsetlocale (LC_ALL, myCodePageTable[iLiSel].tcCountry);
				uiCurrentCodePage = myCodePageTable[iLiSel].unCodePage;
				setCodePage (uiCurrentCodePage);

//				hAccelTable = LoadAccelerators(hResourceInst, _T("#1"));
				// Redraw the client area of the main window
				InvalidateRect(hMainWindow,NULL,TRUE);
				UpdateWindow(hMainWindow);
				// Update the caption of the main window
//				LoadString(hResourceDll, IDS_PEP_TITLE, szTitle, sizeof(szTitle));
//				WindowSendTextMessage(hMainWindow, WM_SETTEXT, NULL, (LPARAM)szTitle);
				// Load the localized menu
				DestroyMenu(GetMenu(hMainWindow));
				hMenu = LoadPepMenu (hResourceDll, WIDE("#1"));
				SetMenu(hMainWindow, hMenu);
				SendMessage (hMainWindow, PM_PEPMENU, 0, 0);
//				DrawMenuBar(hMainWindow);

				GetLangFont(iLiSel);

				memset(szTitle, 0x00, sizeof(szTitle));
				LoadString(hPepInst/*hResourceDll*/, IDS_PEP_TITLE, szTitle, PEP_STRING_LEN_MAC(szTitle));
				WindowRedrawText(hMainWindow, szTitle);

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


int PepChangeLanguage (HWND hWnd)
{

	return DialogBoxPopup(hResourceDll, (LPCWSTR)IDD_CHANGE_LANGUAGE, hWnd, (DLGPROC)LanguageChange);
}
/*
* ===========================================================================
**  Synopsis:   VOID    ProgInitialize()
*
**  Input   :   HANDLE  hPep    - instance handle of PEP.EXE
*
**  Return  :   None
*
**  Description:
*       This procedure initialize PEPPROG.DLL.
* ===========================================================================
*/
VOID    ProgInitialize(HANDLE hPep)
{
    /* ----- Store instance handle ----- */

    hProgPepInst  = hPep;
}

/* ===== End of File (PEP.C) ===== */
