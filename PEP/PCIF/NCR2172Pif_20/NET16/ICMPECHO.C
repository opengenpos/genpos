/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : ICMP Echo Message Send & Receive
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : ICMPECHO.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT IcmpEcho(IPADDR Faddr, CHAR CONST *pchText, SHORT sTextLen, SHORT sSeq)
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
#include <dgram.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT IcmpEcho(IPADDR Faddr, CHAR CONST *pchText, SHORT sTextLen, SHORT sSeq)
:
:   Faddr       foreign IP address
:   pchText     Text to send 
:   sTextLen    Text Length
:   sSeq        message sequence number
:
:   Return      = OK if successful
:               < 0  if error
:
:   "IcmpEcho" performs pingpong test.
------------------------------------------------------------------------*/
SHORT IcmpEcho(IPADDR Faddr, CHAR CONST *pchText, SHORT sTextLen, SHORT sSeq)
{
    static  DLC   ReqDlc, RepDlc;
    DLC     *pReqDlc, *pRepDlc;
    IP      *pRepIp, *pReqIp;
    ICMP    *pReqIcmp, *pRepIcmp;
    SHORT   sStatus;
    USHORT  usTimeLmt, usTimeCtr;

    /*  Fill in ICMP header; pass to ipsend to fill in IP header */

    pReqDlc = &ReqDlc;
    pRepDlc = &RepDlc;
    pReqIp = (IP *)pReqDlc->auchData;
    pReqIp->i_proto = IPRO_ICMP;
    pReqIcmp = (ICMP *) pReqIp->i_data;
    pReqIcmp->ic_typ = (CHAR) ICRQECH;  /* make echo request message */
    pReqIcmp->ic_code = 0;
    pReqIcmp->ic_cksum = 0;
    pReqIcmp->ic_id = 0;
    pReqIcmp->ic_seq = sSeq;
    blkcopy(pReqIcmp->ic_data, pchText, sTextLen);
    if (isodd(sTextLen)) {
        pReqIcmp->ic_data[sTextLen++] = NULLCH;
    }
    pReqIcmp->ic_cksum = cksum( (VOID *)pReqIcmp, (SHORT)( (UHLEN+sTextLen)>>1 ) );

    /* send a echo request message */

    if ( (sStatus = IpSend(Faddr, pReqDlc, (SHORT)(UHLEN+sTextLen) ) ) >= 0) {

        /* receive a reply echo message */

        usTimeLmt = (USHORT)(10000);		/* 10 sec */

        for (usTimeCtr = (USHORT)0; usTimeLmt > usTimeCtr ; ) {
            if ( (sStatus = IpRecv(pRepDlc, sizeof(RepDlc) ) ) == TIMEOUT ) {
                usTimeCtr += (USHORT)ASY_TO_TIME;
            }
            else if (sStatus < 0) {
                break;
            }
            else if (sStatus <= (SHORT)(UHLEN+DHLEN+IPHLEN)) {
                                       /* ICMP header length is same as UHLEN */
                sStatus = TIMEOUT;
                XinuSleepm( (USHORT)SLEEP_TIME);
                usTimeCtr += (USHORT)SLEEP_TIME;
            }
            else {
                break;
            }
        }

        if (sStatus > 0) {
            pRepIp = (IP *)pRepDlc->auchData;
            pRepIcmp = (ICMP *)pRepIp->i_data;
            if (( pRepIcmp->ic_typ != ICRPECH ) ||
                ( pRepIcmp->ic_seq != sSeq )    ||
                ( sTextLen != net2hs(pRepIp->i_paclen) - IPHLEN - UHLEN ) ||
                ( memcmp(pReqIcmp->ic_data, pRepIcmp->ic_data, sTextLen) != 0 ) ) {

                sStatus = XINU_MESSAGE_ERROR;
            } else {
                sStatus = OK;
            }
        }
    }

    return(sStatus);
}

