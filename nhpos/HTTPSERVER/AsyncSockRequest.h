#if !defined(AFX_ASYNCSOCKREQUEST_H__F028071D_A2CD_4737_8ACB_8F6791FA6180__INCLUDED_)
#define AFX_ASYNCSOCKREQUEST_H__F028071D_A2CD_4737_8ACB_8F6791FA6180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsyncSockRequest.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CAsyncSockRequest command target

class CAsyncSockRequest : public CAsyncSocket
{
// Attributes
public:
	BOOL   m_bKilled;

// Operations
public:
	CAsyncSockRequest();
	virtual ~CAsyncSockRequest();
	int GetBytes (BYTE *pLineOfText, int nSize);
	int PutBytes (BYTE *pLineOfText, int nBytes);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsyncSockRequest)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAsyncSockRequest)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	BYTE   *m_cbaInputBuffer;
	int     m_nInputBufferCount;
	int     m_nInputBufferSize;
	BYTE   *m_cbaOutputBuffer;
	int     m_nOutputBufferCount;
	int     m_nOutputBufferSize;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASYNCSOCKREQUEST_H__F028071D_A2CD_4737_8ACB_8F6791FA6180__INCLUDED_)
