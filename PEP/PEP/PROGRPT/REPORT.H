/////////////////////////////////////////////////////////////////////////////
// report.h : header file
//
// Copyright (C) 1996 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __REPORT_H__
#define __REPORT_H__

/////////////////////////////////////////////////////////////////////////////

#include "item.h"

/////////////////////////////////////////////////////////////////////////////

class CProgrptDoc;
class CReportView;

#define	RPT_NUM_ROW			(150 + 1)
#define	RPT_PT_SHEETX		30
#define	RPT_PT_SHEETY		(RPT_SPACE_Y +          m_sizeFont.cy)
#define	RPT_PT_SHEETYY		(RPT_SPACE_Y + pReport->m_sizeFont.cy)
#define	RPT_SHEET_LEFT		3
#define	RPT_SHEET_TOP		6
#define	RPT_SHEET_RIGHT		1
#define	RPT_SHEET_BOTTOM	1
#define	RPT_DEFAULT_ROW		5
#define	RPT_SPACE_Y			5
#define	RPT_LEVEL_X			80
#define	RPT_HIT_ADJUST		2
#define	RPT_HIT_LINE		2
#define	RPT_NML_LINE		1
#define	RPT_COLOR_WHITE		RGB(255, 255, 255)
#define	RPT_COLOR_GRAY		RGB(192, 192, 192)
#define	RPT_COLOR_BLACK		RGB(0, 0, 0)
#define	RPT_COLOR_LRED		RGB(255, 0, 0)
#define	RPT_COLOR_LBLUE		RGB(0, 0, 255)
#define	RPT_COLOR_DBLUE		RGB(0, 0, 128)
#define	RPT_COLOR_BG		RGB(0, 128, 128)
#define	RPT_INDEX_TMP		0
#define	RPT_INDEX_NEW		1
#define	RPT_CANCEL			(-1)

class CReport : public CObject
{
protected:
	DECLARE_SERIAL(CReport)
	CReport();
public:
	CReport(CProgrptDoc* pDoc);
	CReport(CProgrptDoc* pDoc, int nFile, int nDevice, CString& strTitle);
	virtual	~CReport();

	enum Mode
	{
		none, insertItem, insertRow, resizeSheet, resizeLoop, moveItem, selectItem
	};

// Attributes
public:
	int				m_nOperation;		// action parameter
	int				m_nGroup;			// action parameter
	int				m_nField;			// action parameter
	int				m_nItem;			// action parameter
	CSize			m_sizeReport;		// size of report (column, row)
	CSize			m_sizeSheet;		// size of sheet  (x, y)
	CSize			m_sizeFont;			// size of font   (x, y)
	CSize			m_sizeItem;			// size of item   (x, y)
private:
	int				m_nMagic;
	int				m_nFile;			// index number of file
	int				m_nDevice;			// index number of device
	CString			m_strTitle;			// report title
	CProgrptDoc*	m_pDoc;				// my document object
	CReportView*	m_pView;			// my view object
	CObList			m_ItemObj;			// item objects
	int				m_nAction;			// action ID
	BOOL			m_bCapture;			// state of capture
	int				m_nHit;				// hit row
	int				m_nInsert[2];
	CPoint			m_ptMove;
	CPoint			m_ptDown;			// select function
	CPoint			m_ptLast;			// select function
	CRect			m_rectSelect;		// select function
	CRect			m_rectLast;			// select function
	CObList			m_SelectObj;		// selected items
	CObList			m_PasteObj;			// paste items

public:
	int				OpenDocument(int nIndex, const wchar_t * pszPathName);
	BOOL			SaveDocument(int nIndex, const wchar_t * pszPathName);
	long			GetUsedSize();
	CProgrptDoc*	GetDoc() { return m_pDoc; }

	int				GetUnique() { return m_nMagic; }
	int				GetFileIndex() { return m_nFile; }
	int				GetDevice() { return m_nDevice; }
	CString			GetTitle() { return m_strTitle; }
	void			SetTitle(CString& strTitle);
	CString			GetExtendedTitle();
	wchar_t *		FormatReportInfo(int* pnNo, wchar_t * pszBuffer);

	CReportView*	GetView() { return m_pView; }
	void 			SetView(CReportView* pView) { m_pView = pView; }
	void			ActivateWindow();
	void			DestroyWindow();

	POSITION		GetHeadPosition()
						{ return m_ItemObj.GetHeadPosition(); }
	POSITION		GetTailPosition()
						{ return m_ItemObj.GetTailPosition(); }
	CItem*			GetNext(POSITION& pos)
						{ return (CItem*)m_ItemObj.GetNext(pos); }
	CItem*			GetPrev(POSITION& pos)
						{ return (CItem*)m_ItemObj.GetPrev(pos); }
	CItem*			GetAt(POSITION pos)
						{ return (CItem*)m_ItemObj.GetAt(pos); }
	void			RemoveAt(POSITION pos)
						{ m_ItemObj.RemoveAt(pos); }
	POSITION		InsertAfter(POSITION pos, CItem* pItem)
						{ return m_ItemObj.InsertAfter(pos, pItem); }
	POSITION		InsertBefore(POSITION pos, CItem* pItem)
						{ return m_ItemObj.InsertBefore(pos, pItem); }
	POSITION		Find(CItem* pItem, POSITION pos = NULL)
						{ return m_ItemObj.Find(pItem, pos); }

		POSITION	Insert(CItem* pItem);
		POSITION	Insert(CLoopItem* pItem);
			void	MoveItemRow(CItem* pItem, int nRow);
			void	Action(int nState);
			void	UpdateSheetSize(int nRow, BOOL bModified = TRUE);
			void	SetScrollInfo();

			void	InvalidateRect();
			void	InvalidateInsertLine(CDC* pDC, int nRow);
			int		GetInsertLine(int nRow);
			int		GetItemRow(int nY);
			CRect	GetRowRect(int nRow);
			CRect	GetResizeRect();

			void	OnDraw(CDC* pDC);
			void	OnMouseMove(UINT nFlags, CPoint point);
			void	OnLButtonDown(UINT nFlags, CPoint point);
			void	OnLButtonUp(UINT nFlags, CPoint point);
			void	OnLButtonDblClk(UINT nFlags, CPoint point);
			void	OnEditCut();
			BOOL	OnUpdateEditCut();
			void	OnEditCopy();
			BOOL	OnUpdateEditCopy();
			void	OnEditPaste();
			void	OnEditClear();
			BOOL	OnUpdateEditClear();
			BOOL	OnUpdateEditInsertrow();
private:
			void	Init(int nRow);
			void	InvalidateSelect(CDC* pDC, BOOL bCancel = FALSE);
			void	DrawSheet(CDC* pDC);
			void	DrawInsert(CDC* pDC);
			void	DrawSelect(CDC* pDC);
			void	DrawItems(CDC* pDC);
			void	MouseMoveNone(CDC* pDC, CPoint point);
			void	MouseMoveLoop(CDC* pDC, CPoint point);
			void	MouseMoveItem(CDC* pDC, CPoint point);
			void	MouseMoveSelect(CDC* pDC, CPoint point);
			void	MouseMoveInsert(CDC* pDC, CPoint point);
			void	MouseMoveSheet(CDC* pDC, CPoint point);
			void	LButtonDownNone(CDC* pDC, CPoint point);
			void	LButtonDownInsert(CDC* pDC, CPoint point);
			void	LButtonDownRow(CDC* pDC, CPoint point);
			void	LButtonUpLoop(CDC* pDC, CPoint point);
			void	LButtonUpItem(CDC* pDC, CPoint point);
			void	LButtonUpSelect(CDC* pDC, CPoint point);
			void	CopyItem(int nMagic);
			void	PasteItem(CDC* pDC, int nIndex, int nMagic);
			BOOL	RemoveItem(CDC* pDC);
			void	RemoveSelect(CDC* pDC);

	virtual void	Serialize(CArchive& ar);
#ifdef _DEBUG
    void Dump(CDumpContext& dc) const;
#endif
};


#endif
