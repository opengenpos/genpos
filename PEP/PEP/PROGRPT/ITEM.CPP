/////////////////////////////////////////////////////////////////////////////
// item.cpp : implementation file
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

#include "stdafx.h"
#include "progrpt.h"

#include "progrdoc.h"
#include "report.h"
#include "reportvw.h"
#include "dlgloop.h"
#include "dlgprt.h"
#include "dlgmathe.h"

/////////////////////////////////////////////////////////////////////////////
// CItem

IMPLEMENT_DYNAMIC(CItem, CObject)

/////////////////////////////////////////////////////////////////////////////
// CItem diagnostics

#ifdef _DEBUG
void CItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CItem construction/destruction

CItem::CItem()
{
	Init();
}

CItem::CItem(const CItem& itemSrc)
{
	AssignCopy(itemSrc);
}

CItem::CItem(int nOperation)
{
	Init();
	m_nOperation = nOperation;
}

CItem::~CItem()
{
}

/////////////////////////////////////////////////////////////////////////////
// CItem operations

const CItem& CItem::operator =(const CItem& itemSrc)
{
	AssignCopy(itemSrc);
	return *this;
}

void CItem::InvalidateRect(CScrollView* pView, CDC* pDC, CRect rect)
{
	CRect work(rect);

	work.right  += 1;
	work.bottom += 1;

	pDC->LPtoDP(work);
	pView->InvalidateRect(&work, FALSE);
}

void CItem::InvalidateRow(CReport* pReport, CDC* pDC)
{
	CRect rect;

	rect.left   = RPT_PT_SHEETX;
	rect.top    = pReport->GetInsertLine(m_pt.y - 1);
	rect.right  = RPT_PT_SHEETX + pReport->m_sizeSheet.cx;
	rect.bottom = pReport->GetInsertLine(m_pt.y);

	pDC->LPtoDP(rect);
	pReport->GetView()->InvalidateRect(&rect, FALSE);
}

BOOL CItem::GetHitState()
{
	return m_bHit;
}

/////////////////////////////////////////////////////////////////////////////
// CItem implementations

void CItem::Init()
{
	m_nFile      = _FREE;
	m_nOperation = _FREE;
	m_nWidth     = _FREE;
	m_nFormat[0] = _FREE;
	m_nFormat[1] = _FREE;
	m_nCondition = _FREE;

	for (int nCount = 0; nCount < ITEM_NUM_ELEMENT; nCount++)
	{
		Element[nCount].m_nGroup    = _FREE;
		Element[nCount].m_nField[0] = _FREE;
		Element[nCount].m_nField[1] = _FREE;
		Element[nCount].m_nItem     = _FREE;
	}

	m_bHit = FALSE;
}

void CItem::AssignCopy(const CItem& itemSrc)
{
	m_nFile      = itemSrc.m_nFile;
	m_nOperation = itemSrc.m_nOperation;
	m_pt         = itemSrc.m_pt;
	m_nWidth     = itemSrc.m_nWidth;
	m_nFormat[0] = itemSrc.m_nFormat[0];
	m_nFormat[1] = itemSrc.m_nFormat[1];
	m_nCondition = itemSrc.m_nCondition;

	for (int nCount = 0; nCount < ITEM_NUM_ELEMENT; nCount++)
	{
		Element[nCount].m_nGroup    = itemSrc.Element[nCount].m_nGroup;
		Element[nCount].m_nField[0] = itemSrc.Element[nCount].m_nField[0];
		Element[nCount].m_nField[1] = itemSrc.Element[nCount].m_nField[1];
		Element[nCount].m_nItem     = itemSrc.Element[nCount].m_nItem;
	}

	m_bHit = itemSrc.m_bHit;
}

/////////////////////////////////////////////////////////////////////////////
// CLoopItem

IMPLEMENT_DYNAMIC(CLoopItem, CItem)

/////////////////////////////////////////////////////////////////////////////
// CItem diagnostics

#ifdef _DEBUG
void CLoopItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoopItem construction/destruction

CLoopItem::CLoopItem(const CLoopItem& itemSrc)
	: CItem(itemSrc)
{
	m_bResize = itemSrc.m_bResize;
}

CLoopItem::CLoopItem(int nOperation)
	: CItem(nOperation)
{
	m_bResize = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLoopItem operations

static wchar_t szLoopEntry[] = L"%d,%d,%d,%d,%hd,%hd";

int CLoopItem::SetItem(int nFile, const wchar_t * pszData)
{
	int nOperation, nRow;

	swscanf(pszData, szLoopEntry, &nOperation,
		   &nRow, &m_pt.x, &m_nCondition,
		   &Element[0].m_nGroup, &Element[0].m_nField[0]);

	ASSERT(m_nOperation + 1 == nOperation);

	m_nFile = nFile;

	// adjust position of row
	if (m_pt.x > 0)						// start item of loop
		m_pt.y = nRow - 1;
	else								// end item of loop
		m_pt.y = nRow;

	// convert from actual data to index data
	CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
	ASSERT(pConfig != NULL);
	Element[0].m_nGroup    = pConfig->GetGroupIndex(m_nOperation, Element[0].m_nGroup);
	Element[0].m_nField[0] = CInfoLoop::GetMajorIndex(nFile, Element[0].m_nGroup, Element[0].m_nField[0]);

	CInfoLoop* pInfo = CInfoLoop::GetField(nFile, Element[0].m_nGroup, Element[0].m_nField[0]);
	int breturn = -1;
	for (int nIndex = 0; nIndex < LOOP_NUM_COND; nIndex++){
		if (pInfo->Condition[nIndex].m_nNumber == m_nCondition){
			m_nCondition = nIndex;
			breturn = 0;
			break;
		}
	}

	return breturn;
}

void CLoopItem::GetItem(int nFile, wchar_t * pszData, int* pnKey /*=NULL*/)
{
	// adjust position of row
	int nRow;
	if (m_pt.x > 0)						// start item of loop
		nRow = m_pt.y + 1;
	else								// end item of loop
		nRow = m_pt.y;

	int nBias = 0;
	CInfoLoop* pInfo = CInfoLoop::GetField(nFile,
										   Element[0].m_nGroup,
										   Element[0].m_nField[0],
										   &nBias);
	int nCondition = pInfo->Condition[m_nCondition].m_nNumber;

	wsprintf(pszData, szLoopEntry,
			 1 + CFG_INDEX_LOOP, nRow, m_pt.x, nCondition,
			 1 + nBias + Element[0].m_nGroup, pInfo->m_nMajor);


//	if (pnKey != NULL && m_pt.x > 0)
	if (pnKey != NULL && m_pt.x == 1)
		UpdateKeyFlag(nFile, pInfo->m_nMajor, nCondition, pnKey);
}

#define	LOOPKEY_DEPT		0x0001
#define	LOOPKEY_MAJOR		0x0002
#define	LOOPKEY_PLU			0x0004
#define	LOOPKEY_REPORT		0x0008
#define	LOOPKEY_COUPON		0x0010
#define	LOOPKEY_CASHIER		0x0100
#define	LOOPKEY_SERVER		0x0200
#define	LOOPKEY_TERMINAL	0x0400

void CLoopItem::UpdateKeyFlag(int nFile, int nMajor, int nCondition, int* pnKey)
{
	switch (1 + nFile)					// caution !!
	{
	case 1:								// department file
		if (nMajor == 1)				// department number
			*pnKey |= LOOPKEY_DEPT;
		else if (nMajor == 2)			// major department number
			*pnKey |= LOOPKEY_MAJOR;
		break;
	case 2:								// PLU file
		if (nMajor == 1)				// PLU number
			*pnKey |= LOOPKEY_PLU;
		else if (nMajor == 2)			// department number
			*pnKey |= LOOPKEY_DEPT;
		else if (nMajor == 3)			// report code
			*pnKey |= LOOPKEY_REPORT;
		break;
	case 3:								// coupon file
		if (nMajor == 1)				// coupon number
			*pnKey |= LOOPKEY_COUPON;
		break;
	case 4:								// cashier file
		if (nMajor == 1)				// cashier number
			*pnKey |= LOOPKEY_CASHIER;
		break;
//  correct file id, to skip regacy server id, 1.0.09
//	case 5:								// server file
//		if (nMajor == 1)				// server number//
//			*pnKey |= LOOPKEY_SERVER;
//		break;
	case 6:								// individual financial file
//	case 7:								// indiv//idual financial file
		if (nMajor == 1)				// terminal number
			*pnKey |= LOOPKEY_TERMINAL;
		break;
	}
}

#define	LOOP_DEFAULT_ROW		3

int CLoopItem::Insert(CReport* pReport, CDC* pDC, int nFile, int nRow)
{
	CLoopItem* pItem;
	int nLevel = GetLevel(pReport, nRow, &pItem);
	if (nLevel != 0)
	{
		CInfoLoop* pInfo1 = CInfoLoop::GetField(pReport->GetFileIndex(),
											    pReport->m_nGroup,
											    pReport->m_nField);
		CInfoLoop* pInfo2 = CInfoLoop::GetField(pReport->GetFileIndex(),
											    pItem->Element[0].m_nGroup,
											    pItem->Element[0].m_nField[0]);

		if (nLevel >= pInfo1->m_nMinor || pInfo1 == pInfo2)
		{
			AfxMessageBox(IDS_ERRMSG_INSERTLOOP);
			return -1;
		}
	}

	// make and insert top item
	pItem = new CLoopItem(CFG_INDEX_LOOP);
	pItem->m_nFile                = nFile;
	pItem->m_pt.x                 = (nLevel + 1);
	pItem->m_pt.y                 = nRow;
	pItem->m_nCondition           = pReport->m_nItem;
	pItem->Element[0].m_nGroup    = pReport->m_nGroup;
	pItem->Element[0].m_nField[0] = pReport->m_nField;
	pItem->m_bHit                 = TRUE;
	POSITION pos = pReport->Insert(pItem);

	// make and insert end item
	m_nFile                = nFile;
	m_pt.x                 = -(nLevel + 1);
	m_pt.y                 = nRow + LOOP_DEFAULT_ROW;
	m_nCondition           = pReport->m_nItem;
	Element[0].m_nGroup    = pReport->m_nGroup;
	Element[0].m_nField[0] = pReport->m_nField;
	m_bHit                 = TRUE;
	pos = pReport->InsertAfter(pos, this);

	CRect rect;
	rect.left   = RPT_PT_SHEETX;
	rect.top    = pReport->GetInsertLine(pItem->m_pt.y);
	rect.right  = ::GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = pReport->GetInsertLine(m_pt.y);
	InvalidateRect(pReport->GetView(), pDC, rect);

	return LOOP_DEFAULT_ROW;
}

void CLoopItem::Draw(CReport* pReport, CDC* pDC)
{
	int nHigh = pReport->GetInsertLine(m_pt.y);

	DrawLines(pReport, pDC, nHigh);
	DrawProperties(pReport, pDC, nHigh);
}

#define	SHEET_RIGHT	(RPT_PT_SHEETX + pReport->m_sizeSheet.cx)
#define	LOOP_LEFT	(SHEET_RIGHT + RPT_LEVEL_X * (2 - abs(m_pt.x)))
#define	LOOP_RIGHT	(SHEET_RIGHT + RPT_LEVEL_X * (3 - abs(m_pt.x)))

static	int	_nTop[2];

void CLoopItem::DrawLines(CReport* pReport, CDC* pDC, int nHigh)
{
	// draw inside horizontal line
   	CPen* pOldPen = NULL;
	CPen pen;
	if (m_bHit)
   		pen.CreatePen(PS_SOLID, RPT_HIT_LINE, RPT_COLOR_BG);
   	else
   		pen.CreatePen(PS_SOLID, RPT_NML_LINE, RPT_COLOR_GRAY);
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(RPT_PT_SHEETX + 1,                           nHigh);
	pDC->LineTo(RPT_PT_SHEETX + pReport->m_sizeSheet.cx - 1, nHigh);

	// draw outside horizontal line
	if (! m_bHit)
	{
		pDC->SelectStockObject(BLACK_PEN);	// free previous pen
		pOldPen = NULL;
	}
	pDC->MoveTo(SHEET_RIGHT, nHigh);
	pDC->LineTo(LOOP_RIGHT,  nHigh);

	// draw outsize vertical line ?
	int nLevel = abs(m_pt.x) - 1;
	if (m_pt.x > 0)
		_nTop[nLevel] = nHigh;
	else
	{
		pDC->MoveTo(LOOP_RIGHT, _nTop[nLevel]);
		pDC->LineTo(LOOP_RIGHT, nHigh);
	}

	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);			// restore pen
}

void CLoopItem::DrawProperties(CReport* pReport, CDC* pDC, int nHigh)
{
	CInfoLoop* pInfo = CInfoLoop::GetField(pReport->GetFileIndex(),
										   Element[0].m_nGroup,
										   Element[0].m_nField[0]);

	CString strTitle(pInfo->m_strName);
	strTitle += " (" + pInfo->Condition[m_nCondition].m_strName + ")";

	CRect rect;
	rect.left   = SHEET_RIGHT + RPT_LEVEL_X * (2 - abs(m_pt.x)) + RPT_HIT_ADJUST;
	rect.top    = _nTop[abs(m_pt.x) - 1]                        + RPT_HIT_ADJUST;
	rect.right  = LOOP_RIGHT                                    - RPT_HIT_ADJUST;
	rect.bottom = nHigh                                         - RPT_HIT_ADJUST;

	pDC->SelectStockObject(ANSI_VAR_FONT);
	COLORREF crOldText = 0;
	if (m_bHit)
		crOldText = pDC->SetTextColor(RPT_COLOR_BG);
	else
		crOldText = pDC->SetTextColor(RPT_COLOR_BLACK);
	pDC->DrawText(strTitle, -1, rect, DT_WORDBREAK | DT_RIGHT);
	pDC->SetBkColor(crOldText);
}

BOOL CLoopItem::MouseMove(CReport* pReport, CDC* pDC, CPoint point)
{
	if (! m_bHit)
		return FALSE;

	if (! m_bResize)
	{
		int nHigh = pReport->GetInsertLine(m_pt.y);
		CRect rect = GetLineRect(pReport, nHigh);
		if (rect.PtInRect(point))
			return TRUE;
		else
			return FALSE;
	}

	int nNew = pReport->GetItemRow(point.y);
	if (nNew == m_pt.y)					// same position
		return FALSE;

	CLoopItem* pPrev = PrevItem(pReport);
	if ((pPrev != NULL && pPrev->m_pt.y > nNew) ||
		(pPrev == NULL && 0 > nNew))
		return FALSE;
	CLoopItem* pNext = NextItem(pReport);
	if ((pNext != NULL && pNext->m_pt.y < nNew) ||
		(pNext == NULL && pReport->m_sizeReport.cy < nNew))
		return FALSE;
	if ((pPrev != NULL && pPrev->m_pt.y == nNew && pPrev->m_pt.x == abs(m_pt.x)) ||
		(pNext != NULL && pNext->m_pt.y == nNew && abs(pNext->m_pt.x) == m_pt.x))
		return FALSE;

	CRect rect;
	rect.left   = RPT_PT_SHEETX;
	rect.top    = pReport->GetInsertLine(min(m_pt.y, nNew)) - RPT_HIT_ADJUST;
	rect.right  = LOOP_RIGHT;
	rect.bottom = pReport->GetInsertLine(max(m_pt.y, nNew)) + RPT_HIT_ADJUST;
	InvalidateRect(pReport->GetView(), pDC, rect);

	rect.left   = SHEET_RIGHT;
	rect.top    = pReport->GetInsertLine(0);
	rect.right  = LOOP_RIGHT;
	rect.bottom = pReport->GetInsertLine(pReport->m_sizeReport.cy);
	InvalidateRect(pReport->GetView(), pDC, rect);

	m_pt.y = nNew;						// set new position

	return FALSE;
}

BOOL CLoopItem::LButtonDown(CReport* pReport, CDC* pDC, CPoint point)
{
	if (! m_bHit)						// not select
		return FALSE;

	int nHigh = pReport->GetInsertLine(m_pt.y);
	CRect rect = GetLineRect(pReport, nHigh);
	if (! rect.PtInRect(point))
		return FALSE;

	return m_bResize = TRUE;
}

void CLoopItem::LButtonUp(CReport* pReport, CDC* pDC, CPoint point)
{
	if (m_bResize)
		m_bResize = FALSE;
	else
		return;

	POSITION pos = pReport->Find(this);
	if (pos == NULL)
		return;
	pReport->RemoveAt(pos);
	pReport->Insert(this);
}

BOOL CLoopItem::LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point)
{
	if (! m_bHit || m_pt.x < 0)			// not select or end loop item
		return FALSE;

	CLoopPropDlg dlg(pReport->GetFileIndex(), this);
	if (dlg.DoModal() != IDOK)
		return TRUE;

	CLoopItem* pPair = PairItem(pReport);
	if (pPair != NULL)
	{
		pPair->m_nCondition = m_nCondition;
		InvalidatePair(pReport, pDC, pPair);
	}
	else
		ASSERT(FALSE);

	pReport->GetDoc()->SetModifiedFlag();

	return TRUE;
}

BOOL CLoopItem::Select(CReport* pReport, CDC* pDC, CPoint point,
	CRect* pUnion, BOOL bHit /*=FALSE*/)
{
	return FALSE;
}

BOOL CLoopItem::Select(CReport* pReport, CDC* pDC, CRect select,
	CRect* pUnion, BOOL bHit /*=FALSE*/)
{
	return FALSE;
}

void CLoopItem::Cancel(CReport* pReport, CDC* pDC)
{
	if (m_bHit)
	{
		// clean up item of previous position
		CLoopItem* pPair = PairItem(pReport);
		ASSERT(pPair != NULL);

		m_bHit = FALSE;
		pPair->m_bHit = FALSE;

		InvalidatePair(pReport, pDC, pPair);
	}
}

int CLoopItem::Remove(CReport* pReport, CDC* pDC, int nRow)
{
	if (m_pt.y == nRow)					// 
	{
		CLoopItem* pPair = PairItem(pReport);
		ASSERT(pPair != NULL);
		if (pPair->m_pt.y == nRow + 1)	// loop item is already minimized
			return PROGRPT_WARNING;
		else
			return PROGRPT_ERROR;
	}
	else if (m_bHit)					// this item is select
	{
		CLoopItem* pPair = PairItem(pReport);
		ASSERT(pPair != NULL);

		InvalidatePair(pReport, pDC, pPair);

		POSITION pos = pReport->Find(pPair);
		ASSERT(pos != NULL);
		pReport->RemoveAt(pos);
		delete pPair;

		return PROGRPT_OK;
	}									// this item is not select
	else
		return PROGRPT_ERROR;
}

BOOL CLoopItem::HitOnRow(CReport* pReport, CDC* pDC, int nRow)
{
	return FALSE;
}

int CLoopItem::GetSelectLevel(CReport* pReport, CDC* pDC, CPoint point)
{
	if (m_pt.x < 0)						// end loop item
		return -1;

	CLoopItem* pPair = PairItem(pReport);
	if (pPair != NULL)
	{
		CRect rect;
		rect.left   = SHEET_RIGHT;
		rect.top    = pReport->GetInsertLine(min(m_pt.y, pPair->m_pt.y)) +
					  RPT_HIT_ADJUST;
		rect.right  = LOOP_RIGHT;
		rect.bottom = pReport->GetInsertLine(max(m_pt.y, pPair->m_pt.y)) -
					  RPT_HIT_ADJUST;
		if (rect.PtInRect(point))
		{
			InvalidatePair(pReport, pDC, pPair);
			m_bHit        = TRUE;
			pPair->m_bHit = TRUE;
			return m_pt.x;
		}
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CLoopItem implementations

CLoopItem* CLoopItem::PrevItem(CReport* pReport)
{
	POSITION pos = pReport->Find(this);
	ASSERT(pos != NULL);

	pReport->GetPrev(pos);
	while (pos != NULL)
	{
		CItem* pPrev = pReport->GetPrev(pos);
		if (pPrev->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			return (CLoopItem*)pPrev;
	}

	return NULL;
}

CLoopItem* CLoopItem::NextItem(CReport* pReport)
{
	POSITION pos = pReport->Find(this);
	ASSERT(pos != NULL);
	
	pReport->GetNext(pos);
	while (pos != NULL)
	{
		CItem* pNext = pReport->GetNext(pos);
		if (pNext->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			return (CLoopItem*)pNext;
	}

	return NULL;
}

CLoopItem* CLoopItem::PairItem(CReport* pReport)
{
	POSITION pos = pReport->Find(this);
	ASSERT(pos != NULL);

	if (m_pt.x > 0)
	{
		pReport->GetNext(pos);
		while (pos != NULL)
		{
			CItem* pNext = pReport->GetNext(pos);
			if (pNext->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
				abs(pNext->m_pt.x) == m_pt.x)
				return (CLoopItem*)pNext;
		}
	}
	else
	{
		pReport->GetPrev(pos);
		while (pos != NULL)
		{
			CItem* pPrev = pReport->GetPrev(pos);
			if (pPrev->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
				pPrev->m_pt.x == abs(m_pt.x))
				return (CLoopItem*)pPrev;
		}
	}
	
	return NULL;
}

int CLoopItem::GetLevel(CReport* pReport, int nRow, CLoopItem **pFind)
{
	int nLevel = 0;
	CItem* pLevel[2];

	POSITION pos = pReport->GetHeadPosition();
	
	while (pos != NULL)
	{
		CItem* pItem = pReport->GetNext(pos);

		if ((pItem->m_nOperation == CFG_INDEX_LOOP && pItem->m_pt.y >  nRow) ||
			(pItem->m_nOperation != CFG_INDEX_LOOP && pItem->m_pt.y >= nRow))
			break;
		else if (pItem->m_nOperation != CFG_INDEX_LOOP)
			continue;

		if (pItem->m_pt.x > 0)
			pLevel[nLevel++] = pItem;
		else
			pLevel[--nLevel] = NULL;
	}

	if (nLevel > 0)									// item found
		*pFind = (CLoopItem*)pLevel[nLevel - 1];	// set item object

	return nLevel;
}

CRect CLoopItem::GetLineRect(CReport* pReport, int nHigh)
{
	CRect rect;

	rect.left   = SHEET_RIGHT;
	rect.top    = nHigh - RPT_HIT_ADJUST;
	rect.right  = LOOP_RIGHT;
	rect.bottom = nHigh + RPT_HIT_ADJUST;

	return rect;
}

void CLoopItem::InvalidatePair(CReport* pReport, CDC* pDC, CLoopItem* pPair)
{
	CRect rect;

	int nMin = pReport->GetInsertLine(min(m_pt.y, pPair->m_pt.y));
	int nMax = pReport->GetInsertLine(max(m_pt.y, pPair->m_pt.y));

	// clean up top line on inside sheet
	rect.left   = RPT_PT_SHEETX;
	rect.top    = nMin - 1;
	rect.right  = SHEET_RIGHT;
	rect.bottom = nMin + RPT_HIT_LINE;
	InvalidateRect(pReport->GetView(), pDC, rect);

	// clean up bottom line on inside sheet
	rect.top    = nMax - 1;
	rect.bottom = nMax + RPT_HIT_LINE;
	InvalidateRect(pReport->GetView(), pDC, rect);

	// clean up rect on outside sheet
	rect.left   = rect.right;
	rect.top    = nMin - 1;
	rect.right  = LOOP_RIGHT + RPT_HIT_LINE;
	rect.bottom = nMax;
	InvalidateRect(pReport->GetView(), pDC, rect);
}


/////////////////////////////////////////////////////////////////////////////
// CPrintItem

IMPLEMENT_SERIAL(CPrintItem, CItem, 0)

/////////////////////////////////////////////////////////////////////////////
// CPrintItem diagnostics

#ifdef _DEBUG
void CPrintItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintItem construction/destruction

CPrintItem::CPrintItem()
{
}

CPrintItem::CPrintItem(const CPrintItem& itemSrc)
	: CItem(itemSrc)
{
}

CPrintItem::CPrintItem(int nOperation)
	: CItem(nOperation)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPrintItem serialization

void CPrintItem::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (WORD)m_nFile;
		ar << (WORD)m_nOperation;
		ar << m_pt;
		ar << (WORD)m_nWidth;
		ar << (WORD)m_nFormat[0] << (WORD)m_nFormat[1];
		ar << (WORD)m_nCondition;

		for (int nIndex = 0; nIndex < ITEM_NUM_ELEMENT; nIndex++)
		{
			ar << (WORD)Element[nIndex].m_nGroup;
			ar << (WORD)Element[nIndex].m_nField[0];
			ar << (WORD)Element[nIndex].m_nField[1];
			ar << (WORD)Element[nIndex].m_nItem;
		}
	}
	else
	{
		WORD wTemp;

		ar >> wTemp;	m_nFile = wTemp;
		ar >> wTemp;	m_nOperation = wTemp;
		ar >> m_pt;
		ar >> wTemp;	m_nWidth = wTemp;
		ar >> wTemp;	m_nFormat[0] = wTemp;
		ar >> wTemp;	m_nFormat[1] = wTemp;
		ar >> wTemp;	m_nCondition = wTemp;

		for (int nIndex = 0; nIndex < ITEM_NUM_ELEMENT; nIndex++)
		{
			ar >> wTemp;	Element[nIndex].m_nGroup = wTemp;
			ar >> wTemp;	Element[nIndex].m_nField[0] = wTemp;
			ar >> wTemp;	Element[nIndex].m_nField[1] = wTemp;
			ar >> wTemp;	Element[nIndex].m_nItem = wTemp;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPrintItem operations

static wchar_t szPrintEntry[] = L"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d";

int CPrintItem::SetItem(int nFile, const wchar_t * pszData)
{
	int nOperation;
	int nData[ITEM_NUM_ELEMENT][4];

	swscanf(pszData, szPrintEntry, &nOperation,
		   &m_pt.y, &m_pt.x, &m_nWidth,
		   &m_nFormat[0], &m_nFormat[1], &m_nCondition,
		   &nData[0][0], &nData[0][1], &nData[0][2], &nData[0][3],
		   &nData[1][0], &nData[1][1], &nData[1][2], &nData[1][3],
		   &nData[2][0], &nData[2][1], &nData[2][2], &nData[2][3]);

	ASSERT(m_nOperation + 1 == nOperation);

	if (nData[0][0] == 0)				// End of Report
		return m_pt.y;

	m_nFile = nFile;
	int breturn = -1;
	for (int nIndex = 0; nIndex < ITEM_NUM_ELEMENT; nIndex++)
	{
		if (nData[nIndex][0] == 0)
			break;

		// convert from actual data to index data
		CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
		ASSERT(pConfig != NULL);
		Element[nIndex].m_nGroup    = pConfig->GetGroupIndex(m_nOperation, nData[nIndex][0]);
		Element[nIndex].m_nField[0] = CInfoPrint::GetMajorIndex(nFile, Element[nIndex].m_nGroup, nData[nIndex][1]);
		CInfoPrint* pInfo = CInfoPrint::GetField(nFile, Element[nIndex].m_nGroup, Element[nIndex].m_nField[0]);
		Element[nIndex].m_nField[1] = nData[nIndex][2] - pInfo->m_nMinor;

		for (int nIndex2 = 0; nIndex2 < LOOP_NUM_COND; nIndex2++){
			if (pInfo->Item[nIndex2].m_nNumber == nData[nIndex][3]){
				Element[nIndex].m_nItem = nIndex2;
				breturn = 0;
				break;
			}
		}
	}
	
	return breturn;
}

void CPrintItem::GetItem(int nFile, wchar_t * pszData, int* pnKey /*=NULL*/)
{
	int nData[ITEM_NUM_ELEMENT][4];

	for (int nIndex = 0; nIndex < ITEM_NUM_ELEMENT; nIndex++)
	{
		int nBias = 0;
		CInfoPrint* pInfo = CInfoPrint::GetField(nFile,
												 Element[nIndex].m_nGroup,
												 Element[nIndex].m_nField[0],
												 &nBias);
		if (pInfo != NULL)
		{
			nData[nIndex][0] = 1 + nBias + Element[nIndex].m_nGroup;
			nData[nIndex][1] = pInfo->m_nMajor;
			nData[nIndex][2] = pInfo->m_nMinor + Element[nIndex].m_nField[1];
			nData[nIndex][3] = pInfo->Item[Element[nIndex].m_nItem].m_nNumber;
		}
		else
		{
			nData[nIndex][0] = 0;
			nData[nIndex][1] = 0;
			nData[nIndex][2] = 0;
			nData[nIndex][3] = 0;
		}
	}
	
	wsprintf(pszData, szPrintEntry,
			 1 + CFG_INDEX_PRINT, m_pt.y, m_pt.x, m_nWidth,
			 m_nFormat[0], m_nFormat[1], m_nCondition,
			 nData[0][0], nData[0][1], nData[0][2], nData[0][3],
			 nData[1][0], nData[1][1], nData[1][2], nData[1][3],
			 nData[2][0], nData[2][1], nData[2][2], nData[2][3]);
}

int CPrintItem::Insert(CReport* pReport, CDC* pDC, int nFile, int nRow)
{
	CInfoPrint* pInfo = CInfoPrint::GetField(pReport->GetFileIndex(),
											 pReport->m_nGroup,
											 pReport->m_nField);

	int nMinor;
	if (pInfo->m_nMinorCount == 1)
		nMinor = 0;
	else
		nMinor = (pReport->m_nItem >= 0) ? (pReport->m_nItem) : (0);

	int nColumn = 0;
	for (int nCount = 0; nCount < PRINT_NUM_ITEM; nCount++)
	{
		CPrintItem* pItem;
		if (pInfo->Item[nCount + 1].m_nNumber == _FREE)	// last item
			pItem = this;
		else
			pItem = new CPrintItem(m_nOperation);

		pItem->m_nFile                = nFile;
		pItem->m_pt.x                 = 1 + nColumn;
		pItem->m_pt.y                 = 1 + nRow;
//		pItem->m_pt.y                 = 1 + nRow + nCount;
		pItem->m_nWidth               = min(pReport->m_sizeReport.cx,
											pInfo->Item[nCount].m_nWidth);
		pItem->m_nFormat[0]           = pInfo->Item[nCount].m_nFormat;
		pItem->m_nFormat[1]           = (pInfo->m_nFlags & PRINT_F_RIGHT) ?
										PITEM_FORMAT_RIGHT : PITEM_FORMAT_LEFT;
		pItem->m_nCondition           = 0;
		pItem->Element[0].m_nGroup    = pReport->m_nGroup;
		pItem->Element[0].m_nField[0] = pReport->m_nField;
		pItem->Element[0].m_nField[1] = nMinor;
		pItem->Element[0].m_nItem     = nCount;
		pItem->m_bHit                 = TRUE;

		pReport->Insert(pItem);

		if (pInfo->Item[nCount + 1].m_nNumber != _FREE)
			nColumn += pItem->m_nWidth + 1;
		else												// end of index
			break;
	}

	Invalidate(pReport, pDC);

	return (pReport->m_sizeReport.cy == nRow) ? (1) : (0);
}

void CPrintItem::Draw(CReport* pReport, CDC* pDC)
{
	if (! pReport->GetView()->GetPrintState())	// hide print field
		return;

	ASSERT(m_nWidth > 0);

	wchar_t szData[64];
	int nLength = FormatItemString(pReport, szData);
	CRect wide = GetRect(pReport, m_pt, m_nWidth);

	// draw border line
	CBrush* pOldBrush = NULL;
   	CBrush brush(RPT_COLOR_BG);
    if (m_bHit)
		pOldBrush = pDC->SelectObject(&brush);
	else if (m_nFormat[1] & PITEM_FORMAT_WIDE)	// double wide
		pDC->SelectStockObject(WHITE_BRUSH);
	else
		pDC->SelectStockObject(NULL_BRUSH);
	pDC->SelectStockObject(BLACK_PEN);
	pDC->Rectangle(&wide);
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);		// restore brush

	// draw item
	pDC->SelectStockObject(OEM_FIXED_FONT);
	COLORREF crOldText = 0;
	if (m_bHit)
		crOldText = pDC->SetTextColor(RPT_COLOR_WHITE);
	else
		crOldText = pDC->SetTextColor(RPT_COLOR_BLACK);
	for (int nCount = 0; nCount < nLength; nCount++)
	{
		pDC->TextOut(wide.left + 1 + pReport->m_sizeItem.cx * nCount,
					 wide.top  + 1,
					 szData + nCount, 1);
	}
	pDC->SetBkColor(crOldText);

	if (m_nFormat[1] & PITEM_FORMAT_WIDE)	// double wide
	{
		CRect rect = GetRect(pReport, m_pt, nLength);
		pDC->StretchBlt(wide.left + 1, wide.top,
						wide.right - wide.left - 2, wide.bottom - wide.top,
						pDC,
						rect.left + 1, rect.top,
						rect.right - rect.left - 2, rect.bottom - rect.top,
						SRCCOPY);
	}
}

int CPrintItem::FormatItemString(CReport* pReport, wchar_t * pszData)
{
	CInfoConfig* pConfig;
	CFG_OPEGROUP* pOpeGroup;
	int nFill;
	CString strData;
	CProgrptDoc* pDoc = pReport->GetDoc();
	INFO_FORMAT* pFormat = ptheInfo->GetFormat(m_nFormat[0]);

	switch (pFormat->m_nNumber)
	{
	case 0:								// date/time format
		nFill = L' ';
		strData  = (pDoc->m_bDate) ? L"DD/MM" : L"MM/DD";
		strData += " ";
		strData += (pDoc->m_bTime) ? L"HH:MM" : L"MM:HHAM";
		break;
	case 1:								// string format
		nFill = L'?';
		pConfig = ptheInfo->GetConfig(pReport->GetFileIndex());
		if (pConfig == NULL)
			break;
		pOpeGroup = pConfig->GetGroup(CFG_INDEX_PRINT, Element[0].m_nGroup);
		if (pOpeGroup != NULL && pOpeGroup->m_nGroup == CFG_INDEX_MNEMO)
		{
			int nIndex1 = Element[0].m_nField[0];
			int nIndex2 = Element[0].m_nField[1];
			if (! pReport->GetDoc()->m_strMnemo[nIndex1].IsEmpty())
			{
				POSITION pos =
					pReport->GetDoc()->m_strMnemo[nIndex1].FindIndex(nIndex2);
				if (pos != NULL)
					strData = pReport->GetDoc()->m_strMnemo[nIndex1].GetAt(pos);
				nFill = ' ';
			}
		}
		break;
	default:							// each number format
		nFill = L' ';
		strData = pFormat->m_strName;
		break;
	}

	int nLength = 0;
	int nWidth = (m_nFormat[1] & PITEM_FORMAT_WIDE) ? (m_nWidth / 2) : (m_nWidth);
	int nRest = nWidth;
	int nString = strData.GetLength();
	if (m_nFormat[1] & PITEM_FORMAT_LEFT)
	{
		nLength = min(nRest, nString);
		wcsncpy (pszData, strData, nLength);
		nRest -= nLength;
	}
	else if (nString < nRest)
	{
		nLength = nRest - nString;
		memset(pszData, nFill, nLength);
		nRest -= nLength;
	}

	if (m_nFormat[1] & PITEM_FORMAT_LEFT)
		memset(pszData + nLength, nFill, nRest);
	else
		wcsncpy(pszData + nLength, strData.Right(nRest), nRest);
	
	*(pszData + nWidth) = L'\0';

	return wcslen(pszData);
}

static	int	_nMove;

BOOL CPrintItem::MouseMove(CReport* pReport, CDC* pDC, CPoint point)
{
	if (! m_bHit)
		return FALSE;

	CPoint ptNew = GetPoint(pReport, point.x - _nMove, point.y);
	if (ptNew.x == -1 || ptNew.y == -1)
		return FALSE;

	ptNew.x++;							// adjust one-based
	ptNew.y++;							// adjust one-based
	
	if (ptNew.x + m_nWidth - 1 > pReport->m_sizeReport.cx ||
		ptNew.y				   > pReport->m_sizeReport.cy)
		return FALSE;

	// clean up item of previous position
//	InvalidateRow(pReport, pDC);
	CRect rect = GetRect(pReport, m_pt, m_nWidth);
	InvalidateRect(pReport->GetView(), pDC, rect);

	m_pt = ptNew;						// set new position

	// draw item of new position
	rect = GetRect(pReport, m_pt, m_nWidth);
	InvalidateRect(pReport->GetView(), pDC, rect);

	return TRUE;
}

BOOL CPrintItem::LButtonDown(CReport* pReport, CDC* pDC, CPoint point)
{
	CRect rect = GetRect(pReport, m_pt, m_nWidth);
	if (! rect.PtInRect(point))
		return FALSE;

	m_bHit = TRUE;
	_nMove = point.x - rect.left;

	InvalidateRect(pReport->GetView(), pDC, rect);

	return TRUE;
}

void CPrintItem::LButtonUp(CReport* pReport, CDC* pDC, CPoint point)
{
	if (! m_bHit)
		return;

	POSITION pos = pReport->Find(this);
	if (pos == NULL)
		return;
	pReport->RemoveAt(pos);
	pReport->Insert(this);
}

BOOL CPrintItem::LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point)
{
	CRect rect = GetRect(pReport, m_pt, m_nWidth);
	if (! rect.PtInRect(point))
		return FALSE;

	CPrintItem temp(*this);
	CPrintPropDlg dlg(pReport->GetFileIndex(), pReport->m_sizeReport.cx, &temp);
	if (dlg.DoModal() == IDOK)
		*this = temp;
	else
		return TRUE;

	CRect rectNew = GetRect(pReport, m_pt, m_nWidth);
	if (rect.right < rectNew.right)
		InvalidateRect(pReport->GetView(), pDC, rectNew);
	else
		InvalidateRect(pReport->GetView(), pDC, rect);

	pReport->GetDoc()->SetModifiedFlag();

	return TRUE;
}

BOOL CPrintItem::Select(CReport* pReport, CDC* pDC, CPoint point,
	CRect* pUnion, BOOL bHit /*=FALSE*/)
{
	CRect rect = GetRect(pReport, m_pt, m_nWidth);

	if (bHit || rect.PtInRect(point))
	{
		m_bHit = TRUE;
		InvalidateRect(pReport->GetView(), pDC, rect);

		pUnion->left   = (pUnion->left == -1) ?
						 (rect.left) : (min(rect.left, pUnion->left));
		pUnion->top    = (pUnion->top == -1) ?
						 (rect.top) : (min(rect.top, pUnion->top));
		pUnion->right  = (pUnion->right == -1) ?
						 (rect.right) : (max(rect.right, pUnion->right));
		pUnion->bottom = (pUnion->bottom == -1) ?
						 (rect.bottom) : (max(rect.bottom, pUnion->bottom));
	}

	return m_bHit;
}

BOOL CPrintItem::Select(CReport* pReport, CDC* pDC, CRect select,
	CRect* pUnion, BOOL bHit /*=FALSE*/)
{
	CRect rect = GetRect(pReport, m_pt, m_nWidth);

	if (bHit || (select.PtInRect(rect.TopLeft()) &&
				 select.PtInRect(rect.BottomRight())))
	{
		m_bHit = TRUE;
		InvalidateRect(pReport->GetView(), pDC, rect);

		pUnion->left   = (pUnion->left == -1) ?
						 (rect.left) : (min(rect.left, pUnion->left));
		pUnion->top    = (pUnion->top == -1) ?
						 (rect.top) : (min(rect.top, pUnion->top));
		pUnion->right  = (pUnion->right == -1) ?
						 (rect.right) : (max(rect.right, pUnion->right));
		pUnion->bottom = (pUnion->bottom == -1) ?
						 (rect.bottom) : (max(rect.bottom, pUnion->bottom));
	}
	
	return m_bHit;
}

void CPrintItem::Cancel(CReport* pReport, CDC* pDC)
{
	if (m_bHit)
	{
		m_bHit = FALSE;

		// clean up item of previous position
		CRect rect = GetRect(pReport, m_pt, m_nWidth);
		InvalidateRect(pReport->GetView(), pDC, rect);
	}
}

BOOL CPrintItem::Remove(CReport* pReport, CDC* pDC, int nRow)
{
	if (m_bHit || m_pt.y == 1 + nRow)	// this item is select
	{
		CRect rect = GetRect(pReport, m_pt, m_nWidth);
		InvalidateRect(pReport->GetView(), pDC, rect);
		return PROGRPT_OK;
	}
	else								// this item is not select
		return PROGRPT_ERROR;
}

BOOL CPrintItem::HitOnRow(CReport* pReport, CDC* pDC, int nRow)
{
	if (m_pt.y == nRow)
		m_bHit = TRUE;
	else
		m_bHit = FALSE;

	return m_bHit;
}

/////////////////////////////////////////////////////////////////////////////

CRect CPrintItem::GetRect(CReport* pReport, CPoint& point, int nLength)
{
	CPoint pt;
	pt.x = RPT_PT_SHEETX + RPT_SHEET_LEFT +
		   (pReport->m_sizeItem.cx)				  * (point.x - 1);
	pt.y = RPT_PT_SHEETYY + RPT_SHEET_TOP  +
		   (pReport->m_sizeItem.cy + RPT_SPACE_Y) * (point.y - 1);

	CRect rect;
	rect.left   = pt.x;
	rect.top    = pt.y;
	rect.right  = pt.x + pReport->m_sizeItem.cx * nLength - 1;
	rect.bottom = pt.y + pReport->m_sizeItem.cy;

	return rect;
}

CPoint CPrintItem::GetPoint(CReport* pReport, int nX, int nY)
{
	CPoint point(-1, -1);				// outside sheet

	if (RPT_PT_SHEETX < nX && nX < RPT_PT_SHEETX + pReport->m_sizeSheet.cx)
		point.x = (nX - RPT_PT_SHEETX - RPT_SHEET_LEFT / 2) /
				  (pReport->m_sizeItem.cx);

	if (RPT_PT_SHEETYY < nY && nY < RPT_PT_SHEETYY + pReport->m_sizeSheet.cy)
		point.y = (nY - RPT_PT_SHEETYY - RPT_SHEET_TOP / 2) /
				  (pReport->m_sizeItem.cy + RPT_SPACE_Y);

	return point;
}

void CPrintItem::Invalidate(CReport* pReport, CDC* pDC)
{
	CRect rect;
	rect.left   = 0;
	rect.top    = RPT_PT_SHEETYY;
	rect.right  = RPT_PT_SHEETX + pReport->m_sizeSheet.cx + RPT_LEVEL_X + 1;
	rect.bottom = RPT_PT_SHEETYY + pReport->m_sizeSheet.cy + 1;

	pReport->GetView()->OnPrepareDC(pDC);
	pReport->GetView()->InvalidateRect(&rect, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CMatheItem

IMPLEMENT_SERIAL(CMatheItem, CPrintItem, 0)

/////////////////////////////////////////////////////////////////////////////
// CMatheItem diagnostics

#ifdef _DEBUG
void CMatheItem::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatheItem construction/destruction

CMatheItem::CMatheItem()
	: CPrintItem()
{
}

CMatheItem::CMatheItem(const CMatheItem& itemSrc)
	: CPrintItem(itemSrc)
{
	m_lData = itemSrc.m_lData;
}

CMatheItem::CMatheItem(int nOperation)
	: CPrintItem(nOperation)
{
	m_lData = 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CMatheItem serialization

void CMatheItem::Serialize(CArchive& ar)
{
	CPrintItem::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_lData;
	}
	else
	{
		ar >> m_lData;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMatheItem operations

static wchar_t szMathePre[]   = L"%d,%d,%d,%d,%d";
static wchar_t szMatheEntry[] = L"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d";
static wchar_t szMatheValue[] = L"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld,%d,%d,%d,%d";

int CMatheItem::SetItem(int nFile, const wchar_t * pszData)
{
	int nOperation;
	int nData[ITEM_NUM_ELEMENT][4];

	swscanf(pszData, szMathePre,
		   &nOperation, &m_pt.y, &m_pt.x, &m_nWidth, &m_nFormat[0]);

	if (m_nFormat[0] == 1)				// format type of one value
		return SetItemValue(nFile, pszData);
	
	swscanf(pszData, szPrintEntry,
		   &nOperation, &m_pt.y, &m_pt.x, &m_nWidth,
		   &m_nFormat[0], &m_nFormat[1], &m_nCondition,
		   &nData[0][0], &nData[0][1], &nData[0][2], &nData[0][3],
		   &nData[1][0], &nData[1][1], &nData[1][2], &nData[1][3],
		   &nData[2][0], &nData[2][1], &nData[2][2], &nData[2][3]);

	ASSERT(m_nOperation + 1 == nOperation);

	m_nFile = nFile;
	
	// convert from actual data to index data
	CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
	ASSERT(pConfig != NULL);
	Element[0].m_nGroup    = pConfig->GetGroupIndex(m_nOperation, nData[0][0]);
	Element[0].m_nField[0] = CInfoMathe::GetMajorIndex(nFile, Element[0].m_nGroup, nData[0][1]);
	CInfoMathe* pMathe = CInfoMathe::GetField(nFile, Element[0].m_nGroup, Element[0].m_nField[0]);
	Element[0].m_nField[1] = nData[0][2] - pMathe->m_nMinor;
	Element[0].m_nItem     = 0;

	int breturn = -1;
	for (int nIndex = 1; nIndex < ITEM_NUM_ELEMENT; nIndex++)
	{
		if (nData[nIndex][0] == 0)
			break;

		CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
		ASSERT(pConfig != NULL);
		Element[nIndex].m_nGroup    = pConfig->GetGroupIndex(CFG_INDEX_PRINT, nData[nIndex][0]);
		Element[nIndex].m_nField[0] = CInfoPrint::GetMajorIndex(nFile, Element[nIndex].m_nGroup, nData[nIndex][1]);
//		Element[nIndex].m_nField[0] = nData[nIndex][1] - 1;
		CInfoPrint* pInfo = CInfoPrint::GetField(nFile, Element[nIndex].m_nGroup, Element[nIndex].m_nField[0]);
		Element[nIndex].m_nField[1] = nData[nIndex][2] - pInfo->m_nMinor;

		for (int nIndex2 = 0; nIndex2 < LOOP_NUM_COND; nIndex2++){
			if (pInfo->Item[nIndex2].m_nNumber == nData[nIndex][3]){
				Element[nIndex].m_nItem = nIndex2;
				breturn = 0;	//item found
				break;
			}
		}
	}	
	return breturn;
}

int CMatheItem::SetItemValue(int nFile, const wchar_t * pszData)
{
	int nOperation;
	int nData[2][4];

	swscanf(pszData, szMatheValue, &nOperation,
		   &m_pt.y, &m_pt.x, &m_nWidth,
		   &m_nFormat[0], &m_nFormat[1], &m_nCondition,
		   &nData[0][0], &nData[0][1], &nData[0][2], &nData[0][3],
		   &m_lData,
		   &nData[1][0], &nData[1][1], &nData[1][2], &nData[1][3]);

	ASSERT(m_nOperation + 1 == nOperation);

	m_nFile = nFile;
	
	// convert from actual data to index data
	CInfoConfig* pConfig = ptheInfo->GetConfig(nFile);
	ASSERT(pConfig != NULL);
	Element[0].m_nGroup    = pConfig->GetGroupIndex(m_nOperation, nData[0][0]);
	Element[0].m_nField[0] = CInfoMathe::GetMajorIndex(nFile, Element[0].m_nGroup, nData[0][1]);
	CInfoMathe* pMathe = CInfoMathe::GetField(nFile, Element[0].m_nGroup, Element[0].m_nField[0]);
	Element[0].m_nField[1] = nData[0][2] - pMathe->m_nMinor;
	Element[0].m_nItem     = Element[0].m_nField[0];
	
	int breturn = -1;
	if (nData[1][0] != 0)
	{
		Element[2].m_nGroup    = pConfig->GetGroupIndex(CFG_INDEX_PRINT, nData[1][0]);
		Element[2].m_nField[0] = CInfoPrint::GetMajorIndex(nFile, Element[2].m_nGroup, nData[1][1]);
		CInfoPrint* pInfo = CInfoPrint::GetField(nFile, Element[2].m_nGroup, Element[2].m_nField[0]);
		if (pInfo == NULL)				// bug fix - 01.00.05
			return -1;
			
		Element[2].m_nField[1] = nData[1][2] - pInfo->m_nMinor;

		for (int nIndex2 = 0; nIndex2 < LOOP_NUM_COND; nIndex2++){
			if (pInfo->Item[nIndex2].m_nNumber == nData[1][3]){
				Element[2].m_nItem = nIndex2;
				breturn = 0;	//item found
				break;
			}
		}		
	}	
	return breturn;
}

void CMatheItem::GetItem(int nFile, wchar_t * pszData, int* pnKey /*=NULL*/)
{
	if (m_nFormat[0] == 1)				// format type of one value
	{
		GetItemValue(nFile, pszData);
		return;
	}

	int nData[ITEM_NUM_ELEMENT][4], nBias = 0;

	// convert from index data to actual data
	CInfoMathe* pMathe = CInfoMathe::GetField(nFile,
											  Element[0].m_nGroup,
											  Element[0].m_nField[0],
											  &nBias);
	if (pMathe != NULL)
	{
		nData[0][0] = 1 + nBias + Element[0].m_nGroup;
		nData[0][1] = pMathe->m_nMajor;
		nData[0][2] = pMathe->m_nMinor + Element[0].m_nField[1];
		nData[0][3] = 0;
	}
	else
	{
		nData[0][0] = 0;
		nData[0][1] = 0;
		nData[0][2] = 0;
		nData[0][3] = 0;
	}
	
	for (int nIndex = 1; nIndex < ITEM_NUM_ELEMENT; nIndex++)
	{
		CInfoPrint* pInfo = CInfoPrint::GetField(nFile,
												 Element[nIndex].m_nGroup,
												 Element[nIndex].m_nField[0],
												 &nBias);
		if (pInfo != NULL)
		{
			nData[nIndex][0] = 1 + nBias + Element[nIndex].m_nGroup;
			nData[nIndex][1] = pInfo->m_nMajor;
//			nData[nIndex][1] = 1 + Element[nIndex].m_nField[0];
			nData[nIndex][2] = pInfo->m_nMinor + Element[nIndex].m_nField[1];
			nData[nIndex][3] = pInfo->Item[Element[nIndex].m_nItem].m_nNumber;
		}
		else
		{
			nData[nIndex][0] = 0;
			nData[nIndex][1] = 0;
			nData[nIndex][2] = 0;
			nData[nIndex][3] = 0;
		}
	}
	
	wsprintf(pszData, szMatheEntry,
			 1 + CFG_INDEX_MATHE, m_pt.y, m_pt.x, m_nWidth,
			 m_nFormat[0], m_nFormat[1], m_nCondition,
			 nData[0][0], nData[0][1], nData[0][2], nData[0][3],
			 nData[1][0], nData[1][1], nData[1][2], nData[1][3],
			 nData[2][0], nData[2][1], nData[2][2], nData[2][3]);
}
	
void CMatheItem::GetItemValue(int nFile, wchar_t * pszData)
{
	int nData[2][4], nBias = 0;

	CInfoMathe* pMathe = CInfoMathe::GetField(nFile,
											  Element[0].m_nGroup,
											  Element[0].m_nField[0],
											  &nBias);
	if (pMathe != NULL)
	{
		nData[0][0] = 1 + nBias + Element[0].m_nGroup;
		nData[0][1] = pMathe->m_nMajor;
		nData[0][2] = pMathe->m_nMinor + Element[0].m_nField[1];
		nData[0][3] = 0;
	}
	else
	{
		nData[0][0] = 0;
		nData[0][1] = 0;
		nData[0][2] = 0;
		nData[0][3] = 0;
	}
	
	CInfoPrint* pPrint = CInfoPrint::GetField(nFile,
											  Element[2].m_nGroup,
											  Element[2].m_nField[0],
											  &nBias);
	if (pPrint != NULL)
	{
		nData[1][0] = 1 + nBias + Element[2].m_nGroup;
		nData[1][1] = pPrint->m_nMajor;
//		nData[1][1] = 1 + Element[2].m_nField[0];
		nData[1][2] = pPrint->m_nMinor + Element[2].m_nField[1];
		nData[1][3] = pPrint->Item[Element[2].m_nItem].m_nNumber;
	}
	else
	{
		nData[1][0] = 0;
		nData[1][1] = 0;
		nData[1][2] = 0;
		nData[1][3] = 0;
	}
	
	wsprintf(pszData, szMatheValue,
			 1 + CFG_INDEX_MATHE, m_pt.y, m_pt.x, m_nWidth,
			 m_nFormat[0], m_nFormat[1], m_nCondition,
			 nData[0][0], nData[0][1], nData[0][2], nData[0][3],
			 m_lData,
			 nData[1][0], nData[1][1], nData[1][2], nData[1][3]);
}

int CMatheItem::Insert(CReport* pReport, CDC* pDC, int nFile, int nRow)
{
	m_nFile                = nFile;
	m_pt.x                 = 1;
	m_pt.y                 = 1 + nRow;
	m_nWidth               = 1;
	m_nFormat[1]           = 0;
	m_nCondition           = 0;
	Element[0].m_nGroup    = pReport->m_nGroup;
	Element[0].m_nField[0] = pReport->m_nField;
	Element[0].m_nField[1] = pReport->m_nItem;
	Element[0].m_nItem     = 0;
	m_bHit                 = TRUE;

	CMathePropDlg dlg(pReport->GetFileIndex(), this);
	if (dlg.DoModal() != IDOK)
		return -1;

	pReport->Insert(this);

	Invalidate(pReport, pDC);

	return (pReport->m_sizeReport.cy == nRow) ? (1) : (0);
}

void CMatheItem::Draw(CReport* pReport, CDC* pDC)
{
	if (! pReport->GetView()->GetMatheState())	// hide mathe field
		return;

	ASSERT(m_nWidth > 0);

	wchar_t szData[64];
	int nLength = FormatItemString(pReport, szData);
	CRect rect = GetRect(pReport, m_pt, nLength);

	// draw border line
	CBrush* pOldBrush = NULL;
   	CBrush brush(RPT_COLOR_BG);
    if (m_bHit)
		pOldBrush = pDC->SelectObject(&brush);
    else
		pDC->SelectStockObject(NULL_BRUSH);
	CPen* pOldPen = NULL;
	CPen pen(PS_SOLID, RPT_NML_LINE, RPT_COLOR_LRED);
	pOldPen = pDC->SelectObject(&pen);
	pDC->Rectangle(&rect);
	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);			// restore pen
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);		// restore brush

	// draw item item
	pDC->SelectStockObject(OEM_FIXED_FONT);
	COLORREF crOldText = 0;
	if (m_bHit)
		crOldText = pDC->SetTextColor(RPT_COLOR_WHITE);
	else
		crOldText = pDC->SetTextColor(RPT_COLOR_BLACK);
	for (int nCount = 0; nCount < nLength; nCount++)
	{
		pDC->TextOut(rect.left + 1 + pReport->m_sizeItem.cx * nCount,
					 rect.top  + 1,
					 szData + nCount, 1);
	}
	pDC->SetBkColor(crOldText);
}

int CMatheItem::FormatItemString(CReport* pReport, wchar_t * pszData)
{
	if (Element[0].m_nField[0] == 0)		// total field
		*pszData = L'T';
	else if (Element[0].m_nField[0] == 1)	// counter field
		*pszData = L'C';
	else									// unkown
		*pszData = L' ';

	*(pszData + m_nWidth) = L'\0';

	return wcslen(pszData);
}

BOOL CMatheItem::LButtonDblClk(CReport* pReport, CDC* pDC, CPoint point)
{
	CRect rect = GetRect(pReport, m_pt, m_nWidth);
	if (! rect.PtInRect(point))
		return FALSE;

	CMathePropDlg dlg(pReport->GetFileIndex(), this);
	if (dlg.DoModal() == IDOK)
		pReport->GetDoc()->SetModifiedFlag();

	return TRUE;
}
