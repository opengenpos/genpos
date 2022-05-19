/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Window Refresh Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosRefre.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
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
#include <tchar.h>
#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#define  __EventSubSystem
#include "vos.h"
#include "vosl.h"


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
** Synopsis:    VOID VosRefresh(VOID);
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosRefresh(VOID)
{
    if (! (usVosConfig & VOS_CONFIG_INIT)) {/* not initialization */
        PifAbort(MODULE_VOS_REFRESH, FAULT_BAD_ENVIRONMENT);
    }

    /* --- check operator display type --- */

    if (! (usVosConfig & VOS_CONFIG_LCD)) { /* not LCD            */
        return;                             /* exit ...           */
    }

    PifRequestSem(usVosApiSem);

    VosExecInit(VosVideo.usMode,            /* video mode         */
                VosVideo.usMinutes);        /* back light         */
    VosExecRefresh();                       /* display all window */

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    VOID VosExecRefresh(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecRefresh(VOID)
{
    SHORT       sWin;
    USHORT      usHandle;
    VOSWINDOW   FAR *pWin;

    /* --- check number of window --- */

    if (! usVosNumWindow) {                 /* not exist window       */
        return;
    }

    /* --- refresh all window --- */

    for (sWin = usVosNumWindow - 1; sWin >= 0; sWin--) {
        if ((usHandle = ausVosQueue[sWin]) == 0) {  /* free handle    */
            continue;                               /* next ...       */
        }

        pWin = &aVosWindow[usHandle - 1];   /* set window address     */

        VosDisplayPhyWindow(pWin);          /* display window         */
        VosExecCursor(pWin);                /* set cursor status      */
    }
}


/* ======================================= */
/* ========== End of VosRefre.C ========== */
/* ======================================= */
