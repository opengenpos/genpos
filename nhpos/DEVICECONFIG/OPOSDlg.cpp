// OPOSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "OPOSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OPOSDlg dialog


OPOSDlg::OPOSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OPOSDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(OPOSDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void OPOSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OPOSDlg)
	DDX_Control(pDX, IDC_POSPRINTER1, m_OPOSPrinter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OPOSDlg, CDialog)
	//{{AFX_MSG_MAP(OPOSDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OPOSDlg message handlers
