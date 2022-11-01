#pragma once
#include "afxwin.h"


// RPageCashierAction dialog

class RPageCashierAction : public RPageBase
{
	DECLARE_DYNAMIC(RPageCashierAction)

public:
	RPageCashierAction();
	virtual ~RPageCashierAction();

// Dialog Data
	enum { IDD = IDD_PAGE_CASHIERACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnsendcommand();
	afx_msg void OnCbnSelchangeCecaActiontype();

public:
	CString m_actionType;
	CString m_employeeId;
	long m_amount;
	UINT m_totalId;

public:

public:
	CButton m_btnSend;
	CComboBox m_cActionType;
	CEdit m_cAmount;
	CEdit m_cTotalId;
};
