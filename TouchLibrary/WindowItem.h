
#if !defined(WINDOWITEM_H_INCLUDED)

#define WINDOWITEM_H_INCLUDED

#include "stdafx.h"
#include "WindowControl.h"
#include "DefaultData.h"


// SCHEMA_WINDOWITEM is used to differentiate versions of the WindowItem
// class when serializing.  It is used in the IMPLEMENT_SERIAL macro which
// is embedded in WindowItem.cpp as described in the MSDN library for
// implementing serializable objects using Microsoft MFC.

#define SCHEMA_WINDOWITEM  1

/*Structure containing all the business rules used by the window item
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagBusinessRules{
	UINT NumOfPicks;			//number of selections that can be made in a popup window before it is dismissed
}BusinessRules;

/*Structure containing all the attributes to be serialized for the WindowItem class
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagWinItemAttributes{
	unsigned long signatureStart;
	UINT GroupNum;			//id of the window group this window item belongs to
	BusinessRules busRules;	//struct containing the business rules for this window item
	BOOL Show;				//used in Extended class to indicate whether this window item is visible or not
	UINT oepBtnWidth;
	UINT oepBtnHeight;
	UINT oepBtnDown;
	UINT oepBtnAcross;
	COLORREF oepBtnColor;
	CWindowButton::ButtonShape oepBtnShape;
	CWindowButton::ButtonPattern oepBtnPattern;
	LOGFONT oepBtnFont;
	COLORREF oepBtnFontColor;
	struct _WebBrowserStruct {
		WCHAR  url[256];
	} webbrowser;
}WinItemAttributes;

class CWindowItem : public CWindowControl
{
// Construction
public:
	CWindowItem(UINT id, int row, int column, int width = 12, int height = 12);
	CWindowItem (CWindowControl *wc = 0);
	virtual ~CWindowItem();

// Attributes
public:

	WinItemAttributes windowAttributes;
	int  nEndOfArchive;
	DefaultData WindowDef;

// Operations
public:
	virtual int SearchForItem (int row, int column, UINT id = 0);
	virtual int SearchForItemCorner (int row, int column, UINT id = 0);
	virtual CWindowControl *GetDataThis (void) {if (pclListCurrent) { return ButtonItemList.GetAt (pclListCurrent);} else { return 0;} }
	virtual CWindowControl *GetDataNext (void) {pclListCurrent = pclList; if (pclList) { return ButtonItemList.GetNext (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataFirst (void) {pclListCurrent = pclList = ButtonItemList.GetHeadPosition (); if (pclList) { return ButtonItemList.GetNext (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataPrev (void) {pclListCurrent = pclList; if (pclList) { return ButtonItemList.GetPrev (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataLast (void) {pclListCurrent = pclList = ButtonItemList.GetTailPosition (); if (pclList) { return ButtonItemList.GetPrev (pclList);} else { return 0;} }
	virtual unsigned GetDataCount (void) {return ButtonItemList.GetCount ();}
	virtual void AddDataLast (CWindowControl *button) {ButtonItemList.AddTail (button);}
	virtual CWindowControl *DeleteDataThis (void);

	CRect CWindowItem::getButtonRectangle (int row, int column);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ButtonItem)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual void Serialize (CArchive &ar);

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);

	virtual BOOL WindowDisplay (CDC* pDC);
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}
	virtual void ControlSerialize (CArchive &ar);

	static CWindowItem * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == WindowContainer)
			return (static_cast <CWindowItem *> (pwc));
		else
			return NULL;
	}
	
//	DECLARE_SERIAL (CWindowItem)

	CList <CWindowControl *, CWindowControl *> ButtonItemList;
	POSITION pclList;
	POSITION pclListCurrent;

private:
	// disable the = operator.  We do not want C++ compiler generating one
	// for us and causing errors by creating copies of our data structures.
	CWindowItem & operator = (CWindowItem & junk);

protected:




	static LPCTSTR myClassName;
	static BOOL    bAmRegistered;
	static DWORD myStyle;


	// Generated message map functions
protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnPaint(void);
	//{{AFX_MSG(WindowItem)
		// NOTE - the ClassWizard will add and remove member functions here.
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif