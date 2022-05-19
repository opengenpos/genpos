// AsyncSockListen.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncSockListen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsyncSockListen

CAsyncSockListen::CAsyncSockListen() : m_pCSocket(0)
{
}

CAsyncSockListen::~CAsyncSockListen()
{
	if (m_pCSocket) {
		m_pCSocket->ShutDown (SD_BOTH);
		m_pCSocket->Close ();
		m_pCSocket = 0;
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAsyncSockListen, CAsyncSocket)
	//{{AFX_MSG_MAP(CAsyncSockListen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAsyncSockListen member functions

void CAsyncSockListen::OnAccept(int nErrorCode) 
{
	if ( Accept(casInUse ) )
	{
		casInUse.AsyncSelect( FD_READ | FD_WRITE | FD_CLOSE );
	}
}

void CAsyncSockListen::SocketSet (CAsyncSockListen *pCSocket)
{
	m_pCSocket = pCSocket;
}
