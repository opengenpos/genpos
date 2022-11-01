/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  StoreNoDlg.h
//
//  PURPOSE:    Store Number dialog class definitions and declarations.
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

#if !defined(AFX_STORENODLG_H__1EABDEE1_4871_11D2_A9F2_0000399BE006__INCLUDED_)
#define AFX_STORENODLG_H__1EABDEE1_4871_11D2_A9F2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CStoreNoDlg
//---------------------------------------------------------------------------

class CStoreNoDlg : public CDialog
{
// Construction
public:
	CStoreNoDlg(CWnd* pParent = NULL);   // standard constructor
    CStoreNoDlg( const USHORT usStoreNo, const USHORT usTermNo, CWnd* pParent = NULL );

// Dialog Data
	//{{AFX_DATA(CStoreNoDlg)
	enum { IDD = IDD_SET_REGNO };
	short	m_sStoreNo;
	short	m_sTerminalNo;
	//}}AFX_DATA

	short   m_sChangeConfirmed;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStoreNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	short	m_sStoreNoSaved;
	short	m_sTerminalNoSaved;

	// Generated message map functions
	//{{AFX_MSG(CStoreNoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STORENODLG_H__1EABDEE1_4871_11D2_A9F2_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( StoreNoDlg.h ) //////////////////////////////
