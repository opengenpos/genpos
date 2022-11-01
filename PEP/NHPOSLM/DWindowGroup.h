#if !defined(AFX_DWINDOWGROUP_H__A39FA4C0_27F2_4B44_A41B_1705BC6533E8__INCLUDED_)
#define AFX_DWINDOWGROUP_H__A39FA4C0_27F2_4B44_A41B_1705BC6533E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWindowGroup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDWindowGroup dialog

class CDWindowGroup : public CDialog
{
// Construction
public:
	BOOL ValidateGroupDelete(CWindowItemExt * wi, UINT index);
	BOOL isNew;
	CNewLayoutView* myView;
	CDWindowGroup(CWindowGroup* wg = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDWindowGroup)
	enum { IDD = IDD_DLG_GROUPS };
	CButton	m_btnEdit;
	CButton	m_btnSubmit;
	CButton m_rdoEdit;
	CButton m_rdoSubmit;
	CListBox	m_ListBoxGroups;
	CString	m_csGroupDesc;
	CString	m_csGroupName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWindowGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDWindowGroup)
	afx_msg void OnDeleteGroup();
	afx_msg void OnEditGroup();
	afx_msg void OnSubmitChange();
	afx_msg void OnRadioAdd();
	afx_msg void OnRadioEdit();
	afx_msg void OnGroupListSelect();
	afx_msg void OnButtonSetactwnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWINDOWGROUP_H__A39FA4C0_27F2_4B44_A41B_1705BC6533E8__INCLUDED_)
