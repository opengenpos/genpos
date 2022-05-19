// DialogDisconnectedSatParams.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "DialogDisconnectedSatParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogDisconnectedSatParams dialog
void CDialogDisconnectedSatParams::SetStaticTermLabelText (void)
{
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_TERM_LABEL);
	if (m_IsSatelliteOverride) {
		pStatic->SetWindowText (_T("Override Name for this Terminal"));
	} else {
		pStatic->SetWindowText (_T("Name of Last Joined Master Terminal"));
	}

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DISCONNECTED_SAT_MASTER);
	if (m_IsSatelliteOverride) {
		pEdit->SetWindowText (m_SatelliteOverride);
	} else {
		pEdit->SetWindowText (m_LastJoinMaster);
	}
}

CDialogDisconnectedSatParams::CDialogDisconnectedSatParams(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDisconnectedSatParams::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDisconnectedSatParams)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_IsSatelliteOverride = 0;
	m_IsDisconnectedSatellite = 0;
	m_IsJoinedDisconnectedSatellite = 0;
	m_LastJoinMaster = _T("");
	m_SatelliteOverride = _T("");
}

void CDialogDisconnectedSatParams::SetParameters (int IsSatelliteOverride, int IsDisconnected, int IsJoined, TCHAR *LastMaster, TCHAR *SatOverride)
{
	m_IsSatelliteOverride = IsSatelliteOverride;
	m_IsDisconnectedSatellite = IsDisconnected;
	m_IsJoinedDisconnectedSatellite = IsJoined;
	m_LastJoinMaster = LastMaster;
	m_SatelliteOverride = SatOverride;
}

void CDialogDisconnectedSatParams::GetParameters (int &IsSatelliteOverride, int &IsDisconnected, int &IsJoined, TCHAR *LastMaster, TCHAR *SatOverride)
{
	IsSatelliteOverride = m_IsSatelliteOverride;
	IsDisconnected = m_IsDisconnectedSatellite;
	IsJoined = m_IsJoinedDisconnectedSatellite;
	_tcscpy (LastMaster, m_LastJoinMaster);
	_tcscpy (SatOverride, m_SatelliteOverride);
}

void CDialogDisconnectedSatParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDisconnectedSatParams)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	if (m_IsSatelliteOverride) {
		DDX_Text(pDX, IDC_EDIT_DISCONNECTED_SAT_MASTER, m_SatelliteOverride);
	} else {
		DDX_Text(pDX, IDC_EDIT_DISCONNECTED_SAT_MASTER, m_LastJoinMaster);
	}
	DDV_MaxChars(pDX, m_LastJoinMaster, 30);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDisconnectedSatParams, CDialog)
	//{{AFX_MSG_MAP(CDialogDisconnectedSatParams)
	ON_BN_CLICKED(IDC_RADIO_STANDARD_SATELLITE, OnNhposStandardSatellite)
	ON_BN_CLICKED(IDC_RADIO_DISCONNECTED_SATELLITE, OnNhposDisconnectedSatellite)
	ON_BN_CLICKED(IDC_NHPOS_DISCONNECTED_SAT_JOINED, OnNhposDisconnectedSatJoined)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_DISCONNECTED_OVERRIDE, &CDialogDisconnectedSatParams::OnBnClickedRadioDisconnectedSatellite2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDisconnectedSatParams message handlers

void CDialogDisconnectedSatParams::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDialogDisconnectedSatParams::OnNhposStandardSatellite() 
{
	if (IsDlgButtonChecked(IDC_RADIO_STANDARD_SATELLITE)) {
		m_IsSatelliteOverride = m_IsDisconnectedSatellite = 0;
	} else {
		m_IsDisconnectedSatellite = 1;
	}
	SetStaticTermLabelText();
}

void CDialogDisconnectedSatParams::OnNhposDisconnectedSatellite() 
{
	if (IsDlgButtonChecked(IDC_RADIO_DISCONNECTED_SATELLITE)) {
		m_IsDisconnectedSatellite = 1;
	} else {
		m_IsDisconnectedSatellite = 0;
	}
	SetStaticTermLabelText();
}

void CDialogDisconnectedSatParams::OnNhposDisconnectedSatJoined() 
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

BOOL CDialogDisconnectedSatParams::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_IsDisconnectedSatellite) {
		CheckDlgButton(IDC_RADIO_DISCONNECTED_SATELLITE, BST_CHECKED);
		CheckDlgButton(IDC_RADIO_STANDARD_SATELLITE, BST_UNCHECKED);
		CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_UNCHECKED);
	} else if (m_IsSatelliteOverride) {
		CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_CHECKED);
		CheckDlgButton(IDC_RADIO_DISCONNECTED_SATELLITE, BST_UNCHECKED);
		CheckDlgButton(IDC_RADIO_STANDARD_SATELLITE, BST_UNCHECKED);
	} else {
		CheckDlgButton(IDC_RADIO_STANDARD_SATELLITE, BST_CHECKED);
		CheckDlgButton(IDC_RADIO_DISCONNECTED_SATELLITE, BST_UNCHECKED);
		CheckDlgButton(IDC_RADIO_DISCONNECTED_OVERRIDE, BST_UNCHECKED);
	}

	if (m_IsJoinedDisconnectedSatellite) {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_CHECKED);
	} else {
		CheckDlgButton(IDC_NHPOS_DISCONNECTED_SAT_JOINED, BST_UNCHECKED);
	}

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DISCONNECTED_SAT_MASTER);
	pEdit->LimitText(30);
	
	SetStaticTermLabelText();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDisconnectedSatParams::OnBnClickedRadioDisconnectedSatellite2()
{
	if (IsDlgButtonChecked(IDC_RADIO_DISCONNECTED_OVERRIDE)) {
		m_IsSatelliteOverride = 1;
	} else {
		m_IsSatelliteOverride = 0;
	}
	SetStaticTermLabelText();
}
