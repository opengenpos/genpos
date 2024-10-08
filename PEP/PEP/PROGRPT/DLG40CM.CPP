/////////////////////////////////////////////////////////////////////////////
// dlg40cm.cpp : implementation file
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
#include "dlg40cm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C40CMDlg dialog


C40CMDlg::C40CMDlg(CProgrptDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(C40CMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C40CMDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
}

void C40CMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C40CMDlg)
	DDX_Control(pDX, IDC_40CM_MNEMO, m_editMnemo);
	DDX_Control(pDX, IDC_40CM_LIST, m_LbList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(C40CMDlg, CDialog)
	//{{AFX_MSG_MAP(C40CMDlg)
	ON_LBN_SELCHANGE(IDC_40CM_LIST, OnSelchange40cmList)
	ON_EN_KILLFOCUS(IDC_40CM_MNEMO, OnKillfocus40cmMnemo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// C40CMDlg message handlers

static wchar_t szFormat[] = L"%d \t%s";

BOOL C40CMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editMnemo.LimitText(C40CM_MAX);

	// set tab stops in list box.
    int nTabs[1] = {40};
	m_LbList.SetTabStops(1, nTabs);

	for (int nIndex = 0; nIndex < MNEMO_MAX_40CM; nIndex++)
	{
		POSITION pos = m_pDoc->m_strMnemo[MNEMO_INDEX_40CM].FindIndex(nIndex);
		CString strMnemo;
		if (pos != NULL)
			strMnemo = m_pDoc->m_strMnemo[MNEMO_INDEX_40CM].GetAt(pos);
		else
			strMnemo = L"";
		wchar_t szData[64];
		wsprintf(szData, szFormat, nIndex + 1, strMnemo);
		m_LbList.InsertString(-1, szData);
	}
	m_LbList.SetCurSel(0);

	OnSelchange40cmList();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void C40CMDlg::OnOK()
{
	OnKillfocus40cmMnemo();

	m_pDoc->m_strMnemo[MNEMO_INDEX_40CM].RemoveAll();
	POSITION pos = m_pDoc->m_strMnemo[MNEMO_INDEX_40CM].GetHeadPosition();
	for (int nIndex = 0; nIndex < MNEMO_MAX_40CM; nIndex++)
	{
		wchar_t szData[64];
		m_LbList.GetText(nIndex, szData);
		CString strMnemo(wcschr(szData, L'\t') + 1);
		m_pDoc->m_strMnemo[MNEMO_INDEX_40CM].InsertAfter(pos, strMnemo);
	}
	
	CDialog::OnOK();
}

void C40CMDlg::OnSelchange40cmList()
{
	wchar_t szData[64];
	int nIndex = m_LbList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		m_LbList.GetText(nIndex, szData);
		const wchar_t * pszMnemo = wcschr(szData, L'\t') + 1;
		m_editMnemo.SetWindowText(pszMnemo);
	}
	else
		ASSERT(FALSE);
}

void C40CMDlg::OnKillfocus40cmMnemo()
{
	CString strMnemo;
	m_editMnemo.GetWindowText(strMnemo);

	wchar_t szData[64];
	int nIndex = m_LbList.GetCurSel();
	if (nIndex != LB_ERR)
	{
		wsprintf(szData, szFormat, nIndex + 1, strMnemo);
		m_LbList.DeleteString(nIndex);
		m_LbList.InsertString(nIndex, szData);
		m_LbList.SetCurSel(nIndex);
	}
	else
		ASSERT(FALSE);
}
