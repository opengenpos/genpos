/* pifsio.c : Defines the entry point for the DLL application. */

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "piflocal.h"

/* --- I/O --- */
UCHAR  PIFENTRY PifGetKeyLock(VOID)
{
	return 0;
}

SHORT  PIFENTRY PifGetWaiter(VOID)
{
	return 0;
}

USHORT PIFENTRY PifOpenDrawer(USHORT usDrawerId)
{
	return 0;

	usDrawerId = usDrawerId;
}

USHORT PIFENTRY PifDrawerStatus(USHORT usDrawerId)
{
	return 0;

	usDrawerId = usDrawerId;
}

SHORT  PIFENTRY PifOpenMsr(VOID)
{
	return 0;
}

SHORT  PIFENTRY PifReadMsr(MSR_BUFFER FAR *pMsrBuffer)
{
	return 0;

	pMsrBuffer = pMsrBuffer;
}

VOID   PIFENTRY PifCloseMsr(VOID)
{
	return;
}

VOID   PIFENTRY PifDisplayString(USHORT usDispId, USHORT usRow,
                                 USHORT usColumn, CONST TCHAR FAR *puchString,
                                 USHORT usChars, UCHAR uchAttr)
{
	return;

	usDispId = usDispId;
	usRow = usRow;
	usColumn = usColumn;
	puchString = puchString;
	usChars = usChars;
	uchAttr = uchAttr;
}

VOID   PIFENTRY PifDisplayAttr(USHORT usDispId, USHORT usRow, USHORT usColumn,
                               CONST TCHAR FAR *puchString, USHORT usChars)
{
	return;

	usDispId = usDispId;
	usRow = usRow;
	usColumn = usColumn;
	puchString = puchString;
	usChars = usChars;
}

VOID   PIFENTRY PifLightDescr(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr)
{
	return;

	usDispId = usDispId;
	usDescrId = usDescrId;
	uchAttr = uchAttr;
}

//-----------------------------------------------------------------------------------

// The following NetComIoEx() family of functions provide the capability to have a single
// interface to either a communication port or to a network port allowing functionality
// that was dependent on a communication (COM) port only to also be used over a LAN by
// supplying a network address and port.
// These functions use a special identifier in the most significant nibble of the port handle
// to indicate the type of the handle and whether it is a COM port or a network address and port.

//  PifOpenNetComIoEx(USHORT usPortId, CONST PROTOCOL *pProtocol);
//  PifReadNetComIoEx(USHORT usPort, VOID *pBuffer, USHORT usBytes);
//  PifWriteNetComIoEx(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes);
//  PifControlNetComIoEx(USHORT usPort, USHORT usFunc, ...);
//  PifCloseNetComIoEx(USHORT usPort);


// PifOpenNetComIoEx() - used to replace functions PifOpenCom(), PifNetOpen() and PifNetOpenEx().
SHORT  PIFENTRY PifOpenNetComIoEx(USHORT usPortId, CONST PROTOCOL *pProtocol)
{
	SHORT  sHandle = -1;

	switch (pProtocol->fPip) {
		// Use communications port protocol.
		case PIF_COM_PROTOCOL_NON:
		case PIF_COM_PROTOCOL_PIP:
		case PIF_COM_PROTOCOL_YAM:
		case PIF_COM_PROTOCOL_ESC:
		case PIF_COM_PROTOCOL_XON:
			sHandle = PifOpenCom(usPortId, pProtocol);
			if (sHandle >= 0) {
				sHandle |= 0x1000;
			}
			break;

		// Use terminal UDP network protocol.
		case PIF_COM_PROTOCOL_LAN:
		    sHandle = PifNetOpenEx(&pProtocol->xgHeader, PIF_NET_GENERALPORTMODE);  /* Open network         */
			if (sHandle >= 0) {
				sHandle |= 0x2000;
			}
			break;

		// Use raw TCP network protocol.
		case PIF_COM_PROTOCOL_TCP:
			sHandle = PifNetOpenEx(&pProtocol->xgHeader, PIF_NET_TCPCONNECTMODE);  /* Open network         */
			if (sHandle >= 0) {
				// if we open succeeded then set up our default control values.
				PifNetControl(sHandle, PIF_NET_SET_MODE, PIF_NET_DMODE | PIF_NET_TMODE);
				PifNetControl(sHandle, PIF_NET_SET_TIME, 5000);
				sHandle |= 0x3000;
			}
			break;

		// Unsupported protocol so just return an error.
		default:
			break;
	}

	return sHandle;
}

SHORT  PIFENTRY PifReadNetComIoEx(USHORT usPort, VOID *pBuffer, USHORT usBytes)
{
	SHORT  sRet = -1;

	if (usPort < 0x7fff) {
		switch (usPort & 0x7000) {
			case 0x1000:
				sRet = PifReadCom((usPort & 0x0fff), pBuffer, usBytes);
				break;
			case 0x2000:
			case 0x3000:
				sRet = PifNetReceive((usPort & 0x0fff), pBuffer, usBytes);
				break;
			default:
				break;
		}
	}

	return sRet;
}

SHORT  PIFENTRY PifWriteNetComIoEx(USHORT usPort, CONST VOID *pBuffer, USHORT usBytes)
{
	SHORT  sRet = -1;

	if (usPort < 0x7fff) {
		switch (usPort & 0x7000) {
			case 0x1000:
				sRet = PifWriteCom((usPort & 0x0fff), pBuffer, usBytes);
				break;
			case 0x2000:
			case 0x3000:
				sRet = PifNetSend((usPort & 0x0fff), pBuffer, usBytes);
				break;
			default:
				break;
		}
	}

	return sRet;
}

/*
 *   PifControlNetComIoEx() using the same argument types as PifNetControl() call either
 *   perform a Control setting change to either a communications port or a network port.
 *
 *   This function replaces both PifNetControl() and PifControlCom() however the way that
 *   some arguments are used vary between those two functions. When using the function code
 *   PIF_COM_SEND_TIME with PifControlCom() the argument for the time was an address of a USHORT.
 *
 *   With the function code PIF_NET_SET_TIME with the PifNetControl() function the argument for the
 *   time was just a USHORT value and not the address of the USHORT value.
 *
 *   With PifControlNetComIoEx() we chose to use the PifNetControl() standard since it actually seemed
 *   easier by allowing constants or variables to be used.
*/
SHORT  PIFENTRY PifControlNetComIoEx(USHORT usPort, USHORT usFunc, ...)
{
	SHORT  sRet = -1;
    ULONG  *pulArg = (ULONG*)(&usFunc);

	if (usPort < 0x7fff) {
		switch (usPort & 0x7000) {
			case 0x1000:
				sRet = PifControlComInternal((usPort & 0x0fff), usFunc, pulArg);
				break;
			case 0x2000:
			case 0x3000:
				sRet = PifNetControlInternal((usPort & 0x0fff), usFunc, pulArg);
				break;
			default:
				break;
		}
	}

	return sRet;
}

VOID   PIFENTRY PifCloseNetComIoEx(USHORT usPort)
{
	if (usPort < 0x7fff) {
		switch (usPort & 0x7000) {
			case 0x1000:
				PifCloseCom(usPort & 0x0fff);
				break;
			case 0x2000:
			case 0x3000:
				PifNetClose(usPort & 0x0fff);
				break;
			default:
				break;
		}
	}
}

