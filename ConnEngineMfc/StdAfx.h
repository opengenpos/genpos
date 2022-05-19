// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AEEFC232_FB92_488F_B3D6_E1975AAB5F84__INCLUDED_)
#define AFX_STDAFX_H__AEEFC232_FB92_488F_B3D6_E1975AAB5F84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(GENPOSBUILD)
#include "versioninfo.h"
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
// See Using the Windows Headers  https://msdn.microsoft.com/library/windows/desktop/aa383745
// See Modifying WINVER and _WIN32_WINNT https://msdn.microsoft.com/en-us/library/6sehtctf.aspx
//    Windows XP       0x0501
//    Windows XP SP2   0x0502
//    Windows 7        0x0601
//    Windows 8        0x0602
//    Windows 8.1      0x0603
//    Windows 10       0x0A00
#if !defined(WINVER_FOR_GENPOS)
#define WINVER_FOR_GENPOS  0x0601          // GenPOS targets Windows 7 and Windows POS Ready 7 and later.
#endif

#ifndef WINVER                             // Allow use of features specific to Windows XP or later.
#define WINVER WINVER_FOR_GENPOS           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT                       // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT WINVER_FOR_GENPOS     // Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS                     // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS WINVER_FOR_GENPOS   // Change this to the appropriate value to target other versions of Windows.
#endif


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AEEFC232_FB92_488F_B3D6_E1975AAB5F84__INCLUDED_)
