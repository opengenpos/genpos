/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Network Demultiplexing Queue Allocation
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : NQALLOC.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   NqAlloc(VOID);
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
:   SHORT   NqAlloc(VOID);
:
:   Return      = device descriptor if successful
:               = SYSERR if error
:
:   "DgAlloc" allocates a network demultiplexing queue.
------------------------------------------------------------------------*/
SHORT   NqAlloc(VOID)
{
    SHORT   i;
    NETQ    NEAR *pNetq;

    for (i=0 ; i<Ndg ; i++) {
        pNetq = &Net.netqs[i];
        if (!pNetq->valid) {
            pNetq->valid = TRUE;
            pNetq->uport = -1;
            pNetq->pid   = BADPID;
            return(i);
        }
    }
    return(SYSERR);
}
