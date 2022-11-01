/*
*===========================================================================
* Title       : Misc Function
* Category    : Serial I/O Driver for PC, NCR 2170 Operating System
* Program Name: SIOPMISC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Compact Model
* Options     : /c /Asfu /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Aug-11-92:00.00.01:O.Nakada   : Initial
* Mar-28-95:03.00.00:O.Nakada   : Correspond to Windows 3.1 API
*          :        :           :
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <conf.h>
#include <kernel.h>
#include "sio.h"


/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/


/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    int SioSetErrorCode(VOID)
*
** Return:      Serial Port Status
*               0    - Success
*               -57  - CTS and DSR Low
*               -59  - CTS Low
*               -60  - DSR Low
*               -63  - Framing Error
*
** Description: This function convert to Error Code for SIO driver form
*               error code for windows.
*===========================================================================
*/
int SioSetErrorCode(VOID)
{
	return (SYSERR);
#if	0
    int nStatus;
    COMSTAT ComStatus;

    nStatus = GetCommError(SioData.hComm, &ComStatus);

    /* ----- check CTS low and DSR low ----- */
#if (WINVER >= 0x030a)
    if (((ComStatus.status & CSTF_CTSHOLD) && (ComStatus.status & CSTF_DSRHOLD)) ||
#else                                       /* WINVER >= 0x030a */
    if (((ComStatus.fCtsHold == 1) && (ComStatus.fDsrHold == 1)) ||
#endif                                      /* WINVER >= 0x030a */
        ((nStatus & CE_CTSTO) && (nStatus & CE_DSRTO))) {
#if PC_DEBUG
        MessageBox(NULL, "CTS and DSR Low", NULL, MB_OK);
#endif
        return (SIO_ERROR_CTS_DSR);
    }
    /* ----- check CTS low ----- */
#if (WINVER >= 0x030a)
    else if ((ComStatus.status & CSTF_CTSHOLD) ||
#else                                       /* WINVER >= 0x030a */
    else if ((ComStatus.fCtsHold == 1) ||
#endif                                      /* WINVER >= 0x030a */
             (nStatus & CE_CTSTO)) {
#if PC_DEBUG
        MessageBox(NULL, "CTS Low", NULL, MB_OK);
#endif
        return (SIO_ERROR_CTS);
    }
    /* ----- check DSR low ----- */
#if (WINVER >= 0x030a)
    else if ((ComStatus.status & CSTF_DSRHOLD) ||
#else                                       /* WINVER >= 0x030a */
    else if ((ComStatus.fDsrHold == 1) ||
#endif                                      /* WINVER >= 0x030a */
             (nStatus & CE_DSRTO)) {
#if PC_DEBUG
        MessageBox(NULL, "DSR Low", NULL, MB_OK);
#endif
        return (SIO_ERROR_DSR);
    }
    else if (nStatus & CE_FRAME) {          /* Framing Error */
#if PC_DEBUG
        MessageBox(NULL, "Framing Error", NULL, MB_OK);
#endif
        return (SIO_ERROR_FRAMING);
    }
    else if (nStatus & CE_MODE) {           /* no Port Address */
#if PC_DEBUG
        MessageBox(NULL, "no Port Address", NULL, MB_OK);
#endif
        return (SIO_ERROR_NO_PORT);
    }
    else if (nStatus & CE_OVERRUN) {        /* Over Run Error */
#if PC_DEBUG
        MessageBox(NULL, "Over Run Error", NULL, MB_OK);
#endif
        return (SIO_ERROR_OVERRUN);
    }
    else if (nStatus & CE_RXPARITY) {       /* Parity Error */
#if PC_DEBUG
        MessageBox(NULL, "Parity", NULL, MB_OK);
#endif
        return (SIO_ERROR_PARITY);
    }
    else if ((nStatus & CE_RXOVER) ||       /* Buffer Overflow */
             (nStatus & CE_TXFULL)) {
#if PC_DEBUG
        MessageBox(NULL, "Buffer Overflow", NULL, MB_OK);
#endif
        return (SIO_ERROR_OVERFLOW);
    }
    else if (nStatus == 0) {
        return (nStatus);                   /* Successful */
    }
    else {
#if PC_DEBUG
        MessageBox(NULL, "Other Error", NULL, MB_OK);
#endif
        return (SYSERR);                    /* Other Error */
    }
#endif
}

