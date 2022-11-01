#pragma once
#include "afxwin.h"

#include "PCSample.h"
#include "R20_PC2172.h"
#include "afxcmn.h"

// CDiaCashierEdit dialog

class CEditInt : public CEdit
{
public:
	int GetWindowTextAsInt (void);
	void SetWindowTextAsInt (int iValue);
};

class CTabCtrlDialog : public CTabCtrl
{
public:
	CTabCtrlDialog();
	~CTabCtrlDialog();
	void InsertItemDialogTemplate (UINT nIDTemplate, int nItem, TCITEM* pTabCtrlItem);

public:
	struct {
		UINT     nIDTemplate;
		CDialog  *pDialog;
	}  m_pDialogData[10];
};

class CDiaCashierEdit : public CDialog
{
	DECLARE_DYNAMIC(CDiaCashierEdit)

public:
	CDiaCashierEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDiaCashierEdit();

	BOOL  ImportCasif (CASIF *pCashier);
	BOOL  ExportCasif (CASIF *pCashier);

// Dialog Data
	enum { IDD = IDD_CASHIER_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CASIF   m_paraCashier;

private:
	CEdit m_CashierName;
	CEditInt m_SupervisorId;
	CEditInt m_TeamNumber;
	CEditInt m_GuestCheckStart;
	CEditInt m_GuestCheckEnd;
	CTabCtrlDialog m_TabCtrl;

	TCITEM  m_TabItemOne;
	TCITEM  m_TabItemTwo;
	TCITEM  m_TabItemThree;

	struct TabToStatus {
		short  sTabItem;
		int    iDlgItem;
		short  sOffset;
		UCHAR  uchBit;
	};

	static const struct TabToStatus m_TabItemOneStatus[];

public:
	afx_msg void OnTcnSelchangeTabCashierEditStatus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTabCashierEditStatus(NMHDR *pNMHDR, LRESULT *pResult);
};
