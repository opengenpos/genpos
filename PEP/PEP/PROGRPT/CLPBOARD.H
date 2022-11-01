/////////////////////////////////////////////////////////////////////////////
// clpboard.h : header file
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

#ifndef _CLPBOARD_H_
#define _CLPBOARD_H_

class CClipboardFile : private CFile
{
private:
	CWnd*		m_pOwnerWnd;			//
	UINT		m_nClipboardFormat;		// format
	UINT		m_nOpenFlags;			// mode of open function
	BOOL		m_bOpenClipboard;		// open clipboard
	BOOL		m_bEmptyClipboard;		// empty clipboard

	HANDLE		m_hBuffer;				// handle of global memory
	DWORD		m_dwBufSize;			// size of global memory
	BYTE FAR*	m_lpBufCur;
	BYTE FAR*	m_lpBufTop;

public:
	CClipboardFile(CWnd* pWnd);
	~CClipboardFile();

	operator CFile*() { return (CFile*) this; };

	BOOL Open(UINT nClipboardFormat, UINT nOpenFlags, DWORD dwBufSize = 512);
	UINT Read(void FAR* lpBuf, UINT nCount);
	void Write(const void FAR* lpBuf, UINT nCount);
	void WriteHandle(HANDLE hHandle);
	void Flush();
	void Close();
	LONG Seek(LONG lOff, UINT nFrom);
};

#endif
