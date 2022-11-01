#if !defined(AFX_DENTRYPLU_H__1E415A88_A628_40E3_B473_ED0D30BFD134__INCLUDED_)
#define AFX_DENTRYPLU_H__1E415A88_A628_40E3_B473_ED0D30BFD134__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DEntryPLU.h : header file
//

#include "WindowControl.h"
#include "WindowButtonExt.h"

#include "P003.H"

/////////////////////////////////////////////////////////////////////////////
// CDEntryPLU dialog
#define MaxSizeOfPLU 14		//maximum size of PLUs

#define WIDE(s) L##s

class CDEntryPLU : public CDialog
{
// Construction
public:
	CDEntryPLU(CWnd* pParent = NULL);   // standard constructor
	CWindowButton::ButtonActionUnion  myActionUnion;
	WCHAR CheckDigitCalc(WCHAR *PluNo);

	CString previousPLU;
// Dialog Data
	//{{AFX_DATA(CDEntryPLU)
	enum { IDD = IDD_P03_PLUENTRY };
	CString	m_csExtFsc;
	CString	m_csExtMsg;
	CString	m_csExtMnemonics;
	int m_iExtFSC;
	CString m_csLabelType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEntryPLU)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEntryPLU)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DENTRYPLU_H__1E415A88_A628_40E3_B473_ED0D30BFD134__INCLUDED_)
