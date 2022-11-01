/*
*===========================================================================
* Title       : Serial I/O Driver Header File
* Category    : Serial I/O Driver for PC, NCR 2170 Operating System
* Program Name: SIO.H
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
* Aug-10-92:00.00.01:O.Nakada   : Initial
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
*   Define Declarations 
*===========================================================================
*/
/*
*===========================================================================
*   Driver Status
*===========================================================================
*/
#define SIO_OPEN                0x01


/*
*===========================================================================
*   I/O Control Function
*===========================================================================
*/
#define SIO_FUNC_STATUS         1           /* Get Line/Modem status */
#define SIO_FUNC_RECEIVE_TIMER  5           /* Set Receive Timer */
#define SIO_FUNC_SEND_TIMER     6           /* Set Send Timer */
#define SIO_FUNC_DTR_ON         7           /* DTR On */
#define SIO_FUNC_DTR_OFF        8           /* DTR Off */
#define SIO_FUNC_FLUSH_READ     9           /* Flush Read Queue */
#define SIO_FUNC_FLUSH_WRITE    10          /* Flush Write Queue */


/*
*===========================================================================
*   Line/Modem Status
*===========================================================================
*/
#define SIO_STS_FRAMING         0x0800
#define SIO_STS_PARITY          0x0400
#define SIO_STS_OVERRUN         0x0200
#define SIO_STS_RLSD            0x0080
#define SIO_STS_DSR             0x0020
#define SIO_STS_CTS             0x0010


/*
*===========================================================================
*   Send/Receive Queue Size for Windows
*===========================================================================
*/
#define SIO_RECEIVE_QUEUE       3072
#define SIO_SEND_QUEUE          3072


/*
*===========================================================================
*   Error Status
*===========================================================================
*/
#define SIO_OPEN_BAUD           (-12)       /* invalid baud rate */
#define SIO_OPEN_CONTROL        (-50)       /* invalid parity, stop bits, data bits */

#define SIO_ERROR_CTS_DSR       (-57)       /* CTS Low and DSR Low */
#define SIO_ERROR_CTS           (-59)       /* CTS Low */
#define SIO_ERROR_DSR           (-60)       /* DSR Low */
#define SIO_ERROR_OVERRUN       (-62)       /* Overrun Error */
#define SIO_ERROR_FRAMING       (-63)       /* Framing Error */
#define SIO_ERROR_PARITY        (-64)       /* Parity Error */
#define SIO_ERROR_NO_PORT       (-65)       /* No Port Address */
#define SIO_ERROR_OVERFLOW      (-150)      /* Buffer Overflow */


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
    UCHAR   fchStatus;                      /* Asynchronous Driver Status */
    USHORT  usReceiveTimer;                 /* Receive Timer (msec) */
    USHORT  usSendTimer;                    /* Send Timer (msec) */
    short   hComm;                          /* Communication Handle for Windows */
} SIODATA;


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern SIODATA  SioData;


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
USHORT SioGetStatus(VOID);
int SioSetErrorCode(VOID);

