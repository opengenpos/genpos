#pragma once
#include "afxwin.h"

// RPageEmployeeChange dialog

class RPageEmployeeChange : public RPageBase
{
	DECLARE_DYNAMIC(RPageEmployeeChange)

public:
	RPageEmployeeChange();
	virtual ~RPageEmployeeChange();

// Dialog Data
	enum { IDD = IDD_PAGE_EMPLOYEECHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_actionType;
public:
	CString m_employeeId;
public:
	CEdit m_cMnemonic;
public:
	CString m_mnemonic;
public:
	CEdit m_cJobCode;
public:
	CString m_jobCode;
public:
	CEdit m_cEmployeeId;
public:
	afx_msg void OnCbnSelchangeCeActiontype();
public:
	afx_msg void OnBnClickedCeSend();
};
