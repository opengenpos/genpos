/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  LogonDlg.cpp
//
//  PURPOSE:    Log on dialog class definitions and declarations.
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Terminal.h"

#include "PCSample.h"
#include "ModemDlg.h"

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

BEGIN_MESSAGE_MAP(CModemDlg, CDialog)
	//{{AFX_MSG_MAP(CModemDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CModemDlg::CModemDlg()
//
//  PURPOSE :   Constructor of Modem dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CModemDlg::CModemDlg( CWnd* pParent ) :
    CDialog(CModemDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CModemDlg)
    m_strCommand  = _T("");
	m_strPhone = _T("");
	m_nPulse = 1;
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CModemDlg::CModemDlg()
//
//  PURPOSE :   Constructor of Modem dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CModemDlg::CModemDlg(
    const CString strCommand,
    const CString strPhone,
    const int nPulse,
    CWnd* pParent
    ) : CDialog(CModemDlg::IDD, pParent)
{
    m_strCommand = strCommand;
    m_strPhone = strPhone;
    m_nPulse = nPulse;
}

//===========================================================================
//
//  FUNCTION :  CModem::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CModemDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CModemDlg)
    DDX_Text(pDX, IDC_EDIT_ATCOMMAND, m_strCommand);
    DDV_MaxChars(pDX, m_strCommand, 40);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_strPhone);
	DDV_MaxChars(pDX, m_strPhone, 40);
	DDX_Radio(pDX, IDC_RADIO_PULSE, m_nPulse);
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CModemDlg::OnInitDialog()
//
//  PURPOSE :   Initialize dialog box contents before it is displayed.
//
//  RETURN :    TRUE    - set initial focus to first item.
//              FALSE   - set initial focus to next item.
//
//===========================================================================

BOOL CModemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    int     nCheckedID;
    
    nCheckedID = ( m_nPulse == 0 ) ? IDC_RADIO_PULSE : IDC_RADIO_TONE;

    CheckRadioButton( IDC_RADIO_PULSE, IDC_RADIO_TONE, nCheckedID );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////// END OF FILE ( ModemDlg.cpp ) /////////////////////////////
