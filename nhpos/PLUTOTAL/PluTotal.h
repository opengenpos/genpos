// PluTotal.h : main header file for the PLUTOTAL DLL
//

#if !defined(AFX_PLUTOTAL_H__5AAC14DB_B42E_11D3_BE4E_00A0C961E76F__INCLUDED_)
#define AFX_PLUTOTAL_H__5AAC14DB_B42E_11D3_BE4E_00A0C961E76F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include	"PluTtlAp.h"

/////////////////////////////////////////////////////////////////////////////
// CPluTotalApp
// See PluTotal.cpp for the implementation of this class
//

class CPluTotalApp : public CWinApp
{
public:
	CPluTotalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluTotalApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPluTotalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUTOTAL_H__5AAC14DB_B42E_11D3_BE4E_00A0C961E76F__INCLUDED_)
