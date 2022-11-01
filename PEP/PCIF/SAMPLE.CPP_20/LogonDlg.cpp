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
#include "LogonDlg.h"
#include "SerialPortDlg.h"

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

BEGIN_MESSAGE_MAP(CLogonDlg, CDialog)
	//{{AFX_MSG_MAP(CLogonDlg)
	ON_BN_CLICKED(IDC_PB_SERIALPORT, OnSerialPortClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CLogonDlg::CLogonDlg()
//
//  PURPOSE :   Constructor of Log on dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CLogonDlg::CLogonDlg( CWnd* pParent ) : 
    CDialog( CLogonDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CLogonDlg)
	m_bTerminalNo = 1;
	m_strPassword = _T("");
	m_strPortNo   = _T("COM2");
	m_strBaudRate = _T("19200");
	m_strDataBits = _T("8");
	m_strParity   = _T("None");
	m_strStopBits = _T("1");
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CLogonDlg::CLogonDlg()
//
//  PURPOSE :   Constructor of Log on dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CLogonDlg::CLogonDlg(
    const BYTE bTerminalNo,
    const CSerialInfo& SerialInfo,
    CWnd* pParent
    ) : CDialog( CLogonDlg::IDD, pParent )
{
    // --- initialize member variables with user specified parameter ---

    if (( 0 < bTerminalNo ) && ( bTerminalNo <= MAX_TERMINAL_NO ))
    {
	    m_bTerminalNo = bTerminalNo;
    }
    else
    {
	    m_bTerminalNo = 1;
    }

	m_strPassword = _T("");

    m_infoSerial = SerialInfo;

	m_strPortNo.Format  ( _T("COM%1u"), SerialInfo.m_uchPortNo );
	m_strBaudRate.Format( _T("%u"),     SerialInfo.m_usBaudRate );
	m_strDataBits.Format( _T("%u"),     SerialInfo.m_uchDataBits );
	m_strStopBits.Format( _T("%u"),     SerialInfo.m_uchStopBits );

    switch ( SerialInfo.m_uchParityBits )
    {
    case 0:
	    m_strParity = _T( "None" );
        break;
    case 1:
	    m_strParity = _T( "Odd" );
        break;
    case 2:
    default:
	    m_strParity = _T( "Even" );
        break;
    }
}

//===========================================================================
//
//  FUNCTION :  CLogonDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CLogonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogonDlg)
	DDX_Text(pDX, IDC_EDIT_TERMNO, m_bTerminalNo);
	DDV_MinMaxByte(pDX, m_bTerminalNo, 1, 16);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 10);
	DDX_Text(pDX, IDC_TEXT_PORT, m_strPortNo);
	DDX_Text(pDX, IDC_TEXT_BAUD, m_strBaudRate);
	DDX_Text(pDX, IDC_TEXT_DATABITS, m_strDataBits);
	DDX_Text(pDX, IDC_TEXT_PARITY, m_strParity);
	DDX_Text(pDX, IDC_TEXT_STOPBITS, m_strStopBits);
	//}}AFX_DATA_MAP
}

//===========================================================================
//
//  FUNCTION :  CLogonDlg::OnSerialPortClicked()
//
//  PURPOSE :   Show the serial port configuration dialog box on screen.
//
//  RETURN :    No return value.
//
//===========================================================================

void CLogonDlg::OnSerialPortClicked() 
{
    // --- show the serial port configuration dialog on screen ---

    CSerialPortDlg  dlg( m_strPortNo, m_strBaudRate, m_strDataBits,
                        m_strParity, m_strStopBits );

    if ( dlg.DoModal() == IDOK )
    {
        // --- store inputed configulatioin to member variables ---

        m_strBaudRate = dlg.m_strBaudRate;
        m_strDataBits = dlg.m_strDataBits;
        m_strParity   = dlg.m_strParity;
        m_strPortNo   = dlg.m_strPortNo;
        m_strStopBits = dlg.m_strStopBits;

        GetDlgItem( IDC_EDIT_PASSWORD )->GetWindowText( m_strPassword );

        // --- update dialog controls with new value ---

        UpdateData( FALSE );
    }
}

//===========================================================================
//
//  FUNCTION :  CLogonDlg::OnOK()
//
//  PURPOSE :   Get inputed data on dialog box.
//
//  RETURN :    No return value.
//
//===========================================================================

void CLogonDlg::OnOK() 
{
    UCHAR   uchPortNo;
    USHORT  usBaudRate;
    UCHAR   uchDataBits;
    UCHAR   uchParity;
    UCHAR   uchStopBits;

    // --- get inputed value on dialog controls ---

    uchPortNo   = static_cast< UCHAR  >( m_strPortNo[ 3 ] - '0' );
    usBaudRate  = static_cast< USHORT >( _ttoi(( LPCTSTR )m_strBaudRate ));
    uchDataBits = static_cast< UCHAR  >( _ttoi(( LPCTSTR )m_strDataBits ));
    uchStopBits = static_cast< UCHAR  >( _ttoi(( LPCTSTR )m_strStopBits ));

    if ( m_strParity == _T( "None" ))
    {
        uchParity   = 0;
    }
    else if ( m_strParity == _T( "Odd" ))
    {
        uchParity   = 1;
    }
    else
    {
        uchParity   = 2;
    }

    m_infoSerial.m_uchPortNo     = uchPortNo;
    m_infoSerial.m_usBaudRate    = usBaudRate;
    m_infoSerial.m_uchDataBits   = uchDataBits;
    m_infoSerial.m_uchParityBits = uchParity;
    m_infoSerial.m_uchStopBits   = uchStopBits;
	
	CDialog::OnOK();
}

//////////////////// END OF FILE ( CLogonDlg.cpp ) //////////////////////////
