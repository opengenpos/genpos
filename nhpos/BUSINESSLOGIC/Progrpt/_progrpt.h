/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.1              ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Internal Header file
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGRPT.H
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date        Ver.Rev  : NAME        : Description
*   Mar-13-96 : 03.01.00 : T.Nakahata  : Initial
*   May-24-96 : 03.01.07 : T.Nakahata  : Add resetted terminal# print routine
*
** NCR2172 **
*
*   Oct-05-99 : 01.00.00 : M.Teraki    : Added #pragma(...)
*   Dec-13-99 : 01.00.00 : hrkato      : Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

/* --- internal system define --- */
#define PROGRPT_INVALID_HANDLE      -1      /* invalid handle value */
#define PROGRPT_SEM_INIT_CO         1       /* initial count for semaphore */
#define PROGRPT_BYTE_PER_BLOCK      1024UL  /* no. of bytes in a file block */

#define PROGRPT_OFFSET_FILEHEADER   0UL     /* offset for file header */


/* --- work buffer size --- */
#define PROGRPT_BACKUP_WORK_SIZE    400     /* buffer for back up function */
#define PROGRPT_NUMFORM_WORK_SIZE   16      /* buffer for numeric data format */
#define PROGRPT_DATETIME_WORK_SIZE  16      /* buffer for date/time format */
#define PROGRPT_TERMID_WORK_SIZE    16      /* buffer for terminal ID format */


/* --- data type --- */
#define PROGRPT_TYPE_STRING         0       /* string (w/o NULL) */
#define PROGRPT_TYPE_CHAR           1       /* signed char */
#define PROGRPT_TYPE_UCHAR          2       /* unsigned char */
#define PROGRPT_TYPE_SHORT          3       /* signed short int */
#define PROGRPT_TYPE_USHORT         4       /* unsigned short int */
#define PROGRPT_TYPE_LONG           5       /* singed long int */
#define PROGRPT_TYPE_ULONG          6       /* unsigned long int */
#define PROGRPT_TYPE_TOTAL          7       /* TOTAL structure */
#define PROGRPT_TYPE_LTOTAL         8       /* LTOTAL structure */
#define PROGRPT_TYPE_D13            9       /* D13DIGITS structure */
#define PROGRPT_TYPE_HUGEINT        10      /* hugeint (UCHAR 8byte) */
#define PROGRPT_TYPE_PRICE          11      /* price (UCHAR 3byte) */
#define PROGRPT_TYPE_DATE           12      /* DATE structure */
#define PROGRPT_TYPE_ADJNO          13      /* adjective no. */
#define PROGRPT_TYPE_INVALID        0xff    /* invalid data type */


#define PROGRPT_CONTINUE            1
#define PROGRPT_EOF                 2


/* --- indicate what type of loop (PLU file) --- */
#define PROGRPT_LOOP_NONE           0
#define PROGRPT_LOOP_PLU            1
#define PROGRPT_LOOP_DEPT           2
#define PROGRPT_LOOP_REPORT         3


/*
===========================================================================
    MACRO FUNCTION
===========================================================================
*/
#ifdef DEBUG
    #define ASSERT(f)   \
        if ( f )        \
            ;           \
        else            \
            PifAbort( MODULE_PROGRPT, FAULT_INVALID_DATA )
#else
    #define ASSERT(f)   ;
#endif

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    USHORT  usSeqNO;                /* Sequnece Number */
    ULONG   offulFilePosition;      /* Write and Read Position */
    UCHAR   uchStatus;              /* Status EOF, CONTINUE */
} PROGRPT_BACKUP;

typedef struct {
    ULONG  ulDataLen;              /* Data Length  */
    UCHAR   auchData[1];            /* PROGRPT File Data  */
} PROGRPT_BAKDATA;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
===========================================================================
    EXTERNAL VARIABLE
===========================================================================
*/
extern USHORT   husProgRptSem;
extern TCHAR    auchProgRptBuff[ PROGRPT_MAX_COLUMN + 1 ];
extern UCHAR    uchProgRptColumn;
extern UCHAR    uchProgRptRow;

extern BOOL     fProgRptWithEJ;
extern BOOL     fProgRptEmptyLoop;

extern UCHAR    uchRptOpeCount;
extern USHORT   usRptPrintStatus;
extern UCHAR    uchRptMldAbortStatus;


/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*-----------------------------------
    PROGRPT FILE I/F (_PROGFIL.C)
-----------------------------------*/

VOID  ProgRpt_InitFile( VOID );
SHORT ProgRpt_NewCreate( VOID );
SHORT ProgRpt_OldOpen( VOID );
VOID  ProgRpt_Close( VOID );
VOID  ProgRpt_Delete( VOID );
SHORT ProgRpt_ExpandFile( UCHAR uchNoOfBlock );

UCHAR ProgRpt_LoadNoOfBlock( VOID );

BOOL ProgRpt_InitFileHeader( UCHAR uchNoOfBlock );
BOOL ProgRpt_GetFileHeader( PROGRPT_FHEAD *pFHeader );

BOOL ProgRpt_ReadFile( ULONG  ulReadPosition,
                       VOID   *pvBuffer,
                       USHORT usNoOfBytesToRead,
                       ULONG *pulActualRead );
BOOL ProgRpt_WriteFile( ULONG  ulWritePosition,
                        VOID   *pvBuffer,
                        ULONG  ulNoOfBytesToWrite );


/*-----------------------------------
    PROGRPT COMMON (_PROGCMN.C)
-----------------------------------*/

SHORT ProgRpt_GetProgRptInfo( UCHAR uchProgRptNo, PROGRPT_RPTHEAD *pHeader );
SHORT ProgRpt_GetEODPTDInfo( USHORT           usReportType,
                             USHORT           *pusNoOfRpt,
                             PROGRPT_EODPTDIF *paEODPTDRec );

SHORT ProgRpt_PrintAndFeed( PROGRPT_RPTHEAD *pRptHead, VOID *puchBuffer );
VOID  ProgRpt_PrintHeader( UCHAR uchMinorClass,
                           UCHAR uchProgRptNo,
                           UCHAR uchReportName,
                           UCHAR uchResetType );
VOID  ProgRpt_Feed( USHORT usDevice, UCHAR uchNoOfFeed );
BOOL  ProgRpt_Print( PROGRPT_RPTHEAD *pRptHead );
VOID  ProgRpt_PrintLoopBegin( PROGRPT_RPTHEAD *pRptHead,
                              PROGRPT_LOOPREC *pLoopRec );
VOID  ProgRpt_PrintEOF( PROGRPT_RPTHEAD *pRptHead,
                        PROGRPT_PRINTREC *pPrintRec );

VOID  ProgRpt_InitAccumulator( VOID );
VOID  ProgRpt_GetAccumulator( PROGRPT_ITEM *pTargetItem, D13DIGITS *phintOutput );
VOID  ProgRpt_SetAccumulator( PROGRPT_ITEM *pTargetItem, D13DIGITS *phintNewValue );
VOID ProgRpt_CalcAccumulator( UCHAR uchCondition,
                              D13DIGITS *phugeAnswer,
                              D13DIGITS *phugeValue1,
                              D13DIGITS *phugeValue2 );
VOID  ProgRpt_GetMnemonic( PROGRPT_ITEM *pTargetItem,
                           TCHAR *pszString,
                           UCHAR uchItemKeyNo );
VOID  ProgRpt_IntegerToString( PROGRPT_PRINTREC *pPrintRec,
                               D13DIGITS        *pD13Digit,
                               TCHAR            *pszDestBuff );
VOID  ProgRpt_SetDateTimeBuff( PROGRPT_PRINTREC *pPrintRec, N_DATE *pDateTime );
BOOL  ProgRpt_GetCurDateTime( N_DATE *pDateTime );
VOID  ProgRpt_FormatPrintBuff( PROGRPT_PRINTREC *pPrintRec,
                               TCHAR *pszSource,
                               UCHAR uchSrcStrLen,
                               UCHAR uchDataType );
BOOL ProgRpt_IsPrintCondTRUE( UCHAR uchCondition,
                              D13DIGITS *pD13Value1,
                              D13DIGITS *pD13Value2 );
SHORT ProgRpt_ConvertError( SHORT sRetCode );
BOOL ProgRpt_PluIsAll( TCHAR *puchPluNo );          /* Saratoga */


/*-----------------------------------
    PROGRPT ISSUE DEPT (_PROGDPT.C)
-----------------------------------*/

SHORT ProgRpt_DeptPrint( UCHAR  uchProgRptNo,
                         UCHAR  uchMinorClass,
                         USHORT usDeptNo,           /* Saratoga */
                         UCHAR  uchMjrDeptNo );
SHORT ProgRpt_DeptReset( UCHAR uchMinorClass );
BOOL ProgRpt_DeptIsResettable( UCHAR uchMinorClass );
BOOL ProgRpt_DeptIsBadLoopKey( SHORT *psLDTCode,
                               UCHAR uchProgRptNo,
                              USHORT usDeptNo,      /* Saratoga */
                               UCHAR uchMjrDeptNo );
SHORT ProgRpt_DeptLOOP( PROGRPT_RPTHEAD * pReportInfo,
                        PROGRPT_LOOPREC * pLoopRec,
                        UCHAR uchMinorClass,
                        USHORT usDeptNo,            /* Saratoga */
                        UCHAR uchMjrDeptNo,
                        BOOL    fByMjrDept,
                        ULONG   ulBeginOffset );
SHORT ProgRpt_DeptPRINT( PROGRPT_RPTHEAD * pReportInfo,
                         PROGRPT_PRINTREC *pPrintRec,
                         UCHAR uchMinorClass,
                         OPDEPT_PARAENTRY *pDeptPara,           /* Saratoga */
                         TTLDEPT *pTtlDept,
                         UCHAR uchLoopMjrNo );
SHORT ProgRpt_DeptMATH( PROGRPT_MATHOPE *pMathRec, TTLDEPT *pTtlDept );

SHORT ProgRpt_DeptFindDept( MDEPTIF *pMjrDeptIF,
                            TTLDEPT *pTtlDept,
                            UCHAR   uchMinorClass,
                            BOOL    fByMjrDept );
BOOL  ProgRpt_DeptIsLoopTRUE( PROGRPT_LOOPREC * pLoopRec, OPDEPT_PARAENTRY *pParaDept );    /* Saratoga */
BOOL  ProgRpt_DeptIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
                               TTLDEPT *pTtlDept );
UCHAR ProgRpt_DeptQueryDataType( PROGRPT_ITEM *pTargetItem );
BOOL  ProgRpt_DeptGetInteger( PROGRPT_ITEM *pTargetItem,
                              TTLDEPT      *pTtlDept,
                              D13DIGITS    *hintOutput );
BOOL  ProgRpt_DeptGetDateTime( PROGRPT_ITEM *pTargetItem,
                               UCHAR uchMinorClass,
                               TTLDEPT      *pTtlDept,
                               N_DATE         *pDate );
BOOL  ProgRpt_DeptGetString( PROGRPT_ITEM *pTargetItem,
                             OPDEPT_PARAENTRY     *pDeptPara,   /* Saratoga */
                             TCHAR        *pszString,
                             UCHAR        uchMjrDeptNo );
BOOL ProgRpt_DeptPrintRootPara( PROGRPT_PRINTREC *pPrintRec, UCHAR   uchLoopMjrNo );


/*-----------------------------------
    PROGRPT ISSUE PLU (_PROGPLU.C)
-----------------------------------*/

SHORT ProgRpt_PluPrint( UCHAR uchProgRptNo, UCHAR uchMinorClass,
                        TCHAR *puchPluNo,            /* Saratoga */
                        USHORT usDeptNo,            /* Saratoga */
                        UCHAR uchReportCode );
SHORT ProgRpt_PluReset( UCHAR uchMinorClass );
BOOL ProgRpt_PluIsResettable( UCHAR uchMinorClass );
BOOL ProgRpt_PluIsBadLoopKey( SHORT *psLDTCode,
                              UCHAR uchProgRptNo,
                              TCHAR *puchPluNo,     /* Saratoga */
                              USHORT usDeptNo,      /* Saratoga */
                              UCHAR uchReportCode );

SHORT ProgRpt_PluLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR   uchMinorClass,
                       TCHAR   *puchPluNo,          /* Saratoga */
                       USHORT  usDeptNo,            /* Saratoga */
                       UCHAR   uchReportCode,
                       SHORT   sLoopType,
                       ULONG   ulBeginOffset );
SHORT ProgRpt_PluPRINT( PROGRPT_RPTHEAD  *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR uchMinorClass,
                        PLUIF   *pPluIf,
                        TTLPLU  *pTtlPlu,
                        USHORT  usLoopDeptNo,
                        UCHAR   uchLoopReportNo );
SHORT ProgRpt_PluMATH( PROGRPT_MATHOPE *pMathRec, PLUIF  *pPluIf, TTLPLU *pTtlPlu );

SHORT ProgRpt_PluFindPlu( PLUIF     *pPluIf,
                          TTLPLU    *pTtlPlu,
                          UCHAR     uchMinorClass,
                          SHORT     sLoopType,
                          USHORT    usFindKeyNo,
                          TCHAR     *puchPluNo );           /* Saratoga */
BOOL ProgRpt_PluIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
                            PLUIF   *pPluIf );
BOOL ProgRpt_PluIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec, PLUIF  *pPluIF, TTLPLU *pTtlPlu );
UCHAR ProgRpt_PluQueryDataType( PROGRPT_ITEM *pTargetItem );
BOOL ProgRpt_PluGetInteger( PROGRPT_ITEM *pTargetItem,
                            PLUIF        *pPluIf,
                            TTLPLU       *pTtlPlu,
                            D13DIGITS    *phugeOutput );
BOOL ProgRpt_PluGetDateTime( PROGRPT_ITEM *pTargetItem,
                             UCHAR uchMinorClass,
                             TTLPLU       *pTtlPlu,
                             N_DATE         *pDateTime );
BOOL ProgRpt_PluGetString( PROGRPT_ITEM *pTargetItem,
                           PLUIF        *pPluIf,
                           USHORT       usDeptNo,           /* Saratoga */
                           TCHAR        *pszString );
BOOL ProgRpt_PluGetPrice( PLUIF   *pPluIf, D13DIGITS  *pulPrice );
BOOL ProgRpt_PluGetAdjNo( PROGRPT_PRINTREC *pPrintRec, PLUIF *pPluIf, TCHAR *pszString );
BOOL ProgRpt_PluPrintRootPara( PROGRPT_PRINTREC *pPrintRec, USHORT  usLoopDeptNo, UCHAR   uchLoopReportNo );

SHORT   ProgRpt_PluLabelAnalysis (PARAPLU *ParaPlu);        /* saratoga */

/*-----------------------------------
    PROGRPT ISSUE COUPON (_PROGCPN.C)
-----------------------------------*/

SHORT ProgRpt_CpnPrint( UCHAR  uchProgRptNo, UCHAR  uchMinorClass, USHORT usCouponNo );
SHORT ProgRpt_CpnReset( UCHAR  uchMinorClass );
BOOL ProgRpt_CpnIsResettable( UCHAR  uchMinorClass );
BOOL ProgRpt_CpnIsBadLoopKey( SHORT  *psLDTCode, UCHAR  uchProgRptNo, USHORT usCouponNo );

SHORT ProgRpt_CpnLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR    uchMinorClass,
                       USHORT   usCouponNo,
                       ULONG    ulBeginOffset );
SHORT ProgRpt_CpnPRINT( PROGRPT_RPTHEAD *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR    uchMinorClass,
                        CPNIF  *pCpnIF,
                        TTLCPN *pTtlCpn );
SHORT ProgRpt_CpnMATH( PROGRPT_MATHOPE *pMathRec, CPNIF  *pCpnIF, TTLCPN *pTtlCpn );


SHORT ProgRpt_CpnFindCoupon( CPNIF     *pCpnIF, TTLCPN *pTtlCpn, UCHAR     uchMinorClass );
BOOL ProgRpt_CpnIsPrintTRUE( PROGRPT_PRINTREC *pPrintRec, CPNIF  *pCpnIF, TTLCPN *pTtlCpn );
UCHAR ProgRpt_CpnQueryDataType( PROGRPT_ITEM *pTargetItem );
BOOL ProgRpt_CpnGetInteger( PROGRPT_ITEM *pTargetItem, CPNIF *pCpnIF, TTLCPN  *pTtlCpn, D13DIGITS *phugeOutput );
BOOL ProgRpt_CpnGetDateTime( PROGRPT_ITEM *pTargetItem, UCHAR  uchMinorClass, TTLCPN *pTtlCpn, N_DATE   *pDateTime );
BOOL ProgRpt_CpnGetString( PROGRPT_ITEM *pTargetItem, CPNIF *pCpnIF, TCHAR        *pszString );

/*-----------------------------------
    PROGRPT ISSUE CASHIER (_PROGCAS.C)
-----------------------------------*/

SHORT ProgRpt_CasPrint( UCHAR  uchProgRptNo, UCHAR  uchMinorClass, ULONG  ulCashierNo );
SHORT ProgRpt_CasReset( UCHAR uchMinorClass, ULONG ulCashierNo );
BOOL  ProgRpt_CasIsResettable( UCHAR uchMinorClass, ULONG ulCashierNo );
BOOL  ProgRpt_CasIsBadLoopKey( SHORT *psLDTCode, UCHAR uchProgRptNo, ULONG ulCashierNo );
SHORT ProgRpt_CasPrintResetCasID( UCHAR uchProgRptNo, ULONG ulCashierNo );


SHORT ProgRpt_CasLOOP( PROGRPT_RPTHEAD * pReportInfo, PROGRPT_LOOPREC * pLoopRec, UCHAR  uchMinorClass, ULONG  ulCashierNo, ULONG  ulBeginOffset );
SHORT ProgRpt_CasPRINT( PROGRPT_RPTHEAD * pReportInfo, PROGRPT_PRINTREC *pPrintRec, UCHAR  uchMinorClass, CASIF *pCasPara, TTLCASHIER *pTtlCas );
SHORT ProgRpt_CasMATH( PROGRPT_MATHOPE *pMathRec, TTLCASHIER *pTtlCas );

SHORT ProgRpt_CasFindCashier( CASIF *pCasIF, TTLCASHIER *pTtlCas, UCHAR uchMinorClass );
BOOL  ProgRpt_CasIsLoopTRUE( PROGRPT_LOOPREC * pLoopRec, CASIF *pCasIF );
BOOL  ProgRpt_CasIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec, TTLCASHIER *pTtlCas );

UCHAR ProgRpt_CasQueryDataType( PROGRPT_ITEM *pTargetItem );
BOOL  ProgRpt_CasGetInteger( PROGRPT_ITEM *pTargetItem, TTLCASHIER   *pTtlCas, D13DIGITS *phugeOutput );
BOOL  ProgRpt_CasGetDateTime( PROGRPT_ITEM *pTargetItem, UCHAR uchMinorClass, TTLCASHIER *pTtlCas, N_DATE  *pDate );
BOOL  ProgRpt_CasGetString( PROGRPT_ITEM *pTargetItem, CASIF *pCasPara, TCHAR *pszString );

/*-----------------------------------
    PROGRPT ISSUE FINANCIAL (_PROGFIN.C)
-----------------------------------*/
SHORT ProgRpt_FinPrint( UCHAR  uchProgRptNo, UCHAR  uchMinorClass );
SHORT ProgRpt_FinReset( UCHAR uchMinorClass );
BOOL  ProgRpt_FinIsResettable( UCHAR uchMinorClass );

SHORT ProgRpt_FinPRINT( PROGRPT_RPTHEAD *pReportInfo, PROGRPT_PRINTREC *pPrintRec, UCHAR uchMinorClass, TTLREGFIN *pTtlInd );
SHORT ProgRpt_FinMATH( PROGRPT_MATHOPE *pMathRec, TTLREGFIN *pTtlInd );

BOOL ProgRpt_FinIsPrintTRUE( PROGRPT_PRINTREC *pPrintRec, TTLREGFIN *pTtlInd );
UCHAR ProgRpt_FinQueryDataType( PROGRPT_ITEM *pTargetItem );
BOOL ProgRpt_FinGetInteger( PROGRPT_ITEM *pTargetItem, TTLREGFIN   *pTtlInd, D13DIGITS *phugeOutput );
BOOL ProgRpt_FinGetDateTime( PROGRPT_ITEM *pTargetItem, UCHAR uchMinorClass, TTLREGFIN *pTtlInd, N_DATE *pDateTime );
BOOL ProgRpt_FinGetString( PROGRPT_ITEM *pTargetItem, TCHAR *pszString );

/*-----------------------------------
    PROGRPT ISSUE IND-FIN (_PROGIND.C)
-----------------------------------*/
SHORT ProgRpt_IndPrint( UCHAR  uchProgRptNo, UCHAR  uchMinorClass, USHORT usTerminalNo );
SHORT ProgRpt_IndReset( UCHAR uchReportType, USHORT usTerminalNo );
BOOL  ProgRpt_IndIsResettable( USHORT usTerminalNo );
BOOL  ProgRpt_IndIsBadLoopKey( SHORT *psLDTCode, UCHAR uchProgRptNo, USHORT usTerminalNo );
SHORT ProgRpt_IndPrintResetTermID( UCHAR uchProgRptNo, USHORT usTerminalNo );

SHORT ProgRpt_IndLOOP( PROGRPT_RPTHEAD * pReportInfo, PROGRPT_LOOPREC * pLoopRec, UCHAR  uchMinorClass, USHORT usTerminalNo, ULONG  ulBeginOffset );

SHORT ProgRpt_IndFindTerminal( USHORT usTerminalNo, TTLREGFIN *pTtlInd, UCHAR uchMinorClass );

/*-------------------------------------------------------------------
    PROGRPT ISSUE HOURLY (_PROGHUR.C) - do not support at R3.1
-------------------------------------------------------------------*/
SHORT ProgRpt_HourReset( UCHAR uchMinorClass );
BOOL  ProgRpt_HourIsResettable( UCHAR uchMinorClass );


/*-------------------------------------------------------------------
    PROGRPT ISSUE SERVICE TIME (_PROGTIM.C) - do not support at R3.1
-------------------------------------------------------------------*/
SHORT ProgRpt_TimeReset( UCHAR uchMinorClass );
BOOL  ProgRpt_TimeIsResettable( UCHAR uchMinorClass );


/*-----------------------------------
    PROGRPT ISSUE EOD (_PROGEOD.C)
-----------------------------------*/
SHORT ProgRpt_EODPrint( UCHAR     uchReportType, PROGRPT_EODPTDIF *paEODRec, USHORT    usNoOfRpt );
VOID  ProgRpt_EODReset( UCHAR     uchReportType, PROGRPT_EODPTDIF *paEODRec, USHORT    usNoOfRpt );
BOOL  ProgRpt_EODIsResettable( PROGRPT_EODPTDIF *paEODRec, USHORT    usNoOfRpt );


/*-----------------------------------
    PROGRPT ISSUE PTD (_PROGPTD.C)
-----------------------------------*/
SHORT ProgRpt_PTDPrint( UCHAR   uchReportType, PROGRPT_EODPTDIF *paPTDRec, USHORT  usNoOfRpt );
VOID  ProgRpt_PTDReset( UCHAR   uchReportType, PROGRPT_EODPTDIF *paPTDRec, USHORT  usNoOfRpt );
BOOL  ProgRpt_PTDIsResettable( PROGRPT_EODPTDIF *paPTDRec, USHORT    usNoOfRpt );

/* ===== End of File (_PROGRPT.H) ===== */