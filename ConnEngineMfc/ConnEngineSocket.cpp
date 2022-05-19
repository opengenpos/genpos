// ConnEngineSocket.cpp : implementation file
//

#include "stdafx.h"
#include "string.h"
#include "ConnEngineMfc.h"
#include "ConnEngine.h"
#include "ConnEngineSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template< class Type >
 CHAR * FindStringInString (Type *pStringToFind, Type *pStringToSearch, Type **pStringToSearchLast)
{
	Type *pStringToFindSave = pStringToFind;

	for ( ; *pStringToSearch; pStringToSearch++) {
		if (*pStringToFind == *pStringToSearch) {
			Type *pStringToSearchSave = pStringToSearch;

			while (*pStringToSearch && *pStringToFind) {
				if (*pStringToSearch != *pStringToFind)
					break;
				pStringToSearch++; pStringToFind++;
			}

			if (*pStringToFind == 0) {
				// found a match so report it
				return (CHAR *)pStringToSearchSave;
			}
			pStringToFind = pStringToFindSave;
			pStringToSearch = pStringToSearchSave;
		}
	}

	*pStringToSearchLast = pStringToSearch - 30;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CConnEngineListenSocket member functions

void CConnEngineListenSocket::OnAccept(int nErrorCode) 
{
	if (nErrorCode == 0 && (m_ConnectedSocket.m_ulStateIndicators & CONNENGINE_STATUS_SOCK_CONNECTED) == 0) {
		m_ConnectedSocket.ResetSocketControl();
		m_ConnectedSocket.SetCallBack (m_pFnCallBack);
		m_ConnectedSocket.SetWindowHandleExt (m_hWinHandle, m_wReceiveMsgId, m_wSocketCloseMsgId);
		m_ConnectedSocket.m_theApp = this->m_theApp;
		m_ConnectedSockAddrLen = sizeof(m_ConnectedSockAddr);
		m_iConnectedSocketInUse = Accept (m_ConnectedSocket, &m_ConnectedSockAddr, &m_ConnectedSockAddrLen);
		if (! m_iConnectedSocketInUse) {
			TRACE3("%S(%d): CConnEngineListenSocket::OnAccept(). GetLastError = %d\n", __FILE__, __LINE__, GetLastError());
		}
		else {
			m_ConnectedSocket.m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONNECTED;
		}
	}
	else if (nErrorCode == 0) {
		// Since we only allow one connected socket, if we get any other requests
		// we want to do the accept and then close the socket.
		// by doing this we ensure that the accept() queue stays empty.
		int          tempSockAddrLen;
		CAsyncSocket tempSock;
		SOCKADDR     tempSockAddr;

		tempSockAddrLen = sizeof(tempSockAddr);
		if (Accept (tempSock, &tempSockAddr, &tempSockAddrLen)) {
			tempSock.Close ();
		}
		TRACE2("%S(%d): CConnEngineListenSocket::OnAccept().  Already in use.\n", __FILE__, __LINE__);
	}
	else {
		TRACE3("%S(%d): CConnEngineListenSocket::OnAccept().  nErrorCode = %d.\n", __FILE__, __LINE__, nErrorCode);
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

CConnEngineListenSocket::CConnEngineListenSocket()
{
	m_iConnectedSocketInUse = false;
	m_theApp = 0;
	m_pFnCallBack= 0;
	m_hWinHandle = 0;
	m_wReceiveMsgId = 0;
	m_wSocketCloseMsgId = 0;
}

CConnEngineListenSocket::~CConnEngineListenSocket()
{
	// nothing to do here as the underlying CAsyncSocket destructor
	// will do everything for us.
	//
	// we are seeing an ASSERT() fail within the CAsyncSocket destructor
	// with the Close() returning a SOCKET_ERROR value.
	// however we have been unable to determine the root cause.
}

void CConnEngineListenSocket::SetWindowHandle (HWND hWinHandle, UINT wReceiveMsgId)
{
	m_hWinHandle = hWinHandle;
	m_wReceiveMsgId = wReceiveMsgId;
	m_wSocketCloseMsgId = 0;

	m_ConnectedSocket.SetWindowHandle (hWinHandle, wReceiveMsgId);
}

void CConnEngineListenSocket::SetWindowHandleExt (HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId)
{
	m_hWinHandle = hWinHandle;
	m_wReceiveMsgId = wReceiveMsgId;
	m_wSocketCloseMsgId = wSocketCloseMsgId;

	m_ConnectedSocket.SetWindowHandleExt (hWinHandle, wReceiveMsgId, wSocketCloseMsgId);
}


void CConnEngineListenSocket::SetCallBack (TCHAR * ((*pFnCallBack)(TCHAR *pMsg)))
{
	m_pFnCallBack = pFnCallBack;
	m_ConnectedSocket.SetCallBack (pFnCallBack);
}

int CConnEngineListenSocket::SockSendText (LPCTSTR pText, int nChars)
{
	int iRetStatus = 0;

	if ((m_ConnectedSocket.m_ulStateIndicators & CONNENGINE_STATUS_SOCK_CONNECTED) != 0) {
		m_ConnectedSocket.SockSendText (pText, nChars);
		iRetStatus = 1;
	}
	return iRetStatus;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CConnEngineListenSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CConnEngineListenSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

#if 0
static void LogMessage (char *aszMsg)
{
	FILE *hFile = fopen ("C:\\socklogfile.txt", "a+");

	if (hFile) {
		fprintf (hFile, "%s", aszMsg);
		fclose (hFile);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnEngineSocket

CConnEngineSocket::CConnEngineSocket()
{
	m_pMsgBufferPointers[0] = m_MsgBuffer_01;
	m_pMsgBufferPointers[1] = m_MsgBuffer_02;
	m_pMsgBufferPointers[2] = m_MsgBuffer_03;
	m_nMsgBufferIndex = 0;

	m_MsgBufferTemp[0] = 0;
	m_nMsgBuffer_Offset = 0;
	m_pMsgStart = 0;
	m_pMsgStop = 0;
	m_dwLastError = 0;

	m_usMsgBufferTempSendIndex = 0;
	m_usMsgBufferTempSendCurrent = 0;

	for (int iIndex = 0; iIndex < sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]); iIndex++) {
		m_MsgBufferTempSendList[iIndex].nBytesLength = m_MsgBufferTempSendList[iIndex].nBytesSent = 0;
	}

	m_CharSize = UcharString;

	m_hWinHandle = 0;
	m_wReceiveMsgId = 0;
	m_pFnCallBack = 0;
	m_theApp = 0;

	m_ulStateIndicators = 0;
}

void CConnEngineSocket::ResetSocketControl (void)
{
	m_nMsgBufferIndex = 0;
	m_MsgBufferTemp[0] = 0;
	m_nMsgBuffer_Offset = 0;
	m_pMsgStart = 0;
	m_pMsgStop = 0;
	m_dwLastError = 0;

	m_usMsgBufferTempSendIndex = 0;
	m_usMsgBufferTempSendCurrent = 0;

	for (int iIndex = 0; iIndex < sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]); iIndex++) {
		m_MsgBufferTempSendList[iIndex].nBytesLength = m_MsgBufferTempSendList[iIndex].nBytesSent = 0;
	}

	m_ulStateIndicators &= (CONNENGINE_STATUS_SERVERMODE | CONNENGINE_STATUS_CLIENTMODE);  // clear indicators keeping operating mode.
}

CConnEngineSocket::~CConnEngineSocket()
{
	// nothing much to do here as the underlying CAsyncSocket destructor
	// will do everything for us. however we do want to take care of our
	// own status housekeeping.
	//
	// we are seeing an ASSERT() fail within the CAsyncSocket destructor
	// with the Close() returning a SOCKET_ERROR value.
	// however we have been unable to determine the root cause.

	m_ulStateIndicators &= ~CONNENGINE_STATUS_SOCK_CONNECTED;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CConnEngineSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CConnEngineSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CConnEngineSocket member functions

void CConnEngineSocket::SetWindowHandle (HWND hWinHandle, UINT wReceiveMsgId)
{
	m_hWinHandle = hWinHandle;
	m_wReceiveMsgId = wReceiveMsgId;
	m_wSocketCloseMsgId = 0;
}

void CConnEngineSocket::SetWindowHandleExt (HWND hWinHandle, UINT wReceiveMsgId, UINT wSocketCloseMsgId)
{
	m_hWinHandle = hWinHandle;
	m_wReceiveMsgId = wReceiveMsgId;
	m_wSocketCloseMsgId = wSocketCloseMsgId;
}

void CConnEngineSocket::SetCallBack (TCHAR * ((*pFnCallBack)(TCHAR *pMsg)))
{
	m_pFnCallBack = pFnCallBack;
}

int CConnEngineSocket::SendText (SHORT sMsgBufferIndex, int nBytesLength)
{
	int    iRetStatus;
	DWORD  dwLastError = 0;

	if (sMsgBufferIndex >= 0 && nBytesLength > 0) {
		m_MsgBufferTempSendList[sMsgBufferIndex].nBytesSent = 0;
		m_MsgBufferTempSendList[sMsgBufferIndex].nBytesLength = nBytesLength;
	}

	while (m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength > 0) {
		iRetStatus = Send (m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].MsgBufferTempSend + m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesSent, m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength);
		if (iRetStatus != m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength) {
//			char  xBuff[128];
			dwLastError = GetLastError ();
//			sprintf (xBuff, "Send(): iRetStatus = %d, nBytesLength = %d, dwLastError = %d\n", iRetStatus, nBytesLength, dwLastError);
//			LogMessage (xBuff);
			if (SOCKET_ERROR == iRetStatus && dwLastError != WSAEWOULDBLOCK) {
				m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_SEND_ERROR;
				m_dwLastError= dwLastError;
				m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesSent = m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength = 0;
				m_usMsgBufferTempSendCurrent++;    // Toggle the sending buffer we are using
				if (m_usMsgBufferTempSendCurrent >= sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]))
					m_usMsgBufferTempSendCurrent = 0;
			} else {
				if (dwLastError == WSAEWOULDBLOCK) {
					iRetStatus = 0;
				}
				m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesSent += iRetStatus;
				m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength -= iRetStatus;
				if (m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength <= 0) {
					m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesSent = m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength = 0;
					m_usMsgBufferTempSendCurrent++;    // Toggle the sending buffer we are using
					if (m_usMsgBufferTempSendCurrent >= sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]))
						m_usMsgBufferTempSendCurrent = 0;
				}
				if (dwLastError == WSAEWOULDBLOCK) {
					break;
				}
			}
		} else {
			m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesSent = m_MsgBufferTempSendList[m_usMsgBufferTempSendCurrent].nBytesLength = 0;
			m_usMsgBufferTempSendCurrent++;    // Toggle the sending buffer we are using
			if (m_usMsgBufferTempSendCurrent >= sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]))
				m_usMsgBufferTempSendCurrent = 0;
		}
	}

	return 0;
}

int CConnEngineSocket::SockSendText (LPCTSTR pText, int nChars)
{
	int  nBytesLength = 0;
	UCHAR  *pSendBufferSave;

	if (m_CharSize == UcharString) {
		UCHAR  *pSendBuffer = 0;

		pSendBuffer = pSendBufferSave = m_MsgBufferTempSendList[m_usMsgBufferTempSendIndex].MsgBufferTempSend;     // Save the starting address of whichever buffer to allow us to figure number of characters below

		strcpy ((CHAR *)pSendBuffer, "<Start>");
		pSendBuffer += strlen((CHAR *)pSendBuffer);
		int    nCharCount = nChars;
		for ( ; nCharCount > 0; nCharCount--) {
			*pSendBuffer++ = (UCHAR)*pText++;
		}
		strcpy ((CHAR *)pSendBuffer, "</Start>");
		pSendBuffer += strlen((CHAR *)pSendBuffer);
		nBytesLength = pSendBuffer - pSendBufferSave;
	}
	else {
		WCHAR  *pSendBuffer = 0;

		pSendBuffer = (WCHAR  *)m_MsgBufferTempSendList[m_usMsgBufferTempSendIndex].MsgBufferTempSend;
		pSendBufferSave = (UCHAR *)pSendBuffer  ;     // Save the starting address of whichever buffer to allow us to figure number of characters below

		wcscpy ((WCHAR *)pSendBuffer, L"<Start>");
		pSendBuffer += wcslen((WCHAR *)pSendBuffer);
		int    nCharCount = nChars;
		for ( ; nCharCount > 0; nCharCount--) {
			*pSendBuffer++ = *pText++;
		}
		wcscpy (pSendBuffer, L"</Start>");
		pSendBuffer += wcslen((WCHAR *)pSendBuffer);
		nBytesLength = (UCHAR *)pSendBuffer - pSendBufferSave;
	}

	SendText (m_usMsgBufferTempSendIndex, nBytesLength);

	m_usMsgBufferTempSendIndex++;    // Toggle the sending buffer we are using
	if (m_usMsgBufferTempSendIndex >= sizeof(m_MsgBufferTempSendList)/sizeof(m_MsgBufferTempSendList[0]))
		m_usMsgBufferTempSendIndex = 0;

	return 1;
}

static CHAR *aszReceiveErrorMsg = "<Start><CONNENGINE>\r<lErrorCode>-1</lErrorCode>\r<aszErrorText>Error parsing incoming message.</aszErrorText>\r<nSockRead>%d</nSockRead>\r<aszNearText>%s</aszNearText>\r</CONNENGINE>\r</Start>";
static WCHAR *tszReceiveErrorMsg = L"<Start><CONNENGINE>\r<lErrorCode>-1</lErrorCode>\r<aszErrorText>Error parsing incoming message.</aszErrorText>\r<nSockRead>%d</nSockRead>\r<aszNearText>%s</aszNearText>\r</CONNENGINE>\r</Start>";

void CConnEngineSocket::OnReceive(int nErrorCode) 
{
	static int i=0;

	WCHAR  *pWcharMsgTextTags[] = {
			L"<Start>",
			L"</Start>"
		};

	CHAR  *pCharMsgTextTags[] = {
			"<Start>",
			"</Start>"
		};

	i++;

	int    nRead = 0;
	DWORD  dwLastError = 0;

	nRead = Receive(m_MsgBufferTemp + m_nMsgBuffer_Offset, sizeof(m_MsgBufferTemp) - m_nMsgBuffer_Offset); 
	dwLastError = GetLastError ();

	switch (nRead)
	{
		case 0:
			Close();
			break;

		case SOCKET_ERROR:
			if (GetLastError() != WSAEWOULDBLOCK) 
			{
				TCHAR   xBuffer[128];
				m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_RECV_ERROR;
				m_dwLastError= dwLastError;
				_swprintf (xBuffer, _T("CConnEngineSocket::OnReceive(): Error occurred.  GetLastError = %d"), m_dwLastError);
				AfxMessageBox (xBuffer);
				Close();
			}
			break;

		default:
		do {
			if (m_CharSize == UcharString) {
				// If we are expecting standard ASCII string data then we will
				// use a CHAR type of pointer for everything.  However since
				// our final output is to be UNICODE we will do a conversion from
				// ASCII characters to UNICODE characters
				int  nMsgBufferStart = 0;
				CHAR *pMsgBufferTemp = (CHAR *)m_MsgBufferTemp;
				m_pcMsgBufferLastSearch = 0;

				m_nMsgBuffer_Offset += (nRead / sizeof(CHAR));
				pMsgBufferTemp[m_nMsgBuffer_Offset] = 0;   //terminate the string
				pMsgBufferTemp[m_nMsgBuffer_Offset+1] = 0; //terminate the message with empty string

				if (m_pMsgStart == 0) {
					m_pMsgStart = FindStringInString (pCharMsgTextTags[0], pMsgBufferTemp + nMsgBufferStart, &m_pcMsgBufferLastSearch);
				}

				if (m_pMsgStop == 0) {
					m_pMsgStop = FindStringInString (pCharMsgTextTags[1], pMsgBufferTemp + nMsgBufferStart, &m_pcMsgBufferLastSearch);
				}
#if 0
				if (m_pMsgStart == 0 || m_pMsgStop == 0) {
					*(m_pcMsgBufferLastSearch + 30) = 0;  // ensure there is an end of string terminator in the near the text.
					for (int iIndex = 0; i < 30 && m_pcMsgBufferLastSearch[iIndex]; iIndex++) {
						if (m_pcMsgBufferLastSearch[iIndex] == '<')
							m_pcMsgBufferLastSearch[iIndex] = '{';
						if (m_pcMsgBufferLastSearch[iIndex] == '>')
							m_pcMsgBufferLastSearch[iIndex] = '}';
					}
					sprintf (pMsgBufferTemp, aszReceiveErrorMsg, nRead, m_pcMsgBufferLastSearch);
					m_nMsgBuffer_Offset = strlen (pMsgBufferTemp);  // find the end of the new string
					pMsgBufferTemp[m_nMsgBuffer_Offset] = 0;   //terminate the string
					pMsgBufferTemp[m_nMsgBuffer_Offset+1] = 0; //terminate the message with empty string
					m_pMsgStart = FindStringInString (pCharMsgTextTags[0], pMsgBufferTemp, &m_pcMsgBufferLastSearch);
					m_pMsgStop = FindStringInString (pCharMsgTextTags[1], pMsgBufferTemp, &m_pcMsgBufferLastSearch);
				}
#endif
				if (m_pMsgStart != 0 && m_pMsgStop != 0) {
					m_pMsgStart += strlen(pCharMsgTextTags[0]);
					*m_pMsgStop = 0;
					int iIndex = 0;
					while ((m_pMsgBufferPointers[m_nMsgBufferIndex][iIndex++] = *m_pMsgStart++) != 0) ;

					if (m_hWinHandle != 0 && m_wReceiveMsgId != 0) {
						int i = PostMessage (m_hWinHandle, m_wReceiveMsgId, (WPARAM)m_pMsgBufferPointers[m_nMsgBufferIndex], 0);
						if (i == 0) {
			TRACE3("%S(%d): CConnEngineSocket::OnReceive(). GetLastError = %d\n", __FILE__, __LINE__, GetLastError());
						}
					}

					if (m_pFnCallBack != 0)
						m_pFnCallBack(m_pMsgBufferPointers[m_nMsgBufferIndex]);

					m_nMsgBufferIndex = ((m_nMsgBufferIndex + 1) % 3);

					m_pMsgStop += strlen(pCharMsgTextTags[1]);
					if (m_pMsgStop < pMsgBufferTemp + m_nMsgBuffer_Offset) {
						memmove (pMsgBufferTemp, m_pMsgStop, (char *)(pMsgBufferTemp + m_nMsgBuffer_Offset) - (char *)m_pMsgStop);
						m_nMsgBuffer_Offset = (pMsgBufferTemp + m_nMsgBuffer_Offset) - m_pMsgStop;
						pMsgBufferTemp[m_nMsgBuffer_Offset] = 0;   //terminate the string
						pMsgBufferTemp[m_nMsgBuffer_Offset+1] = 0; //terminate the message with empty string
					}
					else {
						m_nMsgBuffer_Offset = 0;
					}
					m_pMsgStart = 0;
					m_pMsgStop = 0;
					nRead = 0;
				} else {
					break;
				}
			}
			else {
				int  nMsgBufferStart = 0;
				WCHAR *pMsgBufferTemp = (WCHAR *)m_MsgBufferTemp;

				m_nMsgBuffer_Offset += (nRead / sizeof(WCHAR));
				pMsgBufferTemp[m_nMsgBuffer_Offset] = 0; //terminate the string
				m_pwMsgBufferLastSearch = 0;

				if (m_pMsgStart == 0) {
					m_pMsgStart = FindStringInString (pWcharMsgTextTags[0], pMsgBufferTemp + nMsgBufferStart, &m_pwMsgBufferLastSearch);
				}

				if (m_pMsgStop == 0) {
					m_pMsgStop = FindStringInString (pWcharMsgTextTags[1], pMsgBufferTemp + nMsgBufferStart, &m_pwMsgBufferLastSearch);
				}

#if 0
				if (m_pMsgStart == 0 || m_pMsgStop == 0) {
					wcscpy (pMsgBufferTemp, tszReceiveErrorMsg);    // put our error string into the buffer
					m_nMsgBuffer_Offset = wcslen (pMsgBufferTemp);  // find the end of the new string
					pMsgBufferTemp[m_nMsgBuffer_Offset] = 0;   //terminate the string
					pMsgBufferTemp[m_nMsgBuffer_Offset+1] = 0; //terminate the message with empty string
					m_pMsgStart = FindStringInString (pWcharMsgTextTags[0], pMsgBufferTemp, &m_pwMsgBufferLastSearch);
					m_pMsgStop = FindStringInString (pWcharMsgTextTags[1], pMsgBufferTemp, &m_pwMsgBufferLastSearch);
				}
#endif
				if (m_pMsgStart != 0 && m_pMsgStop != 0) {
					WCHAR  *pMsgStart = (WCHAR *)m_pMsgStart;
					WCHAR  *pMsgStop = (WCHAR *)m_pMsgStop;

					pMsgStart += wcslen(pWcharMsgTextTags[0]);
					*pMsgStop = 0;
					int iIndex = 0;
					while ((m_pMsgBufferPointers[m_nMsgBufferIndex][iIndex++] = *pMsgStart++) != 0) ;

					if (m_hWinHandle != 0 && m_wReceiveMsgId != 0) {
						int i = ::PostMessage (m_hWinHandle, m_wReceiveMsgId, (WPARAM)m_pMsgBufferPointers[m_nMsgBufferIndex], 0);
						if (i == 0) {
			TRACE3("%S(%d): CConnEngineSocket::OnReceive(). GetLastError = %d\n", __FILE__, __LINE__, GetLastError());
						}
					}

					if (m_pFnCallBack != 0)
						m_pFnCallBack(m_pMsgBufferPointers[m_nMsgBufferIndex]);

					m_nMsgBufferIndex = ((m_nMsgBufferIndex + 1) % 3);

					pMsgStop += wcslen(pWcharMsgTextTags[1]);
					if (pMsgStop < pMsgBufferTemp + m_nMsgBuffer_Offset) {
						memmove (pMsgBufferTemp, pMsgStop, (char *)(pMsgBufferTemp + m_nMsgBuffer_Offset) - (char *)pMsgStop);
						m_nMsgBuffer_Offset = (pMsgBufferTemp + m_nMsgBuffer_Offset) - pMsgStop;
						pMsgBufferTemp[m_nMsgBuffer_Offset] = 0;   //terminate the string
						pMsgBufferTemp[m_nMsgBuffer_Offset+1] = 0; //terminate the message with empty string
					}
					else {
						m_nMsgBuffer_Offset = 0;
					}

					m_pMsgStart = 0;
					m_pMsgStop = 0;
					nRead = 0;
				} else {
					break;
				}
			}
		} while (1);
		break;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void CConnEngineSocket::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
//	char xBuff[128];

//	sprintf (xBuff, "OnSend (): nErrorCode = %d\n", nErrorCode);
//	LogMessage (xBuff);

	SendText (-1, 0);

	CAsyncSocket::OnSend(nErrorCode);
}

// When a socket closes for some reason, this event handling method is called.
void CConnEngineSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	Close();
	ResetSocketControl();
	m_ulStateIndicators &= ~CONNENGINE_STATUS_SOCK_CONNECTED;

	CAsyncSocket::OnClose(nErrorCode);

	TRACE3("%S(%d): CConnEngineSocket::OnClose(). nErrorCode = %d\n", __FILE__, __LINE__, nErrorCode);

	if (m_hWinHandle != 0 && m_wSocketCloseMsgId != 0) {
		int i = ::PostMessage (m_hWinHandle, m_wSocketCloseMsgId, (WPARAM)0, (LPARAM)nErrorCode);
		if (i == 0) {
			TRACE3("%S(%d): CConnEngineSocket::OnClose(). GetLastError = %d\n", __FILE__, __LINE__, GetLastError());
		}
	}
}

// When a connect() call is made that returns a GetLastError() of WSAEWOULDBLOCK (10035)
// then when the connect actually happens, this event handling method is called.
void CConnEngineSocket::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (nErrorCode == 0) {
		BOOL  bTcpNoDelay = 0;

		m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONNECTED;     // now connected so indicate we are.
		m_ulStateIndicators &= ~CONNENGINE_STATUS_SOCK_CONN_ERROR;   // now connected so clear connection error if it is set.
		m_dwLastError = 0;
		SetSockOpt (TCP_NODELAY, &bTcpNoDelay, sizeof(bTcpNoDelay), IPPROTO_TCP);
	} else {
		m_ulStateIndicators |= CONNENGINE_STATUS_SOCK_CONN_ERROR;    // set connection error.
		m_dwLastError = nErrorCode;
	}
	CAsyncSocket::OnConnect(nErrorCode);
}
