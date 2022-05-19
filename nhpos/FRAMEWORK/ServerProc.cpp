// ServerProc.cpp : implementation file
//

#include "stdafx.h"
#include "framework.h"
#include "ServerProc.h"
#include "unicodeFixes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////

// misc. data structures

typedef struct _PROCADR {				// array of procedure address
	INT			iIndex;						// index to control
	LPCTSTR		lpName;						// ptr. to a proc. name
} PROCADR, *PPROCADR;



/////////////////////////////////////////////////////////////////////////////
// CServerProc

CServerProc::CServerProc(LPCTSTR lpszDllName /* = 0 */)
{
	if (lpszDllName && _tcslen(lpszDllName) > 4) {
		m_sDllName = lpszDllName;
	}
}

CServerProc::~CServerProc()
{
}

void CServerProc::SetDllName (LPCTSTR lpszDllName)
{
	if (lpszDllName && _tcslen(lpszDllName) > 4) {
		m_sDllName = lpszDllName;
	}
}


BOOL CServerProc::LoadDllProcs(LPCTSTR lpszDllName /*  = 0 */)
{
	BOOL		fCompleted = TRUE;		// assume good status
	FARPROC	*	ppfnProc;

	// declare procedure of System Monitor

	static	PROCADR		afnDlls[] = { {  1, _T("StartThread")			},
									  {  2, _T("StopThread")			},
									  {  3, _T("IssueRequest")			},
									  {  4, _T("ProvideCallBack")		},
									  {  5, _T("ProvideWinHandle")		},
									  {  6, _T("WaitOnConnection")		},
									  {  7, _T("ReadRequest")			},
									  {  8, _T("ParsePayLoadXML")		},
									  {  9, _T("IssueFilePointerRequest")	},
									  //{  7, _T("GetDeviceCapability")	},
									  //{  8, _T("GetParameter")			},
									  //{  9, _T("SetParameter")			},
									  {  0, NULL						}
									 };

	if (lpszDllName && _tcslen(lpszDllName) > 4) {
		m_sDllName = lpszDllName;
	}

	// load System Monitor dll

	if (! (m_hDll = AfxLoadLibrary(m_sDllName)))
	{
		DWORD	dwRC = ::GetLastError();
		return FALSE;
	}

	// get each procedure address and put into the myInterface struct.
	// The myInterface struct is of type ServerProcInterface which
	// contains a member for each of the possible server procedure DLL
	// entry points.

	for (PPROCADR pTable = afnDlls; pTable->lpName; pTable++)
	{
		// save the address

		switch (pTable->iIndex)
		{
		case 1:
			ppfnProc = (FARPROC *)&myInterface.m_pfnStartThread;
			break;
		case 2:
			ppfnProc = (FARPROC *)&myInterface.m_pfnStopThread;
			break;
		case 3:
			ppfnProc = (FARPROC *)&myInterface.m_pfnIssueRequest;
			break;
		case 4:
			ppfnProc = (FARPROC *)&myInterface.m_pfnProvideCallBack;
			break;
		case 5:
			ppfnProc = (FARPROC *)&myInterface.m_pfnProvideWinHandle;
			break;
		case 6:
			ppfnProc = (FARPROC *)&myInterface.m_pfnWaitOnConnection;
			break;
		case 7:
			ppfnProc = (FARPROC *)&myInterface.m_pfnReadRequest;
			break;
		case 8:
			ppfnProc = (FARPROC *)&myInterface.m_pfnParsePayLoad;
			break;
		case 9:
			ppfnProc = (FARPROC *)&myInterface.m_pfnIssueFilePointerRequest;
			break;
		default:
			break;
		}

		// get the procedure address
#ifdef _WIN32_WCE
		*ppfnProc  = GetProcAddress(m_hDll, pTable->lpName);
#else
		*ppfnProc  = GetProcAddressUnicode(m_hDll, pTable->lpName);
#endif
		fCompleted = (*ppfnProc) ? fCompleted : FALSE;
		NHPOS_ASSERT(*ppfnProc);
	}

	if (! fCompleted)
	{
		AfxFreeLibrary(m_hDll);
		m_hDll = NULL;

		return FALSE;						// exit ...
	}

	// exit ...
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CServerProc message handlers
