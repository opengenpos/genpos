/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Window Delete Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosWDele.C
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
* May-20-96  G6        O.Nakada     Added invisible cursor control.
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

#include "EvsL.h"      /* ### ADD (2172 for Win32) V1.0 */

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
** Synopsis:    VOID VosDeleteWindow(USHORT usHandle);
*     Input:    usHandle - user handle of window
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID  VosDeleteWindow(USHORT usHandle)
{
    SHORT       sWin;
    USHORT      usReal, usPos, usLoop, usRefresh;
    TCHAR       FAR *puchBuf;
    VOSWINDOW   FAR *pWin;

    _wEvsDebugTrace("DELETE WINDOW");

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;
    pWin   = &aVosWindow[usReal];           /* set window address */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_DELETEWINDOW, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_DELETEWINDOW, FAULT_INVALID_ENVIRON_2);
		return;
    }

    /* ### ADD (2172 for Win32) V1.0 */
    __vEvsDeleteData_VOSWINDOW(pWin);

    /* --- clear current window --- */

    puchBuf = pWin->puchBuf;                /* set window buffer       */
    usLoop  = pWin->PhySize.usRow * pWin->PhySize.usCol;

    for (usPos = 0; usPos < usLoop; usPos++) {
        *puchBuf++ = _T(' ');                   /* space character         */
        *puchBuf++ = VOS_C_WHITE;           /* normal attribute        */
    }

    pWin->usState &= ~VOS_STATE_CTYPE;      /* invisible state         */
    VosDisplayPhyWindow(pWin);              /* display current window  */

    /* --- check overlaping window flag --- */

    for (sWin = usVosNumWindow - 1, usRefresh = FALSE; sWin >= 0; sWin--) {
        if ((usHandle = ausVosQueue[sWin]) == 0) {  /* free handle     */
            continue;                               /* next ...        */
        }

        pWin = &aVosWindow[usHandle - 1];   /* set window address      */
        if (! (pWin->usState & VOS_STATE_OVERLAP)) { /* normal window  */
            continue;                                /* next ...       */
        }

        usRefresh = TRUE;                   /* enable refresh          */
        break;                              /* exit ...                */
    }

    /* --- delete current window resource --- */

    VosUpdateOverlapFlag(usReal, VOS_DISABLE); /* reset overlap flag   */
    VosDeleteQueue(usReal);                 /* delete window in queue  */

    VosMemoryFree(aVosWindow[usReal].usMemInfo,     /* free a memory   */
                  aVosWindow[usReal].puchBuf);
    aVosWindow[usReal].usState = VOS_STATE_FREE;    /* free a window   */
    usVosNumWindow--;                       /* number of window        */

    if (usVosNumWindow) {                   /* exist other window      */
        if (usRefresh) {                    /* overlapped other window */
            VosExecRefresh();               /* refresh other window    */
        } else {
            usReal = ausVosQueue[0] - 1;
            pWin   = &aVosWindow[usReal];   /* set window address      */

            VosExecCursor(pWin);            /* set cursor status      */
        }
    } else {                                /* not exist other window  */
        EvsVioCurOnOff(PIF_VIO_OFF, NULL);  /* cursor off              */
        VosVideo.usState &= ~VOS_STATE_CTYPE;
    }

    PifReleaseSem(usVosApiSem);
}


/* ======================================= */
/* ========== End of VosWDele.C ========== */
/* ======================================= */


