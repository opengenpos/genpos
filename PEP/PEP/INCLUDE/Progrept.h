/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.3              ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Header file
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : PROGREPT.H
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            : /AM /W4 /G1s /Za /Zp /Os /Zi
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date        Ver.Rev  : NAME        : Description
*   Mar-01-96 : 00.00.01 : T.Nakahata  : Initial
*   Sep-03-98 : 03.03.00 : A.Mitsui    : V3.3 Initial
*   Jan-24-00 : 01.00.00 : K.Yanagida  : Saratoga Initial
*
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

/* --- programmable report version --- */
/*#define PROGRPT_VERSION         0x0330  * 2170 HPUS Release 3.3 */
#define PROGRPT_VERSION         0x0100  /* Saratoga Release 1.0 */


/* --- operation code for programmable report item --- */
#define PROGRPT_OP_LOOP         1       /* loop operation */
#define PROGRPT_OP_PRINT        2       /* print operation */
#define PROGRPT_OP_MATH         3       /* mathematical operation */
#define PROGRPT_OP_OFFSET       0       /* zero based offset for operation code */


/* --- row & column data for programmable report item --- */
#define PROGRPT_ROW_OFFSET      1       /* zero based offset for row data */
#define PROGRPT_COLUMN_OFFSET   2       /* zero based offset for column data */
#define PROGRPT_MAXCOL_OFFSET   3       /* zero based offset for max column data */

/* --- group code for loop/print operation --- */
#define PROGRPT_GROUP_KEY       1       /* key           (for loop/print) */
#define PROGRPT_GROUP_TOTALCO   2       /* total/counter (only print) */
#define PROGRPT_GROUP_PARAM     3       /* parameter     (only print) */
#define PROGRPT_GROUP_MNEMO     4       /* mnemonic      (only print) */
#define PROGRPT_GROUP_ACCUM     5       /* accumulator   (only print) */


/* --- group code for mathematical operation --- */
#define PROGRPT_MATH_SET        1       /* set     (=) (accumlator = ope1) */
#define PROGRPT_MATH_ADD        2       /* add     (+) (accumlator = ope1 + ope2) */
#define PROGRPT_MATH_SUB        3       /* subtract(-) (accumlator = ope1 - ope2) */
#define PROGRPT_MATH_MUL        4       /* multiply(*) (accumlator = ope1 * ope2) */
#define PROGRPT_MATH_DIV        5       /* divide  (/) (accumlator = ope1 / ope2) */

#define PROGRPT_MATH_OPERAND    0       /* calculate with operand1 & operand2 */
#define PROGRPT_MATH_ONEVALUE   1       /* directry calculate with one value */

/* --- print format no. for print operation --- */
#define PROGRPT_PRT_AUTO        0       /* auto (for date/time) */
#define PROGRPT_PRT_STRING      1       /* print string */
#define PROGRPT_PRT_0           2       /*   10000      */
#define PROGRPT_PRT_0_0         3       /*   10000.0    */
#define PROGRPT_PRT_0_00        4       /*   10000.00   */
#define PROGRPT_PRT_0_000       5       /*   10000.000  */
#define PROGRPT_PRT_CMA_0       6       /*  10,000      */
#define PROGRPT_PRT_CMA_0_0     7       /*  10,000.0    */
#define PROGRPT_PRT_CMA_0_00    8       /*  10,000.00   */
#define PROGRPT_PRT_CMA_0_000   9       /*  10,000.000  */
#define PROGRPT_PRT_D_0         10      /*  $10000      */
#define PROGRPT_PRT_D_0_0       11      /*  $10000.0    */
#define PROGRPT_PRT_D_0_00      12      /*  $10000.00   */
#define PROGRPT_PRT_D_0_000     13      /*  $10000.000  */
#define PROGRPT_PRT_D_CMA_0     14      /* $10,000      */
#define PROGRPT_PRT_D_CMA_0_0   15      /* $10,000.0    */
#define PROGRPT_PRT_D_CMA_0_00  16      /* $10,000.00   */
#define PROGRPT_PRT_D_CMA_0_000 17      /* $10,000.000  */
#define PROGRPT_PRT_A_0         18      /*  @10000      */
#define PROGRPT_PRT_A_0_0       19      /*  @10000.0    */
#define PROGRPT_PRT_A_0_00      20      /*  @10000.00   */
#define PROGRPT_PRT_A_0_000     21      /*  @10000.000  */
#define PROGRPT_PRT_A_CMA_0     22      /* @10,000      */
#define PROGRPT_PRT_A_CMA_0_0   23      /* @10,000.0    */
#define PROGRPT_PRT_A_CMA_0_00  24      /* @10,000.00   */
#define PROGRPT_PRT_A_CMA_0_000 25      /* @10,000.000  */
#define PROGRPT_PRT_N_0         26      /*  #10000      */
#define PROGRPT_PRT_N_0_0       27      /*  #10000.0    */
#define PROGRPT_PRT_N_0_00      28      /*  #10000.00   */
#define PROGRPT_PRT_N_0_000     29      /*  #10000.000  */
#define PROGRPT_PRT_N_CMA_0     30      /* #10,000      */
#define PROGRPT_PRT_N_CMA_0_0   31      /* #10,000.0    */
#define PROGRPT_PRT_N_CMA_0_00  32      /* #10,000.00   */
#define PROGRPT_PRT_N_CMA_0_000 33      /* #10,000.000  */
#define PROGRPT_PRT_0_PER       34      /*   10000%     */
#define PROGRPT_PRT_0_0_PER     35      /*   10000.0%   */
#define PROGRPT_PRT_0_00_PER    36      /*   10000.00%  */
#define PROGRPT_PRT_0_000_PER   37      /*   10000.000% */
#define PROGRPT_PRT_CMA_0_PER       38  /*  10,000%     */
#define PROGRPT_PRT_CMA_0_0_PER     39  /*  10,000.0%   */
#define PROGRPT_PRT_CMA_0_00_PER    40  /*  10,000.00%  */
#define PROGRPT_PRT_CMA_0_000_PER   41  /*  10,000.000% */

#define PROGRPT_PRT_MAX_NUMFORM 40      /* max numeric format */

#define PROGRPT_PRTCHAR_MINUS   '-'
#define PROGRPT_PRTCHAR_DOLLER  '$'
#define PROGRPT_PRTCHAR_AT      '@'
#define PROGRPT_PRTCHAR_NUM     '#'
#define PROGRPT_PRTCHAR_PER     '%'
#define PROGRPT_PRTCHAR_COMMA   ','
#define PROGRPT_PRTCHAR_DECIMAL '.'
#define PROGRPT_PRTCHAR_ZERO    '0'
#define PROGRPT_PRTCHAR_SPACE   ' '


/* --- print format style for print operation --- */
#define PROGRPT_PRINT_RIGHT     0x01    /* right adjust print */
#define PROGRPT_PRINT_LEFT      0x04    /* left adjust print */
#define PROGRPT_PRINT_DOUBLE    0x08    /* double width print */


/* --- print condition for print operation --- */
#define PROGRPT_PRTCOND_COMPUL  0       /* compulsory print */
#define PROGRPT_PRTCOND_LESS    1       /* ( ope1 <  ope2 ) */
#define PROGRPT_PRTCOND_LESSEQ  2       /* ( ope1 <= ope2 ) */
#define PROGRPT_PRTCOND_GREAT   3       /* ( ope1 >  ope2 ) */
#define PROGRPT_PRTCOND_GREATEQ 4       /* ( ope1 >= ope2 ) */
#define PROGRPT_PRTCOND_EQUAL   5       /* ( ope1 == ope2 ) */
#define PROGRPT_PRTCOND_NOEQUAL 6       /* ( ope1 != ope2 ) */
#define PROGRPT_PRTCOND_ANDZERO 10      /* ( ope1 != 0 && ope2 != 0 ) */
#define PROGRPT_PRTCOND_ORZERO  11      /* ( ope1 != 0 || ope2 != 0 ) */
#define PROGRPT_PRTCOND_NOTZERO 12      /* ( ope1 != 0    ) */


/* --- major code for mnemonic group --- */
#define PROGRPT_MNEMO_TRANS     1       /* transaction mnemonic (PRG #20) */
#define PROGRPT_MNEMO_REPORT    2       /* report name          (PRG #22) */
#define PROGRPT_MNEMO_SPECIAL   3       /* special mnemonic     (PRG #23) */
#define PROGRPT_MNEMO_ADJ       4       /* adjective mnemonic   (PRG #46) */
#define PROGRPT_MNEMO_MDEPT     5       /* major dept mnemonic  (PRG #48) */
#define PROGRPT_MNEMO_16CHAR    6       /* 16 char menmonic (only prog.rpt) */
#define PROGRPT_MNEMO_40CHAR    7       /* 40 char mnemonic (only prog.rpt) */


/* --- major code for accumulator group --- */
#define PROGRPT_ACCUM_TOTAL     1       /* accumulator for total */
#define PROGRPT_ACCUM_COUNTER   2       /* accumulator for counter */


/* --- major code for key group --- */
    /* dept */
#define PROGRPT_DEPTKEY_DEPTNO  1       /* department no.       (1...50) */
#define PROGRPT_DEPTKEY_MDEPTNO 2       /* major department no. (1...10) */
    /* plu */
#define PROGRPT_PLUKEY_PLUNO    1       /* plu no.        (1...9999)*/
#define PROGRPT_PLUKEY_DEPTNO   2       /* department no. (1...50) */
#define PROGRPT_PLUKEY_REPORT   3       /* report code    (1...15)*/
    /* coupon */
#define PROGRPT_CPNKEY_CPNNO    1       /* coupon no.   (1...100) */
    /* cashier */
#define PROGRPT_CASKEY_CASNO    1       /* cashier no.  (1...999) */
    /* server */
#define PROGRPT_WAIKEY_WAINO    1       /* server no.   (1...999) */
    /* individual financial */
#define PROGRPT_INDKEY_TERMNO   1       /* terminal no. (1...16) */


/* --- loop key indicate flag --- */
#define PROGRPT_KEYFLAG_DEPTNO  0x0001  /* use department no. */
#define PROGRPT_KEYFLAG_MDEPTNO 0x0002  /* use major department no. */
#define PROGRPT_KEYFLAG_PLUNO   0x0004  /* use plu no. */
#define PROGRPT_KEYFLAG_REPORT  0x0008  /* use report code */
#define PROGRPT_KEYFLAG_CPNNO   0x0010  /* use coupon no. */
/*                              0x0020     reserved */
/*                              0x0040     reserved */
/*                              0x0080     reserved */
#define PROGRPT_KEYFLAG_CASNO   0x0100  /* use cashier no. */
#define PROGRPT_KEYFLAG_WAINO   0x0200  /* use waiter no. */
#define PROGRPT_KEYFLAG_TERMNO  0x0400  /* use terminal no. */
/*                              0x0800     reserved */
/*                              0x1000     reserved */
/*                              0x2000     reserved */
/*                              0x4000     reserved */
/*                              0x8000     reserved */


/* --- major code for total/counter group --- */
                                    /*  O = implemented         */
                                    /*dept,plu,cpn,cas,wai,fin  */
#define PROGRPT_TTL_FROM        1   /*  O   O   O   O   O   O   */
#define PROGRPT_TTL_TO          2   /*  O   O   O   O   O   O   */
#define PROGRPT_TTL_DEPTTOTAL   3   /*  O   .   .   .   .   .   */
#define PROGRPT_TTL_PLUTOTAL    3   /*  .   O   .   .   .   .   */
#define PROGRPT_TTL_CPNTOTAL    3   /*  .   .   O   .   .   .   */
#define PROGRPT_TTL_CGGTOTAL    3   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_ALLTOTAL    4   /*  O   O   .   .   .   .   */
#define PROGRPT_TTL_NETTOTAL    4   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_TAXABLE     5   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_TAXTOTAL    6   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_TAXEXMPT    7   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_NONTAXABLE  8   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_TRAINGGT    9   /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_DECTIP      10  /*  .   .   .   O   O   O   */ /* V3.3 */
#define PROGRPT_TTL_DGGTTL      11  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_WAITOTAL    12  /*  .   .   .   .   O   .   */
#define PROGRPT_TTL_PLUSVOID    13  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_PREVOID     14  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CONSCPN     15  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_ITEMDISC    16  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_MODDISC     17  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_REGDISC     18  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_COMBCPN     19  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_PO          20  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_ROA         21  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_TPO         22  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_TENDER      23  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_OFFTEND     24  /*  .   .   .   .   .   O   */
#define PROGRPT_TTL_FOREIGN     25  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_SERVICE     26  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_ADDCHK      27  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_CHARGE      28  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CANCEL      29  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_MISC        30  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_AUDACT      31  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_NOSALE      32  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_ITEMPROCO   33  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_NOPERSON    34  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CHKOPEN     35  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CHKCLOSE    36  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CUSTOMER    37  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_HASHDEPT    38  /*  .   .   .   O   .   O   */
#define PROGRPT_TTL_BONUS       39  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_CONSTAX     40  /*  .   .   .   O   O   O   */
#define PROGRPT_TTL_TRANTO      41  /*  .   .   .   O   O   .   */ /* V3.3 */
#define PROGRPT_TTL_TRANFROM    42  /*  .   .   .   O   O   .   */ /* V3.3 */
#define PROGRPT_TTL_POSTRECCO   43  /*  .   .   .   O   O   .   */
#define PROGRPT_TTL_VATTOTAL    44  /*  .   .   .   O   .   .   */ /* Add V3.3 */
#define PROGRPT_TTL_TENDONHAND  45  /*  .   .   .   O   .   .   */ /* Saratoga */
#define PROGRPT_TTL_FCONHAND    46  /*  .   .   .   O   .   .   */ /* Saratoga */
#define PROGRPT_TTL_LOAN        47  /*  .   .   .   .   .   O   */ /* Saratoga */
#define PROGRPT_TTL_PICKUP      48  /*  .   .   .   .   .   O   */ /* Saratoga */

/* --- item type of total/counter group --- */
#define PROGRPT_TTLITEM_TOTAL   1   /* total */
#define PROGRPT_TTLITEM_CO      2   /* counter */


/* --- major code for parameter group --- */
    /* dept */
#define PROGRPT_DEPTPARA_MNEMO  1       /* department mnemonic */
    /* plu */
#define PROGRPT_PLUPARA_DEPTMNEMO   1   /* department mnemonic */
#define PROGRPT_PLUPARA_MNEMO   2       /* plu mnemonic */
#define PROGRPT_PLUPARA_ADJNO   3       /* adjective code */
#define PROGRPT_PLUPARA_PRICE   4       /* unit price */
    /* coupon */
#define PROGRPT_CPNPARA_MNEMO   1       /* coupon mnemonic */
#define PROGRPT_CPNPARA_PRICE   2       /* unit price */
    /* cashier */
#define PROGRPT_CASPARA_NAME    1       /* cashier name */
    /* server */
#define PROGRPT_WAIPARA_NAME    1       /* server name */


/* --- loop brace --- */
#define PROGRPT_LOOP_BEGIN_ROOT 0x01    /* begin point of root loop item */
#define PROGRPT_LOOP_BEGIN_NEST 0x02    /* begin point of nest loop item */
#define PROGRPT_LOOP_END_ROOT   0xff    /* end point of root loop item */
#define PROGRPT_LOOP_END_NEST   0xfe    /* end point of nest loop item */


/* --- loop condition ---*/
    /* common use */
#define PROGRPT_ALLREAD         0x00    /* read report all records */
#define PROGRPT_ALLRESET        0x00    /* reset report all records */
    /* dept/plu */
#define PROGRPT_COND_PLUS       0x01    /* only plus(+) item(s) */
#define PROGRPT_COND_MINUS      0x02    /* only minus(-) item(s) */
#define PROGRPT_COND_HASH       0x04    /* only hash item(s) */
#define PROGRPT_COND_SCALE      0x08    /* only scalable item(s) */
    /* cashier/server */
#define PROGRPT_COND_TRAINING   0x01    /* only trainig operator(s) */
#define PROGRPT_COND_REGULAR    0x02    /* only regular (non-training) */


/* --- target total file type --- */
#define PROGRPT_FILE_DEPT       1       /* department */
#define PROGRPT_FILE_PLU        2       /* plu */
#define PROGRPT_FILE_CPN        3       /* combination coupon */
#define PROGRPT_FILE_CAS        4       /* cashier */
#define PROGRPT_FILE_WAI        5       /* server */
#define PROGRPT_FILE_FIN        6       /* register financial */
#define PROGRPT_FILE_INDFIN     7       /* individual financial */
#define PROGRPT_FILE_HOUR       8       /* hourly activity (reserved) */
#define PROGRPT_FILE_GCF        9       /* guest check (reserved) */
#define PROGRPT_FILE_SERTIME    10      /* service time (reserved) */
#define PROGRPT_FILE_EJALL      11      /* e/j cluster reset (reserved) */
#define PROGRPT_FILE_NOTEXEC    0xFFFF  /* not execute at EOD/PTD */


/* --- report output device --- */
#define PROGRPT_DEVICE_RJ21     1       /* r/j printer (21 columns) */
#define PROGRPT_DEVICE_RJ24     2       /* r/j printer (24 columns) */
#define PROGRPT_DEVICE_THERM42  3       /* thermal printer (42 columns) */
#define PROGRPT_DEVICE_THERM56  4       /* thermal printer (56 columns-reserved) */
#define PROGRPT_DEVICE_MLD      5       /* multi-line display (only read report) */


/* --- max. no. of print items --- */
#define PROGRPT_MAX_REPORT_ITEM 150     /* no. of items in 1 report format */
#define PROGRPT_MAX_16CHAR_ADR  20      /* 16 char mnemonic for prog rpt */
#define PROGRPT_MAX_40CHAR_ADR  10      /* 40 char mnemonic for prog rpt */
#define PROGRPT_MAX_ACCUMULATOR 10      /* accumulator for prog rpt */


/* --- record size --- */
#define PROGRPT_MAX_16CHAR_LEN  16      /* max length of 16 chara mnemonic */
#define PROGRPT_MAX_40CHAR_LEN  40      /* max length of 40 chara mnemonic */
#define PROGRPT_MAX_MARK_FHEAD  8       /* max length of mark of file header */
#define PROGRPT_MAX_MARK_LEN    4       /* max length of mark of struct */
#define PROGRPT_MAX_DESC_LEN    32      /* max length of description */
#define PROGRPT_MAX_REC_LEN     sizeof(PROGRPT_RECORD)      /* max length of format record */ //ESMITH PROGRPT

#define PROGRPT_MAX_COLUMN_21   21      /* max column no. (r/j printer) */
#define PROGRPT_MAX_COLUMN_24   24      /* max column no. (r/j printer) */
#define PROGRPT_MAX_COLUMN_42   42      /* max column no. (thermal printer) */
#define PROGRPT_MAX_COLUMN_56   56      /* max column no. (thermal printer) */
#define PROGRPT_MAX_COLUMN_MLD  40      /* max column no. (multi-line display) */
#define PROGRPT_MAX_COLUMN      PROGRPT_MAX_COLUMN_42
                                        /* max column of this version (56->42) */
#define PROGRPT_MAX_ROW_SPOOL   10      /* max row of spool buffer */

/* --- mark of structure --- */
#define PROGRPT_MARK_FHEAD      WIDE("REPORT\0\0")
#define PROGRPT_MARK_EOD        WIDE("EOD\0")
#define PROGRPT_MARK_PTD        WIDE("PTD\0")
#define PROGRPT_MARK_16CHAR     WIDE("16C\0")
#define PROGRPT_MARK_40CHAR     WIDE("40C\0")
#define PROGRPT_MARK_REPORT     WIDE("\xFF\xFF\xFF\x00")


/* --- AM/PM symbol --- */
#define PROGRPT_AM_SYMBOL       "AM"
#define PROGRPT_PM_SYMBOL       "PM"

#define PROGRPT_PLU_LEN         STD_PLU_NUMBER_LEN      /* Saratoga */

/*
===========================================================================
    TYPEDEF
===========================================================================
*/
#pragma pack(push, 1)
/* --- header for programmable report file --- */

typedef struct {
    WCHAR   auchMark[ PROGRPT_MAX_MARK_FHEAD ];     /* mark of header */
    USHORT  usVersion;                              /* version */
    ULONG   ulFileSize;                             /* file size */
    ULONG   ulOffsetEOD;                            /* offset for EOD */
    ULONG   ulOffsetPTD;                            /* offset for PTD */
    ULONG   ulOffset16Char;                         /* offset for 16 char */
    ULONG   ulOffset40Char;                         /* offset for 40 char */
    USHORT  usNoOfReport;                           /* no of report format */
    ULONG   aulOffsetReport[ MAX_PROGRPT_FORMAT ];  /* offset for each report */
} PROGRPT_FHEAD;


/* --- structure for EOD/PTD report --- */
typedef struct {
    USHORT  usFileType;                             /* total file type */
    USHORT  usReportNo;                             /* report format no. */
} PROGRPT_EODPTDREC;

typedef struct {
    WCHAR   auchMark[ PROGRPT_MAX_MARK_LEN ];       /* mark of structure */
    USHORT  usUseProgRpt;                           /* indicate progrpt EOD */
    USHORT  usNoOfRecord;                           /* no of EOD/PTD record */
    PROGRPT_EODPTDREC aEODPTD[ MAX_PROGRPT_EODPTD ];/* EOD/PTD record */
    WCHAR   auchReserved[ 4 ];                      /* reserved */
} PROGRPT_EODPTD;


/* --- structure for 16 characters mnemonic --- */
typedef struct {
    WCHAR   auchMnemonic[ PROGRPT_MAX_16CHAR_LEN ]; /* 16 char mnemonic */
} PROGRPT_16CHARREC;

typedef struct {
    WCHAR   auchMark[ PROGRPT_MAX_MARK_LEN ];       /* mark of structure */
    USHORT  usNoOfRecord;                           /* no of 16 char record */
    PROGRPT_16CHARREC a16Char[ PROGRPT_MAX_16CHAR_ADR ];
    WCHAR   auchReserved[ 10 ];                     /* reserved */
} PROGRPT_16CHAR;


/* --- structure for 40 characters mnemonic --- */
typedef struct {
    WCHAR   auchMnemonic[ PROGRPT_MAX_40CHAR_LEN ]; /* 16 char mnemonic */
} PROGRPT_40CHARREC;

typedef struct {
    WCHAR   auchMark[ PROGRPT_MAX_MARK_LEN ];       /* mark of structure */
    USHORT  usNoOfRecord;                           /* no of 40 char record */
    PROGRPT_40CHARREC a40Char[ PROGRPT_MAX_40CHAR_ADR ];
    WCHAR   auchReserved[ 2 ];                      /* reserved */
} PROGRPT_40CHAR;


/* --- structure for programmable report format --- */
    /* header */
typedef struct {
    WCHAR   auchMark[ PROGRPT_MAX_MARK_LEN ];       /* mark of structure */
    USHORT  usFileType;                             /* total file type */
    USHORT  usDevice;                               /* output device */
    WCHAR   auchDesc[ PROGRPT_MAX_DESC_LEN ];       /* description */
    USHORT  usKeyFlag;                              /* loop key flag */
    ULONG   ulReportSize;                           /* size of this report */
    USHORT  usNoOfLoop;                             /* no of loop record */
    USHORT  usNoOfPrint;                            /* no of print record */
    USHORT  usNoOfMath;                             /* no of math record */
} PROGRPT_RPTHEAD;

    /* item */
typedef struct {
    UCHAR   uchGroup;                               /* group code */
    UCHAR   uchMajor;                               /* major code */
    UCHAR   uchMinor;                               /* minor code */
    UCHAR   uchItem;                                /* item code */
} PROGRPT_ITEM;

// following define is used in several programmable reports structs
// in order to use an anonymous union containing an anonymous struct
// in order to standardize the size of the programmable report data file
// binary record used in database file PARAMRPT.
#define PROGRPT_UNION_STRUCT  \
	union {  \
		SHORT   sOperand1;                              /* operand 1 (SHORT) */  \
		LONG    lOperand1;                              /* operand 1 (LONG) */  \
		PROGRPT_ITEM Operand1;                          /* operand 1 */  \
		struct {  \
			UCHAR   uchReserved3;                           /* */  \
			USHORT  usLoopEnd;                              /* offset for loop end */  \
			UCHAR   uchReserved4;                           /* */  \
		};  \
	};

/* loop record */
typedef struct {
    UCHAR   uchOperation;                           /* operation code (LOOP) */
    UCHAR   uchRow;                                 /* position (row) */
    UCHAR   uchLoopBrace;                           /* loop brace (begin/end) */
    UCHAR   uchReserved1;                           /* */
	UCHAR   auchReserved2[ 2 ];                     /* */
    UCHAR   uchCondition;                           /* condition code */
    PROGRPT_ITEM LoopItem;                          /* loop record */
	PROGRPT_UNION_STRUCT
	PROGRPT_ITEM Reserved5;                         /* */
} PROGRPT_LOOPREC;

    /* print record */
typedef struct {
    UCHAR   uchOperation;                           /* operation code (PRINT) */
    UCHAR   uchRow;                                 /* position (row) */
    UCHAR   uchColumn;                              /* position (column) */
    UCHAR   uchMaxColumn;                           /* max no of column */
	UCHAR   auchFormat[ 2 ];                        /* print format/style */
    UCHAR   uchCondition;                           /* condition code */
    PROGRPT_ITEM PrintItem;                         /* print item */
	PROGRPT_UNION_STRUCT
	PROGRPT_ITEM Operand2;                          /* ope2 for condition */
} PROGRPT_PRINTREC;

    /* mathematics record ( accumulator = opr1 (+,-,*,/) opr2 ) */
typedef struct {
    UCHAR   uchOperation;                           /* operation code (MATH) */
    UCHAR   uchRow;                                 /* position (row) */
    UCHAR   uchColumn;                              /* position (column) */
    UCHAR   uchMaxColumn;                           /* max no of column */
	UCHAR   auchFormat[ 2 ];                        /* format & style */
    UCHAR   uchCondition;                           /* condition code */
    PROGRPT_ITEM Accumulator;                       /* accumulator */
	PROGRPT_UNION_STRUCT
	PROGRPT_ITEM Operand2;                          /* operand 2 */
} PROGRPT_MATHOPE;

    /* mathematics record ( accumulator.total = LONG (+,-,*,/) opr2) */
typedef struct {
    UCHAR   uchOperation;                           /* operation code (MATH) */
    UCHAR   uchRow;                                 /* position (row) */
    UCHAR   uchColumn;                              /* position (column) */
    UCHAR   uchMaxColumn;                           /* max no of column */
	UCHAR   auchFormat[ 2 ];                        /* format style */
    UCHAR   uchCondition;                           /* condition code */
    PROGRPT_ITEM Accumulator;                       /* accumulator */
	PROGRPT_UNION_STRUCT
	PROGRPT_ITEM Operand2;                          /* operand 2 */
} PROGRPT_MATHTTL;

    /* mathematics record ( accumulator.counter = SHORT (+,-,*,/) opr2) */
typedef struct {
    UCHAR   uchOperation;                           /* operation code (MATH) */
    UCHAR   uchRow;                                 /* position (row) */
    UCHAR   uchColumn;                              /* position (column) */
    UCHAR   uchMaxColumn;                           /* max no of column */
	UCHAR   auchFormat[ 2 ];                        /* format & style */
    UCHAR   uchCondition;                           /* condition code */
    PROGRPT_ITEM Accumulator;                       /* accumulator */
	PROGRPT_UNION_STRUCT
	PROGRPT_ITEM Operand2;                          /* operand 2 */
} PROGRPT_MATHCO;


    /* --- record for PROGRAMMABLE REPORT FILE --- */
typedef union {
    PROGRPT_LOOPREC  Loop;                          /* loop record */
    PROGRPT_PRINTREC Print;                         /* print record */
    PROGRPT_MATHOPE  MathOpe;                       /* math (with operand) */
    PROGRPT_MATHTTL  MathTtl;                       /* math (with total) */
    PROGRPT_MATHCO   MathCo;                        /* math (with count) */
} PROGRPT_RECORD;


#pragma pack(pop)

/*
===========================================================================
    EXTERNAL VARIABLE
===========================================================================
*/

/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/* ===== End of File (PROGREPT.H) ===== */
