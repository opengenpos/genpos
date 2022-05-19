// FrameworkWndEditBox.h: interface for the CFrameworkWndEditBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEWORKWNDEDITBOX_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_)
#define AFX_FRAMEWORKWNDEDITBOX_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WindowEditBox.h"

class CFrameworkWndEditBox : public CWindowEditBox  
{
public:
	CFrameworkWndEditBox(CWindowControl *wc);
	CFrameworkWndEditBox(int id, int row, int column, int width = 3, int height = 3);
	virtual ~CFrameworkWndEditBox();

	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);

	CFont*	m_labelFont;

	static CFrameworkWndEditBox * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == EditBoxControl)
			return (static_cast <CFrameworkWndEditBox *> (pwc));
		else
			return NULL;
	}

	HBITMAP hIconBitmap;

	//{{AFX_MSG(CFrameworkWndEditBox)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FRAMEWORKWNDEDITBOX_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_)
