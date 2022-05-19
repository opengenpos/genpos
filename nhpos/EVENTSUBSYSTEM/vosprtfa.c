/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display Printf (with Attribute) Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosPrtfA.C
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
** Synopsis:    VOID VosPrintfAttr(USHORT usHandle, UCHAR uchAttr,
*                                  UCHAR FAR *pszFormat, ...);
*     Input:    usHandle  -
*               uchAttr   -
*               pszFormat - 
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosPrintfAttr(USHORT usHandle, UCHAR uchAttr, CONST TCHAR *pszFormat, ...)
{
    USHORT      usReal, usLength;
    TCHAR       szString[VOS_NUM_BUFFER];
    VOSWINDOW   FAR *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;                  /* real window handle   */
    pWin   = &aVosWindow[usReal];           /* set window address   */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle      */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_PRINTFATTR, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle      */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_PRINTFATTR, FAULT_INVALID_ENVIRON_2);
		return;
    }

    usLength = VosEditPrintf(pWin,
                             pszFormat,         /* format string    */
                             (SHORT *)(&pszFormat + 1),/* arguments */
                             szString,          /* string buffer    */
                             VOS_NUM_BUFFER); /* string length    */
    VosExecWindowActive(usReal, FALSE);
    VosExecStringAttr(usReal,               /* real handle          */
                      szString,             /* string buffer        */
                      usLength,             /* string length        */
                      uchAttr,              /* character attribute  */
                      FALSE);

    PifReleaseSem(usVosApiSem);
}


/* ======================================= */
/* ========== End of VosPrtfA.C ========== */
/* ======================================= */

