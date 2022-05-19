// DeviceConfigDlg.h : header file
//

#if !defined(AFX_DEVICECONFIGDLG_H__64E15566_0626_4CF7_AFD3_20BB51CC7332__INCLUDED_)
#define AFX_DEVICECONFIGDLG_H__64E15566_0626_4CF7_AFD3_20BB51CC7332__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScfList.h"
#include "ShortCut.h"
/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigDlg dialog
//
//  WARNING FOR WINDOWS 7    **
//     DeviceConfig must be started using Run as Administrator in order to access
//     the Windows Registry keys in the tree of HKEY_LOCAL_MACHINE.
//     If DeviceConfig is run as normal user, any changes will be put into the VirtualStore
//     section of HKEY_CURRENT_USER and changes will not be made to HKEY_LOCAL_MACHINE.
//
//     We have run into a problem with provisioning changes that did not seem to take only
//     to find that we had a VirtualStore section in HKEY_CURRENT_USER which was not modified
//     by DeviceConfig since it was being Run as Administrator resulting in GenPOS using the
//     old provisioning in VirtualStore of HKEY_CURRENT_USER instead.
//  

class CDeviceConfigDlg : public CDialog
{
// Construction
public:
	CDeviceConfigDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem = -1,
		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		int nFmt = LVCFMT_LEFT);
	BOOL AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);
	void ReadInConfigFile (void);
	BOOL MakeSettingsString (LPCTSTR atchDeviceName, CString &csSettings);
	BOOL putsettingsDirect (LPCTSTR atchDeviceName, CString &csPort, CString &csSettings);
	BOOL putsettingsEthernet (LPCTSTR atchDeviceName, CString &csSettings);

	void ProcessConfigurationFile (void);

	CScfList myScfList;

	CScfInterface  *m_genposurl;    // special _T("genpos web portal") entry from DeviceConfig.txt

	TCHAR  m_BiosSerialNumber[64];


	CScfList::DevListMap devMapDsiPdcx;  // see https://www.codeproject.com/Articles/13458/CMap-How-to
	CScfList::DevListMap devMapDsiEmvUs;  // see https://www.codeproject.com/Articles/13458/CMap-How-to

typedef struct {
	struct {
		int iDsiPdcxAvailable;
		int iDsiEmvUsAvailable;
	};
	TCHAR MerchantID[25];
	TCHAR PINPadDevice[22];
	TCHAR PINPadPort[3];
	TCHAR PadIdlePrompt[16];
	TCHAR PDC[2];
	TCHAR KeyManagement[2];
	TCHAR MasterKeyIndex[2];
	TCHAR WorkingKey[17];
	TCHAR ShouldRotate[2];
	TCHAR MagneticType[23];
	TCHAR ReaderType[23];
	TCHAR MagReaderTimeout[3];
	TCHAR CheckReaderTimeout[3];
	TCHAR StoreForward[2];
	TCHAR StoreForwardName[21];
} EPTSetting;
	EPTSetting EPTsettings;

	BOOL puteptsetting (LPCTSTR atchDeviceName);

// Dialog Data
	//{{AFX_DATA(CDeviceConfigDlg)
	enum { IDD = IDD_DEVICECONFIG_DIALOG };
	CListCtrl	m_clDeviceList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CShortCut shortCut_NHPOS;
	BOOL      m_bHttpServerEnabled;
	int       m_bConnEngineEnabled;
	int       m_iConnEnginePort;
	int  m_IsDisconnectedSatellite;
	int  m_IsJoinedDisconnectedSatellite;
	int  m_IsSatelliteOverride;
	TCHAR  m_LastJoinMaster[32];
	TCHAR  m_SatelliteOverride[32];
	CString PortSettings;

	CEquipmentDialog   *m_pPinPadListDialog;
	CEquipmentDialog   *m_pEptDllListDialog;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDeviceConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkListDevices(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListDevices(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSettings();
	afx_msg void OnButtonChangeDevice();
	virtual void OnOK();
	afx_msg void OnInitialReset();
	afx_msg void OnTestEnvironment();
	afx_msg void OnNetworkPort();
	afx_msg void OnChangeAssigned();
	afx_msg void OnChangePort();
	afx_msg void OnNhposUnlock();
	afx_msg void OnImportIcons();
	afx_msg void OnExportIcons();
	afx_msg void OnNhposStartup();
	afx_msg void OnChangeOptions();
	afx_msg void OnNhposDisconnectedSat();
	afx_msg void OnNhposDisconnectedSatJoined();
	afx_msg void OnButtonDisconnectedSatParams();
	afx_msg void OnHttpServer();
	afx_msg void OnNhposConnEngine();
	afx_msg void OnTerminalutilitiesImportactivesettings();
	afx_msg void OnTerminalutilitiesExportactivesettings();
	afx_msg void OnGenerateExecuteUserSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioDisconnectedOverride();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICECONFIGDLG_H__64E15566_0626_4CF7_AFD3_20BB51CC7332__INCLUDED_)
