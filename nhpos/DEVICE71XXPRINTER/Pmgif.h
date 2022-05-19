/**
*===========================================================================
* Update Histories
*
* Data       Ver.      Name         Description
* Aug-15-95  G1        O.Nakada     Deleted TYPE_M825_M820, TYPE_VALIDATION
*                                   and TYPE_CUTTER.
*
*** NCR2172 ***
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma pack(...)
*===========================================================================
**/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/

/*************************************/
/**         macros                  **/
/*************************************/
#define OFFSET(STRUCTURE, MEMBER)		&(((STRUCTURE *)NULL)->MEMBER)

/*************************************/
/**         common equ              **/
/*************************************/
#define PMG_TRUE        0               /* true                             */
#define PMG_FALSE       1               /* false                            */
#define PMG_IGNORE      2               /* ignore                           */

#define T_ADDR_RJ       61              /* terget address of R/J            */
#define T_ADDR_S        62              /* terget address of slip           */
#define MAX_PRINT_CHAR  80              /* maximum character of 1 line      */
#ifdef TPOS
#define MAX_PRT_COMM    (120-8)         /* maximum length of communication  */
#else
#define MAX_PRT_COMM    (60-8)          /* maximum length of communication  */
#endif
                                        /* printer selection&same data print*/
#define SIZE_SPOOL_R    1024            /* size of receipt buffer           */
#define SIZE_SPOOL_J    512             /* size of journal buffer           */
#define SIZE_SPOOL_S    512             /* size of slip buffer              */
#define SIZE_STACK_SEND 1024            /* stack for send thread            */
#define SIZE_STACK_RCV  1024            /* stack for receive thread         */

#define PMG_SEL_JOURNAL     0x01        /* journal print selection          */
#define PMG_SEL_RECEIPT     0x02        /* receipt print selection          */
#define PMG_SEL_VALIDATION  0x04        /* validation print selection       */
#define PMG_SEL_SLIP        0x04        /* slip print selection             */
#define PMG_SET_FONT_B      0x01        /* set font B (EPSON thermal printer)*/
#if defined (DEVICE_7158) || defined (DEVICE_7194) || defined (DEVICE_7196)
#define PMG_SET_DEF_LF      54          /* set feed length for default      */
#define PMG_SET_DEF_LF_SLIP 21          /* set feed length for default      */
#define PMG_SET_LF          30          /* set feed length for font B       */
#elif defined (DEVICE_7161)
#define PMG_SET_DEF_LF      24          /* set feed length for default      */
#define PMG_SET_LF          24          /* set feed length for font B       */
#else
#define PMG_SET_DEF_LF      60          /* set feed length for default      */
#define PMG_SET_LF          30          /* set feed length for font B       */
#endif

#define PMG_CHARX2          0x12        /* double width                     */
#define PMG_CHARX4          0x13        /* double width * double hight      */
#define PMG_ESC_CHARX2      0x20        /* double width                     */
#define PMG_ESC_CHARX4      0x30        /* double width * double hight      */

/*****************************************/
/**      serial port configration       **/
/*****************************************/
typedef struct {
	USHORT  usPip;
    UCHAR   uchComPort;
    ULONG   ulComBaud;
    UCHAR   uchComByteFormat;
} SERIAL_CONFIG, *PSERIAL_CONFIG;
    

/*****************************************/
/**         printer configration        **/
/*****************************************/
typedef struct {
    UCHAR       auchIDENT[4];
    UCHAR       ucConfig;               /* printer configration             */
    UCHAR       aucData[31];
} PRTCONFIG;
    

/*****************************************/
/**         printer control             **/
/*****************************************/
typedef struct  {
    SHORT       sPortNo;                /* port number                      */
    SHORT       hPort;                  /* handel of communication port     */
    UCHAR       ucSysPara;              /* system parameters                */
    UCHAR       ucInputSeqNo;           /* sequence numberc of input        */
    UCHAR       ucStatPrt;              /* status of printer                */
    UCHAR       ucSndSeqNo;             /* sequence number of send          */
    UCHAR       ucRcvSeqNo;             /* sequence number of receive       */
    UCHAR       ucStatSend;             /* status of send thread            */
    UCHAR       ucStatReceive;          /* status of receive thread         */
    CHAR        *pchEscString;          /* recovery data                    */
    SHORT       sEscLen;                /* recovery data length             */
    CHAR        *szEscFile;             /* recovery file name               */
    PRTTALLY FAR *pPrtTally;            /* printer tallies                  */
	BOOL		bDoSend;
	BOOL		bDoReceive;
} PRTCTRL;

/***    ucStatSend          ***/
/***    ucStatReceive       ***/
#define SPLST_ERROR         0x01        /* error detected                   */
#define SPLST_2BUFF         0x02        /* 2 buffer full                    */
#define SPLST_FINALIZED     0x04        /* finalized                        */


/*****************************************/
/**         spool buffer                **/
/*****************************************/
typedef struct {
    UCHAR       *pucStart;              /* spool buffer start pointer       */
    UCHAR       *pucEnd;                /* spool buffer end pointer         */
    UCHAR       *pucWrite;              /* next write data pointer          */
    UCHAR       *pucSend;               /* next send data pointer           */
    UCHAR       *pucReceive;            /* next receive data pointer        */
    UCHAR       *pucValidation;         /* start pointer of validation      */
    UCHAR       fbConfig;               /* printer configration             */
    UCHAR       ucMaxChar;              /* character / line                 */
    UCHAR       ucValidChar;            /* character / line validation      */
    UCHAR       fbModeInput;            /* mode of input                    */
    UCHAR       fbModeSpool;            /* mode of spool buffer             */
    UCHAR       ucEndCunt;              /* counter of near end              */
} SPOOLCNT, *PSPOOLCNT;

/***    fbModeInput         ***/
/***    fbModeSpool         ***/
#define SPLMD_VALIDATION    0x01        /* mode in validation           */
#define SPLMD_IMPORTANT     0x02        /* mode in important lines      */
#define SPLMD_IMP_ERROR     0x04        /* mode in important error      */
#define SPLMD_FONT_B        0x08        /* font type (0:A,1:B)          */
#define SPLMD_RCT_JNL       0x10        /* receipt & journal print      */
#define SPLMD_SMALL_VALIDATION 0x20     /* small validation for US Customs */
#define SPLMD_WAIT_EMPTY    0x80        /* wait for empty of spool      */


/*****************************************/
/**         spool data structure        **/
/*****************************************/
typedef struct {
    UCHAR   ucVLI;                      /* VLI of print data            */
    UCHAR   ucDataID;                   /* data ID                      */
    UCHAR   ucInputSeqNo;               /* sequence number of input     */
    UCHAR   fbPrtMode;                  /* mode of printer              */
    UCHAR   ucCommSeqNo;                /* sequence number of comm.     */
} DATACTRL;

typedef struct {
    DATACTRL    aDataCtrl;                  /* data control block       */
    UCHAR       ucPrtData[MAX_PRINT_CHAR];  /* print data               */
} SPOOLDAT;

/***    ucDataID        ***/
#define SPBID_PRINT_DATA_C      0       /* print data                   */
#define SPBID_PRINT_DATA_E      1       /* print data                   */
#define SPBID_PRINT_DATA_RJ_C   2       /* print data, continue         */
#define SPBID_PRINT_DATA_RJ_E   3       /* print data, end              */
#define SPBID_BEGIN_VALIDATION  4       /* begin of validation          */
#define SPBID_END_VALIDATION    5       /* end of validation            */
#define SPBID_BEGIN_IMPORTANT   6       /* begin of important lines     */
#define SPBID_END_IMPORTANT     7       /* end of important lines       */
#define SPBID_FONT_A            8       /* set to font A                */
#define SPBID_FONT_B            9       /* set to font B                */
#define SPBID_END_SPOOL         0xfe    /* end of spool (next to  top)  */
#define SPBID_END_DATA          0xff    /* end of data                  */

/***    fbPrtMode       ***/
#define SPBST_VALIDATION        0x01    /* mode in validation           */
#define SPBST_IMPORTANT         0x02    /* mode in important lines      */
#define SPBST_SKIP              0x10    /* skip the data for error      */
#define SPBST_ERROR             0x20    /* error occured (need reset)   */
#define SPBST_POWERFAIL         0x40    /* power fail occured           */
#define SPBST_EXECUTED          0x80    /* function executed            */


/*************************************/
/**     printer send data           **/
/*************************************/
typedef struct {
    UCHAR       auchIDENT[3];               /* IDENT (not used)             */
    UCHAR       ucSeqNo;                    /* print data sequence number   */
    UCHAR       aucPrtMsg[MAX_PRINT_CHAR];  /* print data                   */
} PRT_SNDMSG;


/*****************************************/
/**     printer responce data           **/
/*****************************************/
typedef struct {
    UCHAR       ucIDENT[3];             /* IDENT (not used)                 */
    UCHAR       ucSeqNo;                /* print data sequence number       */
    UCHAR       ucPrtStatus;            /* printer status                   */
    UCHAR       ucPrtError;             /* printer error code               */
    UCHAR       ucPrtAddr;              /* device address (not used)        */
} PRT_RESPONCE;

/***    ucPrtStatus     ***/
#define PRT_STAT_R_NEAR_END 0x01        /*   receipt paper near end         */
#define PRT_STAT_ERROR      0x04        /* 1:not error                      */
#define PRT_STAT_FORM_IN    0x08        /*   form in                        */
#define PRT_STAT_J_NEAR_END 0x10        /*   journal paper near end         */
#define PRT_STAT_INIT_END   0x20        /* 1:initialize cmplete             */

/***    ucPrtError      ***/
#define PRT_ERR_HOME        1           /* can't move home position         */
#define PRT_ERR_TIMING      2           /* timing palse error               */
#define PRT_ERR_JAM         3           /* motor jam error                  */
#define PRT_ERR_VALIDATION  4           /* validation error                 */
#define PRT_ERR_VOLTAGE     5           /* voltage error                    */


/*****************************************/
/**     semaphore control data          **/
/*****************************************/
typedef struct {
    USHORT  hSemaphore;                 /* semaphore handle                 */
    UCHAR   fbWaitFlag;                 /* wait flag                        */
    CHAR    chCount;                    /* wait conter                      */
} PMGSEM;

/***    user ID     ***/
#define ID_SEND         1               /* send thread                      */
#define ID_RECEIVE      2               /* receive thread                   */
#define ID_WAIT_RJ      3               /* wait for RJ spool                */
#define ID_WAIT_S       4               /* wait for slip spool              */
#define ID_MAX          ID_WAIT_S       /* number of sleep                  */

/***    fsWaitFlag      ***/
#define WAIT_BUFF       0x01            /* wait buffer                      */
#define WAIT_RCT_BUFF   0x02            /* wait buffer                      */
#define WAIT_JNL_BUFF   0x04            /* wait buffer                      */
#define WAIT_DATA       0x08            /* wait data input                  */
#define WAIT_FUNCTION   0x10            /* wait function compleate          */
#define WAIT_RECEIVE    0x20            /* wait receive compleate           */
#define WAIT_ERROR      0x40            /* wait error recovery              */
#define WAIT_WAKEUP     0x80            /* wake up occured                  */


/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/
/***
;       I N T E R F A C E   M O D U L E
***/
VOID    PmgInsSpoolID( USHORT , UCHAR );
VOID    PmgSetSpoolID( USHORT , PSPOOLCNT , UCHAR , UCHAR );
VOID    PmgInsSpoolData( USHORT , UCHAR *, USHORT , BOOL);
VOID    PmgSetSpoolData( USHORT , PSPOOLCNT , UCHAR *, USHORT , UCHAR , BOOL);
USHORT  PmgChkConfig( USHORT );
SHORT   PmgOpenCom( USHORT );
USHORT  PmgFeed2( USHORT, USHORT , UCHAR *);
USHORT PmgReadStatus(USHORT usPrtType, UCHAR *pfbStatus);
#ifdef TPOS
USHORT  PmgKanjiAdjust( UCHAR *, UCHAR *, SHORT );
VOID    PmgSjisToJis( UCHAR *, UCHAR *);
#endif

/***
;       S E N D   T H R E A D
***/
VOID    THREADENTRY PmgSend( VOID );


/***
;       R E C E I V E   T H R E A D
***/
VOID THREADENTRY PmgReceive(VOID);    /* receive thread for R&J       */

USHORT GetSpool(USHORT, VOID *);        /* get pointer to spool         */
#define SPL_DATA        1               /** print data                  **/
#define SPL_SPC_FNC     2               /** special function            **/ 
#define SPL_NO_DATA     3               /** no print data               **/
#define SPL_ERR_RCV     4               /** error in receive thread     **/
#define SPL_ERR_IMP     5               /** error in important lines    **/
#define SPL_ERROR       6               /** error occured               **/
#define SPL_POWER_FAIL  7               /** power fail occured          **/


/***
;       C O M M O N   R O U T I N E
***/
VOID    PmgCreateSem( VOID );
VOID    PmgCreateWait( VOID );
VOID    PmgRequestSem( USHORT );
VOID    PmgReleaseSem( USHORT );
VOID    PmgRequestSemRJ( VOID );
VOID    PmgReleaseSemRJ( VOID );
VOID    PmgRequestSemS( VOID );
VOID    PmgReleaseSemS( VOID );
VOID    PmgRequestWaitSem( VOID );
VOID    PmgReleaseWaitSem( VOID );
VOID    PmgRequestImpSemR( VOID );
VOID    PmgReleaseImpSemR( VOID );
VOID    PmgRequestImpSemJ( VOID );
VOID    PmgReleaseImpSemJ( VOID );
VOID    PmgRequestImpSemS( VOID );
VOID    PmgReleaseImpSemS( VOID );
VOID    PmgSleep( USHORT, UCHAR, USHORT );
VOID    PmgWakeUp( USHORT );
USHORT  PmgChkSleep( USHORT, UCHAR );
VOID    PmgSetCurStat( USHORT, UCHAR );
VOID    PmgResetCurStat( USHORT, UCHAR );
UCHAR   PmgGetCurStat( USHORT );
UCHAR   PmgChkCurStat( USHORT , UCHAR );
VOID    PmgSetInputStat( USHORT, UCHAR );
VOID    PmgResetInputStat( USHORT, UCHAR );
UCHAR   PmgGetInputStat( USHORT );
SHORT   PmgCmpSequence( UCHAR, UCHAR );
VOID    PmgGetCtrl( USHORT, PRTCTRL **, SPOOLCNT **);
SPOOLCNT *PmgGetSpoolCtrl(USHORT usPrtType);
VOID    PmgStopSend(USHORT);
VOID    PmgRestartSend( USHORT );
VOID    PmgMemoryEsc(USHORT , UCHAR * , USHORT );
VOID    PmgReOpenPrt( USHORT );
USHORT  PmgResetPrt( USHORT );
USHORT  PmgSendInitData( USHORT );
SHORT   PmgWriteCom(USHORT, CONST VOID  *, USHORT);
SHORT   PmgReadCom(USHORT, VOID  *, USHORT);
USHORT  PmgAsyncControl(USHORT usPrtType);

VOID PmgWaitRJS( VOID );
VOID PmgWaitRJ( VOID );
VOID PmgWaitS( VOID );
VOID PmgErrorMsg(USHORT usErrorCode);
BOOL PmgGetSerialConfig(PSERIAL_CONFIG pSerialConf);

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/***    End of PMGIF.H    ***/
