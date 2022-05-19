// OPOSTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "OPOSTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OPOSTestDlg dialog


OPOSTestDlg::OPOSTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OPOSTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(OPOSTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void OPOSTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OPOSTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OPOSTestDlg, CDialog)
	//{{AFX_MSG_MAP(OPOSTestDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OPOSTestDlg message handlers
