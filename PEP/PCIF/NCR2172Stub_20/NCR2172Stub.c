/////////////////////////////////////////////////////////////////////////////
// NCR2170Stub.c : 
//
// Copyright (C) 1998 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Jun-11-1998                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>


/////////////////////////////////////////////////////////////////////////////
//	Local Definition
/////////////////////////////////////////////////////////////////////////////

// global data

CRITICAL_SECTION	g_CriticalSection;		// reserved

// function prototypes

VOID	CstInitialize(VOID);				// for STUB16 - PCSTBCOM.C
VOID	CstUninitialize(VOID);				// for STUB16 - PCSTBCOM.C


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
		CstInitialize();
		break;
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&g_CriticalSection);
		CstUninitialize();
		break;
	default:
		break;
	}

	// exit ...

	return TRUE;
}

#ifdef junkyjunk

// These were imported here for missing externals.
// commentted out lines in PEP.C instead.
/*
/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   SHORT       PifWinGetLastError();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////
*/

SHORT PIFENTRY PifWinGetLastError()
{
    return m_sGetLastError;
}

/*
/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   SHORT       StubWinGetLastError();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////
*/

SHORT STUBENTRY StubWinGetLastError()
{
    return m_sGetLastError;
}

#endif
///////////////////////////////	 End of File  ///////////////////////////////
