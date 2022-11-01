/////////////////////////////////////////////////////////////////////////////
// dlgfield.cpp : implementation file
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

#include "mainfrm.h"
#include "progrdoc.h"
#include "report.h"
#include "reportvw.h"
#include "dlgfield.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static wchar_t szField[] = L"FieldPos";

/////////////////////////////////////////////////////////////////////////////
// CFieldDlg dialog

CFieldDlg::CFieldDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFieldDlg)
	m_nLoop = -1;
	//}}AFX_DATA_INIT

	m_pView  = NULL;
	m_nFile  = -1;
	m_bClose = FALSE;
}

CFieldDlg::CFieldDlg(CView* pView)
	: CDialog()
{
	m_pView  = pView;
	m_nFile  = -1;
	m_bClose = FALSE;
}

void CFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldDlg)
	DDX_Control(pDX, IDC_FIELD_TOP, m_BtTop);
	DDX_Control(pDX, IDC_FIELD_ITEM, m_LbItem);
	DDX_Control(pDX, IDC_FIELD_FIELD, m_LbField);
	DDX_Control(pDX, IDC_FIELD_GROUP, m_LbGroup);
	DDX_Radio(pDX, IDC_FIELD_LOOP, m_nLoop);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFieldDlg, CDialog)
	//{{AFX_MSG_MAP(CFieldDlg)
	ON_BN_CLICKED(IDC_FIELD_CLOSE, OnFieldClose)
	ON_BN_CLICKED(IDC_FIELD_INSERT, OnFieldInsert)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_FIELD_GROUP, OnSelchangeFieldGroup)
	ON_BN_CLICKED(IDC_FIELD_LOOP, OnFieldLoop)
	ON_BN_CLICKED(IDC_FIELD_PRINT, OnFieldPrint)
	ON_BN_CLICKED(IDC_FIELD_MATHE, OnFieldMathe)
	ON_LBN_SELCHANGE(IDC_FIELD_FIELD, OnSelchangeFieldField)
	ON_LBN_SELCHANGE(IDC_FIELD_ITEM, OnSelchangeFieldItem)
	ON_LBN_DBLCLK(IDC_FIELD_FIELD, OnDblclkFieldField)
	ON_LBN_DBLCLK(IDC_FIELD_ITEM, OnDblclkFieldItem)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// The one and only CFieldDlg object

CFieldDlg* m_pField;

/////////////////////////////////////////////////////////////////////////////
// CFieldDlg operations

void CFieldDlg::CreateFieldDialog()
{
	if (m_pField == NULL)					// first called
		m_pField = new CFieldDlg();
}

void CFieldDlg::CloseDialog()
{
	if (m_pField == NULL || m_pField->GetSafeHwnd() == NULL)
		return;

	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if (GetWindowPlacement(&wp))
	{
		wp.flags = 0;
		if (IsZoomed())
			wp.flags |= WPF_RESTORETOMAXIMIZED;
		CMainFrame::WriteWindowPlacement(szField, &wp);
	}
}

void CFieldDlg::DestroyDialog()
{
	if (m_pField != NULL)
	{
		if (m_pField->GetSafeHwnd() != NULL)
			m_pField->DestroyWindow();
		delete m_pField;
	}
}

void CFieldDlg::ShowDialog(BOOL bState, CView* pView, int nFile)
{
	if (GetSafeHwnd() == NULL)
		if (bState)
			CDialog::Create(CFieldDlg::IDD);
		else
			return;

	if (m_bShow != bState)					// change state
	{
		m_bShow  = bState;					// set new state
		if (m_bShow)
		{
			m_bClose = FALSE;
			ShowWindow(SW_SHOW);
		}
		else
		{
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			m_bClose = TRUE;
			ShowWindow(SW_HIDE);
		}
	}

	UpdateView(pView, nFile);
}

void CFieldDlg::ActiveDialog(BOOL bState, CView* pView, int nFile)
{
	if (m_bClose)
		return;
	if (GetSafeHwnd() == NULL)
		if (bState)
			CDialog::Create(CFieldDlg::IDD);
		else
			return;

	BOOL bNew;
	if (bState || m_BtTop.GetCheck() > 0)	// show dialog box
		bNew = TRUE;
	else
		bNew = FALSE;

	if (m_bShow != bNew)					// change state
	{
		m_bShow = bNew;						// set new state
		if (m_bShow)
			ShowWindow(SW_SHOW);
		else
		{
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(SW_HIDE);
		}
	}

	UpdateView(pView, nFile);
}

/////////////////////////////////////////////////////////////////////////////
// CFieldDlg implementation

BOOL CFieldDlg::Create()
{
	return CDialog::Create(CFieldDlg::IDD);
}

void CFieldDlg::UpdateView(CView* pView, int nFile)
{
	if (m_pView == pView && m_nFile == nFile)
		return;

	m_pView = pView;
	m_nFile = nFile;

	// set default radio button
//	((CButton*)(GetDlgItem(IDC_FIELD_LOOP)))->SetFocus();
	((CButton*)(GetDlgItem(IDC_FIELD_LOOP)))->SetCheck(1);
	((CButton*)(GetDlgItem(IDC_FIELD_PRINT)))->SetCheck(0);
	((CButton*)(GetDlgItem(IDC_FIELD_MATHE)))->SetCheck(0);

	DisableInsertButton();				// disable "insert" button
	RefreshGroup(CFG_INDEX_LOOP);
}

void CFieldDlg::RefreshGroup(int nOperation)
{
	m_LbGroup.ResetContent();
	m_LbField.ResetContent();
	m_LbItem.ResetContent();
	
	if (m_nFile == -1)
		return;

	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;

	POSITION pos = pConfig->GetHeadGroup();
	while (pos != NULL)
	{
		CFG_OPEGROUP* pOpeGroup = pConfig->GetNextGroup(pos, nOperation);
		if (pOpeGroup == NULL)
			return;
		m_LbGroup.InsertString(-1, pOpeGroup->m_strName);
	}
}

void CFieldDlg::RefreshField()
{
	m_LbField.ResetContent();
	m_LbItem.ResetContent();

	if (m_nFile == -1)
		return;
	int nOperation = GetOperation();
	int nGroup = m_LbGroup.GetCurSel();
	if (nGroup == -1)
		return;

	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(nOperation, nGroup);
	if (pOpeGroup == NULL)
		return;

	int nCount = 0;
	POSITION pos = pConfig->FieldFindIndex(pOpeGroup->m_nIndex);
	while (pos != NULL && nCount < pOpeGroup->m_nCount)
	{
		CInfoField* pField = pConfig->FieldGetNext(pos);
		if (pField == NULL)
			return;
		pField->FieldInsertString(&m_LbField);
		nCount++;
	}
}

void CFieldDlg::RefreshItem()
{
	m_LbItem.ResetContent();

	if (m_nFile == -1)
		return;
	int nOperation = GetOperation();
	int nGroup = m_LbGroup.GetCurSel();
	if (nGroup == -1)
		return;
	int nField = m_LbField.GetCurSel();
	if (nField == -1)
		return;

	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	if (pConfig == NULL)
		return;
	CFG_OPEGROUP* pOpeGroup = pConfig->GetGroup(nOperation, nGroup);
	if (pOpeGroup == NULL)
		return;
	POSITION pos = pConfig->FieldFindIndex(pOpeGroup->m_nIndex + nField);
	if (pos == NULL)
		return;
	CInfoField* pField = pConfig->FieldGetNext(pos);
	if (pField == NULL)
		return;

	CProgrptDoc* pDoc = NULL;
	if (pOpeGroup->m_nGroup == CFG_INDEX_MNEMO)	// mnemonics group
		pDoc = (CProgrptDoc*)(m_pView->GetDocument());
	if (pDoc != NULL)
		pField->ItemInsertString(&m_LbItem, &pDoc->m_strMnemo[pField->m_nMajor - 1]);
	else if (pOpeGroup->m_nGroup == CFG_INDEX_ACCUM) {
		CInfoMathe *p = dynamic_cast<CInfoMathe *>(pField);
		if (p)
			p->ItemInsertString(&m_LbItem);
		else
			pField->ItemInsertString(&m_LbItem);
	} else
		pField->ItemInsertString(&m_LbItem);
}

int CFieldDlg::GetOperation()
{
	if (((CButton*)(GetDlgItem(IDC_FIELD_LOOP)))->GetCheck() == 1)
		return 0;

	if (((CButton*)(GetDlgItem(IDC_FIELD_PRINT)))->GetCheck() == 1)
		return 1;

	if (((CButton*)(GetDlgItem(IDC_FIELD_MATHE)))->GetCheck() == 1)
		return 2;

	return -1;
}

void CFieldDlg::SendInsertMessage()
{
	if (m_pView == NULL || m_nFile == -1)
		return;

	m_nOperation = GetOperation();
	m_nGroup     = m_LbGroup.GetCurSel();
	m_nField     = m_LbField.GetCurSel();
	m_nItem      = m_LbItem.GetCurSel();

#ifdef _DEBUG
	TRACE("CFieldDlg: Insert = %d, %d, %d, %d\n",
		  m_nOperation, m_nGroup, m_nField, m_nItem);
#endif

	if (m_pView != NULL && m_pView->GetSafeHwnd() != NULL)
		m_pView->PostMessage(ID_FIELD_DLG, IDC_FIELD_INSERT);
	else
		ASSERT(FALSE);
}

void CFieldDlg::DisableInsertButton()
{
	GetDlgItem(IDC_FIELD_INSERT)->EnableWindow(FALSE);

	if (m_pView != NULL && m_pView->GetSafeHwnd() != NULL)
		m_pView->PostMessage(ID_FIELD_DLG, IDC_FIELD_CANCEL);
//	else
//		ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CFieldDlg message handlers

BOOL CFieldDlg::OnInitDialog()
{
	// default settings in dialog box
	m_nLoop = 0;
	DisableInsertButton();

	CDialog::OnInitDialog();

	RefreshGroup(CFG_INDEX_LOOP);

#if 0
	// use what the resource file has in the dialog template and not what
	// the .ini file may have so far as window placement and size.
	WINDOWPLACEMENT wp = { 0 };
	if (CMainFrame::ReadWindowPlacement(szField, &wp))
	{
		wp.showCmd = SW_SHOW;
		SetWindowPlacement(&wp);
	}
	else
		CenterWindow();
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFieldDlg::OnOK()
{
}

void CFieldDlg::OnCancel()
{
	if (m_pView != NULL && m_pView->GetSafeHwnd() != NULL)
		m_pView->PostMessage(ID_FIELD_DLG, IDC_FIELD_CANCEL);
}

void CFieldDlg::OnFieldInsert()
{
	SendInsertMessage();
}

void CFieldDlg::OnFieldClose()
{
	if (m_pView == NULL)
		ShowDialog(FALSE, NULL, -1);
	else if (m_pView->GetSafeHwnd() != NULL)
//	if (m_pView != NULL && m_pView->GetSafeHwnd() != NULL)
		m_pView->PostMessage(ID_FIELD_DLG, IDC_FIELD_CLOSE);
	else
		ASSERT(FALSE);
}

void CFieldDlg::OnClose()
{
	if (m_pView == NULL)
		ShowDialog(FALSE, NULL, -1);
	else if (m_pView->GetSafeHwnd() != NULL)
//	if (m_pView != NULL && m_pView->GetSafeHwnd() != NULL)
		m_pView->PostMessage(ID_FIELD_DLG, IDC_FIELD_CLOSE);
	else
		ASSERT(FALSE);
}

void CFieldDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CFieldDlg::OnFieldLoop()
{
  	DisableInsertButton();

	RefreshGroup(CFG_INDEX_LOOP);
}

void CFieldDlg::OnFieldPrint()
{
	DisableInsertButton();

	RefreshGroup(CFG_INDEX_PRINT);
}

void CFieldDlg::OnFieldMathe()
{
	DisableInsertButton();

	RefreshGroup(CFG_INDEX_MATHE);
}

void CFieldDlg::OnSelchangeFieldGroup()
{
	DisableInsertButton();

	RefreshField();
}

void CFieldDlg::OnSelchangeFieldField()
{
	DisableInsertButton();

	RefreshItem();

	if (m_LbItem.GetCount() == 0)
		GetDlgItem(IDC_FIELD_INSERT)->EnableWindow(TRUE);
}

void CFieldDlg::OnDblclkFieldField()
{
	if (m_LbItem.GetCount() > 0)
		return;

	SendInsertMessage();
}

void CFieldDlg::OnSelchangeFieldItem()
{
	GetDlgItem(IDC_FIELD_INSERT)->EnableWindow(TRUE);
}

void CFieldDlg::OnDblclkFieldItem()
{
	SendInsertMessage();
}
