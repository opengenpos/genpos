/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Datagram Pseudo Device Read Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : DGREAD.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   DgRead(DEVICE *pDevice, VOID *pBuff, SHORT sBytes);
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
:  00.00.02  Aug 14,92   T.Kunishige  Changed for PEP
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <dgram.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT   DgRead(DEVICE *pDevice, VOID *pBuff, SHORT sBytes);
:
:   pDevice     Pointer to the device table entry
:   pBuff       buffer to read a packet
:   sBytes      size of pBuff
:
:   Return      = data length if successful
:               = TIMEOUT if timer expired
:               = SYSERR if incorrect argument
:
:   "DgRead" reads one datagram from a datagram protocol pseudo-device.
------------------------------------------------------------------------*/
SHORT   DgRead(DEVICE *pDevice, VOID *pBuff, SHORT sBytes)
{
    DGBLK   NEAR *pDgblk;
    DLC     *pDlc, RcvBuff;
    UDP     *pUdp;
    IP      *pIp;
    SHORT   datalen, len;
    SHORT   sStatus = TIMEOUT;
    USHORT  usTimeLmt, usTimeCtr;

    pDlc = &RcvBuff;
    len = sizeof(RcvBuff);
    pDgblk = &dgtab[pDevice->dvminor];
    if (pDgblk->dg_mode & DG_TMODE) {
        usTimeLmt = (USHORT)pDgblk->dg_time;
    }
    else {
        usTimeLmt = (USHORT)(10000);		/* 10 sec */
    }
    for (usTimeCtr = (USHORT)0; usTimeLmt > usTimeCtr ; ) {
        if ( (sStatus = UdpRecv(pDlc, len)) == TIMEOUT ) {
            usTimeCtr += (USHORT)ASY_TO_TIME;
        }
        else if (sStatus == 0) {
            XinuSleepm( (USHORT)SLEEP_TIME);
            usTimeCtr += (USHORT)SLEEP_TIME;
            sStatus = TIMEOUT;
        }
        else {
            break;
        }
    }

    if (sStatus <= 0) {
       return (sStatus);
    }

    /* copy data into user's buffer & set length */

    pIp   = (IP *) pDlc->auchData;
    pUdp  = (UDP *)pIp->i_data;
    datalen = (SHORT)(net2hs(pUdp->u_udplen) - UHLEN);

    if (pDgblk->dg_mode & DG_NMODE) {
        if ((datalen+XGHLEN) > sBytes ) {
            return(SYSERR);
        }
        blkcopy(((XDGRAM *)pBuff)->xg_faddr, pIp->i_src, IPLEN);
        ((XDGRAM *)pBuff)->xg_fport = net2hs(pUdp->u_sport);
        ((XDGRAM *)pBuff)->xg_lport = pDgblk->dg_lport;
        blkcopy(((XDGRAM *)pBuff)->xg_data, pUdp->u_data, datalen);
        datalen += XGHLEN;
    } else {
        if (datalen > sBytes ) {
            return(SYSERR);
        }
        blkcopy(pBuff, pUdp->u_data, datalen);
    }
    return(datalen);
}
