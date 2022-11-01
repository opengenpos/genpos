/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : UDP Port Initialization Routine
:  Category           : UDP/IP Driver, 2170 XINU Operating System
:  Program Name       : DGINIT.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following valiable and function:
:
:       SHORT   DgInit(DEVICE *pDevice);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

DGBLK   NEAR dgtab[Ndg];

/*-----------------------------------------------------------------------
:   SHORT DgInit(DEVICE *pDevice);
:
:   pDevice     Pointer to the device table entry
:
:   Return      = OK
:
:   "DgInit" initializes UDP ports.
------------------------------------------------------------------------*/
SHORT DgInit(DEVICE *pDevice)
{
    DGBLK   NEAR *pDgblk;

    pDgblk           = &dgtab[pDevice->dvminor];
    pDevice->dvioblk = pDgblk;
    pDgblk->dg_dnum  = pDevice->dvnum;
    pDgblk->dg_state = DG_FREE;
    return(OK);
}

