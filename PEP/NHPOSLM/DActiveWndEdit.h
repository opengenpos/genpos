#if !defined(AFX_DACTIVEWNDEDIT_H__388CC47E_2CB1_42ED_9428_78120E723446__INCLUDED_)
#define AFX_DACTIVEWNDEDIT_H__388CC47E_2CB1_42ED_9428_78120E723446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DActiveWndEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDActiveWndEdit dialog

class CDActiveWndEdit : public CDialog
{
// Construction
public:
	CDActiveWndEdit(CWindowGroup * wg = NULL, CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CDActiveWndEdit)
	enum { IDD = IDD_DLG_ACTIVEWND };
	CButton	m_btnActivate;
	CComboBox	m_cbGroups;
	CListBox	m_lbGroupWindows;
	//}}AFX_DATA

	CNewLayoutView* myView;
	UINT m_nSelGrp;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDActiveWndEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDActiveWndEdit)
	afx_msg void OnButtonActivate();
	afx_msg void OnSelchangeComboGrplist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DACTIVEWNDEDIT_H__388CC47E_2CB1_42ED_9428_78120E723446__INCLUDED_)
