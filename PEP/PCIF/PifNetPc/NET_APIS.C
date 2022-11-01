/****************************************************************************\
||                                                                          ||
||        *=*=*=*=*=*=*=*=*                                                 ||
||        *  NCR 7450 POS *             AT&T GIS Japan, E&M OISO            ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995              ||
||    <|\/~               ~\/|>                                             ||
||   _/^\_                 _/^\_                                            ||
||                                                                          ||
\****************************************************************************/

/*==========================================================================*\
*   Title:
*   Category:
*   Program Name:
* ---------------------------------------------------------------------------
*   Abstract:
*
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2015  -> GenPOS Rel 2.3.0 (EMV support for Electronic Payment)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* ---------------------------------------------------------------------------
*   Update Histories:
* ---------------------------------------------------------------------------
*   Date     | Version  |  By         | Descriptions
* ---------------------------------------------------------------------------
* 10/26/2015 | 02.02.01 | R.Chambers  | Removed ifdefs keeping only SARATOGA_PC source.
\*==========================================================================*/

/*==========================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:   $Revision$
:   $Date$
:   $Author$
:   $Log$
\*==========================================================================*/

/*==========================================================================*\
;+                                                                          +
;+                  I N C L U D E    F I L E s                              +
;+                                                                          +
\*==========================================================================*/

/* --- global haeders --- */
# include   <windows.h>                         /* Windows header           */
# include   "r20_pif.h"

/* --- C-library header --- */
#include    <memory.h>                      /* memxxx() C-library header    */
#include    <stdarg.h>                      /* va_arg() etc.                */

/* --- our common headers --- */
#include    "piftypes.h"                    /* type modifier header         */
#include    "pifnet.h"                      /* PifNet common header         */
#include    "netmodul.h"                    /* my module header             */

/*==========================================================================*\
;+                                                                          +
;+                  L O C A L     D E F I N I T I O N s                     +
;+                                                                          +
\*==========================================================================*/

/* --- queue ID by default --- */
#define     QID_MANAGER     0xFFF0              /* queue ID of managers     */

/*==========================================================================*\
;+                                                                          +
;+          P R O T O T Y P E     D E C L A R A T I O N s                   +
;+                                                                          +
\*==========================================================================*/

/* --- initialization & finalization --- */


/*==========================================================================*\
;+                                                                          +
;+                  S T A T I C   V A R I A B L E s                         +
;+                                                                          +
\*==========================================================================*/

/* --- network module status --- */
static  fNetWorking = FALSE;                    /* initialization status    */

/*==========================================================================*\
;+                                                                          +
;+              C O N S T A N T    D E F I N I T I O N s                    +
;+                                                                          +
\*==========================================================================*/


/*==========================================================================*\
;+                                                                          +
;+              I n i t i a l i z e  &  F i n a l i z e                     +
;+                                                                          +
\*==========================================================================*/

/*==========================================================================*\
;+                                                                          +
;+                      A P I   P r o g r a m                               +
;+                                                                          +
\*==========================================================================*/

/**
;========================================================================
;
;   function : Initialize a network
;
;   format : SHORT      PifNetInitialize(pInfo);
;
;   input  : PNETINF    pInfo;      - ptr. to information data
;
;   output : SHORT      sRet;       - status
;
;========================================================================
**/
SHORT NETENTRY  NetInitialize(PNETINF pInfo)
{
    USHORT      usRet;

    if (fNetWorking) {                          /* I'm working ?            */
        return (PIF_ERROR_NET_CRITICAL);            /* cannot do my job     */
    }

    /* --- spawn network manager thread --- */
    if (usRet = SpawnNetManager(QID_MANAGER, pInfo)) {
        return (usRet);
    }

    /* --- mark working status --- */
    fNetWorking = TRUE;                         /* set I'm working !        */
    return (PIF_OK);
}

/**
;========================================================================
;
;   function : Open a network
;
;   format : SHORT      PifNetOpen(pHeader);
;
;   input  : PXGHEADER  pHeader;    - ptr. to parameters
;
;   output : SHORT      sHandle;    - status
;
;========================================================================
**/
SHORT NETENTRY  NetOpenEx(PXGHEADER pHeader)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;
    POPNDAT     pOpen;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
    pOpen    = (POPNDAT)(&(pManager->datOption));

    /* --- make command packet --- */
    pManager->usHandle  = 0;                    /* network handle           */
    pManager->usCommand = CMD_OPEN;             /* command code             */
    pManager->usLength  = sizeof(OPNDAT);       /* optional data length     */

    /* --- make optional data --- */
    pOpen->ulFaddr = *(LPULONG)(pHeader->auchFaddr);    /* target IP address*/
    pOpen->usFport = pHeader->usFport;          /* target port no.          */
    pOpen->usLport = pHeader->usLport;          /* local port no.           */

    /* --- invoke a function --- */
    usRet = ExeNetOpenCli(pOpen);
    return (usRet);
}

/**
;========================================================================
;
;   function : Receive from a network
;
;   format : SHORT      PifNetReceive(usHandle, pvBuffer, usBytes);
;
;   input  : USHORT     usHandle;       - network handle
;            PVOID      pvBuffer;       - ptr. to a buffer
;            USHORT     usBytes;        - no. of bytes
;
;   output : SHORT      sRet;           - status
;
;========================================================================
**/
SHORT NETENTRY  NetReceive(USHORT usHandle, LPVOID pvBuffer, USHORT usBytes)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;
    PRCVDAT     pReceive;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
    pReceive = (PRCVDAT)(&(pManager->datOption));

    /* --- make command packet --- */
    pManager->usHandle  = usHandle;             /* network handle           */
    pManager->usCommand = CMD_RECEIVE;          /* command code             */
    pManager->usLength  = sizeof(RCVDAT);       /* optional data length     */
    pReceive->usBytes   = usBytes;              /* receive buffer size      */
    pReceive->pvBuffer  = pvBuffer;             /* ptr. to receive buffer   */
    pReceive->pSender   = _NULL;                /* no. sender info.         */

    /* --- invoke a function --- */
    usRet = ExeNetReceiveCli(usHandle, pReceive);
    return (usRet);
}

/**
;========================================================================
;
;   function : Receive from a network
;
;   format : SHORT      PifNetReceive2(usHandle, pvBuffer, usBytes, pSender);
;
;   input  : USHORT     usHandle;       - network handle
;            LPVOID     pvBuffer;       - ptr. to a buffer
;            USHORT     usBytes;        - no. of bytes
;            PXGADDR    pSender;        - ptr. to get sender info.
;
;   output : SHORT      sRet;           - status
;
;========================================================================
**/
SHORT NETENTRY
NetReceive2(USHORT usHandle, LPVOID pvBuffer, USHORT usBytes, PXGADDR pSender)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;
    PRCVDAT     pReceive;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
    pReceive = (PRCVDAT)(&(pManager->datOption));

    /* --- make command packet --- */
    pManager->usHandle  = usHandle;             /* network handle           */
    pManager->usCommand = CMD_RECEIVE;          /* command code             */
    pManager->usLength  = sizeof(RCVDAT);       /* optional data length     */
    pReceive->usBytes   = usBytes;              /* receive buffer size      */
    pReceive->pvBuffer  = pvBuffer;             /* ptr. to receive buffer   */
    pReceive->pSender   = pSender;              /* ptr. to sender info.     */

    /* --- invoke a function --- */
    usRet = ExeNetReceiveCli(usHandle, pReceive);
    return (usRet);
}

/**
;========================================================================
;
;   function : Send to a network
;
;   format : SHORT      PifNetSend(usHandle, pvBuffer, usBytes);
;
;   input  : USHORT     usHandle;       - network handle
;            LPVOID     pvBuffer;       - ptr. to data to be sent
;            USHORT     usBytes;        - no. of bytes
;
;   output : SHORT      sRet;           - status
;
;========================================================================
**/

SHORT NETENTRY  NetSend(USHORT usHandle, LPVOID pvBuffer, USHORT usBytes)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;
    PSNDDAT     pSend;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
    pSend    = (PSNDDAT)(&(pManager->datOption));

    /* --- make command packet --- */
    pManager->usHandle  = usHandle;             /* network handle           */
    pManager->usCommand = CMD_SEND;             /* command code             */
    pManager->usLength  = sizeof(SNDDAT);       /* optional data length     */
    pSend->usBytes      = usBytes;              /* send data length         */
    pSend->pvBuffer     = pvBuffer;             /* ptr. to send data        */

    /* --- invoke a function --- */
    usRet = ExeNetSendCli(usHandle, pSend);
    return (usRet);
}

/**
;========================================================================
;
;   function : Control a network
;
;   format : SHORT      PifNetControl(usHandle, usFunc, ...);
;
;   input  : USHORT     usHandle;       - network handle
;            USHORT     usFunc;         - function code
;            ??????     ...;            - specific parameters
;
;   output : SHORT      sRet;           - status
;
;========================================================================
**/
SHORT NETENTRY  NetControl(USHORT usHandle, USHORT usFunc, ...)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet, usValue;
    PMGRMSG     pManager;
    PCNTDAT     pControl;
    PXGADDR     pAddr;
    va_list     pvaArg;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);
    pControl = (PCNTDAT)(&(pManager->datOption));

    /* --- make command packet --- */
    pManager->usHandle   = usHandle;            /* network handle           */
    pManager->usCommand  = CMD_CONTROL;         /* command code             */
    pManager->usLength   = sizeof(CNTDAT);      /* optional data length     */
    pControl->usFunction = usFunc;              /* sub-function code        */

    /* --- initialize to get a parameter --- */
    va_start(pvaArg, usFunc);                   /* top of variable argument */

    /* --- do each function --- */
    switch (usFunc) {                           /* what function ?          */
    case PIF_NET_SET_MODE:                      /* set a mode ?             */
        pControl->usModeMode = va_arg(pvaArg, USHORT);  /* function mode    */
        break;
    case PIF_NET_SET_TIME:                      /* set timeout value ?      */
        usValue              = va_arg(pvaArg, USHORT);  /* timeout value    */
        pControl->ulTimeTime = (ULONG)usValue;          /* timeout value    */
        break;
    case PIF_NET_SET_TIME_EX:                   /* set timeout value ?      */
        pControl->ulTimeTime = va_arg(pvaArg, ULONG);   /* timeout value    */
        break;
    case PIF_NET_SET_STIME:                     /* set timeout value ?      */
        pControl->usStimeTime = va_arg(pvaArg, USHORT); /* timeout value    */
        break;
    case PIF_NET_SET_REMOTE:                    /* set remote address ?     */
        pAddr                = va_arg(pvaArg, PXGADDR);
        pControl->ulSendAddr = *(LPULONG)(pAddr->auchAddr);
        pControl->usSendPort =            pAddr->usPort;
        break;
    case PIF_NET_SET_EVENT:                     /* set power down event ?   */
        pControl->fEventEvent = va_arg(pvaArg, BOOL);   /* option value     */
        break;
    case PIF_NET_DIAG:                          /* diagnostics ?            */
        pControl->pDiagData   = va_arg(pvaArg, PDGDATA);
        pControl->pDiagBuffer = va_arg(pvaArg, PDGINFO);
        pControl->usDiagBytes = va_arg(pvaArg, USHORT);
        break;
    case PIF_NET_SET_LOG:                       /* set LOG function addr ?  */
        pControl->pfnAddress = va_arg(pvaArg, PFUNLOG);
        break;
	case PIF_NET_GET_USER_INFO:
        pControl->datFunc.usrInfo.pfnAddr = va_arg(pvaArg, PPIFUSRTBLINFO);
		break;
    case PIF_NET_CLEAR:                         /* clear queued messages ?  */
    case PIF_NET_GET_NAME:                      /* get local name ?         */
    case PIF_NET_SET_STANDBY:                   /* set stand by state ?     */
    default:                                    /* others ...               */
        break;
    }

    /* --- invoke a function --- */
    usRet = ExeNetControlCli(usHandle, pControl);
    
    /* --- clean up for variable arguments --- */
    va_end(pvaArg);                             /* clean up arg. pointer    */

    return (usRet);
}

/**
;========================================================================
;
;   function : Close a network
;
;   format : SHORT      PifNetClose(usHandle);
;
;   input  : USHORT     usHandle;       - network handle
;
;   output : SHORT      sRet;           - status
;
;========================================================================
**/
SHORT NETENTRY  NetClose(USHORT usHandle)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_ERROR_NET_NOT_WORK);            /* cannot do my job     */
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);

    /* --- make command packet --- */
    pManager->usHandle  = usHandle;             /* network handle           */
    pManager->usCommand = CMD_CLOSE;            /* command code             */
    pManager->usLength  = 0;                    /* optional data length     */

    /* --- invoke a function --- */
    usRet = ExeNetCloseCli(usHandle);
    return (usRet);
}

/**
;========================================================================
;
;   function : Shutdown a network
;
;   format : SHORT      PifNetFinalize();
;
;   input  : nothing
;
;   output : SHORT      sRet;       - return status
;
;========================================================================
**/
SHORT NETENTRY  NetFinalize(VOID)
{
    UCHAR       aucMessage[sizeof(MSGHDR) + sizeof(MGRMSG)];
    USHORT      usRet;
    PMGRMSG     pManager;

    if (! fNetWorking) {                        /* I'm working ?            */
        return (PIF_OK);                            /* ignore, but completed*/
    }

    /* --- initialize --- */
    pManager = (PMGRMSG)(((PMSGHDR)aucMessage)->aucMessage);

    /* --- make command packet --- */
    pManager->usHandle  = (USHORT)(PIF_NET_HANDLE); /* network handle       */
    pManager->usCommand = CMD_SHUTDOWN;         /* command code             */
    pManager->usLength  = 0;                    /* optional data length     */

    /* --- invoke a function --- */
    usRet = ExeNetShutdownCli(PIF_NET_HANDLE);
    
    fNetWorking = FALSE;                        /* I'm sleeping !           */

    return (usRet);
}

/*==========================================================================*\
;+                      E n d    O f    P r o g r a m                       +
\*==========================================================================*/
