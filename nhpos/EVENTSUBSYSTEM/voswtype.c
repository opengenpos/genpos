/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Set Window Type Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosWType.C
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
* Jun-20-95  G0        O.Nakada     Initial
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
** Synopsis:    VOID VosSetWinType(USHORT usHandle, USHORT usType);
*     Input:    usHandle - user handle of window
*               usType   - window type
*
** Return:      previous window type
*===========================================================================
*/
__declspec( dllexport ) USHORT VosSetWinType(USHORT usHandle, USHORT usType)
{
    USHORT      usReal, usPrevious;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SETWINTYPE, FAULT_INVALID_ENVIRON_1);
		return 0;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SETWINTYPE, FAULT_INVALID_ENVIRON_2);
		return 0;
    }

    /* --- get current window type --- */

    usPrevious = (pWin->usState & VOS_STATE_WNORMAL) ?
                 (VOS_WIN_NORMAL) : (VOS_WIN_OVERLAP);

    /* --- set new blink state --- */

    if (usType == VOS_WIN_NORMAL) {         /* normal window     */
        pWin->usState |=  VOS_STATE_WNORMAL;
    } else {                                /* overlap window    */
        pWin->usState &= ~VOS_STATE_WNORMAL;
    }

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/* ======================================= */
/* ========== End of VosWType.C ========== */
/* ======================================= */
