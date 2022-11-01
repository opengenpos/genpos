/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  SerialPort.h
//
//  PURPOSE:    Serial Port dialog class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORTDLG_H__7D05A2A5_42FE_11D2_A9E3_0000399BE006__INCLUDED_)
#define AFX_SERIALPORTDLG_H__7D05A2A5_42FE_11D2_A9E3_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CSerialPortDlg
//---------------------------------------------------------------------------

class CSerialPortDlg : public CDialog
{
// Construction
public:
	CSerialPortDlg(CWnd* pParent = NULL);   // standard constructor
    CSerialPortDlg(const CSerialInfo& r_SerialInfo, CWnd* pParent = NULL);
    CSerialPortDlg( const CString& strPortNo, const CString& strBaudRate,
        const CString& strDataBits, const CString& strParity, const CString& strStopBits,
        CWnd* pParent = NULL );

// Dialog Data
	//{{AFX_DATA(CSerialPortDlg)
	enum { IDD = IDD_SET_SERIALPORT };
	CString	m_strStopBits;
	CString	m_strPortNo;
	CString	m_strParity;
	CString	m_strDataBits;
	CString	m_strBaudRate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSerialPortDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTDLG_H__7D05A2A5_42FE_11D2_A9E3_0000399BE006__INCLUDED_)

//////////////// END OF FILE ( SerialPortDlg.h ) ////////////////////////////
