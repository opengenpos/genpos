/////////////////////////////////////////////////////////////////////////////
// dlgloop.cpp : implementation file
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

#include "item.h"
#include "dlgloop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoopPropDlg dialog


CLoopPropDlg::CLoopPropDlg(int nFile, CLoopItem* pItem, CWnd* pParent /*=NULL*/)
	: CDialog(CLoopPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoopPropDlg)
	m_strGroup = "";
	m_strMajor = "";
	//}}AFX_DATA_INIT

	m_nFile = nFile;
	m_pItem = pItem;
}

void CLoopPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoopPropDlg)
	DDX_Control(pDX, IDC_LOOP_COND, m_cbCond);
	DDX_Text(pDX, IDC_LOOP_GROUP, m_strGroup);
	DDX_Text(pDX, IDC_LOOP_MAJOR, m_strMajor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoopPropDlg, CDialog)
	//{{AFX_MSG_MAP(CLoopPropDlg)
	ON_CBN_SELCHANGE(IDC_LOOP_COND, OnSelchangeLoopCond)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLoopPropDlg message handlers

BOOL CLoopPropDlg::OnInitDialog()
{
	CInfoConfig* pConfig = ptheInfo->GetConfig(m_nFile);
	CInfoLoop* pLoop = CInfoLoop::GetField(m_nFile,
										   m_pItem->Element[0].m_nGroup,
										   m_pItem->Element[0].m_nField[0]);

	m_strGroup = pConfig->GetGroupName(CFG_INDEX_LOOP,
								       m_pItem->Element[0].m_nGroup);
	m_strMajor = pLoop->m_strName;
	
	CDialog::OnInitDialog();

	for (int nCount = 0; nCount < LOOP_NUM_COND; nCount++)
		if (pLoop->Condition[nCount].m_nNumber >= 0)
			m_cbCond.InsertString(-1, pLoop->Condition[nCount].m_strName);
	m_cbCond.SetCurSel(m_pItem->m_nCondition);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLoopPropDlg::OnOK()
{
	int nCond = m_cbCond.GetCurSel();
	if (nCond != CB_ERR)
		m_pItem->m_nCondition = nCond;
	else
		ASSERT(FALSE);
	
	CDialog::OnOK();
}

void CLoopPropDlg::OnSelchangeLoopCond()
{
	// TODO: Add your control notification handler code here
	
}
