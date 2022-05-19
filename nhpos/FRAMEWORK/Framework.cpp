/*************************************************************************
 *
 * Framework.cpp
 *
 * $Workfile:: Framework.cpp                                             $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 * --------------------------------------------------------------------------
 *    Georgia Southern University Research Services Foundation
 *    donated by NCR to the research foundation in 2002 and maintained here
 *    since.
 *       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
 *       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
 *       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
 *       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
 *       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
 *       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
 *       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
 *
 *    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
 *
 *  A short history of the NCR Project.
 *  NCR was reviewing its intellectual property portfolio and as part of that effort,
 *  led by NCR vice president Joanne Walter, determined that the NeighborhoodPOS point
 *  of sale product was a candidate for either selling to some one or donating to a
 *  university in return for a tax break. 
 *  In 2001 and 2002 Walter worked with Annie Burriss of the University System of Georgia
 *  to develop an arrangement with support from Governor Roy Barnes.
 *  The first was Georgia Tech who refused the property.  Regent Martin NeSmith,
 *  an alum, suggested Georgia Southern University and in 2002 a deal was struck
 *  involving NCR donating the source code and intellectual property to Georgia
 *  Southern, the state legislature providing funding through the ICAPP program,
 *  and Georgia Southern University providing the facilities and faculty and staff
 *  for development of the software to continue.
 *
 *  Dr. Susan Williams, a faculty member of the Information Systems department in
 *  the College of Business Administration was chosed by Dr. Bruce Grube, President
 *  of the university, and Dr. Vaughn Vandegrift, Provost, to lead the creation of
 *  a software development group in 2002 primarily of students who would be working with
 *  the source code and developing new features and functionality.
 *
 *  An important person during this time was Mr. Dan Parliman, NeighborhoodPOS Product
 *  Manager at NCR, who assisted with the many details of collecting and organizing
 *  the various materials and training on the product.
 *
 *  In August of 2002, Mr. Richard Chambers was hired into the NCR Project by Dr. Williams
 *  as project manager and senior developer.
 *
 *  The first student team of six students was formed from three different departments:
 *  Department of Information Systems, Department of Information Technology, and Department
 *  of Computer Science.  With the creation of the new College of Information Technology
 *  in 2003 with Dr. James Bradford as founding Dean, the group moved from facilities in
 *  a room in the College of Business Administration to the new College of IT building.
 *
 *  In early 2003, NCR triggered a clause in the Donation Agreement by entering into a licensing
 *  agreement with the Foundation and the Georgia Southern development group provided several
 *  updates to NHPOS 1.4 for the NCR 7448. By the end of 2003, NCR announced they would be
 *  manufacture discontinuing the NCR 7448 due to its low profit margins and low volume of sales.
 *  The Georgia Southern development group then ported NHPOS from the Windows CE 2.12 operating
 *  system used by the NCR 7448 with a keyboard data entry to the Windows XP operating system.
 *  This change in operating system platforms was also accompanied by the following changes:
 *    - support touchscreen terminals by inserting a Graphical User Interface allowing mouse actions
 *    - change from the ANSI 8 bit character set to the UNICODE 16 bit character set
 *    - change to using OPOS interfaces for receipt printer and other devices
 *    - add in support for the Datacap DSI Client interface to the NETePay Electronic Payment server
 *  The change in character set was driven by a desire to offer NHPOS 2.0 to the Chinese market in
 *  time for the Beijing 2008 Summer Olympics, a desire that was not fulfilled.
 *************************************************************************
 *
 * $History:: Framework.cpp                                              $
 *
 ************************************************************************/

#include "stdafx.h"
#include "DeviceEngine.h"
#include "Framework.h"
#include "FrameworkIO.h"  // for vbo defines`
#include "Evs.h"
#include "BlfWif.h"
//#include "DFPRExports.h"
#include "SecurityCommandLineParser.h"
#include "appllog.h"
#include "CWebBrowser2.h"

#define _WIN32_DCOM
using namespace std;
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INIT_OLESYSTEM   HRESULT xOleResult; xOleResult = ::OleInitialize(NULL);\
	NHPOS_ASSERT (xOleResult != OLE_E_WRONGCOMPOBJ);\
	NHPOS_ASSERT (xOleResult != RPC_E_CHANGED_MODE);\
	NHPOS_ASSERT (xOleResult != E_UNEXPECTED);\
	NHPOS_ASSERT (xOleResult != E_OUTOFMEMORY);\
	NHPOS_ASSERT (xOleResult != S_FALSE);\
	NHPOS_ASSERT (xOleResult != E_INVALIDARG);

#define UNINIT_OLESYSTEM   	::OleUninitialize();

/////////////////////////////////////////////////////////////////////////////
// CFrameworkApp

BEGIN_MESSAGE_MAP(CFrameworkApp, CWinThread)
	//{{AFX_MSG_MAP(CFrameworkApp)
	//}}AFX_MSG_MAP
    ON_THREAD_MESSAGE(WU_EVS_UPDATE, OnEvsUpdate)
    ON_THREAD_MESSAGE(WM_APP_DEVICEENGINE_IOCTL, OnDeviceEngineIOCTL)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameworkApp construction

CFrameworkApp::CFrameworkApp()
//	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#ifndef DEVENG_REPLACEMENT
	m_pDeviceEngine = 0;
#else
	m_pDeviceEngineThread = 0;
#endif
}

CFrameworkApp::~CFrameworkApp()
{
	//SR 589 We rest for 3 seconds so that totals can completely update 
	//before shutting down. JHHJ
	// If we do the following PifSleep() then we are basically
	// causing the application MessagePump to be halted as well.
	// This is causing problems in the shutdown and we are seeing
	// an assert in the EventSubsystem with a PostMessage() call that
	// is returning an error and GetLastError() is returning
	// dwLastError == 1816 means Window Message queue getting full, ERROR_NOT_ENOUGH_QUOTA
	// Richard Chambers,  Feb-04-2008,  Rel 2.1.0.67
//    PifSleep(3000);                                         
//	PifShutdown();

#ifndef DEVENG_REPLACEMENT
	if(m_pDeviceEngine)
	{
		m_pDeviceEngine->Close();
		delete m_pDeviceEngine;
		m_pDeviceEngine = 0;
	}
#else
	if(m_pDeviceEngineThread){
		//TODO: send message to shut down thread
	}
#endif


	NHPOS_NONASSERT_TEXT("==NOTE: CFrameworkApp::~CFrameworkApp() Genpos terminated.");

	BlFinalizeAndClose();    // CliParaSaveFarData() update memory resident data snapshot and the ASSRTLOG file offset pointers.

	TRACE0("CFrameworkApp::~CFrameworkApp() called.\n");

	UNINIT_OLESYSTEM;
}

// Shutdown Thread waiting on the event
// We need to provide time for GenPOS to finish shutting down so we are using an indirection
// through an event to this shutdown thread in order to provide the additional time for
// things like final control string processing, etc.
// Without this we can see asserts from the Evs subsystem as it tries to update windows
// which no longer exist.  See also CFrameworkWndDoc::OnShutdownMsgRcvd() and PifShutDownApp().
UINT AFX_CDECL FrameworkShutdownThread (LPVOID p)
{
	CFrameworkApp *myApp = (CFrameworkApp *)AfxGetApp();

	while (WaitForSingleObject (myApp->m_hEventShutdownNHPOS, INFINITE) == WAIT_OBJECT_0) {
		Sleep(3000);                                         
		if (PifShutdown() < 0) {
			// a shutdown has already been initiated so now just need to finish closing up everything.
			void * NhposMainHwnd = 0;
			PifGetWindowHandle(&NhposMainHwnd);
			// Give GenPOS time to finish processing any final actions.
			// I would prefer an actual signal or event however not really possible with
			// this architecture.
			Sleep(2000);                                         
			if (NhposMainHwnd) {
				::PostMessage ((HWND)NhposMainHwnd, WM_CLOSE, 0, 0);
			}
			Sleep(10);                                         
			break;
		}
		// PifShutdown() indicates that it is starting a shutdown so we need to wait for the event.
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFrameworkApp object

// WCE MFC apps require the application name to be specified in the CWinApp
// constructor. A help contents filename may also be specified.

CFrameworkApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFrameworkApp initialization
BOOL CFrameworkApp::InitInstance()
{
	INIT_OLESYSTEM;
	AfxEnableControlContainer();

    TCHAR   achDevice[SCF_USER_BUFFER];
    TCHAR   achDLL[SCF_USER_BUFFER];
	int     iProgUpper = 20;
    DWORD   dwResult = 0;
    DWORD   dwCount = 0;

	// Check the command line arguments for 
	// a "register" parameter from DeviceConfig, if it exists
	// bring up the VBOLock registration
	// Dialog, then shutdown NHPOS.
	CSecurityCommandLineParser csCmd;
	this->ParseCommandLine(csCmd);
	
	m_bSmallWindow = FALSE;
	if (csCmd.m_enRegisterState == CSecurityCommandLineParser::StateServerOnly) {
		m_bSmallWindow = TRUE;
	}

//	m_pAXControl->AXInterface (FRAMEWORK_AXCTL_LIC_DETAILS, 0, 0);

	//SR 490 & 491, We do not want device config to be able to run while NHPOS is running
	//If it IS running while NHPOS is already running, it will be in a "read only" mode
	//where the user can read the information in device config, but cannot make any new changes.
	//if NHPOS is already running, we simply exit out of 
	HANDLE hMutexNHPOS, hMutexDeviceConfig;
	hMutexNHPOS = OpenMutex(MUTEX_ALL_ACCESS, 0, _T("NeighborhoodPOSApp"));
	hMutexDeviceConfig = OpenMutex(MUTEX_ALL_ACCESS, 0, _T("DeviceConfigApp"));
	m_hEventShutdownNHPOS = CreateEvent (NULL, TRUE /*bManualReset*/, FALSE /*bInitialState*/, _T("NeighborhoodPOSAppShutdown"));
 
	//if this mutex exists, then device conifg is already open, so we
	//close out the application
	if(hMutexDeviceConfig)
	{
		CString myString;
		myString.LoadString(IDS_DEVICE_CONFIG_OPEN);
		this->m_pMainWnd->MessageBox(myString,NULL,MB_OK);
		exit(1);
	}
	{
		// If in registration mode-> shutdown nhpos
		if (csCmd.m_enRegisterState == CSecurityCommandLineParser::StateShutdown) {
			if (hMutexNHPOS && !SetEvent (m_hEventShutdownNHPOS)) {
				char xBuff[128];

				sprintf (xBuff, "hEventShutdownNHPOS failed. GetLastError = %d", GetLastError());

				NHPOS_ASSERT_TEXT(0, xBuff);
			}
			exit(1);
		}
	}
	//If the mutex does not exist, we will create it, because this is the first time that
	//the application has been created. 
	if (!hMutexNHPOS)
	{
	  // Mutex doesn’t exist. This is
	  // the first instance so create
	  // the mutex.
	  hMutexNHPOS = CreateMutex(0, 0, _T("NeighborhoodPOSApp"));
	} else {
		exit(1);
	}

	{
		CWinThread * hHandle;
		DWORD       dwThreadId;

		hHandle = AfxBeginThread (FrameworkShutdownThread, (LPVOID)&dwThreadId, 0, 0, 0, 0);
	}

#if defined(_DEBUG)
	// if we are in debug mode, then let's give designers a break and shorten the wait time
	iProgUpper = 2;
#endif

	_tcscpy (achDevice, SCF_DATANAME_TIMEOUT);
	dwResult = ScfGetActiveDevice(SCF_TYPENAME_FLASHSCREEN, &dwCount, achDevice, achDLL);
	if (dwResult == SCF_SUCCESS) {
		iProgUpper = _ttoi (achDLL);
		if (iProgUpper < 1)
			iProgUpper = 5;
	}

	CDialog myFlashScreen;
	CProgressCtrl *myProgress;

	myFlashScreen.Create (IDD_FRAMEWORK_DIALOG);
	myProgress = (CProgressCtrl *) myFlashScreen.GetDlgItem (IDC_PROGRESS1);
	myProgress->SetRange (1, iProgUpper);
	myProgress->SetStep (1);
	myProgress->SetPos (1);

	CStatic *myStatic;
	CString myFileIdVersion;
	myFileIdVersion.LoadString (FILE_ID_VER);
	BlSetFrameworkVersionNumber ((LPCTSTR)myFileIdVersion);
	myFileIdVersion = _T("Rel. ") + myFileIdVersion;
	myStatic = (CStatic *) myFlashScreen.GetDlgItem (IDC_STATIC_VERSION);
	myStatic->SetWindowText (myFileIdVersion);

	myFlashScreen.ShowWindow (SW_SHOW);

	for (int iProgPos = 1; iProgPos <= iProgUpper; iProgPos++) {
		Sleep (1000);
		myProgress->SetPos (iProgPos);
	}
	myFlashScreen.ShowWindow (SW_HIDE);
	myFlashScreen.DestroyWindow ();

#ifndef DEVENG_REPLACEMENT
	// create a device engine
	m_pDeviceEngine = new CDeviceEngine;
	NHPOS_ASSERT_TEXT(m_pDeviceEngine != 0, "**ERROR Error allocating CDeviceEngine.");
	m_pDeviceEngine->Open();
#else
	m_pDeviceEngineThread = AfxBeginThread(RUNTIME_CLASS(DeviceEngineThread));
#endif

	/* When initializing, both layout files have to be created if they
	   are not currently there. If they are not created, then when they
	   are sent a layout that is not currently on the terminal NHPOS will
	   crash. To solve this problem both files will be created if they do
	   not already exist.
	 */
	if( !( (InitializeLayout( _T("\\FlashDisk\\NCR\\Saratoga\\Database\\PARAMLA") )) &&
		   (InitializeLayout( _T("\\FlashDisk\\NCR\\Saratoga\\Database\\PARAMLB") ))))
	{
		return FALSE;
	}

	// Set layout file name from system configuration ESMITH LAYOUT
	CString strLayoutFile, strPath(_T("\\FlashDisk\\NCR\\Saratoga\\Database\\"));
    CONST SYSCONFIG *pSysConfig = PifSysConfig();

	// Determine which layout file to use for this terminal.
	// Query the terminal information to obtain the layout file.
	// We check the name below to ensure it starts with PARAML as the
	// layout file name should be PARAMLA through PARAMLZ.  If it isn't
	// valid then we fall back to the old naming convention of if we are
	// touchscreen then use PARAMLA and if we are keyboard use PARAMLB.
	strLayoutFile = BlFwIfTerminalLayout (NULL);
	strLayoutFile.TrimLeft ();
	strLayoutFile.TrimRight ();

	if (strLayoutFile.IsEmpty () || strLayoutFile.Left(6).Compare (_T("PARAML"))) {
		if(pSysConfig->uchKeyType == KEYBOARD_TOUCH)
		{
			strLayoutFile = _T("PARAMLA");
		}
		else
		{
			strLayoutFile = _T("PARAMLB");
		}
	}

	strLayoutFile = strPath + strLayoutFile;

	CFile  fTouchLayout;
	CFileException exTouch;

	// Open the Layout file which will then be treated as an CArchive file.
	// The Layout file contains the serialized window objects to be displayed.
	if (!fTouchLayout.Open (strLayoutFile, CFile::modeRead | CFile::shareDenyWrite, &exTouch)) {
		TCHAR szError[258];
		exTouch.GetErrorMessage(szError, 256);
		NHPOS_ASSERT_TEXT(0, "**ERROR Error opening PARAML layout file.");
		AfxMessageBox (szError);

		// Since the specified file is not available then lets try the default file
		// for this keyboard type.  If that doesn't work then we will give up.
		if(pSysConfig->uchKeyType == KEYBOARD_TOUCH)
		{
			strLayoutFile = _T("PARAMLA");
		}
		else
		{
			strLayoutFile = _T("PARAMLB");
		}

		strLayoutFile = strPath + strLayoutFile;

		if (!fTouchLayout.Open (strLayoutFile, CFile::modeRead | CFile::shareDenyWrite, &exTouch)) {
			NHPOS_ASSERT_TEXT(0, "**ERROR Unable to open default layout file.");
			return FALSE;
		}
	}

	CArchive touchArchive (&fTouchLayout, CArchive::load);
	try {
		touchWindow.Serialize (touchArchive);

		// CSMALL - Check to make sure set Layout is not a newer 
		//	version than NHPOS, if it is, exit out so NHPOS doesn't crash.
		// myFileIdVersion;
/*		int begin = myFileIdVersion.Find('v', 0);
		int end = myFileIdVersion.Find('.',begin);

		UINT nhposMajorVersion = _ttoi(myFileIdVersion.Mid(begin+1, end));
		begin = myFileIdVersion.Find('.',begin+1); // move to next 'section' of version
		end = myFileIdVersion.Find('.',begin+1);
		UINT nhposMinorVersion = _ttoi(myFileIdVersion.Mid(begin+1, end));
		begin = myFileIdVersion.Find('.',begin+1); // move to next 'section' of version
		end = myFileIdVersion.Find('.',begin+1);
		UINT nhposReleaseVersion = _ttoi(myFileIdVersion.Mid(begin+1, end));

		if(touchWindow.documentAttributes.nVersion[0] > nhposMajorVersion ||
				touchWindow.documentAttributes.nVersion[1] > nhposMinorVersion ||
				touchWindow.documentAttributes.nVersion[2] > nhposReleaseVersion)
		{
			TCHAR szError[62] = _T("The set layout file is not supported in this version of NHPOS");
			AfxMessageBox(szError, MB_OK);
			return FALSE;
		}
*/
	}
	catch (CException *ex) {
		TCHAR szError[258];
		ex->GetErrorMessage(szError, 256);
		AfxMessageBox(szError, MB_OK);
		ex->Delete();
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_LAYOUT_FAILED);
		NHPOS_ASSERT_TEXT(0, "**ERROR Exception processing Layout file.");

		// we will drop through and finish coming up so that a remote application can
		// connect and download something that does work.
	}
#if 0
	catch (CMemoryException *ex) {
		TCHAR szError[258];
		ex->GetErrorMessage(szError, 256);
		strError.Format(_T("Error Serializing: %s"), szError);
		AfxMessageBox(strError, MB_OK);
		ex->Delete();
		return FALSE;
	}
	catch (CArchiveException *ex) {
		TCHAR szError[258];
		ex->GetErrorMessage(szError, 256);
		strError.Format(_T("Error Serializing: %s"), szError);
		AfxMessageBox(strError, MB_OK);
		ex->Delete();
		return FALSE;
	}
#endif

	touchArchive.Close();
	fTouchLayout.Close();
	if (m_bSmallWindow) {
		CRect myRect(0, 0, 200, 200);
		myRect.right = GetSystemMetrics (SM_CXSCREEN);
		myRect.left = myRect.right - 200;
		touchWindow.CreateDocument (0, 0, &myRect);
	} else {
		touchWindow.CreateDocument ();
	}

	// Set the main window member of the CWinApp class so that
	// the application will startup and run.  Otherwise, it will just
	// terminate with an error.
	m_pMainWnd = &touchWindow;

#ifndef DEVENG_REPLACEMENT
	m_pDeviceEngine->m_myAppForMessages = this;
#else
	//see DeviceEngineThread::InitInstance()
#endif

	BlFwIfSetFrameworkTouchWindow (m_pMainWnd);

	BlFwIfSetWindowHandle (m_pMainWnd->m_hWnd);

	BlInitConnEngine(8282, m_pMainWnd->m_hWnd, WM_APP_CONNENGINE_MSG_RCVD);

	//call function to display default screens on boot up
	touchWindow.DefaultUserScreenLocal();
	touchWindow.ShowHideGroupMsgRcvd ((WPARAM)0xffffffff, (LPARAM)2);   // must be same as BlFwIfGroupWindowHide (0xffffffff)

	// Remove log files ESMITH PRTFILE
	DeleteFile(BL_PRTFILE_PROG);
	DeleteFile(BL_PRTFILE_SUPER);
	DeleteFile(BL_PRTFILE_REG);

	{
		char xBuff[128];

		sprintf (xBuff, "==NOTE: Serial Number %S", m_BiosSerialNumber);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	BlDelayBalance();

	BlFinalizeAndClose();    // CliParaSaveFarData() update memory resident data snapshot and the ASSRTLOG file offset pointers.

	return TRUE;
}

BOOL CFrameworkApp::PreTranslateMessage(MSG* pMsg)
{

#ifndef DEVENG_REPLACEMENT
	if (m_pDeviceEngine->PreTranslateMessage(pMsg)) //Send to DeviceEngine
	{
		return TRUE;
	}
#else
	//Send to DeviceEngine
	if(m_pDeviceEngineThread->PreTranslateMessage(pMsg))
		return TRUE;
#endif

	return CWinApp::PreTranslateMessage(pMsg); //Default let a handle keyboard message

}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    update message from event subsystem
//
// Prototype:   LRESULT     CFrameworkDlg::OnEvsUpdate(wParam, lParam);
//
// Inputs:      WPARAM      wParam;     -
//              LPARAM      lParam;     -
//
// Outputs:     LRESULT     lResult;    - result code
//
/////////////////////////////////////////////////////////////////////////////

void CFrameworkApp::OnEvsUpdate(WPARAM wParam, LPARAM lParam)
{
//    SHORT   sResult;
//    SHORT   nNewMode;

   /*is my message ?

    if (wParam != SCRMODE_ID)               // invalid
    {
        return 0;                           // exit ...
    }

    // get new keylock position

    sResult = ::EvsGetData(
                    SCRMODE_ID,
                    (ULONG)m_hWnd,
                    &nNewMode,
                    sizeof(nNewMode),
                    NULL);

    if (sResult != EVS_OK)
    {
        return 0;                               // exit ...
    }

    //hange active container

    CContainer* pContainer;
    POSITION    pos = m_lstContainer.GetHeadPosition();

    while (pos != NULL)
    {
        pContainer = (CContainer*)m_lstContainer.GetNext(pos);

        if (pContainer->GetId() == nNewMode)
        {
            pContainer->SetForegroundWindow();
            pContainer->ShowWindow(SW_SHOWMAXIMIZED);
            pContainer->Invalidate();
        }
    }
*/
    // exit ...

}

void CFrameworkApp::OnDeviceEngineIOCTL(WPARAM wParam, LPARAM lParam)
{
#ifndef DEVENG_REPLACEMENT
	m_pDeviceEngine->HandleIOCTL (wParam, lParam);
#else
#endif
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Initialize a layout file with the default layout if it
//				is empty. 
//
// Prototype:   BOOL     CFrameworkDlg::InitializeLayout(CFile fLayout);
//
// Inputs:      CFile    fLayout	- Layout file to initialize
//
// Outputs:     BOOL	 bResult	- Returns whether the action failed or
//									  succeded
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFrameworkApp::InitializeLayout(LPCTSTR lpszFileName)
{
	BOOL bResult;
	CFileException ex;
	CString strError;
	TCHAR szError[1024];

	CFile fLayout(lpszFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

	if(!fLayout.GetLength())
	{
		UINT sizeOfRead;
		BYTE* lpBuff[1500];

		try {
			CFile fDefaultLayout(_T("\\FlashDisk\\NCR\\Saratoga\\Database\\defaultLayout"), CFile::modeRead);
			while(sizeOfRead = fDefaultLayout.Read(lpBuff, 1024))
			{
				fLayout.Write(lpBuff, sizeOfRead);
			}

			bResult = TRUE;
		}
		catch (CFileException* e) {
			e->GetErrorMessage(szError,1023,NULL);
			strError.Format(_T("Error opening default layout file.  Exist? %s"), szError);
			AfxMessageBox(strError, MB_OK);
			bResult = FALSE;
			e->Delete();
		}
	}
	else{ bResult = TRUE; }

	fLayout.Close();

	return bResult;
}


