/////////////////////////////////////////////////////////////////////////////
// NCR2170Pif.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Jun-12-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "pif32.h"
#include "debug.h"

#pragma	pack(push, 1)
#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "R20_PIF.H"     /* Portable Platform Interface */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */
#pragma	pack(pop)


/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////

// global data

CRITICAL_SECTION	g_CriticalSection;		// reserved


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Function:	
//
// Prototype:	BOOL WINAPI	DllMain(hinstDLL, dwReason, lpReserved);
//
// Inputs:		HINSTANCE	hinstDLL;		-
//				DWORD		dwReason;		-
//				LPVOID		lpReserved;		-
//
// Outputs:		BOOL		fResult;		-
//
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&g_CriticalSection);
		DebugInit();
		DebugPrintf(_T("========== Start ==========\r\n"));
		PifInit();
		XinuSysInit();					// initialize device
		PifXinuInit(FALSE);				// initialize Platform Interface
		break;
	case DLL_PROCESS_DETACH:
		PifUninit();
		DebugPrintf(_T("========== Stop ==========\r\n"));
		DebugUninit();
		DeleteCriticalSection(&g_CriticalSection);
		break;
	default:
		break;
	}

	// exit ...

	return TRUE;
}


///////////////////////////////	 End of File  ///////////////////////////////
