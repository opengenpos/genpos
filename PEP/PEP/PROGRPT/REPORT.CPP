/////////////////////////////////////////////////////////////////////////////
// report.cpp : implementation of the CReport class
//
// Copyright (C) 1996-1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date       Version  Author       Comments
// =========  =======  ===========  ========================================
// Mar-01-96           O.Nakada     initial
// Oct-12-98           O.Nakada     HPUS V3.3
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "progrpt.h"

#include "progrdoc.h"
#include "report.h"
#include "reportvw.h"
#include "clpboard.h"

/////////////////////////////////////////////////////////////////////////////

#define	CLIPBOARD_MAX_SIZE		5120L

/////////////////////////////////////////////////////////////////////////////
// CReport

IMPLEMENT_SERIAL(CReport, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CReport diagnostics

#ifdef _DEBUG
void CReport::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CReport construction/destruction

CReport::CReport()
{
	m_pDoc	 = NULL;
	m_pView  = NULL;
	m_nMagic = -1;
}

CReport::CReport(CProgrptDoc *pDoc)
{
	ASSERT(pDoc != NULL);

	m_pDoc	 = pDoc;
	m_pView  = NULL;
	m_nMagic = m_pDoc->GetUnique();	// unique number
}

CReport::CReport(CProgrptDoc *pDoc,
	int nFile, int nDevice, CString& strTitle)
{
	ASSERT(pDoc != NULL);
	ASSERT(nFile >= 0);
	ASSERT(nDevice > 0);

	m_pDoc	   = pDoc;
	m_pView    = NULL;
	m_nMagic   = m_pDoc->GetUnique();	// unique number
	m_nFile    = nFile;					// index of file type
	m_nDevice  = nDevice;				// device number
	m_strTitle = strTitle;				// report title

	Init(RPT_DEFAULT_ROW);
}

CReport::~CReport()
{
	m_SelectObj.RemoveAll();

	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
		delete m_ItemObj.GetNext(pos);
	m_ItemObj.RemoveAll();
}

void CReport::Init(int nRow)
{
	ASSERT(nRow > 0);

	// WARNING: There is some kind of store trampler or type mismatch involving the variable tm.
	//          Running in Debug mode we get a stack check error involving tm. If we dimension by two and then
	//          watch we can see that the last couple of members of tm[1] are being set to non-zero values.
	//          This indicates some kind of a type size problem or something, probably due to being ported
	//          from Visual Studio 6.1 to Visual Studio 2005 and changes in compiler options.
	//          It is as if the GetTextMetrics() is using the wide character version of TEXTMETRIC and
	//          the compiler is generating the narrow character version of TEXTMETRIC.
	//             Richard Chambers, 11/12/2018
	TEXTMETRICW tm[2] = {0};
	{
		CDC dc;
		dc.Attach(::GetDC(NULL));
		dc.SelectStockObject(OEM_FIXED_FONT);
		dc.GetTextMetrics(&tm[0]);
		::ReleaseDC(NULL, dc.Detach());
	}

	INFO_DEVICE* pDevice = ptheInfo->GetDevice(m_nDevice);
	ASSERT(pDevice != NULL);

	m_sizeReport.cx = pDevice->m_nWidth;	// column of report
	m_sizeReport.cy = nRow;					// row of report
	m_sizeFont.cx   = tm[0].tmAveCharWidth;
	m_sizeFont.cy   = tm[0].tmHeight + tm[0].tmExternalLeading;
	m_sizeItem.cx   = 1 + m_sizeFont.cx + 1;
	m_sizeItem.cy   =     m_sizeFont.cy + 1;

	m_bCapture = FALSE;
	m_nHit     = RPT_CANCEL;
	m_ptDown.x = -1;
	m_ptDown.y = -1;
	m_rectSelect.SetRect(-1, -1, -1, -1);

	Action(none);						// cancel all action
}

/////////////////////////////////////////////////////////////////////////////
// CReport serialization

void CReport::Serialize(CArchive& ar)
{
	m_ItemObj.Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CReport operations

static wchar_t szReport[] = L"Report%d";
static wchar_t szFile[]	  = L"File";
static wchar_t szDevice[] = L"Device";
static wchar_t szTitle[]  = L"Title";
static wchar_t szKey[]    = L"Key";
static wchar_t szItem[]   = L"Item%d";

int CReport::OpenDocument(int nIndex, const wchar_t * pszPathName)
{
	wchar_t szSection[32];
	wchar_t szEntry[32];
	wchar_t szData[128];
	int  nFile;

	wsprintf(szSection, szReport, nIndex);

	if (::GetPrivateProfileString(szSection, szFile, L"", szData, sizeof(szData), pszPathName) > 0)
		nFile = _wtoi(szData);
	else
		return 0;

	m_nFile = -1;							// initialize
		
	// convert from file id to file index

	int			nCount = 0;
	POSITION	pos = ptheInfo->m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)ptheInfo->m_File.GetNext(pos);
		if (pFile->m_strOption.IsEmpty())	// normal file ?
		{
			if (pFile->m_nNumber == nFile)
			{
				m_nFile = nCount;			// file index
				break;
			}
			else
				nCount++;
		}
	}
	
	if (m_nFile == -1)						// unsupported or old version file ?
		return -1;

	if (::GetPrivateProfileString(szSection, szDevice, L"", szData, sizeof(szData), pszPathName) > 0)
		m_nDevice = _wtoi(szData);
	else
		return 0;

	if (::GetPrivateProfileString(szSection, szTitle, L"", szData, sizeof(szData), pszPathName) > 0)
		m_strTitle = szData;

	int nRow;
	BOOL	bError = FALSE;
	CLoopItem *pLoop;
	CPrintItem *pPrint;
	CMatheItem *pMathe;

	for (nCount = 1; ; nCount++)
	{
		wsprintf(szEntry, szItem, nCount);

		if (::GetPrivateProfileString(szSection, szEntry, L"", szData, sizeof(szData), pszPathName) <= 0)
			break;

		ASSERT(szData[1] == L',');
		
		switch (szData[0] - L'1')
		{
		case CFG_INDEX_LOOP:
			pLoop = new CLoopItem(CFG_INDEX_LOOP);
			if ((nRow = pLoop->SetItem(m_nFile, szData)) == 0)
				m_ItemObj.AddTail(pLoop);
			else
				delete pLoop;
			break;
		case CFG_INDEX_PRINT:
			pPrint = new CPrintItem(CFG_INDEX_PRINT);
			if ((nRow = pPrint->SetItem(m_nFile, szData)) == 0)
				m_ItemObj.AddTail(pPrint);
			else
				delete pPrint;
			break;
		case CFG_INDEX_MATHE:
			pMathe = new CMatheItem(CFG_INDEX_MATHE);
			if ((nRow = pMathe->SetItem(m_nFile, szData)) == 0)
				m_ItemObj.AddTail(pMathe);
			else
				delete pMathe;
			break;
		default:
			ASSERT(FALSE);
		}
		
		if (nRow > 0)					// end of report
			Init(nRow);
		else if (nRow < 0)
			bError = TRUE;
	}

	if (bError)
		AfxMessageBox(IDS_ERRMSG_FILEFILED);
		
	return 1;
}

BOOL CReport::SaveDocument(int nIndex, const wchar_t * pszPathName)
{
	wchar_t szSection[32];
	wchar_t szEntry[32];
	wchar_t szData[128];

	wsprintf(szSection, szReport, nIndex);

	// convert from file index to file id
	
	int			nCount = 0;
	POSITION	pos = ptheInfo->m_File.GetHeadPosition();
	while (pos != NULL)
	{
		INFO_FILE* pFile = (INFO_FILE*)ptheInfo->m_File.GetNext(pos);
		if (pFile->m_strOption.IsEmpty())	// normal file ?
		{
			if (nCount == m_nFile)
			{
				_itow(pFile->m_nNumber, szData, 10);
				break;
			}
			else
				nCount++;
		}
	}
	
	::WritePrivateProfileString(szSection, szFile, szData, pszPathName);

	_itow(m_nDevice, szData, 10);
	::WritePrivateProfileString(szSection, szDevice, szData, pszPathName);

	::WritePrivateProfileString(szSection, szTitle, m_strTitle, pszPathName);

	int nKey = 0;
	nCount   = 1;
	pos      = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem == NULL)
			break;
		wsprintf(szEntry, szItem, nCount++);
		pItem->GetItem(m_nFile, szData, &nKey);
		::WritePrivateProfileString(szSection, szEntry, szData, pszPathName);
	}

	static wchar_t szEOR[] = L"2,%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
	wsprintf(szEntry, szItem, nCount);
	wsprintf(szData, szEOR, m_sizeReport.cy, m_sizeReport.cx);
	::WritePrivateProfileString(szSection, szEntry, szData, pszPathName);

	_itow(nKey, szData, 10);
	::WritePrivateProfileString(szSection, szKey, szData, pszPathName);

	return TRUE;
}

long CReport::GetUsedSize()
{
	long lUsed = DOC_SIZE_REPORT + DOC_SIZE_ITEM * m_ItemObj.GetCount();  // report header and end of report

	return lUsed;
}

/////////////////////////////////////////////////////////////////////////////

void CReport::SetTitle(CString& strTitle)
{
	m_strTitle = strTitle;
}

CString CReport::GetExtendedTitle()
{
	if (m_strTitle.IsEmpty())
		return L"(" + ptheInfo->GetFileName(m_nFile) + L")";
	else
		return m_strTitle;
}

wchar_t * CReport::FormatReportInfo(int* pnNo, wchar_t * pszBuffer)
{
	INFO_DEVICE* pDevice = ptheInfo->GetDevice(m_nDevice);
	ASSERT(pDevice != NULL);

	if (pDevice->m_nNumber != INFO_DEVICE_LCD)
	{
		wsprintf(pszBuffer, L"%d \t%s \t%s \t%s", *pnNo,
				 ptheInfo->GetFileName(m_nFile),
				 pDevice->m_strName,
				 m_strTitle);
		(*pnNo)++;
	}
	else
		wsprintf(pszBuffer, L"\t%s \t%s \t%s",
				 ptheInfo->GetFileName(m_nFile),
				 pDevice->m_strName,
				 m_strTitle);

	return pszBuffer;
}

/////////////////////////////////////////////////////////////////////////////

void CReport::ActivateWindow()
{
	m_pView->GetParentFrame()->ActivateFrame();
}

void CReport::DestroyWindow()
{
	if (m_pView != NULL)
		m_pView->GetParentFrame()->DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////

POSITION CReport::Insert(CItem* pItem)
{
	ASSERT(! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)));

	m_pDoc->SetModifiedFlag();

	POSITION pos = m_ItemObj.GetHeadPosition();
	if (pos == NULL)
	{
		m_ItemObj.InsertAfter(pos, pItem);
		return pos;
	}
		
	POSITION get;
	while ((get = pos) != NULL)
	{
		CItem* pGet = (CItem*)m_ItemObj.GetNext(pos);

		if (pGet->m_pt.y < pItem->m_pt.y)
			continue;
		else if (pGet->m_pt.y > pItem->m_pt.y)
			break;

		if (pGet->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			break;
		else if (pGet->m_pt.x >= pItem->m_pt.x)
			break;
	}

	if (get != NULL)
		return m_ItemObj.InsertBefore(get, pItem);
	else
		return m_ItemObj.AddTail(pItem);
}

POSITION CReport::Insert(CLoopItem* pItem)
{
	ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)));

	m_pDoc->SetModifiedFlag();

	POSITION pos = m_ItemObj.GetHeadPosition();
	if (pos == NULL)
	{
		m_ItemObj.InsertAfter(pos, pItem);
		return pos;
	}

	POSITION get;
	while ((get = pos) != NULL)
	{
		CItem* pGet = (CItem*)m_ItemObj.GetNext(pos);

		if (pGet->m_pt.y < pItem->m_pt.y)
			continue;
		else if (pGet->m_pt.y > pItem->m_pt.y)
			break;

		if (pGet->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			if (pGet->m_pt.x < pItem->m_pt.x)
				continue;
			else
				break;
		else if (pItem->m_pt.y > pGet->m_pt.y - 1)
			continue;
		else if (pItem->m_pt.x > 0)
			break;
	}

	if (get != NULL)
		return m_ItemObj.InsertBefore(get, pItem);
	else
		return m_ItemObj.AddTail(pItem);
}

void CReport::MoveItemRow(CItem* pItem, int nRow)
{
	ASSERT(pItem != NULL);
	ASSERT(nRow > 0);

	BOOL bState = FALSE;
	POSITION pos = m_ItemObj.GetHeadPosition();

	while (pos != NULL)
	{
		CItem* pWork = (CItem*)m_ItemObj.GetNext(pos);

		if (bState)
			pWork->m_pt.y += nRow;
		if (pWork == pItem)
			bState = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CReport::Action(int nState)
{
	m_nAction = nState;					// set new state

	if (m_nAction != none)				// new action
		return;
	if (m_pView == NULL)
		return;

	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);

	if (m_bCapture)
	{
		m_bCapture = FALSE;
		::ReleaseCapture();
	}

	InvalidateInsertLine(&dc, m_nInsert[RPT_INDEX_NEW]);	// clean up

	m_nInsert[RPT_INDEX_NEW] = -1;
}

/////////////////////////////////////////////////////////////////////////////

void CReport::UpdateSheetSize(int nRow, BOOL bModified /*=TRUE*/)
{
	ASSERT(m_pView != NULL);

	int nOld = m_sizeSheet.cy;

	m_sizeReport.cy += nRow;

	m_sizeSheet.cx = m_sizeItem.cx * m_sizeReport.cx +
					 RPT_SHEET_LEFT + RPT_SHEET_RIGHT;
	m_sizeSheet.cy = (m_sizeItem.cy + RPT_SPACE_Y) * m_sizeReport.cy +
					 RPT_SHEET_TOP + RPT_SHEET_BOTTOM;

	SetScrollInfo();

	CRect rect;
	rect.left   = 0;
	rect.top    = RPT_PT_SHEETY;
	rect.right  = ::GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = RPT_PT_SHEETY + max(nOld, m_sizeSheet.cy);

	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	dc.LPtoDP(&rect);
	m_pView->InvalidateRect(&rect, FALSE);

	if (bModified)
		m_pDoc->SetModifiedFlag();
}

void CReport::SetScrollInfo()
{
	CRect rectClient;
	m_pView->GetClientRect(&rectClient);

	CSize sizePage(rectClient.right / 2, rectClient.bottom / 2);

	CSize sizeFrame(m_sizeSheet);
	sizeFrame.cx += (RPT_LEVEL_X * 3);
	sizeFrame.cy += sizePage.cy;

	m_pView->SetScrollSizes(MM_TEXT, sizeFrame, sizePage);
}

/////////////////////////////////////////////////////////////////////////////

void CReport::InvalidateInsertLine(CDC* pDC, int nRow)
{
	if (nRow == -1)
		return;

	CRect rect;
	rect.left   = RPT_PT_SHEETX + 1;
	rect.top    = GetInsertLine(nRow);
	rect.right  = ::GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = rect.top + 1;			// +1 adjust
	pDC->LPtoDP(&rect);
	m_pView->InvalidateRect(&rect, FALSE);
}

void CReport::InvalidateSelect(CDC* pDC, BOOL bCancel /*=FALSE*/)
{
	CRect rect(m_rectSelect);
	pDC->LPtoDP(&rect);
	m_pView->InvalidateRect(&rect, FALSE);

	if (bCancel)						// cancel
		m_rectSelect.SetRect(-1, -1, -1, -1);
}

int CReport::GetInsertLine(int nRow)
{
	return RPT_PT_SHEETY +
		   RPT_SHEET_TOP / 2 + (m_sizeItem.cy + RPT_SPACE_Y) * nRow;
}

int CReport::GetItemRow(int nY)
{
	if (nY <= RPT_PT_SHEETY || RPT_PT_SHEETY + m_sizeSheet.cy <= nY)
		return -1;						// outside of report sheet

	int	nRow1 = (nY - RPT_PT_SHEETY - RPT_SHEET_TOP / 2) /
				(m_sizeItem.cy + RPT_SPACE_Y);
	int	nRow2 = (nY - RPT_PT_SHEETY - RPT_SHEET_TOP / 2) %
				(m_sizeItem.cy + RPT_SPACE_Y);
	if (nRow2 > (m_sizeItem.cy + RPT_SPACE_Y) / 2)
		nRow1 += 1;

	return nRow1;						// zero-based
}

CRect CReport::GetRowRect(int nRow)
{
	CRect rect;

	rect.left   = RPT_PT_SHEETX;
	rect.top    = GetInsertLine(nRow);
	rect.right  = RPT_PT_SHEETX + m_sizeSheet.cx;
	rect.bottom = GetInsertLine(nRow + 1) + 1;	// adjust

	return rect;
}

CRect CReport::GetResizeRect()
{
	CRect rect;

	rect.left   = RPT_PT_SHEETX;
	rect.top    = RPT_PT_SHEETY + m_sizeSheet.cy - RPT_HIT_ADJUST;
	rect.right  = RPT_PT_SHEETX + m_sizeSheet.cx;
	rect.bottom = RPT_PT_SHEETY + m_sizeSheet.cy + RPT_HIT_ADJUST;

	return rect;
}

/////////////////////////////////////////////////////////////////////////////
// CReport drawing

void CReport::OnDraw(CDC* pDC)
{
	DrawSheet(pDC);
	DrawInsert(pDC);
	DrawSelect(pDC);
	DrawItems(pDC);
}

void CReport::DrawSheet(CDC* pDC)
{
	int nCount;

	pDC->SelectStockObject(OEM_FIXED_FONT);

	// draw background
	CRect client;
	pDC->GetClipBox(client);
	CBrush brush(RPT_COLOR_GRAY);
	brush.UnrealizeObject();
	pDC->FillRect(client, &brush);

	// draw column gauge
	CBrush* pOldBrush = NULL;
	pOldBrush = pDC->SelectObject(&brush);
	pDC->SetBkMode(TRANSPARENT);
	wchar_t szBuffer[16];
	for (nCount = 0; nCount < m_sizeReport.cx; nCount++)
	{
		_itow((nCount + 1) % 10, szBuffer, 10);
		pDC->TextOut(RPT_PT_SHEETX + RPT_SHEET_LEFT + 1 + m_sizeItem.cx * nCount,
					 RPT_PT_SHEETY                      - m_sizeItem.cy,
					 szBuffer, 1);
	}
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);
	
	// draw row gauge
	int nHigh = GetInsertLine(0);
	for (nCount = 0; nCount <= m_sizeReport.cy; nCount++)
	{
		CRect rect;
		rect.left   = 0;
		rect.top    = nHigh;
		rect.right  = RPT_PT_SHEETX;
		nHigh       = GetInsertLine(nCount + 1);
		rect.bottom = nHigh + 1;

		pDC->SelectStockObject(BLACK_PEN);
		pDC->MoveTo(rect.left,  rect.top);
		pDC->LineTo(rect.right, rect.top);
		pDC->SelectStockObject(WHITE_PEN);
		pDC->MoveTo(rect.left,  rect.top + 1);
		pDC->LineTo(rect.right, rect.top + 1);

		if (nCount != m_sizeReport.cy)
		{
			wsprintf(szBuffer, L"%d", nCount + 1);
			pDC->DrawText(szBuffer, wcslen(szBuffer), &rect,
						  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}
	
	// draw base sheet
	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->Rectangle(RPT_PT_SHEETX, RPT_PT_SHEETY, RPT_PT_SHEETX + m_sizeSheet.cx, RPT_PT_SHEETY + m_sizeSheet.cy);

	// select row ?
	if (m_nHit != RPT_CANCEL)
	{
		CRect rect = GetRowRect(m_nHit);
		pOldBrush = NULL;
		CBrush brush1(RPT_COLOR_BG);
		pOldBrush = pDC->SelectObject(&brush1);
		pDC->Rectangle(rect);
		if (pOldBrush != NULL)
			pDC->SelectObject(pOldBrush);
	}
}

void CReport::DrawInsert(CDC* pDC)
{
	if (m_nAction != insertItem && m_nAction != insertRow)
	{
		m_nInsert[RPT_INDEX_NEW] = -1;	// cancel
		return;
	}

	int nSide = ::GetSystemMetrics(SM_CXSCREEN);
	int nHigh = GetInsertLine(m_nInsert[RPT_INDEX_TMP]);

	// draw horizontal line
	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(RPT_PT_SHEETX + 1, nHigh);
	pDC->LineTo(nSide,			   nHigh);

	m_nInsert[RPT_INDEX_NEW] = m_nInsert[RPT_INDEX_TMP];
}

void CReport::DrawSelect(CDC* pDC)
{
	CPen* pOldPen = NULL;
	CPen pen(PS_DOT, 1, RPT_COLOR_BLACK);
	pOldPen = pDC->SelectObject(&pen);

	CRect rect(-1, -1, -1, -1);
	if (m_rectSelect != rect)				// selected
		pDC->Rectangle(&m_rectSelect);

	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);			// restore pen
}

void CReport::DrawItems(CDC* pDC)
{
	POSITION pos = m_ItemObj.GetHeadPosition();
	
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		pItem->Draw(this, pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CReport message handlers

void CReport::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	dc.DPtoLP(&point);

	switch (m_nAction)
	{
	case none:
		MouseMoveNone(&dc, point);
		break;
	case resizeLoop:
		MouseMoveLoop(&dc, point);
		break;
	case moveItem:
		MouseMoveItem(&dc, point);
		break;
	case selectItem:
		MouseMoveSelect(&dc, point);
		break;
	case insertItem:
	case insertRow:
		MouseMoveInsert(&dc, point);
		break;
	case resizeSheet:
		MouseMoveSheet(&dc, point);
		break;
	default:
		break;
	}
}

void CReport::MouseMoveNone(CDC* pDC, CPoint point)
{
	// resize report sheet ?
	CRect rect = GetResizeRect();
	if (rect.PtInRect(point))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_SIZENS));
		return;
	}

	// move items ?
	if (m_rectSelect.PtInRect(point))
	{
		if (m_SelectObj.GetCount() > 1)		// multiple select
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_MOVE));
		return;
	}
	
	// resize loop area ?
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->MouseMove(this, pDC, point))
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_SIZENS));
	}
}

void CReport::MouseMoveLoop(CDC* pDC, CPoint point)
{
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			pItem->MouseMove(this, pDC, point);
	}
}

void CReport::MouseMoveItem(CDC* pDC, CPoint point)
{
	if (! m_bCapture)
	{
		m_bCapture = TRUE;
		m_pView->SetCapture();
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_MOVE));
	}

	CRect rect(m_rectLast);
	rect.left   += (point.x - m_ptMove.x);
	rect.top    += (point.y - m_ptMove.y);
	rect.right  += (point.x - m_ptMove.x);
	rect.bottom += (point.y - m_ptMove.y);
	rect.NormalizeRect();

	CRect sheet(RPT_PT_SHEETX,                  RPT_PT_SHEETY,
				RPT_PT_SHEETX + m_sizeSheet.cx, RPT_PT_SHEETY + m_sizeSheet.cy);
	if (sheet.PtInRect(rect.TopLeft()) &&
		sheet.PtInRect(rect.BottomRight()))
	{
		// clean up previous rectangle
		pDC->DrawFocusRect(m_rectLast);

		m_ptMove = point;
		m_rectLast = rect;
		pDC->DrawFocusRect(m_rectLast);
	}
}

void CReport::MouseMoveSelect(CDC* pDC, CPoint point)
{
	CRect rect(m_ptDown.x, m_ptDown.y, m_ptLast.x, m_ptLast.y);
	rect.NormalizeRect();
	pDC->DrawFocusRect(rect);
	rect.SetRect(m_ptDown.x, m_ptDown.y, point.x, point.y);
	rect.NormalizeRect();
	pDC->DrawFocusRect(rect);
	m_ptLast = point;
}

void CReport::MouseMoveInsert(CDC* pDC, CPoint point)
{
	int	nRow = GetItemRow(point.y);
	if (nRow == -1)
		return;
	if (m_nInsert[RPT_INDEX_TMP] == nRow)	// same row position
		return;

	m_nInsert[RPT_INDEX_TMP] = nRow;		// set new row position

	InvalidateInsertLine(pDC, m_nInsert[RPT_INDEX_TMP]);	// draw new line
	InvalidateInsertLine(pDC, m_nInsert[RPT_INDEX_NEW]);	// clean up
}

void CReport::MouseMoveSheet(CDC* pDC, CPoint point)
{
	int	nRow = (point.y - RPT_PT_SHEETY) / (m_sizeItem.cy + RPT_SPACE_Y);
	if (nRow < 0)
		return;
	if (nRow == 0)
		nRow++;
	if (nRow == m_sizeReport.cy ||		// same row of sheet size
		nRow >= RPT_NUM_ROW)			// maximum row
		return;

	CItem* pItem = NULL;
	POSITION pos = GetTailPosition();
	if (pos != NULL)
		pItem = GetAt(pos);
	if (pItem != NULL && pItem->m_pt.y > nRow)
		return;

	int nOldHigh    = m_sizeSheet.cy;	// backup current sheet size
	m_sizeReport.cy = nRow;				// set new sheet size

	UpdateSheetSize(0);

	CRect rect;
	rect.left   = 0;
	rect.top    = RPT_PT_SHEETY + min(m_sizeSheet.cy, nOldHigh) - 1;
	rect.right  = RPT_PT_SHEETX + m_sizeSheet.cx;
	rect.bottom = RPT_PT_SHEETY + max(m_sizeSheet.cy, nOldHigh);
	pDC->LPtoDP(&rect);
	m_pView->InvalidateRect(&rect, FALSE);
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	dc.DPtoLP(&point);

	switch (m_nAction)
	{
	case none:
		LButtonDownNone(&dc, point);
		break;
	case insertItem:
		LButtonDownInsert(&dc, point);
		Action(none);
		break;
	case insertRow:
		LButtonDownRow(&dc, point);
		Action(none);
		break;
	default:
		break;
	}
}

void CReport::LButtonDownNone(CDC* pDC, CPoint point)
{
	// resize report sheet ?
	CRect rect = GetResizeRect();
	if (rect.PtInRect(point))
	{
		m_nAction  = resizeSheet;
		m_bCapture = TRUE;
		m_pView->SetCapture();
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_SIZENS));
		return;
	}

	// move selected items ?
	if (m_rectSelect.PtInRect(point))
	{
		m_nAction  = moveItem;
		m_ptMove   = point;
		m_rectLast = m_rectSelect;
		m_bCapture = TRUE;
		m_pView->SetCapture();
		::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_MOVE));
		return;
	}
	
	InvalidateSelect(pDC, TRUE);
	
	// cancel row selection
	if (m_nHit != RPT_CANCEL)
	{
		rect = GetRowRect(m_nHit);
		pDC->LPtoDP(&rect);
		m_pView->InvalidateRect(&rect, FALSE);
		m_nHit = RPT_CANCEL;
	}

	// resize loop item ?
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->LButtonDown(this, pDC, point))
		{
			m_bCapture = TRUE;
			m_pView->SetCapture();
			::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_SIZENS));
			m_nAction = resizeLoop;
			return;
		}
	}

	// cancel all selections
	pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		pItem->Cancel(this, pDC);
	}
	m_rectSelect.SetRect(-1, -1, -1, -1);	// initialization
	m_SelectObj.RemoveAll();
	
	// select row ?
	int nHigh = GetInsertLine(0);
	for (int nCount = 0; nCount < m_sizeReport.cy; nCount++)
	{
		CRect rect;
		rect.left   = 0;
		rect.top    = nHigh;
		rect.right  = RPT_PT_SHEETX;
		nHigh       = GetInsertLine(nCount + 1);
		rect.bottom = nHigh + 1;
		if (rect.PtInRect(point))
		{
			pos = m_ItemObj.GetHeadPosition();
			while (pos != NULL)
			{
				CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
				pItem->HitOnRow(this, pDC, nCount + 1);
			}

			rect.left  = RPT_PT_SHEETX;
			rect.right = RPT_PT_SHEETX + m_sizeSheet.cx;
			pDC->LPtoDP(&rect);
			m_pView->InvalidateRect(&rect, FALSE);
			m_nHit = nCount;
			return;
		}
	}

	// select loop item ?
	CItem* pFind = NULL;
	pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			continue;
		int nLevel = ((CLoopItem*)pItem)->GetSelectLevel(this, pDC, point);
		if (nLevel != -1 && pFind != NULL)
		{
			pFind->Cancel(this, pDC);
			return;
		}
		else if (nLevel == 1)
			pFind = pItem;
		else if (nLevel == 2)
			return;
	}
	if (pFind != NULL)
		return;
	
	// select print or mathematics item ?
//	m_rectSelect.SetRect(-1, -1, -1, -1);	// initialization
//	m_SelectObj.RemoveAll();
	POSITION select = m_SelectObj.GetHeadPosition();
	pos = m_ItemObj.GetTailPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetPrev(pos);
		if (! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->Select(this, pDC, point, &m_rectSelect))
		{
			m_SelectObj.AddTail(pItem);

			m_nAction  = moveItem;
			m_ptMove   = point;
			m_rectLast = m_rectSelect;
//			m_bCapture = TRUE;
//			m_pView->SetCapture();
//			::SetCursor(AfxGetApp()->LoadCursor(IDC_PR_MOVE));
			return;
		}
	}

	// start of selection
	m_nAction  = selectItem;
	m_ptDown   = point;
	m_ptLast   = point;
	m_bCapture = TRUE;
	m_pView->SetCapture();
}

void CReport::LButtonDownInsert(CDC* pDC, CPoint point)
{
	if (m_pDoc->CheckSize(DOC_SIZE_FIELDS) == PROGRPT_ERROR)
		return;

	CItem* pItem;

	if (m_nOperation == CFG_INDEX_LOOP)
		pItem = new CLoopItem(m_nOperation);
	else if (m_nOperation == CFG_INDEX_PRINT)
		pItem = new CPrintItem(m_nOperation);
	else if (m_nOperation == CFG_INDEX_MATHE)
		pItem = new CMatheItem(m_nOperation);
	else
		return;

	// backup selected items
	CObList BackupObj;
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->GetHitState())
			BackupObj.AddTail(pItem);
	}
	
	int nMove = pItem->Insert(this, pDC, m_nFile, m_nInsert[RPT_INDEX_NEW]);
	if (nMove > 0)						// insert item and move
	{
		MoveItemRow(pItem, nMove);
		UpdateSheetSize(nMove);
		m_pDoc->UpdateStatusSize();
	}
	else if (nMove == 0)				// insert item
	{
		m_pDoc->UpdateStatusSize();
		m_pDoc->SetModifiedFlag();
	}
	else								// cancel by user
	{
		delete pItem;
		return;
	}
	
	// cancel row selection
	if (m_nHit != RPT_CANCEL)
	{
		CRect rect = GetRowRect(m_nHit);
		pDC->LPtoDP(&rect);
		m_pView->InvalidateRect(&rect, FALSE);
		m_nHit = RPT_CANCEL;
	}

	// cancel selected items
	pos = BackupObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)BackupObj.GetNext(pos);
		pItem->Cancel(this, pDC);
	}

	// select print or mathematics item ?
	CRect rect;								// dummy
	m_rectSelect.SetRect(-1, -1, -1, -1);	// initialization
	m_SelectObj.RemoveAll();
	POSITION select = m_SelectObj.GetHeadPosition();
	pos = m_ItemObj.GetTailPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetPrev(pos);
		if (! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->GetHitState())
		{
			pItem->Select(this, pDC, rect, &m_rectSelect, TRUE);
			m_SelectObj.AddTail(pItem);
		}
	}
}

void CReport::LButtonDownRow(CDC* pDC, CPoint point)
{
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if ((  pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			   pItem->m_pt.y >= m_nInsert[RPT_INDEX_NEW]) ||
			(! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) && 
			   pItem->m_pt.y >= m_nInsert[RPT_INDEX_NEW] + 1))
			pItem->m_pt.y++;			// move item
	}
	
	UpdateSheetSize(1);					// insert one line

	if (m_nHit != RPT_CANCEL && m_nHit >= m_nInsert[RPT_INDEX_NEW])
		m_nHit++;						// move hit row

	// re-select items mark
	CRect rect;							// dummy
	m_rectSelect.SetRect(-1, -1, -1, -1);
	pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		pItem->Select(this, pDC, rect, &m_rectSelect, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnLButtonUp(UINT nFlags, CPoint point)
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	dc.DPtoLP(&point);

	switch (m_nAction)
	{
	case resizeLoop:
		LButtonUpLoop(&dc, point);
		break;
	case moveItem:
		LButtonUpItem(&dc, point);
		break;
	case selectItem:
		LButtonUpSelect(&dc, point);
		break;
	default:
		break;
	}

	Action(none);
}

void CReport::LButtonUpLoop(CDC* pDC, CPoint point)
{
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)))
			pItem->LButtonUp(this, pDC, point);
	}
}
	
void CReport::LButtonUpItem(CDC* pDC, CPoint point)
{
	// clean up previous rectangle
	pDC->DrawFocusRect(m_rectLast);

	CPoint delta;
	delta.x =  (m_rectLast.left - RPT_PT_SHEETX - RPT_SHEET_LEFT / 2) /
			   (m_sizeItem.cx);
	delta.y =  (m_rectLast.top  - RPT_PT_SHEETY - RPT_SHEET_TOP / 2) /
			   (m_sizeItem.cy + RPT_SPACE_Y);
	delta.x -= (m_rectSelect.left - RPT_PT_SHEETX - RPT_SHEET_LEFT / 2) /
			   (m_sizeItem.cx);
	delta.y -= (m_rectSelect.top  - RPT_PT_SHEETY - RPT_SHEET_TOP / 2) /
			   (m_sizeItem.cy + RPT_SPACE_Y);

	if (delta.x == 0 && delta.y == 0)
		return;

	POSITION pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		POSITION find = m_ItemObj.Find(pItem);
		if (find == NULL)
			continue;
		pItem->m_pt.x += delta.x;
		pItem->m_pt.y += delta.y;
		m_ItemObj.RemoveAt(find);
		Insert(pItem);
	}

	// select items mark
	CRect rect;							// dummy
	m_rectSelect.SetRect(-1, -1, -1, -1);
	pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		pItem->Select(this, pDC, rect, &m_rectSelect, TRUE);
	}

	m_pDoc->UpdateAllViews(NULL);
	m_pDoc->SetModifiedFlag();
}
	
void CReport::LButtonUpSelect(CDC* pDC, CPoint point)
{
	// selected ?
	if (m_ptDown.x != m_ptLast.x || m_ptDown.y != m_ptLast.y)
	{
		// clean up
		CRect rect(m_ptDown.x, m_ptDown.y, m_ptLast.x, m_ptLast.y);
		rect.NormalizeRect();
		pDC->DrawFocusRect(rect);

		// initialization
		m_rectSelect.SetRect(-1, -1, -1, -1);
		m_SelectObj.RemoveAll();

		// select items		
		POSITION select = m_SelectObj.GetHeadPosition();
		POSITION pos = m_ItemObj.GetHeadPosition();
		while (pos != NULL)
		{
			CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
			if (pItem->Select(this, pDC, rect, &m_rectSelect))
				m_SelectObj.AddTail(pItem);
		}

		m_rectSelect.NormalizeRect();
		InvalidateSelect(pDC);
	}

	m_ptDown.x = -1;
	m_ptDown.y = -1;
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	dc.DPtoLP(&point);

	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->LButtonDblClk(this, &dc, point))
			break;
	}

	pos = m_ItemObj.GetTailPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetPrev(pos);
		if (! pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->LButtonDblClk(this, &dc, point))
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnEditCut()
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);

	CopyItem(-1);
	RemoveSelect(&dc);

	m_pDoc->UpdateStatusSize();
}

BOOL CReport::OnUpdateEditCut()
{
	return OnUpdateEditCopy();
}

void CReport::OnEditCopy()
{
	CopyItem(m_nMagic);
}

BOOL CReport::OnUpdateEditCopy()
{
	if (m_nHit == RPT_CANCEL &&	m_SelectObj.GetCount() > 0)
		return TRUE;
	else
		return FALSE;
}

void CReport::CopyItem(int nMagic)
{
	// check number of copy items
	DWORD dwSize  = max(sizeof(CLoopItem), sizeof(CPrintItem));
		  dwSize  = max(dwSize,            sizeof(CMatheItem));
	if (sizeof(WORD) * 2 +
		m_SelectObj.GetCount() * dwSize > CLIPBOARD_MAX_SIZE)
	{
		AfxMessageBox(IDS_ERRMSG_CLIPBOARD);
		return;
	}

	CClipboardFile clpfile(m_pView);
	clpfile.Open(((CProgrptApp*)AfxGetApp())->m_uPrivateFmt, CFile::modeWrite, CLIPBOARD_MAX_SIZE);
	CArchive ar((CFile*)clpfile, CArchive::store);

	ar << (WORD)m_nFile;				// save file type
	ar << (WORD)nMagic;					// save magic number

	m_SelectObj.Serialize(ar);			// save selected item

	ar.Close();
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnEditPaste()
{
	int nCount = m_ItemObj.GetCount();

	CClipboardFile clpfile(m_pView);
	clpfile.Open(((CProgrptApp*)AfxGetApp())->m_uPrivateFmt, CFile::modeRead, CLIPBOARD_MAX_SIZE);
	CArchive ar((CFile*)clpfile, CArchive::load);

	int nFile, nMagic;
	WORD wTemp;
	ar >> wTemp;	nFile  = (int)wTemp;	// restore file type
	ar >> wTemp;	nMagic = (int)wTemp;	// restore magic number

	if (nFile == m_nFile)					// same file type
		Serialize(ar);						// restore paste items

	ar.Close();

	if (nCount != m_ItemObj.GetCount())		// item found
	{
		CClientDC dc(m_pView);
		m_pView->OnPrepareDC(&dc);
		PasteItem(&dc, nCount, nMagic);		// clean up paste items
	}
	else
	{
		m_pDoc->UpdateStatusMsg(IDS_STSMSG_PASTE);
		MessageBeep(-1);
	}
}

void CReport::PasteItem(CDC* pDC, int nIndex, int nMagic)
{
	int nPos = (nMagic == m_nMagic) ? (1) : (0);

	// move paste items from item list to select list
	m_SelectObj.RemoveAll();
	POSITION pos = m_ItemObj.FindIndex(nIndex);
	while (pos != NULL)
	{
		POSITION find = pos;
		m_SelectObj.AddTail(m_ItemObj.GetNext(pos));
		m_ItemObj.RemoveAt(find);
	}

	// re-insert paste items
	pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		if (pItem->m_nFile != m_nFile)	// invalide file type
			ASSERT(FALSE);

		// adjust row position ?
		if (pItem->m_pt.y >= m_sizeReport.cy)
			pItem->m_pt.y = m_sizeReport.cy;
		else if (m_sizeReport.cy != 1)
			pItem->m_pt.y += nPos;
		else
			pItem->m_pt.y = 1;

		// adjust column position ?
		if (pItem->m_pt.x + pItem->m_nWidth - 1 > m_sizeReport.cx)
			pItem->m_pt.x = 1;

		// adjust width position ?
		if (pItem->m_pt.x + pItem->m_nWidth - 1 > m_sizeReport.cx)
			pItem->m_nWidth = m_sizeReport.cx - pItem->m_pt.x;

		Insert(pItem);
	}

	// cancel row selection
	if (m_nHit != RPT_CANCEL)
	{
		CRect rect = GetRowRect(m_nHit);
		pDC->LPtoDP(&rect);
		m_pView->InvalidateRect(&rect, FALSE);
		m_nHit = RPT_CANCEL;
	}

	// cancel all items
	pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		pItem->Cancel(this, pDC);
	}
	
	// select paste items
	CRect rect;							// dummy
	m_rectSelect.SetRect(-1, -1, -1, -1);
	pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		pItem->Select(this, pDC, rect, &m_rectSelect, TRUE);
	}

	m_pDoc->UpdateAllViews(NULL);
	m_pDoc->UpdateStatusSize();
	m_pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////

void CReport::OnEditClear()
{
	CClientDC dc(m_pView);
	m_pView->OnPrepareDC(&dc);
	
	RemoveSelect(&dc);					// remove selected items
	if (! RemoveItem(&dc))				// remove loop item or items on row
		return;
	if (m_nHit == RPT_CANCEL)			// row is not select
		return;

	// move items
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->m_pt.y > m_nHit)
			pItem->m_pt.y--;
		pItem->HitOnRow(this, &dc, m_nHit + 1);
	}

	// delete row of sheet
	if (m_sizeReport.cy < m_nHit + 1)	// invalide select
	{
		CRect rect = GetRowRect(m_nHit);
		dc.LPtoDP(&rect);
		m_pView->InvalidateRect(&rect, FALSE);
		m_nHit = RPT_CANCEL;			// cancel
	}
	if (m_nHit != RPT_CANCEL && m_sizeReport.cy > 1)
		UpdateSheetSize(-1);
	else
	{
		m_pDoc->UpdateStatusMsg(IDS_STSMSG_DELROW);
		MessageBeep(-1);
	}
	if (m_sizeReport.cy < m_nHit + 1)	// invalide select
		m_nHit = RPT_CANCEL;			// cancel
}

BOOL CReport::OnUpdateEditClear()
{
	if (m_nHit != RPT_CANCEL)			// select row gauge
		return TRUE;

	if (m_SelectObj.GetCount() > 0)		// select items
		return TRUE;

	// check section of loop item
	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetNext(pos);
		if (pItem->IsKindOf(RUNTIME_CLASS(CLoopItem)) &&
			pItem->GetHitState())
			return TRUE;
	}

	return FALSE;
}

BOOL CReport::RemoveItem(CDC* pDC)
{
	int nCount = m_ItemObj.GetCount();
	if (nCount == 0)					// item not found
		return TRUE;

	POSITION pos = m_ItemObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_ItemObj.GetAt(pos);
		int nState = pItem->Remove(this, pDC, m_nHit);
		if (nState == PROGRPT_OK)
		{
			POSITION find = pos;
			m_ItemObj.GetNext(pos);
			m_ItemObj.RemoveAt(find);
			delete pItem;
		}
		else if (nState == PROGRPT_WARNING)
		{
			m_pDoc->UpdateStatusMsg(IDS_STSMSG_DELROW);
			MessageBeep(-1);
			return FALSE;
		}
		else
			m_ItemObj.GetNext(pos);
	}

	if (nCount != m_ItemObj.GetCount())		// deleted items
	{
		m_pDoc->UpdateStatusSize();
		m_pDoc->SetModifiedFlag();
	}

	return TRUE;
}

void CReport::RemoveSelect(CDC* pDC)
{
	if (m_SelectObj.GetCount() == 0)		// not selected
		return;

	int nCount = m_ItemObj.GetCount();

	POSITION pos = m_SelectObj.GetHeadPosition();
	while (pos != NULL)
	{
		CItem* pItem = (CItem*)m_SelectObj.GetNext(pos);
		if (pItem->Remove(this, pDC, m_nHit) == PROGRPT_OK)
		{
			POSITION find = m_ItemObj.Find(pItem);
			if (find != NULL)
				m_ItemObj.RemoveAt(find);
			else
				ASSERT(FALSE);
			delete pItem;
		}
	}
	m_SelectObj.RemoveAll();

	if (nCount != m_ItemObj.GetCount())		// deleted items
	{
		InvalidateSelect(pDC, TRUE);
		m_pDoc->UpdateStatusSize();
		m_pDoc->SetModifiedFlag();
	}
}

BOOL CReport::OnUpdateEditInsertrow()
{
	if (m_sizeReport.cy < RPT_NUM_ROW - 1)
		return TRUE;
	else
		return FALSE;
}
