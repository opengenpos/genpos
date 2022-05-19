/*************************************************************************
 *
 * UIELDEV.c
 *
 * $Workfile:: UIELDEV.c                                                 $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: UIELDEV.c                                                  $
 *
 ************************************************************************/

#include <windows.h>
#include <unicodeFixes.h>
#ifndef _WIN32_WCE
#include <tchar.h>
#endif
#include "UIELDEV.h"



/*
*===========================================================================
*   Local Definitions
*===========================================================================
*/

typedef struct _PROCADR {				// array of procedure address
	INT			iIndex;						// index to control
	LPCTSTR		lpName;						// ptr. to a proc. name
} PROCADR, *PPROCADR;


/*
*===========================================================================
** Synopsis:    BOOL UieLoadDevice()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/

BOOL UieLoadDevice(LPCTSTR lpszName, PLOAD_FUNCTION pFunctions, HINSTANCE* phInstance)
{
	BOOL		fCompleted = TRUE;		// assume good status
	HINSTANCE	hDll;
	FARPROC	*	ppfnProc;
	PPROCADR	pTable;

	// declare procedure of System Monitor

	static	PROCADR		afnDlls[] = { {  1, _T("Open")					},
									  {  2, _T("Close")					},
									  {  3, _T("Write")					},
									  {  4, _T("Read")					},
									  {  5, _T("IoControl")				},
									  {  6, _T("GetDeviceName")			},
									  {  7, _T("GetDeviceCapability")	},
									  {  8, _T("GetParameter")			},
									  {  9, _T("SetParameter")			},
									  {  0, NULL						},
									};

	// load System Monitor dll

	if (! (hDll = LoadLibrary(lpszName)))
	{
		DWORD	dwRC = GetLastError();
		return FALSE;
	}

	// get each procedure address

	for (pTable = afnDlls; pTable->lpName; pTable++)
	{
		// save the address

		switch (pTable->iIndex)
		{
		case 1:
			ppfnProc = (FARPROC *)&pFunctions->Open;
			break;
		case 2:
			ppfnProc = (FARPROC *)&pFunctions->Close;
			break;
		case 3:
			ppfnProc = (FARPROC *)&pFunctions->Write;
			break;
		case 4:
			ppfnProc = (FARPROC *)&pFunctions->Read;
			break;
		case 5:
			ppfnProc = (FARPROC *)&pFunctions->IoControl;
			break;
		case 6:
			ppfnProc = (FARPROC *)&pFunctions->GetDeviceName;
			break;
		case 7:
			ppfnProc = (FARPROC *)&pFunctions->GetDeviceCapability;
			break;
		case 8:
			ppfnProc = (FARPROC *)&pFunctions->GetParameter;
			break;
		case 9:
			ppfnProc = (FARPROC *)&pFunctions->SetParameter;
			break;
		default:
			break;
		}

        // get the procedure address
#ifdef _WIN32_WCE
		*ppfnProc  = GetProcAddress(hDll, pTable->lpName);
#else
        *ppfnProc  = GetProcAddressUnicode(hDll, pTable->lpName);
#endif
		fCompleted = (*ppfnProc) ? fCompleted : FALSE;
	}

	if (! fCompleted)
	{
		FreeLibrary(hDll);

		return FALSE;						// exit ...
	}

	//

	*phInstance = hDll;

	// exit ...

	return TRUE;
}

/*
*===========================================================================
** Synopsis:    VOID UieFreeDevice()
*
*     Input:
*
*    Output:
*
** Return:
*===========================================================================
*/
VOID UieFreeDevice(HINSTANCE hInstance)
{
	FreeLibrary(hInstance);
}

/* ===================================== */
/* ========== End of LoadDev.c ======== */
/* ===================================== */
