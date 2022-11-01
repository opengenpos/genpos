/////////////////////////////////////////////////////////////////////////////
// dlgtitle.cpp : implementation file
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

#include "dlgtitle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleDlg dialog


CTitleDlg::CTitleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTitleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTitleDlg)
	m_strTitle = "";
	//}}AFX_DATA_INIT
}

void CTitleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTitleDlg)
	DDX_Text(pDX, IDC_TITLE_TITLE, m_strTitle);
	DDV_MaxChars(pDX, m_strTitle, 32);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTitleDlg, CDialog)
	//{{AFX_MSG_MAP(CTitleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTitleDlg message handlers

BOOL CTitleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	((CEdit*)GetDlgItem(IDC_TITLE_TITLE))->LimitText(32);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
