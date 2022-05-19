#if !defined(AFX_SAVEWINDOW_H__7258F2D5_296D_407A_A210_4F51277559B6__INCLUDED_)
#define AFX_SAVEWINDOW_H__7258F2D5_296D_407A_A210_4F51277559B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CListWindow dialog

class CListWindow : public CDialog
{
// Construction
public:
	CListWindow(CWnd* pParent = NULL,bool import=false);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CListWindow)
	enum { IDD = IDD_LIST_DIALOG };
	//controler for the edit box
	CEdit	m_fileNames;
	//controler for the combo box
	CComboBox	cmbbox;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	CListCtrl listctrl;
	//holds the images of the icons to display in the list
	CImageList * imageList;
	//the address in the system that is active
	TCHAR root[256];
	//displays the address of the location on the system
	CStatic *addressBar;
	//drives on the system
	CString Drives[26];
	//destination of the icons to be copied to
	CString destination;
	bool m_bexport;
	//used for the text box for file names
	bool userClick;
	CString SelectedNames;

	void createList();
	void GetSubFolder(CString folderName);
	void removeLastAddress();
	void GetDrives();
	void myComputer();
	void Desktop();
	int GetHour(int hour,CString &ampm);

	// Generated message map functions
	//{{AFX_MSG(CListWindow)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUp();
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseupCombo1();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit1();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_SAVEWINDOW_H__7258F2D5_296D_407A_A210_4F51277559B6__INCLUDED_)
