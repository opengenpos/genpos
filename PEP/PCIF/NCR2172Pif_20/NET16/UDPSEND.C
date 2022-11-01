/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Send UDP Packet
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : UDPSEND.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   UdpSend(IPADDR Faddr, SHORT sFport, SHORT sLport,
:                       DLC *pPacket, SHORT sDataLen);
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
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*-----------------------------------------------------------------------
:   SHORT   UdpSend(IPADDR Faddr, SHORT sFport, SHORT sLport,
:                       DLC *pPacket, SHORT sDataLen);
:
:   Faddr       foreign IP address
:   sFport      foreign port number
:   sLport      local port number
:   pPacket     Packet to send
:   sDataLen    Data Length
:
:   Return      = OK if successful
:
:   "UdpSend" sends one UDP datagram to a given (foreign) IP address.
------------------------------------------------------------------------*/
SHORT UdpSend(IPADDR Faddr, SHORT sFport, SHORT sLport, DLC *pPacket,
                SHORT sDataLen)
{
    UDP    *udpptr;
    IP     *ipptr;

    /*  Fill in UDP header; pass to ipsend to fill in IP header */

    ipptr            = (IP *) pPacket->auchData;
    ipptr->i_proto   = IPRO_UDP;
    udpptr           = (UDP *) ipptr->i_data;
    udpptr->u_sport  = hs2net(sLport);
    udpptr->u_dport  = hs2net(sFport);
    udpptr->u_udplen = (SHORT) hs2net((SHORT) (UHLEN+sDataLen));
    if ( isodd(sDataLen) ) {
        udpptr->u_data[sDataLen] = (CHAR)0;
    }
    udpptr->u_ucksum = 0;
    return( IpSend(Faddr, pPacket, (SHORT)(UHLEN+sDataLen) ) );
}

