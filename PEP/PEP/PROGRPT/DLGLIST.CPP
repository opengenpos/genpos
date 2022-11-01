/////////////////////////////////////////////////////////////////////////////
// dlglist.cpp : implementation file
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
#include "dlglist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListPropDlg dialog


CListPropDlg::CListPropDlg(CProgrptDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CListPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListPropDlg)
	m_strTitle = "";
	//}}AFX_DATA_INIT

	m_nFile   = -1;
	m_nDevice = -1;
	m_pDoc    = pDoc;
}

void CListPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListPropDlg)
	DDX_Control(pDX, IDC_LIST_PROP_FILE, m_CbFile);
	DDX_Control(pDX, IDC_LIST_PROP_DEVICE, m_CbDevice);
	DDX_Text(pDX, IDC_LIST_PROP_TITLE, m_strTitle);
	DDV_MaxChars(pDX, m_strTitle, 32);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CListPropDlg, CDialog)
	//{{AFX_MSG_MAP(CListPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListPropDlg message handlers

BOOL CListPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// insert file information on combo box
	POSITION pos = ptheInfo->m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)ptheInfo->m_File.GetNext(pos);
		if (pFile->m_strOption.IsEmpty())
			m_CbFile.InsertString(-1, pFile->m_strName);
	}
	m_CbFile.SetCurSel(0);

	// insert device information on combo box
	pos = ptheInfo->m_Device.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_DEVICE* pDevice = (INFO_DEVICE*)ptheInfo->m_Device.GetNext(pos);
		if (pDevice->m_strOption.IsEmpty())
			m_CbDevice.InsertString(-1, pDevice->m_strName);
	}
	m_CbDevice.SetCurSel(0);

	((CEdit*)GetDlgItem(IDC_LIST_PROP_TITLE))->LimitText(32);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CListPropDlg::OnOK()
{
	m_nFile = m_CbFile.GetCurSel();

	int nIndex = m_CbDevice.GetCurSel();
	POSITION pos = ptheInfo->m_Device.FindIndex(nIndex);
	ASSERT(pos != NULL);
	INFO_DEVICE* pDevice = (INFO_DEVICE*)ptheInfo->m_Device.GetAt(pos);
	ASSERT(pDevice != NULL);
	m_nDevice = pDevice->m_nNumber;

	if (! m_pDoc->IsNewReport(m_nFile, m_nDevice))
	{
		AfxMessageBox(IDS_ERRMSG_LCD);
		return;
	}

	CDialog::OnOK();
}
