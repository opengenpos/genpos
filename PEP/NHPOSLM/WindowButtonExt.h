
#if !defined(WINDOWBUTTONEXT_H_INCLUDED)

#define WINDOWBUTTONEXT_H_INCLUDED

#include "WindowButton.h"
#include "WindowItemExt.h"

class CDButtonEdit;


class CWindowButtonExt : public CWindowButton
{
public:
	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	virtual ~CWindowButtonExt();
	void ControlSerialize(CArchive &ar);
	enum standards {stdWidth = STD_GRID_WIDTH, stdHeight = STD_GRID_WIDTH, stdLeading = 1};

	CWindowButtonExt (int id, int row, int column);
	CWindowButtonExt(CWindowControl *wc);
	CWindowButtonExt & operator = (const CDButtonEdit & cdt);
	

	BOOL WindowDisplay (CDC *pDC);
	
	static CRect getRectangle (int x, int y);
	static CRect getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult);
	static void DrawButton(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CWindowItemExt* pWndItem);
	friend CDButtonEdit;

	static CWindowButtonExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == ButtonControl)
			return (static_cast <CWindowButtonExt *> (pwc));
		else
			return NULL;
	}

	CFont*	m_buttonFont;
	//{{AFX_MSG(CFrameworkWndButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif