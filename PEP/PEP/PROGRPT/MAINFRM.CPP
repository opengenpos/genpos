/////////////////////////////////////////////////////////////////////////////
// mainfrm.cpp : implementation of the CMainFrame class
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Dec-20-02           R.Chambers   Changes to address SR93, add Insert Field
//                                  accellerator and toolbar button.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "mainfrm.h"
#include "progrdoc.h"
#include "listvw.h"
#include "dlgeod.h"
#include "dlgptd.h"
#include "dlg16cm.h"
#include "dlg40cm.h"
#include "dlgfield.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static	wchar_t szFrameFmt[] = L"%u,%u,%d,%d,%d,%d,%d,%d,%d,%d";
static	wchar_t szFramePos[] = L"FramePos";

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_REPORT_NEW, OnReportNew)
	ON_COMMAND(ID_SETTING_EOD, OnSettingEod)
	ON_COMMAND(ID_SETTING_PTD, OnSettingPtd)
	ON_COMMAND(ID_SETTING_16CHAR, OnSetting16char)
	ON_COMMAND(ID_SETTING_40CHAR, OnSetting40char)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_REPORT_NEW, OnUpdateReportNew)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SET_PATHNAME, OnSetPathName)
	ON_MESSAGE(ID_STATUS_MSG, OnStatusMsg)
	ON_COMMAND(ID_INDICATOR_SIZE, OnUpdateFree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
// RJC SR93 added Insert Field to toolbar.  Supporting code already in place.
//     also made changes to toolbar bitmap and accellerator key
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR,
	ID_REPORT_INSERTFIELD,
	ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_INDICATOR_SIZE,		// used and allocated memory indicator
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_SEPARATOR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
//RJC added from example of new Visual C++ 6.0 MFC wizard
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.lpszClass = szClassName;			// change class name
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame operations

void CMainFrame::InitialShowWindow(UINT nCmdShow)
{
	WINDOWPLACEMENT wp;
	if (! ReadWindowPlacement(szFramePos, &wp))
	{
		ShowWindow(nCmdShow);			// default window placement
		return;
	}
	if (nCmdShow != SW_SHOWNORMAL)		// invalid state ?
		wp.showCmd = nCmdShow;			// set default state

	SetWindowPlacement(&wp);			// restore window placement
	ShowWindow(wp.showCmd);
}

BOOL CMainFrame::ReadWindowPlacement(const wchar_t * pszEntry,
	LPWINDOWPLACEMENT pwp)
{
	// read information of window placement
	CString strData = AfxGetApp()->GetProfileString(szClassName, pszEntry);
	if (strData.IsEmpty())				// data not found ?
		return FALSE;

	WINDOWPLACEMENT wp;
	int nRead = swscanf(strData, szFrameFmt,
					   &wp.flags, &wp.showCmd,
					   &wp.ptMinPosition.x, &wp.ptMinPosition.y,
					   &wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
					   &wp.rcNormalPosition.left,
					   &wp.rcNormalPosition.top,
					   &wp.rcNormalPosition.right,
					   &wp.rcNormalPosition.bottom);
	if (nRead != 10)					// converted error ?
		return FALSE;

	wp.length = sizeof wp;
	*pwp = wp;
	return TRUE;
}

void CMainFrame::WriteWindowPlacement(const wchar_t * pszEntry,
	LPWINDOWPLACEMENT pwp)
{
	wchar_t szData[sizeof(L"-32767") * 8 + sizeof(L"65535") * 2];

	swprintf_s(szData, sizeof(szData)/sizeof(szData[0]), szFrameFmt,
			pwp->flags, pwp->showCmd,
			pwp->ptMinPosition.x, pwp->ptMinPosition.y,
			pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
			pwp->rcNormalPosition.left,
			pwp->rcNormalPosition.top,
			pwp->rcNormalPosition.right,
			pwp->rcNormalPosition.bottom);

	// write information of window placement
	AfxGetApp()->WriteProfileString(szClassName, pszEntry, szData);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// set information of status bar
	CDC* pDC = GetDC();
	UINT nID, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(0, nID, SBPS_NORMAL | SBPS_STRETCH, cxWidth);
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_DISABLED, 0);
	m_wndStatusBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_DISABLED, 0);
	m_wndStatusBar.SetPaneInfo(6, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_DISABLED, 0);

	CFieldDlg::CreateFieldDialog();			// create "Insert Field" dialog box

	return 0;
}

void CMainFrame::OnClose()
{
	// update window placement of main frame
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if (GetWindowPlacement(&wp))
	{
		wp.flags = 0;
		if (IsZoomed())
			wp.flags |= WPF_RESTORETOMAXIMIZED;
		WriteWindowPlacement(szFramePos, &wp);
	}

	m_pField->CloseDialog();			// close "Insert Field" dialog box

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDestroy()
{
	CFieldDlg::DestroyDialog();			// remove object of "Insert Field"

	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnReportNew()
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());

	if (pDoc != NULL)
		pDoc->GetListView()->CreateNewReportAndFrame();
	else
		TRACE0("Warning: Active document not found\n");
}

void CMainFrame::OnUpdateReportNew(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());

	pCmdUI->Enable(pDoc->GetCountReport(TRUE) < REPORT_MAX_SHEET);
}

void CMainFrame::OnSettingEod()
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
	if (pDoc == NULL)
		return;

	CEodDlg dlg(pDoc);
	if (dlg.DoModal() != IDOK)
		return;

	pDoc->SetModifiedFlag();
}

void CMainFrame::OnSettingPtd()
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
	if (pDoc == NULL)
		return;

	CPtdDlg dlg(pDoc);
	if (dlg.DoModal() != IDOK)
		return;

	pDoc->SetModifiedFlag();
}

void CMainFrame::OnSetting16char()
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
	if (pDoc == NULL)
		return;

	C16CMDlg dlg(pDoc);
	if (dlg.DoModal() != IDOK)
		return;

	pDoc->SetModifiedFlag();
	pDoc->UpdateAllViews(NULL);
}

void CMainFrame::OnSetting40char()
{
	CMDIChildWnd* pWnd = MDIGetActive();
	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
	if (pDoc == NULL)
		return;

	C40CMDlg dlg(pDoc);
	if (dlg.DoModal() != IDOK)
		return;

	pDoc->SetModifiedFlag();
	pDoc->UpdateAllViews(NULL);
}

long CMainFrame::OnSetPathName(UINT wParam, LONG lParam)
{
	CMDIChildWnd* pWnd = MDIGetActive();
	if (pWnd == NULL)
		return 0L;

	CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
	if (pDoc != NULL)
		pDoc->SetPathName(AfxGetApp()->m_lpCmdLine);
	else
		ASSERT(FALSE);

	return 0L;
}

long CMainFrame::OnStatusMsg(UINT wParam, LONG lParam)
{
	CString strMsg;
	if (! strMsg.LoadString(wParam))
		ASSERT(FALSE);
	m_wndStatusBar.SetPaneText(0, strMsg);
	return 0L;
}

void CMainFrame::OnUpdateFree()
{
	wchar_t szText[16];
	long lAlloc = -1L, lUsed;

	CMDIChildWnd* pWnd = MDIGetActive();
	if (pWnd != NULL)
	{
		CProgrptDoc* pDoc = (CProgrptDoc*)(pWnd->GetActiveDocument());
		if (pDoc != NULL && (lAlloc = pDoc->GetAllocSize()) > 0L)
		{
			lUsed = pDoc->GetUsedSize();
			wsprintf(szText, L"%05ld of %05ld", lUsed, lAlloc);
		}
	}

	if (lAlloc == -1L)
		wcscpy(szText, L"00000 of ?????");

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_SIZE), szText);
}
