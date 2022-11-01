/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  DateTimeDlg.cpp
//
//  PURPOSE:    Date and Time dialog class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "PCSample.h"
#include "DateTimeDlg.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//          M E S S A G E    M A P    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDateTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CDateTimeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CDateTimeDlg::CDateTimeDlg()
//
//  PURPOSE :   Constructor of Date and Time dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CDateTimeDlg::CDateTimeDlg( CWnd* pParent ) :
    CDialog( CDateTimeDlg::IDD, pParent )
{
    //{{AFX_DATA_INIT(CDateTimeDlg)
    m_nMonth = -1;
    m_nWeekDay = -1;
    m_sDay = 0;
    m_sHour = 0;
    m_sMinute = 0;
    m_sYear = 0;
    //}}AFX_DATA_INIT
}

//===========================================================================
//
//  FUNCTION :  CDateTimeDlg::CDateTimeDlg()
//
//  PURPOSE :   Constructor of Date and Time dialog class.
//
//  RETURN :    No return value.
//
//===========================================================================

CDateTimeDlg::CDateTimeDlg( const CLIDATETIME& ThisDate, CWnd* pParent ) :
    CDialog( CDateTimeDlg::IDD, pParent )
{
    m_sYear    = ThisDate.usYear;
    m_nMonth   = ThisDate.usMonth - 1;
    m_sDay     = ThisDate.usMDay;
    m_nWeekDay = ThisDate.usWDay;
    m_sHour    = ThisDate.usHour;
    m_sMinute  = ThisDate.usMin;
}

//===========================================================================
//
//  FUNCTION :  CDateTimeDlg::DoDataExchange()
//
//  PURPOSE :   Exchange and validate dialog data.
//
//  RETURN :    No return value.
//
//===========================================================================

void CDateTimeDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDateTimeDlg)
	DDX_CBIndex(pDX, IDC_CB_MONTH, m_nMonth);
	DDX_CBIndex(pDX, IDC_CB_WDAY, m_nWeekDay);
	DDX_Text(pDX, IDC_EDIT_DAY, m_sDay);
	DDV_MinMaxInt(pDX, m_sDay, 1, 31);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_sHour);
	DDV_MinMaxInt(pDX, m_sHour, 0, 23);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_sMinute);
	DDV_MinMaxInt(pDX, m_sMinute, 0, 59);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_sYear);
	DDV_MinMaxInt(pDX, m_sYear, 0, 9999);
	//}}AFX_DATA_MAP
}

////////////////// END OF FILE ( DateTimeDlg.cpp ) //////////////////////////
