// AsyncSockRequest.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncSockRequest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsyncSockRequest

CAsyncSockRequest::CAsyncSockRequest() : m_nInputBufferCount(0), m_nOutputBufferCount(0),
                                         m_nInputBufferSize(2048), m_nOutputBufferSize(2048)
{
	m_cbaInputBuffer = new BYTE [m_nInputBufferSize];
	m_cbaOutputBuffer = new BYTE [m_nOutputBufferSize];
}

CAsyncSockRequest::~CAsyncSockRequest()
{
	ShutDown (SD_BOTH);
	Close ();
	if (m_cbaInputBuffer) {
		delete [] m_cbaInputBuffer;
		m_cbaInputBuffer = 0;
	}
	if (m_cbaOutputBuffer) {
		delete [] m_cbaOutputBuffer;
		m_cbaOutputBuffer = 0;
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAsyncSockRequest, CAsyncSocket)
	//{{AFX_MSG_MAP(CAsyncSockRequest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAsyncSockRequest member functions

void CAsyncSockRequest::OnReceive(int nErrorCode) 
{
	if (m_nInputBufferCount < m_nInputBufferSize) {
		int nBytes = Receive( m_cbaInputBuffer + m_nInputBufferCount, m_nInputBufferSize - m_nInputBufferCount );
		if ( nBytes != SOCKET_ERROR )
		{
			m_nInputBufferCount += nBytes;
		}
	}
}

void CAsyncSockRequest::OnSend(int nErrorCode) 
{
	if (m_nOutputBufferCount) {
		int nBytes = Send( m_cbaOutputBuffer, m_nOutputBufferCount );
		if ( nBytes == SOCKET_ERROR )
		{
			if ( GetLastError() != WSAEWOULDBLOCK )
			{
				ShutDown( both );
				m_bKilled = TRUE;
				//Release();
			}
		}
		else if ( nBytes < m_nOutputBufferCount )
		{
			// still got some left....
			m_nOutputBufferCount -= nBytes;
			memmove (m_cbaOutputBuffer, m_cbaOutputBuffer + nBytes,  m_nOutputBufferCount);
			// set up for next write....
		}
		else {
			m_nOutputBufferCount = 0;
		}
	}
}

void CAsyncSockRequest::OnClose(int nErrorCode) 
{
	CAsyncSocket::OnClose(nErrorCode);
	ShutDown (SD_BOTH);
	Close ();
	if (m_cbaInputBuffer) {
		delete [] m_cbaInputBuffer;
		m_cbaInputBuffer = 0;
	}
	if (m_cbaOutputBuffer) {
		delete [] m_cbaOutputBuffer;
		m_cbaOutputBuffer = 0;
	}
}


int CAsyncSockRequest::GetBytes (BYTE *pLineOfText, int nSize)
{ 
	int nBytes = 0;
	if (m_nInputBufferCount > 0) {
		BYTE *pBuff = m_cbaInputBuffer;
		nBytes = m_nInputBufferCount;
		if (nBytes > nSize) {
			nBytes = nSize;
		}
		memcpy (pLineOfText, pBuff, nBytes);
		m_nInputBufferCount -= nBytes;
		if (m_nInputBufferCount > 0) {
			memmove (m_cbaInputBuffer, m_cbaInputBuffer + nBytes, m_nInputBufferCount);
		}
	}
	return nBytes;
}

int CAsyncSockRequest::PutBytes (BYTE *pLineOfText, int nBytes)
{
	if (m_nOutputBufferSize - m_nOutputBufferCount > nBytes) {
		BYTE * pBuff = m_cbaOutputBuffer + m_nOutputBufferCount;

		if (pLineOfText) {
			for (int i = 0; i < nBytes; i++) {
				pBuff[i] = pLineOfText[i];
			}
			m_nOutputBufferCount += nBytes;
			int nBytes = Send( m_cbaOutputBuffer, m_nOutputBufferCount );
			m_nOutputBufferCount -= nBytes;
			if ( nBytes == SOCKET_ERROR )
			{
				if ( GetLastError() != WSAEWOULDBLOCK )
				{
					ShutDown( both );
					m_bKilled = TRUE;
					//Release();
				}
			}
			else if ( nBytes < m_nOutputBufferCount )
			{
				// still got some left....
				m_nOutputBufferCount -= nBytes;
				memmove (m_cbaOutputBuffer, m_cbaOutputBuffer + nBytes, m_nOutputBufferCount);
				// set up for next write....
			}
			else {
				m_nOutputBufferCount = 0;
			}
		}
	}
	return 0;
}