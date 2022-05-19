#if !defined(AFX_SERVERTHREAD_H__F0C0697D_7516_4BA1_9219_F0D4C69F609A__INCLUDED_)
#define AFX_SERVERTHREAD_H__F0C0697D_7516_4BA1_9219_F0D4C69F609A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerThread.h : header file
//

#include "AsyncSockListen.h"
#include "AsyncSockRequest.h"


/////////////////////////////////////////////////////////////////////////////
// CServerThread thread

class CServerThread : public CWinThread
{
	DECLARE_DYNCREATE(CServerThread)
protected:
	CServerThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CAsyncSockListen  casAccept;
	CAsyncSockRequest casInUse;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CServerThread();


	// Generated message map functions
	//{{AFX_MSG(CServerThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERTHREAD_H__F0C0697D_7516_4BA1_9219_F0D4C69F609A__INCLUDED_)
