/*
*===========================================================================
* Title       : Write Function
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASYPWRT.C
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
* Jul-28-92:00.00.01:O.Nakada   : Initial
* Jul-01-93:01.00.00:O.Nakada   : Modify off line check.
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
#include "asy.h"


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
** Synopsis:    SHORT AsyWrite(DEVICE *pDevTbl, VOID *pWriteBuf,
*                              SHORT sWriteLen)
*     Input:    pDevTbl   - Device Table
*               pWriteBuf - Write Buffer 
*               sWriteLen - Write Buffer Length
*
** Return:      Function Status
*               SYSERR
*
** Description:
*===========================================================================
*/
SHORT AsyWrite(DEVICE *pDevTbl, VOID *pWriteBuf, SHORT sWriteLen)
{
    SHORT   sStatus;
    USHORT  usLine;
    USHORT  usCalculateCrc;

    /* Check ASY Driver Status */
    if (!(AsyData.fchStatus & ASY_OPEN)) {
        return (SYSERR);
    }
        
    /* Check Write Buffer Length */
    if (sWriteLen == 0) {
        return (sWriteLen);
    }
    else if (sWriteLen < 0) {
        return (SYSERR);
    }

    /* check line status */
    usLine = (USHORT)XinuControl(AsyData.hsSio,
                                 ASY_SIO_FUNC_STATUS);
    if (!(usLine & ASY_SIO_STS_DSR)) {
        return (XINU_OFFLINE);              /* off line */
    }

    /* Set DLC Address */
    *(UCHAR FAR *)pWriteBuf = AsyData.uchDlcAddr;

    /* Calculate CRC */
    usCalculateCrc = XinuCrc(pWriteBuf, sWriteLen, 0);

    /* Flush Read Queue */
    XinuControl(AsyData.hsSio,
                ASY_SIO_FUNC_FLUSH_READ);

    /* Write Select */
    if ((sStatus = AsyWriteSelect(AsyData.hsSio)) !=
        ASY_SELECT_SUCCESS) {
        return (sStatus);
    }

    /* Write Message */
    sStatus = AsyWriteMessage(AsyData.hsSio,
                              pWriteBuf,
                              sWriteLen,
                              usCalculateCrc);
    return (sStatus);

    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}


/*
*===========================================================================
** Synopsis:    SHORT AsyWriteSelect(SHORT hsSio)
*     Input:    hsSio - SIO Driver Handle
*
** Return:      Function Status
*               SYSERR             - System Error
*               TIMEOUT            - Timeout
*               ASY_SELECT_SUCCESS - 
*
** Description:
*===========================================================================
*/
SHORT AsyWriteSelect(SHORT hsSio)
{
    SHORT           sStatus;
    USHORT          usTimer;
    static UCHAR    auchBuffer[3];
    CHAR            chRetry = 3;

    while (chRetry >= 0) {
        sStatus = AsyWriteSelectData(hsSio);
        switch (sStatus) {
        case ASY_SLEEP:                     /* Receive Timeout */
            /* Set Receive Timer */
            usTimer = ASY_RESPONSE_TIMER;
            XinuControl(hsSio,
                        ASY_SIO_FUNC_RECEIVE_TIMER,
                        (USHORT FAR *)&usTimer,
                        sizeof(usTimer));

            do {
                sStatus = XinuRead(hsSio,
                                   auchBuffer,
                                   sizeof(auchBuffer));
            } while (((sStatus < 0) && (sStatus != TIMEOUT)) ||
                     (sStatus == sizeof(auchBuffer)));
            break;

        case ASY_SELECT_SUCCESS:
            return (sStatus);

        case ASY_TIMEOUT:
            break;

        default:                            /* not Use */
            return (SYSERR);
        }
        --chRetry;
    }

    /* Write EOT */
    *auchBuffer = ASY_EOT;
    XinuWrite(hsSio, auchBuffer, 1);

    return (TIMEOUT);
}


/*
*===========================================================================
** Synopsis:    SHORT AsyWriteSelectData(SHORT hsSio)
*     Input:    hsSio - SIO Driver Handle
*
** Return:      Function Status
*               ASY_SELECT_SUCCESS - Read Select
*               ASY_SLEEP          - 
*               ASY_TIMEOUT        - Timeout
*
** Description:
*===========================================================================
*/
SHORT AsyWriteSelectData(SHORT hsSio)
{
    USHORT          usTimer;
    SHORT           sStatus;
    static UCHAR    auchReadBuffer[1];
    static UCHAR    auchWriteBuffer[3] = {ASY_EOT, ASY_SEL, ASY_ENQ};

    /* Write Select */
    if ((sStatus = XinuWrite(hsSio, auchWriteBuffer, 3)) != 3) {
        return (ASY_SLEEP);
    }

    /* Set Response Timer */
    usTimer = ASY_RESPONSE_TIMER;
    XinuControl(hsSio,
                ASY_SIO_FUNC_RECEIVE_TIMER,
                (USHORT FAR *)&usTimer,
                sizeof(usTimer));

    /* Read ACK/NAK */
    if ((sStatus = XinuRead(hsSio, auchReadBuffer, 1)) != 1) {
        if (sStatus == TIMEOUT) {
            return (ASY_TIMEOUT);
        }
        else {
            return (ASY_SLEEP);
        }
    }
    if (*auchReadBuffer == ASY_ACK) {
        return (ASY_SELECT_SUCCESS);
    }
    else {
        return (ASY_SLEEP);
    }
}



/* ---------- End of ASYPWRT.C ---------- */
