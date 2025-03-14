/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP Action Code Init
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   ACTINIT.C
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
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
#include    <Windows.h>

#include    "action.h"
#include    "actinit.h"

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
**  Synopsis    :   int WINAPI  LibMain()
*
**  Input       :   HANDLE  hInst;
*                   WORD    wDS;
*                   WORD    wHeapSize;
*                   LPSTR   lpszCmd;
*
**  Return      :   TRUE;
*
**  Description:
*       This procedure called by LinEntry when this DLL is Loaded
* ===========================================================================
*/
#ifdef junkyjunk
int WINAPI LibMain(HANDLE hInst, WORD wDS, WORD wHeapSize, LPSTR lpszCmd)
{
    if (wHeapSize) {        /* LibEntry() has called LocalInit() */

        /* ----- Unlock default data segment ----- */

//        UnlockData(NULL);

    }

    return TRUE;

    hInst   = hInst;
    wDS     = wDS;
    lpszCmd = lpszCmd;
}

#else

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif
/* ===== End of ACTINIT.C ===== */
