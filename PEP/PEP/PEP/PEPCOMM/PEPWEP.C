/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Common WEP
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEPWEP.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
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
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <windows.h>

#include    "pepcomm.h"
#include    "pepwep.h"

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
* ===========================================================================
**  Synopsis:   int WINAPI  WEP();
*
**  Input   :   int nParam;
*
**  Return  :   TRUE;
*
**  Description:
*       This procedure called by Windows when this DLL is unloaded.
* ===========================================================================
*/
#ifdef junkyjunk
int WINAPI WEP(int nParam)
{
    return TRUE;

    PEP_REFER(nParam);
}
#endif
/* ===== End of WEP.C ===== */
