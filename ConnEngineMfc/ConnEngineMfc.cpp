// ConnEngineMfc.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ConnEngineMfc.h"

#define CONNENGINE_EXPORTS
#include "ConnEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CConnEngineMfcApp

BEGIN_MESSAGE_MAP(CConnEngineMfcApp, CWinApp)
	//{{AFX_MSG_MAP(CConnEngineMfcApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnEngineMfcApp construction

CConnEngineMfcApp::CConnEngineMfcApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_ListenSocket = 0;
	m_ConnectedSocket = 0;

	m_hWinHandle = 0;
	m_wReceiveMsgId = WM_APP;
	m_wSocketCloseMsgId = m_wReceiveMsgId + 1;

	m_pFnCallBack = 0;

	m_ulStateIndicators = 0;
}

CConnEngineMfcApp::~CConnEngineMfcApp()
{
	if (m_ListenSocket) {
		delete m_ListenSocket;
		m_ListenSocket = 0;
	}

	if (m_ConnectedSocket) {
		delete m_ConnectedSocket;
		m_ConnectedSocket = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CConnEngineMfcApp object

CConnEngineMfcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CConnEngineMfcApp initialization

BOOL CConnEngineMfcApp::InitInstance()
{
#if 0
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CConnEngineMfcApp::StartEngineAsServer (int nPort, HWND hWinHandle, UINT wReceiveMsgId)
{
	if (m_ListenSocket) {
		delete m_ListenSocket;
	}

	m_ListenSocket = new CConnEngineListenSocket;

	m_ListenSocket->Create (nPort);

	if (hWinHandle != 0)
		m_ListenSocket->m_hWinHandle = hWinHandle;
	else if (m_hWinHandle != 0)
		m_ListenSocket->m_hWinHandle = m_hWinHandle;

	if (wReceiveMsgId != 0)
		m_ListenSocket->m_wReceiveMsgId = wReceiveMsgId;
	else if (m_hWinHandle != 0)
		m_ListenSocket->m_wReceiveMsgId = m_wReceiveMsgId;

	m_ListenSocket->m_theApp = this;
	m_ulStateIndicators |= CONNENGINE_STATUS_SERVERMODE;
	m_ListenSocket->Listen (15);

	return TRUE;
}

BOOL CConnEngineMfcApp::StopEngineAsServer ()
{
	if (m_ListenSocket != 0) {
		m_ListenSocket->Close ();
		delete m_ListenSocket;
		m_ListenSocket = 0;

	}

	m_ulStateIndicators &= ~(CONNENGINE_STATUS_SERVERMODE);

	return TRUE;
}

BOOL CConnEngineMfcApp::StartEngineAsClient (TCHAR *ptcsAddress, int nPort, HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId)
{
	BOOL  bRetStatus = TRUE;

	if (m_ConnectedSocket) {
		delete m_ConnectedSocket;
	}

	m_ConnectedSocket = new CConnEngineSocket;

	m_ConnectedSocket->Create ();

	if (hWinHandle != 0)
		m_ConnectedSocket->m_hWinHandle = hWinHandle;
	else if (m_hWinHandle != 0)
		m_ConnectedSocket->m_hWinHandle = m_hWinHandle;

	if (wReceiveMsgId != 0)
		m_ConnectedSocket->m_wReceiveMsgId = wReceiveMsgId;
	else if (m_hWinHandle != 0)
		m_ConnectedSocket->m_wReceiveMsgId = m_wReceiveMsgId;

	if (wSocketCloseMsgId != 0)
		m_ConnectedSocket->m_wSocketCloseMsgId = wSocketCloseMsgId;
	else if (m_hWinHandle != 0)
		m_ConnectedSocket->m_wSocketCloseMsgId = m_wSocketCloseMsgId;

	m_ConnectedSocket->m_theApp = this;

	// If the IP address string is a NULL pointer or an empty string then use localhost
	m_ulStateIndicators |= CONNENGINE_STATUS_CLIENTMODE;
	if (ptcsAddress == 0 || ptcsAddress[0] == 0) {
		if (! m_ConnectedSocket->Connect (_T("127.0.0.1"), nPort)) {
			m_ConnectedSocket->m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONN_ERROR;
			m_ConnectedSocket->m_dwLastError = GetLastError();
			// There are a couple of possible errors from GetLastError() that indicate transient conditions.
			//   10035  WSAEWOULDBLOCK - A non-blocking socket operation could not be completed immediately.
			TRACE3("%S(%d): CConnEngineMfcApp::StartEngineAsClient(). GetLastError = %d\n", __FILE__, __LINE__, m_ConnectedSocket->m_dwLastError);

			if (m_ConnectedSocket->m_dwLastError != WSAEWOULDBLOCK)
				bRetStatus = FALSE;

			// NOTE:  do not delete the m_ConnectedSocket object since it contains status
			//        information on the last attempt to connect.
		} else {
			m_ConnectedSocket->m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONNECTED;
		}
	}
	else {
		if (! m_ConnectedSocket->Connect (ptcsAddress, nPort)) {
			m_ConnectedSocket->m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONN_ERROR;
			m_ConnectedSocket->m_dwLastError = GetLastError();
			// There are a couple of possible errors from GetLastError() that indicate transient conditions.
			//   10035  WSAEWOULDBLOCK - A non-blocking socket operation could not be completed immediately.
			TRACE3("%S(%d): CConnEngineMfcApp::StartEngineAsClient() to %s\n", __FILE__, __LINE__, ptcsAddress);
			TRACE3("%S(%d): CConnEngineMfcApp::StartEngineAsClient(). GetLastError = %d\n", __FILE__, __LINE__, m_ConnectedSocket->m_dwLastError);

			if (m_ConnectedSocket->m_dwLastError != WSAEWOULDBLOCK)
				bRetStatus = FALSE;

			// NOTE:  do not delete the m_ConnectedSocket object since it contains status
			//        information on the last attempt to connect.
		} else {
			m_ConnectedSocket->m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONNECTED;
		}
	}

	return bRetStatus;
}

BOOL CConnEngineMfcApp::StopEngineAsClient ()
{
	if (m_ConnectedSocket != 0) {
		m_ConnectedSocket->m_ulStateIndicators = 0;
		m_ConnectedSocket->Close ();
		delete m_ConnectedSocket;
		m_ConnectedSocket = 0;
	}

	m_ulStateIndicators &= ~(CONNENGINE_STATUS_CLIENTMODE);

	return TRUE;
}


ULONG CConnEngineMfcApp::RetrieveStatus (ULONG *ulStateIndicators)
{
	ULONG  ulState = m_ulStateIndicators;

	if ((ulState & CONNENGINE_STATUS_CLIENTMODE) != 0) {
		if (m_ConnectedSocket != 0)
			ulState |= m_ConnectedSocket->m_ulStateIndicators;
	}

	if ((ulState & CONNENGINE_STATUS_SERVERMODE) != 0) {
		if (m_ListenSocket != 0) {
			if (m_ListenSocket->m_iConnectedSocketInUse) {
				ulState |= m_ListenSocket->m_ConnectedSocket.m_ulStateIndicators;
			}
		}
	}

	return ulState;
}


//=====================================================================
//
//----------------    CConnEngine class follows    --------------------

// This is an example of an exported function.
CONNENGINE_API int fnConnEngineSetDomainNamePort(CONNENGINEHANDLE hConnEngineSocket, char *aszDomainName, int nPortNo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return 42;
}

CONNENGINE_API int fnConnEngineStartEngine (int nPort, HWND hWinHandle, UINT wReceiveMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (hWinHandle == 0)
		hWinHandle = theApp.m_hWinHandle;

	if (wReceiveMsgId == 0)
		wReceiveMsgId = theApp.m_wReceiveMsgId;

	theApp.StartEngineAsServer (nPort, hWinHandle, wReceiveMsgId);
	return 0;
}

CONNENGINE_API int fnConnEngineStopEngine ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.StopEngineAsServer();
	return 0;
}

CONNENGINE_API TCHAR * fnConnEngineRetrieveNextMessage (CONNENGINEHANDLE hConnEngineSocket, TCHAR *tcBuffer, LONG tcBufferSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return tcBuffer;
}

CONNENGINE_API int fnConnEngineSendMessage (CONNENGINEHANDLE hConnEngineSocket, LPCTSTR tcBuffer, LONG tcBufferSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (theApp.m_ListenSocket != 0) {
		if (theApp.m_ListenSocket->SockSendText(tcBuffer, tcBufferSize))
			return 1;
	}
	else if (theApp.m_ConnectedSocket != 0) {
		if (theApp.m_ConnectedSocket->SockSendText(tcBuffer, tcBufferSize))
			return 1;
	}
	return 0;
}


// Indicates the status of the engine and connectivity
// Returned result is a bit mask with the following settings
//   Mask Value              Meaning
//   0x00000001     Listening for connection
//   0x00000002     Connected
//   0x00000010     Message is available for read
//   0x00000100     Read message buffers full, none available
//   0x00000200     Write message buffers full, none available
CONNENGINE_API unsigned long fnConnEngineStartStatus (CONNENGINEHANDLE hConnEngineSocket)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	unsigned long  ulIndicators = 0;

	if (theApp.m_ListenSocket != 0)
		ulIndicators |= CONNENGINE_STATUS_SERVERMODE;

	return ulIndicators;
}

CONNENGINE_API int fnConnEngineSetWindowHandle (CONNENGINEHANDLE hConnEngineSocket, HWND hWinHandle, UINT wReceiveMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.m_hWinHandle = hWinHandle;
	theApp.m_wReceiveMsgId = wReceiveMsgId;

	if (theApp.m_ListenSocket != 0) {
		theApp.m_ListenSocket->SetWindowHandle (hWinHandle, wReceiveMsgId);
	}
	return 0;
}

CONNENGINE_API int fnConnEngineSetWindowHandleExt (CONNENGINEHANDLE hConnEngineSocket, HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.m_hWinHandle = hWinHandle;
	theApp.m_wReceiveMsgId = wReceiveMsgId;
	theApp.m_wSocketCloseMsgId = wSocketCloseMsgId;

	if (theApp.m_ListenSocket != 0) {
		theApp.m_ListenSocket->SetWindowHandleExt (hWinHandle, wReceiveMsgId, wSocketCloseMsgId);
	}
	return 0;
}

CONNENGINE_API int fnConnEngineSetCallBack (CONNENGINEHANDLE hConnEngineSocket, TCHAR * ((*pFnCallBack)(TCHAR *pMsg)))
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.m_pFnCallBack = pFnCallBack;

	if (theApp.m_ListenSocket != 0) {
		theApp.m_ListenSocket->SetCallBack(pFnCallBack);
	}
	return 0;
}

CONNENGINE_API int fnConnEngineConnectTo (TCHAR *tcsAddress, int nPort, HWND hWinHandle, UINT wReceiveMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (hWinHandle == 0)
		hWinHandle = theApp.m_hWinHandle;

	if (wReceiveMsgId == 0)
		wReceiveMsgId = theApp.m_wReceiveMsgId;

	theApp.StartEngineAsClient (tcsAddress, nPort, hWinHandle, wReceiveMsgId);

	return 0;
}

CONNENGINE_API int fnConnEngineConnectToExt (TCHAR *tcsAddress, int nPort, HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (hWinHandle == 0)
		hWinHandle = theApp.m_hWinHandle;

	if (wReceiveMsgId == 0)
		wReceiveMsgId = theApp.m_wReceiveMsgId;

	if (wSocketCloseMsgId == 0)
		wSocketCloseMsgId = theApp.m_wSocketCloseMsgId;

	return theApp.StartEngineAsClient (tcsAddress, nPort, hWinHandle, wReceiveMsgId, wSocketCloseMsgId);
}

CONNENGINE_API int fnConnEngineDisconnect (VOID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.StopEngineAsClient ();

	return 0;
}

CONNENGINE_API ULONG  fnConnEngineRetrieveStatus (CONNENGINEHANDLE hConnEngineSocket)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theApp.RetrieveStatus (0);
}
