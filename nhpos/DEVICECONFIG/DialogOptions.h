#if !defined(AFX_DIALOGOPTIONS_H__E3D70D84_8802_4993_A9C6_C60E7750797D__INCLUDED_)
#define AFX_DIALOGOPTIONS_H__E3D70D84_8802_4993_A9C6_C60E7750797D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogOptions dialog

class CDialogOptions : public CEquipmentDialog
{
// Construction
public:
	CDialogOptions(CWnd* pParent = NULL);   // standard constructor

	virtual void writeChanges (void);       // required for a CEquipmentDialog


	BOOL AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem = -1,
		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		int nFmt = LVCFMT_LEFT);
	BOOL AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);
	void GetDataFrom(CDialogOptions &fromDialog);
	BOOL readOptions();

	DWORD    m_dwDevCount;    // count of number of items in the following arrays
	CString  xNameList[128], xTypeList[128], xValueList[128], xTempValueList[128];

// Dialog Data
	//{{AFX_DATA(CDialogOptions)
	enum { IDD = IDD_DIALOG_OPTION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListOptions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListOptions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOptionChange();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGOPTIONS_H__E3D70D84_8802_4993_A9C6_C60E7750797D__INCLUDED_)
