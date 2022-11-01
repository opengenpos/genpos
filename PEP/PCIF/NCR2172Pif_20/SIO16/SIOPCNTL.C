/*
*===========================================================================
* Title       : I/O Control Function
* Category    : Serial I/O Driver for PC, NCR 2170 Operating System
* Program Name: SIOPCNTL.C
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
* Jun-15-98:        :O.Nakada   : Win32
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
#include <conf.h>
#include <kernel.h>
#include "sio.h"
#include "sio32.h"


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
** Synopsis:    SHORT SioCntl(DEVICE *pDevTbl, SHORT func, ...)
*     Input:    pDevTbl  - Device Table
*               func     - I/O Control Function
*
** Return:      OK     - Successful
*               SYSERR - System Error
*
** Description:
*===========================================================================
*/
SHORT SioCntl(DEVICE *pDevTbl, SHORT sFunc, ...)
{
    if (! (SioData.fchStatus & SIO_OPEN))
	{
        return (SYSERR);
    }

    switch (sFunc)
	{
    case SIO_FUNC_STATUS:					// get comm. status ?
		return (SioCntlGetStatus32());
    case SIO_FUNC_RECEIVE_TIMER:            // set receive timer ?
		SioData.usReceiveTimer = **(USHORT **)((LONG *)&sFunc + 1);

		if (! SioCntlSetTimer32(SIO32_CNTL_RXTIMER, SioData.usReceiveTimer))
		{
			return (SYSERR);				// exit ...
        }
        break;
    case SIO_FUNC_SEND_TIMER:               // set send timer ?
		SioData.usSendTimer = **(USHORT **)((LONG *)&sFunc + 1);

		if (! SioCntlSetTimer32(SIO32_CNTL_TXTIMER, SioData.usSendTimer))
		{
			return (SYSERR);				// exit ...
        }
        break;
    case SIO_FUNC_FLUSH_READ:               // clear the input buffer
        if (! SioCntlClearBuffer32(SIO32_CNTL_RXCLEAR))
		{
			return (SYSERR);
        }
        break;
    case SIO_FUNC_FLUSH_WRITE:              // clears the output buffer
        if (! SioCntlClearBuffer32(SIO32_CNTL_TXCLEAR))
		{
			return (SYSERR);
        }
        break;
    case SIO_FUNC_DTR_ON:
        if (! SioCntlDtr32(TRUE))
		{
			return (SYSERR);
        }
        break;
    case SIO_FUNC_DTR_OFF:
        if (! SioCntlDtr32(FALSE))
		{
			return (SYSERR);
        }
        break;
    default:
		return (SYSERR);
    }

    return (OK);
}


///////////////////////////////	 End of File  ///////////////////////////////
