#if !defined(AFX_DBUTTONEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_)
#define AFX_DBUTTONEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_

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
#include "WindowButtonExt.h"
#include "WindowDocumentExt.h"
#include "afxwin.h"

class CWindowButtonExt;

typedef struct
{
	int		ActionTextId;
	UINT	MinRange;
	UINT	MaxRange;
	int		RangeTextId;
	int		ExtMsgTextId;
	int		ExtMnemonicTextId;
} SDEntryParams, *PSDEntryParams;

class CDButtonEdit : public CDialog
{
// Construction
public:
	CDButtonEdit(CWindowButtonExt *pb = NULL, CWnd* pParent = NULL);   // standard constructor
	CDButtonEdit & operator = (const CWindowButtonExt & cdt);
	CWindowDocumentExt  *pDoc;
	friend CWindowButtonExt;
	CString myWindowName;

// Dialog Data
	//{{AFX_DATA(CDButtonEdit)
	enum { IDD = IDD_EDIT_BUTTONCTRL};
	CStatic	m_cStaticAction;
	CStaticColorPatch	m_StaticShowTextColor;
	CStaticColorPatch	m_StaticShowFaceColor;
	CComboBox	m_ComboHeight;
	CComboBox	m_ComboWidth;
	CButton     m_ButtonShapeRectangle;
	CButton     m_ButtonShapeElipse;
	CButton     m_ButtonShapeRoundedRectangle;
	CButton     m_ButtonPatternNone;
	CButton     m_ButtonPatternHoriz;
	CButton     m_ButtonPatternVert;
	CString m_csPreview;
	CString	m_csCaption;
	CString	m_csStaticCaption;
	CString m_csIcon;
	CString	m_csHeightMultiplier;
	CString	m_nWidthMultiplier;
	CButton	m_UseWinDef;
	CButton	m_UseCustom;
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
	
	CWindowButton::ButtonShape    myShape;
	CWindowButton::ButtonPattern  myPattern;
	CWindowButton::CaptionAlignment myCapAlignment;
	CWindowButton::HorizontalIconAlignment myHorIconAlignment;
	CWindowButton::VerticalIconAlignment myVertIconAlignment;
	BOOL horizOrient;
	int            myAction;
	CWindowButton::ButtonActionUnion  myActionUnion;

	USHORT     m_usWidthMultiplier;
	USHORT     m_usHeightMultiplier;
	int        m_iLabelType;
	BOOL         defFlag;
	DefaultData  df;
	BYTE         m_csSpecWin[8];
	ULONG		m_GroupPermissions;

	// Generated message map functions
	//{{AFX_MSG(CDButtonEdit)
	afx_msg void OnButtonTextcolor();
	afx_msg void OnButtonFacecolor();
	afx_msg void OnButtonAssignAction();
	//afx_msg void OnButtonAssignActionOLD(CWindowButton::ButtonActionType actionId, int textId);

	afx_msg void OnRadioWinDef();
	afx_msg void OnRadioCustom();
	afx_msg void OnSetFont();
	afx_msg void OnRadioHorizontal();
	afx_msg void OnRadioVertical();
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
	void _processAssignAction(CWindowButton::ButtonActionType actionId, int actionTextId);
	void _copyActionUnion(CWindowButton::ButtonActionUnion *pDst, CWindowButton::ButtonActionUnion *pSrc);
	void _setActionText(CWindowButton::ButtonActionUnion *pActionUnion, int actionId, bool fUpdateCaption=true);
	void _processDEntryDialog(CDEntry *pDEntry, int actionTextId, PSDEntryParams pParams=NULL);
	CString _getActionText(CWindowButton::ButtonActionUnion *pActionUnion, int actionTextId);
	PSDEntryParams _lookupDEntryParams(int actionTextId);
	CString _getWindowName(UINT windowId);
	CEdit m_cCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBUTTONEDIT_H__1F0025A2_2083_4892_9CFE_3627BD4404C9__INCLUDED_)
