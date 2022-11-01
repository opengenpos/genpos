/////////////////////////////////////////////////////////////////////////////
// reportvw.cpp : implementation file
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
#include "progrpt.h"

#include "progrdoc.h"
#include "reportvw.h"
#include "report.h"
#include "dlgfield.h"
#include "dlgtitle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, CScrollView)

CReportView::CReportView()
{
	m_bPrint = TRUE;
	m_bMathe = TRUE;
}

CReportView::~CReportView()
{
}

BOOL CReportView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	return CScrollView::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CReportView, CScrollView)
	//{{AFX_MSG_MAP(CReportView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_REPORT_SETTITLE, OnReportSettitle)
	ON_COMMAND(ID_REPORT_INSERTFIELD, OnReportInsertfield)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_VIEW_PRINT, OnViewPrint)
	ON_COMMAND(ID_VIEW_MATHE, OnViewMathe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINT, OnUpdateViewPrint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MATHE, OnUpdateViewMathe)
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_INSERTROW, OnEditInsertrow)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTROW, OnUpdateEditInsertrow)
	ON_UPDATE_COMMAND_UI(ID_REPORT_INSERTFIELD, OnUpdateReportInsertfield)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_FIELD_DLG, OnFieldDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
CProgrptDoc* CReportView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProgrptDoc)));
	return (CProgrptDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportView

void CReportView::OnInitialUpdate()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->GetNewReport();
	ASSERT(pReport != NULL);

	pReport->SetView(this);
	pReport->UpdateSheetSize(0, FALSE);

	CScrollView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CReportView drawing

void CReportView::OnActivateView(BOOL bActivate, CView* pActiveView,
	CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

	if (bActivate)						// this view is active
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		pDoc->UpdateStatusSize();		// update status bar

		CReport* pReport = pDoc->FindActiveReport(this);
		ASSERT(pReport != NULL);

		CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
		CWnd* pWnd = pMDIFrameWnd->MDIGetActive();
		ASSERT(pWnd != NULL);

		if (pWnd->IsIconic())			// icon position
			m_pField->ActiveDialog(FALSE, NULL, -1);
		else							// maximized or normal position
			m_pField->ActiveDialog(TRUE, this, pReport->GetFileIndex());
#if	0
	TRACE("CReportView activate: %d, 0x%08lX, 0x%08lX\n",
		bActivate, pActiveView, pDeactiveView);
#endif
	}
}

void CReportView::OnDraw(CDC* pDC)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnDraw(pDC);
}

void CReportView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CScrollView::OnUpdate(pSender, lHint, pHint);

	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	if (pReport != NULL)
	{
		CString strTitle;
		strTitle = pDoc->GetTitle() + ":" + pReport->GetExtendedTitle();
		GetParentFrame()->SetWindowText(strTitle);
	}
}

void CReportView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	if (pReport != NULL)
		pReport->SetScrollInfo();
}

/////////////////////////////////////////////////////////////////////////////
// CReportView implementation

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers

int CReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CReportView::OnDestroy()
{
	CScrollView::OnDestroy();

	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	m_pField->ActiveDialog(FALSE, NULL, -1);

	pReport->SetView(NULL);
}

void CReportView::OnMouseMove(UINT nFlags, CPoint point)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnMouseMove(nFlags, point);

	CScrollView::OnMouseMove(nFlags, point);
}

void CReportView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnLButtonDown(nFlags, point);

	CScrollView::OnLButtonDown(nFlags, point);
}

void CReportView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnLButtonDblClk(nFlags, point);

	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CReportView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnLButtonUp(nFlags, point);

	CScrollView::OnLButtonUp(nFlags, point);
}

long CReportView::OnFieldDlg(UINT wParam, LONG lParam)
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	switch (wParam)
	{
	case IDC_FIELD_CLOSE:
		m_pField->ShowDialog(FALSE, this, pReport->GetFileIndex());
	case IDC_FIELD_CANCEL:
		pReport->Action(CReport::none);
		break;
	case IDC_FIELD_INSERT:
		pReport->m_nOperation = m_pField->m_nOperation;
		pReport->m_nGroup     = m_pField->m_nGroup;
		pReport->m_nField     = m_pField->m_nField;
		pReport->m_nItem      = m_pField->m_nItem;
		pReport->Action(CReport::insertItem);
		break;
	default:
		TRACE("CReportView::OnFieldDlg %x, %lx\n", wParam, lParam);
	}
	
	return 0L;
}

void CReportView::OnReportInsertfield()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	m_pField->ShowDialog(TRUE, this, pReport->GetFileIndex());
}

void CReportView::OnUpdateReportInsertfield(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CReportView::OnReportSettitle()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	CTitleDlg dlg;
	dlg.m_strTitle = pReport->GetTitle();
	if (dlg.DoModal() != IDOK)
		return;							// cancel by user

	pReport->SetTitle(dlg.m_strTitle);
	pDoc->SetModifiedFlag();
	pDoc->UpdateAllViews(NULL);
}

void CReportView::OnEditCut()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnEditCut();
}

void CReportView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		CReport* pReport = pDoc->FindActiveReport(this);
		ASSERT(pReport != NULL);

		pCmdUI->Enable(pReport->OnUpdateEditCut());
	}
}

void CReportView::OnEditCopy()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnEditCopy();
}

void CReportView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		CReport* pReport = pDoc->FindActiveReport(this);
		ASSERT(pReport != NULL);

		pCmdUI->Enable(pReport->OnUpdateEditCopy());
	}
}

void CReportView::OnEditPaste()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnEditPaste();
}

void CReportView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(::IsClipboardFormatAvailable(
			((CProgrptApp*)AfxGetApp())->m_uPrivateFmt));
}

void CReportView::OnEditClear()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->OnEditClear();
}

void CReportView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		CReport* pReport = pDoc->FindActiveReport(this);
		ASSERT(pReport != NULL);

		pCmdUI->Enable(pReport->OnUpdateEditClear());
	}
}

void CReportView::OnViewPrint()
{
	m_bPrint = ! m_bPrint;
	Invalidate(FALSE);
}

void CReportView::OnUpdateViewPrint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bPrint);
}

void CReportView::OnViewMathe()
{
	m_bMathe = ! m_bMathe;
	Invalidate(FALSE);
}

void CReportView::OnUpdateViewMathe(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bMathe);
}

void CReportView::OnEditInsertrow()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CReport* pReport = pDoc->FindActiveReport(this);
	ASSERT(pReport != NULL);

	pReport->Action(CReport::insertRow);
}

void CReportView::OnUpdateEditInsertrow(CCmdUI* pCmdUI)
{
	CMDIFrameWnd* pMDIFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	CWnd* pWnd = pMDIFrameWnd->MDIGetActive();

	if (pWnd != NULL && pWnd->IsIconic())
		pCmdUI->Enable(FALSE);
	else
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		CReport* pReport = pDoc->FindActiveReport(this);
		ASSERT(pReport != NULL);

		pCmdUI->Enable(pReport->OnUpdateEditInsertrow());
	}
}

void CReportView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar) {
    case VK_HOME:
		OnVScroll(SB_TOP, 0, NULL);
		OnHScroll(SB_LEFT, 0, NULL);
		break;
    case VK_END:
		OnVScroll(SB_BOTTOM, 0, NULL);
		OnHScroll(SB_RIGHT, 0, NULL);
		break;
    case VK_UP:
		OnVScroll(SB_LINEUP, 0, NULL);
		break;
    case VK_DOWN:
		OnVScroll(SB_LINEDOWN, 0, NULL);
		break;
    case VK_PRIOR:
		OnVScroll(SB_PAGEUP, 0, NULL);
		break;
    case VK_NEXT:
		OnVScroll(SB_PAGEDOWN, 0, NULL);
		break;
    case VK_LEFT:
		OnHScroll(SB_LINELEFT, 0, NULL);
		break;
    case VK_RIGHT:
		OnHScroll(SB_LINERIGHT, 0, NULL);
		break;
    default:
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}
