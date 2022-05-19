// DialogOptionValue.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "DialogOptionValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogOptionValue dialog


CDialogOptionValue::CDialogOptionValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogOptionValue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogOptionValue)
	m_newValue = _T("");
	//}}AFX_DATA_INIT
}


void CDialogOptionValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOptionValue)
	DDX_Text(pDX, IDC_EDIT_OPTIONS, m_newValue);
	DDV_MaxChars(pDX, m_newValue, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOptionValue, CDialog)
	//{{AFX_MSG_MAP(CDialogOptionValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOptionValue message handlers

BOOL CDialogOptionValue::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
