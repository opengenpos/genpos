#if !defined(AFX_DWINDOWLIST_H__E007B31B_F999_44D0_865F_6890F46FC922__INCLUDED_)
#define AFX_DWINDOWLIST_H__E007B31B_F999_44D0_865F_6890F46FC922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWindowList.h : header file
//

#include "WindowDocumentExt.h"
#include "WindowItemExt.h"
/////////////////////////////////////////////////////////////////////////////
// CDWindowList dialog

class CDWindowList : public CDialog
{
// Construction
public:
	CWindowItemExt* GetSelectedSubWin(CWindowItemExt *wi, UINT id);
	CWindowItemExt* GetSelectedWin(UINT id);
	CDWindowList(CWnd* pParent = NULL);   // standard constructor

	UINT uiActiveID;
	CNewLayoutView *myView;
	CWindowDocumentExt *pDoc;
	void SetTreeItem(CWindowItem *wi, HTREEITEM ti);
	BOOL forWinBtn;			//used to determine if this dialog is called to show another window or to select a window to call

// Dialog Data
	//{{AFX_DATA(CDWindowList)
	enum { IDD = IDD_WINDOW_LIST };
	CListBox	m_ListBoxWindowList;
	CButton m_PropButton;
	CTreeCtrl	m_DocControlTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWindowList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDWindowList)
	afx_msg void OnDblClkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnDialog5Showprop();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWINDOWLIST_H__E007B31B_F999_44D0_865F_6890F46FC922__INCLUDED_)
