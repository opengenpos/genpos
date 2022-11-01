/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Write Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : DGWRITE.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   DgWrite(DEVICE *pDevice, VOID *pBuff, SHORT sBytes);
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
:                                     Third argument (sBytes) changed from data
:                                     length to a buffer length including header
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT   DgWrite(DEVICE *pDevice, XDGRAM *pBuff, SHORT sBytes);
:
:   pDevice     Pointer to the device table entry
:   pBuff       a packet to send
:   sBytes      size of the packet
:
:   Return      = OK if successful
:               = XINU_ACCESS_DENIED if physical drive is not ready
:               = XINU_NETWORK_ERROR if fail to send to the target node
:               = SYSERR if incorrect argument
:
:   "DgWrite" writes one datagram to a datagram protocol pseudo-device.
------------------------------------------------------------------------*/
SHORT   DgWrite(DEVICE *pDevice, XDGRAM *pBuff, SHORT sBytes)
{
    static  DLC  Packet;
    DLC     *pDlc;
    IP      *pIp;
    UDP     *pUdp;
    DGBLK   NEAR *pDgblk;
    SHORT   dstport;
    CHAR    *pdstIP;
    IPADDR  Faddr;

    pDgblk  = &dgtab[pDevice->dvminor];
    if ( pDgblk->dg_mode & DG_NMODE ) {
        sBytes -= XGHLEN;                    /* get data length */
    }
    if ( sBytes < 0 || sBytes > UMAXLEN )
        return(SYSERR);
    pDlc = &Packet;
    pIp  = (IP *)pDlc->auchData;
    pUdp = (UDP *)pIp->i_data;
    dstport = pDgblk->dg_fport;
    pdstIP  = (CHAR *)pDgblk->dg_faddr;
    if ( pDgblk->dg_mode & DG_NMODE ) {
        if (dstport == 0 ) {
            dstport = pBuff->xg_fport;
            pdstIP  = (CHAR *)pBuff->xg_faddr;
        }
        blkcopy(pUdp->u_data, pBuff->xg_data, sBytes);
    } else {
        if (dstport == 0) {
            return(SYSERR);
        }
        blkcopy(pUdp->u_data, (char *) pBuff, sBytes);
    }
    memcpy(Faddr, pdstIP, sizeof(IPADDR));
    return( UdpSend(Faddr, dstport, pDgblk->dg_lport, pDlc, sBytes) );
}


