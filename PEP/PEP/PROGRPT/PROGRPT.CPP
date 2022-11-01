/////////////////////////////////////////////////////////////////////////////
// progrpt.cpp : Defines the class behaviors for the application.
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Dec-20-02           R.Chambers   Changes to address SR92, failed under Windows 98
//                                  also changed resource.h to put resource ids in
//                                  proper ranges.  This was the major problem for the
//                                  Windows 98 message box about corrupt file.
// Dec-30-02		   R.Herrington Fixed SR92 previous fix was only partial
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "mainfrm.h"
#include "progrfrm.h"
#include "progrdoc.h"
#include "listvw.h"
#include "reportvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char  THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

wchar_t szClassName[] = L"Programmable Report";

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp

BEGIN_MESSAGE_MAP(CProgrptApp, CWinApp)
	//{{AFX_MSG_MAP(CProgrptApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp construction

CProgrptApp::CProgrptApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CProgrptApp::~CProgrptApp()
{
	delete ptheInfo;					// remove information
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CProgrptApp object

CProgrptApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// The one and only object

CInfo* ptheInfo;						// information of report field

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp initialization

BOOL CProgrptApp::InitApplication()
{
	DWORD lastError;
	BOOL bRet = TRUE;
	
	CWinApp::InitApplication();  

	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));	// start with NULL defaults

	// get class information for default window class.
	//RPH SR#92 12/30/02
	if ( ! ::GetClassInfo(AfxGetInstanceHandle(),
		AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW ,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		CWinApp::LoadIcon(IDR_MAINFRAME) ),&wndcls)) {
		bRet = FALSE;
		lastError = GetLastError();
	}

	// substitute unique class name for new class
	wndcls.lpszClassName = szClassName;
	
	// register new class and return the result code
	if ( ! ::RegisterClass(&wndcls)) {
		bRet = FALSE;
		lastError = GetLastError();
	}

	return bRet;
}

BOOL CProgrptApp::InitInstance()
{
	if (! FirstInstance())				// already executed
		return FALSE;

	ChangeIniPathName();				// change .INI path name

	if (! CheckIniVersion())			// .INI not found
	{
		AfxMessageBox(IDS_ERRMSG_INI, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetDialogBkColor(RGB(255, 255, 255));	// Set dialog background color to white
	LoadStdProfileSettings();  				// Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pListViewTemplate = new CMultiDocTemplate(
		IDR_REPORTTYPE,
		RUNTIME_CLASS(CProgrptDoc),
		RUNTIME_CLASS(CMDIChildWnd),		// standard MDI child frame
		RUNTIME_CLASS(CListView));
	AddDocTemplate(m_pListViewTemplate);

	m_pReportViewTemplate = new CMultiDocTemplate(
		IDR_SHEETTYPE,
		RUNTIME_CLASS(CProgrptDoc),
		RUNTIME_CLASS(CProgrptFrame),		// standard MDI child frame
		RUNTIME_CLASS(CReportView));
	AddDocTemplate(m_pReportViewTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = pMainFrame;

	// enable file manager drag/drop and DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();
	m_pMainWnd->DragAcceptFiles();

	// The main window has been initialized, so show and update it.
	pMainFrame->InitialShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// read information of report field
	AfxGetApp()->m_pMainWnd->PostMessage(ID_STATUS_MSG, IDS_STSMSG_PROGRPT);
	ptheInfo = new CInfo();
	AfxGetApp()->m_pMainWnd->PostMessage(ID_STATUS_MSG, IDS_STSMSG_CLEAR);

	// set format name of clip board
	m_uPrivateFmt = ::RegisterClipboardFormat(L"ProgrptFormat");

	// Check the command line to see if there is a quote string containing
	// a pathname or not.  If there is a quoted string, then we need to
	// remove the quotes so that we will open the file with the proper name.
	// Quoted filenames are provided to us on a drag and drop or if the
	// user should double click a .rpt file and we have an association
	// between progrpt.exe and the .rpt extension.
	// When PEP invokes progrpt, it does not use quotes around the file name.

	LPTSTR m_lpFirstCmdLine = m_lpCmdLine;
	for(;*m_lpCmdLine != '\"' && *m_lpCmdLine;m_lpCmdLine++);
	if(*m_lpCmdLine == '\"')
	{
		m_lpCmdLine++;
		m_lpFirstCmdLine = m_lpCmdLine;
	}
	for(;*m_lpCmdLine != L'\"' && *m_lpCmdLine; m_lpCmdLine++);
	*m_lpCmdLine = L'\0';

	m_lpCmdLine = m_lpFirstCmdLine;

	// simple command line parsing
	if (m_lpCmdLine[0] == L'\0')
	{
		// create a new (empty) document
		// OnFileNew();
	}
	else if (! IsFileExist(m_lpCmdLine))
	{
		OnFileNew();
		m_pMainWnd->PostMessage(ID_SET_PATHNAME);
	}
	else
	{
		// open an existing document
	
		OpenDocumentFile(m_lpCmdLine);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CProgrptApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp implementations

BOOL CProgrptApp::FirstInstance()
{
	CWnd *PrevCWnd, *ChildCWnd;
  
	// determine if another window with our class name exists...
	if (PrevCWnd = CWnd::FindWindow(szClassName, NULL))
	{
		ChildCWnd=PrevCWnd->GetLastActivePopup();	// if so, does it have any popups?
		PrevCWnd->SetForegroundWindow();			// Bring the main window to the top
		if (PrevCWnd->IsIconic()) 
			PrevCWnd->ShowWindow(SW_RESTORE);		// If iconic, restore the main window
		if (PrevCWnd != ChildCWnd) 
		ChildCWnd->SetForegroundWindow();			// If there are popups, bring them along too!
		return FALSE;								// Return FALSE.  This isn't the first instance
													// and we are done activating the previous one.
	}
	else
		return TRUE;								// First instance. Proceed as normal.
}

void CProgrptApp::ChangeIniPathName()
{
	// get path of .EXE
	wchar_t szBuffer[_MAX_PATH+1];
	GetModuleFileName(AfxGetInstanceHandle(), szBuffer, _MAX_PATH);

	int nLen = wcslen(szBuffer);
	ASSERT(nLen > 4 && szBuffer[nLen - 4] == L'.');	// must end in .EXE
	nLen -= 4;
	szBuffer[nLen] = L'\0';        					// no suffix

	static wchar_t szIni[] = L".INI";
	lstrcat(szBuffer, szIni);
	m_pszProfileName = _wcsdup(szBuffer);
}

BOOL CProgrptApp::CheckIniVersion()
{
	static wchar_t szEntry[] = L"Version";

	CString strData = AfxGetApp()->GetProfileString(szClassName, szEntry);
	if (strData.GetLength() > 0)
		return TRUE;
	else
		return FALSE;
}

BOOL CProgrptApp::IsFileExist(LPCTSTR lpszPathName)
{
	int i;
	char aszPathName[512];

	for (i = 0; i < 512 && lpszPathName[i]; i++) {
		aszPathName[i] = lpszPathName[i];
	}
	aszPathName[i] = 0;
	OFSTRUCT of;
	if (::OpenFile(aszPathName, &of, OF_EXIST) != HFILE_ERROR)
		return TRUE;
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp operations

/////////////////////////////////////////////////////////////////////////////
// CProgrptApp commands