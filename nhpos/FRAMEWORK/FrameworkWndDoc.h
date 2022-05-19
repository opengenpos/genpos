#if !defined(AFX_FRAMEWORKWNDDOC_H__B5271C7A_B374_4A7C_8022_1C5EDB32A030__INCLUDED_)
#define AFX_FRAMEWORKWNDDOC_H__B5271C7A_B374_4A7C_8022_1C5EDB32A030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameworkWndDoc.h : header file
//
#include "WindowDocument.h"
#include "FrameworkWndItem.h"
#include "FrameworkWndButton.h"
#include "FrameworkWndText.h"
#include "MessageDialog.h"
#include "bl.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndDoc window

class CFrameworkWndDoc : public CWindowDocument
{

// Attributes
public:

	CFileStatus	m_SavedLayoutFileStatus;	//Time Layout file last modified
	UINT	m_nTimer;		       //Timer Object to reload layout, FRAME_TIMER_CMD_RELOADLAYOUT
	UINT	m_nTimerOep;	       //Timer Object to reload Oep windows, FRAME_TIMER_CMD_RELOADOEP
	UINT	m_ntimerSignout;       // Timer for the Auto Sign Out
	UINT	m_ntimerForceShutdown; // Timer for doing a Force Shutdown
	UINT	m_ntimerSecondsTimer;  // Timer counting down utility timers once per second, FRAME_TIMER_CMD_SECONDS_TIMER
	WPARAM  m_wParamWindowTranslation; //from WM_APP_SHOW_HIDE_GROUP message for Window Translation in OnShowHideGroupMsgRcvd()

	CMessageDialog   *m_pMessageDialog;
// Operations
public:
	DECLARE_SERIAL(CFrameworkWndDoc)
	CFrameworkWndDoc();
	virtual ~CFrameworkWndDoc();

	virtual BOOL CreateDocument (CWnd *pParentWnd = NULL, UINT nID = 0, CRect *rectSize = 0);
/*
	virtual void ClearDocument (void);
*/
	void RefreshOepWindows (void);

	void Serialize( CArchive& ar );
	void DefaultUserScreenLocal();
	void ShowHideGroupMsgRcvd (WPARAM wParam, LPARAM lParam);

	virtual int SearchForItem (int row, int column, UINT id = 0);
	virtual int SearchForItemCorner (int row, int column, UINT id = 0);
	virtual CWindowControl * SearchForItemByName (TCHAR *tcWindowName);
	virtual CWindowControl * SearchForItemByTypeApplyFunc (int iType = 1, int (*func)(CWindowControl *pw) = 0);
	virtual CWindowControl * SearchForItemByNameInWindow (TCHAR *tcWindowName, TCHAR *tcItemName, CWindowControl *(*pWindowControlList) = 0);
	virtual CWindowControl * SearchForItemByPrefix (int WindowPrefix);
	void CFrameworkWndDoc::DisplayWindowsByGroup (int iGroupNo);
	void CFrameworkWndDoc::DismissWindowsByGroup (int iGroupNo);

	VOID  InternalProcessFrameworkMessage (void  *pFrameworkObject);
	BOOL  InitDocumentViewWnd (void);

	SHORT Mode;

	//void OnLButtonUpFindButton(int row, int column);

/*	
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100) { return FALSE; }
	virtual BOOL WindowDisplay (CDC* pDC) {return FALSE;}
	virtual BOOL WindowDestroy (void) {PostMessage(WM_QUIT, 0, 0); return FALSE;}
	virtual void ControlSerialize (CArchive &ar) {}
*/
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameworkWndDoc)
	//}}AFX_VIRTUAL

private:
	AllObjects  m_myObjectArray[5];
	int         m_myObjectArrayIndex;
	CScrollDocument  *m_pScrollDocument;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameworkWndDoc)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnDefaultWinLoad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPopupWindowByName(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPopdownWindowByName(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnEngineMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBiometricMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXmlMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShutdownMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPowerMsgRcvd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowHideGroupMsgRcvd(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEWORKWNDDOC_H__B5271C7A_B374_4A7C_8022_1C5EDB32A030__INCLUDED_)
