/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display String Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosStr.C
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
** Synopsis:    VOID VosString(USHORT usHandle, UCHAR *puchString,
*                              USHORT usLength);
*     Input:    usHandle   - user handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosString(USHORT usHandle, TCHAR *puchString, USHORT usLength)
{
    USHORT      usReal;
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;                  /* real window handle */
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW ||             /* range error    */
        ! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRING, FAULT_INVALID_HANDLE);
		return;
    }

    VosExecWindowActive(usReal, FALSE);
    VosExecStringAttr(usReal, puchString, usLength, pWin->uchAttr, FALSE);

    PifReleaseSem(usVosApiSem);
}


/* ===================================== */
/* ========== End of VosStr.C ========== */
/* ===================================== */

