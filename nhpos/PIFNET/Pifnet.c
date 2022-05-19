// Pifnet.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

/* --- global haeders --- */
#if defined(SARATOGA) 
# include	<windows.h>							/* Windows header			*/
# include   "pif.h"
# include   "netsem.h"
#elif defined(SARATOGA_PC)
# include	<windows.h>							/* Windows header			*/
#elif	defined (POS7450)						/* 7450 POS model ?				*/
# include	<phapi.h>							/* PharLap header			*/
#elif defined (POS2170)						/* 2170 POS model ?				*/
# include	"ecr.h"								/* ecr common header		*/
#elif defined (SERVER_NT)					/* WindowsNT model ?			*/
# include	<windows.h>							/* Windows header			*/
#endif
/* --- C-library header --- */

#include	<memory.h>						/* memxxx() C-library header	*/
#include	<stdarg.h>						/* va_arg() etc.				*/

/* --- our common headers --- */

#include	"piftypes.h"					/* type modifier header			*/
#include	"pifnet.h"						/* PifNet common header			*/
#include	"netmodul.h"					/* my module header				*/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
#ifdef DEBUG_PIFNET_OUTPUT
    TCHAR  wszDisplay[128];
	DWORD	threadID;
#endif
	switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
#ifdef DEBUG_PIFNET_OUTPUT
			wsprintf(wszDisplay, TEXT("Pifnet.c  DllMain DLL_PROCESS_ATTACH hit.\n"));
			OutputDebugString(wszDisplay);
#endif
#if defined(SARATOGA) 
            NetSemInitialize();
            break;
#endif
        case DLL_PROCESS_DETACH:
#ifdef DEBUG_PIFNET_OUTPUT
			wsprintf(wszDisplay, TEXT("Pifnet.c  DllMain DLL_PROCESS_DETACH hit.\n"));
			OutputDebugString(wszDisplay);
#endif

#if defined(SARATOGA) 
            NetDeleteSemAll();
            break;
#endif
        case DLL_THREAD_ATTACH:
#ifdef DEBUG_PIFNET_OUTPUT
			threadID = GetCurrentThreadId();
			wsprintf(wszDisplay, TEXT("Pifnet.c  DllMain DLL_THREAD_ATTACH hit. Thread:%x\n"), threadID);
			OutputDebugString(wszDisplay);
#endif
			break;
        case DLL_THREAD_DETACH:
#ifdef DEBUG_PIFNET_OUTPUT
			threadID = GetCurrentThreadId();
			wsprintf(wszDisplay, TEXT("Pifnet.c  DllMain DLL_THREAD_DETACH hit. Thread:%x\n"), threadID);
			OutputDebugString(wszDisplay);
#endif
			break;
    }
    return TRUE;
}

