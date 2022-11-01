/////////////////////////////////////////////////////////////////////////////
// progrfrm.h : header file
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

/////////////////////////////////////////////////////////////////////////////
// CProgrptFrame frame

class CProgrptFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CProgrptFrame)
protected:
	CProgrptFrame();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
protected:

// Implementation
protected:
	//RJC added public: below
public:
	virtual ~CProgrptFrame();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Generated message map functions
	//{{AFX_MSG(CProgrptFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
