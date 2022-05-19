/*
*===========================================================================
* Title       : ESC/POS Printer Driver Header File
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCP.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This header file defines ESC/POS Printer (ESCP) Controler for 
*               the 2170 system.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*                   3. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
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

/*==========================================================================
**  Constant definitions
**==========================================================================
*/
extern CHAR EscpConf[];   /* printer configuration information */
extern CHAR Escv[];             /* ESC v sequence                   */

/*==========================================================================
**  FIFO Queue used for "Sequence #" of each print line
**==========================================================================
*/
#define Nesc            1           /* Number of escp controler */
#define ESCP_FIFO_SIZE  8           /* Maximum number of items in FIFO  */

typedef struct {
    UCHAR   uchFirst;               /* Index to the first item in FIFO  */
    UCHAR   uchCount;               /* Number of items in FIFO          */
    UCHAR   uchItem[ESCP_FIFO_SIZE];/* Items in FIFO                    */
} ESCP_FIFO;

SHORT   EscpFifoEnque(ESCP_FIFO *pFifo, UCHAR uchItem);
SHORT   EscpFifoDeque(ESCP_FIFO *pFifo);
SHORT   EscpFifoClear(ESCP_FIFO *pFifo);

/*==========================================================================
**  I/O Control Block Definition
**==========================================================================
*/

typedef struct {
    CHAR    chOpen;                 /* True when the driver is opened   */
    SHORT   sSio;                   /* Serial I/O port handle           */
    PifSemHandle   sSem;
    UCHAR   uchStatus;              /* Printer status saved             */
    SHORT   sEscvCount;             /* "ESC v" Issued Counter           */
    ESCP_FIFO Fifo;                 /* FIFO queue of Sequence #         */
} ESCP;

extern  ESCP    Escp[];        /* nEscp is the number of entries   */

/*==========================================================================
**  User Interface Definitions
**==========================================================================
*/
/*-------------------------------------------------------------------------
*   Parameter given to EscpOpen()
*--------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------
*   Request Message given to EscpWrit()
*--------------------------------------------------------------------------
*/
typedef struct {
    CHAR    achIdent[3];            /* Not used                         */
    UCHAR   uchSeqNo;               /* Sequence #                       */
    UCHAR   auchData[1];            /* Print Data                       */
} ESCP_REQUEST;

/*-------------------------------------------------------------------------
*   Response Message returned by EscpRead()
*--------------------------------------------------------------------------
*/
typedef struct {
    CHAR    achIdent[3];            /* Not used                         */
    UCHAR   uchSeqNo;               /* Sequence #                       */
    UCHAR   uchStatus;              /* Status of the printer            */
    UCHAR   uchError;               /* Error code, always 0             */
    UCHAR   uchAddr;                /* Interconnect address, always 0   */
} ESCP_RESPONSE;

#define ESCP_NEAR_END   0x01        /* Paper roll is near end           */
#define ESCP_NOT_ERROR  0x04        /* Not error                        */
#define ESCP_INIT       0x20        /* Initialize complete              */

/*-------------------------------------------------------------------------
*   I/O Control Functions
*--------------------------------------------------------------------------
*/
#define ESCP_STATUS_READ    1       /* returns printer status           */
#define ESCP_LOOP_TEST      2       /* loop test performs no operation  */
#define ESCP_READ_CONF      3       /* returns printer configuration    */
#define ESCP_CLEAR_SEQ      4       /* synchronizes sequence #          */
#define ESCP_DIRECT_WRITE   5       /* outputs data without ESC v       */

typedef struct {
    CHAR    achIdent[3];            /* Not used                         */
    UCHAR   uchSeqNo;               /* Sequence #                       */
    UCHAR   auchPrtConf;            /* Printer Configuration            */
} ESCP_CONF_HEADER;


SHORT EscpReadStatus(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen);
SHORT EscpLoopTest(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen);
SHORT EscpReadConf(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen);
SHORT EscpClearSeq(ESCP *pEscp);
SHORT EscpDirectWrite(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen);


/*-------------------------------------------------------------------------
*   Printer configuration status 
*--------------------------------------------------------------------------
*/

#define ESCP_CONF_CUTTER    0x01;   /* Printer has cutter. */

/*-------------------------------------------------------------------------
*   Data length 
*--------------------------------------------------------------------------
*/

#define LEN_NULL    1       /* length of NULL */


/*-------------------------------------------------------------------------
*   Return Values
*--------------------------------------------------------------------------
*/

    /* This drivers returns error codes without any conversion which are
       given by the SIO driver */

/*-------------------------------------------------------------------------
*   Response Message returned by EscpRead()
*--------------------------------------------------------------------------
*/

/* ######## Response to PMG ######## */

/***    ucPrtStatus     ***/

//		The status bits for printing must match up in three		
//		files: DeviceIOPrinter.h, ESCP.H, pmg.H. If any of		
//		the status bits change, or new ones are added, they		
//		must be added to all files.								
#define PRT_STAT_R_NEAR_END 0x01        /* receipt paper near end       */
#define PRT_STAT_TAILEND    0x02        /* tail end                     */
#define PRT_STAT_ERROR      0x04        /* not error                    */
#define PRT_STAT_FORM_IN    0x08        /* form in                      */
#define PRT_STAT_J_NEAR_END 0x10        /* journal paper near end       */
#define PRT_STAT_INIT_END   0x20        /* 1:initialize cmplete             */
#define PRT_STAT_POWER_OFF	0x40		/***    printer status of PmgGetPrtStatus (slip)    ***/

/***    ucPrtError      ***/
#define PRT_ERR_HOME        1           /* can't move home position         */
#define PRT_ERR_TIMING      2           /* timing palse error               */
#define PRT_ERR_JAM         3           /* motor jam error                  */
#define PRT_ERR_VALIDATION  4           /* validation error                 */
#define PRT_ERR_VOLTAGE     5           /* voltage error                    */
#define PRT_ERR_COVER_OPEN  6           /* paper cover open                 */
#define PRT_ERR_PAPER_END   7           /* Paper end                        */
#define PRT_ERR_KNIFE       8           /* knife error                      */

/* ######## Response from ESC v ######## */

/***    ucPrtStatus     ***/
#if defined (DEVICE_7158) || defined (DEVICE_7194) || defined (DEVICE_2208)
#define ESCV_STAT_R_NEAR_END    0x01    /* receipt paper near end  */
#define ESCV_STAT_COVER_OPEN    0x02    /* Cover Open  */
#define ESCV_STAT_JAM           0x08    /* Knife or Slip Jam  */
#define ESCV_STAT_J_NEAR_END    0x00    /* 0x00: no journal station */
#define ESCV_STAT_SLIP_IN       0x20    /* On: no slip paper (Slip leading edge) */
#define ESCV_STAT_SLIP_OUT      0x40    /* On: no slip paper (Slip Trailing edge) */
#define ESCV_STAT_NO_VALIDATION 0x00    /* 0x00: no validation station */
#endif

#if defined (DEVICE_7161)
#define ESCV_STAT_R_NEAR_END    0x00    /* 0x00: no sensor */
#define ESCV_STAT_COVER_OPEN    0x00    /* 0x00: no sensor */
#define ESCV_STAT_JAM           0x00    /* 0x00: no sensor */
#define ESCV_STAT_J_NEAR_END    0x00    /* 0x00: no journal station */
#define ESCV_STAT_SLIP_IN       0x20    /* On: no slip paper (Slip leading edge) */
#define ESCV_STAT_SLIP_OUT      0x40    /* On: no slip paper (Slip Trailing edge) */
#define ESCV_STAT_NO_VALIDATION 0x00    /* 0x00: no validation station */
#endif

#if defined (DEVICE_7140)
#define ESCV_STAT_J_NEAR_END    0x01    /* journal paper near end  */
#define ESCV_STAT_R_NEAR_END    0x02    /* receipt paper near end  */
#define ESCV_STAT_NO_SLIP       0x00    /* 0x00: no slip station   */
#define ESCV_STAT_NO_VALIDATION 0x40    /* On: no validation paper */
#endif

#if defined (DEVICE_7196)
#define ESCV_STAT_R_NEAR_END    0x02    /* receipt paper near end  */
#define ESCV_STAT_COVER_OPEN    0x00    /* Cover Open  */
#define ESCV_STAT_JAM           0x00    /* 0x00: no sensor */
#define ESCV_STAT_J_NEAR_END    0x00    /* 0x00: no journal station */
#define ESCV_STAT_SLIP_IN       0x00    /* 0x00: no slip station */
#define ESCV_STAT_SLIP_OUT      0x00    /* 0x00: no slip station */
#define ESCV_STAT_NO_VALIDATION 0x00    /* 0x00: no validation station */
#endif

/* ######## Response from DLE EOT ######## */

#define DLE_EOT_WAIT_ESCV               0
#define DLE_EOT_GET_STATUS              1
#define DLE_EOT_GET_OFFLINE_STATUS      2
#define DLE_EOT_GET_ERROR_STATUS        3
#define DLE_EOT_GET_PAPER_STATUS        4
#define DLE_EOT_GET_SLIP_STATUS         5
#define DLE_EOT_GET_VALIDATION_STATUS   6
#define DLE_EOT_END_STAT                7

#define STAT_DLE_EOT_DATA       0x10    /* DLE EOT status */

/* Printer Information */

/* BYTE 1 : Transmit Printer Status */
#define STAT1_NOUSE_BIT0        0x01    /* Fixed to Off */
#define STAT1_NOUSE_BIT1        0x02    /* Fixed to On */
#define STAT1_DRAWER_CLOSED     0x04    /* On:  cash drawers closed */
                                /* Off: cash drawers open */
#define STAT1_PRINTER_BUSY      0x08    /* On:  Printer is Busy at the RS-232C interface */
                                /* Off: Not busy at the RS-232C interface */
#define STAT1_NOUSE_BIT4        0x10    /* Fixed to On */
#define STAT1_NOUSE_BIT5        0x20    /* Undefined */
#define STAT1_NOUSE_BIT6        0x40    /* Undefined */
#define STAT1_NOUSE_BIT7        0x80    /* Fixed to On */

/* BYTE 2 : Transmit RS-232C Busy Status */
#define STAT2_NOUSE_BIT0        0x01    /* Fixed to Off */
#define STAT2_NOUSE_BIT1        0x02    /* Fixed to On */
#define STAT2_R_COVER_CLOSED    0x04    /* On:  Both receipt and cassette doors closed */
                                /* Off: Receipt or cassette door open */
#define STAT2_NOT_PRESS_PAPER_BOTTON    0x08    /* On:  Paper feed button is not pressed */
                                        /* Off: Paper feed button is pressed */
#define STAT2_NOUSE_BIT4        0x10    /* Fixed to On */
#define STAT2_ERR_NO_PAPER      0x20    /* On:  Printing stopped due to paper condition */
                                /* Off: Printing not stopped due to paper condition */
#define STAT2_ERR_CONDITION     0x40    /* On:  Error condition exists in the printer */
                                /* Off: No error condition */
#define STAT2_NOUSE_BIT7        0x80    /* Fixed to Off */

/* BYTE 3 : Transmit Error Status */
#define STAT3_NOUSE_BIT0        0x01    /* Fixed to Off */
#define STAT3_NOUSE_BIT1        0x02    /* Fixed to On */
#define STAT3_ERR_SLIP_JAM      0x04    /* On:  Slip motor or flip jam occurred */
                                /* Off: No slip motor or flip jam */
#define STAT3_ERR_CUTTER_JAM    0x08    /* On:  Knife error occurred */
                                /* Off: No knife error */
#define STAT3_NOUSE_BIT4        0x10    /* Fixed to On */
#define STAT3_ERR_UNRECOVERABLE 0x20    /* On:  Unrecoverable error occurred */
                                /* Off: No unrecoverable error */
#define STAT3_ERR_AC_VOLTAGE    0x40    /* On:  voltage are out of range */
                                /* Off: voltage are in range */
#define STAT3_NOUSE_BIT7        0x80    /* Fixed to Off */

/* BYTE 4 : Transmit Receipt Paper Status */
#define STAT4_NOUSE_BIT0        0x01    /* Fixed to Off */
#define STAT4_NOUSE_BIT1        0x02    /* Fixed to On */
#define STAT4_JOURNAL_NEAR_END  0x04    /* On:  Journal paper low */
#define STAT4_RECEIPT_NEAR_END  0x08    /* Off: Receipt paper low */
#define STAT4_NOUSE_BIT4        0x10    /* Fixed to On */
#define STAT4_JOURNAL_END       0x20    /* On:  Journal paper exhausted */
#define STAT4_RECEIPT_END       0x40    /* Off: Receipt paper exhausted */
#define STAT4_NOUSE_BIT7        0x80    /* Fixed to Off */

/* BYTE 5 : Transmit Slip Paper Status */
#define STAT5_NOUSE_BIT0        0x01    /* Fixed to Off */
#define STAT5_NOUSE_BIT1        0x02    /* Fixed to On */
#define STAT5_SLIP_NOT_SERECT   0x04    /* On:  Slip paper not selected */
                                /* Off: Slip paper selected */
#define STAT5_SLIP_PAPER_WAIT   0x08    /* On:  Waiting for slip paper insertion */
#define STAT5_NOUSE_BIT4        0x10    /* Fixed to On */
#define STAT5_SLIPIN_NO_PAPER   0x20    /* On:  no slip paper (Slip leading edge) */
#define STAT5_SLIPOUT_NO_PAPER  0x40    /* On:  no slip paper (Slip trailing edge) */
#define STAT5_NOUSE_BIT7        0x80    /* Fixed to Off */

/* BYTE 6 : Transmit Validation Paper Status */
#define STAT6_NOUSE_BIT0            0x01    /* Fixed to Off */
#define STAT6_NOUSE_BIT1            0x02    /* Fixed to On */
#define STAT6_VALIDATE_NOT_SERECT   0x04    /* On:  Validation paper not selected */
                                    /* Off: Validation paper selected */
#define STAT6_VALIDATE_PAPER_WAIT   0x08    /* On:  Waiting for Validation paper insertion */
#define STAT6_NOUSE_BIT4            0x10    /* Fixed to On */
#define STAT6_VALIDATE_NO_PAPER     0x20    /* On:  no Validation paper */
#define STAT6_VALIDATE_NO_PAPER1    0x40    /* On:  no Validation paper */
#define STAT6_NOUSE_BIT7            0x80    /* Fixed to Off */

/*==========================================================================
**  Interface to Serial I/O Driver
**==========================================================================
*/
/*-------------------------------------------------------------------------
*   SioOpen()
*--------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------
*   SioCntl()
*--------------------------------------------------------------------------
*/
#define SIO_SET_RECV_TIMER  5       /* Function to set receive timer    */
#define SIO_SET_SEND_TIMER  6       /* Function to set send timer       */

#define ESCP_TIMER_SEND 3000        /* send timer 3 sec                 */
#define ESCP_TIMER_RECV 100         /* receive timer 100 ms             */
#define ESCP_RETRY_RECV 100          /* wait for response for 30*100ms   */
#define ESCP_RETRY_CLEAR 30         /* clear receive buffer in 30*100ms */

SHORT EscpCloseCom(SHORT sDummy);
SHORT EscpControlCom(SHORT sDummy, SHORT sFunc, ...);
SHORT EscpInitCom(VOID);
SHORT EscpOpenCom(USHORT usPortId, PROTOCOL *pProtocol);
SHORT EscpReadCom(SHORT sDummy, VOID FAR *pBuff, SHORT sCount);
SHORT EscpWriteCom(SHORT sDummy, VOID FAR *pBuff, SHORT sCount);

SHORT EscpReadPrinterStatus(UCHAR *puchStat, UCHAR *puchError);

