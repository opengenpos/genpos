#if !defined(AFX_MODEMDLG_H__503C6283_581E_11D2_AA17_0000399BE006__INCLUDED_)
#define AFX_MODEMDLG_H__503C6283_581E_11D2_AA17_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ModemDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModemDlg dialog

class CModemDlg : public CDialog
{
// Construction
public:
	CModemDlg(CWnd* pParent = NULL);   // standard constructor
    CModemDlg( const CString strCommand,
               const CString strPhone,
               const int nPulse,
               CWnd* pParent = NULL );

// Dialog Data
	//{{AFX_DATA(CModemDlg)
	enum { IDD = IDD_CONNECT_MODEM };
	CString	m_strCommand;
	CString	m_strPhone;
	int		m_nPulse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModemDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMDLG_H__503C6283_581E_11D2_AA17_0000399BE006__INCLUDED_)
