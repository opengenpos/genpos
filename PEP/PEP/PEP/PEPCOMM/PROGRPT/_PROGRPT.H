/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.1              ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Internal Header file
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGRPT.H
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date        Ver.Rev  : NAME        : Description
*   Jan-18-96 : 00.00.01 : T.Nakahata  : Initial
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

/* --- internal system define --- */
#define PROGRPT_INVALID_HANDLE      -1      /* invalid handle value */
#define PROGRPT_SEM_INIT_CO         1       /* initial count for semaphore */
#define PROGRPT_BYTE_PER_BLOCK      1024UL  /* no. of bytes in a file block */

#define PROGRPT_OFFSET_FILEHEADER   0UL     /* offset for file header */


/* --- work buffer size --- */
#define PROGRPT_BACKUP_WORK_SIZE    450     /* buffer for back up function */
#define PROGRPT_NUMFORM_WORK_SIZE   16      /* buffer for numeric data format */
#define PROGRPT_DATETIME_WORK_SIZE  16      /* buffer for date/time format */


/* --- data type --- */
#define PROGRPT_TYPE_CHAR           1       /* signed char */
#define PROGRPT_TYPE_UCHAR          2       /* unsigned char */
#define PROGRPT_TYPE_SHORT          3       /* signed short int */
#define PROGRPT_TYPE_USHORT         4       /* unsigned short int */
#define PROGRPT_TYPE_LONG           5       /* singed long int */
#define PROGRPT_TYPE_ULONG          6       /* unsigned long int */
#define PROGRPT_TYPE_TOTAL          7       /* TOTAL structure */
#define PROGRPT_TYPE_LTOTAL         8       /* LTOTAL structure */
#define PROGRPT_TYPE_DATE           9       /* DATE structure */
#define PROGRPT_TYPE_D13            10      /* D13DIGITS structure */
#define PROGRPT_TYPE_STRING         11      /* string */


#define PROGRPT_CONTINUE            1
#define PROGRPT_EOF                 2

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

typedef struct {
    USHORT  usSeqNO;                /* Sequnece Number */
    ULONG   offulFilePosition;      /* Write and Read Position */
    UCHAR   uchStatus;              /* Status EOF, CONTINUE */
} PROGRPT_BACKUP;

typedef struct {
    USHORT  usDataLen;              /* Data Length  */
    UCHAR   auchData[1];            /* PROGRPT File Data  */
} PROGRPT_BAKDATA;


/*
===========================================================================
    EXTERNAL VARIABLE
===========================================================================
*/
extern SHORT    hsProgRptFile;
extern ULONG    ulProgRptFileSize;

extern WCHAR FARCONST auchPROG_RPT[];

extern UCHAR FARCONST  auchNEW_FILE_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_READ_WRITE[];
extern UCHAR FARCONST  auchOLD_FILE_READ[];

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

BOOL ProgRpt_InitFileHeader( UCHAR uchNoOfBlock );


/* ===== End of File (_PROGRPT.H) ===== */
