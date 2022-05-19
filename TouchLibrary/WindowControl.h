
#if !defined(AFX_WINDOWCONTROL_H__59EEF00B_FFC1_4968_956D_780EDDD9CE10__INCLUDED_)
#define AFX_WINDOWCONTROL_H__59EEF00B_FFC1_4968_956D_780EDDD9CE10__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
// WindowControl.h : header file
//



// SCHEMA_WINDOWCONTROL is used to differentiate versions of the WindowControl
// class when serializing.  It is used in the IMPLEMENT_SERIAL macro which
// is embedded in WindowControl.cpp as described in the MSDN library for
// implementing serializable objects using Microsoft MFC.

#define SCHEMA_WINDOWCONTROL  1

//The following defines are used with data member SpecWin of type WinControlATtributes
//The array is used to define attributes of a window. This allows parameterization of 
//dialog displays and other activites within the layout manager as well as presentation 
// of the windows within NHPOS. The nameing convention is SpecWin, followed by the word Index, to
//represent the index into the SpecWin array. SpecWin followed by the word Mask, is used to 
//turn on or off the appropiate bit or bits for that parameter. For Example to see how this is used
//look at newlayoutview.cpp at line 2923
#define SpecWinIndex_OEP              0    //Index to the SpecWin array member for OEP Window parameters
#define SpecWinMask_OEP_FLAG       0x01    //indicates window contains an OEP display
#define SpecWinMask_AUTO_OEP_FLAG  0x02    //indicates window contains an auto fill OEP display
#define SpecWinMask_FILTER_FLAG    0x04    //indicates window contains Dynamic PLU with filter conditions
#define SpecWinMask_SPCL_OEP_FLAG  0x08    //indicates window contains specialized OEP display group number

#define SPECWIN_SPCL_OEPGROUP_STR_PREAUTH     _T("201")  // 201, display of Preauth Tenders, BL_OEPGROUP_STR_PREAUTH
#define SPECWIN_SPCL_OEPGROUP_STR_REASONCODE  _T("202")  // 202, display of Reason Codes, BL_OEPGROUP_STR_REASONCODE
#define SPECWIN_SPCL_OEPGROUP_STR_AUTOCOUPON  _T("203")  // 203, display of Auto Coupon, BL_OEPGROUP_STR_AUTOCOUPON
#define SPECWIN_SPCL_OEPGROUP_STR_OPENCHECK   _T("204")  // 204, display of Open Guest Checks, BL_OEPGROUP_STR_OPENCHECK


#define UserDefaultWinIndex               1   //Index to the User Default Window array member
#define UserDefaultWin_FLAG	            0x01  //Indicates this window to be used as default window.
#define UserDefaultWin_WEB_BROWSER      0x02  //indicates window contains a web browser control display, IDC_WEB_BROWSER
#define UserDefaultWin_DOCU_VIEW        0x04  //indicates window contains a document/view display, IDC_REPORT_SCROLLABLE
#define UserDefaultWin_SHOWHIDE_GROUP   0x08  //indicates window is part of group permissions show/hide, WM_APP_SHOW_HIDE_GROUP, wParma == 1
#define UserDefaultWin_WINTRANSLATE     0x10  //indicates window should NOT be translated, WM_APP_SHOW_HIDE_GROUP, wParma == 2
#define UserDefaultWin_DISPLAYTRANS     0x20  //indicates window should be translated to second display.
#define UserDefaultWin_ROLLUP_WIN       0x40  //indicates window is a roll up window which will roll up or roll down when clicked.

#define SpecWinIndex_SPR_Type         2    //Index to the SUPR, PROG, REG type access rules,  Rel 2.1.0 only
#define SpecWinMask_SPR_SUPR       0x01    //Indicates this window to be used with SUPR access  Rel 2.1.0 only
#define SpecWinMask_SPR_PROG       0x02    //Indicates this window to be used with PROG access  Rel 2.1.0 only
#define SpecWinMask_SPR_REG        0x04    //Indicates this window to be used with REG access  Rel 2.1.0 only
#define SpecWinMask_SPR_AutoClose  0x10    //Indicates that this control will automatically close the container it is in.

#define SpecWinMask_SPR_SPR_MASK   (SpecWinMask_SPR_SUPR | SpecWinMask_SPR_PROG | SpecWinMask_SPR_REG)

#define SpecWinIndex_WIN_ATTRIB01     2    //Index to the window physical attributes
#define SpecWinIndex_WIN_NOBORDER  0x08    //Indicates that the window should not have a border around it
#define SpecWinIndex_WIN_BORDERLFT 0x10    //Indicates that the window should have a border on left edge
#define SpecWinIndex_WIN_BORDERRHT 0x20    //Indicates that the window should have a border on right edge
#define SpecWinIndex_WIN_BORDERTOP 0x40    //Indicates that the window should have a border on top edge
#define SpecWinIndex_WIN_BORDERBOT 0x80    //Indicates that the window should have a border on bottom edge

#define SpecWinIndex_EventTrigger     3    //Index to the SpecWin array member for event triggering
#define SpecWinEventTrigger_Indic  0x80    //Indicates window is triggered by an event if 1
#define SpecWinEventTrigger_Mask   0x0F    //used to identify the window that will be triggered by an event.  See below IDs.
#define SpecWinEventTrigger_WId_OrderDec  1    //ID of Window used for order declaration event

#define SpecWinIndex_ConnEngine       4        //Index to the SpecWin array member for Connection Engine related settings
#define SpecWinEventConnEngine_Indic   0x01    //Indicates control is used with Connection Engine
#define SpecWinEventScript_Indic       0x02    //Indicates control is used with scripts and can be changed through scripts

#define SpecWinIndex_FILTER_Conditions  5        //Index to the SpecWin array member for Connection Engine related settings
#define SpecWinFilter_ValueMask        0x0f     // lower nibble is used for the filter value
#define SpecWinFilter_Adj              0x10     // filter using the ajective index in the value mask area.
#define SpecWinFilter_AdjNonZero       0x20     // filter using the price to display only non-zero priced PLU varients.
#define SpecWinFilter_AdjValueLogic    0xC0     // equal to (0), less than (0x40), greater than (0x80) adjective index value for PLU variants.
#define SpecWinFilter_AdjValueLogicLT  0x40     // display if PLU adjective index value less than filter value for PLU variants.
#define SpecWinFilter_AdjValueLogicGT  0x80     // display if PLU adjective index value greater than filter value for PLU variants.

#define MaxSizeOfMyCaption 29		//max size of the m_myCaption in the WinControlAttributes structure. This value is
									//one less than the size of the TCHAR m_myCaption. This value is used to set the limit on
									//edit boxes that need to be restricted, as well as the limiters for copying data into this structure.						

/*Structure containing all the attributes to be serialized for the WindowControl class
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagWinControlAttributes{
	unsigned long signatureStart;
	unsigned long  m_nType;		//control type - button, window, document, or text
	UINT m_myId;				//id of control
	int  m_nRow;				//top left grid of control
	int  m_nColumn;				//top left grid of control
	int  m_nHeight;				//window height (pixels)
	int  m_nWidth;				//window width (pixels)
	COLORREF m_colorFace;		//controls face color
	COLORREF m_colorText;		//controls text color
	int  m_usWidthMultiplier;   //window width (grids)
	int  m_usHeightMultiplier;	//window Height (grids)
	int m_myAction;
	TCHAR m_myCaption[30];		//controls caption
	TCHAR m_myName[30];			//controls name
	BOOL isVirtualWindow;		//TRUE - virtual window, FALSE - Popup window
	BOOL useDefault;			//does control use document/window defaults or custom attributes
	LOGFONT lfControlFont;		//Font information for control
	UINT CurrentID;
	BOOL Selected;				//used in Extended class to indicate whether a control is selected or not
	BYTE SpecWin[8];			//used to mark the window as a special window that contains a special type of display

	//Additions made for Multiple-OEP Window Feature - CSMALL 1/11/06
	TCHAR mOEPGroupNumbers[30];
	BOOL	mWindowSizedExplicitly; // added for checkbox to size windows explicitly - CSMALL								
	ULONG	ulMaskGroupPermission;	//2.2.0 added group permission bitmask
}WinControlAttributes;

// Define the standard grid width in pixels which we use for
// drawing the grid squares for all controls. Also defined in
// NewLayout.h in the Layout Manager Project.
#define STD_GRID_WIDTH    15

class CWindowDocument;

/////////////////////////////////////////////////////////////////////////////
// CWindowControl window

class CWindowControl : public CWnd
{
// Construction
public:
	CWindowControl();
	CWindowControl(CWindowControl* wc);
	virtual ~CWindowControl();
	CWindowControl (UINT id, int row, int column, int width = 1 , int height = 1);
	
// Attributes
public:

	// These are the standard button width and height for the buttons in this window.
	// These metrics are in standard increments of one quarter of an inch.
	// In other words, if the value is 1 that means one quarter of an inch, 2 means
	// one half of an inch, 4 means 1 inch and 6 means 1 and one half inches.
	// The button leading is measured in tenths of an inch.
	enum {WinItemHalfInch = 2, WinItemThreeQuartersInch = 3, WinItemOneInch = 4, WinItemOneAndHalfInch = 6};

	// these are the supported types of controls.  Changes to here need to be reflected
	// in changes to CWindowControlSample SerializeNew to that it can generate the new
	// type of control.
	typedef enum {UnknownControl = 0, TextControl = 1, ButtonControl, WindowContainer, LabelControl, DocumentContainer, WindowGroup, WindowContainerAdHoc, ListBoxControl, EditBoxControl} CWindowControlType;
		

// Operations
public:
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100) = 0;

	virtual BOOL WindowDisplay (CDC* pDC) = 0;
	virtual BOOL WindowDestroy (void) = 0;
	virtual void ControlSerialize (CArchive &ar) = 0;

	virtual void Serialize (CArchive &ar);
	virtual CWindowControl & operator = (CWindowControl &wc);
	virtual BOOL PopupWindow (CWindowControl *pParent);
	virtual BOOL PopdownWindow ();
	virtual BOOL WindowUpdateText (TCHAR *tcText, ULONG ulBgColor = 0xFFFFFFFF, ULONG ulTextColor = 0xFFFFFFFF);
	virtual CWindowControl * SearchForItemByName (TCHAR *tcWindowName = 0) { return 0;}  // do nothing unless subclass needs to do something.
	virtual int ChangeRefreshIcon(TCHAR *tcsIconFileName) { return 0; }                  // do nothing unless subclass needs to do something.

	ULONG GetGroupPermission();
	void  SetGroupPermission(ULONG ulMaskGroupPermission);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	friend CWindowDocument;

	WinControlAttributes controlAttributes;

	CString m_FontName;
	CString myCaption;
	CString myName;	

	CWindowControl *pPrev;
	CWindowControl *pContainer;
	static int m_iSpecialId;


//	DECLARE_SERIAL (CWindowControl)


	//{{AFX_MSG(CWindowControl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWCONTROL_H__59EEF00B_FFC1_4968_956D_780EDDD9CE10__INCLUDED_)
