#if !defined(AFX_DISPLAYTRANSACTION_H__030F312F_7AD4_40C5_9797_561798BDD897__INCLUDED_)
#define AFX_DISPLAYTRANSACTION_H__030F312F_7AD4_40C5_9797_561798BDD897__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayTransaction.h : header file
//
#include "resource.h"

#include <ecr.h>
#include <r20_pif.h>
#include <pifctl.h>
#include <paraequ.h>
#include <r20_cpm.h>

/////////////////////////////////////////////////////////////////////////////
// CDisplayTransaction dialog

class CDisplayTransaction : public CDialog
{
// Construction
public:
	CDisplayTransaction(CWnd* pParent = NULL);   // standard constructor

	SetDialogData (CPMRCRSPDATA *pData);

// Dialog Data
	//{{AFX_DATA(CDisplayTransaction)
	enum { IDD = IDD_DIALOG_TRAN_DISPLAY };
	CComboBox	m_cbReturnStatus;
	CString	m_csAmount;
	CString	m_csFuncType;
	CString	m_csSequenceNo;
	CString	m_csDestTerminal;
	CString	m_csSourceTerminal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayTransaction)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplayTransaction)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYTRANSACTION_H__030F312F_7AD4_40C5_9797_561798BDD897__INCLUDED_)
