/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Network Initialization
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : NETINIT.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   NetInit(DEVICE *pDevice);
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
:                                     Added netin process creation
:                                     Moved netout process creation from netin
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

#include <string.h>

NETINFO NEAR Net;                       /* All network parameters */
SHORT   NEAR ipackid;                   /* Internet datagram ID   */

/*-----------------------------------------------------------------------
:   SHORT NetInit(DEVICE *pDevice);
:
:   Return      = OK
:
:   "NetInit" initializes network data structures.
------------------------------------------------------------------------*/
SHORT NetInit(DEVICE *pDevice)
{
    /* Initialize Net structure  */

    Net.chStarted = FALSE;
    Net.nxtprt  = ULPORT;
    Net.npacket = Net.ndrop = Net.nover = 0;    /* clear tallies */
    return(OK);
    pDevice = pDevice;

}
