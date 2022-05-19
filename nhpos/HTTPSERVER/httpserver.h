// httpserver.h : main header file for the HTTPSERVER DLL
//

#if !defined(AFX_HTTPSERVER_H__9EDB8CAE_4390_4BB8_B07A_AD5997B698E2__INCLUDED_)
#define AFX_HTTPSERVER_H__9EDB8CAE_4390_4BB8_B07A_AD5997B698E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <afxmt.h>

#include "ServerThread.h"

/////////////////////////////////////////////////////////////////////////////
// CHttpserverApp
// See httpserver.cpp for the implementation of this class
//
#ifdef cplusplus
extern "C" {
#endif
__declspec(dllexport) int  StartThread ();
__declspec(dllexport) int  StopThread ();
__declspec(dllexport) int  IssueRequest (BYTE *pUserBuffer, int nBufSize);
__declspec(dllexport) int  ProvideCallBack ();
__declspec(dllexport) int  ProvideWinHandle ();
__declspec(dllexport) int  WaitOnConnection ();
__declspec(dllexport) int  ReadRequest (ServerProcInterface *pMyProcInterface);
__declspec(dllexport) int  ParsePayLoadXML (ServerProcInterface *pMyProcInterface);
__declspec(dllexport) int  IssueFilePointerRequest (FILE *fpFile, SHORT  sContentType);

#ifdef cplusplus
};
#endif

class CHttpserverApp : public CWinApp
{
public:

	CHttpserverApp();

	CServerThread * myServer;
	CServerThread * myServer2;

	static CHttpserverApp * GetServerObject();

	// attributes for the class
	typedef enum {RequestUnknown = SERVERPROC_REQUEST_UNKNOWN, RequestGet = SERVERPROC_REQUEST_GET, RequestPost=SERVERPROC_REQUEST_POST, RequestPut=SERVERPROC_REQUEST_PUT} HttpRequestType;

	CSocket casInUse;

	HttpRequestType  m_RequestType;
	BYTE  *m_ucharBufferPayLoad;
	BYTE  m_ucharBuffer[4000], *m_ucharBufferHeaderLine[40];
	BYTE  *m_ucharBufferPostKeyword[40];
	BYTE  *m_ucharBufferPostValue[40];
	int   m_nBytesRead, m_nBytesCount;
	int   m_nHeaderCount;
	FILE  *fpFileStream;
	SHORT sContentType;
	TCHAR m_csGetFileName[128];
	CHAR  m_csGetContentType[128];

// Operations
	int csWaitOnConnection ();
	int csIssueRequest ();
	int csReadRequest ();
	BOOL InterpretXML(const TCHAR* tchXML, VOID* pXEPTResData, USHORT usSize, SHORT sType);

public:
	CSemaphore  *pceConnectSem;
	CSingleLock  *pcsSingleLock;
	CSocket      casAccept;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHttpserverApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHttpserverApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTTPSERVER_H__9EDB8CAE_4390_4BB8_B07A_AD5997B698E2__INCLUDED_)
