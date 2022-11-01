#if !defined(AFX_DWINITEM_H__D9B87B75_DFF7_48D3_830E_C91BB3A1C88E__INCLUDED_)
#define AFX_DWINITEM_H__D9B87B75_DFF7_48D3_830E_C91BB3A1C88E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWinItem.h : header file
//

#include "resource.h"
#include "NewLayoutView.h"
#include "StaticColorPatch.h"
#include "afxwin.h"

#include "WindowItem.h"

class CWindowItemExt;

/////////////////////////////////////////////////////////////////////////////
// CDWinItem dialog

class CDWinItem : public CDialog
{
// Construction
public:
	CDWinItem(CWindowItemExt *pw = NULL, CWnd* pParent = NULL);   // standard constructor
	CDWinItem (int row, int column, int width = 12, int height = 12, CWnd* pParent = NULL);
	CWindowDocumentExt *pDoc;
	CDWinItem & operator = (const CWindowItemExt & cdt);

// Dialog Data
	//{{AFX_DATA(CDWinItem)
	enum { IDD = IDD_EDIT_WINDOWCTRL };
	CComboBox	m_nNumPicks;
	CComboBox	m_ComboGroup;
	CButton	m_SetDefaults;
	CButton	m_UseWinDef;
	CButton	m_UseDocDef;
	CButton	m_ButtonVirtual;
	CButton	m_ButtonPopup;
	CComboBox	m_ComboNoColumns;
	CComboBox	m_ComboNoRows;
	CStaticColorPatch	m_StaticColorPatchFace;
	CString	m_csCaption;
	CString	m_csName;
	CString	m_csStaticCaption;
	CString	m_csStaticName;
	BYTE m_csSpecWin[8];
	CButton m_SetGroupPermission;
	//}}AFX_DATA

	int  m_nRow;
	int  m_nColumn;
	int  myID;
	USHORT  m_usHeightMultiplier;
	USHORT  m_usWidthMultiplier;
	int  m_nVirtual;
	COLORREF   m_colorText;
	COLORREF   m_colorFace;
	BOOL defFlag;
	DefaultData df;
	UINT m_nGroup;
	UINT NumPicks;
	ULONG m_groupPermission;
	CString m_csWebBrowserUrl;

	CWindowItemExt * temp;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWinItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDWinItem)
	afx_msg void OnDialog2ButtonColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnResetWinDefaults();
	afx_msg void OnRadioDocDef();
	afx_msg void OnRadioWinDef();
	afx_msg void OnRadioPopup();
	afx_msg void OnRadioVirtual();
	afx_msg void OnDefaultWindow();
	afx_msg void OnEventTriggerEnabled();
	afx_msg void OnBnClickedSetGroupPermission();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedWebBrowser();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWINITEM_H__D9B87B75_DFF7_48D3_830E_C91BB3A1C88E__INCLUDED_)
