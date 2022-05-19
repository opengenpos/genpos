#if !defined(AFX_WINDOWTEXT_H__81BBCA5A_4D40_40E5_928F_BA2D6FADDC46__INCLUDED_)
#define AFX_WINDOWTEXT_H__81BBCA5A_4D40_40E5_928F_BA2D6FADDC46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowText.h : header file
//

#include "WindowControl.h"
#include "WindowButton.h"



/////////////////////////////////////////////////////////////////////////////
// CWindowText window

class CWindowText : public CWindowControl
{
// Construction
public:
	CWindowText(CWindowControl *wc = 0);
	CWindowText (int id, int row, int column, int width = 3, int height = 6);
	virtual ~CWindowText();

// Attributes
public:
	//Functional Type info of this text control
	// WARNING: There are dependencies on the order of these with the function assignment
	//          used in the Layout Manager CWindowTextItem Edit/Add dialog CDtextItem.
	//
	// WARNING: Be careful when adding and do not delete any of the enum values since these
	//          are used to determine the CWindowItemText function type and we want to
	//          maintain backwards compatibility.
	typedef enum {TypeSnglReceiptMain = 0,              // LCDWIN_ID_REG100
				TypeMultiReceipt1,                      // LCDWIN_ID_REG200
				TypeMultiReceipt2,                      // LCDWIN_ID_REG201
				TypeMultiReceipt3,                      // LCDWIN_ID_REG202
				TypeReceiptNotUsed1, 
				TypeReceiptNotUsed2,
				TypeLeadThru,                          // LCDWIN_ID_COMMON001
				TypeLeadThruDescriptor,                // LCDWIN_ID_COMMON002
				TypeSnglReceiptSub,                    // LCDWIN_ID_REG101
				TypeSnglReceiptScroll,                 // LCDWIN_ID_REG103
				TypeClock,                             // LCDWIN_ID_CLOCK01
				TypeSupProg, 
				TypeOEP, 
				TypeSnglReceiptCustDisplay,            // LCDWIN_ID_REG110, customer display window
				TypeLeadThruCustDisplay,               // LCDWIN_ID_COMMON004, customer display window
				TypeClockCustDisplay,                  // LCDWIN_ID_CLOCK02
				TypeOEPCustDisplay,                    // LCDWIN_ID_REG104, customer display window for OEP
				TypeMultiReceiptGuest1 = 101, 
				TypeMultiReceiptGuest2 = 102, 
				TypeMultiReceiptGuest3 = 103,
				TypeMultiReceiptTotal1 = 201, 
				TypeMultiReceiptTotal2 = 202, 
				TypeMultiReceiptTotal3 = 203} CWindowTextType; 
	int  nEndOfArchive;
	//Uncomment following CWindowTextType to add m_texttype to replace current use of m_myID as the storage for the text window type
	//Also add m_textType to the serialization
	//CWindowTextType m_textType;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowText)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void Serialize( CArchive& ar );
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);
	virtual BOOL WindowDisplay (CDC* pDC) {ASSERT(0); return FALSE; }
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}
	virtual void ControlSerialize (CArchive &ar);

	static CWindowText * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == TextControl)
			return (static_cast <CWindowText *> (pwc));
		else
			return NULL;
	}

	/*contains info for OEP windows, which are a type of text control, 
	Other types of text controls will not use these attributes so initialize to null*/
	typedef struct _tagTextAttributes{
		unsigned long signatureStart;
		UINT oepBtnWidth;
		UINT oepBtnHeight;
		UINT oepBtnDown;
		UINT oepBtnAcross;
		COLORREF oepBtnColor;
		CWindowButton::ButtonShape oepBtnShape;
		CWindowButton::ButtonPattern oepBtnPattern;
		LOGFONT oepBtnFont;
		COLORREF oepBtnFontColor;
		BOOL	UNUSED_isTouchScreen;			//2.2.0	-- this value is ignored, but remains for older layout file support
		int m_nTop;						//window origin y value -- used in Extended class
		int m_nLeft;					//window origin x value -- used in Extended class
		CWindowTextType type;
		//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
		TCHAR mOEPGroupNumbers[30];
		BOOL	UNUSED_mWindowSizedExplicitly;	//2.2.0	-- this value is ignored, but remains for older layout file support
		//added icon support to OEP window buttons
		CWindowButton::HorizontalIconAlignment	oepBtnIconHorizAlign;
		CWindowButton::VerticalIconAlignment	oepBtnIconVertAlign;
		TCHAR oepIconFileName[30];
	}TextAttributes;

	/*contians info for OEP windows, which are a type of text control, 
	Other types of text controls will not use these attributes so initialize to null*/
	TextAttributes textAttributes;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowText)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWTEXT_H__81BBCA5A_4D40_40E5_928F_BA2D6FADDC46__INCLUDED_)
