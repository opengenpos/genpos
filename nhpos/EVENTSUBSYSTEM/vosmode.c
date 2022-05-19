/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Set Video Mode Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosMode.C
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
** Synopsis:    USHORT VosSetMode(USHORT usMode);
*     Input:    usMode - video mode
*
** Return:      previous mode
*===========================================================================
*/
__declspec( dllexport ) USHORT VosSetMode(USHORT usMode)
{
    USHORT  usPrevious;

    if (! (usVosConfig & VOS_CONFIG_INIT)) {    /* not initialization */
        PifAbort(MODULE_VOS_SETMODE, FAULT_BAD_ENVIRONMENT);
    }

    if (! (usVosConfig & VOS_CONFIG_LCD)) {     /* not LCD            */
        return (VosVideo.usMode);               /* exit ...           */
    }

    /* --- check new video mode --- */
    if (! (usMode == VOS_MODE_TEXT8025 ||   /* unsupported video mode */
           usMode == VOS_MODE_TEXT4025)) {
        PifAbort(MODULE_VOS_SETMODE, FAULT_INVALID_ARGUMENTS);
    }

    PifRequestSem(usVosApiSem);

    /* --- check number of window --- */

    if (usVosNumWindow) {                   /* exist window           */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_SETMODE, FAULT_INVALID_FUNCTION);
		return 0;
    }

    if (VosVideo.usMode == usMode) {        /* same mode              */
        PifReleaseSem(usVosApiSem);
        return (usMode);
    }

    /* --- set new video mode --- */

    usPrevious = VosVideo.usMode;           /* get current video mode */
    VosExecInit(usMode, VosVideo.usMinutes);

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/* ====================================== */
/* ========== End of VosMode.C ========== */
/* ====================================== */
