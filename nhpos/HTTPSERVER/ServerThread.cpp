// ServerThread.cpp : implementation file
//

#include "stdafx.h"
#include "ServerThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerThread

IMPLEMENT_DYNCREATE(CServerThread, CWinThread)

CServerThread::CServerThread()
{
}

CServerThread::~CServerThread()
{
}

BOOL CServerThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	BOOL bJJ = AfxSocketInit ();

	ASSERT(bJJ);

	casAccept.Create (8080);
	casAccept.Listen ();

	return TRUE;
}

int CServerThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CServerThread, CWinThread)
	//{{AFX_MSG_MAP(CServerThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerThread message handlers
