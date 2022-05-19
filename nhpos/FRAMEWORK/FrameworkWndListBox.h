#if !defined(AFX_FRAMEWORKWNDLISTBOX_H__2B7DDBDD_24D5_44E4_B0FF_0C7B477F9C7B__INCLUDED_)
#define AFX_FRAMEWORKWNLISTBOX_H__2B7DDBDD_24D5_44E4_B0FF_0C7B477F9C7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameworkWndButton.h : header file
//
#include "pif.h"
#include "WindowListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndButton window

class CFrameworkWndItem;

class CFrameworkWndListBox : public CWindowListBox
{
// Construction
public:
	CFrameworkWndListBox();
	virtual ~CFrameworkWndListBox();

// Attributes
public:
	CFont*	m_listboxFont;
	TCHAR   auchPluNo[16];    // Plu number for BATypePLU_Group, BL_PLU_INFO item for SpecWinMask_OEP_FLAG type window, STD_PLU_NUMBER_LEN
	UCHAR   uchPluAdjMod; // Adjective number for BATypePLU_Group
	int     m_DialogId;	      // The dialog control ID if an OEP auto generated button
	enum {SPL_BTN_LABEL = 7, SPL_BTN_BACK = 8, SPL_BTN_MORE = 9, SPL_BTN_DONE = 10, SPL_BTN_START = 11};
	HBITMAP hBitmapFocus;
//	HBITMAP hBitmapNoFocus;  // we use the same icon for both focus and no focus

protected:

// Operations
public:
	//enum {stdWidth = 15, stdHeight = 15, stdLeading = 1};
	CFrameworkWndListBox(int id, int row, int column, int width = 3, int height = 3);
	CFrameworkWndListBox(CWindowControl *wc);
//	CFrameworkWndButton & operator = (const CDButtonEdit & cdt);

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);
	virtual BOOL WindowDisplay (CDC *pDC);
	void DrawListBoxOnDisplayFocus (CDC* pDC);
	void DrawListBoxOnDisplayNoFocus (CDC* pDC);
	virtual int  ChangeRefreshIcon(TCHAR *tcsIconFileName);

	static void DrawListBox(int nIDCtl, LPDRAWITEMSTRUCT lpDraw, CFrameworkWndItem* pWndItem);
/*
	static CRect getRectangle (int x, int y);
	static CRect getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult);
*/
	static void sendNumericData(UCHAR *data); //processes the PLU data of a keyed PLU, Dept, Coupon, etc.
	static void sendListBoxMessageToBL(CHARDATA* pCharData);//Sends data into BusinessLogic From Button Press

	static CFrameworkWndListBox * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc->controlAttributes.m_nType == ListBoxControl)
			return (static_cast <CFrameworkWndListBox *> (pwc));
		else
			return NULL;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameworkWndButton)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameworkWndButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEWORKWNDLISTBOX_H__2B7DDBDD_24D5_44E4_B0FF_0C7B477F9C7B__INCLUDED_)
