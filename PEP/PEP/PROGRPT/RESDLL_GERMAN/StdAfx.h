// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A854070E_C130_4E82_8106_995FB994F8DA__INCLUDED_)
#define AFX_STDAFX_H__A854070E_C130_4E82_8106_995FB994F8DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#define WINVER_FOR_GENPOS  0x0502          // GenPOS targets Windows XP SP2 and Windows POS Ready 2009 and later.

#ifndef WINVER                             // Allow use of features specific to Windows XP or later.
#define WINVER WINVER_FOR_GENPOS           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT                       // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT WINVER_FOR_GENPOS     // Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS                     // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS WINVER_FOR_GENPOS   // Change this to the appropriate value to target other versions of Windows.
#endif


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A854070E_C130_4E82_8106_995FB994F8DA__INCLUDED_)
