/*mhmh
*****************************************************************************
**    
**  Title:      Platform Interface
**
**  File Name:  PIFCTL.C
**
**  Categoly:   Poratable Platform Interface
**
**  Abstruct:   This module provides the functions of Platform Interface.
**              This module provides the following functions:
**
**                  PIF_IHC_DIAG            IHC chip test
**                  PIF_NET_START           start the IHC pingpong test
**                                          response
**                  PIF_NET_STOP            stop the IHC pingpong test
**                                          response
**
**                  PIF_PINGPONG            start the IHC pingpong test
**                                          request
**
**  Compiler's options: /c /ACw /W4 /Zp /Ot /Gs
**
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**      Ver.  Date          Name            Description        
**      1.00  Jul/31/1992   T.kojima        Initial release
**                                          add IHC test
**      1.01  Aug/19/1992   M.Sugiyama      Added functions of NCR/DLC manager
**
*****************************************************************************
mhmh*/


/*******************
 *  Include Files  *
 ******************/
#include	<windows.h>
#include	"debug.h"

#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "R20_PIF.H"     /* Portable Platform Interface */
#include    "PIFCTL.H"  /* Portable Platform Interface for "PifControl()"   */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */
#include    "net32.h"

extern SHORT	fsNetStatus;	/* network control, 2172 */

/*fhfh
*****************************************************************************
*
**  Synopsis:   SHORT PIFENTRY PifControl(USHORT usFunc, VOID FAR *pArgList);
*               usFunc:     function number
*               pArgList:   pointer to the arguments list
*
**  Return:     according to the function
*               The basic rule is as follows:
*                   
*                   return value >=0 : completion
*                   return value <0  : errors
*
** Description: This function is only used to directly access to the Platform
*               Interface. For example, this function is used with Level 1
*               Diagnostics functions.
*
*****************************************************************************
fhfh*/
SHORT PIFENTRY PifControl(USHORT usFunc, VOID  *pArgList)
{
	USHORT  usPortNo = 0;

    switch (usFunc)
	{
    case PIF_NET_START:
		DebugPrintf(_T("*PifControl: PIF_NET_START\r\n"));
		fsNetStatus = 0; /* 2172 */
        return(XinuNetStart((COMCONTROL  *)pArgList, PifSysConfig()->auchLaddr));

    case PIF_NET_STOP:
		DebugPrintf(_T("*PifControl: PIF_NET_STOP\r\n"));
		fsNetStatus = 0; /* 2172 */
	    return(XinuNetStop());

    case PIF_ASY_READ:
		DebugPrintf(_T("*PifControl: PIF_ASY_READ=%d\r\n"), ((ASYARG  *)pArgList)->usLength);
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_READ, ((ASYARG  *)pArgList)->pBuffer, ((ASYARG  *)pArgList)->usLength));

    case PIF_ASY_WRITE:
		DebugPrintf(_T("*PifControl: PIF_ASY_WRITE=%d\r\n"), ((ASYARG  *)pArgList)->usLength);
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_WRITE, ((ASYARG  *)pArgList)->pBuffer, ((ASYARG  *)pArgList)->usLength));

    case PIF_ASY_RECEIVE_TIMER:
		DebugPrintf(_T("*PifControl: PIF_ASY_RECEIVE_TIMER=%d\r\n"), (USHORT  *)pArgList);
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_RECEIVE_TIMER, (USHORT  *)pArgList));

    case PIF_ASY_SEND_TIMER:
		DebugPrintf(_T("*PifControl: PIF_ASY_SEND_TIMER=%d\r\n"), (USHORT  *)pArgList);
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_SEND_TIMER, (USHORT  *)pArgList));

    case PIF_ASY_DTR_ON:
		DebugPrintf(_T("*PifControl: PIF_ASY_DTR_ON\r\n"));
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_DTR_ON));

    case PIF_ASY_DTR_OFF:
		DebugPrintf(_T("*PifControl: PIF_ASY_DTR_ON\r\n"));
        return(XinuControl(XinuGetDev("asy"), ASY_FUNC_DTR_OFF));

	case PIF_NET_START_LAN_PORT:
		usPortNo = (USHORT)pArgList;
		fsNetStatus |= PIF_NET_LANMODE;	/* LAN connection */
		DebugPrintf(_T("*PifControl: PIF_NET_START_LAN_PORT\r\n"));
		return (NetInitialize32(usPortNo));

    case PIF_NET_START_LAN: /* 2172 */
		fsNetStatus |= PIF_NET_LANMODE;	/* LAN connection */
		DebugPrintf(_T("*PifControl: PIF_NET_START_LAN\r\n"));
		return (NetInitialize32(0));

    case PIF_NET_STOP_LAN: /* 2172 */
		fsNetStatus = 0; /* 2172 */
		DebugPrintf(_T("*PifControl: PIF_NET_STOP_LAN\r\n"));
        NetFinalize32();
		return PIF_OK;

	default:
		PifAbort(MODULE_PIF_CONTROL, FAULT_INVALID_FUNCTION);
		return (SYSERR);
    }
}
