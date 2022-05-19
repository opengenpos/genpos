#if !defined(AFX_DIALOGDEVICELIST_H__B244F13D_F231_4626_BA98_36B933B01F43__INCLUDED_)
#define AFX_DIALOGDEVICELIST_H__B244F13D_F231_4626_BA98_36B933B01F43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDeviceList.h : header file
//

#include <scf.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogDeviceList dialog

class CDialogDeviceList : public CDialog
{
// Construction
public:
	CDialogDeviceList(CWnd* pParent = NULL);   // standard constructor
	BOOL AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem = -1,
		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		int nFmt = LVCFMT_LEFT);
	BOOL AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);


	CString  csDeviceType;
	CString  csNewDevice;
	CString  csInterface;		// if OPOS then OPOS device, if DIRECT then direct device (comm port)

// Dialog Data
	//{{AFX_DATA(CDialogDeviceList)
	enum { IDD = IDD_DIALOG_DEVICELIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDeviceList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDeviceList)
	afx_msg void OnClickListDevices(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDEVICELIST_H__B244F13D_F231_4626_BA98_36B933B01F43__INCLUDED_)
