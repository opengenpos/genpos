#if !defined(AFX_DWINLABEL_H__0BEEB10B_0BE0_40B0_BAD6_5CDEEE4E8913__INCLUDED_)
#define AFX_DWINLABEL_H__0BEEB10B_0BE0_40B0_BAD6_5CDEEE4E8913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWinLabel.h : header file
//
#include "resource.h"
#include "StaticColorPatch.h"
#include "WindowLabelExt.h"
#include "WindowEditBoxExt.h"

/////////////////////////////////////////////////////////////////////////////
// CDWinLabel dialog

class CDWinLabel : public CDialog
{
// Construction
public:
	CDWinLabel(CWindowLabelExt *pl, CWnd* pParent = NULL);   // standard constructor
	CDWinLabel(CWindowEditBoxExt *pl, CWnd* pParent = NULL);   // standard constructor
	CDWinLabel & operator = (const CWindowLabelExt & cdt);
	CDWinLabel & operator = (const CWindowEditBoxExt & cdt);

// Dialog Data
	//{{AFX_DATA(CDWinLabel)
	enum { IDD = IDD_DLG_WINLABEL };
	CComboBox	m_ComboWidth;
	CComboBox	m_ComboHeight;
	CStaticColorPatch	m_StaticShowFaceColor;
	CString	m_csIcon;
	CString	m_csCaption;
	CString m_csPreview;
	int		m_nHeightMultiplier;
	int		m_nWidthMultiplier;
	CStatic m_ShowCap;
	//}}AFX_DATA

	int        nRow;
	int        nColumn;
	CString    myCaption;
	CString    m_myName;
	COLORREF   colorText;
	COLORREF   colorFace;
	LOGFONT	   myLblFont;

	CWindowLabel::CaptionAlignment myCapAlignment;
	CWindowLabel::HorizontalIconAlignment myHorIconAlignment;
	CWindowLabel::VerticalIconAlignment myVertIconAlignment;
	BOOL       horizOrient;
	BOOL       isCounterLabel;
	USHORT     m_usWidthMultiplier;
	USHORT     m_usHeightMultiplier;
	BYTE       m_csSpecWin[8];

	DefaultData df;
	CWindowDocumentExt *pDoc;

	BOOL  bSpecialBorder;
	BOOL  bLeftBorder;
	BOOL  bRightBorder;
	BOOL  bTopBorder;
	BOOL  bBottomBorder;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWinLabel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDWinLabel)
	afx_msg void OnBtnFacecolor();
	afx_msg void OnSetFont();
	afx_msg void OnRadioHorizontal();
	afx_msg void OnRadioVertical();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonBrowse();
	afx_msg void OnSetDir();
	afx_msg void OnSpecialBorder();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditLblcaption();
	afx_msg void OnChangeEditPreview();
	afx_msg void OnChangeConnEngine();
	afx_msg void OnChangeAllowScript();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWINLABEL_H__0BEEB10B_0BE0_40B0_BAD6_5CDEEE4E8913__INCLUDED_)
