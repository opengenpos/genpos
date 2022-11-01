// PEPControl.h : main header file for the PEPCONTROL DLL
//

#if !defined(AFX_PEPCONTROL_H__34CEEF90_F510_406C_9D8C_02016029AC71__INCLUDED_)
#define AFX_PEPCONTROL_H__34CEEF90_F510_406C_9D8C_02016029AC71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SECControl.h"	// Added by ClassView

/****************** Definitions*************************/
// TLDJR.
// When VBO Lock is the tool being used this constant
// should be defined otherwise default to softlocx 
// TLDJR
#define VBOLOCK

// SOFTLOCX_EXPIRE_ALWAYS is only used for testing purposes
// if uncommented this definition caused the softlocx to 
// always expire TLDJR
//#define SOFTLOCX_EXPIRE_ALWAYS
#define SOFTLOCX_EXPIRED  0 //Softlocx is expired ...close pep..

/////////////////////////////////////////////////////////////////////////////
// CPEPControlApp
// See PEPControl.cpp for the implementation of this class
//

class CPEPControlApp : public CWinApp
{
public:
	LONG SoftlocxLicense(VOID);
	CPEPControlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPEPControlApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPEPControlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PEPCONTROL_H__34CEEF90_F510_406C_9D8C_02016029AC71__INCLUDED_)
