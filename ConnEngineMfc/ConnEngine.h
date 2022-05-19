// ConnEngine.h: interface for the CConnEngine class.
//
// This include file contains all of the necessary information
// that allows the use of the ConnEngineMfc dll which provides
// the transport layer for a connection between two applications
// running on the same PC.
//
// Anyone using the functionality of the dll should include this
// file into their application and then use the various interface
// functions provided.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNENGINE_H__E52F7304_9E8C_4D5E_8CBE_7E6F70C61A6A__INCLUDED_)
#define AFX_CONNENGINE_H__E52F7304_9E8C_4D5E_8CBE_7E6F70C61A6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CONNENGINE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CONNENGINE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CONNENGINE_EXPORTS
#define CONNENGINE_API __declspec(dllexport)
#else
#define CONNENGINE_API __declspec(dllimport)
#endif

#define CONNENGINE_STATUS_SERVERMODE       0x0000001  // Indicates operational in Server mode with listen
#define CONNENGINE_STATUS_CLIENTMODE       0x0000002  // Indicates operational in Client mode
#define CONNENGINE_STATUS_SOCK_CONNECTED   0x0000004  // Indicates Server accepted connection or Client is connected
#define CONNENGINE_STATUS_SOCK_CONN_ERROR  0x0000008  // Indicates error on last connect(), see m_dwLastError
#define CONNENGINE_STATUS_SOCK_SEND_ERROR  0x0000010  // Indicates error on last send(), see m_dwLastError
#define CONNENGINE_STATUS_SOCK_RECV_ERROR  0x0000020  // Indicates error on last Receive(), see m_dwLastError

#if defined(__cplusplus)
extern "C" {
#endif

// The type CONNENGINEHANDLE is for future expansion to allow multiple
// sockets to be managed by the dll.
typedef unsigned short CONNENGINEHANDLE;

// following functions are used with a server implementation.
// these functions will setup the parameters for the server and
// start the listen needed to accept a connection from a client
CONNENGINE_API int fnConnEngineSetDomainNamePort(CONNENGINEHANDLE hConnEngineSocket, char *aszDomainName, int nPortNo);
CONNENGINE_API int fnConnEngineStartEngine (int nPort, HWND hWinHandle, UINT wReceiveMsgId);
CONNENGINE_API int fnConnEngineStopEngine ();

// following functions are used with a client implementaton.
// these functions will issue the connect to a client and
// provide a way for a client to establish a communications pipe to a server.
CONNENGINE_API int fnConnEngineConnectTo (TCHAR *tcsAddress, int nPort, HWND hWinHandle, UINT wReceiveMsgId);
CONNENGINE_API int fnConnEngineConnectToExt (TCHAR *tcsAddress, int nPort, HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId);
CONNENGINE_API int fnConnEngineDisconnect (VOID);

// following functions are used by either a server or a client implementation.
CONNENGINE_API unsigned long fnConnEngineStartStatus (CONNENGINEHANDLE hConnEngineSocket);
CONNENGINE_API int fnConnEngineSetWindowHandle (CONNENGINEHANDLE hConnEngineSocket, HWND hWinHandle, UINT m_wReceiveMsgId);
CONNENGINE_API int fnConnEngineSetWindowHandleExt (CONNENGINEHANDLE hConnEngineSocket, HWND hWinHandle, UINT m_wReceiveMsgId, UINT m_wSocketCloseMsgId);
CONNENGINE_API int fnConnEngineSetCallBack(CONNENGINEHANDLE hConnEngineSocket, TCHAR * ((*pFnCallBack)(TCHAR *pMsg)));

CONNENGINE_API TCHAR * fnConnEngineRetrieveNextMessage (CONNENGINEHANDLE hConnEngineSocket, TCHAR *tcBuffer, LONG tcBufferSize);
CONNENGINE_API int     fnConnEngineSendMessage (CONNENGINEHANDLE hConnEngineSocket, LPCTSTR tcBuffer, LONG tcBufferSize);
CONNENGINE_API ULONG   fnConnEngineRetrieveStatus (CONNENGINEHANDLE hConnEngineSocket);

#if defined(__cplusplus)
};
#endif

#endif // !defined(AFX_CONNENGINE_H__E52F7304_9E8C_4D5E_8CBE_7E6F70C61A6A__INCLUDED_)
