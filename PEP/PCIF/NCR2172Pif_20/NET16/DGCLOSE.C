/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Close Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : DGCLOSE.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   DgClose(DEVICE *pDevice);
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
:   SHORT   DgClose(DEVICE *pDevice);
:
:   pDevice     Pointer to the device table entry
:
:   Return      = OK if successful
:
:   "DgClose" closes a dataram pseudo device, making it available again.
------------------------------------------------------------------------*/
SHORT   DgClose(DEVICE *pDevice)
{
   DGBLK    NEAR *pDgblk;
   NETQ     NEAR *pNetq;
   int      nq;

   pDgblk = &dgtab[pDevice->dvminor];
   nq     = pDgblk->dg_netq;
   pNetq  = &Net.netqs[nq];
   pNetq->valid  = FALSE;
   pNetq->uport  = -1;
   pNetq->pid    = BADPID;
   pDgblk->dg_state = DG_FREE;
   return(OK);
}
