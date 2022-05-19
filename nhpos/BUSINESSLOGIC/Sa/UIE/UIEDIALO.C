/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1993       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Dialog Function Module
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIEDIALO.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Meduim/Large Model
* Options     : /c /A[M|Lw] /W4 /G[1|2]s /Os /Zap
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
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
#include <tchar.h>
#include <stdio.h>
#include <shellapi.h>

#include "ecr.h"
#include "paraequ.h"
#include "log.h"
#include "pif.h"
#include "uie.h"
#include "uireg.h"
#include "uiel.h"
#include "uiefunc.h"
#include "uieseq.h"
#include "uiedialo.h"
#include "display.h"
#include "rfl.h"


/*
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieCreateDialog(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID DLLENTRY UieCreateDialog(VOID)
{

    UieSetModeS2D();                            /* Backup to Current Mode */

    if (iuchUieFuncSts == UIE_NORMAL) {         /* Normal Function */
        iuchUieFuncSts = UIE_DIALOG;            /* Set to Dialog Function */
	} else if (iuchUieFuncSts == UIE_DIALOG) {    /* Normal Function */
        iuchUieFuncSts = UIE_DIALOG_SUB;        /* Set to Dialog Function */
    } else {                                    /* Dialog nesting level limit reached */
		NHPOS_ASSERT_TEXT((iuchUieFuncSts == UIE_DIALOG), "Dialog nesting level reached.");
        PifAbort(MODULE_UIE_CDIALOG, FAULT_INVALID_FUNCTION);
    }

    iuchUieRootFunc[iuchUieFuncSts]    = UIE_LINK_EMPTY;
    iuchUieActiveFunc[iuchUieFuncSts]  = UIE_LINK_EMPTY;
    iuchUieCurrentFunc[iuchUieFuncSts] = UIE_LINK_EMPTY;

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;	/* Set to Clear Retry Buffer Flag */	
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieDeleteDialog(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID DLLENTRY UieDeleteDialog(VOID)
{
    UCHAR iuchRoot = iuchUieRootFunc[iuchUieFuncSts];

    if (iuchUieFuncSts == UIE_DIALOG) {                 /* Dialog Function */
        if (iuchRoot != UIE_LINK_EMPTY) {     
            UieDeleteFunc(iuchRoot, UIE_LINK_EMPTY);    /* Delete to All Dialog Function */
        }
        iuchUieFuncSts = UIE_NORMAL;                    /* Set to Normal Function */
	} else if (iuchUieFuncSts == UIE_DIALOG_SUB) {                 /* Dialog Function */
        if (iuchRoot != UIE_LINK_EMPTY) {     
            UieDeleteFunc(iuchRoot, UIE_LINK_EMPTY);    /* Delete to All Dialog Function */
        }
        iuchUieFuncSts = UIE_DIALOG;                    /* Set to Normal Function */
    } else {                                            /* Normal Function */
		NHPOS_ASSERT_TEXT((iuchUieFuncSts == UIE_DIALOG), "Dialog Un-nesting level reached.");
        PifAbort(MODULE_UIE_DDIALOG, FAULT_INVALID_FUNCTION);
    }

    UieSetModeD2S();                                    /* Restore to Previous Mode */

	UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;			/* Set to Clear Retry Buffer Flag */	
}

static TCHAR *tcFindShortCutList (TCHAR *ptcPrefix, TCHAR *ptcBuffer, int nBufferChars)
{
	WIN32_FIND_DATA myFoundFile;
	HANDLE fileSearch;
	BOOL doit = TRUE;
	TCHAR *ptcBufferTemp = ptcBuffer;

	memset(ptcBuffer, 0x00, sizeof(TCHAR) * nBufferChars);

	//we need to change to this directory to look for the files
	_tchdir(STD_FOLDER_DATABASEFILES);

	fileSearch = FindFirstFile (ptcPrefix, &myFoundFile); //find the first file

	//this while loop will continue as long as there are files in the directory
	while (doit && fileSearch != INVALID_HANDLE_VALUE) 
	{
		if (nBufferChars - _tcslen(myFoundFile.cFileName) > 0) {
			_tcscpy (ptcBuffer, myFoundFile.cFileName);
			ptcBuffer += _tcslen (ptcBuffer) + 1;
		}
		else {
			break;
		}
		//find the next file, if there is not one, doit will be FALSE
		doit = FindNextFile (fileSearch, &myFoundFile);
	}
	
	FindClose (fileSearch);
	if (ptcBuffer == ptcBufferTemp) {
		ptcBufferTemp = 0;
	}
	return ptcBufferTemp;
}

SHORT UieShellExecuteFile (TCHAR *aszPathAndName)
{
	SHORT       sRetStatus;
	HINSTANCE   hStatus;
	char xBuff[128];
	int  iLen = _tcslen (aszPathAndName);

	if (iLen < 40)
		iLen = 0;
	else
		iLen -= 40;

	// Documentation on the ShellExecute() Windows API function says the following:
	//   If the function succeeds, it returns a value greater than 32. If the
	//   function fails, it returns an error value that indicates the cause of
	//   the failure.
	hStatus = ShellExecute (NULL, _T("open"), aszPathAndName, NULL, NULL, SW_SHOWNORMAL);
	if ((int)hStatus == ERROR_FILE_NOT_FOUND || (int)hStatus == ERROR_PATH_NOT_FOUND) {
		//  Same error codes as SE_ERR_FNF and SE_ERR_PNF
		sprintf (xBuff, "==WARNING: UieShellExecuteFile(): Not found %S", aszPathAndName + iLen);
		NHPOS_NONASSERT_NOTE("==WARNING", xBuff);
	} else if ((int)hStatus == SE_ERR_NOASSOC) {
		sprintf (xBuff, "==WARNING: UieShellExecuteFile(): No application associated with %S", aszPathAndName + iLen);
		NHPOS_NONASSERT_NOTE("==WARNING", xBuff);
	} else if ((int)hStatus > 32) {
		sprintf (xBuff, "==NOTE: UieShellExecuteFile(): Launched %S", aszPathAndName + iLen);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);
	} else {
		sprintf (xBuff, "==WARNING: UieShellExecuteFile(): Error status %S %d", aszPathAndName + iLen, (int)hStatus);
		NHPOS_NONASSERT_NOTE("==WARNING", xBuff);
	}

	sRetStatus = UIF_SUCCESS;

	return sRetStatus;
}

SHORT UieShellExecuteFileMonitorThread (TCHAR **PointerArray)
{
	TCHAR       myPathBuffer[256] = {0};
	TCHAR       myCommandLine[256] = {0};
	SHORT       sRetStatus;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

	if ((PointerArray[0] != 0) && (PointerArray[1] != 0)) {
		_tcsncpy (myPathBuffer, PointerArray[0], 255);
		_tcsncpy (myCommandLine, PointerArray[1], 255);

		while (1) {
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );

			// Start the child process. 
			if( !CreateProcess( myPathBuffer,  // No module name (use command line). 
				myCommandLine,                 // Command line. 
				NULL,             // Process handle not inheritable. 
				NULL,             // Thread handle not inheritable. 
				FALSE,            // Set handle inheritance to FALSE. 
				0,                // No creation flags. 
				NULL,             // Use parent's environment block. 
				NULL,             // Use parent's starting directory. 
				&si,              // Pointer to STARTUPINFO structure.
				&pi )             // Pointer to PROCESS_INFORMATION structure.
			) 
			{
				DWORD  dwLastError;

				dwLastError = GetLastError();

				{
					char xBuff[128];

					sprintf (xBuff, "CreateProcess(): GetLastError()=%d", dwLastError);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

				sRetStatus = UIF_SUCCESS;
				break;
			}

			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE );

			// Close process and thread handles. 
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			PifSleep (50);
		}
	} else {
		NHPOS_ASSERT(PointerArray[0] != 0);
		NHPOS_ASSERT(PointerArray[1] != 0);
	}

	sRetStatus = UIF_SUCCESS;

	return sRetStatus;
}

SHORT UieShellExecuteFileMonitor (TCHAR *aszPathAndName, TCHAR *aszCommandLine)
{
	static USHORT  usIndex = 0;
	static TCHAR       myPathBuffer[5][256] = {0};
	static TCHAR       myCommandLine[5][256] = {0};
	VOID (THREADENTRY *pFuncTemp)(VOID) = UieShellExecuteFileMonitorThread;   /* Add R3.0 */
	CHAR FARCONST   szUifIniStartupThread[] = "PROCMON";       /* Total Update for Join from Disconnected Satellite */
	static TCHAR *PointerArray[5][2] = {0, 0};
	USHORT usIndexTemp;

	usIndex = ((usIndexTemp = usIndex) + 1) % 5;
	_tcsncpy (myPathBuffer[usIndexTemp], aszPathAndName, 255);
	_tcsncpy (myCommandLine[usIndexTemp], aszCommandLine, 255);

	PointerArray[usIndexTemp][0] = &(myPathBuffer[usIndexTemp][0]);
	PointerArray[usIndexTemp][1] = &(myCommandLine[usIndexTemp][0]);

	/* create total update thread */
	PifBeginThread(pFuncTemp,                   /* Start Address */
				   NULL,                        /* Stack Address is not used*/
				   2000,                   /* Stack Size */
				   PRIO_TOTALS,                 /* Priority */
				   szUifIniStartupThread,       /* Name */
				   PointerArray[usIndexTemp]);
	PifSleep(50);    // give up time slice to allow execution to start

	return 0;
}


SHORT UieShelExecuteLinkFile (TCHAR *aszName, UCHAR uchLen)
{
	SHORT        sRetStatus;
	TCHAR        tcBuffer[256];
	TCHAR        cFilePath[256];
	TCHAR        tcPrefixName[64];

	// get the database folder path and add trailing backslash to prep for adding file name.
	_tcscpy (cFilePath, STD_FOLDER_DATABASEFILES);
	_tcscat (cFilePath, _T("\\"));
	sRetStatus = LDT_EQUIPPROVERROR_ADR;

	if (uchLen > 0) {
		int i = 0;

		for (i = 0; i < uchLen; i++) {
			tcPrefixName[i] = aszName[i];
		}
		tcPrefixName[i] = _T('\0');
		_tcscat (tcPrefixName, _T("*.lnk"));

		if (tcFindShortCutList (tcPrefixName, tcBuffer, sizeof(tcBuffer)/sizeof(tcBuffer[0])))
		{
			int i, j = _tcslen(cFilePath);
			for (i = 0; tcBuffer[i]; i++) {
				cFilePath[j + i] = tcBuffer[i];
			}
			cFilePath[j+i] = _T('\0');
			sRetStatus = UieShellExecuteFile (cFilePath);
			PifSleep(50);    // give up time slice to allow execution to start
		}
		else {
			NHPOS_ASSERT_TEXT(0, "File to launch not found.");
		}
	}
	else {
		NHPOS_ASSERT_TEXT(0, "No input with prefix for file to launch.");
	}

	// Clear the leadthru area as we are done.
	RflClearDisplay (0, 0, 0);
	return sRetStatus;
}

/* ---------- End of UIEDIALO.C ---------- */
