// ScrollReport.cpp : implementation file
//

#include "stdafx.h"
#include "ScrollReport.h"

IMPLEMENT_DYNAMIC(CScrollReportWnd, CWnd)


CScrollReportWnd::CScrollReportWnd(CScrollDocument *pDocument /*= NULL*/)
: m_pDocument(pDocument)
{
}


CScrollReportWnd::~CScrollReportWnd()
{
}

BOOL CScrollReportWnd::InitDocumentView (CScrollDocument *pDocument /*= NULL*/)
{
	if (pDocument) {
		m_pDocument = pDocument;
	}

	ASSERT(m_pDocument);
	if (m_pDocument == 0) {
		return FALSE;
	}

	// Get the client area size of the dialog we are putting the
	// CScrollView into and pull the right edge in sufficient to
	// clear buttons on the right hand side of the dialog.
	RECT  rectSize;
	GetClientRect (&rectSize);
	rectSize.right -= 5;
	rectSize.left  += 5;

	// allocate and set up the view document context linking the view
	// to a particular document, in our case a CScrollDocument.
	CCreateContext pContext;
	pContext.m_pCurrentDoc = m_pDocument;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CScrollReport);

	// Use the approach from CFrameWnd::CreateView() to create a view and
	// link the view with the document.  We use the dynamic CreateObject()
	// functionality to create a CScrollReport view object.  We use the
	// standard child window id for the first view of an instance of the
	// MFC document/view architecture.  We are basing this on a copy of
	// the CFrameWnd::CreateView () method from the MFC source.
	int  nID = AFX_IDW_PANE_FIRST;
	CScrollReport *pView = (CScrollReport *)pContext.m_pNewViewClass->CreateObject();
	ASSERT(pView);
	ASSERT_KINDOF(CWnd, pView);

	if (pView) {
		if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), this, nID, &pContext)) {
			TRACE0("Warning: could not create view for dialog.\n");
			return FALSE;
		}
	} else {
		TRACE0("Warning: dynamic create of CScrollView for dialog failed.\n");
		return FALSE;
	}

	// Set an initial scroll size for the CScrollView which will be
	// modified in the OnDraw () later when presenting the actual view
	// and we have the complete document and can calculate the document's
	// scrollable size properly.
	CSize sizeTotal;
	sizeTotal.cx = rectSize.right;
	sizeTotal.cy = 1 * rectSize.bottom;
	pView->SetScrollSizes(MM_TEXT, sizeTotal);

	pView->ShowWindow(SW_NORMAL);

	/**
	* After a view is created, resize window area of the view to fit into the
	* parent window.  Since this is a CScrollView, set an initial size for the
	* size of the object being scrolled.
	*/
	pView->MoveWindow(&rectSize);
	return TRUE;
}


BEGIN_MESSAGE_MAP(CScrollReportWnd, CWnd)
END_MESSAGE_MAP()

//------------------------------------------------------

IMPLEMENT_DYNCREATE(CScrollDocument, CDocument)

CScrollDocument::CScrollDocument (): iLineCount(0)
{
	RECT  rectSize = {0, 0, 400, 23};

	rectLineSize = rectSize;
}

BEGIN_MESSAGE_MAP(CScrollDocument, CDocument)
END_MESSAGE_MAP()
//----------------------------------------------------------

// CScrollReport
IMPLEMENT_DYNCREATE(CScrollReport, CScrollView)

CScrollReport::CScrollReport() : iButtonDown(0)
{
}

CScrollReport::~CScrollReport()
{
}

int CScrollReport::AddTextLine (TCHAR *tcsLine)
{
	CScrollDocument* pDoc = (CScrollDocument* )GetDocument();

	if (pDoc) {
		pDoc->AddTextLine (tcsLine);
	}

	return 0;
}

BEGIN_MESSAGE_MAP(CScrollReport, CScrollView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CScrollReport::OnDraw(CDC* pDC)
{
	CScrollDocument* pDoc = (CScrollDocument* )GetDocument();

	RECT rect = pDoc->rectLineSize;

	sizeTotal.cx = rect.right;
	sizeTotal.cy = pDoc->GetLineCount() * rect.bottom;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CFont reportFont;

	LOGFONT lf;                        // Used to create the CFont.
	memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
	lf.lfHeight = 10;                  // Request a 20-pixel-high font
	wcscpy (lf.lfFaceName, L"Courier");    //    with face name "Arial".
	reportFont.CreateFontIndirect(&lf);    // Create the font.

	pDC->SelectObject (&reportFont);

	int i = 0;
	wchar_t  *pText = pDoc->GetTextLine (i);
	for (i = 0; pText; i++) {
		pText = pDoc->GetTextLine (i);
		pDC->DrawText(pText, -1, &rect, DT_SINGLELINE  | DT_LEFT | DT_VCENTER);
		rect.top += pDoc->rectLineSize.bottom;
		rect.bottom += pDoc->rectLineSize.bottom;
	}
}

afx_msg void CScrollReport::OnLButtonDown(UINT nFlags, CPoint point)
{
	pointMouseMove = pointButtonDown = point;
	iButtonDown = 1;
}

afx_msg void CScrollReport::OnLButtonUp(UINT nFlags, CPoint point)
{
	pointMouseMove = pointButtonUp = point;
	iButtonDown = 0;
}

afx_msg void CScrollReport::OnMouseMove(UINT nFlags, CPoint point)
{
	if (iButtonDown) {
		int yDiff = point.y - pointMouseMove.y;

		if (abs(yDiff) > 20) {
			CPoint curPos = GetScrollPosition  ();
			yDiff = ((yDiff * 10) + yDiff * 5) / 10;
			if (yDiff < 0) {
				curPos.y -= yDiff;
				if (curPos.y + 200 < sizeTotal.cy) {
					ScrollToPosition (curPos);
				}
			} else {
				curPos.y -= yDiff;
				if (curPos.y < 0) curPos.y = 0;
				ScrollToPosition (curPos);
			}
			pointMouseMove = point;
		}
	}
}
