#pragma once

#include "afxwin.h"
#include "PCSample.h"
#include "R20_PC2172.h"

// CDiaCashierAddEdit dialog

class CDiaCashierAddEdit : public CDialog
{
	DECLARE_DYNAMIC(CDiaCashierAddEdit)

public:
	CDiaCashierAddEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDiaCashierAddEdit();

	SHORT LoadListControl (void);

// Dialog Data
	enum { IDD = IDD_CASHIER_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	SHORT   m_sActualRead;
	SHORT   m_sLastError;
	struct {
		BOOL  bModified;
		SHORT sLastError;
		CASIF paraCashier;
	} m_paraCashier[300];

public:
	CListBox m_CashierListBox;
	afx_msg void OnBnClickedButtonEditSelected();
	afx_msg void OnBnClickedButtonSendChanges();
};


class CDiaCashierSelect : public CDialog
{
	DECLARE_DYNAMIC(CDiaCashierSelect)

public:
	CDiaCashierSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDiaCashierSelect();

	SHORT LoadListControl(void);

	// Dialog Data
	enum { IDD = IDD_CASHIER_SELECT };

	ULONG    m_ulCashierNo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	SHORT   m_sActualRead;
	SHORT   m_sLastError;
	struct {
		BOOL  bModified;
		SHORT sLastError;
		CASIF paraCashier;
	} m_paraCashier[300];

public:
	CListBox m_CashierListBox;
	afx_msg void OnBnClickedButtonEditSelected();
};
