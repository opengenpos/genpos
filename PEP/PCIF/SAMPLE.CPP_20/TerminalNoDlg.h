/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TerminalNoDlg.h
//
//  PURPOSE:    Terminal Number dialog class definitions and declarations.
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

#if !defined(AFX_TERMINALNODLG_H__68DAF2C3_46B6_11D2_A9EB_0000399BE006__INCLUDED_)
#define AFX_TERMINALNODLG_H__68DAF2C3_46B6_11D2_A9EB_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TerminalNoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalNoDlg dialog

class CTerminalNoDlg : public CDialog
{
// Construction
public:
	CTerminalNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerminalNoDlg)
	enum { IDD = IDD_SET_TERMINALNO };
	BYTE	m_bTerminalNo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTerminalNoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALNODLG_H__68DAF2C3_46B6_11D2_A9EB_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( TerminalNoDlg.h ) ///////////////////////////
