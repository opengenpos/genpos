
#if !defined(WINDOWTEXTEXT_H_INCLUDED)

#define WINDOWTEXTEXT_H_INCLUDED

#include "WindowText.h"
class CDTextItem;



class CWindowTextExt : public CWindowText
{
public:
	BOOL WindowCreate(CWindowControl *pParentWnd, UINT nID, int scale);
	CWindowTextExt(int id, int row, int column, int width = 3, int height = 6);
	CWindowTextExt(CWindowControl *wc);
	void ControlSerialize (CArchive &ar);
	BOOL WindowDisplay (CDC* pDC);
	CRect getRectangleSized(int column, int row, USHORT usWidth, USHORT usHeight);
	friend CDTextItem;
	CWindowTextExt& operator = (const CDTextItem & cdt);

	//added Icon support to OEP window buttons
	CString csOEPIconFileName;
	CString csOEPIconFilePath;

	static CWindowTextExt * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == TextControl)
			return (static_cast <CWindowTextExt *> (pwc));
		else
			return NULL;
	}
	BOOL		PreCreateWindow(CREATESTRUCT& cs);
	//{{AFX_MSG(CFrameworkWndText)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif