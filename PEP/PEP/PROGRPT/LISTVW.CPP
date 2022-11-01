/////////////////////////////////////////////////////////////////////////////
// listvw.cpp : implementation of the CListView class
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

#include "report.h"
#include "progrdoc.h"
#include "listvw.h"
#include "reportvw.h"
#include "dlglist.h"
#include "dlgfield.h"
#include "dlgimp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define	LIST_SELECT_BIAS	100

/////////////////////////////////////////////////////////////////////////////
// CListView

IMPLEMENT_DYNCREATE(CListView, CFormView)

BEGIN_MESSAGE_MAP(CListView, CFormView)
	//{{AFX_MSG_MAP(CListView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_REPORT_OPEN, OnReportOpen)
	ON_LBN_DBLCLK(IDC_REPORT_LIST, OnDblclkReportList)
	ON_LBN_SELCANCEL(IDC_REPORT_LIST, OnSelcancelReportList)
	ON_LBN_SELCHANGE(IDC_REPORT_LIST, OnSelchangeReportList)
	ON_UPDATE_COMMAND_UI(ID_REPORT_NEW, OnUpdateReportNew)
	ON_UPDATE_COMMAND_UI(ID_REPORT_OPEN, OnUpdateReportOpen)
	ON_BN_CLICKED(IDC_REPORT_DELETE, OnReportDelete)
	ON_BN_CLICKED(IDC_REPORT_NEW, OnReportNew)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_REPORT_UP, OnReportUp)
	ON_BN_CLICKED(IDC_REPORT_DOWN, OnReportDown)
	ON_COMMAND(ID_REPORT_IMPORT, OnReportImport)
	ON_UPDATE_COMMAND_UI(ID_REPORT_IMPORT, OnUpdateReportImport)
	ON_COMMAND(ID_REPORT_OPEN, OnReportOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListView construction/destruction

CListView::CListView()
	: CFormView(CListView::IDD)
{
	//{{AFX_DATA_INIT(CListView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
}

CListView::~CListView()
{
}

void CListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListView)
	DDX_Control(pDX, IDC_REPORT_UP, m_btnUp);
	DDX_Control(pDX, IDC_REPORT_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_REPORT_NEW, m_btnNew);
	DDX_Control(pDX, IDC_REPORT_LIST, m_lbList);
	DDX_Control(pDX, IDC_REPORT_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_REPORT_DELETE, m_btnDelete);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CListView diagnostics

#ifdef _DEBUG
void CListView::AssertValid() const
{
	CFormView::AssertValid();
}

void CListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CProgrptDoc* CListView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProgrptDoc)));
	return (CProgrptDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListView 

void CListView::OnInitialUpdate()
{
    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);

	if (pDoc->GetListView() == NULL)
		pDoc->SetListView(this);
	else
	{
		TRACE0("Warning: CListView class already registed\n");
		ASSERT(FALSE);
	}
	
	// set tab stops in list box.
	int nTabs[3] = {20, 90, 160};
	((CListBox*)GetDlgItem(IDC_REPORT_LIST))->SetTabStops(3, nTabs);

	// adjust window size
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CListView implementations

BOOL CListView::CreateNewReportFrame(CReport* pReport)
{
    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);

	CDocTemplate* pTemplate =
		((CProgrptApp*)AfxGetApp())->m_pReportViewTemplate;
	CMDIChildWnd* pNewFrame =
		(CMDIChildWnd*)(pTemplate->CreateNewFrame(pDoc, NULL));
    if (pNewFrame != NULL)
    	pDoc->SetNewReport(pReport);
    else
    {
        TRACE0("failed to create new report frame\n");
		return FALSE;					// failed to create
	}

    ASSERT(pNewFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
    pTemplate->InitialUpdateFrame(pNewFrame, pDoc);

	return TRUE;
}

int CListView::CreateNewReportAndFrame()
{
    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);

	// check number of report
	if (pDoc->GetCountReport(TRUE) >= REPORT_MAX_SHEET)
	{
       	TRACE0("failed to create new report\n");
		return -1;						// failed to create
	}

	// check document size
	if (pDoc->CheckSize(DOC_SIZE_REPORT) == PROGRPT_ERROR)
		return -1;

	// do prompt for new report informations
	CListPropDlg dlg(pDoc);
	if (dlg.DoModal() != IDOK)
		return 0;						// cancel by user

	// create new report object
	CReport* pReport = new
		CReport(pDoc, dlg.m_nFile, dlg.m_nDevice, dlg.m_strTitle);
	pDoc->AddReport(pReport);

	if (! CreateNewReportFrame(pReport))
	{
		TRACE0("failed to create new frame\n");
		ASSERT(FALSE);
		pDoc->RemoveReport(pReport);
		delete pReport;
		return -1;						// failed to create
	}

	pDoc->UpdateAllViews(NULL);			// update report list

	return 0;							// successful
}

int CListView::ActivateReportFrame()
{
	int nIndex = m_lbList.GetCurSel();
	if (nIndex == LB_ERR)				// no item is selected ?
		return -1;
    
    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);
	POSITION pos = pDoc->FindIndexReport(nIndex);
	CReport* pReport = pDoc->GetAtReport(pos);
	ASSERT(pReport != NULL);

	if (pReport->GetView() == NULL)
		return CreateNewReportFrame(pReport);

	pReport->ActivateWindow();
	return 0;
}

void CListView::DestroyAllReportFrames()
{
    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);

	POSITION pos = pDoc->GetHeadPositionReport();
	while (pos != NULL)
		pDoc->GetNextReport(pos)->GetView()->DestroyWindow();
}

void CListView::UpDownControl()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	BOOL bUp = FALSE, bDown = FALSE;
	int nIndex = m_lbList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		int nCount = pDoc->GetCountReport(FALSE);	// exclude LCD
		if (nCount == 1 || nIndex >= nCount)
			;
		else if (nIndex == 0)
			bDown = TRUE;
		else if (nIndex == nCount - 1)
			bUp = TRUE;
		else
		{
			bUp = TRUE;
			bDown = TRUE;
		}
	}

	m_btnUp.EnableWindow(bUp);
	m_btnDown.EnableWindow(bDown);
}

/////////////////////////////////////////////////////////////////////////////
// CReportView drawing

void CListView::OnActivateView(BOOL bActivate, CView* pActiveView,
	CView* pDeactiveView)
{
	CFormView::OnActivateView(bActivate, pActiveView, pDeactiveView);

	if (bActivate)						// this view is active
	{
		CProgrptDoc* pDoc = GetDocument();
		ASSERT(pDoc != NULL);
		pDoc->UpdateStatusSize();		// update status bar

		m_pField->ActiveDialog(FALSE, NULL, -1);

#if	0
	TRACE("CListView activate: %d, 0x%08lX, 0x%08lX\n",
		bActivate, pActiveView, pDeactiveView);
#endif
	}
}

void CListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CFormView::OnUpdate(pSender, lHint, pHint);

	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	m_lbList.ResetContent();			// delete all report information

	// update all report information
	int nCount = 1;
	CReport* pReport;
	wchar_t szBuffer[128];
	POSITION pos = pDoc->GetHeadPositionReport();
	while (pos != NULL)
	{
		pReport = pDoc->GetNextReport(pos);
		pReport->FormatReportInfo(&nCount, szBuffer);
		m_lbList.InsertString(-1, szBuffer);
	}

	if (lHint >= LIST_SELECT_BIAS)
		m_lbList.SetCurSel((int)(lHint - LIST_SELECT_BIAS));
	else
	{
		m_btnOpen.EnableWindow(FALSE);
		m_btnDelete.EnableWindow(FALSE);
		m_btnUp.EnableWindow(FALSE);
		m_btnDown.EnableWindow(FALSE);
	}

	// update "new..." push button on list view
	if (GetDocument()->GetCountReport(TRUE) < REPORT_MAX_SHEET)
		m_btnNew.EnableWindow(TRUE);
	else
		m_btnNew.EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CListView message handlers

int CListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create solid brush of gray on dialog-template resouce
	m_hBrush.CreateSolidBrush(RGB(192, 192, 192));

	return 0;
}

void CListView::OnClose()
{
	DestroyAllReportFrames();
	m_hBrush.DeleteObject();

	CFormView::OnClose();
}

void CListView::OnDestroy()
{
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	pDoc->UpdateStatusSize();			// update status bar

	CFormView::OnDestroy();
}

HBRUSH CListView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor) {
	case CTLCOLOR_DLG:
		return (HBRUSH)m_hBrush.GetSafeHandle();	// solid brush of gray
		
	case CTLCOLOR_STATIC:
		pDC->SetBkColor(RGB(192, 192, 192));		// background of gray
		return (HBRUSH)m_hBrush.GetSafeHandle();	// solid brush of gray
	}
	
	return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CListView::OnReportNew()
{
	CreateNewReportAndFrame();
}

void CListView::OnReportOpen()
{
	ActivateReportFrame();
}

void CListView::OnReportImport()
{
	static wchar_t szFilter[] = L"Report Files (*.rpt)|*.rpt|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, L"rpt", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter);
	dlg.m_ofn.lpstrTitle = L"Browse Files";
	if (dlg.DoModal() != IDOK)
		return;

	CImportDlg import(dlg.GetPathName());
	if (import.DoModal() != IDOK)
		return;

    CProgrptDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);

	// import report
	CReport* pReport = new CReport(pDoc);
	if (pReport->OpenDocument(import.m_nReport, dlg.GetPathName()))
		pDoc->AddReport(pReport, FALSE);
	else
		delete pReport;

	// check report size
	if (pDoc->GetAllocSize() > 0L &&
		pDoc->GetAllocSize() <= pDoc->GetUsedSize())	// overflow
	{
		if (AfxMessageBox(IDS_ERRMSG_SIZE, MB_YESNO) != IDYES)
		{
			pDoc->RemoveReport(pReport, FALSE);
			return;
		}
	}

	pDoc->SetModifiedFlag();
	pDoc->UpdateAllViews(NULL);			// update report list
}

void CListView::OnDblclkReportList()
{
	ActivateReportFrame();
}

void CListView::OnSelchangeReportList()
{
	m_btnOpen.EnableWindow(TRUE);
	m_btnDelete.EnableWindow(TRUE);
	UpDownControl();
}

void CListView::OnSelcancelReportList()
{
#if	0
	GetDlgItem(IDC_REPORT_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_REPORT_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_REPORT_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_REPORT_DOWN)->EnableWindow(FALSE);
#endif
}

void CListView::OnReportDelete()
{
	int nIndex = m_lbList.GetCurSel();	// zero-based index
	if (nIndex == LB_ERR)				// no item is selected ?
		return;

    CProgrptDoc* pDoc = GetDocument();
	CReport* pReport = pDoc->GetReport(nIndex + 1);

	// confrim delete message
	CString strFile = ptheInfo->GetFileName(pReport->GetFileIndex());
	CString strMessage;
	AfxFormatString1(strMessage, IDS_ERRMSG_REPORTDELETE, strFile);
	if (AfxMessageBox(strMessage, MB_OKCANCEL) != IDOK)
		return;

	// delete report objects
	pReport->DestroyWindow();
	pDoc->RemoveReport(pReport);
	pDoc->UpdateAllViews(NULL);

	m_lbList.SetFocus();
}

void CListView::OnReportUp()
{
	int nIndex = m_lbList.GetCurSel();
	if (nIndex == LB_ERR)
		return;
	ASSERT(nIndex > 0);
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	POSITION pos = pDoc->FindIndexReport(nIndex);
	CReport* pReport = pDoc->GetAtReport(pos);
	pDoc->RemoveAtReport(pos);
	pos = pDoc->FindIndexReport(nIndex - 1);
	pDoc->InsertBeforeReport(pos, pReport);

	pDoc->UpdateAllViews(NULL, LIST_SELECT_BIAS + nIndex - 1);
	UpDownControl();

	pDoc->SetModifiedFlag();
}

void CListView::OnReportDown()
{
	int nIndex = m_lbList.GetCurSel();
	if (nIndex == LB_ERR)
		return;
	CProgrptDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	int nCount = pDoc->GetCountReport(FALSE);	// exclude LCD
	ASSERT(nIndex + 1 < nCount);

	POSITION pos = pDoc->FindIndexReport(nIndex);
	CReport* pReport = pDoc->GetAtReport(pos);
	pDoc->RemoveAtReport(pos);
	pos = pDoc->FindIndexReport(nIndex);
	pDoc->InsertAfterReport(pos, pReport);

	pDoc->UpdateAllViews(NULL, LIST_SELECT_BIAS + nIndex + 1);
	UpDownControl();

	pDoc->SetModifiedFlag();
}

void CListView::OnUpdateReportNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetCountReport(TRUE) < REPORT_MAX_SHEET);
}

void CListView::OnUpdateReportOpen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetCountReport(TRUE) > 0 &&
				   m_lbList.GetCurSel() != LB_ERR);
}

void CListView::OnUpdateReportImport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetCountReport(TRUE) < REPORT_MAX_SHEET);
}
