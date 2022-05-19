/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Evs DLL Main
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsInit.c
* --------------------------------------------------------------------------
* Compiler    : VC++ Ver. 6.00  by Microsoft Corp.                         
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : initial (for 2172)
*           :          :            :
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

#include <windows.h>

#define  __EVENT_SUB_SYSTEM__DLLMAIN
#include "Evs.h"
#include "EvsL.h"

BOOL WINAPI DllMain(HANDLE hDll,DWORD  dwReason, LPVOID lpReserved){
    switch(dwReason){
        case DLL_PROCESS_ATTACH:
            /* init EventSubSystem */
            __EvsInit();
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }
    return TRUE;
}

/* ======================================================================== */
