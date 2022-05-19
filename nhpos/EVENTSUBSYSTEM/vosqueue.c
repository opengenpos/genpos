/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Queue Control Functions
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosQueue.C
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
#include "vos.h"
#include "vosl.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
USHORT  FAR ausVosQueue[VOS_NUM_WINDOW];    /* queue of window */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID VosInsertQueue(USHORT usReal);
*     Input:    usReal - real handle of window
*
** Return:      nothing
*===========================================================================
*/
VOID VosInsertQueue(USHORT usReal)
{
    USHORT  usHandle;
    SHORT   sWin;

    usHandle = usReal + 1;                  /* make user handle      */

    if (ausVosQueue[0] == 0) {              /* free buffer           */
        ausVosQueue[0] = usHandle;          /* set new window handle */
        return;                             /* exit ...              */
    }

    /* --- move previous handle --- */

    for (sWin = usVosNumWindow - 2; sWin >= 0; sWin--) {
        ausVosQueue[sWin + 1] = ausVosQueue[sWin];  /* move handle   */
    }

    ausVosQueue[0] = usHandle;              /* set new window handle */
}


/*
*===========================================================================
** Synopsis:    VOID VosUpdateQueue(USHORT usReal);
*     Input:    usReal - real handle of window
*
** Return:      nothing
*===========================================================================
*/
VOID VosUpdateQueue(USHORT usReal)
{
    USHORT  usHandle;
    SHORT   sWin;

    usHandle = usReal + 1;                  /* make user handle          */

    if (ausVosQueue[0] == usHandle) {       /* the handle is already top */
        return;                             /* exit ...                  */
    }

    /* --- search handle in queue buffer --- */

    for (sWin = usVosNumWindow - 1; sWin > 0; sWin--) {
        if (ausVosQueue[sWin] == usHandle) {/* free buffer               */
            break;                          /* skip ...                  */
        }
    }

    /* --- move handle in queue buffer --- */

    for (; sWin > 0; sWin--) {
        ausVosQueue[sWin] = ausVosQueue[sWin - 1];  /* move handle       */
    }

    ausVosQueue[0] = usHandle;              /* set new window handle     */
}


/*
*===========================================================================
** Synopsis:    VOID VosDeleteQueue(USHORT usReal);
*     Input:    usReal - real handle of window
*
** Return:      nothing
*===========================================================================
*/
VOID VosDeleteQueue(USHORT usReal)
{
    USHORT  usHandle, usWin;

    usHandle = usReal + 1;                      /* make user handle   */

    /* --- search for delete window handle --- */

    for (usWin = 0; usWin < usVosNumWindow; usWin++) {
        if (ausVosQueue[usWin] == usHandle) {   /* same window handle */
            break;                              /* skip ...           */
        }
    }

    /* --- move handle in queue buffer --- */

    while (usWin + 1 < usVosNumWindow) {
        ausVosQueue[usWin] = ausVosQueue[usWin + 1]; /* move handle   */
        usWin++;
    }

    ausVosQueue[usWin] = 0;                     /* clear queue buffer */
}


/* ======================================= */
/* ========== End of VosQueue.C ========== */
/* ======================================= */
