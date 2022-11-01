#if !defined(AFX_DTEXTITEM_H__557C5160_1126_48DC_97E7_C1CBADDF3427__INCLUDED_)
#define AFX_DTEXTITEM_H__557C5160_1126_48DC_97E7_C1CBADDF3427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DTextItem.h : header file
//

#include "resource.h"
#include "WindowDocumentExt.h"
#include "StaticColorPatch.h"
#include "WindowTextExt.h"

/////////////////////////////////////////////////////////////////////////////
// CDTextItem dialog

class CDTextItem : public CDialog
{
// Construction
public:
	CDTextItem(CWindowTextExt *pb = NULL, CWnd* pParent = NULL);   // standard constructor
	CWindowDocumentExt *pDoc;  //Document that contains this text area
	CDTextItem & operator = (const CWindowTextExt & cdt);
	void SetDlgControls(BOOL flag);
// Dialog Data
	//{{AFX_DATA(CDTextItem)
	enum { IDD = IDD_EDIT_TEXTCTRL };
	CStaticColorPatch	m_StaticShowFaceColor;
	CButton m_def;
	CButton m_cust;
	CButton m_TextFaceColor;
	CButton m_EditFont;
	//}}AFX_DATA

	int  myID;		//
	int  m_nRow;	//how many rows in the window?
	int  m_nColumn;	//how many columns in the window?
	int  m_usHeightMultiplier;	//Window height (grids)
	int  m_usWidthMultiplier;	//Window width (grids)
//	int  m_nTop;	//Window origin y position
//	int  m_nLeft;	//WIndow origin x position
	int  m_nControlType; //Window Functionality Receipt/ type
	COLORREF   colorFace;
	COLORREF   colorText;
	LOGFONT myTextFont;
	CString    myCaption;
	CString    myName;
	BOOL defFlag;
	DefaultData df;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTextItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDTextItem)
	afx_msg void OnDialog6ButtonColor();
	afx_msg void OnSelchange();
	afx_msg void OnRadioDef();
	afx_msg void OnRadioCust();
	afx_msg void OnChangeFont();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DTEXTITEM_H__557C5160_1126_48DC_97E7_C1CBADDF3427__INCLUDED_)
