/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Misc. Functions
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosMisc.C
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
* May-20-96  G6        O.Nakada     Added cursor control in
*                                     VosDisplayPhyWindow().
* 2172 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Oct-13-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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

#include "EvsL.h"		/* ### ADD (2172 for Win32) V1.0 */


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
** Synopsis:    VOID VosExecWindowActive(USHORT usReal, USHORT usComplusory);
*     Input:    usReal - user handle of window
*               usComplusory - show current window
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecWindowActive(USHORT usReal, USHORT usComplusory)
{
//    USHORT      usWin, usOff, usBit, usFound;
    VOSWINDOW   FAR *pWin;

    VosExecBLight(VOS_BLIGHT_ON);               /* back light on        */

    pWin = &aVosWindow[usReal];                 /* set window address   */

    /* --- check window type --- */

    if (pWin->usState & VOS_STATE_WNORMAL &&    /* normal window        */
        pWin->usState & VOS_STATE_OVERLAP) {    /* overlapped           */
        return;                                 /* exit ...             */
    }

    /* --- check current window --- */

    if (usComplusory ||                     /* complusory display       */
        pWin->usState & VOS_STATE_OVERLAP) {/* overlapped window        */
        pWin->usState &= ~VOS_STATE_OVERLAP;/* reset overlap flag       */
        VosDisplayPhyWindow(pWin);          /* display current window   */
    }

    if (pWin->usState & VOS_STATE_WNORMAL) {    /* normal window        */
        return;                                 /* exit ...             */
    }

    /* --- check queue of window --- */

    if (usComplusory ||                     /* complusory display       */
        ausVosQueue[0] != usReal + 1) {     /* other window for active  */
        VosExecCursor(pWin);                /* set cursor status        */
        VosUpdateQueue(usReal);             /* update queue of window   */
    }

    /* --- update overlap flag of other window --- */
#if 0
//SR 853  This is no longer necessary because it was being used for the older 
//terminals in which there was only one window for use which would have to be over-
//lapped.  JHHJ
    usOff = usReal / 8;
    usBit = 0x01 << usReal % 8;

    for (usWin = 0, usFound = 0; usWin < VOS_NUM_WINDOW; usWin++) {
        if (usVosNumWindow == usFound) {        /* all window updated   */
            break;                              /* exit ...             */
        }

        pWin = &aVosWindow[usWin];              /* set current window   */
        if (pWin->usState == VOS_STATE_FREE) {  /* free buffer          */
            continue;                           /* next ...             */
        }

        usFound++;
        if (usWin == usReal) {                  /* same new handle      */
            continue;                           /* next ...             */
        }

        if (pWin->auchOvl[usOff] & usBit) {     // overlap window       
            pWin->usState |= VOS_STATE_OVERLAP; // set overlap flag     
        }
    }
#endif
}


/*
*===========================================================================
** Synopsis:    VOID VosUpdateOverlapFlag(USHORT usReal, USHORT usState);
*     Input:    usReal  - real handle of window
*               usState - update state of overlap flag
*
** Return:      nothing
*===========================================================================
*/
VOID VosUpdateOverlapFlag(USHORT usReal, USHORT usState)
{
    USHORT      usWin, usFound, usOff;
    VOSPOS      NewBeg, NewEnd, CurBeg, CurEnd;
    VOSWINDOW   FAR *pWin, FAR *pNew;

    /* --- set physical position of new window --- */

    pNew         = &aVosWindow[usReal];
    NewBeg.usRow = pNew->PhyBase.usRow;
    NewBeg.usCol = pNew->PhyBase.usCol;
    NewEnd.usRow = pNew->PhyBase.usRow + pNew->PhySize.usRow - 1;
    NewEnd.usCol = pNew->PhyBase.usCol + pNew->PhySize.usCol - 1;

    for (usWin = 0, usFound = 0; usWin < VOS_NUM_WINDOW; usWin++) {
        if (usVosNumWindow == usFound) {        /* all window updated */
            break;                              /* exit ...           */
        }

        pWin = &aVosWindow[usWin];              /* set current window */
        if (pWin->usState == VOS_STATE_FREE) {  /* free buffer        */
            continue;                           /* next ...           */
        }

        usFound++;
        if (usWin == usReal) {                  /* same new handle    */
            continue;
        }

        /* --- set physical position of current window --- */

        CurBeg.usRow = pWin->PhyBase.usRow;
        CurBeg.usCol = pWin->PhyBase.usCol;
        CurEnd.usRow = pWin->PhyBase.usRow + pWin->PhySize.usRow - 1;
        CurEnd.usCol = pWin->PhyBase.usCol + pWin->PhySize.usCol - 1;

        /* --- check overlap window --- */

        if (CurEnd.usRow < NewBeg.usRow || NewEnd.usRow < CurBeg.usRow) {
            continue;
        }
        if (CurEnd.usCol < NewBeg.usCol || NewEnd.usCol < CurBeg.usCol) {
            continue;
        }

        if (usState == VOS_ENABLE) {            /* set function       */
            pNew->auchOvl[usWin  / 8] |= (0x01 << usWin  % 8);
            pWin->auchOvl[usReal / 8] |= (0x01 << usReal % 8);
            continue;                           /* next ...           */
        }

        /* --- reset overlap flag of current window --- */

        pNew->auchOvl[usWin  / 8] &= ~(0x01 << usWin  % 8);
        pWin->auchOvl[usReal / 8] &= ~(0x01 << usReal % 8);

        if (! (pWin->usState & VOS_STATE_OVERLAP)) {
            continue;                           /* next ...           */
        }

        /* --- check overlap flag of other window --- */

        for (usOff = 0; usOff < VOS_NUM_OVL; usOff++) {
            if (pWin->auchOvl[usOff]) {         /* overlap window     */
                break;
            }
        }
        if (usOff == VOS_NUM_OVL) {             /* not found          */
            pWin->usState &= ~VOS_STATE_OVERLAP;/* reset overlap flag */
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID VosDisplayPhyWindow(VOSWINDOW FAR *pWin);
*     Input:    pWin - address of window information
*
** Return:      nothing
*===========================================================================
*/
VOID VosDisplayPhyWindow(VOSWINDOW FAR *pWin)
{
    USHORT  usRow;
    USHORT  usKeep = VosVideo.usState;

    /* --- cursor off --- */

    EvsVioCurOnOff(PIF_VIO_OFF, pWin);
    VosVideo.usState &= ~VOS_STATE_CTYPE;
	if (! (pWin->usState & VOS_STATE_WBUTTONS)) {
		for (usRow = 0; usRow < pWin->PhySize.usRow; usRow++) {
				  EvsVioStrAttr((USHORT)(pWin->PhyBase.usRow + usRow),
						  pWin->PhyBase.usCol,
						  (UCHAR FAR *)(pWin->puchBuf + (usRow * pWin->PhySize.usCol) * 2),
						  pWin->PhySize.usCol,
						  usVosPage,(VOID *)pWin);   /* ### Mod (2171 for Win32) V1.0 */
		}
	}

    /* --- restore curosr state --- */

    if (usKeep & VOS_STATE_CTYPE) {
        EvsVioCurOnOff(PIF_VIO_ON, pWin);
        VosVideo.usState |= VOS_STATE_CTYPE;
    }
}


/*
*===========================================================================
** Synopsis:    VOID VosDisplayRow(VOSWINDOW FAR *pWin, USHORT usRow);
*     Input:    pWin  - address of window information
*               usRow - display row
*
** Return:      nothing
*===========================================================================
*/
VOID VosDisplayRow(VOSWINDOW FAR *pWin, USHORT usRow)
{
    /* --- check window type --- */

    if (pWin->usState & VOS_STATE_WNORMAL &&    /* normal window        */
        pWin->usState & VOS_STATE_OVERLAP) {    /* overlapped           */
        return;                                 /* exit ...             */
    }

            EvsVioStrAttr((USHORT)(pWin->PhyBase.usRow + usRow),
                  pWin->PhyBase.usCol,
                  (UCHAR FAR *)(pWin->puchBuf + (usRow * pWin->PhySize.usCol) * 2),
                  pWin->PhySize.usCol,
                  usVosPage,(VOID *)pWin);   /* ### Mod (2171 for Win32) V1.0 */
}


/* ====================================== */
/* ========== End of VosMisc.C ========== */
/* ====================================== */
