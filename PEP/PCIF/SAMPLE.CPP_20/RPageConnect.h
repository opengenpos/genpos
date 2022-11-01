#pragma once
#include "afxwin.h"


// RPageConnect dialog

class RPageConnect : public RPageBase
{
	DECLARE_DYNAMIC(RPageConnect)

public:
	RPageConnect();
	virtual ~RPageConnect();

// Dialog Data
	enum { IDD = IDD_PAGE_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	LRESULT OnReceiveMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void _initData();
	void _saveIpAddress(CString &newIpAddress);
	virtual BOOL OnInitDialog();
	void _logIncomingMsgs(WPARAM wParam);
	void _displayIncomingMessage(LPCTSTR pText, bool fIndent=false);

public:
	CButton m_btnConnect;
	CString m_ipAddress;
	CString m_commandText;
	CButton m_btnSend;
	CEdit m_editCommand;
	int m_ipPort;

public:
	afx_msg void OnBtnClickedConnect();
	afx_msg void OnBnClickedCesend();
	afx_msg void OnBnClickedCeclear();
public:
	CComboBox m_cIpAddress;
	int m_fLogOutgoingMsgs;
	int m_fLogIncomingMsgs;
public:
	afx_msg void OnBnClickedCeLogMsgOut();
public:
	afx_msg void OnBnClickedCeClearincoming();
public:
	CString m_incomingMsgs;
public:
	CEdit m_cIncomingMsgs;
	afx_msg void OnBnClickedDisconnect();
	CButton m_btnDisconnect;
};
