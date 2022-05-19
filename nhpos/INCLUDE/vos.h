/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Grobal Header File
* Category    : Viode Output Service, 2170 System Application
* Program Name: VOS.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
* Oct-31-95  G3        O.Nakada     Added define for VOS_CBACK().
*** 2172 ***
* Oct-06-99  1.00      K.Iwata      Initial (2172 for Win32)
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
#ifndef _INC_VOS
#define _INC_VOS

#ifdef __cplusplus
extern "C" {
#endif


/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
#define VOS_SUCCESS         0
#define VOS_ERROR           (-1)

#define VOS_DISABLE         0
#define VOS_ENABLE          1

/* --- video mode --- */
#define VOS_MODE_TEXT4025   1
#define VOS_MODE_TEXT8025   3

/* --- back light --- */
#define VOS_BLIGHT_OFF      0
#define VOS_BLIGHT_ON       1

/* --- font type --- */
#define VOS_FONT_NORMAL     0

/* --- character or background color --- */
#define VOS_CBACK(BC)       (BC << 4)
#define VOS_C_BLACK         0x00
#define VOS_C_WHITE         0x01/*0x07*/ //colorpalette-changes
#define VOS_A_BLINK         0x80

/* --- window border --- */
#define VOS_B_NONE          0
#define VOS_B_BOX           0x000F
#define VOS_B_TOP           0x0001
#define VOS_B_BOTTOM        0x0002
#define VOS_B_LEFT          0x0004
#define VOS_B_RIGHT         0x0008
#define VOS_B_SINGLE        0x0010
#define VOS_B_DOUBLE        0x0020
#define VOS_B_CENTER        0x0100

/* --- cursor state --- */
#define VOS_VISIBLE         1
#define VOS_INVISIBLE       0
#define VOS_ERRORVISIBLE	-1

/* --- window type --- */
#define VOS_WIN_OVERLAP     1
#define VOS_WIN_NORMAL      2


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

#ifdef __EventSubSystem /* ### ADD (2172 for Win32) V1.0 */
    #define _DLLSPEC __declspec(dllexport)
#else
    #define _DLLSPEC _declspec(dllimport)
#endif /* __EventSubSystem */

typedef struct {
	USHORT usRow;
	USHORT usCol;
	USHORT usNumRow;
	USHORT usNumCol;
} VOSRECT;

typedef struct _tagVOSPARAM {
	struct _tagVOSPARAM  *pNextParam;
	ULONG  ulDisplayFlags;
	USHORT usRow;
	TCHAR  tchVosString[64];
} VOSPARAM;

_DLLSPEC VOID    VosInitialize(USHORT usMinutes);
_DLLSPEC USHORT  VosSetBLight(USHORT usState);
_DLLSPEC USHORT  VosSetBLightOff(USHORT usMinutes);
_DLLSPEC USHORT  VosSetMode(USHORT usMode);
_DLLSPEC VOID    VosRefresh(VOID);
_DLLSPEC VOID    VosSetWndHasButtons(USHORT usHandle, USHORT usHasButtons);
_DLLSPEC VOID    VosGetWndTxtSize(USHORT usHandle, USHORT *usParmResult);
_DLLSPEC VOID    VosCreateWindow(USHORT usRow, USHORT usCol, USHORT usNumRow,
                        USHORT usNumCol, USHORT usFont, UCHAR uchAttr,
                        USHORT usBorder, USHORT *pusHandle);
_DLLSPEC VOID    VosCreateWindowRect(VOSRECT *pVosRect, USHORT usFont, UCHAR uchAttr,
                     USHORT usBorder, USHORT *pusHandle);
_DLLSPEC VOID    VosDeleteWindow(USHORT usHandle);
_DLLSPEC VOID    VosBorderString(USHORT usHandle, TCHAR *puchString, USHORT usLength,
                        UCHAR uchAttr, USHORT usBorder);
_DLLSPEC USHORT  VosSetWinType(USHORT usHandle, USHORT usType);

_DLLSPEC SHORT  VosSetCurType(USHORT usHandle, USHORT usVisible);
_DLLSPEC USHORT  VosSetBlink(USHORT usHandle, USHORT usState);
_DLLSPEC VOID    VosSetCurPos(USHORT usHandle, USHORT usRow, USHORT usColumn);
_DLLSPEC VOID    VosGetCurPos(USHORT usHandle, USHORT *pusRow, USHORT *pusColumn);

_DLLSPEC VOID    VosScrollDown(USHORT usHandle, USHORT usNumRow);
_DLLSPEC VOID    VosScrollDownAtRow(USHORT usHandle, USHORT usNumber, USHORT usRowFirst);
_DLLSPEC VOID    VosScrollUp(USHORT usHandle, USHORT usNumRow);
_DLLSPEC VOID    VosScrollUpAtRow(USHORT usHandle, USHORT usNumber, USHORT usRowFirst);
_DLLSPEC VOID    VosCls(USHORT usHandle);

_DLLSPEC VOID    VosPrintf(USHORT usHandle, CONST TCHAR  *pszFormat, ...);
_DLLSPEC VOID    VosPrintfAttr(USHORT usHandle, UCHAR uchAttr, CONST TCHAR *pszFormat, ...);
_DLLSPEC VOID    VosString(USHORT usHandle, TCHAR *puchString, USHORT usLength);
_DLLSPEC VOID    VosStringAttr(USHORT usHandle, TCHAR *puchString, USHORT usLength,
                      UCHAR uchAttr);
_DLLSPEC VOID	 VosStringAttrColumn(USHORT usHandle, TCHAR *puchString, USHORT usLength,
					  ULONG *plAttr);
_DLLSPEC  VOID VosShowWindow(USHORT usHandle);
_DLLSPEC VOID    VosDisplayAttr(USHORT usHandle, USHORT usRow, USHORT usCol,
                       TCHAR *puchString, USHORT usLength);

_DLLSPEC VOID	 VosStringEdit(USHORT usHandle, TCHAR *puchString, USHORT usLength, USHORT usType);

#ifdef __cplusplus
}
#endif

#endif                                      /* _INC_VOS */
/* ================================== */
/* ========== End of VOS.H ========== */
/* ================================== */
