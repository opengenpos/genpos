/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Receive IP Datagram
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : IPRECV.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   IpRecv(DLC *pRcvBuff, SHORT sBuffLen);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name          Description
:  00.00.01  Aug 17,92   T.Kunishige   Original
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT    PROCESS IpRecv(DLC *pRcvBuff, SHORT sBuffLen);
:
:   sDevice     Network Input Device ID
:   pRcvBuff    Receive Buffer
:   sBuffLen    size of Receive Buffer
:
:   Return      = data length if successful
:               = TIMEOUT if timer expired
:               = SYSERR if incorrect argument
:
:   "IpRecv" receives packets from sDevice.
------------------------------------------------------------------------*/
SHORT   IpRecv(DLC *pRcvBuff, SHORT sBuffLen)
{
    IP      *pIp;
    SHORT   sStatus;

    Net.npacket++;
    sStatus = XinuRead(ASY, pRcvBuff, sBuffLen);
    if ( sStatus < 0 ) {
        return (sStatus);
    }
    if ( sStatus <= (SHORT)(IPHLEN+DHLEN) ) {
        sStatus = 0;
        return (sStatus);
    }
    pIp = (IP *)pRcvBuff->auchData;

    if  ( memcmp(pIp->i_dest, Net.myaddr, sizeof(IPADDR)) != 0 ) {
        sStatus = 0;
        return (sStatus);
    }

    switch (net2hs(pRcvBuff->usPType)) {
    case DP_IP:
        return (sStatus);

    default: /* just drop packet */
        Net.ndrop++;
        sStatus = 0;
        return (sStatus);
    }
}

