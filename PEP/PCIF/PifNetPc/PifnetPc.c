// Pifnet.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

/* --- global haeders --- */
#if defined(SARATOGA) 
# include   <windows.h>                         /* Windows header           */
# include   "r20_pif.h"
# include   "netsem.h"
#elif defined(SARATOGA_PC)
# include   <windows.h>                         /* Windows header           */
//# include   "pif.h"
# include   "r20_pif.h"
#elif   defined (POS7450)                       /* 7450 POS model ?             */
# include   <phapi.h>                           /* PharLap header           */
#elif defined (POS2170)                     /* 2170 POS model ?             */
# include   "ecr.h"                             /* ecr common header        */
#elif defined (SERVER_NT)                   /* WindowsNT model ?            */
# include   <windows.h>                         /* Windows header           */
#endif
/* --- C-library header --- */

#include    <memory.h>                      /* memxxx() C-library header    */
#include    <stdarg.h>                      /* va_arg() etc.                */

/* --- our common headers --- */

#include    "piftypes.h"                    /* type modifier header         */
#include    "pifnet.h"                      /* PifNet common header         */
#include    "netmodul.h"                    /* my module header             */

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
#if defined(SARATOGA) 
            NetSemInitialize();
#endif
            break;
        case DLL_PROCESS_DETACH:
#if defined(SARATOGA) 
            NetDeleteSemAll();
#endif
            NetFinalize();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

