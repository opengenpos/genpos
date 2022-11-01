/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Allocation
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : DGALLOC.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   DgAlloc(VOID);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name       Description
:  00.00.00  Mar 31,88   D.Comer    Original PC-XINU
:  00.00.01  Jul  3,92   Y.Nozawa   Applied coding convension of 2170
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

/*-----------------------------------------------------------------------
:   SHORT   DgAlloc(VOID);
:
:   Return      = device descriptor if successful
:               = SYSERR if error
:
:   "DgAlloc" allocates a datagram psuedo device and return descriptor.
------------------------------------------------------------------------*/
SHORT   DgAlloc(VOID)
{
    DGBLK   NEAR *dgptr;
    SHORT   i;

    for (i=0 ; i< Ndg ; i++) {
        dgptr = &dgtab[i];
        if (dgptr->dg_state == DG_FREE) {
            dgptr->dg_state = DG_USED;
            return(i);
        }
    }
    return(SYSERR);
}


