// NetworkPortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "NetworkPortDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkPortDlg dialog


CNetworkPortDlg::CNetworkPortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkPortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetworkPortDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNetworkPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkPortDlg)
	DDX_Text(pDX, IDC_EDIT_NETWORK_PORT, m_csNetworkPortNumber);
	DDX_Control(pDX, IDC_EDIT_NETWORK_PORT, m_ebNetworkPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkPortDlg, CDialog)
	//{{AFX_MSG_MAP(CNetworkPortDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkPortDlg message handlers
