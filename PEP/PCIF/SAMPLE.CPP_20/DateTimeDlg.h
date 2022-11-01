/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  DateTimeDlg.h
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
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATETIMEDLG_H__11C15414_47A2_11D2_A9F0_0000399BE006__INCLUDED_)
#define AFX_DATETIMEDLG_H__11C15414_47A2_11D2_A9F0_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CDateTimeDlg
//---------------------------------------------------------------------------

class CDateTimeDlg : public CDialog
{
// Construction
public:
	CDateTimeDlg(CWnd* pParent = NULL);   // standard constructor
    CDateTimeDlg( const CLIDATETIME& ThisDate, CWnd* pParent = NULL );

// Dialog Data
	//{{AFX_DATA(CDateTimeDlg)
	enum { IDD = IDD_SET_DATETIME };
	int		m_nMonth;
	int		m_nWeekDay;
	short	m_sDay;
	short	m_sHour;
	short	m_sMinute;
	short	m_sYear;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDateTimeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATETIMEDLG_H__11C15414_47A2_11D2_A9F0_0000399BE006__INCLUDED_)

///////////////// END OF FILE ( DateTimeDlg.h ) /////////////////////////////
