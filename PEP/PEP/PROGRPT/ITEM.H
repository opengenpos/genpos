/////////////////////////////////////////////////////////////////////////////
// item.h : header file
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

#ifndef __ITEM_H__
#define __ITEM_H__

class CReport;

#define	ITEM_NUM_ELEMENT	3

class CItem : public CObject
{
	DECLARE_DYNAMIC(CItem)
	CItem();
public:
	CItem(const CItem& Src);
	CItem(int nOperation);
	~CItem();

// Attributes
public:
			int		m_nFile;
			int		m_nOperation;
			CPoint	m_pt;
			int		m_nWidth;
protected:
#pragma pack(push, 1)
		typedef struct _ELEMENT
		{
			short	m_nGroup;
			short	m_nField[2];
			short	m_nItem;
		} ELEMENT;
#pragma pack(pop)
			int		m_nFormat[2];
			int		m_nCondition;
			ELEMENT	Element[ITEM_NUM_ELEMENT];
			BOOL	m_bHit;

// Operations
public:
	const CItem& operator =(const CItem& itemSrc);

	virtual int		SetItem(int nFile, const wchar_t * pszData) = 0;
	virtual void	GetItem(int nFile, wchar_t * pszData, int* pnKey = NULL) = 0;
	virtual	int		Insert(CReport* pReport, CDC* pDC, int nFile, int nRow) = 0;
	virtual	void	Draw(CReport* pReport, CDC* pDC) = 0;
	virtual BOOL	MouseMove(CReport* pReport, CDC* pDC, CPoint point) = 0;
	virtual BOOL	LButtonDown(CReport* pReport, CDC* pDC, CPoint point) = 0;
	virtual void	LButtonUp(CReport* pReport, CDC* pDC, CPoint point) = 0;
	virtual BOOL	LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point) = 0;
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CPoint point, CRect* pitems, BOOL bHit = FALSE) = 0;
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CRect select, CRect* pitems, BOOL bHit = FALSE) = 0;
	virtual void	Cancel(CReport* pReport, CDC* pDC) = 0;
	virtual int		Remove(CReport* pReport, CDC* pDC, int nRow) = 0;
	virtual BOOL	HitOnRow(CReport* pReport, CDC* pDC, int nRow) = 0;
	virtual BOOL	GetHitState();
protected:
			void	Init();
			void	AssignCopy(const CItem& itemSrc);
			void	InvalidateRect(CScrollView* pView, CDC* pDC, CRect rect);
			void	InvalidateRow(CReport* pReport, CDC* pDC);

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};


class CLoopItem : public CItem
{
	DECLARE_DYNAMIC(CLoopItem)
public:
	CLoopItem(const CLoopItem& Src);
	CLoopItem(int nOperation);

// Attributes
public:
protected:
			BOOL	m_bResize;

// Operations
public:
	virtual int		SetItem(int nFile, const wchar_t * pszData);
	virtual void	GetItem(int nFile, wchar_t * pszData, int* pnKey = NULL);
	virtual	int		Insert(CReport* pReport, CDC* pDC, int nFile, int nRow);
	virtual	void	Draw(CReport* pReport, CDC* pDC);
	virtual BOOL	MouseMove(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	LButtonDown(CReport* pReport, CDC* pDC, CPoint point);
	virtual void	LButtonUp(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CPoint point,
						   CRect* pitems, BOOL bHit = FALSE);
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CRect select,
						   CRect* pitems, BOOL bHit = FALSE);
	virtual void	Cancel(CReport* pReport, CDC* pDC);
	virtual int		Remove(CReport* pReport, CDC* pDC, int nRow);
	virtual BOOL	HitOnRow(CReport* pReport, CDC* pDC, int nRow);
			int		GetSelectLevel(CReport* pReport, CDC* pDC, CPoint point);					
protected:
			void	UpdateKeyFlag(int nFile, int nMajor, int nCondition, int* pnKey);
			void	DrawLines(CReport* pReport, CDC* pDC, int nHigh);
			void	DrawProperties(CReport* pReport, CDC* pDC, int nHigh);
		CLoopItem*	PrevItem(CReport* pReport);
		CLoopItem*	NextItem(CReport* pReport);
		CLoopItem*	PairItem(CReport* pReport);
			int		GetLevel(CReport* pReport, int nRow, CLoopItem **pFind);
			CRect	GetLineRect(CReport* pReport, int nHigh);
			void	InvalidatePair(CReport* pReport, CDC* pDC, CLoopItem* pPair);

public:
#ifdef _DEBUG
    void Dump(CDumpContext& dc) const;
#endif

	friend class CLoopPropDlg;
};


#define	PITEM_FORMAT_RIGHT	0x01
#define	PITEM_FORMAT_LEFT	0x04
#define	PITEM_FORMAT_WIDE	0x08

class CPrintItem : public CItem
{
	DECLARE_SERIAL(CPrintItem)
	CPrintItem();

public:
	CPrintItem(const CPrintItem& Src);
	CPrintItem(int nOperation);

// Operations
public:
	const CItem& operator =(const CItem& itemSrc);

	virtual int		SetItem(int nFile, const wchar_t* pszData);
	virtual void	GetItem(int nFile, wchar_t * pszData, int* pnKey = NULL);
	virtual	int		Insert(CReport* pReport, CDC* pDC, int nFile, int nRow);
	virtual	void	Draw(CReport* pReport, CDC* pDC);
	virtual BOOL	MouseMove(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	LButtonDown(CReport* pReport, CDC* pDC, CPoint point);
	virtual void	LButtonUp(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point);
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CPoint point, CRect* pitems, BOOL bHit = FALSE);
	virtual BOOL	Select(CReport* pReport, CDC* pDC, CRect select, CRect* pitems, BOOL bHit = FALSE);
	virtual void	Cancel(CReport* pReport, CDC* pDC);
	virtual int		Remove(CReport* pReport, CDC* pDC, int nRow);
	virtual BOOL	HitOnRow(CReport* pReport, CDC* pDC, int nRow);
protected:
			int		FormatItemString(CReport* pReport, wchar_t * pszData);
			CRect	GetRect(CReport* pReport, CPoint& point, int nLength);
			CPoint	GetPoint(CReport* pReport, int nX, int nY);
			void	Invalidate(CReport* pReport, CDC* pDC);

public:
	virtual void	Serialize(CArchive& ar);
#ifdef _DEBUG
    void Dump(CDumpContext& dc) const;
#endif

	friend class CPrintPropDlg;
	friend class CPrintDetailDlg;
};


class CMatheItem : public CPrintItem
{
	DECLARE_SERIAL(CMatheItem)
	CMatheItem();
public:
	CMatheItem(const CMatheItem& Src);
	CMatheItem(int nOperation);

// Attributes
public:
protected:
			LONG	m_lData;

// Operations
public:
	const CItem& operator =(const CItem& itemSrc);

	virtual int		SetItem(int nFile, const wchar_t * pszData);
	virtual void	GetItem(int nFile, wchar_t * pszData, int* pnKey = NULL);
	virtual	int		Insert(CReport* pReport, CDC* pDC, int nFile, int nRow);
	virtual	void	Draw(CReport* pReport, CDC* pDC);
protected:
			int		SetItemValue(int nFile, const wchar_t * pszData);
			void	GetItemValue(int nFile, wchar_t * pszData);
			int		FormatItemString(CReport* pReport, wchar_t * pszData);
	virtual BOOL	LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point);

public:
	virtual void	Serialize(CArchive& ar);
#ifdef _DEBUG
    void Dump(CDumpContext& dc) const;
#endif

	friend class CMathePropDlg;
};


#endif
