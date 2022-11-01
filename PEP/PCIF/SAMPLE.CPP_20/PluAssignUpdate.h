#if !defined(AFX_PLUASSIGNUPDATE_H__767DAE86_14D6_4B93_B30A_E64A3B3500B1__INCLUDED_)
#define AFX_PLUASSIGNUPDATE_H__767DAE86_14D6_4B93_B30A_E64A3B3500B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluAssignUpdate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPluAssignUpdate dialog

class CPluAssignUpdate : public CDialog
{
// Construction
public:
	CPluAssignUpdate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPluAssignUpdate)
	enum { IDD = IDD_PLUASSIGNUPDATE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluAssignUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	USHORT  m_usLockHnd;
	PLUIF   m_PluIf;

	// Generated message map functions
	//{{AFX_MSG(CPluAssignUpdate)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFetchplu();
	afx_msg void OnButtonUpdateplu();
	afx_msg void OnChangeEditPlunumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDeleteplu();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUASSIGNUPDATE_H__767DAE86_14D6_4B93_B30A_E64A3B3500B1__INCLUDED_)
