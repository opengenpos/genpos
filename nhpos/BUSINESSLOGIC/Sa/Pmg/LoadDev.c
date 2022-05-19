/*
*===========================================================================
*
*   Copyright (c) 1999-, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       :
* Category    : Print Manager, 2172 System Application
* Program Name: LoadDevice.c
* --------------------------------------------------------------------------
* Compiler    : MS-VC++ Ver.6.00 by Microsoft Corp.
* Options     :
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                LoadDevice()  -
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Dec-04-99  01.00.00  T.Koyama	    Initial
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
#include "LoadDev.h"
#include <unicodeFixes.h>


/*
*===========================================================================
*   Local Definitions
*===========================================================================
*/

static	HMODULE		hDll;
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
** Synopsis:    BOOL LoadDevice(LPCTSTR	lpszName, PLOAD_FUNCTION pLoadFunction)
*
*     Input:    lpszName
*				pLoadFunction
*
*    Output:    TRUE/FALSE
*
** Return:
*===========================================================================
*/
BOOL LoadDevice(LPCTSTR	lpszName, PLOAD_FUNCTION pLoadFunction)
{
	BOOL		fCompleted = TRUE;		// assume good status
	FARPROC	*	ppfnProc;
	PPROCADR	pTable;

	// declare procedure of System Monitor

	static	PROCADR		afnDlls[] = { {  1, TEXT("Open")				},
									  {  2, TEXT("Close")				},
									  {  3, TEXT("Write")				},
									  {  4, TEXT("Read")				},
									  {  5, TEXT("IoControl")			},
									  {  6, TEXT("GetDeviceName")		},
									  {  7, TEXT("GetDeviceCapability")	},
									  {  8, TEXT("GetParameter")		},
									  {  9, TEXT("SetParameter")		},
									  {  0, NULL					},
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
			ppfnProc = (FARPROC *)&pLoadFunction->Open;
			break;
		case 2:
			ppfnProc = (FARPROC *)&pLoadFunction->Close;
			break;
		case 3:
			ppfnProc = (FARPROC *)&pLoadFunction->Write;
			break;
		case 4:
			ppfnProc = (FARPROC *)&pLoadFunction->Read;
			break;
		case 5:
			ppfnProc = (FARPROC *)&pLoadFunction->IoControl;
			break;
		case 6:
			ppfnProc = (FARPROC *)&pLoadFunction->GetDeviceName;
			break;
		case 7:
			ppfnProc = (FARPROC *)&pLoadFunction->GetDeviceCapability;
			break;
		case 8:
			ppfnProc = (FARPROC *)&pLoadFunction->GetParameter;
			break;
		case 9:
			ppfnProc = (FARPROC *)&pLoadFunction->SetParameter;
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
		hDll = NULL;

		return FALSE;						// exit ...
	}

	// exit ...

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    VOID FreeDevice(VOID)
*
*     Input:    lpszName
*				pLoadFunction
*
*    Output:    TRUE/FALSE
*
** Return:
*===========================================================================
*/
VOID FreeDevice(VOID)
{
	if (hDll != NULL)
	{
		FreeLibrary(hDll);
	}

	hDll = NULL;
}

/* ===================================== */
/* ========== End of LoadDev.c ======== */
/* ===================================== */
