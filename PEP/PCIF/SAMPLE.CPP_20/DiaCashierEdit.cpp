// DiaCashierEdit.cpp : implementation file
//

#include "stdafx.h"
#include "DiaCashierEdit.h"

int CEditInt::GetWindowTextAsInt (void)
{
	CString csJ;
	GetWindowText (csJ);
	return _ttoi (csJ);
}

void CEditInt::SetWindowTextAsInt (int iValue)
{
	CString csJ;
	csJ.Format (_T("%d"), iValue);
	SetWindowText (csJ);
}

/*
 *  InsertItemDialogTemplate ()
 *
 *  Insert into a tab control a tab pane based on the specified dialog template.  The
 *  dialog template describes what the tab pane looks like so far as controls, etc.
 *
 *  NOTE: The STYLE description must be WS_CHILD and not WS_POPUP.  Also the dialog
 *        needs to have as its top coordinate some distance in pixels so that the
 *        various tab descriptions are visible.  For instance an example dialog
 *        template in the resource file may look like:
 *            IDD_CASHIER_TAB_ONE DIALOGEX 0, 10, 178, 113
 *            STYLE DS_SETFONT | DS_FIXEDSYS | WS_CHILD
 *            FONT 8, "MS Shell Dlg", 400, 0, 0x1
 *            BEGIN
 *                LTEXT           "Dialog Tab one",IDC_STATIC,6,44,90,17
 *            END
 *
**/
CTabCtrlDialog::CTabCtrlDialog() {
	for (int i = 0; i < 10; i++) {
		m_pDialogData[i].pDialog = 0;
	}
}

CTabCtrlDialog::~CTabCtrlDialog() {
	for (int i = 0; i < 10; i++) {
		if (m_pDialogData[i].pDialog) delete m_pDialogData[i].pDialog;
	}
}


void CTabCtrlDialog::InsertItemDialogTemplate (UINT nIDTemplate, int nItem, TCITEM* pTabCtrlItem)
{
	InsertItem (nItem, pTabCtrlItem);
	m_pDialogData[nItem].nIDTemplate = nIDTemplate;
	m_pDialogData[nItem].pDialog = new CDialog ();
	if (m_pDialogData[nItem].pDialog) {
		m_pDialogData[nItem].pDialog->Create (nIDTemplate, this);
		m_pDialogData[nItem].pDialog->ShowWindow (FALSE);
	}
}


// CDiaCashierEdit dialog

IMPLEMENT_DYNAMIC(CDiaCashierEdit, CDialog)

const CDiaCashierEdit::TabToStatus CDiaCashierEdit::m_TabItemOneStatus[] = {
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST01, 0, CAS_TRAINING_CASHIER },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST02, 0, CAS_NOT_GUEST_CHECK_OPEN },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST03, 0, CAS_NOT_NON_GUEST_CHECK },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST04, 0, CAS_NOT_CLOSE_OWN_CHECK },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST05, 0, CAS_NOT_CLOSE_ANY_CHECK },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST06, 0, CAS_NOT_TRANSFER_FROM },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST07, 0, CAS_NOT_TRANSFER_TO},
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST08, 1, CAS_SECRET_CODE },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST09, 1, CAS_FINALIZE_GCF },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST10, 1, CAS_FINALIZE_NON_GCF },
		{ 1, IDC_TAB_CASHIER_EDIT_A20ST11, 1, CAS_NOT_SURROGATE },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST01, 1, 0x01 },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST02, 1, 0x01 },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST03, 1, CAS_USE_TEAM },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST04, 1, CAS_NOT_CAS_INT },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST05, 2, CAS_OPEN_CASH_DRAWER_SIGN_IN },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST06, 2, CAS_OPEN_CASH_DRAWER_SIGN_OUT },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST07, 2, CAS_MSR_MANUAL_SIGN_IN },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST08, 2, CAS_AUTO_SIGN_OUT_DISABLED },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST09, 2, CAS_PIN_REQIRED_FOR_SIGNIN },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST10, 2, CAS_PIN_REQIRED_FOR_TIMEIN },
		{ 2, IDC_TAB_CASHIER_EDIT_A20ST10, 3, CAS_PIN_REQIRED_FOR_SUPR }
	};

CDiaCashierEdit::CDiaCashierEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDiaCashierEdit::IDD, pParent)
{
	m_TabItemOne.mask = TCIF_TEXT;
	m_TabItemOne.pszText = _T("Tab 1");
	m_TabItemTwo.mask = TCIF_TEXT;
	m_TabItemTwo.pszText = _T("Tab 2");
	m_TabItemThree.mask = TCIF_TEXT;
	m_TabItemThree.pszText = _T("Tab 3");
}

CDiaCashierEdit::~CDiaCashierEdit()
{
}

void CDiaCashierEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CASHIER_NAME, m_CashierName);
	DDX_Control(pDX, IDC_EDIT_CASHIER_SUPNO, m_SupervisorId);
	DDX_Control(pDX, IDC_EDIT_CASHIER_TEAMNO, m_TeamNumber);
	DDX_Control(pDX, IDC_EDIT_CASHIER_GCSTART, m_GuestCheckStart);
	DDX_Control(pDX, IDC_EDIT_CASHIER_GCEND, m_GuestCheckEnd);
	DDX_Control(pDX, IDC_TAB_CASHIER_EDIT_STATUS, m_TabCtrl);
	if (pDX->m_bSaveAndValidate) {
		m_CashierName.GetWindowText (m_paraCashier.auchCashierName, 20);
		m_paraCashier.usSupervisorID = m_SupervisorId.GetWindowTextAsInt();
		m_paraCashier.uchTeamNo = m_TeamNumber.GetWindowTextAsInt();
		m_paraCashier.usGstCheckStartNo = m_GuestCheckStart.GetWindowTextAsInt();
		m_paraCashier.usGstCheckEndNo = m_GuestCheckEnd.GetWindowTextAsInt();
		for (int i = 0; i < sizeof(m_TabItemOneStatus)/sizeof(m_TabItemOneStatus[0]); i++) {
			int iTab = m_TabItemOneStatus[i].sTabItem;
			int iDlg = m_TabItemOneStatus[i].iDlgItem;
			int iOffset = m_TabItemOneStatus[i].sOffset;
			CButton *p = (CButton *) m_TabCtrl.m_pDialogData[iTab].pDialog->GetDlgItem(iDlg);
			if (p->GetCheck()) {
				m_paraCashier.fbCashierStatus[iOffset] |= m_TabItemOneStatus[i].uchBit;
			} else {
				m_paraCashier.fbCashierStatus[iOffset] &= ~(m_TabItemOneStatus[i].uchBit);
			}
		}
	} else {
		m_CashierName.SetWindowText(m_paraCashier.auchCashierName);
		m_SupervisorId.SetWindowTextAsInt (m_paraCashier.usSupervisorID);
		m_TeamNumber.SetWindowTextAsInt (m_paraCashier.uchTeamNo);
		m_GuestCheckStart.SetWindowTextAsInt (m_paraCashier.usGstCheckStartNo);
		m_GuestCheckEnd.SetWindowTextAsInt (m_paraCashier.usGstCheckEndNo);
		m_TabCtrl.InsertItemDialogTemplate (IDD_CASHIER_TAB_ONE, 1, &m_TabItemOne);
		m_TabCtrl.InsertItemDialogTemplate (IDD_CASHIER_TAB_TWO, 2, &m_TabItemTwo);
		m_TabCtrl.InsertItemDialogTemplate (IDD_CASHIER_TAB_THREE, 3, &m_TabItemThree);
		for (int i = 0; i < sizeof(m_TabItemOneStatus)/sizeof(m_TabItemOneStatus[0]); i++) {
			int iTab = m_TabItemOneStatus[i].sTabItem;
			int iDlg = m_TabItemOneStatus[i].iDlgItem;
			int iOffset = m_TabItemOneStatus[i].sOffset;
			CButton *p = (CButton *) m_TabCtrl.m_pDialogData[iTab].pDialog->GetDlgItem(iDlg);
			if (m_paraCashier.fbCashierStatus[iOffset] & m_TabItemOneStatus[i].uchBit) {
				p->SetCheck (1);
			} else {
				p->SetCheck (0);
			}
		}
		m_TabCtrl.m_pDialogData[1].pDialog->ShowWindow (TRUE);
	}
}


BEGIN_MESSAGE_MAP(CDiaCashierEdit, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CASHIER_EDIT_STATUS, &CDiaCashierEdit::OnTcnSelchangeTabCashierEditStatus)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_CASHIER_EDIT_STATUS, &CDiaCashierEdit::OnTcnSelchangingTabCashierEditStatus)
END_MESSAGE_MAP()


// CDiaCashierEdit message handlers

BOOL  CDiaCashierEdit::ImportCasif (CASIF *pCashier)
{
	m_paraCashier = *pCashier;

	return TRUE;
}

BOOL  CDiaCashierEdit::ExportCasif (CASIF *pCashier)
{
	*pCashier = m_paraCashier;

	return TRUE;
}

void CDiaCashierEdit::OnTcnSelchangeTabCashierEditStatus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int i = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
	m_TabCtrl.m_pDialogData[i + 1].pDialog->ShowWindow (TRUE);

}

void CDiaCashierEdit::OnTcnSelchangingTabCashierEditStatus(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int i = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
	m_TabCtrl.m_pDialogData[i + 1].pDialog->ShowWindow (FALSE);

}
