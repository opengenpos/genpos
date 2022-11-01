/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TerminalNoDlg.cpp
//
//  PURPOSE:    Terminal No. dialog class definitions and declarations.
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
#include "TerminalNoDlg.h"

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

BEGIN_MESSAGE_MAP(CTerminalNoDlg, CDialog)
	//{{AFX_MSG_MAP(CTerminalNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CTerminalNoDlg::CStoreNoDlg()
//
//  PURPOSE :   Constructor of Terminal No. dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTerminalNoDlg::CTerminalNoDlg( CWnd* pParent ) :
    CDialog( CTerminalNoDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CTerminalNoDlg)
	m_bTerminalNo = 1;
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CTerminalNoDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CTerminalNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalNoDlg)
	DDX_Text(pDX, IDC_EDIT_TERMNO, m_bTerminalNo);
	DDV_MinMaxByte(pDX, m_bTerminalNo, 1, 255);
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CTerminalNoDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CTerminalNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // --- limit text length in cashier no. edit box ---

	CEdit*  pEdit;
    pEdit = static_cast< CEdit* >( GetDlgItem( IDC_EDIT_TERMNO ));
    pEdit->LimitText( 3 ); // changed to three to accomodate IP address

	



	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////// END OF FILE ( TerminalNoDlg.cpp ) ////////////////////////
