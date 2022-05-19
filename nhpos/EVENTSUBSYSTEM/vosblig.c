/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Back Light Control Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosBLig.C
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

#include "EvsL.h"

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
** Synopsis:    USHORT VosSetBLight(USHORT usState);
*     Input:    usState - back light state
*
** Return:      previous state
*===========================================================================
*/
_declspec(dllexport)  USHORT VosSetBLight(USHORT usState)
{
    USHORT  usPrevious;

    if (! (usVosConfig & VOS_CONFIG_INIT)) {    /* not initialization */
        PifAbort(MODULE_VOS_SETBLIGHT, FAULT_BAD_ENVIRONMENT);
		return (VOS_BLIGHT_ON);
    }

    if (! (usVosConfig & VOS_CONFIG_LCD)) {     /* not LCD            */
        return (VOS_BLIGHT_ON);                 /* exit ...           */
    }

    PifRequestSem(usVosApiSem);

    usPrevious = VosExecBLight(usState);

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/*
*===========================================================================
** Synopsis:    USHORT VosExecBLight(USHORT usState);
*     Input:    usState - back light state
*
** Return:      previous state
*===========================================================================
*/
USHORT VosExecBLight(USHORT usState)
{
    USHORT  usPrevious;

    /* --- notify to thread to change state of back light --- */

    VosVideo.usState |= VOS_STATE_CHANGE;

    /* --- check back light state --- */

    if ((usState == VOS_BLIGHT_ON &&        /* already back light on  */
         VosVideo.usState & VOS_STATE_BLIGHT) ||
        (usState == VOS_BLIGHT_OFF &&       /* already back light off */
         ! (VosVideo.usState & VOS_STATE_BLIGHT))) {
        return (usState);                   /* exit ...               */
    }

    /* --- update back light --- */

    if (usState == VOS_BLIGHT_ON) {         /* back light on          */
        EvsVioBackLight(PIF_VIO_ON);
        VosVideo.usState |= VOS_STATE_BLIGHT;
        usPrevious        = VOS_BLIGHT_OFF;
    } else {                                /* back light off         */
        EvsVioBackLight(PIF_VIO_OFF);
        VosVideo.usState &= ~VOS_STATE_BLIGHT;
        usPrevious        = VOS_BLIGHT_ON;
    }

    return (usPrevious);
}


/* ====================================== */
/* ========== End of VosBLig.C ========== */
/* ====================================== */
