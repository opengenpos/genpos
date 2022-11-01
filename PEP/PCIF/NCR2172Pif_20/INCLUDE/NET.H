/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : General Network Declarations
:  Category           : Network, 2170 XINU Operating System
:  Program Name       : NET.H
:  -----------------------------------------------------------------------
:  Description : This header file describes network variables and conttants
:                used by the 2170 XINU Operating System.
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Zp /W4 /G2s
:  -----------------------------------------------------------------------
:  Update Histories
:  00.00.00  Mar 31,88   D.Comer      Original PC-XINU, ether.h
:  00.00.01  Jul  3,92   Y.Nozawa     Applied coding convension of 2170
:                                     Added function prototypes
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
\*------------------------------------------------------------------------*/

/* High-level network definitions and constants */

#define BADPID          -4              /* bad process ID                   */
#define NULLCH          '\0'            /* NULL character                   */
#define NETBUFS         4               /* number of network buffers        */
#ifndef Ndg
#define NETQS           1               /* number of xinu ports used to     */
#else                                   /*  demultiplex udp datagram        */
#define NETQS           Ndg
#endif
#define NETQLEN         3               /* size of a demux queue            */
#define NETFQ           3               /* size of input-to-output queue    */
#ifndef NETNLEN
#define NETNLEN         24              /* length of network name           */
#endif

/* Commands for the network pseudo device control call */

#define NC_SETGW        1               /* set gateway IP address           */

#define NETISTK         STK_NETIN_IH    /* stack size for network input     */
#define NETOSTK         STK_NETOUT      /* stack size for network output    */
#define NETIPRI         PRIO_NETIN_IH   /* network runs at high priority    */
#define NETOPRI         PRIO_NETOUT     /* network output priority          */
#define NETINAM         "NETIN_IH"      /* name of In-House network input p.*/
#define NETONAM         "NETOUT"        /* name of network output process   */
#define NETIARGC        2               /* count of args to net input       */
#define NETOARGC        1               /* count of args to net output      */
#define NETISTK2        STK_NETIN_PC    /* stack size for network input     */
#define NETIPRI2        PRIO_NETIN_PC   /* network output priority          */
#define NETINAM2        "NETIN_PC"      /* name of PC network input process*/

/* Mapping of external network UDP "port" to internal Xinu port */

typedef struct  netq {                  /* UDP demultiplexing info          */
        CHAR    valid;                  /* is this entry in use ?           */
        CHAR    pid;                    /* pid of process if one waiting    */
        SHORT   uport;                  /* local UDP "port" number          */
        SHORT   xport;                  /* Corresponding xinu port on       */
} NETQ;                                 /*  which incoming pac. queued      */

typedef struct  netinfo {               /* info and parms. for network      */
        CHAR    chStarted;              /* network is started               */
        CHAR    chFreeQ;                /* idle chain for network buffer    */
/*      CHAR    mnvalid;            */  /* is my host name field valid?     */
/*      CHAR    mavalid;            */  /* is my network address valid?     */
/*      CHAR    myname[NETNLEN];    */  /* domain name of this machine      */
        IPADDR  myaddr;                 /* IP address of this machine       */
        IPADDR  mynet;                  /* Network portion of myaddr        */
/*      IPADDR  gateway;            */  /* IP address of gateway to use     */
        SHORT   nxtprt;                 /* next available local UDP port    */
        USHORT  npacket;                /* # of packets processed           */
        USHORT  ndrop;                  /* # of packets discarded           */
        USHORT  nover;                  /* # dropped because queue full     */
        CHAR    nmutex;                 /* output mutual excl. semaphore    */
        CHAR    chForward;              /* network input-output queue       */
        CHAR    chIcmpSem;              /* ICMP mutual excl. semaphore      */
        CHAR    chIcmpPid;              /* user process ID of ICMP          */
        CHAR    chIcmpQ;                /* ICMP input queue                 */
        CHAR    chReserved;             /* align to 2 bytes boundary        */
        NETQ    netqs[NETQS];           /* UDP packet demux info            */
} NETINFO;

extern  NETINFO NEAR Net;               /* All network parameters           */

#define blkcopy(a,b,c)  memcpy(a,b,c)

VOID    GetAddr(IPADDR MyAddr);
