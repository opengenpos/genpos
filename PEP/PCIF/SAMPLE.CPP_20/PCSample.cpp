/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//  ---------------------------------------------------------------------------
//     Georgia Southern University Research Services Foundation
//     donated by NCR to the research foundation in 2002 and maintained here since.
//        2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
//        2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
//        2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
//        2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
//        2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
//        2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
//        2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
//        2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
// 
//     moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
//     moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
//  ---------------------------------------------------------------------------
//
//  FILE NAME:  PCSample.cpp
//
//  PURPOSE:    Provides PCSample Application & About Dialog box classes.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Shlobj.h>

#include "stdio.h"
#include "stdarg.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"

#include "PCSample.h"
#include "PCSampleDoc.h"
#include "MainFrm.h"
#include "PCSampleView.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//          G L O B A L    V A R I A B L E s
//
/////////////////////////////////////////////////////////////////////////////

CPCSampleApp theApp;
LPCWSTR CPCSampleApp::m_lpszLogFileName = _T("pcsample.log");

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPCSampleApp, CWinApp)
    //{{AFX_MSG_MAP(CPCSampleApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_APP_EXIT, OnAppExit)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::CPCSampleApp()
//
//  PURPOSE :   Constructor of PCSample Application class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleApp::CPCSampleApp()
{
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::~CPCSampleApp()
//
//  PURPOSE :   Destructor of PCSample Application class.
//
//  RETURN :    No return value.
//
//===========================================================================

CPCSampleApp::~CPCSampleApp()
{
#ifdef _DEBUG
    // --- close log file before this object is discarded ---

    m_fileLog.Close();
#endif
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::InitInstance()
//
//  PURPOSE :   Initialize PCSample application per instance.
//
//  RETURN :    TRUE    - Initialization is successful.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CPCSampleApp::InitInstance()
{
	// --- Standard initialization ---

	// --- Change the registry key under which our settings are stored ---

    SetRegistryKey( _T("NCR" ));

    // --- Load standard INI file options (including MRU) ---

    LoadStdProfileSettings();

	// --- Register document templates ---

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate( IDR_MAINFRAME,
                                           RUNTIME_CLASS( CPCSampleDoc ),
                                           RUNTIME_CLASS( CMainFrame ),
                                           RUNTIME_CLASS( CPCSampleView ));
    AddDocTemplate( pDocTemplate );

	// --- Parse command line for standard shell commands ---

    CCommandLineInfo cmdInfo;
    ParseCommandLine( cmdInfo );

	// determine the user's documents folder, create a subfolder for PCSample there
	// if there is not already one, and then set the current directory to that
	// folder.  it will be something like "C:\users\xxx\documents\PCSample"
	WCHAR tempDirPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath (NULL, CSIDL_PERSONAL, NULL, 0, tempDirPath))) {
		wcscat (tempDirPath, L"\\PCSample");
		CreateDirectory (tempDirPath, NULL);
		SetCurrentDirectory (tempDirPath);
	}

	// --- Dispatch commands specified on the command line ---

    BOOL    fSuccess;

    fSuccess = ProcessShellCommand( cmdInfo );

    if ( fSuccess )
    {
        // --- show main frame window on screen ---

        m_pMainWnd->ShowWindow( SW_SHOW );
        m_pMainWnd->UpdateWindow();


#ifdef _DEBUG
        // --- create text file to log messages ---

        UINT    unFlags;
        unFlags = CFile::modeReadWrite | CFile::modeCreate | CFile::typeText;

        m_fileLog.Open( m_lpszLogFileName, unFlags );
#endif
    }

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::OnAppAbout()
//
//  PURPOSE :   Show the about dialog box on screen.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::OnAppExit()
//
//  PURPOSE :   Send "Close" message to main frame window to confirm exit.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleApp::OnAppExit() 
{
	m_pMainWnd->SendMessage( WM_CLOSE );
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::TraceFunction()
//
//  PURPOSE :   Trace function calling information to log file.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleApp::TraceFunction( LPCTSTR lpszFormat, va_list argList )
{
    // --- get current system date & time and store to string ---

    SYSTEMTIME  thisTime;

    ::GetLocalTime( &thisTime );

    m_strLog.Format( _T("%02d/%02d/%04d - %02d:%02d:%02d.%03d > "),
                     thisTime.wMonth,
                     thisTime.wDay,
                     thisTime.wYear,
                     thisTime.wHour,
                     thisTime.wMinute,
                     thisTime.wSecond,
                     thisTime.wMilliseconds );

    // --- format string by user defined format list ---

    CMyString   str;

    str.FormatV( lpszFormat, argList );

    m_strLog += str;

    // --- serialize formatted string to log file ---

    CArchive    ar( &m_fileLog, CArchive::store );
    Serialize( ar );

}

// A-KSo
VOID CPCSampleApp::DisplayText(LPCTSTR lpszFormat, va_list arg_list)
{
	static CPCSampleDoc *pDoc = NULL;
	if (pDoc == NULL)
	{
		CFrameWnd *frameWnd = (CFrameWnd *)AfxGetMainWnd();
		if (frameWnd != NULL)
		{
			pDoc = (CPCSampleDoc *)(frameWnd->GetActiveDocument());
		}
	}
	if (pDoc != NULL)
	{
		pDoc->DisplayText(lpszFormat, arg_list);
	}
}

//===========================================================================
//
//  FUNCTION :  CPCSampleApp::Serialize()
//
//  PURPOSE :   Write the formatted string to log file.
//
//  RETURN :    No return value.
//
//===========================================================================

void CPCSampleApp::Serialize(
     CArchive& ar               // archive object to write string
     )
{
	
    // --- determine this function call is write to file ---
    if ( ar.IsStoring())
    {
 // --- write specified formatted string to log files ---
#ifdef _DEBUG

        ar.WriteString( m_strLog );
        ar.WriteString( CString( "\n" ));
		
#endif	
	}

		
}

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CAboutDlg::CAboutDlg()
//
//  PURPOSE :   Constructor of About dialog box class.
//
//  RETURN :    No return value.
//
//===========================================================================

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CAboutDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CAboutDlg::DoDataExchange(
    CDataExchange* pDX          // pointer to data exchange object
    )
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////// END OF FILE ( PCSample.cpp ) /////////////////////////////
