/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The network on Platform Interface                   **
**                                                                  **
**  File Name:  PIFNET.C                                            **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the network functions of       **
**              Platform Interface.                                 **
**              This module provides the following functions:       **
**                                                                  **
**                PifNetOpen():     Open Network Port               **
**                PifNetSend():     Send from Network Port          **
**                PifNetReceive():  Receive through Network Port    **
**                PifNetControl():  Control Network Port            **
**                PifNetClose():    Close Network Port              **
**                                                                  **
**  Compiler's options: /c /ACw /W4 /Zp /Ot /G1s                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Aug/17/92   T.Kojima    Initial release             **
**              Jun/22/98   O.Nakada    Win32                       **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
*   Include Files   *
\*******************/
#include	<windows.h>
#include	"debug.h"

#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "R20_PIF.H"     /* Portable Platform Interface */
#include    "PIFCTL.H"  /* Portable Platform Interface for "PifControl()"   */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */
#include    <stdlib.h>
#include	"paraequ.h"

#include    "net32.h"

/*******************************\
*   External Static Variable    *
\*******************************/
/* device number of NET driver  */
extern  SHORT   sDev_net;
extern SHORT	fsNetStatus;	/* network control, 2172 */

/*fhfh
**************************************************************************
**                                                                      **
**  Synopsis:   SHORT PIFENTRY PifNetOpen(CONST XGHEADER FAR *pHeader); **
**              pHeader:    pointer to the structure of IP address and  **
**                          UDP ports                                   **
**  Return:     >=0:    Port Handle of UDP                              **
**              <0:     indicates error as follows:                     **
**                      ==PIF_ERROR_NET_NOT_PROVIDED                    **
**                                                                      **
** Description: opens the Netwok's port according with the arguments.   **
**                                                                      **
**************************************************************************
fhfh*/
SHORT PIFENTRY PifNetOpen(CONST XGHEADER FAR *pHeader)
{
	if (fsNetStatus & PIF_NET_LANMODE) {
		return(NetOpen32((VOID *)pHeader));	/* 2172 */
	} else {
	    return(XinuOpen(sDev_net, pHeader, pHeader->usLport));
	}
}

SHORT PIFENTRY PifGetHostByName(CONST WCHAR *awzHostName, UCHAR *puchIPAddr)
{
	SHORT    sRetStatus = PIF_ERROR_SYSTEM;
	struct hostent *pHostEnt;
	CHAR     aszHostName[MAX_PCIF_HOSTNAME_SIZE];   //Changed from hardcode value 64 to MAX_PCIF_HOSTNAME_SIZE   ***PDINU

	wcstombs(aszHostName, awzHostName, sizeof(aszHostName));

	pHostEnt = gethostbyname (aszHostName);
	if (pHostEnt)
	{
		puchIPAddr[0] = *(UCHAR *)(pHostEnt->h_addr_list[0]);
		puchIPAddr[1] = *(UCHAR *)(pHostEnt->h_addr_list[0]+1);
		puchIPAddr[2] = *(UCHAR *)(pHostEnt->h_addr_list[0]+2);
		puchIPAddr[3] = *(UCHAR *)(pHostEnt->h_addr_list[0]+3);

		sRetStatus = PIF_OK;
	}
	else
	{
		sRetStatus = PIF_ERROR_NET_NOT_FOUND;
	}

	return sRetStatus;
}

SHORT PIFENTRY PifNetOpenHost(CONST WCHAR *awzHostName, int iLocalPort, int iFarPort)
{
	XGHEADER Header;
	SHORT    sRetStatus = PIF_ERROR_SYSTEM;

	memset (&Header, 0, sizeof(Header));

	sRetStatus = PifGetHostByName (awzHostName, Header.auchFaddr);

	if (0 <= sRetStatus)
	{
		Header.usLport = iLocalPort;
		Header.usFport = iFarPort;

		if (fsNetStatus & PIF_NET_LANMODE) {
			sRetStatus = NetOpen32((VOID *)&Header);
		}
	}
	else
	{
		sRetStatus = PIF_ERROR_NET_NOT_FOUND;
	}

	return sRetStatus;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifNetSend(USHORT usNet,             **
**                                        CONST VOID FAR *pBuffer,  **
**                                        USHORT usBytes);          **
**              usNet:      port handle                             **
**              pBuffer:    pointer to send buffer                  **
**              usBytes:    send data size                          **
**                                                                  **
**  Return:     IF_OK:  Ok                                          **
**                                                                  **
** Description: sends the data through the network.                 **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifNetSend(USHORT usNet, CONST VOID FAR *pBuffer,
                          USHORT usBytes)
{
	if (fsNetStatus & PIF_NET_LANMODE) {
	    return(NetSend32(usNet, (VOID FAR *)pBuffer, usBytes));	/* 2172 */
	} else {
	    return(XinuWrite(usNet, (VOID FAR *)pBuffer, usBytes));
	}
}

int PIFENTRY PifWsaLastError (VOID)
{
	return WSAGetLastError();
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT  PIFENTRY PifNetReceive(USHORT usNet,         **
**                                            VOID FAR *pBuffer,    **
**                                            USHORT usBytes);      **
**              usNet:      port handle                             **
**              pBuffer:    pointer to receive buffer               **
**              usBytes:    receive buffer size                     **
**                                                                  **
**  Return:     >=0:    Acutual receiving bytes                     **
**               <0:     Indicates errors as follows:               **
**                       ==PIF_ERROR_NET_TIMEOUT                    **
**                       <PIF_ERROR_NET_ERRORS                      **
**                                                                  **
** Description: receives the data through the network.              **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifNetReceive(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes)
{
	if (fsNetStatus & PIF_NET_LANMODE) {
	    return(NetReceive32(usNet, pBuffer, usBytes)); /* 2172 */
	} else {
	    return(XinuRead(usNet, pBuffer, usBytes));
	}
}

/*fhfh
**************************************************************************
**                                                                      **
**  Synopsis:   SHORT PIFENTRY PifNetControl(USHORT usNet,              **
**                                           USHORT usFunc,             **
**                                           ...);                      **
**              usNet:      port handle                                 **
**              usFunc:     function ID                                 **
**              ...:        argument list accroding to the function ID  **
**                          (max argument list: USHORT usArg)           **
**                                                                      **
**  Return:     ==PIF_OK:  Ok                                           **
**                                                                      **
** Description: provides some functions to control for the network.     **
**                                                                      **
**************************************************************************
fhfh*/
SHORT  PIFENTRY PifNetControl(USHORT usNet, USHORT usFunc, ...)
{
#if	WIN32
	VOID       *pArg;
	va_list		marker;
	USHORT		usArg;

	switch (usFunc)
	{
	case PIF_NET_SET_MODE:
	case PIF_NET_SET_TIME:
	case PIF_NET_SET_STIME:						/* set timeout value ?		*/
		va_start(marker, usFunc);
		usArg = va_arg(marker, USHORT);
		va_end(marker);
		DebugPrintf(_T("*PifNetControl: Port=%d, Func=%d, Arg=%d\r\n"), usNet, usFunc, usArg);
		if (fsNetStatus & PIF_NET_LANMODE) {
			return(NetControl32(usNet, usFunc, usArg)); /* 2172 */
		} else {
			return(XinuControl(usNet, usFunc, usArg));
		}
		break;
	case PIF_NET_GET_USER_INFO:
		va_start(marker, usFunc);
		pArg = va_arg(marker, VOID *);
		va_end(marker);
		DebugPrintf(_T("*PifNetControl: Port=%d, Func=%d, Arg=%p\r\n"), usNet, usFunc, pArg);
		if (fsNetStatus & PIF_NET_LANMODE) {
			return(NetControl32(usNet, usFunc, pArg)); /* 2172 */
		} else {
			return(XinuControl(usNet, usFunc, pArg));
		}
		break;
	default:
		PifAbort(MODULE_PIF_NETCONTROL, FAULT_INVALID_FUNCTION);
		return(SYSERR);
	}
#else
    return(XinuControl(usNet, usFunc, *(&usFunc+1)));
#endif
}

/*fhfh
******************************************************************
**                                                              **
**  Synopsis:   VOID PIFENTRY PifNetClose(USHORT usNet);        **
**              usNet:  port handle                             **
**                                                              **
**  Return:     None                                            **
**                                                              **
** Description: closes the specified port.                      **
**                                                              **
******************************************************************
fhfh*/
VOID PIFENTRY PifNetClose(USHORT usNet)
{
    SHORT   sStatus;
	if (fsNetStatus & PIF_NET_LANMODE) {
	    NetClose32(usNet); /* 2172 */
	} else {
		if ((sStatus = XinuClose(usNet)) < 0) {
		    PifAbort(MODULE_PIF_NETCLOSE, (USHORT)sStatus);
		}
    }
    return;
}
