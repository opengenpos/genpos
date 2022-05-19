#if !defined(AFX_WINDOWLISTBOX_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_)
#define AFX_WINDOWLISTBOX_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowButton.h : header file
//

#include "WindowControl.h"



/////////////////////////////////////////////////////////////////////////////
// CWindowListBox window

class CWindowListBox : public CWindowControl
{
// Construction
public:
	CWindowListBox (CWindowControl *wc = 0);
	CWindowListBox (int id, int row, int column, int width = 2, int height = 2);
	virtual ~CWindowListBox();

// Attributes
public:
	enum {stdWidth = STD_GRID_WIDTH, stdHeight = 15, stdLeading = 1};

	typedef enum {ListBoxFontSize6 = 0, ListBoxFontSize8, ListBoxFontSize10, ListBoxFontSize12, ListBoxFontSize14, ListBoxFontSize16} ListBoxFontSize;
	typedef enum {CaptionAlignmentHLeft = 0, CaptionAlignmentHCenter, CaptionAlignmentHRight, CaptionAlignmentVTop, CaptionAlignmentVMiddle, CaptionAlignmentVBottom} CaptionAlignment;


	/*Structure containing all the attributes to be serialized for the WindowListBox class
	any new attributes need to be added at the end of the struct so data from 
	old layout files does not get corrupted during serialization*/
	typedef struct _tagWinLbAttributes{
		unsigned long signatureStart;
		TCHAR myIcon[30];								//ListBox icon file nale
		BOOL HorizontalOriented;						//Caption Orientation - TRUE for horizontal, FALSE for Vertical
	}WinLbAttributes;

	WinLbAttributes lbAttributes;
	CString m_Icon;

	int nEndOfArchive;

// Operations
public:
	virtual void Serialize( CArchive& ar );

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);
	virtual BOOL WindowDisplay (CDC* pDC){return FALSE;}
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}
	virtual void ControlSerialize (CArchive &ar);

	static CRect getRectangle (int row, int column);
	static CRect getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult);

	static CWindowListBox * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == ListBoxControl)
			return (static_cast <CWindowListBox *> (pwc));
		else
			return NULL;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowListBox)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWLISTBOX_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_)
