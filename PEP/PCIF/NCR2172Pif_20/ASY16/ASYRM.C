/*
*===========================================================================
* Title       : Read Message Function
* Category    : Asynchornous Driver, NCR 2170 Operating System
* Program Name: ASYRM.C
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
* Dec-02-92  01.00.02  O.Nakada     Add AsyReceive() for terminal version.
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
#ifdef PCIF_WINDOWS
#pragma message("This file is windows version.")
#else
#pragma message("This file is terminal version.")
#endif


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
** Synopsis:    SHORT AsyReadMessage(SHORT hsSio, VOID *pReadBuf,
*                                    SHORT sReadLen)
*     Input:    hsSio    - SIO Driver Handle
*    Output:    pReadBuf - Read Buffer 
*     Input:    sReadLen - Read Buffer Length
*
** Return:      Function Status
*               0 >             - Read Length
*               0 ==            - Empty
*               SYSERR          - System Error
*               TIMEOUT         - Timeout
*               XINU_READ_ERROR - Read Error
*
** Description:
*===========================================================================
*/
SHORT AsyReadMessage(SHORT hsSio, VOID *pReadBuf, SHORT sReadLen)
{
    USHORT usTimer;
    SHORT  sStatus;
    CHAR   chRetry = 3;

    while (chRetry >= 0) {
        sStatus = AsyReadFirstCharacter(hsSio);
        switch (sStatus) {
        case ASY_READ_SOH:                  /* Read SOH */
            sStatus =
                AsyReadRemainMessage(hsSio,
                                     pReadBuf,
                                     sReadLen);
            switch (sStatus) {
            case ASY_SLEEP:                 /* Receive Timeout */
                usTimer = ASY_RECEIVE_TIMER;
                XinuControl(hsSio,
                            ASY_SIO_FUNC_RECEIVE_TIMER,
                            (USHORT FAR *)&usTimer,
                            sizeof(usTimer));

                do {
#ifdef PCIF_WINDOWS
                    sStatus = XinuRead(hsSio,
                                       pReadBuf,
                                       sReadLen);
#else
                    sStatus = AsyReceive(hsSio,
                                         pReadBuf,
                                         sReadLen);
#endif
                } while (((sStatus < 0) && (sStatus != TIMEOUT)) ||
                         (sStatus == sReadLen));
                break;

            case ASY_CRC_ERROR:             /* Detect CRC Error */
                break;

            case XINU_READ_ERROR:           /* Read Buffer Overflow */
                return (sStatus);

            default:                        /* Successful */
                return (sStatus);
            }
            break;

        case ASY_SLEEP:
            usTimer = ASY_RECEIVE_TIMER;
            XinuControl(hsSio,
                        ASY_SIO_FUNC_RECEIVE_TIMER,
                        (USHORT FAR *)&usTimer,
                        sizeof(usTimer));

            do {
#ifdef PCIF_WINDOWS
                sStatus = XinuRead(hsSio,
                                   pReadBuf,
                                   sReadLen);
#else
                sStatus = AsyReceive(hsSio,
                                     pReadBuf,
                                     sReadLen);
#endif
            } while (((sStatus < 0) && (sStatus != TIMEOUT)) ||
                     (sStatus == sReadLen));
            break;

        case ASY_TIMEOUT:                   /* no-Activity Timeout */
            return (TIMEOUT);

        case ASY_TERMINATE:                 /* Empty */
            return (0);

        default:                            /* not Use */
            return (SYSERR);
        }
        --chRetry;
    }
    return (XINU_READ_ERROR);
}


/*
*===========================================================================
** Synopsis:    SHORT AsyReadFirstCharacter(SHORT hsSio)
*     Input:    hsSio - SIO Driver Handle
*
** Return:      Function Status
*               ASY_READ_SOH  - Read SOH
*               ASY_TIMEOUT   - Timeout
*               ASY_SLEEP     - Receive Timeout or Error
*               ASY_TERMINATE - Empty
*
** Description:
*===========================================================================
*/
SHORT AsyReadFirstCharacter(SHORT hsSio)
{
    USHORT          usTimer;
    SHORT           sStatus;
    static UCHAR    auchBuffer[1];

    /* Set Activity Timer */
    usTimer = ASY_ACTIVITY_TIMER;
    XinuControl(hsSio,
                ASY_SIO_FUNC_RECEIVE_TIMER,
                (USHORT FAR *)&usTimer,
                sizeof(usTimer));

    /* Read First Character */
    for (;;) {
        if ((sStatus = XinuRead(hsSio, auchBuffer, 1)) != 1) {
            if (sStatus == TIMEOUT) {
                return (ASY_TIMEOUT);       /* no-Activity Timeout */
            }
            else {
                return (ASY_SLEEP);
            }
        }

        if (*auchBuffer == ASY_SOH) {
            return (ASY_READ_SOH);
        }
        else if (*auchBuffer == ASY_EOT) {
            return (ASY_TERMINATE);
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT AsyReadRemainMessage(SHORT hsSio, VOID *pReadBuf,
*                                          SHORT sReadLen)
*     Input:    hsSio    - SIO Driver Handle
*    Output:    pReadBuf - Read Buffer 
*     Input:    sReadLen - Read Buffer Length
*
** Return:      Read Message Status
*               > 0             - Successful
*               XINU_READ_ERROR - Read Buffer Overflow
*               ASY_SLEEP       - Receive Timeout or Error
*               ASY_CRC_ERROR   - Detect CRC Error
*
** Description:
*===========================================================================
*/
SHORT AsyReadRemainMessage(SHORT hsSio, VOID *pReadBuf, SHORT sReadLen)
{
    USHORT  usCrc;
    USHORT  usTimer;
    SHORT   sStatus;
    SHORT   sReceiveLen;
    static UCHAR auchBuffer[2];

    /* Set Receive Timer */
    usTimer = ASY_RECEIVE_TIMER;
    XinuControl(hsSio,
                ASY_SIO_FUNC_RECEIVE_TIMER,
                (USHORT FAR *)&usTimer,
                sizeof(usTimer));

    /* Read LEN */
#ifdef PCIF_WINDOWS
    if ((sStatus = XinuRead(hsSio, auchBuffer, 2)) != 2) {
        return (ASY_SLEEP);
    }
#else
    if ((sStatus = AsyReceive(hsSio, auchBuffer, 2)) != 2) {
        return (ASY_SLEEP);
    }
#endif
    sReceiveLen = *(SHORT *)auchBuffer;

    /* Check Read Buffer Length */
    if (sReceiveLen > sReadLen) {           /* Buffer Overflow */
        do {
#ifdef PCIF_WINDOWS
            sStatus = XinuRead(hsSio,
                               pReadBuf,
                               sReadLen);
#else
            sStatus = AsyReceive(hsSio,
                                 pReadBuf,
                                 sReadLen);
#endif
        } while (((sStatus < 0) && (sStatus != TIMEOUT)) ||
                 (sStatus == sReadLen));

        /* Write EOT */
        *auchBuffer = ASY_EOT;
        XinuWrite(hsSio,
                  auchBuffer,
                  1);
        return (XINU_READ_ERROR);
    }
        
    /* Read DATA */
#ifdef PCIF_WINDOWS
    if ((sStatus = XinuRead(hsSio, pReadBuf, sReceiveLen)) !=
        sReceiveLen) {                      /* Read Error */
        return (ASY_SLEEP);
    }
#else
    if ((sStatus = AsyReceive(hsSio, pReadBuf, sReceiveLen)) !=
        sReceiveLen) {                      /* Read Error */
        return (ASY_SLEEP);
    }
#endif

    /* Read CRC */
#ifdef PCIF_WINDOWS
    if ((sStatus = XinuRead(hsSio, auchBuffer, 2)) != 2) {
        return (ASY_SLEEP);
    }
#else
    if ((sStatus = AsyReceive(hsSio, auchBuffer, 2)) != 2) {
        return (ASY_SLEEP);
    }
#endif

    /* Check CRC */
    usCrc = XinuCrc(pReadBuf, sReceiveLen, 0);
    usCrc = XinuCrc(auchBuffer, 2, usCrc);
    if (usCrc == 0) {
        *auchBuffer = ASY_ACK;
        XinuWrite(hsSio,
                  auchBuffer,
                  1);
        return (sReceiveLen);
    }
    else {
        *auchBuffer = ASY_NAK;
        XinuWrite(hsSio,
                  auchBuffer,
                  1);
        return(ASY_CRC_ERROR);
    }
}
