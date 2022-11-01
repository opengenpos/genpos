/*  
*===========================================================================
* Title       : YAM Driver Header File
* Category    : YAM Driver
* Program Name: YAM.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:     This header file defines Ymodem (YAM) Driver the 2170
*               operating system.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Sep-16-92:00.00.01:O.Nakada   : Initial
* Nov-4-92 :        :H.Shibamoto: modify for PC                         
* Nov-25-92:        :H.Shibamoto: add some parameters                      
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
#define YAM_OPEN                0x01

/*
*===========================================================================
*   Message Status
*===========================================================================
*/
#define YAM_R_BLOCK_N           0x02        /* Read Block No.n */
#define YAM_W_BLOCK_N           0x20        /* Write Block No.n */


/*
*===========================================================================
*   I/O Control Function Code
*===========================================================================
*/
#define YAM_FUNC_READ           1           /* Read Function */
#define YAM_FUNC_WRITE          2           /* Write Function */
#define YAM_FUNC_USER_TIMER     3           /* Receive Timer Function */
#define YAM_FUNC_SEND_TIMER     4           /* Send Timer Function */
#define YAM_FUNC_CANCEL         5           /* CANCEL */

#define YAM_SIO_FUNC_STATUS         1           /* Get Line/Modem status */
#define YAM_SIO_FUNC_RECEIVE_TIMER  5           /* Set Receive Timer */
#define YAM_SIO_FUNC_SEND_TIMER     6           /* Set Send Timer */
#define YAM_SIO_FUNC_DTR_ON         7           /* DTR On */
#define YAM_SIO_FUNC_DTR_OFF        8           /* DTR Off */
#define YAM_SIO_FUNC_FLUSH_READ     9           /* Flush Read Queue */
#define YAM_SIO_FUNC_FLUSH_WRITE    10          /* Flush Write Queue */




/*
*===========================================================================
*   Baud Rate
*===========================================================================
*/
#define YAM_BAUDRATE_9600       0xE0        /* Bits Set */
#define YAM_BAUDRATE_19200      0xE0        /* Bits Reset */


/*
*===========================================================================
*   Timer
*===========================================================================
*/

/* used in Buffer clear */
#define YAM_TIMER_SKIP          10          /* Skip Timer      */

/* used in receive process */
#define YAM_TIMER_SEND_C        1000        /* sleep before  sending C     */
#define YAM_TIMER_WAIT_1ST      3000        /* wait for 1st block after receing C */ 
#define YAM_TIMER_NEXTBLOCK     10000       /* wait for next block           */
#define YAM_TIMER_WAIT_ACK      60000       /* wait for response(ACK, NAK)   */

/* used in send process */
#define YAM_TIMER_CTS           1000        /* CTS timer       */

/*
*===========================================================================
*   Command Code
*===========================================================================
*/
#define YAM_SOH                 0x01        /* Start of Header */
#define YAM_STX                 0x02        /* Start of Text */
#define YAM_EOT                 0x04        /* End of Transmission */
#define YAM_ACK                 0x06        /* Acknowledge */
#define YAM_NAK                 0x15        /* Negative Acknowledge */
#define YAM_CAN                 0x18        /* Cancel Message */
#define YAM_CPMEOF              0x1A        /* End of File */
#define YAM_C                   0x43        /* Same as NAK */


/*
*===========================================================================
*   Return Code
*===========================================================================
*/
#define YAM_SIO_TIMEOUT         -0x97       /* Timeout for Serial I/O BIOS */

#define YAM_READ_C              1
#define YAM_READ_ACK            2
#define YAM_READ_NAK            3
#define YAM_READ_CAN            4


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
#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
typedef struct {
    UCHAR   fchStatus;                      /* YAM Driver Status  */
    UCHAR   fchMessage;                     /* Message Status     */
    USHORT  usPort;                         /* Port No.           */
    UCHAR   uchBlockNo;                     /* Block No.          */
    USHORT  usUserRecvTimer;                /* User Receive Timer */
    USHORT  usUserSendTimer;                /* User Send Timer    */
    USHORT  usReceiveTimer;                 /* Receive Timer      */
    USHORT  usSendTimer;                    /* Wake up Timer      */
    SHORT   sHsio;                          /* SIO driver handle  */
} YAMDATA;


/*
*===========================================================================
*   Serial Port Control
*===========================================================================
*/
typedef struct {
    USHORT usPort;                            /* Port # */
    USHORT usBaudRate;                        /* BaudRate */
    UCHAR  uchByteFormat;
} YAMPORT;

#if     (defined _WIN32_WCE || defined WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/* --- bit format --- */

/*  +---+---+---+---+---+---+---+---+ */
/*  | 7   6   5 | 4   3 | 2 | 1   0 | */
/*  +---+---+---+---+---+---+---+---+ */


#define BFMT_WLEN_8         0x03
#define BFMT_WLEN_7         0x02
#define BFMT_WLEN_MASK      0x03

#define BFMT_STOP_2         0x04

#define BFMT_PARITY_MASK    0x18
#define BFMT_PARITY_NONE    0x00
#define BFMT_PARITY_ODD     0x08
#define BFMT_PARITY_ZERO    0x10
#define BFMT_PARITY_EVEN    0x18


/*
*===========================================================================
*   Serial Port Control(SIO)
*===========================================================================
*/

#define CTRL_BYTESIZE_8      8   /* Byte Size = 8 bit */
#define CTRL_BYTESIZE_7      7   /* Byte Size = 7 bit */

#define CTRL_PARITY_NONE    0
#define CTRL_PARITY_ODD     1
#define CTRL_PARITY_EVEN    2
#define CTRL_PARITY_ZERO    4

#define CTRL_STOPBIT_15     0   /* Stop Bit = 1.5 */
#define CTRL_STOPBIT_1      1   /* Stop Bit = 1   */
#define CTRL_STOPBIT_2      2   /* Stop Bit = 2   */


/*
*===========================================================================
*   retry switch
*===========================================================================
*/

#define YAM_RETRY_YES       1
#define YAM_RETRY_BK0       2
#define YAM_RETRY_BK1       3
#define YAM_RETRY_NO        0



/*
*===========================================================================
*   data length
*===========================================================================
*/

#define     DATA_LEN_PATHNAME   128     /* PATHNAME     length */
#define     DATA_LEN_BLOCK      1024    /* 1block       length */
#define     DATA_LEN_HEADER     1       /* HEADER       length */
#define     DATA_LEN_BNUMBER    2       /* BLOCK NUMBER length */
#define     DATA_LEN_CRC        2       /* CRC          length */

#define     DATA_LEN_1BYTE      1       /* 1 byte */

/*
*===========================================================================
*   block No.
*===========================================================================
*/

#define     BLOCK_NUM_0     0
#define     BLOCK_NUM_1     1

/*
*===========================================================================
*   retry count
*===========================================================================
*/

#define     YAM_RETRY       9

/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern YAMDATA NEAR YamData;
extern SHORT   sAsyncWriteLength;           /* async write data length */
extern UCHAR   uchAsyncWriteBuffer[10];     /* async write data buffer */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/


/* YAMREAD.C */
SHORT   YamReadSwitch(VOID *pReadBuf, SHORT sReadLen);
VOID    YamReadFuncCancel(VOID);
VOID    YamReadFuncReset(VOID);

SHORT   YamAsyncWrite(SHORT dev, VOID *buff, SHORT count);
SHORT   YamReadAfterAsyncWrite(SHORT dev, VOID *buffer, SHORT numchars);
VOID    YamCleanupAsyncWrite();

SHORT   YamReadPath(VOID *pReadBuf, SHORT sReadLen);
SHORT   YamRead1KblockWoHdr(VOID *pReadBuf, SHORT sReadLen);

VOID    YamClearBuffer(VOID);


/* YAMRM.C */
SHORT   YamReadBlocktoCrc(VOID *pReadBuf, SHORT sLength, UCHAR *puchRcvBlockNum);
SHORT   YamCheckCrc(VOID *pReadBuf, SHORT sLength, VOID *pCrc );

/* YAMWRITE.C */
SHORT   YamWriteSwitch(VOID *pWriteBuf, SHORT sWriteLen);
SHORT   YamWritePathname(VOID *pWriteBuf, SHORT sWriteLen);
VOID    YamWriteFuncCancel(VOID);
VOID    YamWriteFuncReset(VOID);

SHORT   YamWritePath(VOID *pWriteBuf, SHORT sWriteLen);
SHORT   YamWrite1Kblock(VOID *pWriteBuf, SHORT sWriteLen);
SHORT   YamWriteNullpath(VOID *pWriteBuf, SHORT sWriteLen);

/* YAMWM.C */
SHORT   YamWriteMessage(UCHAR uchHeader, VOID *pWriteBuf, SHORT sWriteLen);

/* YAMRESP.C */
SHORT   YamReadResponse(VOID);


