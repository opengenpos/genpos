/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Internet Control Message Protocol
:  Category           : 2170 XINU Operating System
:  Program Name       : ICMP.H
:  -----------------------------------------------------------------------
:  Description : This header file describes the Internet Control Message 
:                Protocol used by the 2170 XINU Operating System.
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
:  00.00.02  Aug 18,92   T.Kunishige  Change for PEP
\*------------------------------------------------------------------------*/

/* Internet Control Message Protocol Constants and Packet Format */

#define ICRPECH 0                      /* Echo reply                   */
#define ICDESTU 3                      /* Destination unreachable      */
#define ICSRCQN 4                      /* Source quench                */
#define ICREDIR 5                      /* Redirect message type        */
#define ICRQECH 8                      /* Echo request                 */
#define ICTIMEX 11                     /* Time exceeded                */
#define ICPARMP 12                     /* Parameter Problem            */
#define ICTIMST 13                     /* Timestamp message            */
#define ICTIMRP 14                     /* timestamp reply              */
#define ICRQINF 15                     /* Informatiom request          */
#define ICRPINF 16                     /* Information reply            */

/* ICMP packet format (part of the packet that follows the IP header) */

typedef struct  icmp   {               /* ICMP packet                  */
        char   ic_typ;                 /* type of message (see above)  */
        char   ic_code;                /* code (often zero)            */
        short  ic_cksum;               /* checksum of icmp header+data */
        short  ic_id;                  /* for echo type, a message id  */
        short  ic_seq;                 /* for echo type, a seq. number */
        char   ic_data[1];             /* data area of ICMP message    */
} ICMP;

SHORT   IcmpEcho(IPADDR Faddr, CHAR CONST *pchText, SHORT sTextLen, SHORT sSeq);
