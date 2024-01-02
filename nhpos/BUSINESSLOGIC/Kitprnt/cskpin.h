/*========================================================================*\
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
*   Title              : Client/Server KITCHIN PRINTER module, Header file
*   Category           : Client/Server KITCHIN PRINTER module, NCR2170 US HOSPITALITY MODEL
*   Program Name       : CSKPIN.H
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*                          define, typedef, struct, prototype, memory
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date                 Ver.Rev    :NAME         :Description
*   May-06-92          : 00.00.01   :M.YAMAMOTO   :Initial
*   May-19-93          :            :M.YAMAMOTO   :Correct Recover Timer
*   Sep-01-93          : 02.00.00   :M.YAMAMOTO   :Increase Stack Size of
*                                                 :Print Stack Size
*   Apr-18-95          : 02.05.04   :M.SUZUKI     :Chg KPS_W_WIDE 21->20
*                                                 :    KPS_W_WIDE 01->0
*                                                 :Add KPS_FONT_B,KPS_ESC_DRAWER
*                                                 :Add KPS_SHRED_ADDR_#1
*   Nov-06-95          : 03.01.00   :T.Nakahata   :R3.1 Initial
*   Dec-26-95          : 03.01.00   :M.Suzuki     :Chg KPS_PAPER_CUT_FEED
*   Mar-21-96          : 03.01.01   :T.Nakahata   : PRINT_SEL_MASK 0x0f->0xff
*   Jul-12-96          : 03.01.09   :M.Suzuki     :Chg KPS_PAPER_CUT_FEED from 6 to 7
*** 2172 (Saratoga)
*   Feb-29-00          : 1.00       :K.Iwata      :initial
*   May-01-00          :            :M.Teraki     :*Changed both KPS_IDLE_TIMER_VALUE and
*                                                 : KPS_RECOVER_TIMER_VALUE to 30,3 -> 1,1 sec.
*                                                 :*Removed KpsCheckSharedPara()
*
** GenPOS **
*   Apr-24-15          : 02.02.01   :R.Chambers   : removed KPSSHRINF   KpsShrInf with move to cskpin.c
*   Jan-11-18 : 02.02.02 : R.Chambers  :implementing Alt PLU Mnemonic in Kitchen Printing.
*   Dec-24-23 : 04.01.00 : R.Chambers : removed prototype KpsItemPrint(). now static.
\*=======================================================================*/
/*
----------------------------------------------
   DEFINE
----------------------------------------------
*/
#define  KPS_TERMINAL_OF_CLUSTER       16

#define  KPS_IDLE_TIMER_VALUE         (1*1000)
#define  KPS_RECOVER_TIMER            1
#define  KPS_RECOVER_TIMER_VALUE      (KPS_RECOVER_TIMER*1000)
#define  KPS_5_MINUTE                 ((5*60)/KPS_RECOVER_TIMER)

#define  KPS_WAIT_POWER_DOWN_TIMER    (3*1000)

#define  KPS_SIZE_FOR_1ITEM         (24 * (STD_MAX_COND_NUM + 1))        // this may need to change if PRT_KPMAXITEM changes
/*
    PifWriteCom / PifReadCom / PifControlCom
*/
#define  KPS_MONITOR_SEND_TIME_OUT    1000              /*  1.0 SECOND used for monitoring t.o. */
#define  KPS_PRTDATA_SEND_TIME_OUT    2000              /*  2.0 SECOND used for printdata t.o. */
//#define  KPS_PRTDATA_SEND_TIME_OUT    4500              /*  4.5 SECOND used for printdata t.o. */
#define  KPS_RECEIVE_TIME_OUT         4500              /*  4.5 SECOND */
#define  KPS_READ_BUFFER_SIZE           10
#define  KPS_PRINT_DATA_SIZE          (KPS_FRAME_SIZE - KPS_HEADER_SIZE - KPS_CUSTOMER_NAME_SIZE - KPS_OPERATOR_SIZE - KPS_TRAILER_SIZE)

/*  PRINT BUFFER OFFSET */
#define KPS_COM_FRAME                   0
#define KPS_SEQ_NO                      2
#define KPS_DATA_HEAD                   4

/*    MEMORY ACCESS PROINTER */

/* auchKpsMainMemory
   auchKpsRcvMem */
#define  KPS_HEADER_SIZE                27
#define  KPS_CUSTOMER_NAME_SIZE         19              /* R3.0 */
#define  KPS_OPERATOR_SIZE              (4+1+20+1)      /* V3.3 */
/* #define  KPS_OPERATOR_SIZE              (4+1+10)     / R3.1 */

#define  KPS_CONS_SIZE                  4
#define  KPS_HEADER                     0
#define  KPS_CUSTOMER_NAME              (KPS_HEADER + KPS_OPERATOR_SIZE + KPS_HEADER_SIZE) /* R3.1 */
#define  KPS_TRAILER                    (KPS_CUSTOMER_NAME + KPS_CUSTOMER_NAME_SIZE)
#define  KPS_DATA                       (KPS_TRAILER + KPS_TRAILER_SIZE)
#define  KPS_BUFFER_HEAD                (6 + KPS_NUMBER_OF_PRINTER + KPS_NUMBER_OF_PRINTER)
#define  KPS_PRINT_BUFFER_SIZE          (KPS_TRAILER_SIZE + 16) /* V3.3 */
/* #define  KPS_PRINT_BUFFER_SIZE          (KPS_TRAILER_SIZE + 10) */

#define  KPS_DATE_TIME                  14
#define  KPS_ILI                        0               /* OFFSET */
#define  KPS_PRINT_SEL                  1               /* OFFSET PRINTER SELECTION */
#define  KPS_PRINT_SEL_MASK             0xff            /* MASK FOR PRINTER SELECTION */
/*#define  KPS_PRINT_SEL_MASK             0x0f          MASK FOR PRINTER SELECTION */
#define  KPS_VOID_MASK                  0x10
#define  KPS_COLOR_MASK                 0x20


/* ESC CODE
 * The following are standard characters used to create Epson ESC/P
 * escape code sequences for printer output changes such as print color,
 * print head movement, and paper cut. Epson is a defacto standard kitchen printer.
*/
#define KPS_ESC                     0x1b               // ESC or escape character used to start printer escape sequence for a printer command.
#define KPS_INIT                    0x40
#define KPS_NO_PAPER                0x63
#define KPS_NO_PAPER_STOP           0x01
#define KPS_W_WIDE                  0x20                /* Chg R3.0 21->20 */
#define KPS_S_WIDE                  0x00                /* Chg R3.0 01->00 */
#define KPS_FONT_B                  0x01                /* Add R3.0 */
#define KPS_ESC_BLACK               0x00                // set print color to black, requires KPS_ESC KPS_r sequence
#define KPS_ESC_RED                 0x01                // set print color to red, requires KPS_ESC KPS_r sequence
#define KPS_21                      0x21                // exclamation point (!) needed for some printer escape sequences.
#define KPS_r                       0x72                // lower case letter R, use for setting print color
#define KPS_d                       0x64                // lower case letter D, use for feeding lines and paper cut
#define KPS_ESC_FULL_CUT            0x69
#define KPS_ESC_PARTIAL_CUT         0x6d                /* RFC, Saratoga */
#define KPS_LF                      0x0a                // line feed character. this is not a return character but a line feed.
#define KPS_SPACE                   0x20                /* SPACE CODE FOR PRINTER */
#define KPS_ESC_STATUS              0x76
#define KPS_ESC_DRAWER              0x75                /* Add R3.0 Dummy Command */
#define KPS_DOUBLE                  0x12                /* Double Wide character indicator. See also FSC_DOUBLE_WIDE and PRT_DOUBLE and RFL_DOUBLE. Add R3.0 */

#define KPS_7B                      0X7B
#define KPS_0                       0X00

#define KPS_PAPER_CUT_FEED            7                 /* 5 -> 6 for 7193 Printer Dec-26-95 */
#define KPS_HEAD_FEED                10
#define KPS_HEAD_FEED_RECOVER         2

#define  KPS_FIRST_FRAME             0x01
#define  KPS_LAST_FRAME              0x80

/* Printer Status Codes */
#define KPS_PAPER_LOW				0x01
#define KPS_COVER_OPEN				0x02
#define KPS_PAPER_OUT				0x04
#define KPS_KNIFE_SLIP_JAM			0x08
#define KPS_STATUS_MASK				0x0E // paper low not a reason to set status down

#define  KPS_SUCCESS					0//RPH 11-11-3 SR# 201
#define  KPS_LOCK                       -1
#define  KPS_PRINT_ERROR                -2
#define  KPS_UNLOCK                     -3
#define  KPS_NOT_OPEN                   -4
#define  KPS_OPEND_ERROR                -5
#define  KPS_NOT_AS_MASTER              -6
#define  KPS_ERROR                      -7

#define  KPS_NO_READ_SIZE				-32//RPH 11-11-3 SR# 201

/* ### MOD 2172 Rel1.0 (Saratoga) move to cskp.h */
/*#define  KPS_NUMBER_OF_PRINTER          8             /* No. of Remote Printer (R3.1) */
/*#define  KPS_NUMBER_OF_PRINTER          4        */
#define  KPS_CONV_1                     100
#define  KPS_RETRY_COUNTER              2               /* FOR KITCHEN PRINTER IS BUSY */

/* Process Message */
#define  KPS_MSG_FROM_MONITER          'M'
#define  KPS_MSG_FROM_RCV_MONITER      'R'
#define  KPS_PRINT_MODULE              'P'
/*
    for KpsAlt()
*/
#define KPS_ALTER                       1
#define KPS_NOT_ALTER                   2
#define KPS_ALTER_RCV                   3

#define		KPS_TRANS_BGN	0x0016		/* Begin transaction call for shared printing*/
#define		KPS_TRANS_END	0x0017		/* End Transaction call for shared printing*/

#define		KPS_KITCHENPRINTING	  1
#define		KPS_REGULARPRINTING	  2

/*
    Power Down Recovery
*/
#define KPS_PRINT_DOWN                  1

#define KPS_NUMBER_OF_ALTMARK           9               /* Number of Alternation Mark */
#define KPS_NUMBER_OF_POWER_DOWN_MARK    21             /* NUmber of Power Down Mark Recover */
#define KPS_NUMBER_OF_DOWN_PRINTER_MARK  42             /* NUmber of Down Printer Mark */

#define KPS_NUMBER_QUE                  2               /* Number of Buffer Que */
/*
    KPS_FLAG
*/
#define KPS_STOP_RECOVERY               1               /* Stop Recovery Process */
#define KPS_POWERUP_OK                  2               /* Server and notice board
                                                           is standby-ok */
/***************************** ADD R3.0 ********************************/
/*
    SHARED PARAMETER
*/
#define KPS_SHRED_ADDR_1                32              /* Shared Printer Parameter Address */
#define KPS_SHRED_PRINTER               31              /* Shared Printer */

/***************************** ADD R3.0 ********************************/

/* ===== New Defines (Release 3.1) BEGIN ===== */

#define KPS_MANUAL_ALTER                0x8000          /* manual alternation */

/* ===== New Defines (Release 3.1) END ===== */

/***************************** ADD Saratoga ******************************/

/*
----------------------------------------------
   TYPEDEF
----------------------------------------------
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    TCHAR   uchCons[KPS_CONS_SIZE];                     /* Consec. No */
    TCHAR   uchOther[KPS_TRAILER_SIZE - KPS_CONS_SIZE]; /* Other Data */
}KPSTRAI;

typedef struct {
    TCHAR   COM_ID;                                     /* Frame Id */
    TCHAR   COM_SEQ_NO;                                 /* Sequence No */
    TCHAR   uchOperatorName[KPS_OPERATOR_SIZE];         /* R3.1 */
    TCHAR   uchHeader[KPS_HEADER_SIZE+KPS_CUSTOMER_NAME_SIZE];  /* Storage for Header */
    KPSTRAI Trailer;                                    /* Storage for Trailer */
    TCHAR   auchPrintData[KPS_PRINT_DATA_SIZE];
}KPS_1_FRAME;

typedef struct {
    VOID        *pNextQue;                              /* Next Que Address */
    ULONG       ulWritePoint;                           /* Data Size and Write Pointer */
    ULONG       ulReadPoint;                            /* Read Pointer */
    UCHAR       auchOutputPrinter[KPS_NUMBER_OF_PRINTER];   /* Printer No to output */
    UCHAR       auchRequestRecover[KPS_NUMBER_OF_PRINTER];  /* Printer No in Recovery */
    DATE_TIME   Date_Time;                              /* Date and Time save area when received */
    SHORT       hsKpsBuffFH;                            /* Print Data Size */

    /* ### ADD 2172 Rel1.0 (Saratoga) */
    USHORT  usOutPrinterInfo;                           /*  */
}KPSBUFFER;

typedef struct {
    KPSBUFFER   *pFirstQue;
    KPSBUFFER   *pLastQue;
}KPSQUE;

/***************************** ADD R3.0 ********************************/
typedef struct {
    SHRHEADERINF    HdInf;                          /* SHRHEADERINF is defined in 'SHR.H' */
    UCHAR           auchPrtData[KPS_PRINT_BUFFER_SIZE*2 + 1]; /* +1 is length */
}KPSSHRINF;
/***************************** ADD R3.0 ********************************/


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
----------------------------------------------
   PROTOTYPE
----------------------------------------------
*/

VOID  Kps_Table4Clear(UCHAR *auchTable);
SHORT KpsLockCheck(UCHAR uchUniqueAddress);
/*VOID  THREADENTRY KpsPrintProcess(VOID);*/
/*VOID  THREADENTRY KpsTimerProcess(VOID);*/
VOID  KpsMoniter(USHORT usInit);
VOID  KpsPrintEdit(KPSBUFFER *KpsBuffer);
VOID  KpsEachTran(KPSBUFFER *KpsBuffer);
VOID  KpsEachItem(KPSBUFFER *KpsBuffer);
VOID  KpsPrintEditRecover(KPSBUFFER *KpsBuffer);
SHORT KpsTranEdit(UCHAR uchPrintNo, KPSBUFFER *auchRcvBuffer, UCHAR uchPrinter, UCHAR *pauchDownPriter);
SHORT KpsItemEdit(UCHAR uchPrintNo, KPSBUFFER *pBuffer, UCHAR uchPrinter, UCHAR *pauchDownPriter);
SHORT KpsAlt(UCHAR uchPrintNo, UCHAR uchPrinter);
SHORT KpsPaperCut(UCHAR uchPrinterNo);
SHORT KpsTransactionPrint(UCHAR uchPrinterNo, UCHAR uchType);
SHORT KpsVoidPrint(UCHAR uchPrinterNo);
VOID  KpsMakeOutPutPrinter(KPSBUFFER *pKpsBuffer, UCHAR *puchPrinterStatus);
SHORT Kps_Print(UCHAR uchPrinterNo, UCHAR uchColor, UCHAR uchChrtype, TCHAR *puchDataAddress, UCHAR uchSize);
VOID  KpsAltCheck(UCHAR uchPrinterNo, KPSBUFFER *pKpsBuffer);
SHORT KpsWriteCom(UCHAR uchPrinterNo, TCHAR *puchBuffer, UCHAR cuchCode);
VOID  KpsDisplayPrtError(VOID);
VOID  KpsClearPrtError(VOID);
SHORT KpsPowerDownRecover(UCHAR uchPrinterNo);
SHORT KpsDownPrinterPrint(UCHAR uchPrinterNo);
SHORT KpsPaperFeed(UCHAR uchPrinterNo, UCHAR uchdFeed);
VOID  KpsClearRecoveryQue(VOID);
SHORT KpsRecoveryCheckAndInit(KPSQUE *Kpsque);
VOID  *KpsQueTransfer(KPSQUE *KpsDeque, KPSQUE *KpsEnque);
SHORT KpsPowerDownCheck(VOID);
SHORT KpsCheckFlag(VOID);
SHORT KpsCheckAsMaster(VOID);
SHORT KpsWaitPowerDown(VOID);
SHORT KpsCheckPrinting(VOID);
VOID  KpsStartPrint(VOID);
SHORT KpsCheckTime(DATE_TIME *pOld_Date_Time, DATE_TIME *pNow_Date_Time);
/* VOID KpsCheckSharedPara( VOID ); removed */          /* Add R3.0 */
VOID KpsShrInit(VOID);                                  /* Add R3.0 */
UCHAR   KpsCheckPort(VOID);                             /* Add R3.0 */

/* ### ADD 2172 Rel1.0 (Saratoga) */
/*** cskpconv.c ***/
VOID    _KpsMakePortTable(void);
SHORT   _KpsComNo2KPNo(SHORT nComNo);
SHORT   _KpsKPNo2ComNo(SHORT nKPNo);
USHORT  _KpsPortSts2PrtSts(void);
BOOL    _KpsIsEnable(SHORT usPrinterNo);
BOOL    _KpsIsSharedPrinter(SHORT nPrinterNo);
VOID    _KpsSetPrinterSts(SHORT sPrinterNo, BOOL bSetVal);
SHORT   _KpsGetMyTerminalNo(void);
SHORT   _KpsGetMySharedPrinterNo (UCHAR uchPrinterNo);
VOID    _ParaTest(void);                                /* for DEBUG */
BOOL    _KpsIsEpson(SHORT nPrinterNo);    /* EPSON check      */

/*** cskpinEx.c ***/
SHORT   _KpsGetAltPrinterNo(USHORT usOutPrinterInfo);
VOID    KpsMakeOutPutPrinterEx(KPSBUFFER *pKpsBuffer, USHORT * pPrinterSts);
BOOL    _KpsIsAlt(USHORT usOutPrinterInfo);


/*
-------------------------------------------------------------------------
   MEMORY
-------------------------------------------------------------------------
*/
extern USHORT        husKpsQueue;
extern PifSemHandle  husKpsQueueSem;

/*extern UCHAR   uchKpsPrinterStatus;*/                 /* printer status table */
extern USHORT  g_uchPortStatus;                         /* port status table    */

extern UCHAR   g_uchKpsTerminalLock;                    /* terminal check During excuting Terminal No. (uniqu address) */
extern SHORT   g_hasKpsPort[KPS_NUMBER_OF_PRINTER];     /* Port handle save area */
extern USHORT  usKpsTimerValue;                         /* Timer value */
extern UCHAR   g_auchKpsSysConfig[KPS_NUMBER_OF_PRINTER]; /* System Configration */
/* extern UCHAR   auchKpsSysConfig[PIF_LEN_PORT];  System Configration */

extern UCHAR   uchKpsPrintDown;                         /* for power down recovery */
extern UCHAR   uchKpsPrinting;                          /* Print thread acting */

extern  KPSQUE      KpsEmptyQue;                        /* buffer empty que */
extern  KPSQUE      KpsProcQue;                         /* buffer process que */
extern  KPSQUE      KpsRcvQue;                          /* buffer recovery que */

extern  UCHAR       auchKpsDownPrinter[KPS_NUMBER_OF_PRINTER];  /* save id of down printer */
extern  UCHAR       uchKpsFlag;                         /* Stop recovery process when
                                                           KPS_STOP_RECOVERY bit is ON.
                                                           Start Print when KPS_POWERUP_OK bit is ON. */

/*extern  UCHAR     uchKpsPrinterType;*/                /* Printer Type Flag Add R3.0  *//* ### DEL 2172 Rel1.0 */
// extern  KPSSHRINF   KpsShrInf;                          /* shared buffer    Add R3.0 */

/* ===== End of File (CSKPIN.H) ===== */
