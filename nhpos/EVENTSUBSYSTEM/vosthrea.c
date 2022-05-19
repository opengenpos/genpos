/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Automatic Back Light Control Thread
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosThrea.C
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
** Synopsis:    VOID THREADENTRY VosThread(VOID);
*
** Return:      nothing
*===========================================================================
*/
#if 0

// RJC - Removed this function for now as the code where it was
//       being used is now commented out.

VOID THREADENTRY VosThread(VOID)
{
    ULONG   ulProgress = 0, ulOff = 0;

    for (;;) {                          /* endless loop */

    PifSleep(VOS_SLEEP_THREAD);

    /* --- check state of back light        --- */
    /* --- first time VOS_STATE_CHANGE = on --- */

    if (VosVideo.usState & VOS_STATE_CHANGE) {  /* changed parameter */
        VosVideo.usState &= ~VOS_STATE_CHANGE;  /* reset             */
        ulProgress = 0U;
        ulOff      = (ULONG)VosVideo.usMinutes * 60000U /
                     (ULONG)VOS_SLEEP_THREAD;
        continue;                           /* next ...              */
    }

    /* --- check progress time --- */

    if (ulOff == 0 ||                       /* disable automatic off */
        ++ulProgress < ulOff) {             /* not timeout           */
        continue;                           /* next ...              */
    }

/* ### DEL 2172 Rel1.0   VosSetBLight(VOS_BLIGHT_OFF); */          /* back light off        */

    }                                   /* endless loop */
}

#endif

/* ======================================= */
/* ========== End of VosThrea.C ========== */
/* ======================================= */
