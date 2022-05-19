// SecretNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "SecretNumberDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecretNumberDlg dialog


CSecretNumberDlg::CSecretNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSecretNumberDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSecretNumberDlg)
	m_strSecretNumber = _T("");
	m_bPasswdStatus	  =	FALSE;
	//}}AFX_DATA_INIT
}


void CSecretNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecretNumberDlg)
	DDX_Control(pDX, IDC_SECRETNUM_SECRETNUM_EDIT, m_editSecretNumber);
	DDX_Text(pDX, IDC_SECRETNUM_SECRETNUM_EDIT, m_strSecretNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSecretNumberDlg, CDialog)
	//{{AFX_MSG_MAP(CSecretNumberDlg)
	ON_BN_CLICKED(IDOK, ON_IDOK)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON0, &CSecretNumberDlg::OnBnClickedButton0)
	ON_BN_CLICKED(IDC_BUTTON1, &CSecretNumberDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSecretNumberDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSecretNumberDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CSecretNumberDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CSecretNumberDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CSecretNumberDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CSecretNumberDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CSecretNumberDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CSecretNumberDlg::OnBnClickedButton9)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecretNumberDlg message handlers

void CSecretNumberDlg::ON_IDOK() 
{
	CString m_strMsg;

	UpdateData();
	
	if(CheckSecretNo(m_strSecretNumber) == INIRST_ENTERSECRETNO_ERR)
	{
		/* ---- Error Occured ---- */
		m_strMsg.LoadString(IDS_ENTSECNO_ERR_MSG);
		AfxMessageBox(m_strMsg,IDOK);
	}
	else
	{
		/* ----- Load Reset Dialog ---- */
		m_bPasswdStatus = TRUE;
		CDialog::OnOK();
	}
	
}


int CSecretNumberDlg::CheckSecretNo(const CString strKeyCode)
{
	int m_nStatus = INIRST_ENTERSECRETNO_ERR;

	for(int nIndex=0; nIndex < strKeyCode.GetLength(); nIndex++)
	{
		m_PassWd.PushChar((const char)strKeyCode.GetAt(nIndex));
	}

	if(m_PassWd.CheckInputStringSize() && m_PassWd.Check() )
	{
		/* ---- Password Succeed ---- */
		m_nStatus = INIRST_SELECTION;
	}
	else
	{
		/* ---- Password Error ---- */
		m_nStatus = INIRST_ENTERSECRETNO_ERR;
		m_PassWd.ClearInputBuff();
		CEdit   *pclEditBox = (CEdit *)GetDlgItem(IDC_SECRETNUM_SECRETNUM_EDIT);
		pclEditBox->SetWindowText(L"");
	}

	return	m_nStatus;
}

BOOL CSecretNumberDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_PassWd.Generate();
	m_PassWd.ClearInputBuff();
	
	return TRUE;
}

void CSecretNumberDlg::HandlePinPadEntry (const TCHAR  tcsChar)
{
	TCHAR mytcsCharStr[2] = {0};
	mytcsCharStr[0] = tcsChar;
	CEdit   *pclEditBox = (CEdit *)GetDlgItem (IDC_SECRETNUM_SECRETNUM_EDIT);
	CString  myText;
	pclEditBox->GetWindowText (myText);
	myText.Append (mytcsCharStr);
	pclEditBox->SetWindowText (myText);

}

void CSecretNumberDlg::OnBnClickedButton0()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('0'));
}
void CSecretNumberDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('1'));
}
void CSecretNumberDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('2'));
}
void CSecretNumberDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('3'));
}
void CSecretNumberDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('4'));
}
void CSecretNumberDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('5'));
}
void CSecretNumberDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('6'));
}
void CSecretNumberDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('7'));
}
void CSecretNumberDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('8'));
}
void CSecretNumberDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	HandlePinPadEntry (_T('9'));
}
