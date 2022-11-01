// StaticColorPatch.cpp : implementation file
//

#include "stdafx.h"
//#include "controlstest.h"
#include "StaticColorPatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticColorPatch

CStaticColorPatch::CStaticColorPatch(COLORREF xColor /* = 0 */) :
	m_FillColor (xColor)
{
}

CStaticColorPatch::~CStaticColorPatch()
{
}


BEGIN_MESSAGE_MAP(CStaticColorPatch, CStatic)
	//{{AFX_MSG_MAP(CStaticColorPatch)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticColorPatch message handlers

void CStaticColorPatch::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	int oldDCContext = dc.SaveDC ();

	CRect myRect;
	GetClientRect (&myRect);

	try
	{
		CBrush myBrush (m_FillColor);
		CPen   myPen (PS_SOLID, 1, m_FillColor);

		dc.SelectObject (&myBrush);
		dc.SelectObject (&myPen);
		dc.Rectangle (&myRect);
	}
	catch(CResourceException* e)
	{
		e->ReportError();
		e->Delete();
	}


	dc.RestoreDC (oldDCContext);
	
	// Do not call CStatic::OnPaint() for painting messages
}

