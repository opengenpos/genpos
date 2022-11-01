/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : User Datagram Protocol
:  Category           : 2170 XINU Operating System
:  Program Name       : UDP.H
:  -----------------------------------------------------------------------
:  Description : This header file describes the User Datagram Protocol
:                used by the 2170 XINU Operating System.
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Zp /W4 /G2s
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name         Description
:  00.00.00  Mar 31,88   D.Comer      Original PC-XINU, ether.h
:  00.00.01  Jul  3,92   Y.Nozawa     Applied coding convension of 2170
:                                     Added function prototypes
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
\*------------------------------------------------------------------------*/

/* DARPA User Datagram Protocol (UDP) formats */

typedef struct  udp   {             /* message format of DARPA UDP          */
        SHORT   u_sport;            /* source      UDP port number          */
        SHORT   u_dport;            /* destination UDP port number          */
        SHORT   u_udplen;           /* length of UDP data                   */
        SHORT   u_ucksum;           /* UDP check-sum (0x0=no check-sum)     */
        CHAR    u_data[UMAXLEN];    /* data field in UDP message            */
} UDP;

/* UDP constants */

#define UPROTO      (CHAR)17        /* IP protocol type ( 17 == UDP )       */
#define ULPORT      2050            /* initial UDP local port number        */

/* Assigned UDP port numbers */

#define UECHO       7               /* echo server                          */
#define UDISCARD    9               /* discard packet                       */
#define UUSERS      11              /* users server                         */
#define UDAYTIME    13              /* day and time server                  */
#define UQOTD       17              /* quote of the day server              */
#define UCHARGEN    19              /* character generator                  */
#define UTIME       37              /* time server                          */
#define UWHOIS      43              /* who is server (user information)     */
#define UDNAME      53              /* DARPA domain name server             */
#define UTFTP       69              /* trivial file transfer protocol server*/
#define URWHO       513             /* remote who server (ruptime)          */


/* assign ANYLPORT                      */

SHORT   udpnxtp(VOID);
SHORT   UdpSend(IPADDR Faddr, SHORT sFPort, SHORT sLPort, 
                DLC *pPacket, SHORT sDataLen);
SHORT   UdpRecv(DLC *pRcvBuff, SHORT sBuffLen);
