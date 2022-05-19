#if !defined(AFX_SERVERPROC_H__313EEFB2_C863_4FFD_93EE_B03D751FDF36__INCLUDED_)
#define AFX_SERVERPROC_H__313EEFB2_C863_4FFD_93EE_B03D751FDF36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerProc.h : header file
//

#include "SCF.h"

#ifdef SERVERPROC_EXPORTS
#define SERVERPROC_API __declspec(dllexport)
#else
#define SERVERPROC_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif
	// This function should be the same as the definition of the 
	// header file (DeviceIO.h).
	// Because, this use not a static link but LoadLibrary() API.
	// We should be able to use WINAPI for these functions but
	// compiling in Release cause compiler errors.
typedef struct {
	unsigned char  *m_ucharBuffer;
	int             m_nBytesCount;
	int             m_nRequestType;
	unsigned char	*m_ucharBufferPayLoad;
	TCHAR           *m_tcharFileName;
	TCHAR           *m_tcharDtdFileName;
	TCHAR           *m_tcharReqBuffer;
	TCHAR           *m_tcharRespBuffer;
	int             m_nReqBufferSize;
	int             m_nRespBufferSize;
	unsigned char  **m_ucharBufferHeaderLine;
	unsigned char  **m_ucharBufferPostLineKeyword;
	unsigned char  **m_ucharBufferPostLineValue;
	int	( *m_pfnStartThread)();
	int	( *m_pfnStopThread)();
	int	( *m_pfnIssueRequest)();
	int	( *m_pfnProvideCallBack)();
	int	( *m_pfnProvideWinHandle)();
	int	( *m_pfnWaitOnConnection)();
	int	( *m_pfnReadRequest)();
	int	( *m_pfnParsePayLoad)();
	int	( *m_pfnIssueFilePointerRequest)();
} ServerProcInterface;

typedef struct {
	TCHAR *tcKeyword;
	TCHAR *tcValue;
} HttpXmlTableItem, *pHttpXmlTableItem;


#define  SERVERPROC_REQUEST_UNKNOWN   0
#define  SERVERPROC_REQUEST_GET       1
#define  SERVERPROC_REQUEST_PUT       2
#define  SERVERPROC_REQUEST_POST      3

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus

/////////////////////////////////////////////////////////////////////////////
// CServerProc thread

class CServerProc 
{
public:
	CServerProc(LPCTSTR lpszDllName = 0);           // protected constructor used by dynamic creation
	virtual ~CServerProc();

// Attributes
public:
	ServerProcInterface  myInterface;

// Operations
public:
	void SetDllName (LPCTSTR lpszDllName);
	BOOL LoadDllProcs(LPCTSTR lpszDllName = 0);

// Internal variables.
protected:
	CString			m_sDllName;				// DLL name
	HMODULE			m_hDll;					// DLL handle

};

#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERPROC_H__313EEFB2_C863_4FFD_93EE_B03D751FDF36__INCLUDED_)
