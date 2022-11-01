// PluAssignUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "pcsample.h"

#include "R20_PC2172.h"

#include "PluAssignUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
*===========================================================================
** Synopsis:    VOID  RflConv3bto4b( ULONG *ulPrice, UCHAR *auchPrice )
*               
*     Input:    *ulPrice    : Destination Price Save Area 
*               *auchPrice  : Pointer of Source Price Save Area  
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Convert 3 Byte Data to ULONG Type Data 
*===========================================================================
*/
VOID RflConv3bto4b( ULONG *ulPrice, UCHAR *auchPrice )
{
    *ulPrice = ( ULONG)(*auchPrice) + 256L*( ULONG)(*(auchPrice + 1)) + 65536L*( ULONG)(*(auchPrice + 2));
}

/*
*===========================================================================
** Synopsis:    VOID  RflConv4bto3b( UCHAR *auchPrice, ULONG ulPrice )
*               
*     Input:    ulPrice     : Source Price  
*               *auchPrice  : Pointer of Destination Price Save Area  
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Convert ULONG Type Data to 3 Byte Data 
*===========================================================================
*/
VOID RflConv4bto3b( UCHAR *auchPrice, ULONG ulPrice )
{
    *(auchPrice + 2) = ( UCHAR)(ulPrice/65536L);
    *(auchPrice + 1) = ( UCHAR)((ulPrice - 65536L*( ULONG)(*(auchPrice + 2)))/256L);
    *auchPrice = ( UCHAR)(ulPrice - 65536L*( ULONG)(*(auchPrice + 2)) - 256L*( ULONG)(*(auchPrice + 1)));                                                               
}

/////////////////////////////////////////////////////////////////////////////
// CPluAssignUpdate dialog


CPluAssignUpdate::CPluAssignUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CPluAssignUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluAssignUpdate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPluAssignUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluAssignUpdate)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPluAssignUpdate, CDialog)
	//{{AFX_MSG_MAP(CPluAssignUpdate)
	ON_BN_CLICKED(IDC_BUTTON_FETCHPLU, OnButtonFetchplu)
	ON_BN_CLICKED(IDC_BUTTON_UPDATEPLU, OnButtonUpdateplu)
	ON_EN_CHANGE(IDC_EDIT_PLUNUMBER, OnChangeEditPlunumber)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DELETEPLU, OnBnClickedButtonDeleteplu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluAssignUpdate message handlers

BOOL CPluAssignUpdate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit    *pEdit;
	CButton  *pButton;

	memset (&m_PluIf, 0, sizeof(m_PluIf));

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLUNUMBER);
	pEdit->EnableWindow (TRUE);
	pEdit->SetLimitText (12);

	pButton = (CButton *)GetDlgItem (IDC_BUTTON_UPDATEPLU);
	pButton->EnableWindow (FALSE);
	pButton = (CButton *)GetDlgItem (IDC_BUTTON_DELETEPLU);
	pButton->EnableWindow (FALSE);

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_MNEMONIC);
	pEdit->EnableWindow (FALSE);
	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_GROUPNUMBER);
	pEdit->EnableWindow (FALSE);
	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_TABLENUMBER);
	pEdit->EnableWindow (FALSE);
	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_PRICE);
	pEdit->EnableWindow (FALSE);
	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_DEPTNUMBER);
	pEdit->EnableWindow (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPluAssignUpdate::OnButtonFetchplu() 
{
	TCHAR    tcsEditLine [64];
	ULONG    ulNumber;
	CEdit    *pEdit;
	CButton  *pButton;
	int      i, j;


	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLUNUMBER);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);

	memset (&m_PluIf, 0, sizeof(m_PluIf));
	for (i = 0; i < 12; i++) {
		m_PluIf.auchPluNo[i] = _T('0');
	}

	j = _tcslen(tcsEditLine);

	for (j--, i = 11; i >= 0 && j >= 0; i--, j--) {
		m_PluIf.auchPluNo[i] = tcsEditLine[j];
	}

	m_PluIf.uchPluAdj = 1;   // default to the main item price

	short sRetStatus = 0;

	sRetStatus = CliOpPluIndRead (&m_PluIf, m_usLockHnd);

	if (sRetStatus == 0) {
		pButton = (CButton *)GetDlgItem (IDC_BUTTON_UPDATEPLU);
		pButton->EnableWindow (TRUE);

		pButton = (CButton *)GetDlgItem (IDC_BUTTON_DELETEPLU);
		pButton->EnableWindow (TRUE);

		pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_MNEMONIC);
		pEdit->EnableWindow (TRUE);
		memset (tcsEditLine, 0, sizeof(tcsEditLine));
		memcpy (tcsEditLine, m_PluIf.ParaPlu.auchPluName, sizeof(m_PluIf.ParaPlu.auchPluName));
		pEdit->SetWindowText (tcsEditLine);

		pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_GROUPNUMBER);
		pEdit->EnableWindow (TRUE);
		_stprintf (tcsEditLine, _T("%d"), m_PluIf.ParaPlu.uchGroupNumber);
		pEdit->SetWindowText (tcsEditLine);

		pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_TABLENUMBER);
		pEdit->EnableWindow (TRUE);
		_stprintf (tcsEditLine, _T("%d"), m_PluIf.ParaPlu.uchTableNumber);
		pEdit->SetWindowText (tcsEditLine);

		pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_DEPTNUMBER);
		pEdit->EnableWindow (TRUE);
		_stprintf (tcsEditLine, _T("%d"), m_PluIf.ParaPlu.ContPlu.usDept);
		pEdit->SetWindowText (tcsEditLine);

		pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_PRICE);
		pEdit->EnableWindow (TRUE);
		RflConv3bto4b(&ulNumber, m_PluIf.ParaPlu.auchPrice);
		_stprintf (tcsEditLine, _T("%d"), ulNumber);
		pEdit->SetWindowText (tcsEditLine);
	}
	else {
		CString csMsg;

		csMsg.Format (_T("Error retrieve PLU %d"), sRetStatus);
		AfxMessageBox (csMsg);
	}
}

void CPluAssignUpdate::OnButtonUpdateplu() 
{
	TCHAR    tcsEditLine [64];
	ULONG    ulNumber;
	CEdit    *pEdit;

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_MNEMONIC);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_GROUPNUMBER);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);
	ulNumber = _ttol (tcsEditLine);
	m_PluIf.ParaPlu.uchGroupNumber = (UCHAR)ulNumber;

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_TABLENUMBER);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);
	ulNumber = _ttol (tcsEditLine);
	m_PluIf.ParaPlu.uchTableNumber = (UCHAR)ulNumber;

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_DEPTNUMBER);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);
	ulNumber = _ttol (tcsEditLine);
	m_PluIf.ParaPlu.ContPlu.usDept = (UCHAR)ulNumber;

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLU_PRICE);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);
	ulNumber = _ttol (tcsEditLine);
	RflConv4bto3b(m_PluIf.ParaPlu.auchPrice, ulNumber);

	SerOpUpdateExistingPlu (&m_PluIf, m_usLockHnd);
}

void CPluAssignUpdate::OnChangeEditPlunumber() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CButton  *pButton;

	pButton = (CButton *)GetDlgItem (IDC_BUTTON_UPDATEPLU);
	pButton->EnableWindow (TRUE);
}

void CPluAssignUpdate::OnBnClickedButtonDeleteplu()
{
	TCHAR    tcsEditLine [64];
	CEdit    *pEdit;
	CButton  *pButton;
	int      i, j;

	pEdit = (CEdit *)GetDlgItem (IDC_EDIT_PLUNUMBER);
	memset (tcsEditLine, 0, sizeof(tcsEditLine));
	pEdit->GetLine (0, tcsEditLine, sizeof(tcsEditLine) - 2);

	memset (&m_PluIf, 0, sizeof(m_PluIf));
	for (i = 0; i < 12; i++) {
		m_PluIf.auchPluNo[i] = _T('0');
	}

	j = _tcslen(tcsEditLine);

	for (j--, i = 11; i >= 0 && j >= 0; i--, j--) {
		m_PluIf.auchPluNo[i] = tcsEditLine[j];
	}

	m_PluIf.uchPluAdj = 1;   // default to the main item price

	short sRetStatus = 0;

	sRetStatus = SerOpPluDelete (&m_PluIf, m_usLockHnd);
	if (sRetStatus != 0) {
		CString csMsg;

		csMsg.Format (_T("Error delete PLU %d"), sRetStatus);
		AfxMessageBox (csMsg);
	} else {
		sRetStatus = SerOpPluFileUpdate(m_usLockHnd);
		if (sRetStatus != 0) {
			CString csMsg;

			csMsg.Format (_T("Error update PLU file %d"), sRetStatus);
			AfxMessageBox (csMsg);
		}
	}

	pButton = (CButton *)GetDlgItem (IDC_BUTTON_DELETEPLU);
	pButton->EnableWindow (FALSE);

}
