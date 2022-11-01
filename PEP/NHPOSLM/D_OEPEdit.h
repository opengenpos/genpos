#if !defined(AFX_D_OEPEDIT_H__7C537C89_083D_489F_885C_0019AB678AC0__INCLUDED_)
#define AFX_D_OEPEDIT_H__7C537C89_083D_489F_885C_0019AB678AC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// D_OEPEdit.h : header file
//
#include "WindowTextExt.h"
#include "resource.h"
#include "StaticColorPatch.h"
/////////////////////////////////////////////////////////////////////////////
// CD_OEPEdit dialog

class CD_OEPEdit : public CDialog
{
// Construction
public:
	void CalculateWinDimensions();
	void CalculateButtonsPerWindow();
	CD_OEPEdit(CWindowTextExt * wi, CWindowItemExt* pParent = NULL);   // standard constructor
	CD_OEPEdit & operator = (const CWindowTextExt & cdt);
	void saveTo(CWindowTextExt* cdt);

// Dialog Data
	//{{AFX_DATA(CD_OEPEdit)
	enum { IDD = IDD_DLG_OEP };
	CButton	m_buttonOEPShapeRect;
	CButton	m_buttonOEPShapeElipse;
	CButton	m_buttonOEPShapeRoundRect;
	CButton	m_buttonOEPPattNone;
	CButton	m_buttonOEPPattVert;
	CButton	m_buttonOEPPattHoriz;
	CButton	m_buttonOEPFaceColor;
	CComboBox	m_comboOEPWidth;
	CComboBox	m_comboOEPHeight;
	CStaticColorPatch m_staticOEPBtnColor;
	CStaticColorPatch m_staticOEPWinColor;
	int		m_nOEPHeight;
	int		m_nOEPWidth;
	int		m_nOEPIconHorizRadioSelection;
	int		m_nOEPIconVertRadioSelection;
	CString m_csOEPIconFilePath;
	CString	m_csOEPIconFileName;

	BYTE m_csSpecWin[8];
	//}}AFX_DATA

	//All Text Items
	UINT myID;
	int        mRow;
	int        mColumn;
	USHORT     m_usWidthMultiplier;
	USHORT     m_usHeightMultiplier;

	COLORREF   colorFace;
	COLORREF   colorText;
	LOGFONT myTextFont;
	BOOL defFlag;
	DefaultData df;
	CString    myCaption;
	CString    myName;
	

	//OEP specific
	COLORREF   btnColorFace;
	CWindowButton::ButtonShape    myShape;
	CWindowButton::ButtonPattern  myPattern;
	UINT m_usBtnWidth;
	UINT m_usBtnHeight;
	UINT m_BtnDown;
	UINT m_BtnAcross;
	LOGFONT myBtnFont;
	COLORREF btnFontColor;
	//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	CString m_OEPGroupNumbers;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CD_OEPEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CD_OEPEdit)
	afx_msg void OnOepEditBtnFont();
	afx_msg void OnOepEditWinFont();
	afx_msg void OnOepSetBtnFaceColor();
	afx_msg void OnOepSetWinColor();
	afx_msg void OnChangeComboWidthHeight();
	afx_msg void OnChangeComboOepBtnAcross();
	afx_msg void OnChangeComboOepBtnDown();
	afx_msg void OnChangeComboOEPWinGridWidth();
	afx_msg void OnChangeComboOEPWinGridHeight();
	afx_msg void OnBnClickedIconBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSpecialOep();
	afx_msg void OnBnClickedSplPreauth();
	afx_msg void OnBnClickedSplReasonCode();
	afx_msg void OnBnClickedSplAutoCoupon();
	afx_msg void OnBnClickedSplOpenCheck();
	afx_msg void OnEnChangeEdit1Oepgroupnumbers();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_D_OEPEDIT_H__7C537C89_083D_489F_885C_0019AB678AC0__INCLUDED_)
