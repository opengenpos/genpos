// WindowLabelExt.h: interface for the CWindowLabelExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWLABELEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_)
#define AFX_WINDOWLABELEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_

#include "WindowLabel.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWindowLabelExt : public CWindowLabel  
{
public:
	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	CWindowLabelExt(CWindowControl *wc);
	CWindowLabelExt(int id, int row, int column);
	virtual ~CWindowLabelExt();

//	virtual void ControlSerialize (CArchive &ar);
	BOOL WindowDisplay (CDC *pDC);
	static CWindowLabelExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == LabelControl)
			return (static_cast <CWindowLabelExt *> (pwc));
		else
			return NULL;
	}
	CFont*	m_labelFont;

	//{{AFX_MSG(CWindowLabelExt)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WINDOWLABELEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_)
