/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Control Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : DGCNTL.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   DgCntl(DEVICE *pDevice, SHORT sFunction, ...);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name         Description
:  00.00.00  Mar 31,88   D.Comer      Original PC-XINU
:  00.00.01  Jul  3,92   Y.Nozawa     Applied coding convension of 2170
:  00.00.02  Aug 14,92   T.Kunishige  Changed for PEP
:            Jun 22,98   O.Nakada     Win32
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

/*-----------------------------------------------------------------------
:   SHORT   DgCntl(DEVICE *pDevice, SHORT sFunction, ...);
:
:   pDevice     Pointer to the device table entry
:   sFunction   = DG_SETMODE    : set mode
                = DG_CLEAR      : clear queued packets
:
:   Return      = device descriptor if successful
:               = SYSERR if error
:
:   "DgCntl" is a control function for datagram pseudo-device.
------------------------------------------------------------------------*/
SHORT   DgCntl(DEVICE *pDevice, SHORT sFunction, ...)
{
    DGBLK   NEAR *pDgblk;
    SHORT   ret;
    SHORT   arg;

#if	WIN32
    arg = *(SHORT *)((LONG *)&sFunction+1);
#else
    arg = *(SHORT *)(&sFunction+1);
#endif
    pDgblk = &dgtab[pDevice->dvminor];
    ret   = OK;
    switch (sFunction) {
    case DG_SETMODE:                        /* set mode bits */
        pDgblk->dg_mode = arg;
        break;

    case DG_SETTIME:                        /* set timer */
        pDgblk->dg_time = arg;
        break;

    default:
        ret = SYSERR;
    }
    return(ret);
}
