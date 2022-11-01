/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PasswordDlg.h
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
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSWORDDLG_H__8E9A3464_43B7_11D2_A9E6_0000399BE006__INCLUDED_)
#define AFX_PASSWORDDLG_H__8E9A3464_43B7_11D2_A9E6_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CPasswordDlg
//---------------------------------------------------------------------------

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_CHG_PASSWORD };
	CString	m_strPassword;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__8E9A3464_43B7_11D2_A9E6_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( PasswordDlg.h ) /////////////////////////////
