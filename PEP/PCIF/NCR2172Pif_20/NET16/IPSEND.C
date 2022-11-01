/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Send IP Datagram
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : IPSEND.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   IpSend(IPADDR Faddr, DLC *pPacket, SHORT sDataLen);
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

#include <string.h>
/*-----------------------------------------------------------------------
:   SHORT IpSend(IPADDR Faddr, DLC *pPacket, SHORT sDataLen);
:
:   Faddr       foreign IP address
:   pPacket     Packet to send
:   sDataLen    Data Length
:
:   Return      = OK if successful
:
:   "IpSend" fills in IP header and sends datagram to specified address.
------------------------------------------------------------------------*/
SHORT IpSend(IPADDR Faddr, DLC *pPacket, SHORT sDataLen)
{
    IP *ipptr;

    pPacket->usPType = hs2net(DP_IP);
    ipptr = (IP *) pPacket->auchData;
    ipptr->i_verlen  = IVERLEN;
    ipptr->i_svctyp  = ISVCTYP;
    ipptr->i_paclen  = (SHORT)hs2net( (SHORT) (sDataLen+IPHLEN) );
    ipptr->i_id = hs2net(ipackid++);
    ipptr->i_fragoff = hs2net(IFRAGOFF);
    ipptr->i_tim2liv = ITIM2LIV;
    ipptr->i_chksum  = 0;
    GetAddr(ipptr->i_src);
    memcpy(ipptr->i_dest, Faddr, sizeof(IPADDR));
    ipptr->i_chksum = cksum(ipptr, IPHLEN>>1);
    return( Route(Faddr, pPacket, (SHORT)(DHLEN+IPHLEN+sDataLen) ) );
}

