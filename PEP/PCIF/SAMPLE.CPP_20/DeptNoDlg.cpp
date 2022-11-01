/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  DeptNoDlg.cpp
//
//  PURPOSE:    Department Number dialog class definitions and declarations.
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
#include "DeptNoDlg.h"

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

BEGIN_MESSAGE_MAP(CDeptNoDlg, CDialog)
	//{{AFX_MSG_MAP(CDeptNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CDeptNoDlg::CDeptNoDlg()
//
//  PURPOSE :   Constructor of Department Number dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CDeptNoDlg::CDeptNoDlg( CWnd* pParent ) :
    CDialog(CDeptNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeptNoDlg)
	m_nDeptNo = 0;            // department number of zero means all PLUs
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CDeptNoDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CDeptNoDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeptNoDlg)
	DDX_Text(pDX, IDC_EDIT_DEPTNO, m_nDeptNo);
	DDV_MinMaxInt(pDX, m_nDeptNo, 0, 9999);   // normal range is 1 to 9999 but we allow zero to perform read all by PLU number
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CDeptNoDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CDeptNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // --- limit text length in department no. edit box ---

    CEdit*  pEdit;

    pEdit = static_cast< CEdit* >( GetDlgItem( IDC_EDIT_DEPTNO ));
    pEdit->LimitText( 4 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////// END OF FILE ( DeptNoDlg.cpp ) ////////////////////////////
