// WindowLabelExt.h: interface for the CWindowLabelExt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWEDITBOXEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_)
#define AFX_WINDOWEDITBOXEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_

#include "WindowEditBox.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWindowEditBoxExt : public CWindowEditBox  
{
public:
	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	CWindowEditBoxExt(CWindowControl *wc);
	CWindowEditBoxExt(int id, int row, int column);
	virtual ~CWindowEditBoxExt();

//	virtual void ControlSerialize (CArchive &ar);
	BOOL WindowDisplay (CDC *pDC);
	static CWindowEditBoxExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == EditBoxControl)
			return (static_cast <CWindowEditBoxExt *> (pwc));
		else
			return NULL;
	}
	CFont*	m_labelFont;

	//{{AFX_MSG(CWindowEditBoxExt)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WINDOWEDITBOXEXT_H__ADA5B66E_8E36_4F6C_9AAB_6FF680248F32__INCLUDED_)
