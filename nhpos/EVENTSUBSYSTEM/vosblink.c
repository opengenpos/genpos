/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Set Blink Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosBlink.C
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
** Synopsis:    USHORT VosSetBlink(USHORT usHandle, USHORT usState);
*     Input:    usHandle - user handle of window
*               usState  - blink state
*
** Return:      previous blink state
*===========================================================================
*/
__declspec( dllexport ) USHORT VosSetBlink(USHORT usHandle, USHORT usState)
{
    USHORT      usReal, usPrevious;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SETBLINK, FAULT_INVALID_ENVIRON_1);
		return (VOS_BLIGHT_OFF);
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SETBLINK, FAULT_INVALID_ENVIRON_2);
		return (VOS_BLIGHT_OFF);
    }

    /* --- get current blink state --- */

    usPrevious = (pWin->uchAttr & VOS_A_BLINK) ?
                 (VOS_ENABLE) : (VOS_DISABLE);

    /* --- set new blink state --- */

    if (usState == VOS_ENABLE) {            /* blink attribute    */
        pWin->uchAttr |= VOS_A_BLINK;
    } else {                                /* normal attribute   */
        pWin->uchAttr &= ~VOS_A_BLINK;
    }

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/* ======================================= */
/* ========== End of VosBlink.C ========== */
/* ======================================= */
