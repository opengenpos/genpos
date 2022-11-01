// DLanguageEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DLanguageEdit.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLanguageEdit dialog


CDLanguageEdit::CDLanguageEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDLanguageEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLanguageEdit)
	//}}AFX_DATA_INIT
}


void CDLanguageEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLanguageEdit)
	DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_LanguageSelect);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		Selection = m_LanguageSelect.GetCurSel();
	}
	else {
		((CComboBox *)GetDlgItem (IDC_COMBO_LANGUAGE))->SetCurSel (1);
	}
}


BEGIN_MESSAGE_MAP(CDLanguageEdit, CDialog)
	//{{AFX_MSG_MAP(CDLanguageEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLanguageEdit message handlers

BOOL CDLanguageEdit::OnInitDialog() 
{
	extern 	CodePageTable myCodePageTable [];

	for (int i = 0; myCodePageTable[i].tcCountry ;i++)
	{
		CString lang;
		lang.LoadString(myCodePageTable[i].ulIdsLanguage);
	   ((CComboBox *)GetDlgItem (IDC_COMBO_LANGUAGE))->AddString(lang);
	}
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
