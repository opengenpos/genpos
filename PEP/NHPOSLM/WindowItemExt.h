
#if !defined(WINDOWITEMEXT_H_INCLUDED)

#define WINDOWITEMEXT_H_INCLUDED

#include "WindowItem.h"

//class CDWinItem;


class CWindowItemExt : public CWindowItem
{
public:
	BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */);
	int SearchForControl(int row, int column);
	CWindowItemExt (int id, int row, int column, int width = 3, int height = 6);
	CWindowItemExt(CWindowControl *wc);
	virtual ~CWindowItemExt();

	void RemoveData(POSITION location);

	virtual void ControlSerialize (CArchive &ar);

 	virtual BOOL WindowDisplay (CDC* pDC);

	//friend CDWinItem;
	HBRUSH m_faceBrush;//The brush for the background also used for button background

	
	
	static CWindowItemExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == WindowContainer)
			return (static_cast <CWindowItemExt *> (pwc));
		else
			return NULL;
	}

	//{{AFX_MSG(CWindowItemExt)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
