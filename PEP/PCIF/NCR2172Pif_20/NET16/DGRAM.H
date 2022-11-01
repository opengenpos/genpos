/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo-device Driver Header
:  Category           : UDPIP Driver, 2170 XINU Operating System
:  Program Name       : DGRAM.H
:  -----------------------------------------------------------------------
:  Description : Definition for Datagram Pseudo-device Device
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Zp /W4 /G2s
:  -----------------------------------------------------------------------
:  Update Histories
\*------------------------------------------------------------------------*/

/* DataGram pseudo-device control block */

typedef struct dgblk {                  /* Datagram device control block*/
        SHORT   dg_dnum;                /* device number of this device */
        SHORT   dg_state;               /* whether this device allocated*/
        SHORT   dg_lport;               /* local datagram port number   */
        SHORT   dg_fport;               /* foreign datagram port number */
        UCHAR   dg_faddr[4];            /* foreign machine IP address   */
        SHORT   dg_xport;               /* incoming packet queue        */
        SHORT   dg_netq;                /* index of our netq entry      */
        SHORT   dg_mode;                /* mode of this interface       */
        SHORT   dg_time;                /* timeout value for receive msg*/
} DGBLK;

/* Datagram psuedo-device state constants */

#define DG_FREE         0               /* this device is available     */
#define DG_USED         1               /* this device is in use        */

#define DG_TIME         30              /* read timeout (tenths of sec) */

/* Constants for dgm pseudo-device control functions */

#define DGM_GATE        11               /* Set the default gateway      */
      
#define XGHLEN 8         /* error in ( (sizeof(struct xgram) - UMAXLEN) */

/* Constants for port specifications on INTERNET open call */

#define ANYFPORT        (CHAR *)0       /* Accept any foreign UDP port  */
#define ANYLPORT        0               /* Assign a fresh local port num*/

#define  ASY_TO_TIME    (54*2*9*6)      /* msec */
#define  SLEEP_TIME     (54*2*2)        /* msec */

extern  DGBLK   NEAR dgtab[Ndg];

SHORT   DgAlloc(VOID);
SHORT   NqAlloc(VOID);


