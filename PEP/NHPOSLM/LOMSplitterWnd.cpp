// LOMSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "newlayout.h"
#include "LOMSplitterWnd.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLOMSplitterWnd
CLOMSplitterWnd::CLOMSplitterWnd(int nLevel)
{
	m_pSubSplitterWnd[0] = 0;
	m_pSubSplitterWnd[1] = 0;
	m_cxSplitter     = 0;
	m_cySplitter     = 0;
	m_cxBorderShare  = 0;
	m_cyBorderShare  = 0;
	m_cxSplitterGap  = 1;
	m_cySplitterGap  = 1;
    m_bRulersVisible = TRUE;
	m_cxBorder = 1;
	m_cyBorder = 1;
}

CLOMSplitterWnd::~CLOMSplitterWnd()
{
	if (m_pSubSplitterWnd[0]) {
		delete m_pSubSplitterWnd[0];
		m_pSubSplitterWnd[0] = 0;
	}
	if (m_pSubSplitterWnd[1]) {
		delete m_pSubSplitterWnd[1];
		m_pSubSplitterWnd[1] = 0;
	}
}

BEGIN_MESSAGE_MAP(CLOMSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CLOMSplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLOMSplitterWnd::ShowRulers(BOOL bShow, BOOL bSave)
{
    int nSize       = bShow ? RULER_SIZE : 0;
    int nSizeBorder = bShow ? /*3*/1:1;

	SetRowInfo(0, nSize, 0);
	SetColumnInfo(0, nSize, 0);

    m_cxSplitterGap  = nSizeBorder;
	m_cySplitterGap  = nSizeBorder;
    m_bRulersVisible = bSave ? bShow : m_bRulersVisible;
    RecalcLayout();
}

void CLOMSplitterWnd::UpdateRulersInfo(stRULER_INFO stRulerInfo,int type)
{
	if(type == RT_VERTICAL){
		((CRulerView*)GetPane(0, 0))->UpdateRulersInfo(stRulerInfo);
	}
	else if(type == RT_HORIZONTAL){
		((CRulerView*)GetPane(0, 1))->UpdateRulersInfo(stRulerInfo);
	}
	else
		return;
	//((CRulerView*)GetPane(0, 1))->UpdateRulersInfo(stRulerInfo);
	//((CRulerView*)GetPane(1, 0))->UpdateRulersInfo(stRulerInfo);

    if (((int)(stRulerInfo.DocSize.cx*stRulerInfo.fZoomFactor) < stRulerInfo.DocSize.cx) ||
        ((int)(stRulerInfo.DocSize.cy*stRulerInfo.fZoomFactor) < stRulerInfo.DocSize.cy))
        ShowRulers(FALSE, FALSE);
    else if (m_bRulersVisible)
        ShowRulers(TRUE, FALSE);
}

CLOMSplitterWnd  *CLOMSplitterWnd::AddSubDivision(int nSide, CRuntimeClass *pView1,CRuntimeClass *pView2, CCreateContext* pContext, BOOL bVertical)
{
   ASSERT((nSide == 0) || (nSide == 1));
   ASSERT(m_pSubSplitterWnd[nSide] == NULL);

	int nRow, nCol;
	SideToRowCol(nSide,&nRow,&nCol);

   int nID = IdFromRowCol(nRow,nCol);
   m_pSubSplitterWnd[nSide] = new  CLOMSplitterWnd(m_nLevel+1);
   m_pSubSplitterWnd[nSide]->Create(this,pView1,pView2,pContext,bVertical,nID);
   return(m_pSubSplitterWnd[nSide]);
}

BOOL CLOMSplitterWnd::Create(CWnd* pParentWnd, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext, BOOL bVertical,int nID)
{
	int nRow, nCol;
   m_bVertical = bVertical;
	if (bVertical) {
		nRow = 1;
		nCol = 2;
	}
	else {
		nRow = 2;
		nCol = 1;
	}
	VERIFY(CreateStatic(pParentWnd,nRow,nCol,WS_CHILD|WS_VISIBLE|WS_BORDER,nID));
   if (pView1 != NULL) {
      VERIFY(CreateView(0,0,pView1,CSize(0,0),pContext));
   }
   if (pView2 != NULL) {
	   if (bVertical) {
		   VERIFY(CreateView(0,1,pView2,CSize(0,0),pContext));
	   }
	   else {
		   VERIFY(CreateView(1,0,pView2,CSize(0,0),pContext));
	   }
   }
	return(TRUE);
}

void CLOMSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
    //Lock Splitter
    //CSplitterWnd::OnLButtonDown(nFlags, point);
}

void CLOMSplitterWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    //Lock Splitter
	//CSplitterWnd::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CLOMSplitterWnd message handlers

/////////////////////////////////////////////////////////////////////////////
// CRulerView

IMPLEMENT_DYNCREATE(CRulerView, CView)

CRulerView::CRulerView()
{
	m_rulerType   = RT_HORIZONTAL;
	m_scrollPos   = 0;
	m_lastPos.SetPoint(-1, -1);
    m_fZoomFactor = ZOOM_FACTOR;
}

CRulerView::~CRulerView() {}

BEGIN_MESSAGE_MAP(CRulerView, CView)
	//{{AFX_MSG_MAP(CRulerView)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerView drawing
void CRulerView::UpdateRulersInfo(stRULER_INFO stRulerInfo)
{
    m_DocSize     = stRulerInfo.DocSize;
    m_fZoomFactor = stRulerInfo.fZoomFactor;
    m_scrollPos   = stRulerInfo.ScrollPos;

    if (stRulerInfo.uMessage == RW_POSITION) 
        DrawCursorPos(stRulerInfo.Pos);
    else if ((m_rulerType == RT_HORIZONTAL) && (stRulerInfo.uMessage == RW_HSCROLL) ||
             (m_rulerType == RT_VERTICAL) && (stRulerInfo.uMessage == RW_VSCROLL)) 
	{
        CDC* pDC = GetDC();
        OnDraw(pDC);
        ReleaseDC(pDC);
    }
    else
        Invalidate();
} 

void CRulerView::OnDraw(CDC* pDC)
{ 
	//static int idx = 0;
	//TRACE(_T("CRulerView::OnDraw %d\n"), idx++);
	m_lastPos.SetPoint(-1, -1);
    
	CChildFrame *m_pParent = (CChildFrame*)GetParentFrame();
	int oldMapMode = pDC->SetMapMode(MM_TEXT);

	CFont vFont, hFont;
	vFont.CreateFont(16, 0, 000, 000, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN, _T("Times New Roman"));
	hFont.CreateFont(16, 0, 000, 000, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN, _T("Times New Roman"));
	
    CFont* pOldFont  = pDC->SelectObject((m_rulerType==RT_HORIZONTAL) ? &hFont : &vFont);
	int oldTextAlign = pDC->SetTextAlign((m_rulerType==RT_HORIZONTAL) ? (TA_RIGHT|TA_TOP) : (TA_LEFT|TA_TOP));
	int oldBkMode    = pDC->SetBkMode(TRANSPARENT);

	CRect rulerRect;
	GetClientRect(&rulerRect);

	if (m_rulerType == RT_HORIZONTAL)
	{
		rulerRect.right = m_pParent->ResLoWidth;
    }
	else //(m_rulerType==RT_VERTICAL)
	{
		rulerRect.bottom = m_pParent->ResLoHeight;
	}

	pDC->FillSolidRect(rulerRect, RGB(255, 255, 255));
    DrawTicker(pDC, rulerRect, /*factor*/5, /*begin*/  0,  TRUE, /*thickness*/1);
    DrawTicker(pDC, rulerRect, /*factor*/1, /*begin*/ 12, FALSE, /*thickness*/1);

	// restore DC settings
	pDC->SetMapMode(oldMapMode);
    pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(oldTextAlign);
	pDC->SetBkMode(oldBkMode);
}

// DrawTicker - Draw ruler and place tick marks.
//
// nFactor		The number of grids between tick marks.  In other words,
//				if nFactor is 5, then we put a tick mark every 5 ticks.
// nBegin		The number of pixels down from the top of the ruler,
//				indicating where to start drawing the tick mark.
//
void CRulerView::DrawTicker(CDC* pDC, CRect rulerRect, int nFactor, int nBegin, BOOL bShowPos, int nThickness)
{
    int nSize = (m_rulerType == RT_HORIZONTAL) ? rulerRect.right : rulerRect.bottom;

	// Testing if the resolution size can be divided by STD_GRID_WIDTH without
	// a remainder. If there is a remainder, nSize needs to have 1 (column/row)
	// added to make up for the lost decimals. - CSMALL
	if (nSize % STD_GRID_WIDTH)
		nSize = (nSize/STD_GRID_WIDTH) + 1;
	else
		nSize = (nSize/STD_GRID_WIDTH);

    for (int i=nFactor; i<=nSize; i+=nFactor)
	{
		TCHAR buffer[8];
		if (bShowPos)
			_stprintf_s(buffer, _T("%d"), i);

		int xpos, ypos;
		// Horizontal
		if (m_rulerType == RT_HORIZONTAL)
		{
			//xpos = i*STD_GRID_WIDTH - (STD_GRID_WIDTH/2) - m_scrollPos.x;	// What the heck are the last two values?
			xpos = i*STD_GRID_WIDTH;

			// Horizontal Ruler Tick marks are moved half-a-grid-square
			//  left to line up above the respective column. - CSMALL
            pDC->PatBlt(xpos, rulerRect.top + nBegin, nThickness, rulerRect.bottom, BLACKNESS);
			if (bShowPos)
                pDC->TextOut(xpos, rulerRect.top, buffer, _tcslen(buffer));
		}
		// Vertical
		else
		{	
			// Vertical Ruler Tick marks moved half-a-grid-square up to line up with 
			//  row, and 'TextOut' output is moved 1-and-a-half squares up. - CSMALL
			//ypos = i*STD_GRID_WIDTH - m_scrollPos.y - (STD_GRID_WIDTH/2)-1;
			ypos = i*STD_GRID_WIDTH;
			pDC->PatBlt(rulerRect.left+nBegin, ypos, rulerRect.right, nThickness, BLACKNESS);
			if (bShowPos)
				pDC->TextOut(rulerRect.left+3, i*STD_GRID_WIDTH - m_scrollPos.y-STD_GRID_WIDTH, buffer, _tcslen(buffer));
				//pDC->TextOut(rulerRect.left+3, i*STD_GRID_WIDTH - m_scrollPos.y -(STD_GRID_WIDTH + STD_GRID_WIDTH/2), buffer, _tcslen(buffer));
		}
	}
}

void CRulerView::DrawCursorPos(CPoint NewPos)
{
	// E-KSo Lab
	//return;
	if (((m_rulerType == RT_HORIZONTAL) && (NewPos.x > m_DocSize.cx*m_fZoomFactor)) ||
		((m_rulerType == RT_VERTICAL)   && (NewPos.y > m_DocSize.cy*m_fZoomFactor)))
		return;

	CDC* pDC = GetDC();
	// set the map mode right
	int oldMapMode = pDC->SetMapMode(MM_TEXT);

	CRect clientRect;
	GetClientRect(&clientRect);
	if (m_rulerType==RT_HORIZONTAL)
	{
		// erase the previous position
		if (m_lastPos.x != -1)
			pDC->PatBlt(m_lastPos.x, clientRect.top, 3, clientRect.bottom, DSTINVERT);
		// draw the new position
		m_lastPos.x = NewPos.x;
        pDC->PatBlt(m_lastPos.x, clientRect.top, 3, clientRect.bottom, DSTINVERT);
	}
	else // (m_rulerType==RT_VERTICAL)
	{
		// erase the previous position
		if (m_lastPos.y != -1)
	        pDC->PatBlt(clientRect.left, m_lastPos.y, clientRect.right, 3, DSTINVERT);
		// draw the new position
		m_lastPos.y = NewPos.y;
        pDC->PatBlt(clientRect.left, m_lastPos.y, clientRect.right, 3, DSTINVERT);
	}

	pDC->SetMapMode(oldMapMode);
	ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CRulerView diagnostics

#ifdef _DEBUG
void CRulerView::AssertValid() const
{
	CView::AssertValid();
}

void CRulerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRulerView message handlers

BOOL CRulerView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// create a brush using the same color as the background
	if (cs.lpszClass == NULL)
	{
		HBRUSH hBr=CreateSolidBrush(GetSysColor(COLOR_MENU));
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_BYTEALIGNWINDOW, NULL, hBr);
	}
	return CView::PreCreateWindow(cs);
}

void CRulerView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	if (m_rulerType == RT_HORIZONTAL)
		((CSplitterWnd*)GetParent())->SetActivePane(0, 1);
	else
		((CSplitterWnd*)GetParent())->SetActivePane(0, 0);
}

void CRulerView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView

IMPLEMENT_DYNCREATE(CRulerCornerView, CView)
CRulerCornerView::CRulerCornerView() {}
CRulerCornerView::~CRulerCornerView() {}

BEGIN_MESSAGE_MAP(CRulerCornerView, CView)
	//{{AFX_MSG_MAP(CRulerCornerView)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView drawing
void CRulerCornerView::OnDraw(CDC* pDC) {}

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView diagnostics
#ifdef _DEBUG
void CRulerCornerView::AssertValid() const
{
	CView::AssertValid();
}

void CRulerCornerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView message handlers

BOOL CRulerCornerView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// create a brush using the same color as the background
	if (cs.lpszClass == NULL)
	{
		HBRUSH hBr=CreateSolidBrush(GetSysColor(COLOR_MENU));
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_BYTEALIGNWINDOW, NULL, hBr);
	}
	return CView::PreCreateWindow(cs);
}

void CRulerCornerView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
    ((CSplitterWnd*)GetParent())->SetActivePane(0, 0);	
}
