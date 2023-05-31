// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C63E1D4E_91F8_4E10_83C6_C6453AC95EE7__INCLUDED_)
#define AFX_STDAFX_H__C63E1D4E_91F8_4E10_83C6_C6453AC95EE7__INCLUDED_

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


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxcview.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//defines used for Extended FSC Info
#define  STD_MAX_DEPT			9999	// Max Department # 
#define  STD_PRTMOD_MAX			8		// Number of Print Modifiers 8
#define  STD_ADJ_MAX			20		// Number of Adjectives 20
#define  STD_TENDER_MAX			20		// Number of Tenders 20
#define  STD_TOTAL_MAX			20		// Number of Totals 20
#define  STD_FC_MAX				8		// Number of Foreign Currency 8
#define  STD_ADDCHK_MAX			3		// Number of Add Check 3
#define  STD_IDISC_MAX			6		// Number of Item Discounts 6
#define  STD_RDISC_MAX			6		// Number of Regular Discounts 6
#define  STD_TAXMOD_MAX			11		// Number of Tax Modifiers 11
#define  STD_PRTDMD_MAX			5		// Number of Print Demand 3
#define  STD_CHGTIP_MAX			3		// Number of Charge Tips 3
#define  STD_PRECSH_MAX			8		// Number of Preset Cash was 4 now 8 if MDC 82 Bit A on, IDD_P03_PRECASH
#define  STD_DRCTSHIFT_MAX		9		// Number of Direct Shifts 9
#define  STD_CPN_MAX			255		// Number of Coupons 255
#define  STD_CS_MAX				400 	// Number of Control Strings 400
#define  STD_SEAT_MAX			2		// Number of Seat# 2
#define  STD_ASK_MAX			20		// Number of Ask 20
#define  STD_ADJLEV_MAX			5		// Number of Adjective Levels 5
#define  STD_ORDERDEC_MAX		5		// Number of Order Declarations
#define  STD_GIFT_CRD_MAX		3		// Number of Gift Card types 5

//CWindowButtonExt
#define  MAX_BTN_FONT_SZ		48  // increased from '16' to 48 for Release 4.2
#define  MIN_BTN_FONT_SZ		2
#define  MAX_BTN_HEIGHT			20	// increased from '8' to '20' for Release 2.1 - CSMALL
#define	 MIN_BTN_HEIGHT			1
#define  MAX_BTN_WIDTH			20	// increased from '8' to '20' for Release 2.1 - CSMALL
#define	 MIN_BTN_WIDTH			1

//CWindowItemExt
#define  MAX_WND_HEIGHT			51 // maximum window height allows a window to cover entire 'large' resolution screen height - CSMALL
#define	 MIN_WND_HEIGHT			1
#define  MAX_WND_WIDTH			69 // maximum window width allows a window to cover entire 'large' resolution screen width - CSMALL
#define	 MIN_WND_WIDTH			1
#define	 MAX_WND_USRDIS			10
#define  STD_WND_WIDTH			12
#define  STD_WND_HEIGHT			12

//CWindowTextExt
#define  MAX_TXT_HEIGHT			51 // allows control to extend max resolution height - CSMALL
#define	 MIN_TXT_HEIGHT			1
#define  MAX_TXT_WIDTH			69 // allows control to extend max resolution width - CSMALL
#define	 MIN_TXT_WIDTH			1

//CWindowLabelExt
#define  MAX_LBL_FONT_SZ		74
#define  MIN_LBL_FONT_SZ		2
#define  MAX_LBL_HEIGHT			51 // allows control to extend max resolution height - CSMALL
#define	 MIN_LBL_HEIGHT			1
#define  MAX_LBL_WIDTH			69 // allows control to extend max resolution width - CSMALL
#define	 MIN_LBL_WIDTH			1

#define  UNDO_MEMFILE_GROWSIZE	1024;

//Color Defines
#define  RED(x)		(x & 0x000000FF)
#define  GREEN(x)	(x & 0x0000FF00)>>0x08
#define  BLUE(x)	(x & 0x00FF0000)>>0x10
#define  DARKEN(x)	(x & 0x50)<<0x01

#define  DEF_FONT_COLOR				0x00000000					//black
#define  DEF_BTN_COLOR				0x00c0c0c0					//grey
#define  DEF_WND_COLOR				0x00c0c0c0					//grey
#define  DEF_TXT_COLOR				0x00ffffff					//white
#define  SELECTED_GRID				COLORREF RGB(250, 250, 70)	//yellow
#define	 NONSELECTED_GRID			COLORREF RGB(200, 200, 250)	//light blue(LOM screen color)
#define  OUTSIDE_RESOLUTION_GRID	COLORREF RGB(240,240,255)	//lighter blue
#define  SELECTED_CONTROL			COLORREF RGB(250, 0, 0)		//red
#define  NONSELECTED_CONTROL		COLORREF RGB(0, 0, 0)		//black

//NewLayoutView
// The following defines are for the setting of the target screen resolution.  If the maximum possible size
// should change then in addition to adding a new define, the maximum screen size must also be changed
// in the following variables:
//    const int  CWindowDocument::nGridColumns = 128;
//    const int  CWindowDocument::nGridRows = 72;
#define  LAYOUT_SCREENWIDTH				 800
#define  LAYOUT_SCREENHEIGHT			 600
#define  LAYOUT_SCREENWIDTH_HIRES		1024
#define  LAYOUT_SCREENHEIGHT_HIRES		 768
#define  LAYOUT_SCREENWIDTH_1280		1280
#define  LAYOUT_SCREENHEIGHT_1280		1024
#define  LAYOUT_SCREENWIDTH_1440		1440
#define  LAYOUT_SCREENHEIGHT_1440		 900
#define  LAYOUT_SCREENWIDTH_1920		1920
#define  LAYOUT_SCREENHEIGHT_1920		1080
#define  LAYOUT_SCREENWIDTH_1680		1680
#define  LAYOUT_SCREENHEIGHT_1680		1050
#define  LAYOUT_SCREENWIDTH_2560		2560
#define  LAYOUT_SCREENHEIGHT_2560		2048

//Splitter Window Defines
#define LEFT_SIDE     0
#define RIGHT_SIDE    1
#define TOP_SIDE      LEFT_SIDE    
#define BOTTOM_SIDE   RIGHT_SIDE
// ruler types
#define RT_VERTICAL	  0
#define RT_HORIZONTAL 1

// hint information
#define RW_VSCROLL	  1
#define RW_HSCROLL	  2
#define RW_POSITION   3

#define RULER_SIZE		/*16*/25
#define ZOOM_FACTOR		1

#define MAX_WIDTH_LO	800
#define MAX_HEIGHT_LO	600

#define MAX_WIDTH_HI	1028
#define MAX_HEIGHT_HI	764
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C63E1D4E_91F8_4E10_83C6_C6453AC95EE7__INCLUDED_)
