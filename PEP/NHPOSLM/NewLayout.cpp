// NewLayout.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NewLayout.h"


#include "MainFrm.h"
#include "ChildFrm.h"
#include "NewLayoutDoc.h"
#include "NewLayoutView.h"
#include "DefaultEdit.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*This variable will need to be incremented ANY time any new attributes
are added to any of the classes for serialization purposes*/
		// incremented to 2 for Multiple OEP Window Features - CSMALL 1/11/2006
unsigned long CNewLayoutApp::SerializationVersion = 2;
/////////////////////////////////////////////////////////////////////////////
// CNewLayoutApp

BEGIN_MESSAGE_MAP(CNewLayoutApp, CWinApp)
	//{{AFX_MSG_MAP(CNewLayoutApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP, OnHelp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutApp construction

CNewLayoutApp::CNewLayoutApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	ClipBoardList.RemoveAll();
	ASSERT(ClipBoardList.IsEmpty());
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNewLayoutApp object

CNewLayoutApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutApp initialization

BOOL CNewLayoutApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_NEWLAYTYPE,
		RUNTIME_CLASS(CNewLayoutDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CNewLayoutView/*CLeftView*/));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CLOMCommandLineInfo cmdInfo;
	/*initialize language identifier to English so we wont have garbage if no language 
	flag is set on teh command line*/
	cmdInfo.lang = LANG_ENGLISH;
	cmdInfo.sublang = SUBLANG_ENGLISH_US;
	//CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	BOOL success = pMainFrame->ProcessCmdLineLang(cmdInfo.lang, cmdInfo.sublang);
	if(!success){
		AfxMessageBox(IDS_CMDLINE_LANG_NF,MB_OK,0);
	}
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->UpdateWindow();

	return TRUE;
}


int CNewLayoutApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
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

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
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
void CNewLayoutApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// App command to run the dialog
void CNewLayoutApp::OnHelp()
{
	WinHelp (0, HELP_FINDER);
}

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutApp message handlers




