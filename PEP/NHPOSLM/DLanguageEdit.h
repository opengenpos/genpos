#if !defined(AFX_DLANGUAGEEDIT_H__FDE3DBAD_F081_4279_AF3A_24CFE3AC5CF0__INCLUDED_)
#define AFX_DLANGUAGEEDIT_H__FDE3DBAD_F081_4279_AF3A_24CFE3AC5CF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLanguageEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLanguageEdit dialog

class CDLanguageEdit : public CDialog
{
// Construction
public:
	CDLanguageEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLanguageEdit)
	enum { IDD = IDD_CHANGE_LANGUAGE };
	CComboBox	m_LanguageSelect;
	//}}AFX_DATA
int Selection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLanguageEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLanguageEdit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLANGUAGEEDIT_H__FDE3DBAD_F081_4279_AF3A_24CFE3AC5CF0__INCLUDED_)
