/*
*===========================================================================
* Title       : Write Message Function
* Category    : Asynchornous Driver, NCR 2170 Operating System
* Program Name: ASYWM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A/7.00 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c {/ACw | /Asfw} /W4 {/G1s | /G2s} /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
*    Date    Ver.      Name         Description
* Jul-28-92  00.00.01  O.Nakada     Initial
*
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
** Synopsis:    SHORT AsyWriteMessage(SHORT hsSio, VOID *pReadBuf,
*                                     SHORT sReadLen, USHORT usCrc)
*     Input:    hsSio     - SIO Driver Handle
*    Output:    pWriteBuf - Write Buffer 
*     Input:    sWriteLen - Write Buffer Length
*               usCrc     - CRC Data
*
** Return:      Write Data Length
*               > 0              - Successful
*               XINU_WRITE_ERROR - Write Error
*
** Description:
*===========================================================================
*/
SHORT AsyWriteMessage(SHORT hsSio, VOID *pWriteBuf, SHORT sWriteLen,
                      USHORT usCrc)
{
    SHORT   sStatus;
    USHORT  usTimer;
    CHAR    chRetry = 3;
    static UCHAR auchBuffer[3];

    while (chRetry >= 0) {
        sStatus =
            AsyWriteMessageData(hsSio,
                                pWriteBuf,
                                sWriteLen,
                                usCrc);
        switch (sStatus) {
        case ASY_SLEEP:                     /* Receive Timeout */
            /* Set Response Timer */
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

        case ASY_TIMEOUT:                   /* Timeout */
            break;

        case XINU_WRITE_ERROR:              /* Write Buffer Overflow */
            return (sStatus);

        default:                            /* Successful */
            return (sStatus);
        }
        --chRetry;
    }

    /* Write EOT */
    *auchBuffer = ASY_EOT;
    XinuWrite(hsSio, auchBuffer, 1);

    return (XINU_WRITE_ERROR);
}


/*
*===========================================================================
** Synopsis:    SHORT AsyWriteMessageData(SHORT hsSio, VOID *pWriteBuf,
*                                         SHORT sWriteLen, USHORT usCrc)
*     Input:    hsSio     - SIO Driver Handle
*    Output:    pWriteBuf - Write Buffer 
*     Input:    sWriteLen - Write Buffer Length
*
** Return:      Write Message Status
*               > 0              - Successful
*               XINU_WRITE_ERROR - Write Buffer Overflow or Detect CRC Error
*               AST_TIMEOUT      - no-Response Timeout
*               ASY_SLEEP        - Receive Timeout or Error
*
** Description:
*===========================================================================
*/
SHORT AsyWriteMessageData(SHORT hsSio, VOID *pWriteBuf, SHORT sWriteLen,
                          USHORT usCrc)
{
    USHORT  usTimer;
    SHORT   sStatus;
    static UCHAR auchBuffer[3];

    /* Write SOH and LEN */
    *auchBuffer                    = ASY_SOH;
    *(SHORT FAR *)(auchBuffer + 1) = sWriteLen;
    if ((sStatus = XinuWrite(hsSio, auchBuffer, 3)) != 3) {
        return (ASY_SLEEP);
    }

    /* Write DATA */
    if ((sStatus = XinuWrite(hsSio, pWriteBuf, sWriteLen)) !=
        sWriteLen) {                        /* Write Error */
        return (ASY_SLEEP);
    }

    /* Write CRC */
    *(UCHAR FAR *)auchBuffer       = (UCHAR)(usCrc >> 8);   /* CRC High */
    *((UCHAR FAR *)auchBuffer + 1) = (UCHAR)usCrc;          /* CRC Low */
    if ((sStatus = XinuWrite(hsSio, auchBuffer, 2)) != 2) {
        return (ASY_SLEEP);
    }

    /* Set Response Timer */
    usTimer = ASY_RESPONSE_TIMER;
    XinuControl(hsSio,
                ASY_SIO_FUNC_RECEIVE_TIMER,
                (USHORT FAR *)&usTimer,
                sizeof(usTimer));

    /* Read ACK/NAK */
    if ((sStatus = XinuRead(hsSio, auchBuffer, 1)) != 1) {
        if (sStatus == TIMEOUT) {
            return (ASY_TIMEOUT);
        }
        else {
            return (ASY_SLEEP);
        }
    }
    if (*auchBuffer == ASY_ACK) {           /* ACK */
        return (sWriteLen);
    }
    else if (*auchBuffer == ASY_EOT) {      /* EOT */
        return (XINU_WRITE_ERROR);
    }
    else {                                  /* NAK or Other Character */
        return (ASY_SLEEP);
    }
}



