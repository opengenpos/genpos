// DeviceConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include <atlbase.h>

#include <scf.h>
#include "DeviceConfig.h"
#include "DeviceConfigDlg.h"
#include "ScfInterface.h"
#include "ScfParameters.h"
#include "pif.h"
#include "DialogDeviceList.h"
//#include "DialogDrawerList.h"
#include "DialogPortSettings.h"
#include "OposPinPadOptions.h"
#include "NetworkPortDlg.h"
#include <Winsock.h>
#include <Windows.h>
#include "ecr.h"
#include "DialogInitialReset.h"
#include "SecretNumberDlg.h"
#include "TestDlg.h"
#include "ShortCut.h"
#include "DialogOptions.h"
#include "SaveWindow.h"
#include "DialogDisconnectedSatParams.h"
#include <process.h>  // For VBOLock Security (creates independent processes)

#include "dragdropDlg.h"

/*================================================================================
	This header file is needed because PluTotalOpen() PluTotalClose() has
	to initialy be called or an exception will occur upon exit of the program
*/
#include "PluTtlD.H"
#include "afxwin.h"
/*================================================================================*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigDlg dialog

CDeviceConfigDlg::CDeviceConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeviceConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_IsDisconnectedSatellite = 0;
	m_IsJoinedDisconnectedSatellite = 0;
	m_pPinPadListDialog = 0;
	m_pEptDllListDialog = 0;
	_tcscpy (m_BiosSerialNumber, _T("000"));
	memset(m_LastJoinMaster, 0, sizeof(m_LastJoinMaster));
	memset(m_SatelliteOverride, 0, sizeof(m_SatelliteOverride));
}

void CDeviceConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceConfigDlg)
	DDX_Control(pDX, IDC_LIST_DEVICES, m_clDeviceList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeviceConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CDeviceConfigDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DEVICES, OnDblclkListDevices)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEVICES, OnClickListDevices)
	ON_BN_CLICKED(IDC_BUTTON_SETTINGS, OnButtonSettings)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_DEVICE, OnButtonChangeDevice)
	ON_COMMAND(IDM_INITIAL_RESET, OnInitialReset)
	ON_COMMAND(IDM_TEST_ENVIRONMENT, OnTestEnvironment)
	ON_COMMAND(IDS_NETWORK_PORT, OnNetworkPort)
	ON_COMMAND(IDM_NHPOS_UNLOCK, OnNhposUnlock)
	ON_COMMAND(IDS_IMPORT_ICONS, OnImportIcons)
	ON_COMMAND(IDS_EXPORT_ICONS, OnExportIcons)
	ON_BN_CLICKED(IDC_NHPOS_STARTUP, OnNhposStartup)
	ON_BN_CLICKED(IDC_CHANGE_OPTIONS, OnChangeOptions)
	ON_BN_CLICKED(IDC_NHPOS_DISCONNECTED_SAT, OnNhposDisconnectedSat)
	ON_BN_CLICKED(IDC_NHPOS_DISCONNECTED_SAT_JOINED, OnNhposDisconnectedSatJoined)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECTED_SAT_PARAMS, OnButtonDisconnectedSatParams)
	ON_BN_CLICKED(IDC_RADIO_DISCONNECTED_OVERRIDE, OnBnClickedRadioDisconnectedOverride)
	ON_BN_CLICKED(IDC_NHPOS_HTTPSERVER, OnHttpServer)
	ON_BN_CLICKED(IDC_NHPOS_CONNENGINE, OnNhposConnEngine)
	ON_COMMAND(ID_TERMINALUTILITIES_IMPORTACTIVESETTINGS, OnTerminalutilitiesImportactivesettings)
	ON_COMMAND(ID_TERMINALUTILITIES_EXPORTACTIVESETTINGS, OnTerminalutilitiesExportactivesettings)
	ON_COMMAND(ID_TERMINALUTILITIES_USER_SETUP_SCRIPT, OnGenerateExecuteUserSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

typedef enum {Ignore_Control = 0, Direct_Control=1, OPOS_Control=2, Ethernet_Control=3, Note_Control=4, DLL_Control=5, Special_Control=6, Wedge_Control=7} SCF_CONTROL_TYPE;
typedef enum {Ignore = 0, DIRECT=1, OPOS=2, ETHERNET=3, NOTE=4, DLL=5, SPECIAL=6, WEDGE=7} DEVICE_INTERFACE;

static int enumControlType [] = {			// These need to be in the same order at the SCF_CONTROL_TYPE enum above
	IDS_CONTROL_TYPE_IGNORE,
	IDS_CONTROL_TYPE_DIRECT,
	IDS_CONTROL_TYPE_OPOS,
	IDS_CONTROL_TYPE_ETHERNET,
	IDS_CONTROL_TYPE_NOTE,
	IDS_CONTROL_TYPE_DLL,
	IDS_CONTROL_TYPE_SPECIAL,
	IDS_CONTROL_TYPE_WEDGE
};

static int enumInterfaceType [] = {
	IDS_CONTROL_TYPE_IGNORE,
	IDS_CONTROL_INT_DIRECT,
	IDS_CONTROL_INT_OPOS,
	IDS_CONTROL_INT_ETHERNET,
	IDS_CONTROL_INT_NOTE,
	IDS_CONTROL_INT_DLL,
	IDS_CONTROL_INT_SPECIAL,
	IDS_CONTROL_INT_WEDGE
};

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
typedef struct {
	TCHAR *aszDevName;					// SCF_TYPE_NAME for lookup by SCF in Active Device List, OPOS options at Platform Software\SystemConfig\SystemParameter
	int   nItem;						// Item number in the displayed CListCtrl
	SCF_CONTROL_TYPE  scfType;			// The type of control such as OPOS, etc
	TCHAR *aszRegistryKey;				// Registry key if it is an OPOS control.  sub-key underneath OLEforRetail\ServiceOPOS
	DWORD dwMaxDevices;					// Maximum number of same kind of device
	DEVICE_INTERFACE  deviceInterface;	// The  way the control is interfaced, via ethernet, com port etc.
	CEquipmentDialog    *pDialogOptions;
} DEVICE_LIST_TABLE, *PDEVICE_LIST_TABLE;

static DEVICE_LIST_TABLE myDeviceList [] = {
	{ SCF_TYPENAME_PRINTER,    0, OPOS_Control,   SCF_OPOS_NAME_PRINTER,  1, OPOS,    0},
	{ SCF_TYPENAME_PRINTER,    1, Direct_Control, NULL,                   1, DIRECT,  0},
	{ SCF_TYPENAME_SCANNER,    2, Direct_Control, NULL,                   2, DIRECT,  0},
	{ SCF_TYPENAME_SCANNER1,   3, Wedge_Control,  NULL,                   1, WEDGE,   0},
	{ SCF_TYPENAME_SCALE,      4, Direct_Control, NULL,                   1, DIRECT,  0},
	{ SCF_TYPENAME_MSR,        5, OPOS_Control,   SCF_OPOS_NAME_MSR,      1, OPOS,    0},
	{ SCF_TYPENAME_MSR,        6, Direct_Control, NULL,                   1, DIRECT,  0},
	{ SCF_TYPENAME_MSR1,       7, OPOS_Control,   SCF_OPOS_NAME_MSR,      1, OPOS,    0},
	{ SCF_TYPENAME_MSR2,       8, Wedge_Control,  NULL,                   1, WEDGE,   0},
	{ SCF_TYPENAME_SIGCAP1,    9, OPOS_Control,   SCF_OPOS_NAME_SIGCAP,   1, OPOS,    0},
	{ SCF_TYPENAME_PINPAD1,   10, OPOS_Control,   SCF_OPOS_NAME_PINPAD,   1, OPOS,    0},
	{ SCF_TYPENAME_CDISPLAY,  11, OPOS_Control,   SCF_OPOS_NAME_CDISPLAY, 1, OPOS,    0},
	{ SCF_TYPENAME_COIN,      12, OPOS_Control,   SCF_OPOS_NAME_COIN,     1, OPOS,    0},
	{ SCF_TYPENAME_DRAWER1,   13, OPOS_Control,   SCF_OPOS_NAME_DRAWER,   1, OPOS,    0}, //ESMITH Increase to 2 from 1
	{ SCF_TYPENAME_DRAWER2,	  14, OPOS_Control,  SCF_OPOS_NAME_DRAWER,    1, OPOS,    0}, //JHALL INCREASE to 2 from 1 the right way.
	{ SCF_TYPENAME_CHARGE_POSTING,    15, Direct_Control, NULL, 1, DIRECT,        0},
	{ SCF_TYPENAME_CHARGE_POSTINGLAN, 16, Ethernet_Control, NULL, 1, ETHERNET,    0},
	{ SCF_TYPENAME_EPT,             17, Direct_Control,   NULL, 1, DIRECT,    0},
	{ SCF_TYPENAME_EPT_EX,          18, Direct_Control,   NULL, 1, DIRECT,    0},
	{ SCF_TYPENAME_EPT_EX_LAN,      19, Ethernet_Control, NULL, 1, ETHERNET,  0},
	{ SCF_TYPENAME_EPT_EX_DLL,      20, Special_Control,  NULL, 1, SPECIAL,   0},
	{ SCF_TYPENAME_EPT_EX_DLL1,     21, Ethernet_Control, NULL, 1, SPECIAL,   0},
	{ SCF_TYPENAME_EPT_EX_DLL2,     22, DLL_Control,      NULL, 1, SPECIAL,   0},
	{ SCF_TYPENAME_EPT_EX_DLL3,     23, DLL_Control,      NULL, 1, SPECIAL,   0},
	{ SCF_TYPENAME_BEVERAGE,        24, Direct_Control,   NULL, 1, DIRECT,    0},
	{ SCF_TYPENAME_KITCHEN_CRT,     25, Ethernet_Control, NULL, 1, ETHERNET,  0},
	{ SCF_TYPENAME_KITCHEN_PRINTER, 26, Direct_Control,   NULL, 5, DIRECT,    0},
	{ SCF_TYPENAME_KEYBOARD,        27, Note_Control,     NULL, 1, NOTE,      0},
	{ SCF_TYPENAME_KEYLOCK,         28, OPOS_Control,     SCF_OPOS_NAME_KEYLOCK, 1, OPOS,    0}, //ESMITH
	{ SCF_TYPENAME_PC,              29, Direct_Control,   NULL, 1, DIRECT,    0},
	{ NULL,                          0, Ignore_Control,   NULL, 0, Ignore,    0}
};


BOOL CDeviceConfigDlg::AddColumn(CListCtrl  *pclView, LPCTSTR strItem,int nItem,int nSubItem,int nMask,int nFmt)
{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	lvc.cx = pclView->GetStringWidth(lvc.pszText) + 15;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return pclView->InsertColumn(nItem,&lvc);
}

BOOL CDeviceConfigDlg::AddItem(CListCtrl  *pclView, int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	lvItem.pszText = (LPTSTR) strItem;
	if(nImageIndex != -1){
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage |= LVIF_IMAGE;
	}
	if(nSubItem == 0)
		return pclView->InsertItem(&lvItem);
	return pclView->SetItem(&lvItem);
}

BOOL CDeviceConfigDlg::putsettingsDirect (LPCTSTR atchDeviceName, CString &csPort, CString &csSettings)
{
	TCHAR atchSetting[SCF_USER_BUFFER];
	CString csTemp;
	DWORD  dwRet, dwCount;

	CScfParameters myScfParms (csSettings);      // in CDeviceConfigDlg::putsettingsDirect()

	TRACE2("  putsettingsDirect\n    csBaud = %s  csCharBits = %s\n", myScfParms.csBaud, myScfParms.csCharBits);
	TRACE3("     csParity = %s  csStopBits %s  csFlowControl %s\n", myScfParms.csParity, myScfParms.csStopBits, myScfParms.csFlowControl);
	TRACE2("     csIPAddr = %s  csPortNo %s\n", myScfParms.csIPAddr, myScfParms.csPortNo);

	csPort.TrimRight ();
	csTemp = csPort.Mid (3);
	_tcscpy (atchSetting, csTemp);
	TRACE2("    csPort = %s, csTemp = %s\n", csPort, csTemp);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PORT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);

	if (myScfParms.csBaud == _T("") || myScfParms.csCharBits == _T("") || myScfParms.csParity == _T("") || myScfParms.csStopBits == _T("")) {
		if (myScfParms.csIPAddr == _T("") || myScfParms.csPortNo == _T("")) {
			return FALSE;
		}
	}

	_tcscpy (atchSetting, myScfParms.csIPAddr);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPADDR, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csPortNo);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPPORT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csBaud);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_BAUD, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csCharBits);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_CHARBIT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csParity);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PARITY, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csStopBits);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_STOPBIT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	_tcscpy (atchSetting, myScfParms.csFlowControl);
	dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_HANDSHAKE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);

	return TRUE;
}

BOOL CDeviceConfigDlg::putsettingsEthernet (LPCTSTR atchDeviceName, CString &csSettings)
{
	TCHAR atchSetting[SCF_USER_BUFFER];
	CString csTemp;
	DWORD  dwRet, dwCount;

	TRACE1("  putsettingsEthernet: atchDeviceName = %s\n", atchDeviceName);

	CScfParameters myScfParms (csSettings);    // in CDeviceConfigDlg::putsettingsEthernet().

	TRACE2("    csIPAddr = %s  csPortNo = %s\n", myScfParms.csIPAddr, myScfParms.csPortNo);
	TRACE3("    csIPAddr2 = %s  csIPAddr3 = %s  csIPAddr4 = %s\n", myScfParms.csIPAddr2, myScfParms.csIPAddr3, myScfParms.csIPAddr4);

	csTemp = atchDeviceName;

	if (myScfParms.csPortNo.IsEmpty ()) {
		_tcscpy (atchSetting, myScfParms.csPortNo);
		dwRet = ScfResetActiveDevice(SCF_DATANAME_IPADDR, atchDeviceName, atchSetting);
	}
	else {
		_tcscpy (atchSetting, myScfParms.csPortNo);
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PORT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	if (myScfParms.csIPAddr.IsEmpty ()) {
		_tcscpy (atchSetting, myScfParms.csIPAddr);
		dwRet = ScfResetActiveDevice(SCF_DATANAME_IPADDR, atchDeviceName, atchSetting);
	}
	else {
		_tcscpy (atchSetting, myScfParms.csIPAddr);
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPADDR, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	if (myScfParms.csIPAddr2.IsEmpty ()) {
		_tcscpy (atchSetting, myScfParms.csIPAddr2);
		dwRet = ScfResetActiveDevice(SCF_DATANAME_IPADDR2, atchDeviceName, atchSetting);
	}
	else {
		_tcscpy (atchSetting, myScfParms.csIPAddr2);
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPADDR2, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	if (myScfParms.csIPAddr3.IsEmpty ()) {
		_tcscpy (atchSetting, myScfParms.csIPAddr3);
		dwRet = ScfResetActiveDevice(SCF_DATANAME_IPADDR3, atchDeviceName, atchSetting);
	}
	else {
		_tcscpy (atchSetting, myScfParms.csIPAddr3);
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPADDR3, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	if (myScfParms.csIPAddr4.IsEmpty ()) {
		_tcscpy (atchSetting, myScfParms.csIPAddr4);
		dwRet = ScfResetActiveDevice(SCF_DATANAME_IPADDR4, atchDeviceName, atchSetting);
	}
	else {
		_tcscpy (atchSetting, myScfParms.csIPAddr4);
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IPADDR4, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	return TRUE;
}
BOOL CDeviceConfigDlg::puteptsetting (LPCTSTR atchDeviceName)
{
	DWORD  dwRet, dwCount;
	TCHAR atchSetting[SCF_USER_BUFFER];

	memset(atchSetting, 0, sizeof(atchSetting));
	if (EPTsettings.MerchantID[0] != 0) {
		memcpy (atchSetting, EPTsettings.MerchantID, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_MERCHANTID, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.KeyManagement[0] != 0) {
		memcpy (atchSetting, EPTsettings.KeyManagement, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_KEYMANAGETYPE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.MasterKeyIndex[0] != 0) {
		memcpy (atchSetting, EPTsettings.MasterKeyIndex, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_MASTERKEYINDEX, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.WorkingKey[0] != 0) {
		memcpy (atchSetting, EPTsettings.WorkingKey, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_WORKINGKEY, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.PadIdlePrompt[0] != 0) {
		memcpy (atchSetting, EPTsettings.PadIdlePrompt, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_IDLEPROMPT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.PINPadPort[0] != 0) {
		memcpy (atchSetting, EPTsettings.PINPadPort, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PORT, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.ShouldRotate[0] != 0) {
		memcpy (atchSetting, EPTsettings.ShouldRotate, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_SHOULDROTATE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.PINPadDevice[0] != 0) {
		memcpy (atchSetting, EPTsettings.PINPadDevice, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PINPAD, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.PDC[0] != 0) {
		memcpy (atchSetting, EPTsettings.PDC, sizeof(atchSetting));
		dwRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_PDC, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.MagneticType[0] != 0) {
		memcpy (atchSetting, EPTsettings.MagneticType, sizeof(atchSetting));
		dwRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_MAGSTRIPETYPE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.ReaderType[0] != 0) {
		memcpy (atchSetting, EPTsettings.ReaderType, sizeof(atchSetting));
		dwRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_CHECKREADTYPE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.MagReaderTimeout[0] != 0) {
		memcpy (atchSetting, EPTsettings.MagReaderTimeout, sizeof(atchSetting));
		dwRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_MAGSTRIPETO, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}	
	if (EPTsettings.CheckReaderTimeout[0] != 0) {
		memcpy (atchSetting, EPTsettings.CheckReaderTimeout, sizeof(atchSetting));
		dwRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_CHECKREADTO, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}
	if (EPTsettings.StoreForwardName[0] != 0) {
		memcpy (atchSetting, EPTsettings.StoreForwardName, sizeof(atchSetting));
		dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_STOREFORWARDFILE, SCF_DATA_STRING, atchSetting, _tcslen(atchSetting) * sizeof(TCHAR), &dwCount);
	}

	return TRUE;
}

BOOL CDeviceConfigDlg::MakeSettingsString (LPCTSTR atchDeviceName, CString &csSettings)
{
	TCHAR atchSetting [SCF_USER_BUFFER];
	DWORD dwRet, dwCount, dwDataType;
	BOOL bRet = FALSE;

	csSettings = _T("");
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_BAUD, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		bRet = TRUE;
		csSettings = csSettings + atchSetting + _T(",");
	}
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_CHARBIT, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting + _T(",");
	}
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_PARITY, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting + _T(",");
	}
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_STOPBIT, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting + _T(",");
	}
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_HANDSHAKE, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting;
	}

	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_IPADDR, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting + _T(",");
	}
	dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_IPPORT, &dwDataType, atchSetting, sizeof(atchSetting), &dwCount);
	if (dwRet == SCF_SUCCESS && dwCount > 0) {
		csSettings = csSettings + atchSetting;
	}
	return bRet;
}

void CDeviceConfigDlg::ProcessConfigurationFile (void)
{
	TCHAR   atchDeviceOptions[SCF_USER_BUFFER];
	TCHAR   atchDeviceOptions2[SCF_USER_BUFFER];
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];
	DWORD   dwCount, dwDataType;
    int     dwRet, nItem = 1;

	memset(&atchDeviceOptions, 0x00, sizeof(atchDeviceOptions));
	memset(&atchDeviceOptions2, 0x00, sizeof(atchDeviceOptions2));

	// Read in the standard configuration file and then review the contents
	// of the Registry against what is in the configuration file.
	// If there are items in the configuration file that are not in the Registry
	// then we will add them as part of the initialization.
	//
	// First of all we make sure that if the type of active device specified exists.  If not, add it to
	// the Registry list of possible active devices.
	//
	// Next we check to see if it is not an OPOS device.  If not, then we check to see if already exists in
	// the SystemParameters part of the Registry.  If not, then we go ahead and add it so that it will be
	// available for provisioning later.
	ReadInConfigFile ();
	if (! myScfList.myScfList.IsEmpty ()) {
		CScfInterface *myScf;
		POSITION myPos = myScfList.myScfList.GetHeadPosition ();

		while (myPos) {
			myScf = myScfList.myScfList.GetNext (myPos);
			if (! myScf->csDeviceActiveType.IsEmpty ()) {
				TRACE2(" myScfList.myScfList.GetNext\n    csDeviceActiveType = %s   csDeviceInterface = %s \n", myScf->csDeviceActiveType, myScf->csDeviceInterface);
				dwRet = ScfGetActiveDevice(myScf->csDeviceActiveType, &dwCount, atchDeviceName, atchDllName);
				// If the Active device type does not exist.  Make one that is empty.
				if (dwRet != SCF_SUCCESS && dwRet != SCF_NO_DATA) {
					dwRet = ScfSetActiveDevice(myScf->csDeviceActiveType, NULL, NULL);
				}

				// Check the Interface Type (SCF_INTERFACE_DIRECT...etc), if the type
				// isn't there, then put the appropriate value - TLDJR
//				dwRet = ScfGetParameter(myScf->csDeviceName, SCF_DATANAME_PRINTPORT, &dwDataType, atchDeviceName, sizeof(atchDeviceName), &dwCount);
//				if ( dwRet != SCF_SUCCESS )
//				{
					_tcscpy (atchDeviceName, myScf->csDeviceInterface);
					dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);					
//				}

				// if the specific device type in SystemParameters doesn't exist, add it to SystemParameters
				// if it is DIRECT, ETHERNET, or NOTE.  OPOS devices are from the NCR OLE Retail SDK and
				// are installed differently.  These are devices NHPOS manages itself.
				if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_OPOS) != 0) {
					dwRet = ScfGetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDeviceName, sizeof(atchDeviceName), &dwCount);
					if (dwRet != SCF_SUCCESS) {
						if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_DIRECT) == 0) {
							CScfParameters myScfParms (myScf->csDeviceSettings);    // in CDeviceConfigDlg::ProcessConfigurationFile()
							_tcscpy (atchDeviceName, myScfParms.csBaud);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_BAUD, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csCharBits);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_CHARBIT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csParity);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PARITY, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csStopBits);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_STOPBIT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csFlowControl);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_HANDSHAKE, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csIPAddr);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPADDR, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csPortNo);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceDLL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceActiveType);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_TYPENAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, SCF_INTERFACE_DIRECT);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
						}
						else if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_ETHERNET) == 0) {
							CScfParameters myScfParms (myScf->csDeviceSettings);    // in CDeviceConfigDlg::ProcessConfigurationFile()
							_tcscpy (atchDeviceName, myScfParms.csIPAddr);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPADDR, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScfParms.csPortNo);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceDLL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceActiveType);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_TYPENAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, SCF_INTERFACE_ETHERNET);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
						}
						else if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_NOTE) == 0) {
							_tcscpy (atchDeviceName, myScf->csDeviceDLL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceActiveType);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_TYPENAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, SCF_INTERFACE_NOTE);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
						}
						else if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_SPECIAL) == 0) {
							CScfParameters myScfParms (myScf->csDeviceSettings);    // in CDeviceConfigDlg::ProcessConfigurationFile()
							_tcscpy (atchDeviceName, myScfParms.csIPAddr);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPADDR, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							//only add the second IP address key if it is a mercury payment
							if(myScf->csDeviceName == SCF_DEVICENAME_EPTEXDLL_MERCURY/*"Mercury Payments EPT"*/)
							{
								_tcscpy (atchDeviceName, myScfParms.csIPAddr2);
								dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPADDR2, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							}
							else if(myScf->csDeviceName == SCF_DEVICENAME_EPTEXDLL_SECURE /*"DataCap Secure"*/ || myScf->csDeviceName == SCF_DEVICENAME_EPTEXDLL_SECURETOKENIZED /*"DataCap Secure Tokenized"*/)
							{
								_tcscpy (atchDeviceName, myScfParms.csIPAddr2);
								dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_IPADDR2, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							}
							_tcscpy (atchDeviceName, myScfParms.csPortNo);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceDLL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceActiveType);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_TYPENAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, SCF_INTERFACE_SPECIAL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
						}
						else if (myScf->csDeviceInterface.Compare (SCF_INTERFACE_WEDGE) == 0) {
							_tcscpy (atchDeviceName, myScf->csDeviceDLL);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_DLLNAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, myScf->csDeviceActiveType);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_TYPENAME, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
							_tcscpy (atchDeviceName, SCF_INTERFACE_WEDGE);
							dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, atchDeviceName, _tcslen(atchDeviceName) * sizeof(TCHAR), &dwCount);
						}
					}
				}

				//we check to see if there are any options for this device, if there is, and the option key has not been set
				//we set the key in the registry. JHHJ
				if(myScf->csDeviceOptions.Compare(_T("NONE")) !=0)
				{
					TCHAR* optionName;
					TCHAR* optionValue;
					wchar_t *context;

					_tcscpy (atchDeviceOptions, myScf->csDeviceOptions);
					optionName = wcstok(atchDeviceOptions, _T("="), &context);
					optionValue = wcstok(NULL, _T(";"), &context);
					dwRet = ScfGetParameter (myScf->csDeviceName, SCF_DATANAME_OPTION, &dwDataType, atchDeviceName, sizeof(atchDeviceName), &dwCount);

					if(dwRet < 0)
					{
						dwRet = ScfSetParameter (myScf->csDeviceName, SCF_DATANAME_OPTION, SCF_DATA_STRING, optionValue, _tcslen(atchDeviceOptions) * sizeof(TCHAR), &dwCount);
						continue;

					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigDlg message handlers


// this is required to build the wrappers for the msxml3 dll
// notice MSXML3 lives in the MSXML2 namespace (gotta love it)
//
// See also the following postings on StackOverflow concerning MSXML and MSXML versions.
//    https://stackoverflow.com/questions/1075891/what-became-of-msxml-4-0
//    https://stackoverflow.com/questions/951804/which-version-of-msxml-should-i-use
//

#import <msxml3.dll>

static int ChildNodeField (MSXML2::IXMLDOMNodePtr &pNode, TCHAR *xBuff )
{
	int  iRet = 0;
	TCHAR  *FieldsList[] = {
		0,
		L"Description",
		L"Interface",
		L"NumPadTypes",
		L"SecureDeviceID",     // to match SecureDeviceID1, etc. tag
		L"SecureDevice"        // to match SecureDevice1PadType1, etc. tags
	};

	if (!pNode) return 0;

	for (int i = 1; i < sizeof(FieldsList)/sizeof(FieldsList[0]); i++) {
		if ((_tcsncmp ( W2T(pNode->GetnodeName()), FieldsList[i], _tcslen(FieldsList[i])) == 0 ? i : 0)) {
			_tcscpy (xBuff, W2T(pNode->Gettext()));
			return i;
		}
	}
	
	return 0;
}

static BOOL RecurseDeviceList (MSXML2::IXMLDOMNodePtr &pNode, CScfList::DevListMap &devMap)
{
	TCHAR  xBuff[256] = {0};
	if (!pNode) return TRUE;

	CArray <CString> *p = new CArray <CString>;
	CString arrayKey;

	MSXML2::IXMLDOMNodePtr pNodeChild = pNode->GetfirstChild();
	for ( ; pNodeChild; pNodeChild = pNodeChild->GetnextSibling()) {
		int i = ChildNodeField (pNodeChild, xBuff);
		if (i == 1) {
			TRACE2 ("Child  %d  %s\n", i, xBuff);
		} else {
			switch (i) {
				case 4:
					arrayKey = xBuff;
					break;
				case 5:
					p->Add(CString(xBuff));
					break;
			}
			TRACE2 ("       %d  %s\n", i, xBuff);
		}
	}

	devMap[(LPCTSTR)arrayKey] = p;

	return RecurseDeviceList (pNode->GetnextSibling(), devMap);
}

BOOL CDeviceConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	memset(&EPTsettings, 0, sizeof(EPTsettings));

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Terminal Information Variables Set
		// In response to SR 495 the following information was added to the Environment
		//	Setup Menu. Added was the Terminal Host Name, Error Checking and Handling of
		//	the Terminal Number, and also the IP Address will be displayed in the dialog
		//	window. RZACHARY
		int			i;
		CHAR	    pszHostName[32] = "";
		CHAR		*pszIPAddress;
		TCHAR		szHostData[32] = {0};
		TCHAR		szTermNum[3] = {0, 0, 0};
		TCHAR		szIpAddr[STD_MAX_NUM_DEVICE] = _T("192.0.0.1");

		gethostname(pszHostName, sizeof(pszHostName));
		pszHostName[31] = '\0';

		for( i = 0; i < sizeof(pszHostName); i++){
			szHostData[i] = pszHostName[i];
			if(pszHostName[i] == '-'){
				if(pszHostName[i+1] >= '0'){
					szTermNum[0] = pszHostName[i+1];
					szTermNum[1] = pszHostName[i+2];
				}
			}
		}

		// WARNING: gethostbyname() may fail if a terminal has been turned off for several minutes
		//          or if after being off for some time, the terminal has just been turned on.
		//          there is a chance the host name information in the directory may have gone
		//          stale due to lack of communication with a terminal and been discarded.
		struct hostent *ipaddr = gethostbyname(pszHostName);
		if (ipaddr) {
			for(i = 0; ipaddr->h_addr_list[i] != 0; i++){
				struct in_addr addr;
				memcpy(&addr, ipaddr->h_addr_list[i], sizeof(struct in_addr));
				pszIPAddress = inet_ntoa(addr);
			}
			for(i = 0; i < STD_MAX_NUM_DEVICE; i++){
				szIpAddr[i] = pszIPAddress[i];
			}
			if (szTermNum[0]) {
				this->SetDlgItemText(IDC_TERMNUM_ERR_TF, szTermNum );
			}
			else {
				this->SetDlgItemText(IDC_TERMNUM_ERR_TF, _T("Not Setup"));
			}
			this->SetDlgItemText(IDC_IPADDR_TF, szIpAddr );
		}
		else {
			this->SetDlgItemText(IDC_TERMNUM_ERR_TF, _T("Not Setup"));
			this->SetDlgItemText(IDC_IPADDR_TF, _T("Not Found"));
		}

		this->SetDlgItemText(IDC_COMPNAME_TF, szHostData);

		this->SetDlgItemText(IDC_STATIC_SERIALNUMBER, m_BiosSerialNumber);

		//Version Number Information
			//In response to SR 607 the NHPOS version Number has been added to the Device Config
			// Dialog Window. RZACHARY
		DWORD dwHandle;
		DWORD dwLen = 0;
		unsigned int puLen;
		TCHAR	pInfoBuf[1024];
		TCHAR versionNumber[40];

		VS_FIXEDFILEINFO *lpBuf;

		TCHAR *myFileName = _T("C:\\FlashDisk\\NCR\\Saratoga\\Program\\Framework.exe");
		memset(versionNumber, 0x00, sizeof(versionNumber));

		dwLen = GetFileVersionInfoSize(myFileName, &dwHandle);
		GetFileVersionInfo(myFileName,dwHandle,sizeof(pInfoBuf), pInfoBuf);

		if( dwLen != 0){
			VerQueryValue(pInfoBuf,_T("\\"),(void **)&lpBuf, &puLen);

			_stprintf(versionNumber, _T("%2.2d.%2.2d.%2.2d.%2.2d\n"),
					HIWORD(lpBuf->dwFileVersionMS), LOWORD(lpBuf->dwFileVersionMS),
					HIWORD(lpBuf->dwFileVersionLS), LOWORD(lpBuf->dwFileVersionLS));

			this->SetDlgItemText(IDC_VERSIONNUMBER_TF, versionNumber);
		}
		else{
			this->SetDlgItemText(IDC_VERSIONNUMBER_TF, _T("NHPOS NOT FOUND"));
		}


		// TODO: Add extra initialization here

    ProcessConfigurationFile ();

	TCHAR   atchDeviceOptions[SCF_USER_BUFFER];
	TCHAR   atchDeviceOptions2[SCF_USER_BUFFER];
    TCHAR   atchDeviceName[SCF_USER_BUFFER];
    TCHAR   atchDllName[SCF_USER_BUFFER];
    TCHAR   *patchDeviceName;
    TCHAR   *patchDllName;
	DWORD   dwCount, dwDataType;
    int     dwRet, nItem = 1;

	memset(&atchDeviceOptions, 0x00, sizeof(atchDeviceOptions));
	memset(&atchDeviceOptions2, 0x00, sizeof(atchDeviceOptions2));

	CString myControlType;
	CString myControlInterface;
    TCHAR   portname[SCF_USER_BUFFER];
    TCHAR   portname2[SCF_USER_BUFFER];

	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_DEVICES);

	myControlType.LoadString (IDS_HEADING_1);
	AddColumn (pclView, myControlType, 0);
	myControlType.LoadString (IDS_HEADING_2);
	AddColumn (pclView, myControlType, 1);
	myControlType.LoadString (IDS_HEADING_3);
	AddColumn (pclView, myControlType, 2);
	myControlType.LoadString (IDS_HEADING_4);
	AddColumn (pclView, myControlType, 3);
	myControlType.LoadString (IDS_HEADING_5);
	AddColumn (pclView, myControlType, 4);
	myControlType.LoadString (IDS_HEADING_6);
	AddColumn (pclView, myControlType, 5);

	CString myPortSettings;
	DWORD   dwCountActive = 0;
	PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;
	TCHAR buffer[SCF_USER_BUFFER];

	for (nItem = 0; pMyDeviceList->aszDevName; pMyDeviceList++) {
		CString csName = pMyDeviceList->aszDevName;
		DWORD dwRetVal;//,dwDataType,dwCount;

		atchDeviceName[0] = _T('\0');
		atchDllName[0] = _T('\0');
		dwRet = ScfGetActiveDevice(pMyDeviceList->aszDevName, &dwCountActive, atchDeviceName, atchDllName);

		CString cspatchDeviceName = atchDeviceName;

		buffer[0] = _T('\0');
		dwRetVal = ScfGetParameter(cspatchDeviceName, SCF_DATANAME_PRINTPORT, &dwDataType, buffer, sizeof(buffer), &dwCount);
		CString csInterface = CString(buffer);
		
		// Sometimes the SCF_DATANAME_PRINTPORT parameter isn't set. This is
		// ususally because a customer was using a previous version of deviceconfig
		// that didn't have that functionality. if the registry key isn't there,
		// or it is empty, then set csInterface to the appropriate value - TLDJR
		if ( dwRetVal != SCF_SUCCESS || csInterface.IsEmpty() )
		{
			switch( pMyDeviceList->deviceInterface )
			{
			case Ignore: 
				break;
			case DIRECT:
				csInterface = SCF_INTERFACE_DIRECT;
				break;
			case OPOS:
				csInterface = SCF_INTERFACE_OPOS;
				break;
			case ETHERNET:
				csInterface = SCF_INTERFACE_ETHERNET;
				break;
			case NOTE:
				csInterface = SCF_INTERFACE_NOTE;
				break;
			case DLL:
				csInterface = SCF_INTERFACE_DLL;
				break;
			case SPECIAL:
				csInterface = SCF_INTERFACE_SPECIAL;
				break;
			case WEDGE:
				csInterface = SCF_INTERFACE_WEDGE;
				break;
			}
			
			_tcscpy (buffer, csInterface);
			dwRetVal = ScfSetParameter (atchDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, buffer, _tcslen(buffer) * sizeof(TCHAR), &dwCount);
		}

		  TRACE3("ScfGetActiveDevice %d %s  Name %s  !\n", nItem, pMyDeviceList->aszDevName, atchDeviceName);
		  TRACE1("     dwRet = %d !\n", dwRet);
		  TRACE1("     pMyDeviceList %d!\n", pMyDeviceList->nItem);
		  TRACE2("     Dev type %s   Dev Name %s!\n", pMyDeviceList->aszDevName, atchDeviceName);

		myControlType.LoadString (enumControlType[pMyDeviceList->scfType]);
		myControlInterface.LoadString ( enumInterfaceType[pMyDeviceList->deviceInterface]);

		patchDeviceName = atchDeviceName;
		patchDllName = atchDllName;

		DWORD  dwCountSave = dwCountActive;


		// We allow multiple devices for a particular types of devices.
		// dwCountActive = pMyDeviceList->dwMaxDevices;

		DWORD  dwRet2;
		while (dwCountActive > 0) {
			AddItem (pclView, nItem, 0, pMyDeviceList->aszDevName);
			AddItem (pclView, nItem, 2, myControlType);
			AddItem (pclView, nItem, 5, myControlInterface);

			TRACE2("     pMyDeviceList->scfType %d  type IDS = %d!\n", pMyDeviceList->scfType, enumControlType[pMyDeviceList->scfType]);
			// OPOS control info is stored in ActiveDevices list with a string name of the type such as CashDrawer
			// and a string value of the actual control such as NCRCashDrawer.1 which means that we need to use
			// the atchDllName from ScfGetActiveDevice to fill in column 1, Assigned to Name, for OPOS controls
			if (pMyDeviceList->deviceInterface == DIRECT && csInterface == SCF_INTERFACE_DIRECT) {
				pMyDeviceList->pDialogOptions = new CDialogOptions ();
				pMyDeviceList->pDialogOptions->csDeviceNameOption = patchDeviceName;
				pMyDeviceList->pDialogOptions->csDeviceName = patchDeviceName;
				pMyDeviceList->pDialogOptions->readOptions ();

				AddItem (pclView, nItem, 1, patchDeviceName);
    			dwRet2 = ScfGetParameter (patchDeviceName, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
				if (dwRet2 == SCF_SUCCESS || dwRet == SCF_NO_DATA) {
					CString csPortName;
					csPortName = _T("COM");
					csPortName += portname;
					AddItem (pclView, nItem, 3, csPortName);
					MakeSettingsString (patchDeviceName, myPortSettings);
					AddItem (pclView, nItem, 4, myPortSettings);
				}
			}
			else if (pMyDeviceList->deviceInterface == OPOS && csInterface == SCF_INTERFACE_OPOS) {
				AddItem (pclView, nItem, 1, patchDllName);
				AddItem (pclView, nItem, 3, pMyDeviceList->aszRegistryKey);
				// ---------------------------------
				// if this is a special OPOS device with specific provisioning needs put its dialog assignment here
				if (_tcscmp (pMyDeviceList->aszDevName, SCF_TYPENAME_PINPAD1) == 0) {
					m_pPinPadListDialog = pMyDeviceList->pDialogOptions = new COposPinPadOptions;
					m_pPinPadListDialog->csDeviceName = patchDeviceName;
					m_pPinPadListDialog->readOptions ();
				}
				// ---------------------------------
			}
			else if (pMyDeviceList->deviceInterface == ETHERNET) {
			    AddItem (pclView, nItem, 1, patchDeviceName);

				CString tempStr;
				tempStr = patchDeviceName;
				
				memset(portname, _T('\0'), sizeof(SCF_USER_BUFFER));
				memset(portname2, _T('\0'), sizeof(SCF_USER_BUFFER));

				dwRet = ScfGetParameter (patchDeviceName, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
				dwRet2 = ScfGetParameter (patchDeviceName, SCF_DATANAME_IPADDR, &dwDataType, portname2, sizeof(portname2), &dwCount);

				myPortSettings.Format( _T("%s,"), portname2);
				myPortSettings += portname;

				AddItem (pclView, nItem, 4, myPortSettings);
			}
			else if (pMyDeviceList->deviceInterface == DLL || pMyDeviceList->deviceInterface == SPECIAL) {
			    AddItem (pclView, nItem, 1, patchDeviceName);
				CString tempStr;
				tempStr = patchDeviceName;
				
				memset(portname, 0, sizeof(SCF_USER_BUFFER));
				memset(portname2, 0, sizeof(SCF_USER_BUFFER));

				dwRet = ScfGetParameter (patchDeviceName, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
				dwRet2 = ScfGetParameter (patchDeviceName, SCF_DATANAME_IPADDR, &dwDataType, portname2, sizeof(portname2), &dwCount);

				myPortSettings.Format( _T("%s,"), portname2);
				myPortSettings += portname;

				AddItem (pclView, nItem, 4, myPortSettings);

				// ---------------------------------
				// if this is a special DLL device with specific provisioning needs put its dialog assignment here
				if (_tcscmp (pMyDeviceList->aszDevName, SCF_TYPENAME_EPT_EX_DLL) == 0) {
				}
				// ---------------------------------
			}
			else if (pMyDeviceList->deviceInterface == WEDGE) {
			    AddItem (pclView, nItem, 1, patchDeviceName);
				CString tempStr;
				tempStr = patchDeviceName;
				
				memset(portname, 0, sizeof(SCF_USER_BUFFER));
				memset(portname2, 0, sizeof(SCF_USER_BUFFER));

				dwRet = ScfGetParameter (patchDeviceName, SCF_DATANAME_WEDGE_ISWEDGE, &dwDataType, portname, sizeof(portname), &dwCount);
				if (dwRet > 0) {
					dwRet = ScfGetParameter (patchDeviceName, SCF_DATANAME_WEDGE_PREFIX, &dwDataType, portname, sizeof(portname), &dwCount);
					dwRet2 = ScfGetParameter (patchDeviceName, SCF_DATANAME_WEDGE_SUFFIX, &dwDataType, portname2, sizeof(portname2), &dwCount);

					myPortSettings.Format( _T("%s,"), portname2);
					myPortSettings += portname;
				}
				else {
					myPortSettings = _T("");
				}

				AddItem (pclView, nItem, 4, myPortSettings);
			}
			patchDeviceName += _tcslen(patchDeviceName) + 1;
			patchDllName += _tcslen(patchDllName) + 1;
			nItem++;
			dwCountActive--;
		}

		while (dwCountSave < pMyDeviceList->dwMaxDevices) {
			AddItem (pclView, nItem, 0, pMyDeviceList->aszDevName);
			AddItem (pclView, nItem, 2, myControlType);
			AddItem (pclView, nItem, 5, myControlInterface);
			dwCountSave++;
			nItem++;
		}
	}

	pclView->SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	/*===========================================================
		Initial call to open and close database. If this
		call is not made and a user does not perform an
		initial reset and Delete the PLU Total Database
		the application will cause an exception on exit. ESMITH
	============================================================*/
	PLUTOTAL_DB_HANDLE      g_hdDB; /* database handle*/

	g_hdDB = PLUTOTAL_HANDLE_NULL;
	PluTotalOpen(&g_hdDB,TRUE,FALSE);
	PluTotalClose(g_hdDB);


	//SR 490 & 491, We do not want device config to be able to run while NHPOS is running
	//If it IS running while NHPOS is already running, it will be in a "read only" mode
	//where the user can read the information in device config, but cannot make any new changes
	HANDLE hMutex;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, _T("NeighborhoodPOSApp"));

	//If the mutex does not exist, we will create it, because this is the first time that
	//the application has been created.
	if (!hMutex)
	{
	  // Mutex doesn’t exist. This is
	  // the first instance so create
	  // the mutex.
	  hMutex = CreateMutex(0, 0, _T("DeviceConfigApp"));
	}else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CHANGE_DEVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SETTINGS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DISCONNECTED_SAT_PARAMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHANGE_OPTIONS)->EnableWindow(FALSE);
	}

	//SR 606 Add the ability to add and remove NHPOS from starting up
	shortCut_NHPOS.setLinkDescription(_T("NCR General POS Point Of Sale Application Software"));
	shortCut_NHPOS.setLinkFileName(_T("NCR General POS Application"));
	shortCut_NHPOS.setLinkTarget(_T("C:\\FlashDisk\\NCR\\Saratoga\\Program\\Framework.exe"));
	shortCut_NHPOS.setWorkingDirectory(_T("C:\\FlashDisk\\NCR\\Saratoga\\Program\\"));
	shortCut_NHPOS.setLinkPathStartup();

	if (shortCut_NHPOS.isShortCut()) {
		CheckDlgButton(IDC_NHPOS_STARTUP,BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_STARTUP,BST_UNCHECKED);
	}

	m_IsDisconnectedSatellite = 0;
	m_IsJoinedDisconnectedSatellite = 0;

	m_bConnEngineEnabled = 0;
	m_iConnEnginePort = 8282;

	m_bHttpServerEnabled = FALSE;

	{
		TCHAR   atchDeviceName[SCF_USER_BUFFER];
		TCHAR   atchDllName[SCF_USER_BUFFER];
		TCHAR   *pDeviceName;
		TCHAR   *pDllName;
		DWORD   dwRet, dwCount;

		pDeviceName = atchDeviceName;
		pDllName    = atchDllName;

		dwRet = ScfGetActiveDevice(SCF_TYPENAME_HTTPSERVER, &dwCount, pDeviceName, pDllName);

		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			m_bHttpServerEnabled = TRUE;
			CheckDlgButton(IDC_NHPOS_HTTPSERVER, BST_CHECKED);
		}
		else {
			CheckDlgButton(IDC_NHPOS_HTTPSERVER, BST_UNCHECKED);
		}

		pDeviceName = atchDeviceName;
		pDllName    = atchDllName;
		dwRet = ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC, &dwCount, pDeviceName, pDllName);
		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			CEdit  *pclView = (CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_PORT);
			m_bConnEngineEnabled = 0;
			m_iConnEnginePort = 8282;
			for (DWORD dwI = 0; dwI < dwCount; dwI++) {
				if (wcsstr(pDeviceName, SCF_DATANAME_ISENABLED) != NULL) {
					if (_ttoi (pDllName) > 0) {
						m_bConnEngineEnabled = 1;
					}
				}
				if (wcsstr(pDeviceName, SCF_DATANAME_PORT) != NULL) {
					m_iConnEnginePort = _ttoi (pDllName);
				}
				pDeviceName += wcslen(pDeviceName) + 1;
				pDllName += wcslen(pDllName) + 1;
			}

			CString csConnEnginePort;
			csConnEnginePort.Format(_T("%d"), m_iConnEnginePort);
			if (m_bConnEngineEnabled)
				CheckDlgButton(IDC_NHPOS_CONNENGINE, BST_CHECKED);
			else
				CheckDlgButton(IDC_NHPOS_CONNENGINE, BST_UNCHECKED);

			pclView->SetWindowText(csConnEnginePort);
		}
		else {
			CheckDlgButton(IDC_NHPOS_CONNENGINE, BST_UNCHECKED);
		}
	}

	patchDeviceName = atchDeviceName;
	patchDllName = atchDllName;

	dwRet = ScfGetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, &dwCountActive, atchDeviceName, atchDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCountActive)) {
		DWORD  dwI = 0;
		for (dwI = 0; dwI < dwCountActive; dwI++) {
			if (wcsstr(patchDeviceName, SCF_DEVICENAME_JOIN_ISDISCONNECTED) != NULL) {
				if (_ttoi (patchDllName) > 0) {
					m_IsDisconnectedSatellite = 1;
				}
			}

			if (wcsstr(patchDeviceName, SCF_DEVICENAME_JOIN_JOINSTATUS) != NULL) {
				if (_ttoi (patchDllName) > 0) {
					m_IsJoinedDisconnectedSatellite = 1;
				}
			}

			if (wcsstr(patchDeviceName, SCF_DEVICENAME_JOIN_LASTJOINMASTER) != NULL) {
				memset(m_LastJoinMaster, 0, sizeof(m_LastJoinMaster));
				wcsncpy (m_LastJoinMaster, patchDllName, 30);
			}
			patchDeviceName += wcslen(patchDeviceName) + 1;
			patchDllName += wcslen(patchDllName) + 1;
		}
	}

	if (m_IsDisconnectedSatellite) {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_UNCHECKED);
	}

	if (m_IsJoinedDisconnectedSatellite) {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_UNCHECKED);
	}

	m_IsSatelliteOverride = 0;
	memset (m_SatelliteOverride, 0, sizeof(m_SatelliteOverride));
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_OVERRIDENAME_SAT, &dwCountActive, atchDeviceName, atchDllName);
	if ((dwRet == SCF_SUCCESS) && (dwCountActive)) {
		if (wcsstr(atchDeviceName, SCF_DEVICENAME_OVERRIDE_HOSTNAME) != NULL) {
			m_IsSatelliteOverride = (atchDllName[0]) ? 1 : 0;
			_tcsncpy (m_SatelliteOverride, atchDllName, 30);
		}
	}
	if (m_IsSatelliteOverride) {
		CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_UNCHECKED);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeviceConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDeviceConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDeviceConfigDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDeviceConfigDlg::OnDblclkListDevices(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	OnButtonChangeDevice ();
	*pResult = 0;
}

/**

  Read in the DeviceConfig.txt configuration file.

# Each section describes a possible type of device.  There are three main kinds of devices:
#      DIRECT uses the older direct to communications port protocol
#      OPOS uses the new OPOS control protocol which is implemented for a few devices
#      NOTE means that the device doesn't use a control as such and this is a data item
#      WEDGE means that the device is a wedge keyboard type of device
#
# The format of each section is as follows:
#      line 1 -> Contains the name of the device such as NCR 7194 Printer
#                This line is used as a registry key to identify the specific device
#                Ensure it is same as defined string such as SCF_DEVICENAME_EPTEXDLL_MERCURY.
#      line 2 -> the DLL to use if this is a DIRECT device.
#                The registry key beneath OLEforRetail\ServiceOPOS for OPOS devices (for instance CashDrawer
#      line 3 -> the Active device type which describes the type of work the device does
#                This value is one of the Active device registry keys used to find active devices.
#            BEVERAGE -> device is a beverage dispenser
#            CDISPLAY -> device is a customer display or Line Display
#            COIN -> device is a coin dispenser
#            DRAWER -> device is a Cash Drawer
#            KEYBOARD -> device is a keyboard
#            KITCHEN_CRT -> device is a kitchen CRT or Kitchen Display System
#            KITCHEN_PRINTER -> device is a kitchen printer (normally a dot matrix printer)
#            MSR -> device is a magnetic strip reader
#            PRINTER -> device is a POS Printer (normally a thermal receipt printer)
#            SCALE -> device is a scale
#            SCANNER -> device is a scanner
#      line 4 -> the device interface:
#            DIRECT -> device connects to a COMM port and uses a proprietary DLL
#            OPOS -> device connects through an OPOS control
#            ETHERNET -> device is accessed through an Ethernet LAN using an IP address
#            NOTE -> device is none of the above but should be noted.  Mainly used for keyboards.
#            WEDGE -> device uses a keyboard wedge interface.  Mainly used for MSR
#      line 5 has several different uses depending on the device type
#            DIRECT -> COMM port settings for the device baud, bits, parity, stop, handshake (9600,8,NONE,1,NONE)
#            ETHERNET -> IP address and IP port number (192.168.0.23, 2172)
#            OPOS -> the registry subkey for the device profile located under the type (NCRCashDrawer.2 beneath OLEforRetail\ServiceOPOS\CashDrawer)
#	 line 6 -> Contains Options specific for the specified device profile.
# The data in the lines for each device are stored in keys and values in the Windows Registry.
#
# NOTICE:
#   The data that is already stored in the Windows Registry takes precedence over the descriptions in this
#   file so you will need to delete any items that need to be reset using changes that have been made to this file.
#
# The data is first stored in HKEY_LOCAL_MACHINE/SOFTWARE/NCR/Platform Software/SystemConfig/SystemParameter
# The first line containing the name of the device is used to create a key in SystemParameter with that device name.
# The following lines are used to add values to the key.
#   - line 2 is put into DLLName
#   - line 3 is put into TypeName
#   - line 4 is put into ActivePort
#   - line 5 is ignored for OPOS and WEDGE devices
#   - line 5 is parsed and put into the following items for DIRECT devices
#      . Baud
#      . CharBit
#      . Parity
#      . StopBit
#      . HandShake
#   - line 5 is parsed and put into the following items for ETHERNET devices
#      . IPAddr
#      . Port
#   - line 6 will vary
#
# OPOS devices uses the profiles stored in HKEY_LOCAL_MACHINE/SOFTWARE/OLEForRetail/ServiceOPOS
# These profiles are setup using the NCR Retail Services Platform application
#
**/
void CDeviceConfigDlg::ReadInConfigFile (void)	//called when the Device Config program is run 
{
	CStdioFile myConfigFile;

	try {
		if (myConfigFile.Open (_T("DeviceConfig.txt"), CFile::modeRead | CFile::typeText)) {
			CString myBuffer;
			int nPound;
			CString myDeviceEntry [6];
			int nEntryNo = 0;
			try {
				while(myConfigFile.ReadString (myBuffer)){
					nPound = myBuffer.Find (_T('#'), 0);
					if (nPound >= 0)
						myBuffer = myBuffer.Left (nPound);
					myBuffer.TrimLeft ();
					myBuffer.TrimRight ();
					if (myBuffer.IsEmpty () || myBuffer == _T("") ) {
						if (nEntryNo > 3) {
							CScfInterface *myScf = new (CScfInterface);
							myScf->csDeviceName = myDeviceEntry[0];
							myScf->csDeviceDLL = myDeviceEntry[1];
							myScf->csDeviceActiveType = myDeviceEntry[2];
							myScf->csDeviceInterface = myDeviceEntry[3];
							myScf->csDeviceSettings = myDeviceEntry[4];
							myScf->csDeviceOptions = myDeviceEntry[5];
							if (myScf->csDeviceName.Compare (_T("genpos web portal")) == 0) {
								m_genposurl = myScf;
								TRACE2("ReadInConfigFile  CScfInterface special \n    csDeviceName = %s csDeviceDLL = %s\n", myScf->csDeviceName, myScf->csDeviceDLL);
								nEntryNo = 0;
								continue;
							}
							myScfList.myScfList.AddTail (myScf);
							TRACE2("ReadInConfigFile  CScfInterface \n    csDeviceName = %s csDeviceDLL = %s\n", myScf->csDeviceName, myScf->csDeviceDLL);
							TRACE3("    csDeviceActiveType = %s csDeviceInterface = %s  csDeviceSettings = %s\n", myScf->csDeviceActiveType, myScf->csDeviceInterface, myScf->csDeviceSettings);
						}
						nEntryNo = 0;
						continue;
					}
					else {
						myDeviceEntry[nEntryNo] = myBuffer;
						nEntryNo++;
					}
				}
			}
			catch (CFileException *myE) {
				if (myE->m_cause != CFileException::endOfFile ) {
				}
				myE->Delete();
			}
			myConfigFile.Close ();
		}
	}
	catch (CFileException *myE) {
		if (myE->m_cause != CFileException::endOfFile ) {
		}
		myE->Delete();
	}
}


void CDeviceConfigDlg::OnClickListDevices(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

//	GetDlgItem(IDC_CHANGE_OPTIONS)->EnableWindow(FALSE);

	*pResult = 0;
}


void CDeviceConfigDlg::OnButtonSettings()	//called when the "Change Port Settings" button is clicked
{
	// TODO: Add your control notification handler code here
	CListCtrl  *pclView = (CListCtrl *)GetDlgItem (IDC_LIST_DEVICES);
	POSITION pos = pclView->GetFirstSelectedItemPosition();

	if (pos == NULL) {
		TRACE0("No items were selected!\n");
	}
	else
	{
		CString    atchOPOSCOMMText;
		CDialogPortSettings myDevListDialog;
		CDialogDeviceList myDevTypeDialog;

		CString    atchOPOS;
		atchOPOS.LoadString (IDS_CONTROL_TYPE_OPOS);
		CString    atchDirect;
		atchDirect.LoadString (IDS_CONTROL_TYPE_DIRECT);
		CString    atchEthernet;
		atchEthernet.LoadString (IDS_CONTROL_TYPE_ETHERNET);
		CString    atchNote;
		atchNote.LoadString (IDS_CONTROL_TYPE_NOTE);
		CString    atchDLL;
		atchDLL.LoadString (IDS_CONTROL_TYPE_DLL);
		CString    atchSpecial;
		atchSpecial.LoadString (IDS_CONTROL_TYPE_SPECIAL);
		CString    atchWedge;
		atchWedge.LoadString (IDS_CONTROL_TYPE_WEDGE);

	   while (pos)
	   {
			int nItem = pclView->GetNextSelectedItem(pos);

			// we want this to be exactly what was loaded into the CListCtrl from the LoadString ()
			// above.  Being the same, we can then do comparisons.
			myDevListDialog.csInterface = atchOPOSCOMMText = pclView->GetItemText(nItem, 5);

			TRACE1("Item %d was selected!\n", nItem);
			// you could do your own processing on nItem here
			myDevListDialog.port = pclView->GetItemText(nItem, 3);
			myDevListDialog.portSettings = pclView->GetItemText(nItem, 4);
			TRACE2("  Port and Settings are %s  and  %s\n", myDevListDialog.port, myDevListDialog.portSettings);

			//checking to see if device selected was a ept_ex_dll, if so
			//we call ept dialog
			myDevTypeDialog.csDeviceType = pclView->GetItemText(nItem, 0);
			myDevListDialog.csDeviceName = pclView->GetItemText(nItem,1);
			if (myDevTypeDialog.csDeviceType == SCF_TYPENAME_EPT_EX_DLL)
			{
			}
			else if (myDevTypeDialog.csDeviceType == SCF_TYPENAME_PINPAD1)
			{
				if (m_pPinPadListDialog->csDeviceName != myDevListDialog.csDeviceName) {
					m_pPinPadListDialog->csDeviceName = myDevListDialog.csDeviceName;
					m_pPinPadListDialog->readOptions ();
				}
				int iRet = m_pPinPadListDialog->DoModal();
			}
			else{
				int iRet = myDevListDialog.DoModal ();

				if (iRet == IDOK) {
					AddItem (pclView, nItem, 3, myDevListDialog.port);
					AddItem (pclView, nItem, 4, myDevListDialog.portSettings);
				}
			}
		}
	}
}

void CDeviceConfigDlg::OnButtonChangeDevice()	//this is called when the "Change Assigned To" button is clicked 
												//when changing the EPT device
{
	// TODO: Add your control notification handler code here

	CListCtrl  *pclView = (CListCtrl *)GetDlgItem (IDC_LIST_DEVICES);
	POSITION pos = pclView->GetFirstSelectedItemPosition();

	if (pos == NULL) {
		TRACE0("No items were selected!\n");
	}
	else
	{
		CString    atchOPOSCOMMText;
		CString	   interfaceCOMMText;

		CString    atchOPOS;
		atchOPOS.LoadString (IDS_CONTROL_TYPE_OPOS);
		CString    atchDirect;
		atchDirect.LoadString (IDS_CONTROL_TYPE_DIRECT);
		CString    atchEthernet;
		atchEthernet.LoadString (IDS_CONTROL_TYPE_ETHERNET);
		CString    atchNote;
		atchNote.LoadString (IDS_CONTROL_TYPE_NOTE);
		CString    atchDLL;
		atchDLL.LoadString (IDS_CONTROL_TYPE_DLL);
		CString    atchSpecial;
		atchSpecial.LoadString (IDS_CONTROL_TYPE_SPECIAL);
		CString    atchWedge;
		atchWedge.LoadString (IDS_CONTROL_TYPE_WEDGE);

		CString    interfaceOPOS;
		interfaceOPOS.LoadString (IDS_CONTROL_INT_OPOS);
		CString    interfaceDirect;
		interfaceDirect.LoadString (IDS_CONTROL_INT_DIRECT);
		CString    interfaceEthernet;
		interfaceEthernet.LoadString (IDS_CONTROL_INT_ETHERNET);
		CString    interfaceNote;
		interfaceNote.LoadString (IDS_CONTROL_INT_NOTE);
		CString    interfaceDLL;
		interfaceDLL.LoadString (IDS_CONTROL_INT_DLL);
		CString    interfaceSpecial;
		interfaceSpecial.LoadString (IDS_CONTROL_INT_SPECIAL);
		CString    interfaceWedge;
		interfaceWedge.LoadString (IDS_CONTROL_INT_WEDGE);

	   while (pos)
	   {
			CDialogDeviceList myDevListDialog;

			int nItem = pclView->GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			// you could do your own processing on nItem here
			myDevListDialog.csDeviceType = pclView->GetItemText(nItem, 0);
			TRACE1("Item name is %s\n", myDevListDialog.csDeviceType);
			// we want this to be exactly what was loaded into the CListCtrl from the LoadString ()
			// above.  Being the same, we can then do comparisons.
			atchOPOSCOMMText = pclView->GetItemText(nItem, 2);
			interfaceCOMMText = pclView->GetItemText(nItem, 5);
			myDevListDialog.csInterface = interfaceCOMMText;

			if (atchOPOSCOMMText == atchDirect) {
				myDevListDialog.csInterface = SCF_INTERFACE_DIRECT;
			}
			else if (atchOPOSCOMMText == atchOPOS) {
				myDevListDialog.csInterface = SCF_INTERFACE_OPOS;
				PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;

				for (; pMyDeviceList->aszDevName; pMyDeviceList++) {
					if (myDevListDialog.csDeviceType.Compare (pMyDeviceList->aszDevName) == 0) {
						myDevListDialog.csDeviceType = pMyDeviceList->aszRegistryKey;
						break;
					}
				}
			}
			else if (atchOPOSCOMMText == atchEthernet) {
				myDevListDialog.csInterface = SCF_INTERFACE_ETHERNET;
			}
			else if (atchOPOSCOMMText == atchNote) {
				myDevListDialog.csInterface = SCF_INTERFACE_NOTE;
			}
			else if (atchOPOSCOMMText == atchDLL) {
				myDevListDialog.csInterface = SCF_INTERFACE_DLL;
			}
			else if (atchOPOSCOMMText == atchSpecial) {
				myDevListDialog.csInterface = SCF_INTERFACE_SPECIAL;
			}
			else if (atchOPOSCOMMText == atchWedge) {
				myDevListDialog.csInterface = SCF_INTERFACE_WEDGE;
			}
			else {
				return;
			}

			int iRet = myDevListDialog.DoModal ();
			DWORD   dwRet, dwDataType, dwCount;

			CString myPortSettings;
			TCHAR   portname[32];

			if (iRet == IDOK) {
				PDEVICE_LIST_TABLE pMyDeviceList = &myDeviceList[nItem];

				AddItem (pclView, nItem, 1, myDevListDialog.csNewDevice);
				if (myDevListDialog.csNewDevice.Compare (_T("NONE")) != 0) {
					if (interfaceCOMMText == SCF_INTERFACE_DIRECT) {
						dwRet = ScfGetParameter (myDevListDialog.csNewDevice, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
						if (dwRet == SCF_SUCCESS) {
							CString csPortName;
							csPortName = _T("COM");
							csPortName += portname;
							AddItem (pclView, nItem, 3, csPortName);
						}
						if (MakeSettingsString (myDevListDialog.csNewDevice, myPortSettings)) {
							AddItem (pclView, nItem, 4, myPortSettings);
						}
					}
					else if (interfaceCOMMText == SCF_INTERFACE_OPOS) {
						myPortSettings = _T("");
						AddItem (pclView, nItem, 3, myDevListDialog.csDeviceType);
						AddItem (pclView, nItem, 4, myPortSettings);
					}
					else if (interfaceCOMMText == SCF_INTERFACE_ETHERNET) {
						DWORD   dwRet2;
						TCHAR   portname2[32];

						memset (portname, 0, sizeof(portname));
						memset (portname2, 0, sizeof(portname2));

						dwRet = ScfGetParameter (myDevListDialog.csNewDevice, SCF_DATANAME_IPADDR, &dwDataType, portname, sizeof(portname), &dwCount);
						dwRet2 = ScfGetParameter (myDevListDialog.csNewDevice, SCF_DATANAME_PORT, &dwDataType, portname2, sizeof(portname2), &dwCount);

						myPortSettings.Format( _T("%s,"), portname);
						myPortSettings += portname2;
						
						AddItem (pclView, nItem, 4, myPortSettings);
					}
					else if (interfaceCOMMText == SCF_INTERFACE_SPECIAL) {
						DWORD   dwRet2;
						TCHAR   portname2[32];

						memset (portname, 0, sizeof(portname));
						memset (portname2, 0, sizeof(portname2));

						dwRet = ScfGetParameter (myDevListDialog.csNewDevice, SCF_DATANAME_IPADDR, &dwDataType, portname, sizeof(portname), &dwCount);
						dwRet2 = ScfGetParameter (myDevListDialog.csNewDevice, SCF_DATANAME_PORT, &dwDataType, portname2, sizeof(portname2), &dwCount);

						PortSettings = "";
						myPortSettings.Format( _T("%s,"), portname);
						myPortSettings += portname2;
						
						AddItem (pclView, nItem, 4, myPortSettings);
					}
					else {
						myPortSettings = _T("");
						AddItem (pclView, nItem, 3, myPortSettings);
						AddItem (pclView, nItem, 4, myPortSettings);
					}
				}
				else {
					myPortSettings = _T("");
					AddItem (pclView, nItem, 3, myPortSettings);
					AddItem (pclView, nItem, 4, myPortSettings);
				}
			}
		}
	}
}

void CDeviceConfigDlg::OnOK()	//this isn't the internal OK buttons but the final OK button when the Device Config closes
{
	CListCtrl  *pclView = (CListCtrl  *)GetDlgItem (IDC_LIST_DEVICES);
	int nItemCount = pclView->GetItemCount ();
	int nItem;
	DWORD    dwDataType, dwCount, dwRet;
	TCHAR  atchDeviceTemp [SCF_USER_BUFFER], atchDllName [SCF_USER_BUFFER];
	CString    atchDeviceType;
	CString    atchDeviceName;
	CString    atchPortName;
	CString    atchPortSettings;
	CString    atchOPOSCOMMText;

	CString    atchOPOS;
	atchOPOS.LoadString (IDS_CONTROL_TYPE_OPOS);
	CString    atchDirect;
	atchDirect.LoadString (IDS_CONTROL_TYPE_DIRECT);
	CString    atchEthernet;
	atchEthernet.LoadString (IDS_CONTROL_TYPE_ETHERNET);
	CString    atchNote;
	atchNote.LoadString (IDS_CONTROL_TYPE_NOTE);
	CString    atchDLL;
	atchDLL.LoadString (IDS_CONTROL_TYPE_DLL);
	CString    atchSpecial;
	atchSpecial.LoadString (IDS_CONTROL_TYPE_SPECIAL);
	CString    atchWedge;
	atchWedge.LoadString (IDS_CONTROL_TYPE_WEDGE);

	PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;
	for (; pMyDeviceList->aszDevName; pMyDeviceList++) {
		dwRet = ScfResetActiveDeviceAll(pMyDeviceList->aszDevName);
	}

	for (nItem = 0; nItem < nItemCount; nItem++) {
		atchDeviceType = pclView->GetItemText(nItem, 0);
		atchDeviceType.TrimLeft ();
		atchDeviceType.TrimRight ();

		atchDeviceName = pclView->GetItemText(nItem, 1);
		atchDeviceName.TrimLeft ();
		atchDeviceName.TrimRight ();

		// we want this to be exactly what was loaded into the CListCtrl from the LoadString ()
		// above.  Being the same, we can then do comparisons.
		atchOPOSCOMMText = pclView->GetItemText(nItem, 2);

		atchPortName = pclView->GetItemText(nItem, 3);
		atchPortName.TrimLeft ();
		atchPortName.TrimRight ();

		atchPortSettings = pclView->GetItemText(nItem, 4);
		atchPortSettings.TrimLeft ();
		atchPortSettings.TrimRight ();

		// If we are looking at the first printer slot (when nItem = 0)
		// and there is a device provisioned, then it set's the other printer
		// slot to NONE. -TLDJR
		if (nItem == 0 && !(atchDeviceName.IsEmpty () || (atchDeviceName.Compare (_T("NONE")) == 0)))
		{
		   	pclView->SetItemText(nItem+1,1,_T("NONE"));
			pclView->SetItemText(nItem+1,4,_T(""));

			// Set the Active Printer port to OPOS in the registry
			TCHAR tmpBuffer[SCF_USER_BUFFER];
		    DWORD dRet,dwCnt;
	    	_tcscpy(tmpBuffer, atchOPOSCOMMText);
			dRet = ScfSetParameter(atchDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, tmpBuffer, _tcslen(tmpBuffer)*sizeof(TCHAR),&dwCnt);

		}
		else if( nItem == 1 && !(atchDeviceName.IsEmpty () || (atchDeviceName.Compare (_T("NONE")) == 0)))
		{	
			// If we're looking at the second printer (Direct to Comm)
			// then write the active device type
			// Set the Active Printer port to "Direct To COMM in the registry
			TCHAR tmpBuffer[SCF_USER_BUFFER];
		    DWORD dRet,dwCnt;
	    	_tcscpy(tmpBuffer,atchOPOSCOMMText);
			dRet = ScfSetParameter(atchDeviceName,SCF_DATANAME_PRINTPORT,SCF_DATA_STRING,tmpBuffer, _tcslen(tmpBuffer)*sizeof(TCHAR),&dwCnt);
		}

		// If we are looking at an MSR device and the next item on the list is also an
		// MSR device, we check to see if the first one is provisioned. If it is, then 
		// we set the second device to NONE, this stops DeviceConfig from provisioning two MSR devices
		if ( atchDeviceType.CompareNoCase(SCF_OPOS_NAME_MSR) == 0 && (pclView->GetItemText( nItem + 1 , 0)).CompareNoCase(SCF_OPOS_NAME_MSR) == 0 )
		{
			if ( atchDeviceName.CompareNoCase(_T("NONE")) != 0  && !atchDeviceName.IsEmpty() )
			{
				CString csTemp = pclView->GetItemText( nItem + 1 , 1 );
				if ( !csTemp.IsEmpty() && csTemp.CompareNoCase(_T("NONE")) != 0 ) AfxMessageBox(CString("ERROR - Only one MSR device at a time can be provisioned!"));
				pclView->SetItemText( nItem + 1, 1, _T("NONE"));
			}
		}

		TCHAR  xxBuffer[SCF_USER_BUFFER];

		dwRet = ScfGetActiveDevice(atchDeviceType, &dwCount, atchDeviceTemp, atchDllName);
		TRACE3(" ScfGetActiveDevice atchDeviceType %s  atchDeviceTemp %s  atchDllName %s \n", atchDeviceType, atchDeviceTemp, atchDllName);
		if (dwRet == SCF_SUCCESS || dwRet == SCF_NO_DATA) {
			if (atchDeviceName.IsEmpty () || (atchDeviceName.Compare (_T("NONE")) == 0)) {
			}
			else {
				if (atchOPOSCOMMText == atchDirect) {
					if (atchPortName.IsEmpty () || (atchPortName == _T(""))) {
						AfxMessageBox((CString)"ERROR - A COM Port must be provisioned for the: " + atchDeviceName);
						return;
						//continue;
					}
					if (atchPortSettings.IsEmpty () || (atchPortSettings == _T(""))) {
						AfxMessageBox((CString)"ERROR - A COM Port must be provisioned for the: " + atchDeviceName);
						return;
						//continue;
					}
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);
					_tcscpy (xxBuffer, SCF_INTERFACE_DIRECT);
					dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, xxBuffer, _tcslen(xxBuffer) * sizeof(TCHAR), &dwCount);
					putsettingsDirect (pclView->GetItemText(nItem, 1), atchPortName, atchPortSettings);
				}
				else if (atchOPOSCOMMText == atchOPOS) {
					/*===========================================================================
						dwRet = ScfSetActiveDevice(atchDeviceType, atchPortName, atchDeviceName);
						This previous line applied 'atchPortName' for the key name of an
						OPOSControl for the 'Active Device'. When extending the number of
						CashDrawers that are availiable, the previous implementation would write
						over the current key in the registry (Key name must be unique). Instead
						allowing the name of the device to be the key name insterted into the
						registry (Assuming that any device name is already considered unique),
						will allow for multiple OPOSControl devices to be inserted into the registry
						as an 'Active Device' for any device type. ESMITH
					===========================================================================*/
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDeviceName);
					_tcscpy (xxBuffer, SCF_INTERFACE_OPOS);
					dwRet = ScfSetParameter (atchDeviceName, SCF_DATANAME_PRINTPORT, SCF_DATA_STRING, xxBuffer, _tcslen(xxBuffer) * sizeof(TCHAR), &dwCount);

					PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;
					CDialogDeviceList myDevListDialog;
					//Function is used to write changes to device parameters when the user clicks ok in the device config dialog window
					for (; pMyDeviceList->aszDevName; pMyDeviceList++) {
						if (pMyDeviceList->pDialogOptions && pMyDeviceList->aszDevName == atchDeviceType) {
							pMyDeviceList->pDialogOptions->writeChanges ();
							delete pMyDeviceList->pDialogOptions;
							pMyDeviceList->pDialogOptions = 0;
							break;
						}
					}
				}
				else if (atchOPOSCOMMText == atchEthernet) {
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);
					putsettingsEthernet (pclView->GetItemText(nItem, 1), atchPortSettings);
				}
				else if (atchOPOSCOMMText == atchNote) {
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);
				}
				else if (atchOPOSCOMMText == atchDLL) {
					PortSettings.TrimLeft ();
					PortSettings.TrimRight ();
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);
					putsettingsEthernet (pclView->GetItemText(nItem, 1), PortSettings);	//puts EPT settings into registry
					puteptsetting(pclView->GetItemText(nItem, 1));
				}
				else if (atchOPOSCOMMText == atchSpecial) {
					PortSettings.TrimLeft ();
					PortSettings.TrimRight ();
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);

					BOOL  bWroteChanges = FALSE;

					PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;
					CDialogDeviceList myDevListDialog;
					//Function is used to write changes to device parameters when the user clicks ok in the device config dialog window
					for (; pMyDeviceList->aszDevName; pMyDeviceList++) {
						if (pMyDeviceList->pDialogOptions && pMyDeviceList->aszDevName == atchDeviceType) {
							pMyDeviceList->pDialogOptions->writeChanges ();
							delete pMyDeviceList->pDialogOptions;
							pMyDeviceList->pDialogOptions = 0;
							bWroteChanges = TRUE;
							break;
						}
					}

					if (! bWroteChanges) {
						putsettingsEthernet (pclView->GetItemText(nItem, 1), PortSettings);	//puts EPT settings into registry
						puteptsetting(pclView->GetItemText(nItem, 1));
					}
				}
				else if (atchOPOSCOMMText == atchWedge) {
					dwRet = ScfGetParameter (atchDeviceName, SCF_DATANAME_DLLNAME, &dwDataType, atchDllName, sizeof(atchDllName), &dwCount);
					dwRet = ScfSetActiveDevice(atchDeviceType, atchDeviceName, atchDllName);
				}
			}
		}
	}

	dwRet = ScfResetActiveDeviceAll(SCF_TYPENAME_HTTPSERVER);
	dwRet = ScfGetActiveDevice(SCF_TYPENAME_HTTPSERVER, &dwCount, atchDeviceTemp, atchDllName);
	if (m_bHttpServerEnabled) {
		atchDeviceName = _T("Port");
		atchPortSettings = _T("8080");
		dwRet = ScfSetActiveDevice(SCF_TYPENAME_HTTPSERVER, atchDeviceName, atchPortSettings);
	}

	dwRet = ScfResetActiveDeviceAll(SCF_TYPENAME_CONNENGINEMFC);
	atchDeviceName = SCF_DATANAME_ISENABLED;
	if (m_bConnEngineEnabled) {
		_tcscpy (atchDllName, SCF_DLLNAME_DISCONNECTED_YES);
	} else {
		_tcscpy (atchDllName, SCF_DLLNAME_DISCONNECTED_NO);
	}

//	if (m_bConnEngineEnabled) {
//		atchDeviceName = _T("Port");
//		atchPortSettings = _T("8282");
//	}
	dwRet = ScfSetActiveDevice(SCF_TYPENAME_CONNENGINEMFC, atchDeviceName, atchDllName);

	atchDeviceName = SCF_DEVICENAME_JOIN_ISDISCONNECTED;
	if (m_IsDisconnectedSatellite) {
		_tcscpy (atchDllName, SCF_DLLNAME_DISCONNECTED_YES);
	} else {
		_tcscpy (atchDllName, SCF_DLLNAME_DISCONNECTED_NO);
	}
	dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

	atchDeviceName = SCF_DEVICENAME_JOIN_JOINSTATUS;
	if (m_IsJoinedDisconnectedSatellite && m_IsDisconnectedSatellite) {
		_tcscpy (atchDllName, SCF_DLLNAME_JOINSTATUS_JOINED);
	} else {
		_tcscpy (atchDllName, SCF_DLLNAME_JOINSTATUS_UNJOINED);
	}
	dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

	atchDeviceName = SCF_DEVICENAME_JOIN_LASTJOINMASTER;
	_tcscpy (atchDllName, m_LastJoinMaster);
	dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

	atchDeviceName = SCF_DEVICENAME_OVERRIDE_HOSTNAME;
	if (m_IsSatelliteOverride) {
		_tcscpy (atchDllName, m_SatelliteOverride);
	} else {
		atchDllName[0] = 0;
	}
	dwRet = ScfSetActiveDevice(SCF_TYPENAME_OVERRIDENAME_SAT, atchDeviceName, atchDllName);

	CDialog::OnOK();
}

void CDeviceConfigDlg::OnInitialReset()
{
	// TODO: Add your command handler code here
		CSecretNumberDlg secretNumberDlg;
	CDialogInitialReset initialResetDlg;

	if(secretNumberDlg.DoModal() == IDOK)
	{
		if(secretNumberDlg.m_bPasswdStatus)
		{
			initialResetDlg.DoModal();
		}
	}
	else
	{
		//NO SECRET NUMBER ENTERED
	}
}

void CDeviceConfigDlg::OnTestEnvironment()
{
	TestDlg testDlg;

	testDlg.DoModal();
	// TODO: Add your command handler code here

}
void CDeviceConfigDlg::OnNetworkPort()
{
    DWORD   dwDisposition, dwType, dwBytes;
    TCHAR  wszData[16];
    LONG   lRet;
    HKEY hKey;
    TCHAR  wszKey[SCF_USER_BUFFER];

	CNetworkPortDlg testDlg;

	wsprintf(wszKey, TEXT("%s\\%s"), PIFROOTKEY, NETWORKKEY);

	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

	if (lRet == ERROR_SUCCESS) {
		dwBytes = sizeof(wszData);

		lRet = RegQueryValueEx(hKey, PORT, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);

		if (lRet == ERROR_SUCCESS) {
			testDlg.m_csNetworkPortNumber.Format (_T("%s"), wszData);
		}
		else {
			testDlg.m_csNetworkPortNumber = _T("2172");
		}
	}

	if (testDlg.DoModal() == IDOK) {
		// attempt to create the key
		lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, wszKey, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

		if (lRet == ERROR_SUCCESS) {
			_tcscpy (wszData, testDlg.m_csNetworkPortNumber);

			dwBytes = (_tcslen(wszData) + 1) * sizeof(TCHAR);

			lRet = RegSetValueEx(hKey, PORT, 0, REG_SZ, (UCHAR *)&wszData[0], dwBytes);

			if (lRet != ERROR_SUCCESS) {
				TRACE1("CDeviceConfigDlg::OnNetworkPort: Error in RegSetValueEx.  GetLastError () = %d\n", GetLastError());
			}
		}
	}
}


//added import and export icons   SS
//both of the import and export code is inside of the SaveWindow class
//if the import menu was selected then the imoprt option in the SaveWindow is set
//or if the export is selected it knows to ask for a destination to copy to
void CDeviceConfigDlg::OnImportIcons()
{
	CListWindow listwindow(NULL,true);
	listwindow.DoModal();
}
void CDeviceConfigDlg::OnExportIcons()
{
	CListWindow listwindow(NULL,false);
	listwindow.DoModal();
}

void CDeviceConfigDlg::OnNhposStartup()
{
	if (shortCut_NHPOS.isShortCut()) {
		shortCut_NHPOS.DeleteFromStartup();
		CheckDlgButton(IDC_NHPOS_STARTUP,BST_UNCHECKED);
	} else {
		shortCut_NHPOS.CreateInStartup();
		CheckDlgButton(IDC_NHPOS_STARTUP,BST_CHECKED);
	}
}

void CDeviceConfigDlg::OnChangeOptions() 
{
	CListCtrl  *pclView = (CListCtrl *)GetDlgItem (IDC_LIST_DEVICES);
	POSITION pos = pclView->GetFirstSelectedItemPosition();

	if (pos == NULL) {
		TRACE0("No items were selected!\n");
	}
	else
	{
		CString    atchOPOSCOMMText;
		CString	   interfaceCOMMText;

		CString    atchOPOS;
		atchOPOS.LoadString (IDS_CONTROL_TYPE_OPOS);
		CString    atchDirect;
		atchDirect.LoadString (IDS_CONTROL_TYPE_DIRECT);
		CString    atchEthernet;
		atchEthernet.LoadString (IDS_CONTROL_TYPE_ETHERNET);
		CString    atchNote;
		atchNote.LoadString (IDS_CONTROL_TYPE_NOTE);
		CString    atchDLL;
		atchDLL.LoadString (IDS_CONTROL_TYPE_DLL);
		CString    atchSpecial;
		atchSpecial.LoadString (IDS_CONTROL_TYPE_SPECIAL);
		CString    atchWedge;
		atchWedge.LoadString (IDS_CONTROL_TYPE_WEDGE);

		CString    interfaceOPOS;
		interfaceOPOS.LoadString (IDS_CONTROL_INT_OPOS);
		CString    interfaceDirect;
		interfaceDirect.LoadString (IDS_CONTROL_INT_DIRECT);
		CString    interfaceEthernet;
		interfaceEthernet.LoadString (IDS_CONTROL_INT_ETHERNET);
		CString    interfaceNote;
		interfaceNote.LoadString (IDS_CONTROL_INT_NOTE);
		CString    interfaceDLL;
		interfaceDLL.LoadString (IDS_CONTROL_INT_DLL);
		CString    interfaceSpecial;
		interfaceSpecial.LoadString (IDS_CONTROL_INT_SPECIAL);
		CString    interfaceWedge;
		interfaceWedge.LoadString (IDS_CONTROL_INT_WEDGE);

		PDEVICE_LIST_TABLE pMyDeviceList = myDeviceList;

	   while (pos)
	   {
			CDialogDeviceList myDevListDialog;

			int nItem = pclView->GetNextSelectedItem(pos);

			myDevListDialog.csDeviceType = pclView->GetItemText(nItem, 0);
			TRACE1("Itemas %d was selected!\n", nItem);

			// you could do your own processing on nItem here
			// we want this to be exactly what was loaded into the CListCtrl from the LoadString ()
			// above.  Being the same, we can then do comparisons.
			atchOPOSCOMMText = pclView->GetItemText(nItem, 2);
			interfaceCOMMText = pclView->GetItemText(nItem, 5);

			myDevListDialog.csInterface = atchOPOSCOMMText;
			if (atchOPOSCOMMText == atchDirect) {
				myDevListDialog.csInterface = SCF_INTERFACE_DIRECT;
			}
			else if (atchOPOSCOMMText == atchOPOS) {
				myDevListDialog.csInterface = SCF_INTERFACE_OPOS;
				pMyDeviceList = myDeviceList;

				for (; pMyDeviceList->aszDevName; pMyDeviceList++) {
					if (myDevListDialog.csDeviceType.Compare (pMyDeviceList->aszDevName) == 0) {
						myDevListDialog.csDeviceType = pMyDeviceList->aszRegistryKey;

						if (! pMyDeviceList->pDialogOptions) {
							pMyDeviceList->pDialogOptions = new CDialogOptions ();

							pMyDeviceList->pDialogOptions->csDeviceNameOption = pclView->GetItemText(nItem, 1);
							pMyDeviceList->pDialogOptions->csDeviceName = myDevListDialog.csDeviceType;

							TRACE1("Name1 name is %s\n", myDevListDialog.csDeviceType);
							TRACE1("Name2 name is %s\n", pMyDeviceList->pDialogOptions->csDeviceNameOption);
						}

						int ret2 = pMyDeviceList->pDialogOptions->DoModal();
						break;
					}
				}
			}
			else if (atchOPOSCOMMText == atchEthernet) {
				myDevListDialog.csInterface = SCF_INTERFACE_ETHERNET;
			}
			else if (atchOPOSCOMMText == atchNote) {
				myDevListDialog.csInterface = SCF_INTERFACE_NOTE;
			}
			else if (atchOPOSCOMMText == atchDLL) {
				myDevListDialog.csInterface = SCF_INTERFACE_DLL;
			}
			else if (atchOPOSCOMMText == atchWedge) {
				myDevListDialog.csInterface = SCF_INTERFACE_WEDGE;
			}
			else {
				return;
			}
		}
	}
}

void CDeviceConfigDlg::OnHttpServer() 
{
	m_bHttpServerEnabled = (! m_bHttpServerEnabled);

	if (m_bHttpServerEnabled) {
		CheckDlgButton(IDC_NHPOS_HTTPSERVER,BST_CHECKED);
	}
	else {
		CheckDlgButton(IDC_NHPOS_HTTPSERVER,BST_UNCHECKED);
	}
}

void CDeviceConfigDlg::OnNhposUnlock() 
{
	// TODO: Add your control notification handler code here
	// new license key dialog
	CdragdropDlg myDialog;

	if (m_genposurl) {
		myDialog.m_tcsUrl = m_genposurl->csDeviceDLL;
	} else {
		myDialog.m_tcsUrl = _T("https://w3.georgiasouthern.edu/cogs/genpos/api/");
	}
	myDialog.m_tcsSerialNo = m_BiosSerialNumber;
	_tcscpy (myDialog.m_ResellerIdString, _T("12345"));
	myDialog.DoModal();
}

void CDeviceConfigDlg::OnNhposDisconnectedSat() 
{
#if defined(_DEBUG)
	if (IsDlgButtonChecked(IDC_NHPOS_DISCONNECTED_SAT)) {
		m_IsDisconnectedSatellite = 1;
	} else {
		m_IsDisconnectedSatellite = 0;
	}
#else
		if (m_IsDisconnectedSatellite) {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_CHECKED);
		} else {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_UNCHECKED);
		}
#endif
}

void CDeviceConfigDlg::OnNhposDisconnectedSatJoined() 
{
#if defined(_DEBUG)
	if (IsDlgButtonChecked(IDC_NHPOS_DISCONNECTED_SAT_JOINED)) {
		m_IsJoinedDisconnectedSatellite = 1;
	} else {
		m_IsJoinedDisconnectedSatellite = 0;
	}
#else
	if (m_IsJoinedDisconnectedSatellite) {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_UNCHECKED);
	}
#endif
}

void CDeviceConfigDlg::OnButtonDisconnectedSatParams() 
{
	// TODO: Add your control notification handler code here
	int      tempIsJoinedDisconnectedSatellite;
	TCHAR    tempLastJoinMaster[32];
	TCHAR    tempSatelliteOverride[32];

	CDialogDisconnectedSatParams  myDialog;

	myDialog.SetParameters(m_IsSatelliteOverride, m_IsDisconnectedSatellite, m_IsJoinedDisconnectedSatellite, m_LastJoinMaster, m_SatelliteOverride);

	if (myDialog.DoModal () == IDOK) {
		myDialog.GetParameters(m_IsSatelliteOverride, m_IsDisconnectedSatellite, tempIsJoinedDisconnectedSatellite, tempLastJoinMaster, tempSatelliteOverride);

		if (m_IsSatelliteOverride) {
			CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_CHECKED);
			_tcscpy (m_SatelliteOverride, tempSatelliteOverride);
		} else {
			CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_UNCHECKED);
		}

		if (m_IsDisconnectedSatellite) {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_CHECKED);
		} else {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT, BST_UNCHECKED);
		}

		if (m_IsJoinedDisconnectedSatellite) {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_CHECKED);
		} else {
			CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_UNCHECKED);
		}
	}
}

void CDeviceConfigDlg::OnNhposConnEngine() 
{
	CEdit  *pclView = (CEdit *)GetDlgItem (IDC_EDIT_CONNENGINE_PORT);
	TCHAR  tcsPort[32];

	pclView->GetWindowText (tcsPort, 30);
	m_iConnEnginePort = _ttol (tcsPort);

	m_bConnEngineEnabled = (m_bConnEngineEnabled) ? 0 : 1;

	if (m_bConnEngineEnabled) {
		CheckDlgButton(IDC_NHPOS_CONNENGINE, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_CONNENGINE, BST_UNCHECKED);
	}
}

void CDeviceConfigDlg::OnTerminalutilitiesImportactivesettings() 
{
	CString  csFilePath;
	UINT nFlags =  CFile::modeRead;
	CString szFilter = _T("TXT (text file (*.txt)|*.txt||");

	//initialize File Dialog so user can get the import file
	CFileDialog fd(TRUE,_T("txt"),_T("DeviceConfigImport"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);
	int status = fd.DoModal();
	if(status == 2){
		return;
	}
	else{
		csFilePath = fd.GetPathName();
		ScfImportRegistryKeyData (csFilePath);
	}
}

void CDeviceConfigDlg::OnTerminalutilitiesExportactivesettings() 
{
	CString  csFilePath;
	UINT nFlags =  CFile::modeWrite;
	CString szFilter = _T("TXT (text file (*.txt)|*.txt||");

	//initialize File Dialog so user can get the import file
	CFileDialog fd(FALSE,_T("txt"),_T("DeviceConfigExport"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);
	int status = fd.DoModal();
	if(status == 2){
		return;
	}
	else{
		csFilePath = fd.GetPathName();
		ScfExportRegistryKeyData (ACTIVE_DEVICE_KEY, csFilePath);
	}
}


//-------------------------------------

class CDialogUserScript : public CDialog
{
public:
// Dialog Data
	//{{AFX_DATA(CDialogUserScript)
	enum { IDD = IDD_DIALOG_USERSCRIPT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CDialogUserScript (CWnd *pParent = NULL);
	DWORD SetUserRegistryItems ();

	int      m_ButtonId;
	CString  m_csUserGroup;
	CString  m_csUsbStartString;
	int      m_UsbStartEnabled;
	DWORD    m_dwUsbStartType;
	int      m_GenerateUsbCmd;
	int      m_AutoRunEnabled;
	DWORD    m_dwAutoRunType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogUserScript)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedAbort();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CEdit csGroupName;
};

CDialogUserScript::CDialogUserScript (CWnd *pParent /*=NULL*/)
	: CDialog(CDialogUserScript::IDD, pParent), m_ButtonId(0),
	m_dwUsbStartType(0), m_GenerateUsbCmd(0),
	m_AutoRunEnabled(0)
{
	m_csUserGroup = _T("Users");
	//{{AFX_DATA_INIT(CDialogUserScript)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BOOL CDialogUserScript::OnInitDialog()
{
	LONG    lRet = 0;
	CRegKey crKey;
	CString csUsbStor (L"SYSTEM\\CurrentControlSet\\Services\\USBSTOR\\");

	// See the Microsoft Knowledge Base article 103000, CurrentControlSet\Services Subkey Entries
	// http://support.microsoft.com/kb/103000
	m_csUsbStartString = L"USB Mass Storage Start (Unknown)";
	m_UsbStartEnabled = 0;
	if ( (lRet = crKey.Open (HKEY_LOCAL_MACHINE, csUsbStor, KEY_READ)) == ERROR_SUCCESS) {
		crKey.QueryDWORDValue (L"Start", m_dwUsbStartType);
		crKey.Close();
		switch (m_dwUsbStartType) {
			case 3:
				m_csUsbStartString = L"USB Mass Storage Start (Enabled)";
				m_UsbStartEnabled = 1;
				break;
			case 4:
				m_csUsbStartString = L"USB Mass Storage Start (Disabled)";
				m_UsbStartEnabled = 0;
				break;
			default:
				break;
		}
	}
#if 0
	CString csAutoRun (L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\");
	if ( (lRet = crKey.Open (HKEY_LOCAL_MACHINE, csAutoRun, KEY_READ)) == ERROR_SUCCESS) {
		lRet = crKey.QueryDWORDValue (L"NoDriveTypeAutoRun", m_dwAutoRunType);    // valid for Windows 7, etc.
		lRet = crKey.QueryDWORDValue (L"HonorAutoRunSetting", m_dwAutoRunType);   // valid for Windows XP
		crKey.Close();
		m_AutoRunEnabled = ((m_dwAutoRunType & 0x04) == 0);  // check removable media disabled bit
	}
#endif
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

DWORD CDialogUserScript::SetUserRegistryItems ()
{
	LONG    lRet = 0;
	DWORD   dwRetValue = 0;
	DWORD   dwRetUsbStart = 0;
	DWORD   dwRetAutoRun = 0;
	CRegKey crKey;
	CString csUsbStor (L"SYSTEM\\CurrentControlSet\\Services\\USBSTOR\\");

	if ( (lRet = crKey.Open (HKEY_LOCAL_MACHINE, csUsbStor, KEY_READ | KEY_WRITE)) == ERROR_SUCCESS) {
		lRet = crKey.QueryDWORDValue (L"Start", m_dwUsbStartType);
		dwRetUsbStart = m_dwUsbStartType;
		switch (m_dwUsbStartType) {
			case 3:
				if (!m_UsbStartEnabled) {
					m_dwUsbStartType = 0x04;
				}
				break;
			case 4:
				if (m_UsbStartEnabled) {
					m_dwUsbStartType = 0x03;
				}
				break;
			default:
				break;
		}
		if (dwRetUsbStart != m_dwUsbStartType) {
			lRet = crKey.SetDWORDValue (L"Start", m_dwUsbStartType);
		}

		crKey.Close();
	}
#if 0
	CString csAutoRun (L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\");
	if ( (lRet = crKey.Open (HKEY_LOCAL_MACHINE, csAutoRun, KEY_READ | KEY_WRITE)) == ERROR_SUCCESS) {
		lRet = crKey.QueryDWORDValue (L"HonorAutoRunSetting", m_dwAutoRunType);    // valid for Windows XP
		dwRetAutoRun = m_dwAutoRunType;
		lRet = crKey.QueryDWORDValue (L"NoDriveTypeAutoRun", m_dwAutoRunType);    // valid for Windows 7, etc.
		dwRetAutoRun = m_dwAutoRunType;
		// The following table shows the settings for the NoDriveTypeAutoRun registry entry.
		// These values can be bitwise ORed together to combine settings.
		//  Value         Meaning
		// 0x01 or 0x80  Disables AutoRun on drives of unknown type
		// 0x04          Disables AutoRun on removable drives
		// 0x08          Disables AutoRun on fixed drives
		// 0x10          Disables AutoRun on network drives
		// 0x20          Disables AutoRun on CD-ROM drives
		// 0x40          Disables AutoRun on RAM disks
		// 0xFF          Disables AutoRun on all kinds of drives
		// The value of the NoDriveTypeAutoRun registry entry determines which drive
		// or drives the Autorun functionality will be disabled for. For example, if
		// you want to disable Autorun for network drives only, you must set the
		// value of NoDriveTypeAutoRun registry entry to 0x10. 
		if (!m_UsbStartEnabled) {
			m_dwAutoRunType |= 0x04;  // disable auto run on removable drives.
		} else {
			m_dwAutoRunType &= ~(0x04);  // remove disable auto run on removable drives
		}
		if (dwRetAutoRun != m_dwAutoRunType) {
			lRet = crKey.SetDWORDValue (L"HonorAutoRunSetting", m_dwAutoRunType);
		}

		crKey.Close();
	}
#endif
	return dwRetValue;
}

void CDialogUserScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogUserScript)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		((CEdit*)GetDlgItem (IDC_EDIT1))->GetWindowText(m_csUserGroup);
		m_UsbStartEnabled = ((CButton *)GetDlgItem (IDC_CHECK_USBSTOR))->GetCheck ();
		m_GenerateUsbCmd = ((CButton *)GetDlgItem (IDC_CHECK_ADD_USB_PERM))->GetCheck ();
	} else {
		((CEdit *)GetDlgItem (IDC_EDIT1))->SetWindowText(m_csUserGroup);
		((CButton *)GetDlgItem (IDC_CHECK_USBSTOR))->SetWindowText(m_csUsbStartString);
		((CButton *)GetDlgItem (IDC_CHECK_USBSTOR))->SetCheck (m_UsbStartEnabled);
		((CButton *)GetDlgItem (IDC_CHECK_ADD_USB_PERM))->SetCheck (m_GenerateUsbCmd);
	}
}

BEGIN_MESSAGE_MAP(CDialogUserScript, CDialog)
ON_BN_CLICKED(IDOK, &CDialogUserScript::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CDialogUserScript::OnBnClickedCancel)
ON_BN_CLICKED(IDABORT, &CDialogUserScript::OnBnClickedAbort)
END_MESSAGE_MAP()



void CDialogUserScript::OnBnClickedOk()
{
	m_ButtonId = IDOK;
	OnOK();
}

void CDialogUserScript::OnBnClickedCancel()
{
	m_ButtonId = IDCANCEL;
	OnOK();
}

void CDialogUserScript::OnBnClickedAbort()
{
	m_ButtonId = IDABORT;
	OnCancel();
}

int GenerateFileSystemPermissions (char *aszFileNameCmd, char *aszUserGroupName, int bAddUsbDeny)
{
	CHAR  *aszFileNameSql = "GenPOS_SetupUserAccess.sql";
	int   iCount = 0;
	CHAR  *aszUserGroupDefault = "Users";

	if (aszUserGroupName == 0 || aszUserGroupName[0] == 0) {
		aszUserGroupName = aszUserGroupDefault;
	}

	{
#if 1
		char *pLines[] = {
			"@echo Creating GenPOS SQL user group %s\n",
			"sqlcmd -E -i GenPOS_SetupUserAccess.sql\n",
			"@echo Setting file permissions for user group %s\n",
			"cacls C:\\Flashdisk\\NCR\\Saratoga\\Database /T /E /G \"%s\":F\n",
			"mkdir C:\\Flashdisk\\NCR\\Saratoga\\PrintFiles\n",
			"cacls C:\\Flashdisk\\NCR\\Saratoga\\PrintFiles /T /E /G \"%s\":F\n",
			"mkdir C:\\Flashdisk\\NCR\\Saratoga\\Icons\n",
			"cacls C:\\Flashdisk\\NCR\\Saratoga\\Icons /T /E /G \"%s\":F\n",
			"mkdir C:\\Flashdisk\\NCR\\Saratoga\\Log\n",
			"cacls C:\\Flashdisk\\NCR\\Saratoga\\Log /T /E /G \"%s\":F\n",
			"mkdir C:\\tempdisk\n",
			"cacls C:\\tempdisk /T /E /G \"%s\":F\n"
		};
		char *pLinesUsb[] = {
			"@echo Removing USB device access for user group %s\n",
			"cacls %%SystemRoot%%\\Inf\\Usbstor.pnf /E /R \"%s\":F\n",
			"cacls %%SystemRoot%%\\Inf\\Usbstor.pnf /E /D \"%s\":F\n",
			"cacls %%SystemRoot%%\\Inf\\Usbstor.inf /E /R \"%s\":F\n",
			"cacls %%SystemRoot%%\\Inf\\Usbstor.inf /E /D \"%s\":F\n",
			"cacls %%SystemRoot%%\\Inf\\Usb.inf /E /R \"%s\":F\n",
			"cacls %%SystemRoot%%\\Inf\\Usb.inf /E /D \"%s\":F\n"
		};
		char *pLinesFinal[] = {
			"pause\n"
		};
#else
		char *pLines[] = {
			"@echo testing and Changing directory to 'c:\\'",
			"pause"
		};
#endif

		FILE *hFile = fopen (aszFileNameCmd, "w");

		if (hFile) {
			int i;
			for (i = 0; i < (sizeof(pLines)/sizeof(pLines[0])); i++) {
				// print the line adding the user group as many times as needed
				fprintf (hFile, pLines[i], aszUserGroupName, aszUserGroupName, aszUserGroupName);
			}
			if (bAddUsbDeny) {
				for (i = 0; i < (sizeof(pLinesUsb)/sizeof(pLinesUsb[0])); i++) {
					// print the line adding the user group as many times as needed
					fprintf (hFile, pLinesUsb[i], aszUserGroupName, aszUserGroupName, aszUserGroupName);
				}
			}
			for (i = 0; i < (sizeof(pLinesFinal)/sizeof(pLinesFinal[0])); i++) {
				// print the line adding the user group as many times as needed
				fprintf (hFile, pLinesFinal[i], aszUserGroupName, aszUserGroupName, aszUserGroupName);
			}
			fclose (hFile);
			iCount++;
		}
	}

	{
		char *pLines[] = {
			"exec sp_grantlogin N'NT AUTHORITY\\SYSTEM'\n",
			"EXEC dbo.sp_grantdbaccess @loginame = N'NT AUTHORITY\\SYSTEM', @name_in_db = N'NT AUTHORITY\\SYSTEM';\n",
			"exec sp_addrolemember @rolename = N'db_owner', @membername = N'NT AUTHORITY\\SYSTEM';\n",
			"\n",
			"if not exists (select * from dbo.sysusers where name = N'%s')\n",
			"    EXEC sp_grantdbaccess N'BUILTIN\\%s', N'%s'\n",
			"    exec sp_addrolemember @rolename = N'db_owner', @membername = N'%s';\n",
			"GO\n",
			"\n"
		};
		char *pDbs[] = {"PluTtlDC", "PluTtlDS", "PluTtlPC", "PluTtlPS", "PluTtlTS"};

		FILE *hFile = fopen (aszFileNameSql, "w");

		if (hFile) {
			int i, j;
			for (j = 0; j < (sizeof(pDbs)/sizeof(pDbs[0])); j++) {
				fprintf (hFile, "USE [%s]\nGO\n", pDbs[j]);
				for (i = 0; i < (sizeof(pLines)/sizeof(pLines[0])); i++) {
					// print the line adding the user group as many times as needed
					fprintf (hFile, pLines[i], aszUserGroupName, aszUserGroupName, aszUserGroupName);
				}
			}
			fclose (hFile);
			iCount++;
		}
	}

	return iCount;
}

void CDeviceConfigDlg::OnGenerateExecuteUserSetup() 
{
	CDialogUserScript  dialog(this);

	int iSet = dialog.DoModal ();
	if (iSet == IDOK) {
		dialog.SetUserRegistryItems ();
		iSet = dialog.m_ButtonId;
		CHAR  *aszFileNameCmd = "GenPOS_SetupUserAccess.cmd";
		CHAR   aszGroupName [64];
		aszGroupName[63] = 0;
		for (int i = 0; i < 63; i++) {
			aszGroupName[i] = 0;
			if (i > dialog.m_csUserGroup.GetLength()) break;
			aszGroupName[i] = dialog.m_csUserGroup[i];
		}
		int nCount = GenerateFileSystemPermissions (aszFileNameCmd, aszGroupName, dialog.m_GenerateUsbCmd);
		if (nCount == 2 && iSet == IDOK) {
			// all files needed have been generated.  Execute.
			CString csName = aszFileNameCmd;
			HINSTANCE   hStatus = ShellExecute (NULL, _T("open"), csName, NULL, NULL, SW_SHOWNORMAL);
			if ((ULONG)hStatus == ERROR_FILE_NOT_FOUND) {
				AfxMessageBox (_T("ERROR: User setup script ShellExecute failed."));
			}
		} else if (nCount != 2) {
			AfxMessageBox (_T("ERROR: User setup script generation failed."));
		} else {
			AfxMessageBox (_T("NOTE: User setup scripts generated, not executed."));
		}
	}
}


void CDeviceConfigDlg::OnBnClickedRadioDisconnectedOverride()
{
	//  See also usage of the following defines for Windows Registry keys used in DeviceConfig:
	//     SCF_TYPENAME_OVERRIDENAME_SAT
	//     SCF_DEVICENAME_OVERRIDE_HOSTNAME
	//  See use of function PifOverrideHostName() in GenPOS startup.
	if (IsDlgButtonChecked(IDC_RADIO_DISCONNECTED_OVERRIDE)) {
		m_IsSatelliteOverride = 1;
	} else {
		m_IsSatelliteOverride = 0;
	}
}
