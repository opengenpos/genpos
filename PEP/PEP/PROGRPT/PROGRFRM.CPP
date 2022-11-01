/////////////////////////////////////////////////////////////////////////////
// progrfrm.cpp : implementation file
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

#include "progrfrm.h"
#include "progrdoc.h"
#include "reportvw.h"
#include "report.h"
#include "dlgfield.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgrptFrame

IMPLEMENT_DYNCREATE(CProgrptFrame, CMDIChildWnd)

CProgrptFrame::CProgrptFrame()
{
}

CProgrptFrame::~CProgrptFrame()
{
}

BOOL CProgrptFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	return CMDIChildWnd::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CProgrptFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CProgrptFrame)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProgrptFrame message handlers

void CProgrptFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED)
	{
		CReportView* pView = (CReportView*)GetActiveFrame()->GetActiveView();
		if (pView == NULL)
			return;
		CProgrptDoc* pDoc = (CProgrptDoc*)pView->GetDocument();
		if (pDoc == NULL)
			return;
		CReport* pReport = pDoc->FindActiveReport(pView);
		ASSERT(pReport != NULL);
		m_pField->ActiveDialog(TRUE, pView, pReport->GetFileIndex());
	}
}
