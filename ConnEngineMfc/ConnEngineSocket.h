#if !defined(AFX_CONNENGINESOCKET_H__407999EC_0F43_4C89_B091_F7BABC580AA3__INCLUDED_)
#define AFX_CONNENGINESOCKET_H__407999EC_0F43_4C89_B091_F7BABC580AA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnEngineSocket.h : header file
//

class CConnEngineMfcApp;


/////////////////////////////////////////////////////////////////////////////
// CConnEngineSocket command target

#define CCONNENGINESOCKET_MSGBUF_SIZE 257000

class CConnEngineSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CConnEngineSocket();
	virtual ~CConnEngineSocket();

	void SetWindowHandle (HWND hWinHandle, UINT wReceiveMsgId);
	void SetWindowHandleExt (HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId);
	void SetCallBack (TCHAR * ((*pFnCallBack)(TCHAR *pMsg)));
	int  SockSendText (LPCTSTR pText, int nChars);
	int  SendText (SHORT sMsgBufferIndex, int nChars);

	void ResetSocketControl (void);

	typedef enum {UcharString = 1, WcharString = 2} CharSize;

	CharSize m_CharSize;

	UCHAR  m_MsgBufferTemp[CCONNENGINESOCKET_MSGBUF_SIZE];
	int    m_nMsgBuffer_Offset;

	USHORT m_usMsgBufferTempSendIndex;
	USHORT m_usMsgBufferTempSendCurrent;

	struct {
		int    nBytesLength;
		int    nBytesSent;
		UCHAR  MsgBufferTempSend[CCONNENGINESOCKET_MSGBUF_SIZE];
	} m_MsgBufferTempSendList[4];

	CHAR  *m_pMsgStart;
	CHAR  *m_pMsgStop;

	TCHAR  m_MsgBuffer_01[CCONNENGINESOCKET_MSGBUF_SIZE];
	TCHAR  m_MsgBuffer_02[CCONNENGINESOCKET_MSGBUF_SIZE];
	TCHAR  m_MsgBuffer_03[CCONNENGINESOCKET_MSGBUF_SIZE];

	TCHAR  *m_pMsgBufferPointers[3];
	int    m_nMsgBufferIndex;

	HWND   m_hWinHandle;
	UINT   m_wReceiveMsgId;
	UINT   m_wSocketCloseMsgId;
	TCHAR * ((*m_pFnCallBack)(TCHAR *pMsg));

	ULONG   m_ulStateIndicators;
	DWORD   m_dwLastError;

	CConnEngineMfcApp *m_theApp;

	CHAR   *m_pcMsgBufferLastSearch;
	WCHAR  *m_pwMsgBufferLastSearch;
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnEngineSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CConnEngineSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

class CConnEngineListenSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CConnEngineListenSocket();
	virtual ~CConnEngineListenSocket();

	void SetWindowHandle (HWND hWinHandle, UINT wReceiveMsgId);
	void SetWindowHandleExt (HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId);
	void SetCallBack (TCHAR * ((*pFnCallBack)(TCHAR *pMsg)));

	int  SockSendText (LPCTSTR pText, int nChars);

	CConnEngineSocket  m_ConnectedSocket;
	int                m_iConnectedSocketInUse;
	SOCKADDR           m_ConnectedSockAddr;
	int                m_ConnectedSockAddrLen;

	HWND      m_hWinHandle;
	UINT      m_wReceiveMsgId;
	UINT      m_wSocketCloseMsgId;
	TCHAR * ((*m_pFnCallBack)(TCHAR *pMsg));

	CConnEngineMfcApp *m_theApp;

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnEngineListenSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CConnEngineListenSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNENGINESOCKET_H__407999EC_0F43_4C89_B091_F7BABC580AA3__INCLUDED_)
