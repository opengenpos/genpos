// MessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "framework.h"
#include "MessageDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDialog dialog


CMessageDialog::CMessageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDialog)
	DDX_Control(pDX, IDC_EDIT_DIALOG_MESSAGE, m_DisplayText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDialog, CDialog)
	//{{AFX_MSG_MAP(CMessageDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDialog message handlers

BOOL CMessageDialog::CreateMe (CWnd * pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CMessageDialog::SetTextAndShow (TCHAR *lParam)
{
	ShowWindow(SW_SHOW);
	BringWindowToTop();

	m_DisplayText.SetWindowText (lParam);

	UpdateData (FALSE);

	return TRUE;
}

BOOL CMessageDialog::UnshowAndRemove ()
{
	// TODO: Add extra validation here
	ShowWindow(SW_HIDE);

	return TRUE;
}


void CMessageDialog::OnOK()
{
	// TODO: Add extra validation here
	ShowWindow(SW_HIDE);
	
	CDialog::OnOK();
}

void CMessageDialog::OnCancel() 
{
	// TODO: Add extra cleanup here

	ShowWindow(SW_HIDE);
	
	CDialog::OnCancel();
}
