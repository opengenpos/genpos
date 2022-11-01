/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Open Routine
:  Category           : UDP/IP Driver, 2170 XINU Operating System
:  Program Name       : DGMOPEN.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following valiable and function:
:
:       SHORT   NetOpen(DEVICE *pDevice, ...);
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
:                                   Changed the 2nd argument (foreign port)
:                                     from ASCIIZ to binary
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

#include <string.h>

DGBLK   NEAR dgtab[Ndg];

/*-----------------------------------------------------------------------
:   SHORT   NetOpen(DEVICE *pDevice, ...);
:
:   pDevice     Pointer to the device table entry
:   ...         takes the following:
:
:       struct forport {        foreign port address
:           IPADDR IPaddr;      IP Address
:           SHORT  sPort;       UDP Port Number
:       } *pForPort;
:       SHORT  sLocPort;        Local UDP Port Number
:
:   Return      = OK
:
:   "DgmOpen" opens a fresh datagram pseudo device and returns descriptor.
------------------------------------------------------------------------*/
SHORT   NetOpen(DEVICE *pDevice, ...)
{
    DGBLK   NEAR *pDgblk;
    NETQ    NEAR *pNetq;
    SHORT   slot;
    SHORT   nq;
    SHORT   i;
    struct forport {        /* foreign port address */
        IPADDR IPaddr;      /* IP Address           */
        SHORT  sPort;       /* UDP Port Number      */
    } *pForPort;
    SHORT  sLocPort;        /* Local UDP Port Number*/

    pForPort = *(struct forport **)(&pDevice+1);
    sLocPort = *(SHORT *)((struct forport **)(&pDevice+1)+1);
    if( (slot = DgAlloc()) == SYSERR ) {
        return(SYSERR);
    }
    pDgblk = &dgtab[slot];
    if (sLocPort == ANYLPORT) {
        sLocPort = udpnxtp();
    } else {
        for (i=0 ; i<NETQS ; i++) {        /* check in NET */
            if (Net.netqs[i].valid &&
                   (Net.netqs[i].uport == sLocPort)) {
                pDgblk->dg_state = DG_FREE;
                return(SYSERR);
            }
        }
    }
    pDgblk->dg_fport = pForPort->sPort;
    blkcopy(pDgblk->dg_faddr, pForPort->IPaddr, sizeof(IPADDR));
    if ((nq=NqAlloc()) == SYSERR) {
        pDgblk->dg_state = DG_FREE;
        return(SYSERR);
    }
    pNetq = &Net.netqs[nq];
    pNetq->uport = pDgblk->dg_lport  = sLocPort;
    pDgblk->dg_xport = pNetq->xport;
    pDgblk->dg_netq  = nq;
    pDgblk->dg_mode  = DG_NMODE;
    pDgblk->dg_time  = 0x0;                  /* added by T.Maruyama */
    return(pDgblk->dg_dnum);
}
