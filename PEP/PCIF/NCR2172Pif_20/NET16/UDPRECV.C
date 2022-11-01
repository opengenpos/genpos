/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Receive UDP Datagram
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : UDPRECV.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   UdpRecv(DLC *pRcvBuff, SHORT sBuffLen);
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
:   SHORT    PROCESS UdpRecv(DLC *pRcvBuff, SHORT sBuffLen);
:
:   sDevice     Network Input Device ID
:   pRcvBuff    Receive Buffer pointer
:   sBuffLen    size of Receive Buffer
:
:   Return      = data length if successful
:               = TIMEOUT if timer expired
:               = SYSERR if incorrect argument
:
:   "UDPRecv" receives packets from sDevice.
------------------------------------------------------------------------*/
SHORT   UdpRecv(DLC *pRcvBuff, SHORT sBuffLen)
{
    UDP     *pUdp;
    IP      *pIp;
    NETQ    NEAR *nqptr;
    SHORT   dport, sStatus;
    SHORT   i;
    IPADDR  addr;
    ICMP    *pIcmp;
    SHORT   len;

    sStatus = IpRecv(pRcvBuff, sBuffLen);
    if ( sStatus <= 0 ) {
        return (sStatus);
    }
    if ( sStatus <= (SHORT)(UHLEN+IPHLEN+DHLEN) ) {
        sStatus = 0;
        return (sStatus);
    }
    pIp = (IP *)pRcvBuff->auchData;

    switch (pIp->i_proto) {

    case IPRO_ICMP:                 /* ICMP: pass to icmp input routine */

        pIcmp = (ICMP *) pIp->i_data;
        switch (pIcmp->ic_typ) {

        case ICRQECH:                           /* echo request message ? */
            pIcmp->ic_typ = (CHAR) ICRPECH;   /* make echo reply message */
            memcpy(pIp->i_dest, pIp->i_src, sizeof(IPADDR));
            len = (SHORT)(net2hs(pIp->i_paclen) - IPHLEN);
            if (isodd(len)) {
                pIp->i_data[len++] = NULLCH;
            }
            pIcmp->ic_cksum = 0;
            pIcmp->ic_cksum = cksum( (VOID *)pIcmp, (SHORT)(len>>1) );
            memcpy(addr, pIp->i_dest, sizeof(IPADDR));
            IpSend(addr, pRcvBuff, len);
            sStatus = 0;
            return (sStatus);

        default:
            sStatus = 0;
            return (sStatus);
        }

    case IPRO_UDP:                  /* UDP: demultiplex based on UDP "port" */
        pUdp = (UDP *) pIp->i_data;
        dport  = net2hs(pUdp->u_dport);
        for (i=0 ; i<NETQS ; i++) {
            nqptr = &Net.netqs[i];
            if (nqptr->uport == dport) {
                return(sStatus);
            }
        }
        Net.ndrop++;
        sStatus = 0;
        return (sStatus);

    default:
        Net.ndrop++;
        sStatus = 0;
        return (sStatus);
    }
}

