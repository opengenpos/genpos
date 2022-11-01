/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PasswordDlg.cpp
//
//  PURPOSE:    Password dialog class definitions and declarations.
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

BEGIN_MESSAGE_MAP(CSerialPortDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialPortDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CSerialPortDlg::CSerialPortDlg()
//
//  PURPOSE :   Constructor of Serial Port dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CSerialPortDlg::CSerialPortDlg( CWnd* pParent ) :
    CDialog( CSerialPortDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CSerialPortDlg)
	m_strStopBits = _T("");
	m_strPortNo   = _T("");
	m_strParity   = _T("");
	m_strDataBits = _T("");
	m_strBaudRate = _T("");
	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CSerialPortDlg::CSerialPortDlg()
//
//  PURPOSE :   Constructor of Serial Port dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CSerialPortDlg::CSerialPortDlg(
    const CSerialInfo& SerialInfo,
    CWnd* pParent ) :
    CDialog( CSerialPortDlg::IDD, pParent )
{
    // --- initialize member variables with user specified parameter ---

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
//  FUNCTION :  CSerialPortDlg::CSerialPortDlg()
//
//  PURPOSE :   Constructor of Serial Port dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CSerialPortDlg::CSerialPortDlg(
    const CString& strPortNo,
    const CString& strBaudRate,
    const CString& strDataBits,
    const CString& strParity,
    const CString& strStopBits,
    CWnd* pParent ) :
    CDialog( CSerialPortDlg::IDD, pParent )
{
    // --- initialize member variables with user specified parameter ---

	m_strPortNo   = strPortNo;
	m_strBaudRate = strBaudRate;
	m_strDataBits = strDataBits;
	m_strParity   = strParity;
	m_strStopBits = strStopBits;
}

//===========================================================================
//
//  FUNCTION :  CSerialPortDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CSerialPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialPortDlg)
	DDX_CBString(pDX, IDC_CB_STOPBITS, m_strStopBits);
	DDX_CBString(pDX, IDC_CB_PORTNO, m_strPortNo);
	DDX_CBString(pDX, IDC_CB_PARITY, m_strParity);
	DDX_CBString(pDX, IDC_CB_DATABITS, m_strDataBits);
	DDX_CBString(pDX, IDC_CB_BAUD, m_strBaudRate);
	//}}AFX_DATA_MAP
}

////////////////// END OF FILE ( SerialPort.cpp ) ///////////////////////////
