#include "afxwin.h"
#if !defined(AFX_LANDLG_H__D67285D3_F347_11D3_B841_0000C0C438EF__INCLUDED_)
#define AFX_LANDLG_H__D67285D3_F347_11D3_B841_0000C0C438EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLanDlg dialog
extern CPCSampleApp theApp;
class CLanDlg : public CDialog
{
// Construction
public:
	typedef enum {HostConnectIP = 1, HostConnectHost = 2 } HostConnectType;

	CLanDlg(CWnd* pParent = NULL);   // standard constructor
    CLanDlg(const BYTE bTerminalNo, const CSerialInfo& r_SerialInfo, CWnd* pParent = NULL );
	 

    CSerialInfo m_infoSerial;

	HostConnectType  m_HostConnectType;

// Dialog Data
	//{{AFX_DATA(CLanDlg)
	enum { IDD = IDD_CONNECT_LAN };
	BYTE	m_bIPAddress1;
	BYTE	m_bIPAddress2;
	BYTE	m_bIPAddress3;
	CString	m_strPassword;
    CString m_strHostName;	//PDINU
	BYTE	m_bTerminalNum;
	BYTE	m_bTerminalNo;
	CButton m_HostConnect;
	CButton m_IpConnect;
	BOOL m_SaveResetDataToDatabase;
	CString m_strDbFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLanDlg)
	afx_msg void OnChangeEditTerminalNumber();
	afx_msg void OnChangeEditPasswordLan();
	afx_msg void OnIpConnect();
	afx_msg void OnHostConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include <afxtempl.h>

class CSelectActivityList : public CDialog
{
// Construction
public:
	CSelectActivityList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectActivityList)
	enum { IDD = IDD_SELECTACTIVITYLIST };
	//}}AFX_DATA

	CList<CString, CString &> *m_list;
	CString  m_csSelection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectActivityList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectActivityList)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANDLG_H__D67285D3_F347_11D3_B841_0000C0C438EF__INCLUDED_)
