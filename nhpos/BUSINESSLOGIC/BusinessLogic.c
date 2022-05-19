/*
*===========================================================================
*
*   Copyright (c) 1999-, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       : 
* Category    : Buisiness Logic, 2172 System Application
*             : MS-Win32 Dependent module definitions
* Program Name: blwindep.c (Business Logic WINdows DEPendent module)
* --------------------------------------------------------------------------
* Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                 DllMain()     - set conversion table of user mode
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Oct-01-99  01.00.00  M.Teraki	    Initial
*
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

/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include "pif.h"

/*
*===========================================================================
*   Local Definitions
*===========================================================================
*/

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    BOOL WINAPI DllMain(HINSTANCE hinstDll,
*				    DWORD fdwReason,
* 				    LPVOID lpvReserved)
*
*     Input:    See Win32 API Documents
*
*    Output:    See Win32 API Documents
*
**   Return:    See Win32 API Documents
*===========================================================================
*/
BOOL WINAPI
DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
      case DLL_PROCESS_ATTACH:
	  /*
	   * DLL is attaching to the address space of the current process.
	   */
	  break;

      case DLL_THREAD_ATTACH:	/* A new thread is being created in the current process. */
      case DLL_THREAD_DETACH:	/* A thread is exiting cleanly. */
      case DLL_PROCESS_DETACH:
	  /*
	   * The calling process is detaching the DLL from its address space.
	   */
	  break;
    }
    return (TRUE);
}

/* ===================================== */
/* ========== End of blwindep.c ======== */
/* ===================================== */
