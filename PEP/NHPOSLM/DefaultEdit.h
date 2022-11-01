#if !defined(AFX_DEFAULTEDIT_H__38AD3118_194F_409D_A652_56B6B5953B03__INCLUDED_)
#define AFX_DEFAULTEDIT_H__38AD3118_194F_409D_A652_56B6B5953B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefaultEdit.h : header file
//
#include "resource.h"
#include "StaticColorPatch.h"
#include "WindowButton.h"
/////////////////////////////////////////////////////////////////////////////
// DefaultEdit dialog

class DefaultEdit : public CDialog
{
// Construction
public:
	DefaultEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DefaultEdit)
	enum { IDD = IDD_DLG_DEFAULTS };
	CButton	m_PatternVertical;
	CButton	m_PatternNone;
	CButton	m_PatternHorizontal;
	CButton	m_ShapeRoundRect;
	CButton	m_ShapeRect;
	CButton	m_ShapeElipse;
	CStatic	m_FontSample;
	CStaticColorPatch	m_StaticShowTextWinColor;
	CStaticColorPatch	m_StaticShowWinColor;
	CStaticColorPatch	m_StaticShowFaceColor;
	CStaticColorPatch	m_StaticShowTextColor;
	CString	m_DefHeight;
	CString	m_DefWidth;
	//}}AFX_DATA
	 

	COLORREF ButtonDefault;
	COLORREF WindowDefault;
	COLORREF TextWinDefault;
	
	LOGFONT FontDefault;
	COLORREF FontColorDefault;

	CWindowButton::ButtonShape    ShapeDefault;
	CWindowButton::ButtonPattern  PatternDefault;
	USHORT     WidthDefault;
	USHORT     HeightDefault;
	
	LOGFONT lf;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DefaultEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DefaultEdit)
	afx_msg void OnChangeDefBtnColor();
	afx_msg void OnChangeDefFont();
	afx_msg void OnChangeDefTextWinColor();
	afx_msg void OnChangeDefWinColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDefFontColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFAULTEDIT_H__38AD3118_194F_409D_A652_56B6B5953B03__INCLUDED_)
