#if !defined(AFX_WINDOWDOCUMENT_H__ED115701_03ED_4010_BBD1_A96B14E93A00__INCLUDED_)
#define AFX_WINDOWDOCUMENT_H__ED115701_03ED_4010_BBD1_A96B14E93A00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowDocument.h : header file
//

#include <afxtempl.h>

#include "WindowControl.h"
#include "DefaultData.h"
#include "WindowGroup.h"


/*Structure containing all the attributes to be serialized for the WindowDocument class
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagWinDocAttributes{
	unsigned long	signatureStart;
	CRect			myStartupClientRect;
	CRect			myClientRect;
	unsigned long	Version;		
	UINT			uiInitialID;		//used to show initial active window

	//BEGIN File Summary members
	UINT			nVersion[3];		//1st index is major vers, 2nd is minor vers. and 3rd index is release
	TCHAR			summary[1025];		//dialog limits text to 1024 so we add an extra space for NULL character at end
	SYSTEMTIME		lastModified[5];	//store system time of the last 5 times the document has been saved
	TCHAR			PEPfile[31];		//primary PEP file to use with layout - dialog limits text to 30 so we add an extra space for NULL character at end
	TCHAR			IconDir[MAX_PATH + 1]; //icon directory - add 1 for null terminating character
}WinDocAttributes;


/////////////////////////////////////////////////////////////////////////////
// CWindowDocument window
class CDialogTesterView;

class CWindowDocument : public CWindowControl
{
// Construction
public:
	// The units for the parameters are in grids so we are basically saying that the
	// CWindowDocument is a control which covers the entire screen which on an 800x600
	// resolution screen would be from 0,0 (the top left corner) to 54, 40 (the bottom
	// right corner.
	//    800x600  -> 54 grid columns and 40 grid rows
	//   1024x768  -> 69 grid columns and 51 grid rows
	//   1280x1024  -> 86 grid columns and 69 grid rows
	//   1920x1080 -> 128 grid columns and 72 grid rows  (LAYOUT_SCREENWIDTH_1920 x LAYOUT_SCREENHEIGHT_1920)
	// defaults for this constructor need to be:
	//   CWindowDocument::nGridColumns
	//   CWindowDocument::nGridRows
	CWindowDocument(int nGridColumns = 128, int nGridRows = 72);
	virtual ~CWindowDocument();

	virtual BOOL CreateDocument (CWnd *pParentWnd = NULL, UINT nID = 0, CRect *rectSize = 0);

	virtual void ClearDocument (void);

	virtual void Serialize( CArchive& ar );

	// We provide a number of different ways to search through the layout manager document's contents.
	// There are several different data structures that make up a layout manager document which describe
	// the various data items contained in the document. The basic items are windows of various kinds
	// and the controls such as buttons that go into those windows.
	// Then we have more abstract data items such as window groups which are collections of windows.
	virtual int SearchForItem (int row, int column, UINT id = 0);
	virtual int SearchForItemCorner (int row, int column, UINT id = 0);
	virtual CWindowControl * SearchForItemByName (TCHAR *tcWindowName = 0);
	virtual CWindowControl * SearchForItemByPrefix (int	WindowPrefix);
	virtual CWindowControl * SearchForItemByNameInWindow (TCHAR *tcWindowName, TCHAR *tcItemName, CWindowControl *(*pWindowControlList) = 0);
	virtual	CWindowControl * SearchNextWindowItemDown(CWindowControl *pc, UINT nId);
	virtual	CWindowControl * SearchForItemByEventTrigger_WId (int SpecWinEventTrigger_WId);
	virtual CWindowControl * SearchForItemByWindowId (int iWindowNo);
	virtual CWindowControl * SearchForGroupActiveWindow (int iGroupNo);
	virtual int              SearchForGroupActiveWindowId (int iGroupNo);

	virtual	int SearchForItemByGroupNo (int iGroupNo, CList <CWindowControl *, CWindowControl *> &list);

	static CRect StdRectangle (void);

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100) { return FALSE; }
	
	virtual BOOL WindowDisplay (CDC* pDC) {return FALSE;}
	virtual BOOL WindowDestroy (void) {PostMessage(WM_QUIT, 0, 0); return FALSE;}
	virtual void ControlSerialize (CArchive &ar) {}


// Attributes
public:
	friend CDialogTesterView;


// Operations
public:
	virtual CWindowControl *GetDataThis (void) {if (pclListCurrent) { return listControls.GetAt (pclListCurrent);} else { return 0;} }
	virtual CWindowControl *GetDataNext (void) {pclListCurrent = pclList; if (pclList) { return listControls.GetNext (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataFirst (void) {pclListCurrent = pclList = listControls.GetHeadPosition (); if (pclList) { return listControls.GetNext (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataPrev (void) {pclListCurrent = pclList; if (pclList) { return listControls.GetPrev (pclList);} else { return 0;} }
	virtual CWindowControl *GetDataLast (void) {pclListCurrent = pclList = listControls.GetTailPosition (); if (pclList) { return listControls.GetPrev (pclList);} else { return 0;} }
	virtual unsigned GetDataCount (void) {return listControls.GetCount ();}
	virtual void AddDataLast (CWindowControl *myControl) {listControls.AddTail (myControl);}
	virtual CWindowControl *DeleteDataThis (void);

	virtual BOOL ToggleWindow (CWindowControl *pParent = 0, UINT nId = 0);
	virtual BOOL GetWindowName(UINT nId, TCHAR *tcsName /* [32] */ = 0); // tcsName must hold at least 32 text characters.
	virtual BOOL PopupWindow (CWindowControl *pParent = 0, UINT nId = 0);
	virtual BOOL PopupWindow (CWindowControl *pParent = 0, CWindowControl *pc = 0 );
	virtual BOOL PopupWindow (CWindowControl *pParent = 0, TCHAR *tcWindowName = 0);
	virtual BOOL PopupWindow (CWindowControl *pParent = 0, int WindowPrefix = 0);
	virtual BOOL PopdownWindow (CWindowControl *pParent = 0, CWindowControl *pc = 0 );
	virtual BOOL PopdownWindow (CWindowControl *pParent = 0, TCHAR *tcWindowName = 0);
	virtual BOOL PopdownWindow (CWindowControl *pParent = 0, int WindowPrefix = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowDocument)
	//}}AFX_VIRTUAL



// Implementation
public:
	DECLARE_DYNAMIC(CWindowDocument);


	WinDocAttributes documentAttributes;
	DefaultData DocumentDef;
	CString csSummary;
	CString csPEP;
	CString csIconDir;


	static CWindowDocument *pDoc;

	const static int  m_nStdDeciInchWidth;   // standard width in tenths of an inch, 10 = .10 inch
	const static int  m_nStdDeciInchHeight;  // standard height in tenths of an inch, 10 = .10 inch
	const static int  m_nStdDeciInchLeading; // standard button leading in tenths of an inch, 10 = .10 inch
	const static int  nGridColumns;
	const static int  nGridRows;
	// Attributes
public:
	CList <CWindowControl *, CWindowControl *> listControls;
	CList <CWindowGroup *, CWindowGroup *> listGroups;

	POSITION  pclListCurrent, pclList;

private:
	// disable the = operator.  We do not want C++ compiler generating one
	// for us and causing errors by creating copies of our data structures.
	CWindowDocument & operator = (CWindowDocument & junk);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWDOCUMENT_H__ED115701_03ED_4010_BBD1_A96B14E93A00__INCLUDED_)
