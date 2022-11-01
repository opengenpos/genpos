// RPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "PCSample.h"
#include "RPropertySheet.h"


// RPropertySheet

IMPLEMENT_DYNAMIC(RPropertySheet, CPropertySheet)

RPropertySheet::RPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

RPropertySheet::RPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

RPropertySheet::~RPropertySheet()
{
}


BEGIN_MESSAGE_MAP(RPropertySheet, CPropertySheet)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// RPropertySheet message handlers


int RPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


void RPropertySheet::OnMove(int x, int y)
{
	CPropertySheet::OnMove(x, y);

	// TODO: Add your message handler code here
}

void RPropertySheet::OnClose()
{

}

void RPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertySheet::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}


void RPropertySheet::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: Add your message handler code here
}


BOOL RPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam==1 || wParam==2 || wParam==9) 
	{
		return TRUE;
	}
	return CPropertySheet::OnCommand(wParam, lParam);
}


BOOL RPropertySheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Get rid of those buttons we do not want...
	RECT  sRect;
   
	// Center the window
	int sx = ::GetSystemMetrics(SM_CXSCREEN);
	int sy = ::GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(&sRect);

	SetWindowPos(&wndNoTopMost, 
		(sx-(sRect.right-sRect.left)) / 2, 
		(sy-(sRect.bottom-sRect.top)) / 2, 
		sRect.right  - sRect.left, 
		sRect.bottom - sRect.top, // - 10,
		SWP_SHOWWINDOW
		);

	CMenu* pMenu = GetSystemMenu (FALSE);
	if (pMenu)
       pMenu->DeleteMenu(SC_CLOSE, MF_BYCOMMAND);

	return bResult;
}
