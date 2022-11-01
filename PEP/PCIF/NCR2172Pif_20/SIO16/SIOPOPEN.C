/*
*===========================================================================
* Title       : Open Function
* Category    : Serial I/O Driver for PC, NCR 2170 Operating System
* Program Name: SIOPOPEN.C
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
* Aug-10-92:00.00.01:O.Nakada   : Initial
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
#include <windows.h>
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
** Synopsis:    SHORT SioInit(DEVICE *pDevTbl, ...)
*     Input:    pDevTbl  - Device Table
*
** Return:      OK - Successful
*
** Description:
*===========================================================================
*/
SHORT SioOpen(DEVICE *pDevTbl, ...)
{
	SHORT	sResult;
	ULONG	ulBaudRate;
	UCHAR	uchPort, uchByteSize, uchParity, uchStopBits;

	// still opened ?

    if (SioData.fchStatus & SIO_OPEN)		// already opened
	{
        return (SYSERR);
    }

	// get communication parameters

	uchPort      = (UCHAR)((*(COMCONTROL **)(&pDevTbl + 1))->uchPort) & 0x7F;
	ulBaudRate   = (ULONG) (*(COMCONTROL **)(&pDevTbl + 1))->usBaudRate;
	uchByteSize  = (UCHAR) (*(COMCONTROL **)(&pDevTbl + 1))->uchByteSize;
	uchParity    = (UCHAR) (*(COMCONTROL **)(&pDevTbl + 1))->uchParity;
	uchStopBits  = (UCHAR) (*(COMCONTROL **)(&pDevTbl + 1))->uchStopBits;

	// check baud rate

    switch (ulBaudRate)
	{
    case 300:
    case 600:
    case 1200:
    case 2400:
    case 4800:
    case 9600:
    case 19200:
	case 38400:
	case 57600:
	case 115200:
        break;
    default:
        return (SIO_OPEN_BAUD);
    }

	// check parity
    switch (uchParity)
	{
    case 0:									// no parity ?
        uchParity = NOPARITY;
        break;
    case 1:									// odd parity ?
        uchParity = ODDPARITY;
        break;
    case 2:									// even parity ?
        uchParity = EVENPARITY;
        break;
    case 3:									// mark parity ?
        uchParity = MARKPARITY;
        break;
    default:								// unknown ...
        return (SIO_OPEN_CONTROL);
    }

	// check data length

    switch (uchByteSize)
	{
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        break;
    default:								// unknown ...
        return (SIO_OPEN_CONTROL);
    }

	// check stop bit

    switch (uchStopBits)
	{
    case 1:									// 1 stop bit ?
        uchStopBits = ONESTOPBIT;
        break;
    case 0:									// 1.5 stop bits ?
        uchStopBits = ONE5STOPBITS;
        break;
    case 2:									// 2 stop bits ?
        uchStopBits = TWOSTOPBITS;
        break;
    default:								// unknown ...
        return (SIO_OPEN_CONTROL);
    }

	// open a serial port

	sResult = SioOpen32(uchPort, ulBaudRate, uchByteSize, uchParity, uchStopBits);

	if (sResult != SIO32_OK)
	{
        return (SIO_OPEN_CONTROL);			// exit ...
	}

	// SIO device opened

    SioData.fchStatus |= SIO_OPEN;

	// exit ...

    return (pDevTbl->dvnum);
}


///////////////////////////////	 End of File  ///////////////////////////////
