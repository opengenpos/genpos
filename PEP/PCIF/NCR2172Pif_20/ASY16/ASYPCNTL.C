/*
*===========================================================================
* Title       : I/O Control Function
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASYPCNTL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Compact Model
* Options     : /c /ACw /W4 /G1s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Sep-18-92:00.00.01:O.Nakada   : Initial
* Jul-01-93:01.00.00:O.Nakada   : Modify off line check.
* Jun-22-98:        :O.Nakada   : Win32
*          :        :           :
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
#include <conf.h>
#include <kernel.h>
#include "asy.h"


/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/


/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT AsyCntl(DEVICE *pDevTbl, SHORT func, ...)
*     Input:    pDevTbl  - Device Table
*               sFunc    - I/O Control Function
*
** Return:      Serial I/O Driver Status
*
** Description:
*===========================================================================
*/
SHORT AsyCntl(DEVICE *pDevTbl, SHORT sFunc, ...)
{
    USHORT  usLine;

    if (!(AsyData.fchStatus & ASY_OPEN)) {  /* Closed ASY Driver */
        return (SYSERR);
    }

    switch (sFunc) {
    case ASY_FUNC_READ:
        /* check line status */
        usLine = (USHORT)XinuControl(AsyData.hsSio,
                                     ASY_SIO_FUNC_STATUS);
        if (!(usLine & ASY_SIO_STS_DSR)) {
            return (XINU_OFFLINE);              /* off line */
        } else {
#if	WIN32
            return (XinuRead(AsyData.hsSio,
                             *(VOID FAR **)((LONG *)&sFunc + 1),
                             *(USHORT *)((VOID FAR **)((LONG *)&sFunc + 1) + 1)));
#else
            return (XinuRead(AsyData.hsSio,
                             *(VOID FAR **)(&sFunc + 1),
                             *(USHORT *)((VOID FAR **)(&sFunc + 1) + 1)));
#endif
        }

    case ASY_FUNC_WRITE:
        /* check line status */
        usLine = (USHORT)XinuControl(AsyData.hsSio,
                                     ASY_SIO_FUNC_STATUS);
        if (!(usLine & ASY_SIO_STS_DSR)) {
            return (XINU_OFFLINE);              /* off line */
        } else {
#if	WIN32
            return (XinuWrite(AsyData.hsSio,
                              *(VOID FAR **)((LONG *)&sFunc + 1),
                              *(USHORT *)((VOID FAR **)((LONG *)&sFunc + 1) + 1)));
#else
            return (XinuWrite(AsyData.hsSio,
                              *(VOID FAR **)(&sFunc + 1),
                              *(USHORT *)((VOID FAR **)(&sFunc + 1) + 1)));
#endif
        }

    case ASY_FUNC_RECEIVE_TIMER:
#if	WIN32
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_RECEIVE_TIMER,
                            (USHORT FAR *)(*(USHORT FAR **)((LONG *)&sFunc + 1))));
#else
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_RECEIVE_TIMER,
                            (USHORT FAR *)(*(USHORT FAR **)(&sFunc + 1))));
#endif

    case ASY_FUNC_SEND_TIMER:
#if	WIN32
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_SEND_TIMER,
                            (USHORT FAR *)(*(USHORT FAR **)((LONG *)&sFunc + 1))));
#else
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_SEND_TIMER,
                            (USHORT FAR *)(*(USHORT FAR **)(&sFunc + 1))));
#endif

    case ASY_FUNC_DTR_ON:
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_DTR_ON));

    case ASY_FUNC_DTR_OFF:
        return (XinuControl(AsyData.hsSio,
                            ASY_SIO_FUNC_DTR_OFF));

    default:
        return (SYSERR);
    }

    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}



/* ---------- End of ASYPCNTL.C ---------- */
