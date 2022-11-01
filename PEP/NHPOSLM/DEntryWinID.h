#ifndef H_DENTRYWINID_SMUR_20080131
#define H_DENTRYWINID_SMUR_20080131

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DEntryWinID.h : header file
//
// This file cloned and modified from DEntryPLU.h
// Defines the popup dialogue for entering a Window ID
// Used in button creation in the layout manager
// Specifically created to satisfy the needs of two new FSC codes:
// FSC_WINDOW_DISPLAY and FSC_WINDOW_DISMISS

#include "WindowControl.h"
#include "WindowButtonExt.h"

#include "P003.H"

/////////////////////////////////////////////////////////////////////////////
// CDEntryWinID dialog
#define MaxSizeOfWinID 14		//determines the digit cap on the input box.  maximum size of Window IDs

#define WIDE(s) L##s		

class CDEntryWinID : public CDialog
{
// Construction
public:
	CDEntryWinID(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;
//	WCHAR CheckDigitCalc(WCHAR *WinID);	 //used for internal validation... see DEntryPLU.cpp for implementation code

// Dialog Data
	//{{AFX_DATA(CDEntryWinID)
	enum { IDD = IDD_P03_WINID };
	CString	m_csExtFsc;
	CString	m_csExtMsg;
	CString	m_csExtMnemonics;
	int m_iExtFSC;
	CString m_csLabelType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEntryWinID)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEntryWinID)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // H_DENTRYWINID_SMUR_20080131
