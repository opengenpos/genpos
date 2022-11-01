/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Internet Control Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : NETCNTL.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   NetCntl(DEVICE *pDevice, SHORT sFunction, ...);
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
:            Jun 22,98   O.Nakada   Win32
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT   NetCntl(DEVICE *pDevice, SHORT sFunction, ...);
:
:   pDevice     Pointer to the device table entry
:   sFunction =
:   1. NET_PINGPONG : pingpong test
:       IPADDR  Faddr;  destination IP Address
:
:       Return  = OK if successful
:               = SYSERR if error
:
------------------------------------------------------------------------*/
SHORT   NetCntl(DEVICE *pDevice, SHORT sFunction, ...)
{
    static const CHAR szPingPong[] = "This is a ping-pong message\n";
    IPADDR  *pFaddr;
    static  SHORT sSeq = 0;

    pDevice = pDevice;
    if (sFunction==NET_PINGPONG) {              /* ping-pong test */
#if	WIN32
        pFaddr = *(IPADDR **)((LONG *)&sFunction+1);
#else
        pFaddr = *(IPADDR **)(&sFunction+1);
#endif
        return( IcmpEcho(*pFaddr, szPingPong, sizeof(szPingPong), sSeq++) );
    }
    return(SYSERR);
}

