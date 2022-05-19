#if !defined(AFX_NETWORKPORTDLG_H__7614EDBD_8B45_4A2E_AEAA_C07310707731__INCLUDED_)
#define AFX_NETWORKPORTDLG_H__7614EDBD_8B45_4A2E_AEAA_C07310707731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetworkPortDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkPortDlg dialog

class CNetworkPortDlg : public CDialog
{
// Construction
public:
	CNetworkPortDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetworkPortDlg)
	enum { IDD = IDD_DIALOG_NETWORK_PORT };
	CEdit	m_ebNetworkPort;
	CString m_csNetworkPortNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetworkPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetworkPortDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKPORTDLG_H__7614EDBD_8B45_4A2E_AEAA_C07310707731__INCLUDED_)
