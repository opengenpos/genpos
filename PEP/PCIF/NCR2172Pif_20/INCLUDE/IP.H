/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Internet Protocol
:  Category           : 2170 XINU Operating System
:  Program Name       : IP.H
:  -----------------------------------------------------------------------
:  Description : This header file describes the Internet Protocol
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

/* DARPA IP (Internet Protocol) constants and datagram formats */

#define IPTMASK (UCHAR)0xE0         /* mask for IP type in top byte of addr */
#define IPATYP  (UCHAR)0x00         /* IP class A address (1st 2 bits)      */
#define IPBTYP  (UCHAR)0x40         /* IP class B address (1st 2 bits)      */
#define IPCTYP  (UCHAR)0xC0         /* IP class C address (1st 2 bits)      */
                                    /* use IPCTYP only in ECR90 project     */


/* Assigned Protocol numbers */

#define IPRO_ICMP       1           /* protocol type for ICMP packets       */
#define IPRO_UDP        17          /* protocol type for UDP packets        */

typedef struct ip   {               /* format of DARPA internat datagram    */
    UCHAR   i_verlen;               /* IP ver.(0x40) + hdr len (5)          */
    UCHAR   i_svctyp;               /* service type ( 0: normal service     */
    SHORT   i_paclen;               /* packet length in byte                */
    SHORT   i_id;                   /* datagram ID (to help gateways frag.) */
    SHORT   i_fragoff;              /* fragment offset (0 for 1st fragment) */
    UCHAR   i_tim2liv;              /* time to live in gateway hops (10)    */
    UCHAR   i_proto;                /* IP protocol (UDP is assigned 17)     */
    SHORT   i_chksum;               /* 1s compl. of sum of shorts in header */
    IPADDR  i_src;                  /* IP address of source                 */
    IPADDR  i_dest;                 /* IP address of destination            */
    UCHAR   i_data[1];              /* IP datagram data area                */
} IP;

/* Datagram field constants used by simple IP software */

#define IVERLEN     0x45            /* current version length value         */
#define ISVCTYP     '\0'            /* service type for normal service      */
#define IFRAGOFF    0               /* fragment offset ( 0: no fragment)    */
#define ITIM2LIV    1               /* initial time-to-live value           */

extern  SHORT   NEAR ipackid;       /* Internet datagram id                 */

SHORT   IpSend(IPADDR Faddr, DLC *pPacket, SHORT sDataLen);
SHORT   Route(IPADDR Faddr, DLC *pPacket, SHORT sTotalLen);
SHORT   IpRecv(DLC *pRcvBuff, SHORT sBuffLen);
