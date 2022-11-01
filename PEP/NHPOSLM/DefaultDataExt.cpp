// DefaultDataExt.cpp: implementation of the DefaultDataExt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewLayout.h"
#include "DefaultDataExt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DefaultDataExt::DefaultDataExt()
{

}

DefaultDataExt::~DefaultDataExt()
{

}

void DefaultDataExt::PopupDefaultDlg()
{
	DefaultEdit DefDlg;
	
	DefDlg.ButtonDefault = ButtonDefault;
	DefDlg.WindowDefault = WindowDefault;
	DefDlg.TextWinDefault = TextWinDefault;
	
	DefDlg.FontDefault = FontDefault;
	DefDlg.FontColorDefault = FontColorDefault;

	if(DefDlg.DoModal() == IDOK){

		ButtonDefault = DefDlg.ButtonDefault;
		WindowDefault = DefDlg.WindowDefault;
		TextWinDefault = DefDlg.TextWinDefault;
		
		FontDefault = DefDlg.FontDefault;
		FontColorDefault = DefDlg.FontColorDefault;
	}

}