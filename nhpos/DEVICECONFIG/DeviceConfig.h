// DeviceConfig.h : main header file for the DEVICECONFIG application
//

#if !defined(AFX_DEVICECONFIG_H__23533310_AC44_4C5E_8E5E_E3DFAB9094D5__INCLUDED_)
#define AFX_DEVICECONFIG_H__23533310_AC44_4C5E_8E5E_E3DFAB9094D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ScfList.h"

class CEquipmentDialog : public CDialog
{
public:
	explicit CEquipmentDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	explicit CEquipmentDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	virtual void writeChanges(void);
	virtual BOOL readOptions(void);

	CString csDeviceName;
	CString csDeviceNameOption;

	int iDsiPdcxAvailable;
	int iDsiEmvUsAvailable;

	CScfList::DevListMap *devMapDsiPdcx;  // see https://www.codeproject.com/Articles/13458/CMap-How-to
	CScfList::DevListMap *devMapDsiEmvUs;  // see https://www.codeproject.com/Articles/13458/CMap-How-to

};

/////////////////////////////////////////////////////////////////////////////
// CDeviceConfigApp:
// See DeviceConfig.cpp for the implementation of this class
//

class CDeviceConfigApp : public CWinApp
{
public:
	CDeviceConfigApp();

	TCHAR  m_BiosSerialNumber[64];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceConfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDeviceConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICECONFIG_H__23533310_AC44_4C5E_8E5E_E3DFAB9094D5__INCLUDED_)
