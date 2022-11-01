/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  GCNoDlg.cpp
//
//  PURPOSE:    Guest Check Number dialog class definitions and declarations.
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
#include "GCNoDlg.h"

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

BEGIN_MESSAGE_MAP(CGCNoDlg, CDialog)
	//{{AFX_MSG_MAP(CGCNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CGCNoDlg::CGCNoDlg()
//
//  PURPOSE :   Constructor of Guest Check Number dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CGCNoDlg::CGCNoDlg( CWnd* pParent ) :
    CDialog( CGCNoDlg::IDD, pParent )
{
    //{{AFX_DATA_INIT(CGCNoDlg)
    m_sGCNo = 1;
    //}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CGCNoDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CGCNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGCNoDlg)
	DDX_Text(pDX, IDC_EDIT_GCNO, m_sGCNo);
	DDV_MinMaxInt(pDX, m_sGCNo, 1, 9999);
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CGCNoDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CGCNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // --- limit text length in guest check no. edit box ---

    CEdit*  pEdit;

    pEdit = static_cast<CEdit*>(GetDlgItem( IDC_EDIT_GCNO ));
    pEdit->LimitText( 4 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////// END OF FILE ( GCNoDlg.cpp ) ////////////////////////////
