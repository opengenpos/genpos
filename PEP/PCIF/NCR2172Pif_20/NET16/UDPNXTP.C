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
:  Program Name       : UDPNXTP.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:        SHORT    udpnxtp(VOID);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name       Description
:  00.00.01  Jul  3,92   Y.Nozawa   Original
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*-----------------------------------------------------------------------
:   SHORT    udpnxtp(VOID);
:
:   Return      = automatically assigned UDP Port number
:
:   udpnxtp assigns a new UDP port number when a user requests 'ANYLPORT'.
------------------------------------------------------------------------*/
SHORT    udpnxtp(VOID)
{
   if (Net.nxtprt < ULPORT) {
      Net.nxtprt = ULPORT;
   }
   return(Net.nxtprt++);
}

