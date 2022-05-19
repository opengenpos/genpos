// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Winsock.h>

#include "DeviceConfig.h"
#include "TestDlg.h"
#include "OPOSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TestDlg dialog


TestDlg::TestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestDlg, CDialog)
	//{{AFX_MSG_MAP(TestDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PING_TEST, &TestDlg::OnBnClickedPingTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestDlg message handlers

BOOL TestDlg::OnInitDialog() 
{
	OPOSDlg	            oposDlg;

	if( !oposDlg.Create(IDD_DIALOG_OPOS_TEST))
		//OPOS is missing something
	{
		//not AVAILABLE
		this->SetDlgItemText(IDC_OPOS_PRINTER_TF, _T("NOT AVAILABLE"));
	}
	else
	{	
		//AVAILABLE
		this->SetDlgItemText(IDC_OPOS_PRINTER_VERSION, oposDlg.m_OPOSPrinter.GetControlObjectDescription());
		this->SetDlgItemText(IDC_OPOS_PRINTER_TF, _T("AVAILABLE"));
		oposDlg.DestroyWindow();
	} 

	// do discovery for Master Terminal and display the status
	//Terminal Information Variables Set
		// In response to SR 495 the following information was added to the Environment
		//	Setup Menu. Added was the Terminal Host Name, Error Checking and Handling of
		//	the Terminal Number, and also the IP Address will be displayed in the dialog
		//	window. RZACHARY
		CHAR		pszHostName[32] = "";
		CHAR		*pszIPAddress;
		TCHAR		szHostData[32] = {0};
		TCHAR		szTermNum[3] = {0, 0, 0};
		TCHAR		szIpAddr[32] = _T("192.0.0.1");

		gethostname(pszHostName, sizeof(pszHostName));
		pszHostName[31] = '\0';

		this->SetDlgItemText(IDC_IND_MASTER_IP, _T("N"));
		this->SetDlgItemText(IDC_STATIC_MASTER_IP, _T("Master IP Address"));
		for( ULONG i1 = 0; i1 < sizeof(pszHostName); i1++){
			szHostData[i1] = pszHostName[i1];
			if(pszHostName[i1] == '-'){
				struct hostent *ipaddr;
				pszHostName[i1+1] = '1';
				pszHostName[i1+2] = 0;
				// WARNING: gethostbyname() may fail if a terminal has been turned off for several minutes
				//          or if after being off for some time, the terminal has just been turned on.
				//          there is a chance the host name information in the directory may have gone
				//          stale due to lack of communication with a terminal and been discarded.
				ipaddr = gethostbyname(pszHostName);
				m_csHostName = pszHostName;
				this->SetDlgItemText(IDC_STATIC_MASTER_IP, m_csHostName);
				if (ipaddr) {
					for(USHORT i = 0; ipaddr->h_addr_list[i] != 0; i++){
						struct in_addr addr;
						memcpy(&addr, ipaddr->h_addr_list[i], sizeof(struct in_addr));
						pszIPAddress = inet_ntoa(addr);
					}
					for(USHORT i = 0; i < 32; i++){
						szIpAddr[i] = pszIPAddress[i];
					}
					this->SetDlgItemText(IDC_STATIC_MASTER_IP, szIpAddr );
					this->SetDlgItemText(IDC_IND_MASTER_IP, _T("Y"));
				}
				break;
			}
		}

	CDialog::OnInitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TestDlg::OnBnClickedPingTest()
{
	// create a ping command line to use with the system() function
	// to run ping in the cmd.exe console.  command line looks like:
	//    ipconfig /all & ping -n 6 "hostname" & pause
	// the & chains two commands on same command line without checking
	// whether the first command worked or not.
	// we have added the ipconfig command to display the basic network information.
	int  i;
	char myCmdLine[128] = {0};
	CString csPingCommand ("ipconfig /all & ping -n 6 \"");
	csPingCommand += m_csHostName;
	csPingCommand += _T("\" & pause");  // pause command to keep the window open
	for (i = 0; i < csPingCommand.GetLength (); i ++) {
		myCmdLine[i] = csPingCommand[i];
	}
	system (myCmdLine);
}
