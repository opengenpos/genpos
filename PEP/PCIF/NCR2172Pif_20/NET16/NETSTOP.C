/*========================================================================*\
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                                          ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:  Title              : Network Stop Routine
:  Category           : UDP/IP, 2170 XINU Operating System
:  Program Name       : NETSTOP.C
:  -----------------------------------------------------------------------
:  Description : This module contains the following function:
:
:       SHORT   XinuNetStop(VOID);
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

/*-----------------------------------------------------------------------
:   SHORT XinuNetStop(VOID);
:
:   Return      = OK if successful
:               = XINU_NOT_FOUND if not started yet
:
:   "NetStop" stops network services.
------------------------------------------------------------------------*/
SHORT XinuNetStop(VOID)
{
    SHORT   i, status;
    NETQ    NEAR *nqptr;

    if (!Net.chStarted) {
        return(XINU_NOT_FOUND);
    }

    status = XinuClose(ASY);		                /* ASY STOP */
    if (status == OK) {
        Net.chStarted = FALSE;
    }
    else {
        return(status);
    }

    /* delete network intput queues */
    for ( i=0 ; i<NETQS ; i++) {
        nqptr = &Net.netqs[i];
        nqptr->valid = FALSE;
        nqptr->xport = -1;
    }

    /* delete network input-output forward queue */
    Net.chForward = -1;

    /* delete ICMP input queue */
    Net.chIcmpQ = -1;

    return(status);

}
