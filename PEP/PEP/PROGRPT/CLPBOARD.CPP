/////////////////////////////////////////////////////////////////////////////
// clpboard.cpp : implementation file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "clpboard.h"

CClipboardFile::CClipboardFile(CWnd* pWnd)
{
	m_pOwnerWnd       = pWnd;
	m_bOpenClipboard  = m_pOwnerWnd->OpenClipboard();
	m_bEmptyClipboard = FALSE;
	m_hBuffer         = NULL;
	m_dwBufSize       = 0;
	m_lpBufTop        = NULL;
	m_lpBufCur        = NULL;
	m_nOpenFlags      = 0;
}

CClipboardFile::~CClipboardFile()
{
	::CloseClipboard();
}

BOOL CClipboardFile::Open(UINT nClipboardFormat, UINT nOpenFlags,
	DWORD dwBufSize /*=512*/)
{
	ASSERT_VALID(m_pOwnerWnd);

	if (! m_bOpenClipboard)
		return FALSE;

	m_nClipboardFormat = nClipboardFormat;
	m_nOpenFlags = nOpenFlags;
	m_dwBufSize = dwBufSize;

	if (m_nOpenFlags & modeWrite)
	{
		if (!m_bEmptyClipboard)
		{
			EmptyClipboard();
			m_bEmptyClipboard = TRUE;
		}
		m_hBuffer = ::GlobalAlloc(GHND, m_dwBufSize);
	}
	else								// modeRead
	{
		m_hBuffer = ::GetClipboardData(m_nClipboardFormat);
		m_dwBufSize = ::GlobalSize(m_hBuffer);
		if (m_hBuffer == NULL)
			return FALSE;
	}

	return TRUE;
}

UINT CClipboardFile::Read(void FAR* lpBuf, UINT nCount)
{
//	ASSERT(m_nOpenFlags & modeRead);

	if (m_lpBufCur == NULL)
		m_lpBufTop = m_lpBufCur = (BYTE FAR*)::GlobalLock(m_hBuffer);

	if (m_lpBufTop + m_dwBufSize < m_lpBufCur + nCount)
		nCount = (UINT)(m_lpBufCur - m_lpBufTop + (DWORD)nCount - m_dwBufSize);
	memcpy(lpBuf, m_lpBufCur, nCount);
	m_lpBufCur += nCount;

	return nCount;
}

void CClipboardFile::Write(const void FAR* lpBuf, UINT nCount)
{
	ASSERT(m_nOpenFlags & modeWrite);

	if (m_lpBufCur == NULL)
		m_lpBufTop = m_lpBufCur = (BYTE FAR*)::GlobalLock(m_hBuffer);

	if (m_lpBufTop + m_dwBufSize < m_lpBufCur + nCount)
	{
		DWORD grow = (((DWORD)(m_lpBufCur + (DWORD)nCount -
							  m_lpBufTop - m_dwBufSize - 1) >> 9) + 1) << 9;
		m_hBuffer = ::GlobalReAlloc(m_hBuffer,
									m_dwBufSize += grow,
									GMEM_MOVEABLE | GMEM_ZEROINIT);
	}
	memcpy(m_lpBufCur, lpBuf, nCount);
	m_lpBufCur += nCount;
}

void CClipboardFile::WriteHandle(HANDLE hHandle)
{
	if (m_hBuffer != NULL)
		::GlobalFree(m_hBuffer);

	m_hBuffer = hHandle;
}

void CClipboardFile::Flush()
{
	if (m_hBuffer == NULL)
		return;

	if (m_nOpenFlags & modeWrite)
	{
//		::GlobalUnlock(m_hBuffer);
		::SetClipboardData(m_nClipboardFormat, m_hBuffer);
		m_hBuffer = NULL;
	}
	else								 // modeRead
	{
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
}

void CClipboardFile::Close()
{
	if (m_hBuffer != NULL)
		Flush();
}

LONG CClipboardFile::Seek(LONG lOff, UINT nFrom)
{
	return 0L;
}
