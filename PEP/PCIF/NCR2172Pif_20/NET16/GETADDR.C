/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Get Local IP Address
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : GETADDR.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       VOID    GetAddr(IPADDR MyAddr);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name        Description
:  00.00.00  Mar 31,88   D.Comer     Original PC-XINU
:  00.00.01  Jul  3,92   Y.Nozawa    Applied coding convension of 2170
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   VOID    GetAddr(IPADDR MyAddr);
:
:   MyAddr      Local IP Address
:
:   "GetAddr" copy the local IP address onto MyAddr.
------------------------------------------------------------------------*/
VOID    GetAddr(IPADDR MyAddr)
{
    memcpy(MyAddr, Net.myaddr, sizeof(IPADDR));
}

