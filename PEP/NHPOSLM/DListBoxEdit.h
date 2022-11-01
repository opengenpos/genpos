#if !defined(AFX_DLISTBOXEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_)
#define AFX_DLISTBOXEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DButtonEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDButtonEdit dialog

#include "resource.h"
#include "DEntryAdjPls.h"
#include "StaticColorPatch.h"
#include "DefaultData.h"
#include "WindowListBoxExt.h"
#include "WindowDocumentExt.h"
#include "afxwin.h"

class CWindowListBoxExt;

class CDListBoxEdit : public CDialog
{
// Construction
public:
	CDListBoxEdit(CWindowListBoxExt *pb = NULL, CWnd* pParent = NULL);   // standard constructor
	CDListBoxEdit & operator = (const CWindowListBoxExt & cdt);
	CWindowDocumentExt  *pDoc;
	friend CWindowListBoxExt;
	CString myWindowName;

// Dialog Data
	//{{AFX_DATA(CDButtonEdit)
	enum { IDD = IDD_EDIT_LISTBOXCTRL};
	CStatic	m_cStaticAction;
	CStaticColorPatch	m_StaticShowTextColor;
	CStaticColorPatch	m_StaticShowFaceColor;
	CComboBox	m_ComboHeight;
	CComboBox	m_ComboWidth;
	CString m_csPreview;
	CString	m_csCaption;
	CString	m_csStaticCaption;
	CString m_csIcon;
	CString	m_csHeightMultiplier;
	CString	m_nWidthMultiplier;
	CButton m_SetFaceColor;
	CButton m_SetTextColor;
	CButton m_SetFont;
	CStatic m_ShowCap;
	CButton m_SetGroupPermission;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDButtonEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDlgControls(BOOL flag);

	int        nRow;
	int        nColumn;
	CString    myCaption;
	CString    m_myName;
	COLORREF   colorText;
	COLORREF   colorFace;
	LOGFONT	   myBtnFont;
	
	BOOL horizOrient;
	int            myAction;

	USHORT     m_usWidthMultiplier;
	USHORT     m_usHeightMultiplier;
	int        m_iLabelType;
	BOOL         m_defFlag;
	DefaultData  m_defParent;
	BYTE         m_csSpecWin[8];
	ULONG		m_GroupPermissions;

	// Generated message map functions
	//{{AFX_MSG(CDListBoxEdit)
	afx_msg void OnButtonTextcolor();
	afx_msg void OnButtonFacecolor();
	afx_msg void OnButtonAssignAction();

	afx_msg void OnRadioWinDef();
	afx_msg void OnRadioCustom();
	afx_msg void OnSetFont();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeEditCaption();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowse();
	afx_msg void OnSetDir();
	afx_msg void OnChangeConnEngine();
	afx_msg void OnChangeAllowScript();
	afx_msg void OnBnClickedGroupPermissions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString _getWindowName(UINT windowId);
	CEdit m_cCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLISTBOXEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_)
