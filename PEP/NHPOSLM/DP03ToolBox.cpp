// DP03ToolBox.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DP03ToolBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDP03ToolBox dialog


CDP03ToolBox::CDP03ToolBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDP03ToolBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDP03ToolBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDP03ToolBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDP03ToolBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDP03ToolBox, CDialog)
	//{{AFX_MSG_MAP(CDP03ToolBox)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDP03ToolBox message handlers

BOOL CDP03ToolBox::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (LOWORD(wParam) >= IDD_P03_NOFUNC && LOWORD(wParam) <= IDD_P03_LAST) {
		EndDialog (LOWORD(wParam) | 0x0ff00000);
		return TRUE;
	}
	else
		return CDialog::OnCommand(wParam, lParam);
}
