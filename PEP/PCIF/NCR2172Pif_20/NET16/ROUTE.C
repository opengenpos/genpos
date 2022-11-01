/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Route IP Datagram
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : ROUTE.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   Route(IPADDR Faddr, DLC *pPacket, SHORT sTotalLen);
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
:   SHORT Route(IPaddr FADDR, DLC *pPacket, SHORT sTotalLen);
:
:   Faddr       foreign IP address
:   pPacket     Packet to send
:   sTotalLen   Total packet Length
:
:   Return      = OK if successful
:
:   "Route" routes a datagram to a given IP address.
------------------------------------------------------------------------*/
SHORT   Route(IPADDR Faddr, DLC *pPacket, SHORT sTotalLen)
{
    IP     *ipptr;
    SHORT   result;
    SHORT   dev;

    dev = ASY;
    pPacket->uchDest = (UCHAR)1;        /* dest ID is fixed as 1 */
    ipptr = (IP *) pPacket->auchData;
    result = XinuWrite(dev, (char*) pPacket, sTotalLen);
    return(result);
    Faddr = Faddr;
}
