#if !defined(AFX_ASYNCSOCKLISTEN_H__1A4D0767_05E8_4D1F_A615_8F40B2971A01__INCLUDED_)
#define AFX_ASYNCSOCKLISTEN_H__1A4D0767_05E8_4D1F_A615_8F40B2971A01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsyncSockListen.h : header file
//

#include "AsyncSockRequest.h"

/////////////////////////////////////////////////////////////////////////////
// CAsyncSockListen command target

class CAsyncSockListen : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CAsyncSockListen();
	virtual ~CAsyncSockListen();
	void SocketSet (CAsyncSockListen *pCSocket);

	CAsyncSockListen *m_pCSocket;
	CAsyncSockRequest casInUse;

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsyncSockListen)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAsyncSockListen)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASYNCSOCKLISTEN_H__1A4D0767_05E8_4D1F_A615_8F40B2971A01__INCLUDED_)
