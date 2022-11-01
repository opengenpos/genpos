/*
*===========================================================================
* Title       : Asynchornous Driver Header File
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASY.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Compact Model
* Options     : /c /Asfu /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
*    Date    Ver.      Name         Description
* Jul-28-92  00.00.01  O.Nakada     Initial
* Dec-01-92  01.00.02  O.Nakada     Modify monitor, receive, response,
*                                   non-activity timer.
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
*   Define Declarations 
*===========================================================================
*/
/*
*===========================================================================
*   Driver Status
*===========================================================================
*/
#define ASY_OPEN                    0x01


/*
*===========================================================================
*   I/O Control Function of Asynchoronus Driver
*===========================================================================
*/
#define ASY_FUNC_READ               1       /* Read Message */
#define ASY_FUNC_WRITE              2       /* Write Message */
#define ASY_FUNC_TIMER              3       /* Set Receive Timer */
#define ASY_FUNC_RECEIVE_TIMER      3       /* Set Receive Timer */
#define ASY_FUNC_SEND_TIMER         4       /* Set Receive Timer */
#define ASY_FUNC_DTR_ON             5       /* DTR On */
#define ASY_FUNC_DTR_OFF            6       /* DTR Off */


/*
*===========================================================================
*   I/O Control Function of Serial I/O Driver
*===========================================================================
*/
#define ASY_SIO_FUNC_STATUS         1       /* get line/modem status */
#define ASY_SIO_FUNC_RECEIVE_TIMER  5       /* Set Receive Timer */
#define ASY_SIO_FUNC_SEND_TIMER     6       /* Set Receive Timer */
#define ASY_SIO_FUNC_DTR_ON         7       /* DTR On */
#define ASY_SIO_FUNC_DTR_OFF        8       /* DTR Off */
#define ASY_SIO_FUNC_FLUSH_READ     9       /* Flush Read Queue */
#define ASY_SIO_FUNC_FLUSH_WRITE    10      /* Flush Write Queue */


/*
*===========================================================================
*   Line/Modem Status
*===========================================================================
*/
#define ASY_SIO_STS_FRAMING         0x0800
#define ASY_SIO_STS_PARITY          0x0400
#define ASY_SIO_STS_OVERRUN         0x0200
#define ASY_SIO_STS_RLSD            0x0080
#define ASY_SIO_STS_DSR             0x0020
#define ASY_SIO_STS_CTS             0x0010


/*
*===========================================================================
*   Timer
*===========================================================================
*/
#define ASY_SLEEP_TIMER             110

#define ASY_ENDLESS_TIMER           0
#define ASY_RECEIVE_TIMER           ((54 * 2) * 9)
#define ASY_RESPONSE_TIMER          (ASY_RECEIVE_TIMER * 4)
#define ASY_ACTIVITY_TIMER          (ASY_RECEIVE_TIMER * 5)


/*
*===========================================================================
*   Command Code
*===========================================================================
*/
#define ASY_SOH                     0x01
#define ASY_EOT                     0x04
#define ASY_ENQ                     0x05
#define ASY_ACK                     0x06
#define ASY_POL                     0x07
#define ASY_SEL                     0x08
#define ASY_NAK                     0x15


/*
*===========================================================================
*   Return Code
*===========================================================================
*/
#define ASY_SLEEP                   (-1000)
#define ASY_TIMEOUT                 (-1001)
#define ASY_READ_SOH                (-1002)
#define ASY_TERMINATE               (-1003)
#define ASY_CRC_ERROR               (-1004)
#define ASY_WRITE_ERROR             (-1005)
#define ASY_SELECT_SUCCESS          (-1006)


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Work Area
*===========================================================================
*/
typedef struct {
    SHORT     hsSio;                        /* SIO Driver Handle */
    UCHAR     fchStatus;                    /* Asynchronous Driver Status */
    UCHAR     uchDlcAddr;                   /* Source DLC Address */
} ASYDATA;


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern ASYDATA AsyData;


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ASYRM.C */
SHORT  AsyReadMessage(SHORT hsSio, VOID *pBuffer, SHORT sLength);
SHORT  AsyReadFirstCharacter(SHORT hsSio);
SHORT  AsyReadRemainMessage(SHORT hsSio, VOID *pReadBuf, SHORT sReadLen);

/* ASYWM.C */
SHORT  AsyWriteMessage(SHORT hsSio, VOID *pBuffer, SHORT sLength,
                       USHORT usCrc);
SHORT  AsyWriteMessageData(SHORT hsSio, VOID *pWriteBuf, SHORT sWriteLen,
                           USHORT usCrc);
SHORT  AsyWriteSelect(SHORT hsSio);
SHORT  AsyWriteSelectData(SHORT hsSio);



