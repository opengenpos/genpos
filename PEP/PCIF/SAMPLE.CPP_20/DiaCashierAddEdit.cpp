// DiaCashierAddEdit.cpp : implementation file
//

#include "stdafx.h"

#include "DiaCashierAddEdit.h"

#include "DiaCashierEdit.h"


// CDiaCashierAddEdit dialog

IMPLEMENT_DYNAMIC(CDiaCashierAddEdit, CDialog)

CDiaCashierAddEdit::CDiaCashierAddEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDiaCashierAddEdit::IDD, pParent), m_sActualRead(0)
{

}

CDiaCashierAddEdit::~CDiaCashierAddEdit()
{
}

void CDiaCashierAddEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate) {
		DDX_Control(pDX, IDC_CASHIER_LIST, m_CashierListBox);
	} else {
		DDX_Control(pDX, IDC_CASHIER_LIST, m_CashierListBox);

		ULONG  ausCashierOnTerminal[ MAX_TERMINAL_NO ] = {0};
		SHORT   sResult;

		// --- get all of terminal# which is signed in by cashier ---
		sResult = ::SerCasChkSignIn( &ausCashierOnTerminal[ 0 ] );

		// WARNING: The following loop assumes that the listbox control does not have LB_SORT
		//          style which will sort the strings added to the listbox. Should the
		//          listbox control have LB_SORT style then the strings added could be
		//          out of order of the m_paraCashier[] array items.
		for (int i = 0; i < m_sActualRead; i++) {
			CString csListItem;
			CHAR    aSignedIn = _T(' ');
			for (int j = 0; j < MAX_TERMINAL_NO; j++) {
				if (ausCashierOnTerminal[j] == m_paraCashier[i].paraCashier.ulCashierNo) {
					aSignedIn = _T('X');
					break;
				}
			}
			csListItem.Format (_T(" %c   %8.8d    %-20.20s"), aSignedIn, m_paraCashier[i].paraCashier.ulCashierNo, m_paraCashier[i].paraCashier.auchCashierName);
			m_CashierListBox.AddString (csListItem);
		}


	}
}


BEGIN_MESSAGE_MAP(CDiaCashierAddEdit, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_SELECTED, &CDiaCashierAddEdit::OnBnClickedButtonEditSelected)
	ON_BN_CLICKED(IDC_BUTTON_SEND_CHANGES, &CDiaCashierAddEdit::OnBnClickedButtonSendChanges)
END_MESSAGE_MAP()


// CDiaCashierAddEdit message handlers

SHORT CDiaCashierAddEdit::LoadListControl (void)
{
	ULONG   ulCashierNoList[300];

	m_sActualRead = ::SerCasAllIdRead( sizeof(ulCashierNoList), ulCashierNoList );

	for (int i = 0; i < m_sActualRead; i++) {
		memset (&m_paraCashier[i], 0, sizeof(m_paraCashier[0]));
		m_paraCashier[i].bModified = FALSE;
		m_paraCashier[i].paraCashier.ulCashierNo = ulCashierNoList[i];
		m_paraCashier[i].sLastError = ::SerCasIndRead( &(m_paraCashier[i].paraCashier) );
	}
	return m_sActualRead;
}

void CDiaCashierAddEdit::OnBnClickedButtonEditSelected()
{
	int   iIndex = m_CashierListBox.GetCurSel ();

	CDiaCashierEdit  myEdit;
	myEdit.ImportCasif (&(m_paraCashier[iIndex].paraCashier));
	if (myEdit.DoModal () == IDOK) {
		m_paraCashier[iIndex].bModified = TRUE;
		m_paraCashier[iIndex].paraCashier = myEdit.m_paraCashier;
	}
}

void CDiaCashierAddEdit::OnBnClickedButtonSendChanges()
{
	for (int i = 0; i < m_sActualRead; i++) {
		if (m_paraCashier[i].bModified) {
			m_paraCashier[i].sLastError = ::SerCasAssign( &(m_paraCashier[i].paraCashier) );
			if (m_paraCashier[i].sLastError == 0) {
				m_paraCashier[i].bModified = FALSE;
			}
		}
	}
}



// CDiaCashierSelect dialog

IMPLEMENT_DYNAMIC(CDiaCashierSelect, CDialog)

CDiaCashierSelect::CDiaCashierSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDiaCashierSelect::IDD, pParent), m_sActualRead(0), m_ulCashierNo(0)
{

}

CDiaCashierSelect::~CDiaCashierSelect()
{
}

void CDiaCashierSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate) {
		DDX_Control(pDX, IDC_CASHIER_LIST, m_CashierListBox);
	}
	else {
		DDX_Control(pDX, IDC_CASHIER_LIST, m_CashierListBox);

		ULONG  ausCashierOnTerminal[MAX_TERMINAL_NO] = { 0 };
		SHORT   sResult;

		// --- get all of terminal# which is signed in by cashier ---
		sResult = ::SerCasChkSignIn(&ausCashierOnTerminal[0]);

		// WARNING: The following loop assumes that the listbox control does not have LB_SORT
		//          style which will sort the strings added to the listbox. Should the
		//          listbox control have LB_SORT style then the strings added could be
		//          out of order of the m_paraCashier[] array items.
		for (int i = 0; i < m_sActualRead; i++) {
			CString csListItem;
			CHAR    aSignedIn = _T(' ');
			for (int j = 0; j < MAX_TERMINAL_NO; j++) {
				if (ausCashierOnTerminal[j] == m_paraCashier[i].paraCashier.ulCashierNo) {
					aSignedIn = _T('X');
					break;
				}
			}
			csListItem.Format(_T(" %c   %8.8d    %-20.20s"), aSignedIn, m_paraCashier[i].paraCashier.ulCashierNo, m_paraCashier[i].paraCashier.auchCashierName);
			m_CashierListBox.AddString(csListItem);
		}
	}
}


BEGIN_MESSAGE_MAP(CDiaCashierSelect, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_SELECTED, &CDiaCashierSelect::OnBnClickedButtonEditSelected)
END_MESSAGE_MAP()


// CDiaCashierSelect message handlers

SHORT CDiaCashierSelect::LoadListControl(void)
{
	ULONG   ulCashierNoList[300] = { 0 };

	m_sActualRead = ::SerCasAllIdRead(sizeof(ulCashierNoList), ulCashierNoList);

	for (int i = 0; i < m_sActualRead; i++) {
		memset(&m_paraCashier[i], 0, sizeof(m_paraCashier[0]));
		m_paraCashier[i].bModified = FALSE;
		m_paraCashier[i].paraCashier.ulCashierNo = ulCashierNoList[i];
		m_paraCashier[i].sLastError = ::SerCasIndRead(&(m_paraCashier[i].paraCashier));
	}
	return m_sActualRead;
}

void CDiaCashierSelect::OnBnClickedButtonEditSelected()
{
	int   iIndex = m_CashierListBox.GetCurSel();

	if (iIndex >= 0) m_ulCashierNo = m_paraCashier[iIndex].paraCashier.ulCashierNo;

	CDialog::OnOK();
}


