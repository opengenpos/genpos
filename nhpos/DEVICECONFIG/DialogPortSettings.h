#if !defined(AFX_DIALOGPORTSETTINGS_H__B7DC98A0_82FD_4E61_844B_DAFE8E97A8B3__INCLUDED_)
#define AFX_DIALOGPORTSETTINGS_H__B7DC98A0_82FD_4E61_844B_DAFE8E97A8B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogPortSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPortSettings dialog

class CDialogPortSettings : public CDialog
{
// Construction
public:
	CDialogPortSettings(CWnd* pParent = NULL);   // standard constructor

	CString port;
	CString portSettings;
	CString csInterface;
	CString csDeviceName;

// Dialog Data
	//{{AFX_DATA(CDialogPortSettings)
	enum { IDD = IDD_DIALOG_PORT_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPortSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPortSettings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPORTSETTINGS_H__B7DC98A0_82FD_4E61_844B_DAFE8E97A8B3__INCLUDED_)
