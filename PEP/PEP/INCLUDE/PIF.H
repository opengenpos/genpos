/*hhhh
*****************************************************************************
**    
**  Title:      Header File for Portable Platform Interface
**
**  File Name:  PIF.H
**
**  Categoly:   Portable Platform Interface
**
**  Abstruct:   This header file provides function prototype declarations
**              for Portable Platform Interface functions and defines data
**              types and symbolic constants used with the functions.
**              Applications, System Applications, Recovery Manager, Platform
**              Interface and Simulator modules should include this file.
**
**              When the Simulator includes this header, the Simulator should
**              defines "SIM". When the Platform Interface includes this
**              header, the Platform Interface should defines "PIFXINU".
**              When the other modules includes this header, they should
**              define nothing. 
**
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date       Name           Description        
**  1.00      MAY-01-92  I.Shimizu      Initial release
**  00.00.01  AUG-17-92  I.Shimizu      Correspond to rev. G document
**            AUG-18-92  I.Shimizu      Add DEVICE_COIN_DISPENSER
**            AUG-20-92  I.Shimizu      Add PIF_ERROR_COM_BUSY
**            AUG-24-92  I.Shimizu      Change PIF_ERROR_COM_BUSY
**            SEP-04-92  I.Shimizu      Add PIF_ERROR_NET_XXXX
**                                      Add PifOpenMsr(), PifReadMsr() and
**                                      PifClose()
**                                      Modify PifGetWaiter()
**                                      Change the loaded S/W information
**            SEP-05-92  I.Shimizu      Add usRestartFlag in SYSCONFIG
**            SEP-11-92  O.Nakada       Modify fPip of PROTOCOL
**            SEP-11-92  O.Nakada       Add PIF_COM_PROTOCOL_XXX
**            SEP-16-92  O.Nakada       Add PIF_YAM_XXX
**            SEP-17-92  I.Shimizu      Modify PIF_SOFT_TABLE, PIF_SOFT_INFO
**                                      and SYSCONFIG
**            SEP-17-92  O.Nakada       Add PIF_COM_SEND_TIME,
**                                      PIF_YAM_SEND_TIMER
**            SEP-19-92  I.Shimizu      Modify PifXinuInit()
**            SEP-21-92  I.Shimizu      Add usInitFlag into SYSCONFIG
**                                      Delete uchInitSumCheck in SYSCONFIG
**                                      Add ausComBaud and auchComByteFormat
**                                      into SYSCONFIG
**            SEP-22-92  I.Shimizu      Add pointer to tally area in SYSCONFIG
**            SEP-24-92  I.Shimizu      Add pointer to RS232 log area
**                                      and POWER_UP_MENU
**            SEP-30-92  I.Shimizu      Add PIF_ERROR_MSR_POWER_FAILURE
**            OCT-06-92  I.Shimizu      Delete PIF_ERROR_COM_BUSY
**            OCT-09-92  I.Shimizu      Correct auchPFileDate in SYSCONF
**            OCT-10-92  I.Shimizu      Modify PIF_SOFT_INFO
**            OCT-10-92  I.Shimizu      Modify PIF_SOFT_INFO
**            NOV-24-92  T.KOJIMA       Add PifControlFile and
**                                      PIF_FILE_CHANGE_SIZE
**
*****************************************************************************
hhhh*/

/*******************************
 *  Define Symbolic Constants  *
 *******************************/
#ifndef PIF_INCLUDED_DEFINED
#define PIF_INCLUDED_DEFINED


#ifndef CDECL
#define CDECL _cdecl
#endif

/* data type and modifier of function entry */
#define PIFENTRY

/* data type and modifier of thread entry */
#define THREADENTRY  CDECL 

/* function return values or return data */
#define PIF_OK  1

#define PIF_ERROR_FILE_EXIST        -5
#define PIF_ERROR_FILE_EOF          -6
#define PIF_ERROR_FILE_DISK_FULL    -7
#define PIF_ERROR_FILE_NOT_FOUND    -8

#define PIF_ERROR_COM_POWER_FAILURE -2
#define PIF_ERROR_COM_TIMEOUT       -3
#define PIF_ERROR_COM_NOT_PROVIDED  -4
#define PIF_ERROR_COM_BUSY          -5
#define PIF_ERROR_COM_ABORTED       -9
#define PIF_ERROR_COM_ERRORS        -10
#define PIF_ERROR_COM_LOOT_TEST         -55
#define PIF_ERROR_COM_NO_DSR_AND_CTS    -57
#define PIF_ERROR_COM_NO_CTS            -59
#define PIF_ERROR_COM_NO_DSR            -60
#define PIF_ERROR_COM_OVERRUN           -62
#define PIF_ERROR_COM_FRAMING           -63
#define PIF_ERROR_COM_PARITY            -64
#define PIF_ERROR_COM_MONITOR           -66
#define PIF_ERROR_COM_BUFFER_OVERFLOW   -150
#define PIF_ERROR_COM_NO_INTERRUPT      -151
#define PIF_ERROR_COM_TIMEOUT_M_L       -162
#define PIF_ERROR_COM_COMM_M_L          -163

#define PIF_NET_ONLINE              1
#define PIF_NET_OFFLINE             0
#define PIF_ERROR_NET_POWER_FAILURE -2
#define PIF_ERROR_NET_TIMEOUT       -3
#define PIF_ERROR_NET_NOT_PROVIDED  -4
#define PIF_ERROR_NET_ALREADY_EXIST -5
#define PIF_ERROR_NET_EOF           -6
#define PIF_ERROR_NET_DISK_FULL     -7
#define PIF_ERROR_NET_NOT_FOUND     -8
#define PIF_ERROR_NET_ABORTED       -9
#define PIF_ERROR_NET_ACCESS_DENIED -10
#define PIF_ERROR_NET_READ          -11
#define PIF_ERROR_NET_WRITE         -12
#define PIF_ERROR_NET_OFFLINE       -13
#define PIF_ERROR_NET_UNREACHABLE   -14
#define PIF_ERROR_NET_MESSAGE       -15
#define PIF_ERROR_NET_BUSY          -16     // target busy 
#define PIF_ERROR_NET_ERRORS        -20
#define PIF_ERROR_NET_BADSIZE		-21		// indicates provided buffer is too small for packet
#define PIF_ERROR_NET_BIND_ERR      -23     // indicates error when binding to port
#define PIF_ERROR_NET_ILLEGAL		-31		// indicates generic error.

#define PIF_ERROR_NET_BAD_DATA          (-37)   /* bad data       (critical)*/
#define PIF_ERROR_NET_CRITICAL          (-38)   /* critical error (critical)*/
#define PIF_ERROR_NET_QUEUE             (-39)   /* queue error    (critical)*/
#define PIF_ERROR_NET_MEMORY            (-40)   /* memory error   (critical)*/
#define PIF_ERROR_NET_NOT_WORK          (-41)   /* not initialized(critical)*/

#define PIF_KEYERROR                0    /* indicate keyboard error */
#define PIF_KEYLOCK                 1    /* indicate first position of keylock */
#define PIF_ERROR_KEY_POWER_FAILURE 0xFF /* indicate power failure */
                                         /* These values must not be changed. */

#define PIF_DRAWER_CLOSED           0   /* This value must not be changed. */
#define PIF_DRAWER_OPENING          3
#define PIF_DRAWER_OPENED           2
#define PIF_DRAWER_NOT_PROVIDED     1

#define PIF_ERROR_WAITER_NOT_PROVIDED   -4

#define PIF_MSR_NO_DATA             0
#define PIF_ERROR_MSR_POWER_FAILURE -2
#define PIF_ERROR_MSR_NOT_PROVIDED  -4


/* used with serial communication functions */
#define PIF_PIP_GET_STATUS      1
#define PIF_PIP_LOOP_TEST       2
#define PIF_PIP_GET_CONFIG      3
#define PIF_PIP_RESET_SEQ       4

#define PIF_COM_GET_STATUS      1
#define PIF_COM_LOOP_TEST       2
#define PIF_COM_GET_RECEIVED_DATA_SIZE  3
#define PIF_COM_GET_CONFIG      4
#define PIF_COM_SET_TIME        5
#define PIF_COM_SEND_TIME       6

#define PIF_YAM_READ            1
#define PIF_YAM_WRITE           2
#define PIF_YAM_RECEIVE_TIMER   3
#define PIF_YAM_SEND_TIMER      4
#define PIF_YAM_CANCEL          5

#define PIF_COM_PROTOCOL_NON    0
#define PIF_COM_PROTOCOL_PIP    1
#define PIF_COM_PROTOCOL_YAM    2

#define PIF_FILE_CHANGE_SIZE    1


/* used with network functions */
#define PIF_NET_SET_MODE    1       /* function IDs of control function */
    #define PIF_NET_NMODE   0x01   
    #define PIF_NET_DMODE   0x02
    #define PIF_NET_TMODE   0x20
#define PIF_NET_CLEAR       2
#define PIF_NET_SET_TIME    3
#define PIF_NET_GET_STATUS  4

#define ANYPORT 0       /* indicate any port assignment */

/* used with "control keyboard, PifControlKeyboard()" */
#define PIF_KEY_ENABLE      0
#define PIF_KEY_DISABLE     1

/* used with display functions */
#define DISP_OPER   1
#define DISP_CUST   2

#define DISP_ATTR_NORMAL    0x00
#define DISP_ATTR_BLINK     0x80
#define DISP_ATTR_COMMA     0x20
#define DISP_ATTR_DP        0x40

#define DISP_DESCR_OFF      0
#define DISP_DESCR_ON       1
#define DISP_DESCR_BLINK    2

/* used with "get system configuration, PifSysConfig()" */
/* for Hardware Configuration */
#define DISP_NONE           0x00 
#define DISP_2X20           0x80                                                   
#define DISP_10N10D         0x90 
#define DISP_LCD            0xA0
#define DISP_4X20           0xB0 /* 2172 */

/*  Types of keyboards that can be used */
#define KEYBOARD_MICRO_NAME		_T("7448MICRO")
#define KEYBOARD_CONV_NAME		_T("7448CONV")
#define KEYBOARD_WEDGE_68_NAME	_T("5932_68KEY")
#define KEYBOARD_WEDGE_78_NAME	_T("5932_78KEY")
#define KEYBOARD_NONE_NAME		_T("NONE")
#define KEYBOARD_TOUCH_NAME		_T("TOUCH")

#define KEYBOARD_NONE       0x00
#define KEYBOARD_TOUCH      0x02  // Using touch screen terminal with no keyboard
#define KEYBOARD_CONVENTION 0x10  // NCR 7448 terminal with Conventional keyboard
#define KEYBOARD_MICRO      0x12  // NCR 7448 terminal with Micromotion keyboard
#define KEYBOARD_PCSTD_102  0x20  // standard PC 102 key keyboard
#define KEYBOARD_WEDGE_68   0x22  // 68 key NCR 5932 Wedge keyboard
#define KEYBOARD_WEDGE_78   0x24  // 78 key NCR 5932-7000 Wedge keyboard

#define DRAWER_NONE         0x00
#define DRAWER_PROVIDED     0xFF

#define IMSR_NONE           0x00
#define IMSR_TRACK12        0x03
#define IMSR_TRACK2         0x02
#define IMSR_TRACK2J        0x0A

#define COM_BOARD_NONE      0x00
#define COM_PROVIDED        0xFF

#define NET_BOARD_NONE      0x00
#define NET_BOARD_NCR_DLC   0xFE
#define NET_BOARD_PROVIDED  0xFF

#define WAITER_NONE         0x00
#define WAITER_LOCK         0x01
#define WAITER_PEN          0x02

#define SIZE_64KB           1

/* for System Parameters */
#define DEVICE_NONE             0
#define DEVICE_PC               1
#define DEVICE_SLIP_PRINTER     2
#define DEVICE_KITCHEN_PRINTER  3
#define DEVICE_SCANNER          4
#define DEVICE_SCALE            5
#define DEVICE_PREPAY_CARD      6
#define DEVICE_COIN_DISPENSER   7
#define DEVICE_RJ_PRINTER       99

#define COM_BYTE_ODD_PARITY     0x08
#define COM_BYTE_EVEN_PARITY    0x18
#define COM_BYTE_2_STOP_BITS    0x04
#define COM_BYTE_7_BITS_DATA    0x02
#define COM_BYTE_8_BITS_DATA    0x03

/* for Interface Parameters */
#define POWER_UP_RESET              0x01
#define POWER_UP_CLEAR_TOTAL        0x02
#define POWER_UP_CLEAR_WHOLE_MEMORY 0x04
#define POWER_UP_MENU               0x40
#define POWER_UP_CLEAR_FILE         0x80

#define LOAD_MODE_UNIQUE            0x0001
#define LOAD_MODE_GROUP             0x0002

#define LOAD_MODE_DEPT              0x0800
#define LOAD_MODE_PLU               0x1000
#define LOAD_MODE_PARA3             0x2000
#define LOAD_MODE_PARA2             0x4000
#define LOAD_MODE_PARA1             0x8000

#define IF_PARA_LOADED_SOFTWARE         0x0001
#define IF_PARA_REQUEST_TURN_OFF_POWER  0x0002
#define IF_PARA_LOAD_REQUEST_FROM_APPL  0x0004

#define IF_PARA_LOADED_DEPT             0x0800
#define IF_PARA_LOADED_PLU              0x1000
#define IF_PARA_LOADED_PARA3            0x2000
#define IF_PARA_LOADED_PARA2            0x4000
#define IF_PARA_LOADED_PARA1            0x8000


/* length */
#define PIF_LEN_THREAD_NAME     9 
#define PIF_LEN_PORT            5
#define PIF_LEN_UDATA           1400 /*should match CLI_MAX_REQDATA define*/
#define PIF_LEN_IP              4
#define PIF_LEN_OPTION          8
#define PIF_LEN_PFILE           5
#define PIF_LEN_PFILE_DATE      4
#define PIF_LEN_EXCEPTION_LOG   100
#define PIF_LEN_FAULT_LOG       10
#define PIF_LEN_KEY_TRACK       200
#define PIF_LEN_FUNC_TRACK      50
#define PIF_LEN_SOFTWARE_ID     17
#define PIF_LEN_PACKAGE_ID      17
#define PIF_LEN_TRACK1          76
#define PIF_LEN_TRACK2          37
#define PIF_LEN_TRACKJ          69


/************************
 *  Declare Data Types  *
 ************************/
/* PIFDEF data type is used by the Recovery Manager, the Portable Platform   */
/* Interface and the Simulator to access the table of pointer to a function. */
typedef VOID (PIFENTRY *PIFDEF)(VOID);

  
/************************
 *  Declare Structures  *
 ************************/
/* structures used with each functions */
typedef struct {      
    USHORT  usSec;
    USHORT  usMin;
    USHORT  usHour;
    USHORT  usMDay;
    USHORT  usMonth;
    USHORT  usYear;
    USHORT  usWDay;
} DATE_TIME;

typedef struct {
    SHORT   fPip;
    USHORT  usPipAddr;
    USHORT  usComBaud;
    UCHAR   uchComByteFormat;
    UCHAR   uchComTextFormat;
    UCHAR   auchComNonEndChar[4];
    UCHAR   auchComEndChar[3];
    UCHAR   uchComDLEChar;
} PROTOCOL;
#define LEN_COM_PARA  12  /* this is the size from usComBaud to uchComDLEChar */

typedef struct {
    UCHAR   auchFaddr[PIF_LEN_IP];
    USHORT  usFport;
    USHORT  usLport;
} XGHEADER;

typedef struct {
    UCHAR   auchFaddr[PIF_LEN_IP];
    USHORT  usFport;
    USHORT  usLport;
    UCHAR   auchData[PIF_LEN_UDATA];
} XGRAM;

typedef struct {
    UCHAR   uchASCII;
    UCHAR   uchScan;
} CHARDATA;

typedef struct {
    USHORT  usDuration;
    USHORT  usFrequency;
} STANZA;

/* structures for the logging and tracking functions */
/* The logging and tracking area are defined in the system, and each usMark, */
/* usMaxCount and usCount are initialied by the system.                      */
/* for the Exception Log */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aExceptionLog */
    USHORT  usCount;    /* current count */
    struct {            /* ring buffer (ring buffer) */   
        UCHAR   auchDateTime[7]; /* mmddyyyyhhmmss */
        USHORT  usModuleId;      /* module ID */
        USHORT  usExceptionCode; /* exception code (error code or event code) */
    } aExceptionLog[PIF_LEN_EXCEPTION_LOG];
} PIF_EXCEPTION_LOG;

/* for the Fault Log */
typedef struct {
    USHORT  usMark;                   /* 0xFFFF fixed */
    USHORT  usMaxCount;               /* max number of apRetAddr */
    UCHAR   auchThreadName
            [PIF_LEN_THREAD_NAME-1];  /* thread name */
    USHORT  usFaultModule;            /* module ID */
    UCHAR   uchFaultCode;             /* fault code */
    UCHAR   auchDateTime[7];          /* mmddyyyyhhmmss */
    USHORT  usXinuStatus;             /* XINU status coverted to "+" */
    VOID    FAR *pCurrentStack;       /* current stack pointer */
    PIFDEF  apRetAddr[PIF_LEN_FAULT_LOG]; /* return address to the callers */
} PIF_FAULT_LOG;
    
/* for the Key Track */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of auchKeyTrack */
    USHORT  usCount;    /* current count */
    UCHAR   auchKeyTrack[PIF_LEN_KEY_TRACK]; /* tracking area (ring buffer) */
} PIF_KEY_TRACK;

/* for the Function Track of the Recovery Manager */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aFuncTrack */
    USHORT  usCount;    /* current count */
    struct {            /* tracking area (ring buffer) */
        USHORT  usModuleId; /* module ID */
        PIFDEF  pRetAddr1;  /* return address of the caller */
        PIFDEF  pRetAddr2;  /* return address of the caller of the caller */
    } aFuncTrack[PIF_LEN_FUNC_TRACK];
} PIF_FUNC_TRACK;    

/* for the Error Track of the XINU status of the Platform Interface */
typedef struct {
    USHORT  usMark;     /* 0xFFFF fixed */
    USHORT  usMaxCount; /* max number of aFuncTrack */
    USHORT  usCount;    /* current count */
    struct {            /* tracking area (ring buffer) */
        USHORT  usModuleId;   /* module ID */
        USHORT  usXinuStatus; /* XINU status converted to "+" */
    } aErrorTrack[PIF_LEN_FUNC_TRACK];
} PIF_ERROR_TRACK;

/* for Resource Log of the Recovery Manager */
typedef struct {
    VOID *paXinuTable;
    VOID *paThreadTable;    
    VOID *paSemTable;
    VOID *paFileTable;
    VOID *paPortTable;
    VOID *paNetTable;
    USHORT  usMaxThread;
    USHORT  usMaxSem;
    USHORT  usMaxFile;
    USHORT  usMaxPort;
    USHORT  usMaxNet;
} PIF_RESOURCE;

/* Loaded Software Segment Information Table */
typedef struct {
    USHORT  usLoadType;                 /* 0: Unrecoverable  1: Recoverable */
    VOID    (*StartAddress)(VOID);  /* Start Address */
    USHORT  usCodeSegment;              /* The First Code Segment Adddress */
    ULONG   ulCodeSize;                 /* Total Size of Code Segments */
    USHORT  ulDataSrcSegment;           /* Initial Data to Load*/
    USHORT  usDataSize;                 /* Initialized Data Segment Size */
    USHORT  usDataBssSize;              /* Uninitialized Data Segment Size */
    USHORT  usDataTargetSegment;        /* Runtime Data Segment Address */
    USHORT  usStackSize;                /* Main Thread Stack Size */
    USHORT  usStackSegment;             /* Runtime Stack Segment Address */
    USHORT  usFarDataSrcSegment;        /* -+                   */
    USHORT  usFarDataSize;              /*  +- RAM machine only */
    USHORT  usFarDataBssSize;           /*  |                   */
    USHORT  usFarDataTargetSegment;     /* -+                   */
} PIF_SOFT_TABLE;

/* Loaded Software Information */
typedef struct {
    PIF_SOFT_TABLE *apSoftTable[4];
    USHORT  usCodeSegment;
    UCHAR   uchNoPage;
    UCHAR   auchSoftwareId[PIF_LEN_SOFTWARE_ID];
    UCHAR   auchPackageId[PIF_LEN_PACKAGE_ID];
    USHORT  usSoftwareType;
    UCHAR   auchReserved_A[25];
    USHORT  ausSoftwareCrc[8];
    UCHAR   auchReserved_B[15];
    UCHAR   uchSumCheck;    /* Sum-Check of the Loaded Software Information */
} PIF_SOFT_INFO;

/* structure of system configuration */
typedef struct {
    /* Hardware Configuration */
    UCHAR   uchOperType;
    UCHAR   uchCustType;
    UCHAR   uchKeyType;
    UCHAR   uchDrawer1;
    UCHAR   uchDrawer2;
    UCHAR   uchIntegMSRType;
    UCHAR   uchCom;
    UCHAR   uchNet;
    UCHAR   uchWaiterType;
    UCHAR   auchReserved_A[7];      /* for 16 bytes boundary */

    UCHAR   uchSRAM1Size;
    UCHAR   uchSRAM2Size;
    UCHAR   uchDRAMSize;
    UCHAR   uchROM1Size;
    UCHAR   uchROM2Size;
    UCHAR   auchReserved_B[11];     /* for 16 bytes boundary */

    /* System Parameters */
    UCHAR   auchLaddr[PIF_LEN_IP];
    UCHAR   auchComPort[PIF_LEN_PORT];
    USHORT  ausComBaud[PIF_LEN_PORT];
    UCHAR   auchComByteFormat[PIF_LEN_PORT];
    USHORT  ausOption[PIF_LEN_OPTION];
    USHORT  usSoftwareNumber;               
    USHORT  ausPFileNumber[PIF_LEN_PFILE];  /* parameter file number */
    UCHAR   auchReserved_C[11];     /* for 16 bytes boundary */
    UCHAR   uchSumCheck;            /* Sum-Check of the System Parameters */

    /* Interface Parameters */
    UCHAR   uchPowerUpMode;
    USHORT  usLoadMode;
    PIFDEF  CONST *papPifFuncTable;   /* pointer to the table of */
                                          /* pointer to a function   */
    PIF_EXCEPTION_LOG *pExceptionLog; /* pointer to the Exception Log area */
    PIF_FAULT_LOG     *pFaultLog;     /* pointer to the Fault Log area */
    PIF_KEY_TRACK     *pKeyTrack;     /* pointer to the Key Track area */
    PIF_FUNC_TRACK    *pFuncTrack;    /* pointer to the Function Track area */
    PIF_ERROR_TRACK   *pErrorTrack;   /* pointer to the Error Track area */
    PIF_RESOURCE      *pResource;     /* pointer to the Resource Log */
    PIF_SOFT_INFO     *pSoftInfo;     /* pointer to the Loeaded S/W Info */
    VOID *pTallyRJ;                   /* pointer to the R/J's tallies */
    VOID *pTallySlip;                 /* pointer to the slip's tallies */
    VOID *pTallyDLC;                  /* pointer to the NCR DLC tallies */
    VOID *pLogRS232;                  /* pointer to the RS232 log area */
    USHORT  usInterfacePara;              /* Interface Parpameter */
    UCHAR   auchPFileDate[PIF_LEN_PFILE]  /* date for parameter file */
                         [PIF_LEN_PFILE_DATE];
    USHORT  usStoreNumber;
    USHORT  usRegisterNumber;
    UCHAR   auchReserved_D[10]; /* for 16 bytes boundary */
    UCHAR   uchRAMDiskPage;
    VOID    *pRAMDiskAddr;
    USHORT  usInitFlag;
    USHORT  usRestartFlag;
} SYSCONFIG;


/* function data type of Platform Interface */
typedef USHORT (PIFENTRY *PIFDISABLE)(VOID);
typedef VOID   (PIFENTRY *PIFRESTORE)(USHORT usFlags);


/*********************************
 *  Declare Function Prototypes  *
 *********************************/
/* Entry Point of Application */
VOID   PifMain(VOID);

/* Startup of Recovery Manager */
VOID   main(VOID);

/* Function of Platform Interface Initialization */
VOID   PifXinuInit(BOOL fOsType);

/* Function of Queue Initialization */
VOID   PifInitQueue(VOID);

/* Functions of Potable Platform Interface */
VOID   PIFENTRY PifAbort(USHORT usFaultModule, USHORT usFaultCode);
USHORT PIFENTRY PifBeginThread(VOID (THREADENTRY *pStart)(VOID),
                               VOID *pStack, USHORT usStackSize,
                               USHORT usPriority,
                               CONST CHAR *pszThreadName, ...);
VOID   PIFENTRY PifEndThread(VOID);

/* the following functions not defined in the Recovery Manager */
UCHAR  CONST FAR * PIFENTRY PifGetProcessId(VOID); /* for the Recovery Manager */
USHORT PIFENTRY PifGetThreadId(VOID);     /* for the users */

USHORT PIFENTRY PifCreateSem(USHORT usCount);
VOID   PIFENTRY PifDeleteSem(USHORT usSem);
VOID   PIFENTRY PifRequestSem(USHORT usSem);
VOID   PIFENTRY PifReleaseSem(USHORT usSem);

USHORT PIFENTRY PifCreateQueue(USHORT usCount);
VOID   PIFENTRY PifDeleteQueue(USHORT usQueue);
VOID   PIFENTRY PifWriteQueue(USHORT usQueue, CONST VOID *pMessage);
VOID * PIFENTRY PifReadQueue(USHORT usQueue);

VOID   PIFENTRY PifSleep(USHORT usMsec);
VOID   PIFENTRY PifGetDateTime(DATE_TIME *pDateTime);
VOID   PIFENTRY PifSetDateTime(CONST DATE_TIME *pDateTime);

SHORT  PIFENTRY PifOpenDirFile(CONST WCHAR *pszPath,
							   CONST WCHAR *pszFileName,
                            CONST CHAR *pszMode);
SHORT  PIFENTRY PifOpenFile(CONST WCHAR *pszFileName,
                            CONST CHAR *pszMode);
USHORT PIFENTRY PifReadFile(USHORT usFile, VOID FAR *pBuffer, ULONG usBytes);
VOID   PIFENTRY PifWriteFile(USHORT usFile, CONST VOID *pBuffer,
                             ULONG usBytes);
SHORT  PIFENTRY PifSeekFile(USHORT usFile, ULONG ulPosition,
                            ULONG *pulActualPosition);
VOID   PIFENTRY PifCloseFile(USHORT usFile);
VOID   PIFENTRY PifDeleteFile(CONST WCHAR FAR *pszFileName);
SHORT  PIFENTRY PifControlFile(USHORT usPort, USHORT usFunc, ...);

SHORT  PIFENTRY PifOpenCom(USHORT usPortId, CONST PROTOCOL *pProtocol);
SHORT  PIFENTRY PifReadCom(USHORT usPort, VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifWriteCom(USHORT usPort, CONST VOID *pBuffer,
                            USHORT usBytes);
SHORT  PIFENTRY PifControlCom(USHORT usPort, USHORT usFunc, ...);
VOID   PIFENTRY PifCloseCom(USHORT usPort);

SHORT  PIFENTRY PifNetOpen(CONST XGHEADER FAR *pHeader);
SHORT  PIFENTRY PifNetOpenHost(CONST WCHAR *awzHostName, int iLocalPort, int iFarPort);
SHORT  PIFENTRY PifGetHostByName(CONST WCHAR *awzHostName, UCHAR *puchIPAddr);
SHORT  PIFENTRY PifNetSend(USHORT usNet, CONST VOID *pBuffer,
                           USHORT usBytes);
SHORT  PIFENTRY PifNetReceive(USHORT usNet, VOID *pBuffer, USHORT usBytes);
SHORT  PIFENTRY PifNetControl(USHORT usNet, USHORT usFunc, ...);
VOID   PIFENTRY PifNetClose(USHORT usNet);

VOID   PIFENTRY PifGetChar(CHARDATA *pInputChar);
VOID   PIFENTRY PifControlKeyboard(USHORT usFunc);
UCHAR  PIFENTRY PifGetKeyLock(VOID);
SHORT  PIFENTRY PifGetWaiter(VOID);

VOID   PIFENTRY PifDisplayString(USHORT usDispId, USHORT usRow,
                                 USHORT usColumn, CONST UCHAR FAR *puchString,
                                 USHORT usChars, UCHAR uchAttr);
VOID   PIFENTRY PifDisplayAttr(USHORT usDispId, USHORT usRow, USHORT usColumn,
                               CONST UCHAR FAR *puchString, USHORT usChars);
VOID   PIFENTRY PifLightDescr(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr);

USHORT PIFENTRY PifOpenDrawer(USHORT usDrawerId);
USHORT PIFENTRY PifDrawerStatus(USHORT usDrawerId);

VOID   PIFENTRY PifBeep(USHORT usMsec);
VOID   PIFENTRY PifPlayStanza(CONST STANZA FAR *pStanza);
VOID   PIFENTRY PifToneVolume(USHORT usVolume);

SYSCONFIG CONST * PIFENTRY PifSysConfig(VOID);

USHORT PIFENTRY PifDisable(VOID); /* for the Recovery Manager */
VOID   PIFENTRY PifRestore(USHORT usFlags); /* for the Recovery Manager */

VOID   PIFENTRY PifLog(USHORT usModuleId, USHORT usExceptionCode);
VOID   PIFENTRY PifTrackKey(UCHAR uchData);

VOID   PIFENTRY PifLoadFarData(USHORT usSel);
VOID   PIFENTRY PifSaveFarData(VOID);

SHORT  PIFENTRY PifControl(USHORT usFunc, VOID FAR *pArgList);

VOID   PIFENTRY PifRestart(USHORT usLoadMode);

USHORT PIFENTRY PifTurnOffPower(VOID);

SHORT  PIFENTRY PifOpenMsr(VOID);
VOID   PIFENTRY PifCloseMsr(VOID);

/* loggign function */
VOID   CDECL PifSubLogging(USHORT usFuncId);

#endif
