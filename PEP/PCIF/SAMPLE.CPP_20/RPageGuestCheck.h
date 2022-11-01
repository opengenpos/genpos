#pragma once
#include "afxwin.h"

// RPageGuestCheck dialog

class RPageGuestCheck : public RPageBase
{
	DECLARE_DYNAMIC(RPageGuestCheck)

public:
	RPageGuestCheck();
	virtual ~RPageGuestCheck();

// Dialog Data
	enum { IDD = IDD_PAGE_GUESTCHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_guestCheck;
public:
	afx_msg void OnBnClickedCeSend();
};
