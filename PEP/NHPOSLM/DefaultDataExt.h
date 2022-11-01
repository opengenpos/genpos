// DefaultDataExt.h: interface for the DefaultDataExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTDATAEXT_H__05A4F5A8_7874_4B9B_84D4_E8152FD50F1F__INCLUDED_)
#define AFX_DEFAULTDATAEXT_H__05A4F5A8_7874_4B9B_84D4_E8152FD50F1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DefaultData.h"
#include "DefaultEdit.h"
class DefaultDataExt  
{
public:
	DefaultDataExt();
	virtual ~DefaultDataExt();

	COLORREF ButtonDefault;
	COLORREF WindowDefault;
	COLORREF TextWinDefault;
	
	LOGFONT FontDefault;
	COLORREF FontColorDefault;

	void PopupDefaultDlg();

};

#endif // !defined(AFX_DEFAULTDATAEXT_H__05A4F5A8_7874_4B9B_84D4_E8152FD50F1F__INCLUDED_)
// CDButtonEdit message handlers

