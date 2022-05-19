#pragma once


// COposPinPadOptions dialog

class COposPinPadOptions : public CEquipmentDialog
{
	DECLARE_DYNAMIC(COposPinPadOptions)

public:
	COposPinPadOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~COposPinPadOptions();

	virtual void writeChanges (void);

	BOOL UpdateDialogBox (int iDlgItemId, LPCTSTR csDeviceName, LPCTSTR csScfFieldName, TCHAR *csScfFieldDefault, int iLimit, int Status);
	BOOL UpdateRegistry (CString csFieldValue, LPCTSTR csDeviceName, LPCTSTR csScfFieldName, TCHAR *csScfFieldDefault);
	BOOL readOptions (void);


	CString port;
	CString csKeyManagementType;
	CString csIdlePrompt;
	CString csMasterKeyIndex;
	CString csWorkingKey;
	CString csShouldRotate;

	typedef struct {
		CString MerchantID;
		CString PINPadPort;
		CString PadIdlePrompt;
		CString KeyManagement;
		CString MasterKeyIndex;
		CString WorkingKey;
		CString ShouldRotate;
	} EPTSetting;

	EPTSetting EPTsetting;

#define USE_REGISTRY	1
#define USE_DATA		2

// Dialog Data
	enum { IDD = IDD_DIALOG_OPOS_PINPAD };

protected:
	short UpdateDialogSettings (void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COposPinPadOptions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
