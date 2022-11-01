/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  MDeptNoDlg.h
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
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDEPTNODLG_H__2FAD9591_4EF4_11D2_AA02_0000399BE006__INCLUDED_)
#define AFX_MDEPTNODLG_H__2FAD9591_4EF4_11D2_AA02_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CMDeptNoDlg
//---------------------------------------------------------------------------

class CMDeptNoDlg : public CDialog
{
// Construction
public:
	CMDeptNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMDeptNoDlg)
	enum { IDD = IDD_SET_MDEPTNO };
	BYTE	m_bMajorDeptNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDeptNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMDeptNoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDEPTNODLG_H__2FAD9591_4EF4_11D2_AA02_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( MDeptNo.h ) /////////////////////////////////