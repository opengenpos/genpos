/*
*===========================================================================
* Title       : Modem Connect Function
* Category    : PC Interface for STUB DLL, NCR 2170 PC Interface
* Program Name: PCIFCON.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Large Model
* Options     : /c /Alnw /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Sep-07-92:00.00.01:O.Nakada   : Initial
* Sep-06-95:03.00.00:O.Nakada   : Modified receive timer from 30 to 65.
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
#include <string.h>
#include <stdlib.h>
#include "ecr.h"
#include "r20_pif.h"
#include "pifctl.h"
#include "pcif.h"
#include "pcifl.h"


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
extern USHORT          husCliExeNet;       /* Semapho, ISP - APPLI */

SHORT	PcifConnectSub(UCHAR FAR * puchData);


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
** Synopsis:    SHORT STUBENTRY PcifConnect(UCHAR FAR *puchData)
*     Input:    puchData - Send Data
*
** Return:      PCIF_SUCCESS - Successful
*
** Description:
*               AT - atention prefix of Hayes AT command
*               E0 - echo off
*               Q0 - characters not echoed
*               V0 - digit result code
*===========================================================================
*/
SHORT STUBENTRY PcifConnect(UCHAR FAR *puchData)
{
	SHORT	sStatus;

    PifRequestSem(husCliExeNet);
	sStatus = PcifConnectSub(puchData);
    PifReleaseSem(husCliExeNet);
    return sStatus;    
}

SHORT PcifConnectSub(UCHAR FAR *puchData)
{
    SHORT   sStatus;
    USHORT  usRead;
    USHORT  usReadTimer;
    UCHAR   auchBuffer[67];
    PCIF_CONNECT Connect;

    /* ----- set receive timer ----- */
    usReadTimer = 2000;                         /* 2 sec */
    PifControl(PIF_ASY_RECEIVE_TIMER, &usReadTimer);

    /* ----- initial strings ----- */
    strcpy((CHAR *)auchBuffer, "AT");     /* AT command */
    Connect.pData    = auchBuffer;
    Connect.usLength = (USHORT)strlen((CHAR *)auchBuffer);
    if (PifControl(PIF_ASY_WRITE, &Connect) != (SHORT)Connect.usLength) {
        return (PCIF_ERROR);
    }

    PifSleep(200);

    strcpy((CHAR *)auchBuffer, "E0Q0V0\r\n");       /* AT command */
    Connect.pData    = auchBuffer;
    Connect.usLength = (USHORT)strlen((CHAR *)auchBuffer);
    if (PifControl(PIF_ASY_WRITE, &Connect) != (SHORT)Connect.usLength) {
        return (PCIF_ERROR);
    }

    /* ----- receive echo back and result code ----- */
    Connect.usLength = 2;
    PifControl(PIF_ASY_READ, &Connect);
    if (*auchBuffer == 'A' && *(auchBuffer + 1) == 'T') {   /* enable echo back */
        Connect.usLength = 10;
        PifControl(PIF_ASY_READ, &Connect);
    }

    PifSleep(500);

    strcpy((CHAR *)auchBuffer, "AT");
    Connect.pData    = auchBuffer;
    Connect.usLength = (USHORT)strlen((CHAR *)auchBuffer);
    if (PifControl(PIF_ASY_WRITE, &Connect) != (SHORT)Connect.usLength) {
        return (PCIF_ERROR);
    }
    PifSleep(200);

    /* ----- set receive timer ----- */
    usReadTimer = 65000;                        /* 65 sec */
    PifControl(PIF_ASY_RECEIVE_TIMER, &usReadTimer);

    if (((*puchData == 'A') || (*puchData == 'a')) &&
        ((*(puchData + 1) == 'T') || (*(puchData + 1) == 't'))) {
        puchData += 2;
    }
    /* ----- send data ----- */
    Connect.pData    = puchData;
    Connect.usLength = (USHORT)strlen((CHAR *)puchData);
    if (PifControl(PIF_ASY_WRITE, &Connect) != (SHORT)Connect.usLength) {
        return (PCIF_ERROR);
    }

    /* ----- receive result code ----- */
    usRead  = 0;
    sStatus = 0;
    Connect.usLength = 1;
    do {
        usRead += sStatus;
        Connect.pData = auchBuffer + usRead;
        sStatus = PifControl(PIF_ASY_READ, &Connect);
    } while ((sStatus == 1) && (*(auchBuffer + usRead) != '\r'));

    if (sStatus < 0) {
        return (sStatus);
    } else {
        *(auchBuffer + usRead) = '\0';
        return ((SHORT)atoi((CHAR *)auchBuffer));
    }
}
