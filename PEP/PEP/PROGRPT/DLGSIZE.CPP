// dlgsize.cpp : implementation file
//

#include "stdafx.h"
#include "progrpt.h"

#include "dlgsize.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeDlg dialog


CSizeDlg::CSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSizeDlg)
	m_bDisp = FALSE;
	m_strMessage = "";
	//}}AFX_DATA_INIT
}

void CSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSizeDlg)
	DDX_Control(pDX, IDC_SIZE_ICON, m_Icon);
	DDX_Check(pDX, IDC_SIZE_DISP, m_bDisp);
	DDX_Text(pDX, IDC_SIZE_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CSizeDlg)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSizeDlg message handlers

BOOL CSizeDlg::OnInitDialog()
{
	m_strMessage.LoadString(IDS_ERRMSG_SIZE);

	CDialog::OnInitDialog();

	CMenu* pMenu = GetSystemMenu(FALSE);
	if (pMenu != NULL)
		pMenu->DeleteMenu(SC_CLOSE, MF_BYCOMMAND);

	m_Icon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

	CenterWindow();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSizeDlg::OnYes()
{
	UpdateData(TRUE);
	EndDialog(IDYES);
}

void CSizeDlg::OnNo()
{
	UpdateData(TRUE);
	EndDialog(IDNO);
}

void CSizeDlg::OnCancel()
{
}
