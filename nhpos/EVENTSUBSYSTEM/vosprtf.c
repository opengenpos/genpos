/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display Printf Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosPrtf.C
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
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Aug-03-21 : 02.04.01 : R.Chambers : _tcslen() changed to tcharlen(), remove casts.
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
#include "rfl.h"
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
** Synopsis:    VOID VosPrintf(USHORT usHandle, UCHAR FAR *pszFormat, ...);
*     Input:    usHandle  - user handle of window
*               pszFormat - format string
*               ...       - arguments
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosPrintf(USHORT usHandle, CONST TCHAR  *pszFormat, ...)
{
    USHORT      usReal, usLength;
	TCHAR       szString[VOS_NUM_BUFFER] = { 0 };
    VOSWINDOW   *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */

    usReal = usHandle - 1;                  /* real window handle   */
    pWin   = &aVosWindow[usReal];           /* set window address   */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle      */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_PRINTF, FAULT_INVALID_ENVIRON_1);
		return;
    }

    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle      */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_PRINTF, FAULT_INVALID_ENVIRON_2);
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
                      pWin->uchAttr,        /* character attribute  */
                      FALSE);

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    USHORT VosEditPrintf(VOSWINDOW FAR *pWin,
*                                    UCHAR FAR *pszFormat, SHORT *psArgs,
*                                    UCHAR *pszBuf, USHORT usBufLen);
*     Input:    pWin      - address of window information
*               pszFormat - format string
*               psArgs    - arguments
*               pszBuf    - destination buffer
*               usBufLen  - buffer length
*
** Return:      nothing
*===========================================================================
*/
USHORT VosEditPrintf(VOSWINDOW *pWin, CONST TCHAR *pszFormat,
                     SHORT *psArgs, TCHAR *pszBuf, USHORT usBufLen)
{
    USHORT  usPos;
	TCHAR   *pszWork, szWork[VOS_NUM_BUFFER] = { 0 };

    /* --- analysis format strings --- */
    _RflFormStr(pszFormat, psArgs, szWork, VOS_NUM_BUFFER);//(USHORT)sizeof(szWork));

    /* --- adjust '\t' format --- */
    pszWork = szWork;
    usPos   = 0;
    for (;;) {
		TCHAR   *pszNext, *puchTab, *puchNl, *puchCr;

        /* --- search '\t' character in buffer --- */
        puchTab = _tcschr(pszWork, _T('\t'));
        if (puchTab == NULL) {              /* '\t' not found */
            _tcscpy(pszBuf + usPos, pszWork);
            break;                          /* exit ...       */
        }

        /* --- search '\n' or '\r' character in buffer --- */
        puchNl = _tcschr(pszWork, _T('\n'));
        puchCr = _tcschr(pszWork, _T('\r'));
        if (puchNl && puchCr) {             /* '\n' and '\r' found     */
            if (puchNl < puchCr) {          /* first '\n'              */
                *puchNl = '\x00';
                pszNext = puchNl + 1;
            } else {                        /* first '\r'              */
                *puchCr = '\x00';
                pszNext = puchCr + 1;
            }
        } else if (puchNl) {                /* '\n' found              */
            *puchNl = '\x00';
            pszNext = puchNl + 1;
        } else if (puchCr) {                /* '\r' found              */
            *puchCr = '\x00';
            pszNext = puchCr + 1;
        } else {                            /* '\n' and '\r' not found */
            pszNext = pszWork + tcharlen(pszWork);
        }

        RflStrAdjust((pszBuf + usPos), (USHORT)(usBufLen - usPos), pszWork, pWin->CharSize.usCol, (BOOL)RFL_FEED_ON);   /* ### Mod (2171 for Win32) V1.0 */

        pszWork  = pszNext;
        usPos   += tcharlen(pszBuf + usPos);
    }

    return (tcharlen(pszBuf));
}


/* ====================================== */
/* ========== End of VosPrtf.C ========== */
/* ====================================== */
