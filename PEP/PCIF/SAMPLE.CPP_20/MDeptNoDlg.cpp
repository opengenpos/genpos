/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  MDeptNoDlg.cpp
//
//  PURPOSE:    Major Dept. No. dialog class definitions and declarations.
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

#include "pcsample.h"
#include "MDeptNoDlg.h"

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

BEGIN_MESSAGE_MAP(CMDeptNoDlg, CDialog)
	//{{AFX_MSG_MAP(CMDeptNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CMDeptNoDlg::CMDeptNoDlg()
//
//  PURPOSE :   Constructor of Major Department Number dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CMDeptNoDlg::CMDeptNoDlg( CWnd* pParent ) :
    CDialog( CMDeptNoDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CMDeptNoDlg)
	m_bMajorDeptNo = 0;
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CMDeptNoDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CMDeptNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDeptNoDlg)
	DDX_Text(pDX, IDC_EDIT_MDEPTNO, m_bMajorDeptNo);
	DDV_MinMaxByte(pDX, m_bMajorDeptNo, 0, 30);
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CMDeptNoDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CMDeptNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // --- limit text length in department no. edit box ---

	CEdit*  pEdit;

    pEdit = static_cast< CEdit* >( GetDlgItem( IDC_EDIT_MDEPTNO ));
    pEdit->LimitText( 2 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////// END OF FILE ( MDeptNoDlg.cpp ) ////////////////////////////
