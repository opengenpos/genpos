/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Network Start Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : NETSTART.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   XinuNetStart(COMCONTROL *pControl, UCHAR CONST *puchIPAddr);
:
:  -----------------------------------------------------------------------
:  Compile            : MS-C Ver. 7.00 by Microsoft Corp.
:  Memory Model       : Compact Model
:  Compile Option     : /c /Asfu /Za /Zp /W4 /FPa /Gs
:  -----------------------------------------------------------------------
:  Update Histories
:  Ver.      Date        Name         Description
:  00.00.01  Jul 30,92   Y.Nozawa     Original
:  00.00.02  Aug 18,92   T.Kunishige  Changed for PEP
:
\*------------------------------------------------------------------------*/

#include <conf.h>
#include <kernel.h>
#include <network.h>

#include <string.h>

/*-----------------------------------------------------------------------
:   SHORT XinuNetStart(COMCONTROL *pControl, UCHAR CONST *puchIPAddr);
:
:   Return      = OK
:
:   "XinuNetStart" starts network services.
------------------------------------------------------------------------*/
SHORT XinuNetStart(COMCONTROL *pControl, UCHAR CONST *puchIPAddr)
{
    NETQ    NEAR *nqptr;
    SHORT   i, status;

    if (Net.chStarted) {
        return(XINU_ALREADY_EXIST);
    }

    status = XinuOpen(ASY, pControl, (UCHAR)0);     /* src ID is fixed as 0 */
    if (status == OK) {
        Net.chStarted = TRUE;
    }
    else {
        return(status);
    }

    blkcopy(Net.myaddr, puchIPAddr, sizeof(IPADDR));

    /* initialize network input queues */
    for (i=0 ; i<NETQS ; i++) {
        nqptr = &Net.netqs[i];
        nqptr->valid = FALSE;
        nqptr->uport = -1;
    }

    /* create network input-output forward queue */

    /* ICMP input queue */
    Net.chIcmpPid = BADPID;

    return(status);
}
