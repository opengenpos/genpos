// LanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcsample.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Terminal.h"

#include "SerialPortDlg.h"
#include "LanDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLanDlg dialog
//extern CPCSampleApp theApp;

CLanDlg::CLanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanDlg::IDD, pParent)
	, m_strDbFileName(_T(""))
	, m_SaveResetDataToDatabase(FALSE)
{
	//{{AFX_DATA_INIT(CLanDlg)
	m_bIPAddress1 = 0;
	m_bIPAddress2 = 0;
	m_bIPAddress3 = 0;
	m_strPassword = _T("");
	m_bTerminalNum = 0;
	m_bTerminalNo = 0;
	m_strHostName = _T("");    //***PDINU

	//}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CLogonDlg::CLogonDlg()
//
//  PURPOSE :   Constructor of Log on dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CLanDlg::CLanDlg(
    const BYTE bTerminalNo,
    const CSerialInfo& SerialInfo,
    CWnd* pParent
    ) : CDialog( CLanDlg::IDD, pParent )
{
    // --- initialize member variables with user specified parameter ---

	m_bTerminalNum = bTerminalNo; 
	m_bTerminalNo = bTerminalNo;
    m_infoSerial = SerialInfo;


	//IP address is stored in the computers registry when entered
	//is loaded into the dialog box whenever it is called
	m_bIPAddress1 = (UCHAR) theApp.GetProfileInt(_T(""),_T("IP1"), 127);
	m_bIPAddress2 = (UCHAR) theApp.GetProfileInt(_T(""),_T("IP2"), 0);
	m_bIPAddress3 = (UCHAR) theApp.GetProfileInt(_T(""),_T("IP3"), 0);
	m_bTerminalNo = (UCHAR) theApp.GetProfileInt(_T(""),_T("IP4"), 1);
	m_strHostName = theApp.GetProfileString(_T(""),_T("Host Name"), _T("localhost"));	//PDINU
	m_strDbFileName = theApp.GetProfileString(_T(""),_T("Database Name"), _T(""));
	m_SaveResetDataToDatabase = (UCHAR) theApp.GetProfileInt(_T(""),_T("SaveResetToDatabase"), 0);
}


void CLanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanDlg)
	DDX_Text(pDX, IDC_EDIT_IP1, m_bIPAddress1);
	DDV_MinMaxByte(pDX, m_bIPAddress1, 0, 254);
	DDX_Text(pDX, IDC_EDIT_IP2, m_bIPAddress2);
	DDV_MinMaxByte(pDX, m_bIPAddress2, 0, 254);
	DDX_Text(pDX, IDC_EDIT_IP3, m_bIPAddress3);
	DDV_MinMaxByte(pDX, m_bIPAddress3, 0, 254);
	DDX_Text(pDX, IDC_EDIT_PASSWORD_LAN, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 10);
	DDX_Text(pDX, IDC_EDIT_HOST_LAN, m_strHostName);	//Pdinu
	DDV_MaxChars(pDX, m_strHostName, 20);	//PDINU
	DDX_Text(pDX, IDC_EDIT_IP4, m_bTerminalNo);
	DDX_Text(pDX, IDC_EDITTERMNO_LAN, m_bTerminalNum);
	DDX_Control(pDX, IDC_HOST_CONNECT, m_HostConnect);
	DDX_Control(pDX, IDC_IP_CONNECT, m_IpConnect);
	DDX_Text(pDX, IDC_EDIT_DB_NAME, m_strDbFileName);
	DDV_MaxChars(pDX, m_strDbFileName, 48);
	DDX_Check(pDX, IDC_CHECK_USE_DATABASE, m_SaveResetDataToDatabase);
	//}}AFX_DATA_MAP


	//writes the input numbers to the registry to be stored for the ip connecton
	//saved in the registry
	theApp.WriteProfileInt(_T(""),_T("IP1"), m_bIPAddress1);
	theApp.WriteProfileInt(_T(""),_T("IP2"), m_bIPAddress2);
	theApp.WriteProfileInt(_T(""),_T("IP3"), m_bIPAddress3);
	theApp.WriteProfileInt(_T(""),_T("IP4"), m_bTerminalNo);
	theApp.WriteProfileString(_T(""),_T("Host Name"), m_strHostName);	//PDINU
	theApp.WriteProfileString(_T(""),_T("Database Name"), m_strDbFileName);
	theApp.WriteProfileInt(_T(""),_T("SaveResetToDatabase"), m_SaveResetDataToDatabase);

	if(pDX->m_bSaveAndValidate)
	{
		if (m_HostConnect.GetCheck())
		{
			m_HostConnectType = HostConnectHost;
		}
		if (m_IpConnect.GetCheck())
		{
			m_HostConnectType = HostConnectIP;
		}
	}
	else{
		/*  --- Initialize the IP radio button --- */
		m_IpConnect.SetCheck(TRUE);
		m_HostConnect.SetCheck(FALSE);
		GetDlgItem(IDC_EDIT_HOST_LAN)->EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CLanDlg, CDialog)
	//{{AFX_MSG_MAP(CLanDlg)
	ON_BN_CLICKED(IDC_IP_CONNECT, OnIpConnect)
	ON_BN_CLICKED(IDC_HOST_CONNECT, OnHostConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLanDlg message handlers

void CLanDlg::OnIpConnect() 
{

	GetDlgItem(IDC_EDIT_HOST_LAN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IP1)->EnableWindow(TRUE); 
	GetDlgItem(IDC_EDIT_IP2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_IP3)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_IP4)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_IP4)->EnableWindow(TRUE);
}

void CLanDlg::OnHostConnect() 
{
	GetDlgItem(IDC_EDIT_HOST_LAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_IP1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IP2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IP3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IP4)->EnableWindow(FALSE);
}


CSelectActivityList::CSelectActivityList(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectActivityList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLanDlg)
	//}}AFX_DATA_INIT
}
void CSelectActivityList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectActivityList)
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate) {
		POSITION pos = m_list->GetHeadPosition();
		CListBox *pListBox = (CListBox *)GetDlgItem (IDC_LIST1);

		while (pos != NULL) 
		{
			CString &xstrng = m_list->GetNext (pos);
			pListBox->AddString (xstrng);
		}
	} else {
		CListBox *pListBox = (CListBox *)GetDlgItem (IDC_LIST1);
		int iNdx = pListBox->GetCurSel();
		if (iNdx != LB_ERR) {
			pListBox->GetText (iNdx, m_csSelection);
		}
	}
}

BEGIN_MESSAGE_MAP(CSelectActivityList, CDialog)
	//{{AFX_MSG_MAP(CSelectActivityList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

