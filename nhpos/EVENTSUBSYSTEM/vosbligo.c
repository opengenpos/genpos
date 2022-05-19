/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Automatic Back Light Off Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosBLigO.C
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
** Synopsis:    USHORT VosSetBLightOff(USHORT usMinutes);
*     Input:    usMinutes - time of automatic off
*
** Return:      previous time
*===========================================================================
*/
__declspec( dllexport ) USHORT VosSetBLightOff(USHORT usMinutes)
{
    USHORT  usPrevious;

    if (! (usVosConfig & VOS_CONFIG_INIT)) {    /* not initialization */
        PifAbort(MODULE_VOS_SETBLIGHTOFF, FAULT_BAD_ENVIRONMENT);
		return (VOS_BLIGHT_ON);
    }

    if (! (usVosConfig & VOS_CONFIG_LCD)) {     /* not LCD            */
        return (VosVideo.usMinutes);            /* exit ...           */
    }

    PifRequestSem(usVosApiSem);

    usPrevious          = VosVideo.usMinutes;
    VosVideo.usMinutes  = usMinutes;
    VosVideo.usState   |= VOS_STATE_CHANGE;

    PifReleaseSem(usVosApiSem);
    return (usPrevious);
}


/* ======================================= */
/* ========== End of VosBLigO.C ========== */
/* ======================================= */
