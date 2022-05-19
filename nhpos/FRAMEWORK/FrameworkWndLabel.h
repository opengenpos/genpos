// FrameworkWndLabel.h: interface for the CFrameworkWndLabel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEWORKWNDLABEL_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_)
#define AFX_FRAMEWORKWNDLABEL_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WindowLabel.h"

class CFrameworkWndLabel : public CWindowLabel  
{
public:
	CFrameworkWndLabel(CWindowControl *wc);
	CFrameworkWndLabel(int id, int row, int column, int width = 3, int height = 3);
	virtual ~CFrameworkWndLabel();

	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	virtual int ChangeRefreshIcon(TCHAR *tcsIconFileName);

	CFont*	m_labelFont;

	static CFrameworkWndLabel * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == LabelControl)
			return (static_cast <CFrameworkWndLabel *> (pwc));
		else
			return NULL;
	}

	HBITMAP hIconBitmap;

	//{{AFX_MSG(CFrameworkWndLabel)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FRAMEWORKWNDLABEL_H__123D5E10_8E89_41A9_85C1_656CFE73024E__INCLUDED_)
