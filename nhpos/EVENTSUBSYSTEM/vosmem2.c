/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Memory Control Functions 2
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosMem2.C
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
#include "termcfg.h"
#include "pif.h"
#include "log.h"
#include "vos.h"
#include "vosl.h"
#include "vostable.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
UCHAR   auchVosMemUsed[VOS_NUM_MEMINFO] = {0, 0};


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID FAR * VosMemoryAlloc(USHORT usID, USHORT usBytes,
*                                         USHORT FAR *pusMemInfo);
*     Input:    udID       - Window ID
*               usBytes    - length of allocation memory
*    Output:    pusMemInfo - address of memory information
*
** Return:      address of allocation memory
*===========================================================================
*/
VOID  * VosMemoryAlloc(USHORT usID, USHORT usBytes,
                          USHORT FAR *pusMemInfo)
{
    USHORT  usOffset;

    /* --- check window ID --- */

    if (usID == 2170) {                             /* special function */
        *pusMemInfo = VOS_MEM2_SPECIAL;
        return (VosVirtualAlloc(usBytes));
    }

    /* --- get address of memory information --- */

    for (usOffset = 0; aVosMem2Info[usOffset].usID; usOffset++) {
        if (aVosMem2Info[usOffset].usID == usID) {  /* found info.      */
            break;                                  /* next ...         */
        }
    }
    if (usOffset >= VOS_NUM_MEMINFO) {              /* not found        */
        PifAbort(MODULE_VOS_MEMORYALLOC, FAULT_INVALID_ARGUMENTS);
		return NULL;
    }

    /* --- check tag information --- */

    if (auchVosMemUsed[usOffset] == VOS_MEM2_MARK) {/* already used     */
        PifAbort(MODULE_VOS_MEMORYALLOC, FAULT_INVALID_FUNCTION);
		return NULL;
    }

    /* --- check allocation size --- */

    if (aVosMem2Info[usOffset].usBytes != usBytes) {/* invalid size     */
        PifAbort(MODULE_VOS_MEMORYALLOC, FAULT_INVALID_ARGUMENTS);
		return NULL;
    }

    auchVosMemUsed[usOffset] = VOS_MEM2_MARK;       /* set used mark    */
    *pusMemInfo              = usOffset + 1;

    return (aVosMem2Info[usOffset].pvAddr);
}


/*
*===========================================================================
** Synopsis:    VOID VosMemoryFree(USHORT usMemInfo, VOID FAR *pvAddr);
*     Input:    usMemInfo - memory information
*               pvAddr    - address of memory
*
** Return:      nothing
*===========================================================================
*/
VOID VosMemoryFree(USHORT usMemInfo, VOID FAR *pvAddr)
{
    USHORT  usOffset;

    /* --- check special functon --- */

    if (usMemInfo == VOS_MEM2_SPECIAL) {    /* special function       */
        VosVirtualFree(pvAddr);             /* free memory            */
        return;                             /* exit ...               */
    }

    usOffset = usMemInfo - 1;               /* get memory information */

    if (aVosMem2Info[usOffset].pvAddr != pvAddr) { /* invalid address */
        PifAbort(MODULE_VOS_MEMORYFREE, FAULT_INVALID_ARGUMENTS);
		return;
    }

    auchVosMemUsed[usOffset] = 0;           /* free memory            */
}


/* ====================================== */
/* ========== End of VosMem2.C ========== */
/* ====================================== */
