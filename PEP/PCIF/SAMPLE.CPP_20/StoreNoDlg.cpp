/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  StoreNoDlg.cpp
//
//  PURPOSE:    Store No. dialog class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PCSample.h"
#include "StoreNoDlg.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CStoreNoDlg, CDialog)
	//{{AFX_MSG_MAP(CStoreNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CStoreNoDlg::CStoreNoDlg()
//
//  PURPOSE :   Constructor of Store No. dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CStoreNoDlg::CStoreNoDlg( CWnd* pParent ) :
    CDialog( CStoreNoDlg::IDD, pParent ), m_sChangeConfirmed(0)
{
	//{{AFX_DATA_INIT(CStoreNoDlg)
	m_sStoreNo = 0;
	m_sTerminalNo = 0;
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CStoreNoDlg::CStoreNoDlg()
//
//  PURPOSE :   Constructor of Store No. dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CStoreNoDlg::CStoreNoDlg(
    const USHORT usStoreNo,
    const USHORT usRegNo,
    CWnd* pParent ) :
    CDialog( CStoreNoDlg::IDD, pParent ), m_sChangeConfirmed(0)
{
    // --- initialize member variables with user specified parameter ---

	m_sStoreNo    = usStoreNo;
	m_sTerminalNo = usRegNo;
}

//===========================================================================
//
//  FUNCTION :  CStoreNoDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CStoreNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate) {
		m_sStoreNoSaved = m_sStoreNo;
		m_sTerminalNoSaved = m_sTerminalNo;
	}
	//{{AFX_DATA_MAP(CStoreNoDlg)
	DDX_Text(pDX, IDC_EDIT_STORENO, m_sStoreNo);
	DDV_MinMaxInt(pDX, m_sStoreNo, 0, 9999);
	DDX_Text(pDX, IDC_EDIT_TERMNO, m_sTerminalNo);
	DDV_MinMaxInt(pDX, m_sTerminalNo, 0, 999);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		CString csConfirm (_T("Press Ok to confirm change to "));
		m_sChangeConfirmed = IDCANCEL;
		if (m_sStoreNoSaved != m_sStoreNo) {
			csConfirm += _T("Store Number");
			if (m_sTerminalNoSaved != m_sTerminalNo) {
				csConfirm += _T(" and Register Number");
			}
			csConfirm += _T(".");
			m_sChangeConfirmed = AfxMessageBox (csConfirm, MB_OKCANCEL);
		} else if (m_sTerminalNoSaved != m_sTerminalNo) {
			csConfirm += _T("Register Number.");
			m_sChangeConfirmed = AfxMessageBox (csConfirm, MB_OKCANCEL);
		}
		m_sChangeConfirmed = (m_sChangeConfirmed == IDOK) ? 1 : 0;
	}
}

//===========================================================================
//
//  FUNCTION :  CStoreNoDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CStoreNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // --- limit text length in cashier no. edit box ---

    CEdit*  pEdit;

    pEdit = static_cast< CEdit* >( GetDlgItem( IDC_EDIT_STORENO ));
    pEdit->LimitText( 4 );

    pEdit = static_cast< CEdit* >( GetDlgItem( IDC_EDIT_TERMNO ));
    pEdit->LimitText( 3 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////// END OF FILE ( StoreNoDlg.cpp ) ///////////////////////////
