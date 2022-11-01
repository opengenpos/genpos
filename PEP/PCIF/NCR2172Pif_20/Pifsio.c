/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The serial communication on Platform Interface      **
**                                                                  **
**  File Name:  PIFSIO.C                                            **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the serial communication       **
**              functions of Platform Interface.                    **
**              This module provides the following functions:       **
**                                                                  **
**                PifOpenCom():     Open Serial Communication Port  **
**                PifReadCom():     Read from SerialPort            **
**                PifWriteCom():    Write through Serial Port       **
**                PifControlCom():  Control Serial Port             **
**                PifCloseCom():    Close Serial Communication Port **
**                                                                  **
**              But, the all function don't provided the feature.   **
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
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */
#include    <string.h>

/*******************************\
*   External Static Variable    *
\*******************************/
/* device number of YAM driver  */
extern  SHORT   sDev_yam;

/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   SHORT PIFENTRY PifOpenCom(USHORT usPortId,                  **
**                                        CONST PROTOCOL FAR *pProtocol);   **
**              usPortId:   port ID                                         **
**              pProtocol:  pointer to the structure of protocol data       **
**                                                                          **
**  Return:     0 (Port Handle)                                             **
**                                                                          **
** Description: opens the serial communication port according as the        **
**              arguments.                                                  **
**                                                                          **
******************************************************************************
fhfh*/
SHORT PIFENTRY PifOpenCom(USHORT usPortId, CONST PROTOCOL FAR *pProtocol)
{
    ARG_YAM aArgYam;

    if (pProtocol->fPip == PIF_COM_PROTOCOL_YAM) {      /* YAM  */
		DebugPrintf(_T("*PifOpenCom: Port=%hu, Pip=%d\r\n"), usPortId, pProtocol->fPip);

        aArgYam.usPortId = usPortId;
        memcpy(&aArgYam.uchComPara[0], &pProtocol->usComBaud, LEN_COM_PARA);
        return(XinuOpen(sDev_yam, &aArgYam, sizeof(ARG_YAM)));
    }

    return(0);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifReadCom(USHORT usPort,            **
**                                        VOID FAR *pBuffer,        **
**                                        USHORT usBytes);          **
**              usPort:     port handle                             **
**              pBuffer:    pointer to read buffer                  **
**              usBytes:    read buffer size                        **
**                                                                  **
**  Return:     0 (Acutual reading bytes)                           **
**                                                                  **
** Description: reads the data through the serial communication.    **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifReadCom(USHORT usPort, VOID FAR *pBuffer, USHORT usBytes)
{
    return(XinuRead(usPort, pBuffer, usBytes));
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifWriteCom(USHORT usPort,           **
**                                         CONST VOID FAR *pBuffer, **
**                                         USHORT usBytes);         **
**              usPort:     port handle                             **
**              pBuffer:    pointer to output buffer                **
**              usBytes:    output data size                        **
**                                                                  **
**  Return:     0 (Actual sending bytes)                            **
**                                                                  **
**                                                                  **
** Description: outputs the data through the serial communication.  **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifWriteCom(USHORT usPort, CONST VOID FAR *pBuffer,
                           USHORT usBytes)
{
    return(XinuWrite(usPort, (VOID FAR *)pBuffer, usBytes));
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifControlCom(USHORT usPort,         **
**                                           USHORT usFunc, ...);   **
**              usPort:     port handle                             **
**              usFunc:     function ID                             **
**              ...:        argument list according to the function **
**                          ID (max argument list: VOID FAR *pArg,  **
**                          USHORT usArg)                           **
**                                                                  **                   
**  Return:     PIF_OK (depends on the function ID)                 **
**                                                                  **                   
** Description: provides some functions to control for the serial   **
**              communication.                                      **
**                                                                  **                   
**********************************************************************
fhfh*/
SHORT PIFENTRY PifControlCom(USHORT usPort, USHORT usFunc, ...)
{
#if	WIN32
	va_list		marker;
	VOID *		pvBuffer;
	USHORT		usBytes, usTimeout;

	switch (usFunc)
	{
	case PIF_YAM_READ:
	case PIF_YAM_WRITE:
		va_start(marker, usFunc);
		pvBuffer = va_arg(marker, VOID *);
		usBytes  = va_arg(marker, USHORT);
		va_end(marker);
		DebugPrintf(_T("*PifControlCom: Port=%hu, Func=%hu, Bytes=%hu\r\n"),
			usPort, usFunc, usBytes);
	    return(XinuControl(usPort, usFunc, pvBuffer, usBytes));
	case PIF_YAM_RECEIVE_TIMER:
	case PIF_YAM_SEND_TIMER:
		va_start(marker, usFunc);
		usTimeout = va_arg(marker, USHORT);
		va_end(marker);
		DebugPrintf(_T("*PifControlCom: Port=%hu, Func=%hu, Timeout=%hu\r\n"),
			usPort, usFunc, usTimeout);
	    return(XinuControl(usPort, usFunc, usTimeout));
	case PIF_YAM_CANCEL:
		DebugPrintf(_T("*PifControlCom: Port=%hu, Func=%hu\r\n"),
			usPort, usFunc);
	    return(XinuControl(usPort, usFunc));
		break;
	default:
		PifAbort(MODULE_PIF_CONTROLCOM, FAULT_INVALID_FUNCTION);
		return(SYSERR);
	}
#else
    return(XinuControl(usPort, usFunc, *(VOID FAR **)(&usFunc+1),
                       *(USHORT *)((VOID FAR **)(&usFunc+1)+1)));
#endif
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifCloseCom(USHORT usPort);           **
**              usPort:  port handle                                **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: closes the specified port.                          **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifCloseCom(USHORT usPort)
{
    XinuClose(usPort);

    return;
}

