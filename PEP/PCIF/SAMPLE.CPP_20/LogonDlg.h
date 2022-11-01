/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  LogonDlg.h
//
//  PURPOSE:    Log On dialog class definitions and declarations.
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

#if !defined(AFX_LOGONDLG_H__7D05A2A4_42FE_11D2_A9E3_0000399BE006__INCLUDED_)
#define AFX_LOGONDLG_H__7D05A2A4_42FE_11D2_A9E3_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CGCNoDlg
//---------------------------------------------------------------------------

class CLogonDlg : public CDialog
{
// Construction
public:
	CLogonDlg(CWnd* pParent = NULL);   // standard constructor
    CLogonDlg(const BYTE bTerminalNo, const CSerialInfo& r_SerialInfo, CWnd* pParent = NULL );

    CSerialInfo m_infoSerial;

// Dialog Data
	//{{AFX_DATA(CLogonDlg)
	enum { IDD = IDD_CONNECT_LOGON };
	BYTE	m_bTerminalNo;
	CString	m_strPassword;
	CString	m_strPortNo;
	CString	m_strBaudRate;
	CString	m_strDataBits;
	CString	m_strParity;
	CString	m_strStopBits;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogonDlg)
	afx_msg void OnSerialPortClicked();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGONDLG_H__7D05A2A4_42FE_11D2_A9E3_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( LogonDlg.h ) ////////////////////////////////
