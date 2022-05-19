#pragma once

#include "resource.h"

class CScrollDocument : public CDocument
{
	DECLARE_DYNCREATE(CScrollDocument)

public:
	CScrollDocument ();

	void AddTextLine (LPCTSTR lpszTextLine) { if (iLineCount < 10000) { wcsncpy (&wcsDocument[iLineCount * 50], lpszTextLine, 49); iLineCount++; } }
	wchar_t *GetTextLine (int iPos) { if (iPos < iLineCount) return &wcsDocument[iPos * 50]; else return 0; }
	int      GetLineCount () { return iLineCount; }
public:
	RECT    rectLineSize;

protected:
	int     iLineCount;
	wchar_t wcsDocument[50*10000];

	DECLARE_MESSAGE_MAP()
};

class CScrollReport : public CScrollView
{
	DECLARE_DYNCREATE(CScrollReport)

public:
	CScrollReport();
	virtual ~CScrollReport();
	int AddTextLine (TCHAR *tcsLine);

protected:
	int    iButtonDown;
	CPoint pointButtonDown;
	CPoint pointMouseMove;
	CPoint pointButtonUp;
	CSize  sizeTotal;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	DECLARE_MESSAGE_MAP()
};


class CScrollReportWnd : public CWnd
{
	DECLARE_DYNAMIC(CScrollReportWnd)
public:
	CScrollReportWnd(CScrollDocument *pDocument = NULL);
	virtual ~CScrollReportWnd();
	BOOL InitDocumentView (CScrollDocument *pDocument = NULL);

	CScrollDocument *m_pDocument;
	CFont            m_Reportfont;

protected:
	DECLARE_MESSAGE_MAP()
};