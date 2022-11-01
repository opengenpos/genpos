/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  PCSample.h
//
//  PURPOSE:    Sample program for NCR 2170 PC Interface for Win32.
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

#if !defined(AFX_PCSAMPLE_H__70072F04_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_PCSAMPLE_H__70072F04_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "RPageBase.h"


/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CPCSampleApp
//---------------------------------------------------------------------------

class CPCSampleApp : public CWinApp
{
public:
	CPCSampleApp();
    ~CPCSampleApp();

    VOID TraceFunction( LPCTSTR lpszFormat, va_list argList );
	VOID DisplayText( LPCTSTR lpszFormat, va_list argList = NULL );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCSampleApp)
	public:
	virtual BOOL InitInstance();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPCSampleApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    static  LPCWSTR m_lpszLogFileName;

    CStdioFile  m_fileLog;
    CString     m_strLog;
};

//---------------------------------------------------------------------------
//  CAboutDlg
//---------------------------------------------------------------------------

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCSAMPLE_H__70072F04_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

/////////////////// END OF FILE ( PCSample.h ) //////////////////////////////
