/////////////////////////////////////////////////////////////////////////////
// PIFNET32.c : 
//
// Copyright (C) 1999 NCR Corporation, All rights reserved.
//
//
// History:
//
// Date         Version  Author       Comments
// ===========  =======  ===========  =======================================
// Dec-04-1999                        initial
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winsock.h>
#include "pif32.h"
#include "debug.h"

#pragma pack(push, 1)
#include    "XINU.H"    /* XINU                         */
#define     PIFXINU     /* switch for "PIF.H"           */
#include    "R20_PIF.H"     /* Portable Platform Interface  */
#include    "LOG.H"     /* Fault Codes Definition       */
#include    "PIFXINU.H" /* Platform Interface           */
#pragma pack(pop)

#include "net32.h"
#include "pifnet.h"
#define PIF32_NFDES            16          // number of files / directory

static CRITICAL_SECTION    g_csPif32Net;           // for file

static HINSTANCE hNetInstance;

static UCHAR   auchAllHostAddress[PIF_NET_MAX_IP][PIF_LEN_IP];

static struct {                                    // file information table
    SHORT sPifHandle;
    SHORT sNetHandle;
    BOOL fCompulsoryReset;
    CHAR chMode;
    SHORT sCount;
    XGHEADER xgHeader;
    BOOL    fState;
    BOOL    fDirectIP;
} aNetTbl[PIF32_NFDES];

/* --- Network --- */
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT NetOpen32(VOID *pData)                        **
**              pHeader:    IP Address and Port No. Information     **
**                                                                  **
**  return:     handle of network                                   **
**                                                                  **
**  Description:Opening UDP/IP by Winsock                           **     
**                                                                  **
**********************************************************************
fhfh*/
SHORT  NetOpen32(VOID *pData)
/* SHORT  NetOpen32(CONST XGHEADER FAR *pHeader) */
{
    XGHEADER *pHeader;
    SHORT           nIndex;
    SHORT   sNetHandle, sReturn;

    pHeader = pData;

    EnterCriticalSection(&g_csPif32Net);

    // get free table

    for (nIndex = 0; nIndex < PIF32_NFDES; nIndex++)
    {
        if (! aNetTbl[nIndex].fState)
        {
            break;
        }
        if (nIndex == PIF32_NFDES)                      // short resource
        {
            LeaveCriticalSection(&g_csPif32Net);
            Net32AbortEx(MODULE_PIF_NETOPEN, FAULT_SHORT_RESOURCE, TRUE);
        }
    }

    /* open by pifnet.dll */
    sNetHandle = NetOpenEx((XGHEADER *)pHeader);
    if (sNetHandle < 0) {
        Net32AbortEx(MODULE_PIF_NETSEND, FAULT_GENERAL, TRUE);
        return PIF_ERROR_NET_ERRORS;
    }

    /* not wait at NetSend */
    sReturn = NetControl(sNetHandle, PIF_NET_SET_STIME, PIF_NO_WAIT);
    if (sReturn < 0) {
        NetClose(sNetHandle);
        Net32AbortEx(MODULE_PIF_NETSEND, FAULT_GENERAL, TRUE);
        return PIF_ERROR_SYSTEM;
    }

    aNetTbl[nIndex].sPifHandle = nIndex;
    aNetTbl[nIndex].fCompulsoryReset = TRUE;
    aNetTbl[nIndex].chMode = PIF_NET_NMODE;
    aNetTbl[nIndex].sCount = 0;
    memcpy(&aNetTbl[nIndex].xgHeader, pHeader, sizeof(XGHEADER));
    aNetTbl[nIndex].fState   = TRUE;
    aNetTbl[nIndex].sNetHandle = sNetHandle;
    aNetTbl[nIndex].fDirectIP   = FALSE;

    LeaveCriticalSection(&g_csPif32Net);
    return nIndex;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY NetSend32(USHORT usNet,             **
**                                      CONST VOID FAR *pBuffer,    **
**                                      USHORT usBytes)             **
**              usNet:    network handle                            **
**              pBuffer:  sending data buffer                       **
**              usBytes:  size of pBuffer                           **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_NET_ERRORS                                **
**                                                                  **
**  Description:Sending data through UDP/IP                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  NetSend32(USHORT usNet, CONST VOID FAR *pBuffer,
                           USHORT usBytes)
{
    XGRAMEX *pxgram;
    UCHAR  *pData;
    USHORT len, i;
    SHORT sReturn;
	static XGRAMEX   aNetXgram[PIF32_NFDES];

    if (! PifIsValidNet(usNet))
    {
        Net32AbortEx(MODULE_PIF_NETSEND, FAULT_GENERAL, TRUE);
        return PIF_ERROR_NET_ERRORS;
    }

    if (aNetTbl[usNet].chMode & PIF_NET_NMODE) {

        /* normal mode, set socket address from send buffer */

        pxgram = (XGRAMEX *)pBuffer;

        /* remove header fro m send buffer */

        len = usBytes - sizeof(XGHEADER);
        pData = pxgram->auchData;

    } else {

        /* data only mode */

        /* get port no. from saved area */
        pxgram = (XGRAMEX *)&aNetTbl[usNet].xgHeader;

        len = usBytes;
        pData = (UCHAR *)pBuffer;
    }

    if (pxgram->auchFaddr[3] == 0) {

        /* set broadcast address */
        *(LONG*)aNetXgram[usNet].auchFaddr = INADDR_BROADCAST;
        
    } else {
        
        /* send to 2172 terminal */
        
        if ((pxgram->auchFaddr[0] == 192) &&
            (pxgram->auchFaddr[1] == 0) &&
            (pxgram->auchFaddr[2] == 0) &&
            (pxgram->auchFaddr[3] <= PIF_NET_MAX_IP)) {

            i = pxgram->auchFaddr[3];
            i--;
            if (auchAllHostAddress[i][0] == INADDR_ANY) {
                PifGetDestHostAddress(&auchAllHostAddress[i][0], pxgram->auchFaddr);
                if (auchAllHostAddress[i][0] == INADDR_ANY) {
                    
                    /* target does not exists */
                    return PIF_OK;
                }
            }
            memcpy(aNetXgram[usNet].auchFaddr, &auchAllHostAddress[i][0], PIF_LEN_IP);
            aNetTbl[usNet].fDirectIP = FALSE;

        } else {
            
            /* use directed ip address, assume as PC */
            memcpy(aNetXgram[usNet].auchFaddr, pxgram->auchFaddr, PIF_LEN_IP);
            aNetTbl[usNet].fDirectIP = TRUE;
        }
    }
    /* set source no. at the top of data */
    aNetXgram[usNet].usFport = pxgram->usFport;
    aNetXgram[usNet].usLport = pxgram->usLport;
    aNetXgram[usNet].auchData[0] = 0;
    memcpy(&aNetXgram[usNet].auchData[1], pData, usBytes-sizeof(XGHEADER));
    usBytes++;

    /* send by pifnet.dll */
    sReturn = NetSend(aNetTbl[usNet].sNetHandle, &aNetXgram[usNet], usBytes);
    
    return sReturn;
    

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY NetReceive32(USHORT usNet,             **
**                                      CONST VOID FAR *pBuffer,    **
**                                      USHORT usBytes)             **
**              usNet:    network handle                            **
**              pBuffer:  receiving data buffer                     **
**              usBytes:  size of pBuffer                           **
**                                                                  **
**  return:     length of data actually received                    **
**                                                                  **
**  Description:receiving data through UDP/IP                       **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  NetReceive32(USHORT usNet, VOID FAR *pBuffer, USHORT usBytes)
{
    XGRAMEX *pxgram;
    SHORT sRet;
	static XGRAMEX   aNetRcvXgram[PIF32_NFDES];

    if (! PifIsValidNet(usNet))
    {
        Net32AbortEx(MODULE_PIF_NETRECEIVE, FAULT_INVALID_HANDLE, TRUE);
        return PIF_ERROR_NET_ERRORS;
    }
    
    usBytes++;   /* add 1 byte for terminal no */

    /* receive by pifnet.dll */
    sRet = NetReceive(aNetTbl[usNet].sNetHandle, &aNetRcvXgram[usNet], usBytes);

    if (sRet > 0) {

        if (aNetTbl[usNet].chMode & PIF_NET_DMODE) {
            
            sRet--;
            memcpy(pBuffer, &aNetRcvXgram[usNet].auchData[1], sRet);
            
        } else {
        
            pxgram = (XGRAMEX *)pBuffer;

#if defined(PEP_ON_TERM) && defined(_DEBUG)
			// If we are testing on a PC then we want to use direct IP address
			// since we are talking to ourselves, usually using 127.0.0.1
			aNetTbl[usNet].fDirectIP = TRUE;  // force to use direct IP address
#endif
            /* get source no. from top of the data */
            if ((aNetRcvXgram[usNet].auchData[0] != 0) &&
                (aNetTbl[usNet].fDirectIP == FALSE)) {  /* use 2170 ip address */

                pxgram->auchFaddr[0] = 192;
                pxgram->auchFaddr[1] = 0;
                pxgram->auchFaddr[2] = 0;
                pxgram->auchFaddr[3] = aNetRcvXgram[usNet].auchData[0];
                    
            } else {

                /* use direct ip address */
                memcpy(pxgram->auchFaddr, aNetRcvXgram[usNet].auchFaddr, PIF_LEN_IP);
            }

            pxgram->usFport = aNetRcvXgram[usNet].usFport;
            pxgram->usLport = aNetRcvXgram[usNet].usLport;

            sRet--;
            memcpy(pxgram->auchData, &aNetRcvXgram[usNet].auchData[1], sRet-sizeof(XGHEADER));
        }

    }   
    return sRet;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY NetControl32(USHORT usNet,          **
**                                      USHORT usFunc,...)          **
**              usNet:    network handle                            **
**              usFunc:   function code of PifNetControl            **
**                                                                  **
**  return:     PIF_OK                                              **
**              PIF_ERROR_NET_ERRORS                                **
**                                                                  **
**  Description:set misc. control of UDP/IP                         **
**                                                                  **
**********************************************************************
fhfh*/
SHORT  NetControl32(USHORT usNet, USHORT usFunc, ...)
{
    ULONG  *pulArg;
    
    if (! PifIsValidNet(usNet))
    {
        Net32AbortEx(MODULE_PIF_NETCONTROL, FAULT_INVALID_HANDLE, TRUE);
        return PIF_ERROR_NET_ERRORS;
    }

    pulArg = (ULONG*)(&usFunc);
    pulArg++;

    return (NetControl(aNetTbl[usNet].sNetHandle, usFunc, *pulArg));
    

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT NetClose32(USHORT usNet)                      **
**              usNet:    network handle                            **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:closing network                                     **
**                                                                  **
**********************************************************************
fhfh*/
VOID   NetClose32(USHORT usNet)
{

    if (! PifIsValidNet(usNet))
    {
        Net32AbortEx(MODULE_PIF_NETCLOSE, FAULT_INVALID_HANDLE, TRUE);
    }

    NetClose(aNetTbl[usNet].sNetHandle);
    aNetTbl[usNet].sNetHandle = -1;
    aNetTbl[usNet].fState   = FALSE;
    return;

}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT NetInit32(VOID)                           **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:start up routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
USHORT NetInit32(VOID)
{

    WSADATA wsaData;
    DWORD dwError;
//    UCHAR  auchHostAddress[PIF_LEN_IP];
    USHORT i;

	if (WSAStartup(MAKEWORD(2,1), &wsaData) == SOCKET_ERROR) {      /* initialize winsock       */
        dwError = WSAGetLastError();
        WSACleanup();
        return PIF_ERROR_NET_ERRORS;
    }

    InitializeCriticalSection(&g_csPif32Net);
	memset(&aNetTbl, 0, sizeof(aNetTbl));
    //ZeroMemory(&aNetTbl, sizeof(aNetTbl));

    for (i=0; i<PIF_NET_MAX_IP; i++) {

        auchAllHostAddress[i][0] = INADDR_ANY;
    }

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    for (i=0; i<PIF_NET_MAX_IP; i++) {

        auchHostAddress[0] = 192;
        auchHostAddress[1] = 0;
        auchHostAddress[2] = 0;
        auchHostAddress[3] = (UCHAR)(i+1);

        PifGetDestHostAddress(&auchAllHostAddress[i][0], auchHostAddress);
    }
#endif
    hNetInstance = LoadLibrary(TEXT("PIFNETPC.DLL"));
    
    return PIF_OK;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetStartup(VOID)                           **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:start up routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
USHORT   NetInitialize32(USHORT  usPortNo)
{
    NETINF netinf;
    UCHAR  szNetPort[] = "NCRPOS";
    UCHAR  szNetPortLocal[] = "NCRPOS";
    struct servent * pService;
    SHORT sReturn;
	static USHORT  usNetPort;

    pService = getservbyname("NCRPOS", "UDP");
    usNetPort = (USHORT)(pService ? htons(pService->s_port) : 2172);
    _itoa(usNetPort, szNetPort, 10);

    _itoa(2174, szNetPortLocal, 10);
	if (usPortNo != 0)
	{
		_itoa(usPortNo, szNetPortLocal, 10);
	}

    netinf.pszLocalPort  = szNetPortLocal;           /* ptr. to local port no.   */
    netinf.pszRemotePort = szNetPort;                /* ptr. to remote port no.  */

#if defined(PEP_ON_TERM) && defined(_DEBUG)
	// Setting the PEP_ON_TERM in compile line allows for both PEP and the NHPOS application
	// to run on the same PC.  This allows designer testing and debugging using both applications
	// simultaneously on the same PC.  This is not meant for a release build.
#pragma message("***  WARNING:  PEP_ON_TERM set.  Using port 2173 rather than 2172 for PcIF.")
    netinf.pszLocalPort  = "2173";//szNetPort;                /* ptr. to local port no.   */
#else
	{
		LONG   lRet;
		HKEY   hKey;
		TCHAR  wszKey[128] = {0};

		_stprintf (wszKey, _T("%s\\%s"), PIFROOTKEY, NETWORKKEY);

		lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, wszKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hKey);
		if (lRet == ERROR_SUCCESS)
		{
			DWORD  dwType, dwBytes;
			CHAR  wszData[16] = {0};

			dwBytes = sizeof(wszData);

			lRet = RegQueryValueEx(hKey, PORT, 0, &dwType, (UCHAR *)&wszData[0], &dwBytes);
			if (lRet == ERROR_SUCCESS)
			{
				netinf.pszRemotePort  = wszData;                /* ptr. to local port no.   */
			}
			lRet = RegCloseKey (hKey);
		}
	}
#endif

    /* use default vaule in PIFNET module */
    netinf.usMaxUsers    = 0;                   /* max. no. of users        */
    netinf.usMaxQueues   = 0;                   /* max. no. of queues       */
    netinf.usMaxLength   = 0;                   /* max. no. of message bytes*/
    netinf.usWorkMemory  = 0;                   /* max. no. of working mem. */
    netinf.usIdleTime    = 0;                   /* idle time in receiving   */
    
    /* pifnet.dll */
    sReturn = NetInitialize(&netinf);

    return sReturn;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT NetUninit32(VOID)                         **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:finalize routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
VOID NetUninit32(VOID)
{
    SHORT           nIndex;

    WSACleanup();
    FreeLibrary(hNetInstance);

    // free the file handle

    for (nIndex = 0; nIndex < PIF32_NFDES; nIndex++)
    {
        if (aNetTbl[nIndex].fState)
        {
            NetClose(aNetTbl[nIndex].sNetHandle);
        }
    }
    // initialzie

	memset(&aNetTbl, 0, sizeof(aNetTbl));
    //ZeroMemory(&aNetTbl, sizeof(aNetTbl));

    // free critical section

    DeleteCriticalSection(&g_csPif32Net);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifNetFinalize(VOID)                          **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:finalize routine of WinSock                         **
**                                                                  **
**********************************************************************
fhfh*/
VOID NetFinalize32(VOID)
{
    /* pifnet.dll */
    NetFinalize();
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PifGetDestHostAddress(UCHAR *puchDestAddr, UCHAR *puchSrcAddr  **
**                                                                  **
**  return:     none                                                **
**                                                                  **
**  Description:get destination host address from hostname          **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifGetDestHostAddress(UCHAR *puchDestAddr, UCHAR *puchSrcAddr)
{
    struct hostent *hp;
    UCHAR  aszHostName[16] = DEVICENAME;
    UCHAR  aszNum[4];
    USHORT usSrc;
    DWORD  dwError;

    if ((puchSrcAddr[0] == 192) &&
        (puchSrcAddr[1] == 0) &&
        (puchSrcAddr[2] == 0) &&
        (puchSrcAddr[3] <= PIF_NET_MAX_IP)) {

        /* 2172 cluster communication */

        usSrc = (USHORT)puchSrcAddr[3]; /* terminal no */
        if (usSrc/100) {
            aszNum[0] = ((UCHAR)(usSrc/100) | 0x30);
            aszNum[1] = ((UCHAR)((usSrc%100)/10) | 0x30);
            aszNum[2] = ((UCHAR)(usSrc%10) | 0x30);
            aszNum[3] = 0;
        } else
        if (usSrc/10) {
            aszNum[0] = ((UCHAR)(usSrc/10) | 0x30);
            aszNum[1] = ((UCHAR)(usSrc%10) | 0x30);
            aszNum[2] = 0;
        } else {
            aszNum[0] = (UCHAR)(usSrc | 0x30);
            aszNum[1] = 0;
        }

        /* device name "NCR2172-xx" */
        strcat(aszHostName, aszNum);

        hp = gethostbyname(aszHostName);

        if (hp == NULL) {

            puchDestAddr = INADDR_ANY;
            dwError = WSAGetLastError();

        } else {

            memcpy(puchDestAddr,hp->h_addr,hp->h_length);
        }
    } else {

        /* for PC communication */

        memcpy(puchDestAddr,puchSrcAddr, PIF_LEN_IP);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   BOOL        Pif32IsValidFile();
//
// Inputs:      nothing
//
// Outputs:     BOOL        fResult;    -
//
/////////////////////////////////////////////////////////////////////////////

BOOL PifIsValidNet(USHORT usFile)
{
    BOOL    fState;

    EnterCriticalSection(&g_csPif32Net);

    if (usFile < PIF32_NFDES)
    {
        fState = aNetTbl[usFile].fState;    // get handle state
    }
    else
    {
        fState = FALSE;
    }

    LeaveCriticalSection(&g_csPif32Net);

    // exit ...

    return fState;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    
//
// Prototype:   VOID        Net32AbortEx();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID Net32AbortEx(USHORT usFaultModule, USHORT usFaultCode, BOOL fAppl)
{

#ifdef _DEBUG

    PifAbortEx(usFaultModule, usFaultCode, TRUE);

#endif
}
/* end of net32.c */
