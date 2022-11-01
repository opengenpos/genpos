/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Network Description
:  Category           : 2170 XINU Operating System
:  Program Name       : NETWORK.H
:  -----------------------------------------------------------------------
:  Description : This header file includes the following header files:
:
:                   dlc.h       describes Data Link Control
:                   ip.h        describes Internet Protocol
:                   icmp.h      describes Internet Control Message Protocol
:                   udp.h       describes User Datagram Protocol
:                   net.h       describes general network definition
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 6.00A by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /ACw /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
\*------------------------------------------------------------------------*/

/*

Datagram interface (master psedo device)

dgm:
      on DLC      -i ionull      -o dgmopen     -c ioerr
                  -r ioerr       -w ioerr       -s ioerr
                  -n ioerr       -g ioerr       -p ioerr
                  -iint ioerr    -oint ioerr    -csr 0
                  -ivec 0        -ovec 0

 Datagram 'connection' (pseudo-device returned by dgopen)

dg:
      on DLC      -i dginit   -o ioerr       -c dgclose
                  -r dgread   -w dgwrite     -s ioerr
                  -n dgcntl   -g ioerr       -p ioerr
                  -iint ioerr -oint ioerr    -csr 0
                  -ivec 0     -ovec 0
%

 Datagram network interface (master psedo device)

INTERNET    is dgm   on DLC

 Pseudo-device slots for Datagram 'connection'

DGRAM1      is dg    on DLC  
DGRAM2      is dg    on DLC  
DGRAM3      is dg    on DLC  
DGRAM4      is dg    on DLC  
----------------------------------------------------------------------------

  +------+   +------+     +-------+   +------+ +-------+ +-------+
  |dgread|   |dgcntl|     |dgwrite|   |dginit| |dgmopen| |dgclose|  UDP
  |__142_|   |__144_|     |__140__|   |__132_| |__136__| |__138__|
     A                        |                    |
     |                        V                    V
     |                    +---------+       +-------------+
     |                    | udpsend |       | dgalloc 133 |
     |                    |__110____|       | nqalloc 135 |
     |                        |             |_____________|
     |                        |
----------------------------------------------------------------------------
     |    .............       V
  +-----+ : +-------+ :   +------+
  |ip_in|-->|icmp_in|---->|ipsend|
  |_123_| : |__102__| :   |__90__|
     A    :           :      |
     |    :           :      |
     |    :           :      V
     |    :           :   +-----+
     |    :           :   |route|----+-----------+
     |    :           :   |__92_|    |           |
.....|....:  NETWORK  :......|.......|........   |
:    |                       V       |       :   |
:    |     +-------+      +-------+  |       :   |
:    +---->|rarp_in|      |getpath|--)----+  :   |
:    |     |__71___|      |__78___|  |    |  :   |
:    |                       |<------+    |  :   |
:    |                       V            |  :   |             IP
:    |     +------+       +-------+       |  :   |
:    +---->|arp_in|       |getaddr|       |  :   |
:    |     |__81__|       |__160__|       |  :   |
:    |                       |            |  :   |
:    |                       V            |  :   |
:    |                    +-------+       |  :   |
:    |                    |sndrarp|       |  :   |
:    |                    |__69___|       |  :   |
:....|.......................|............|..:   |
     |                       |            V      V
   (netin) 118               |<-----------+------+
     |                       |
----------------------------------------------------------------------------
     A                       |
     |                       V
  +----+                  +-----+                     DLC
  |read|                  |write|
  |____|                  |_____|
----------------------------------------------------------------------------
*/

#include <dlc.h>
#include <ip.h>
#include <icmp.h>
#include <udp.h>
#include <net.h>

/* Declarations data conversion and checksum routines */

SHORT   net2hs(SHORT sWord);                /* network to host USHORT       */
SHORT   hs2net(SHORT sWord);                /* host to network USHORT       */
ULONG   net2hl(ULONG ulDWord);              /* network to host ULONG        */
ULONG   hl2net(ULONG ulDWord);              /* host to network ULONG        */
SHORT   cksum(VOID *pBuff, SHORT sWords);   /* 1s comp of 16-bit 2s comp sum*/
