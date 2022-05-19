// ConnEngineMfc.h : main header file for the CONNENGINEMFC DLL
//

#if !defined(AFX_CONNENGINEMFC_H__76090A8D_4AE3_49ED_9F0C_D29B99F1E2F8__INCLUDED_)
#define AFX_CONNENGINEMFC_H__76090A8D_4AE3_49ED_9F0C_D29B99F1E2F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ConnEngineSocket.h"

/////////////////////////////////////////////////////////////////////////////
// CConnEngineMfcApp
// See ConnEngineMfc.cpp for the implementation of this class
//

class CConnEngineMfcApp : public CWinApp
{
public:
	CConnEngineMfcApp();
	~CConnEngineMfcApp();

	BOOL  StartEngineAsServer (int nPort, HWND hWinHandle = 0, UINT wReceiveMsgId = 0);
	BOOL  StopEngineAsServer ();
	BOOL  StartEngineAsClient (TCHAR *ptcsAddress, int nPort, HWND hWinHandle = 0, UINT wReceiveMsgId = 0, UINT wSocketCloseMsgId = 0);
	BOOL  StopEngineAsClient ();
	ULONG RetrieveStatus (ULONG *ulStateIndicators);

	CConnEngineListenSocket  *m_ListenSocket;
	CConnEngineSocket        *m_ConnectedSocket;

	HWND    m_hWinHandle;
	UINT    m_wReceiveMsgId;
	UINT    m_wSocketCloseMsgId;
	TCHAR * ((*m_pFnCallBack)(TCHAR *pMsg));

	ULONG   m_ulStateIndicators;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnEngineMfcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CConnEngineMfcApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNENGINEMFC_H__76090A8D_4AE3_49ED_9F0C_D29B99F1E2F8__INCLUDED_)
