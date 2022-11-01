/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Data Link Control Protocol
:  Category           : 2170 XINU Operating System
:  Program Name       : DLC.H
:  -----------------------------------------------------------------------
:  Description : This header file describes the Data Link Control
:                used by the 2170 XINU Operating System.
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 6.00A by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /ACw /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name       Description
:  00.00.00  Mar 31,88   D.Comer    Original PC-XINU, ether.h
:  00.00.01  Jul  3,92   Y.Nozawa   Applied coding convension of 2170
:                                   Moved physical layer dependent descriptions
:                                       to ihc.h
:
\*------------------------------------------------------------------------*/

typedef struct dlc {                /* complete structure of DLC packet     */
    UCHAR   uchSrc;                 /* source host address                  */
    UCHAR   uchDest;                /* destination host address             */
    USHORT  usPType;                /* arcnet packet type (see below)       */
    UCHAR   auchData[DDLEN];        /* data in packet                       */
} DLC;

#define DP_LOOP     0x0060          /* packet type: Loopback                */
#define DP_ECHO     0x0200          /* packet type: Echo                    */
#define DP_PUP      0x0400          /* packet type: Xerox PUP protocol      */
#define DP_IP       0x0800          /* packet type: DARPA Internet protocol */
#define DP_ARP      0x0806          /* packet type: Address resolution  "   */
#define DP_RARP     0x8035          /* packet type: reverse     "       "   */
