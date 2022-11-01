
#if !defined(WINDOWLISTBOXEXT_H_INCLUDED)

#define WINDOWLISTBOXEXT_H_INCLUDED

#include "WindowListBox.h"
#include "WindowItemExt.h"

class CDListBoxEdit;


class CWindowListBoxExt : public CWindowListBox
{
public:
	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	virtual ~CWindowListBoxExt();
	void ControlSerialize(CArchive &ar);
	enum standards {stdWidth = STD_GRID_WIDTH, stdHeight = STD_GRID_WIDTH, stdLeading = 1};

	CWindowListBoxExt (int id, int row, int column);
	CWindowListBoxExt(CWindowControl *wc);
	CWindowListBoxExt & operator = (const CDListBoxEdit & cdt);
	

	BOOL WindowDisplay (CDC *pDC);
	
	static CRect getRectangle (int x, int y);
	static CRect getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult);
	static void DrawListBox(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CWindowItemExt* pWndItem);
	friend CDListBoxEdit;

	static CWindowListBoxExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == ListBoxControl)
			return (static_cast <CWindowListBoxExt *> (pwc));
		else
			return NULL;
	}

	CFont*	m_listboxFont;
	//{{AFX_MSG(CFrameworkWndListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif