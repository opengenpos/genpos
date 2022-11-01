/*
* ---------------------------------------------------------------------------
* Title         :   Public Header File of LAyout Module
* Category      :   LAYOUT, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   LAYOUT.H
* Copyright (C) :   1996, NCR Corp. E&M OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi

* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Mame     : Description
* Nov-24-03 : 02.00.00 : J. Hall    : Initial (2.0.0)
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
#ifndef LAYOUT_INCLUDED_DEFINED
#define LAYOUT_INCLUDED_DEFINED

#define	IDS_LAYOUT					24087
#define IDS_LAYOUT_ERR_EXEC       (IDS_LAYOUT + 1)
#define IDS_LAYOUT_ERR_LOAD       (IDS_LAYOUT + 2)
#define IDS_LAYOUT_ERR_PEP        (IDS_LAYOUT + 3)
#define IDS_LAYOUT_ERR_NAME       (IDS_LAYOUT + 4)
#define IDS_LAYOUT_ERR_PATH		(IDS_LAYOUT + 5)

#define IDS_LAYOUT_FILTER       (IDS_LAYOUT + 7)
#define IDS_LAYOUT_FILE         (IDS_LAYOUT + 8)
#define IDS_LAYOUT_CAP_FILE     (IDS_LAYOUT + 9)
#define IDS_LAYOUT_CAP_EXEC     (IDS_LAYOUT + 10)
#define IDS_LAYOUT_EXT          (IDS_LAYOUT + 11)
#define IDS_LAYOUT_TOUCH_ERR	(IDS_LAYOUT + 12)
#define IDS_LAYOUT_KEYBOARD_ERR	(IDS_LAYOUT + 13)

BOOL    LayoutExec(HANDLE, HWND, WCHAR*, LANGID); //ESMITH LAYOUT
BOOL	LayoutFile(HANDLE, HWND, WCHAR*, UINT); //ESMITH LAYOUT
BOOL	CurrentLayoutFile(HANDLE, HWND);


#endif
/* ===== End of File ===== */
