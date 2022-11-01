/*
* ---------------------------------------------------------------------------
* Title         :   Flexible Memory Assignment Header File (Prog. 2)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P002.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-08-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-17-95 : 03.00.00 : H.ISHIDA   : Addition Combination Coupon & Control String Size.
* Jul-12-95 : 03.00.01 : T.Nakahata : modify P02ChkMemUse() parameter
* Sep-13-95 : 01.00.03 : T.Nakahata : correct calcuration (GC, Item, Cons)
* Sep-14-95 : 01.00.03 : T.Nakahata : max=99 at store/recall system
* Jan-23-94 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-03-98 : 03.03.00 : A.Mitsui   : Remove and Change V3.3
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
#define P02_DSCRB_NO           24   /* number of description from resource  */
#define P02_PTD_NO              4   /* number of PTD selection              *//* V3.3 (add Cashier) */
#define P02_GCDSCRB_NO          4   /* number of guest check description    */
#define P02_DSCRB_LEN          72   /* length of string resource            */
#define P02_ERROR_LEN         128   /* length of error description          */
#define P02_DISP_LEN           32   /* length of display memory use/cas sys */
#define P02_TERM_LEN           16   /* length of terminal type              */

/*** NCR2172 ***/
/*#define P02_DEPT_LEN            2*/   /* department data text length          */
/*#define P02_PLU_LEN             4*/   /* PLU data text length                 */
#define P02_DEPT_LEN            3   /* department data text length          */
#define P02_PLU_LEN             6   /* PLU data text length                 */

#define P02_SER_LEN             3   /* server data text length              */
#define P02_CAS_LEN             3   /* operator data text length            */
#define P02_EJ_LEN              5   /* E/J data text length                 */
#define P02_ETK_LEN             3   /* employee data text length            */
#define P02_GC_LEN              4   /* guest check data text length*///JHHJ Change from 3->4 3-14-04
#define P02_CPN_LEN             3   /* Coupon data text length              */
#define P02_CSTR_LEN            4   /* Control String Size data text length */
#define P02_ITEM_LEN            3   /* Item in a transaction text length    */
#define P02_CONS_LEN            3   /* Item in a transaction text length    */
#define P02_RPT_LEN             2   /* Programmable Report data text length */
#define P02_PPI_LEN             3   /* PPI data text length                 */
#define P02_PATH_LEN          128   /* length of temporary file(full path)  */
#define P02_TEMP_LEN           16   /* length of temporary file             */
#define P02_GLOBAL_LEN       1024   /* PLU copy buffer length               */
#define P02_TERM_MASTER         0   /* terminal type master(file config)    */
#define P02_TERM_SATTELITE      1   /* terminal type sattelite(file config) */
#define P02_EJMIN              10   /* minimum data for electronic journal  */
#define P02_STOREC_GCMAX       1999   /* maximum data for GC (Store/Recall)*///JHHJ Change from 99->999 3-15-04

/* spin button */
#define P02_SPIN_STEP           1   /* normal step of spin button           */
#define P02_SPIN_TURBO         30   /* turbo point of spin button           */
#define P02_SPIN_TURSTEP       10   /* turbo step of spin button            */

/* offset definition */              
#define P02_DEPT                0   /* order of Department                  */
#define P02_PLU                 1   /* order of PLU                         */
#define P02_SER                 2   /* order of Server                      */
#define P02_CAS                 3   /* order of Operator                    */
#define P02_EJ                  4   /* order of E/J                         */
#define P02_ETK                 5   /* order of ETK                         */
#define P02_ITEM                6   /* order of ITEM                        */
#define P02_CONS                7   /* order of CONS                        */
#define P02_GC                  8   /* order of guest check                 */
#define P02_CPN                 9   /* order of Coupon                      */
#define P02_CSTR               10   /* order of Control String Size         */
#define P02_RPT                11   /* order of Programmable Report Size    */
#define P02_PPI                12   /* order of PPI                         */
#define P02_FILE_CLR           13   /* clear caution                        */
#define P02_FILE_EXT           14   /* extinguish caution                   */
#define P02_GCDSCRB1           15   /* guest check description 1            */
#define P02_GCDSCRB2           16   /* guest check description 2            */
#define P02_GCDSCRB3           17   /* guest check description 3            */
#define P02_GCDSCRB4           18   /* guest check description 4            */
#define P02_SETRAM             19   /* set ram size error                   */
#define P02_OVERMEM            20   /* out of memory error                  */
#define P02_TEMPFILE           21   /* file name of temporary file for PLU  */
#define P02_ERR_UNKOWN         22   /* error code message                   */
#define P02_ERR_DISKFULL       23   /* disk full error message              */

#define P02_RATE                0   /* % rate display                       */
#define P02_BYTE                1   /* byte display                         */

/* calculate number of block */
#define P02_MAXBLOCK        65535   /* maximum block for loop               */
#define P02_IDXBLKLV1           6   /* check block level for index file     */
#define P02_IDXBLKLV2         262   /* check block level for index file     */
#define P02_BLKBYTE           512   /* bytes for file 1-block               */
#define P02_IDXBLKBYTE        256   /* bytes for index file 1-block         */
#define P02_CALC              100   /* calculate %                          */
#define P02_DEC                10   /* calculate under decimal point        */
#define P02_CALCBYTE            2   /* calculate byte                       */
#define P02_64KB         2048576L  /* RAM size                             */
/* #define P02_64KB            65536L  / RAM size                             */
#define P02_PLUTTL_MAX     30000L  /* upto 30,000 PLU                         */
#define P02_INDTTL_MAX         16L  /* upto 16 terminal                       */

#define P02_INITFLAG       0x0000   /* initial data for check flag          */
#define P02_CHG_USER       0x0001   /* check flag for user hope change      */
#define P02_CHG_FILE       0x0010   /* check flag for file change           */

/* memory use graph value */
/*
#define P02_AREAHIGHT         104
#define P02_AREAWIDTH          88
#define P02_AREALEFT          450
#define P02_AREATOP            52
#define P02_AREARIGHT   (P02_AREALEFT + P02_AREAWIDTH)*/

#define P02_AREAHIGHT         345   /* hight of grapgh area                 */
#define P02_AREAWIDTH         330   /* width of grapgh area                 */
#define P02_AREALEFT         1708   /* left point of graph area             */
#define P02_AREATOP           180   /* top point of graph area              */
#define P02_AREARIGHT   (P02_AREALEFT + P02_AREAWIDTH)
                                    /* right point of graph area            */
#define P02_AREABOTTOM  (P02_AREATOP + P02_AREAHIGHT)
                                    /* bottom point of graph area           */
#define P02_AREAEDGE           10   /* edge of bar in graph area            */
#define P02_PEN_WIDTH           1   /* width of graph area frame            */

#define P02_TTLUPMINSIZE     4096   /* Total update file minimam size       */

#define P02_CLIENT_LEFT(x)      (int)((x) *  11 / 14 )
#define P02_CLIENT_RIGHT(x)     (int)((x) / 70  * 69 )
#define P02_CLIENT_TOP(y)       (int)((y) *  1  / 9 )
#define P02_CLIENT_BOTTOM(y)    (int)((y) *  5  / 12 )

/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/

/*    Dialog ID default value */

#define IDD_P02_ITEM         20201
#define IDD_P02EDIT         (IDD_P02_ITEM + 1)
#define IDD_P02SPIN         (IDD_P02_ITEM + 31)
#define IDD_P02PTD          (IDD_P02_ITEM + 51)
#define IDD_P02MEMUSE       (IDD_P02_ITEM + 70)             /* memory useage    */
#define IDD_P02RATE         (IDD_P02_ITEM + 72 + P02_RATE)  /* % rate display   */
#define IDD_P02BYTE         (IDD_P02_ITEM + 72 + P02_BYTE)  /* byte display     */

#define IDD_P02_TERM        IDD_P02_ITEM                /* terminal type    */
#define IDD_P02_MAXGC       (IDD_P02BYTE + 10)          /* range of gc# */

/*    Dialog ID Each Item Value */

#define IDD_P02EDIT_DEPT    (IDD_P02EDIT + P02_DEPT)    /* department       */
#define IDD_P02EDIT_PLU     (IDD_P02EDIT + P02_PLU)     /* PLU              */
#define IDD_P02EDIT_SER     (IDD_P02EDIT + P02_SER)     /* server           */
#define IDD_P02EDIT_CAS     (IDD_P02EDIT + P02_CAS)     /* operator         */
#define IDD_P02EDIT_EJ      (IDD_P02EDIT + P02_EJ)      /* E/J              */
#define IDD_P02EDIT_ETK     (IDD_P02EDIT + P02_ETK)     /* ETK              */
#define IDD_P02EDIT_GC      (IDD_P02EDIT + P02_GC)      /* guest check      */
#define IDD_P02EDIT_CPN     (IDD_P02EDIT + P02_CPN)     /* Coupon           */
#define IDD_P02EDIT_CSTR    (IDD_P02EDIT + P02_CSTR)    /* Control String Size */
#define IDD_P02EDIT_ITEM    (IDD_P02EDIT + P02_ITEM)    /* Item Storage     */
#define IDD_P02EDIT_CONS    (IDD_P02EDIT + P02_CONS)    /* Consoli. Storage */
#define IDD_P02EDIT_RPT     (IDD_P02EDIT + P02_RPT)     /* Programmable Report */
#define IDD_P02EDIT_PPI     (IDD_P02EDIT + P02_PPI)     /* PPI              */

#define IDD_P02SPIN_DEPT    (IDD_P02SPIN + P02_DEPT)    /* spin for dept.   */
#define IDD_P02SPIN_PLU     (IDD_P02SPIN + P02_PLU)     /* spin for PLU     */
#define IDD_P02SPIN_SER     (IDD_P02SPIN + P02_SER)     /* spin for server  */
#define IDD_P02SPIN_CAS     (IDD_P02SPIN + P02_CAS)     /* spin for operator*/
#define IDD_P02SPIN_EJ      (IDD_P02SPIN + P02_EJ)      /* spin for E/J     */
#define IDD_P02SPIN_ETK     (IDD_P02SPIN + P02_ETK)     /* spin for ETK     */
#define IDD_P02SPIN_ITEM    (IDD_P02SPIN + P02_ITEM)    /* Item Storage     */
#define IDD_P02SPIN_CONS    (IDD_P02SPIN + P02_CONS)    /* Consoli. Storage */
#define IDD_P02SPIN_GC      (IDD_P02SPIN + P02_GC)      /* spin for GC      */
#define IDD_P02SPIN_CPN     (IDD_P02SPIN + P02_CPN)     /* spin for Coupon  */
#define IDD_P02SPIN_CSTR    (IDD_P02SPIN + P02_CSTR)    /* spin for Control String Size */
#define IDD_P02SPIN_RPT     (IDD_P02SPIN + P02_RPT)     /* spin for Programmable Report */
#define IDD_P02SPIN_PPI     (IDD_P02SPIN + P02_PPI)     /* spin for PPI     */

#define IDD_P02PTD_DEPT     (IDD_P02PTD + P02_DEPT)     /* PTD for dept.    */
#define IDD_P02PTD_PLU      (IDD_P02PTD + P02_PLU)      /* PTD for PLU      */
#define IDD_P02PTD_SER      (IDD_P02PTD + P02_SER)      /* PTD for server   */
#define IDD_P02PTD_CAS      (IDD_P02PTD + P02_CAS)      /* PTD for operator */
#define IDD_P02PTD_EJ       (IDD_P02PTD + P02_EJ)       /* PTD for E/J      */
#define IDD_P02PTD_GC       (IDD_P02PTD + P02_GC)       /* PTD for GC       */
#define IDD_P02PTD_CPN      (IDD_P02PTD + P02_CPN)      /* PTD for Coupon   */

//Multilingual PEP RPH 4-21-03
#define IDD_P02_TERMINAL		20300
#define IDD_P02_DEPT			20301
#define IDD_P02_PLU				20302
#define IDD_P02_OPERATOR		20303
#define IDD_P02_EMPLOYEE		20304
#define IDD_P02_EJ				20305
#define IDD_P02_CTLSTR			20306
#define IDD_P02_COUPON			20307
#define IDD_P02_PPI				20308
#define IDD_P02_PROGRPT			20309
#define IDD_P02_GUEST			20310
#define IDD_P02_ITEMA			20311
#define IDD_P02_ITEMB			20312
#define IDD_P02_MEM				20313
#define IDD_P02_AB				20314
#define IDD_P02_NUMITEM			20315
#define IDD_P02_DEPT_RNG		20316
#define IDD_P02_PLU_RNG			20317
#define IDD_P02_OPERATOR_RNG	20318
#define IDD_P02_EMPLOYEE_RNG	20319
#define IDD_P02_EJ_RNG			20320
#define IDD_P02_CTLSTR_RNG		20321
#define IDD_P02_COUPON_RNG		20322
#define IDD_P02_PPI_RNG			20323
#define IDD_P02_PROGRPT_RNG		20324
#define IDD_P02_ITEMA_RNG		20325
#define IDD_P02_ITEMB_RNG		20326

/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/

/*JHHJ 1-8-04, Moved from P002.H and placed in PEP.H , renamed to IDS_PEP_....
so that it these mnemonics can be used in multiple places, mainly in the
error message for transfer "could not take "      ".  This will help by 
not having to use additional mnemonics for something that was already defined.*/

/*#define IDS_P02_FILE_DEPT       (IDS_P02 + P02_DEPT)
#define IDS_P02_FILE_PLU        (IDS_P02 + P02_PLU)
#define IDS_P02_FILE_SER        (IDS_P02 + P02_SER)
#define IDS_P02_FILE_CAS        (IDS_P02 + P02_CAS)
#define IDS_P02_FILE_EJ         (IDS_P02 + P02_EJ)
#define IDS_P02_FILE_ETK        (IDS_P02 + P02_ETK)
#define IDS_P02_FILE_ITEM       (IDD_P02 + P02_ITEM)
#define IDS_P02_FILE_CONS       (IDD_P02 + P02_CONS)
#define IDS_P02_FILE_GC         (IDS_P02 + P02_GC)
#define IDS_P02_FILE_CPN        (IDS_P02 + P02_CPN)
#define IDS_P02_FILE_CSTR       (IDS_P02 + P02_CSTR)
#define IDS_P02_FILE_RPT        (IDS_P02 + P02_RPT)
#define IDS_P02_FILE_PPI        (IDS_P02 + P02_PPI)*/
#define IDS_P02_FILE_CLR        (IDS_P02 + P02_FILE_CLR)
#define IDS_P02_FILE_EXT        (IDS_P02 + P02_FILE_EXT)
#define IDS_P02_GCDSCRB1        (IDS_P02 + P02_GCDSCRB1)
#define IDS_P02_GCDSCRB2        (IDS_P02 + P02_GCDSCRB2)
#define IDS_P02_GCDSCRB3        (IDS_P02 + P02_GCDSCRB3)
#define IDS_P02_GCDSCRB4        (IDS_P02 + P02_GCDSCRB4)
#define IDS_P02_SETRAM          (IDS_P02 + P02_SETRAM)
#define IDS_P02_OVERMEM         (IDS_P02 + P02_OVERMEM)
#define IDS_P02_TEMPFILE        (IDS_P02 + P02_TEMPFILE)

#define IDS_P02_ERR_UNKOWN      (IDS_P02 + P02_ERR_UNKOWN)
#define IDS_P02_ERR_DISKFULL    (IDS_P02 + P02_ERR_DISKFULL)

#define IDS_P02_MAX_GC          (IDS_P02 + P02_ERR_DISKFULL + 1)
#define IDS_P02_MAX_STOREC      (IDS_P02 + P02_ERR_DISKFULL + 2)

#define IDS_P02_RAM_8MB         (IDS_P02_MAX_STOREC + 1)
#define IDS_P02_RAM_16MB        (IDS_P02_RAM_8MB + 1)
#define IDS_P02_RAM_24MB        (IDS_P02_RAM_16MB + 1)
#define IDS_P02_RAM_32MB        (IDS_P02_RAM_24MB + 1)
#define IDS_P02_RAM_40MB        (IDS_P02_RAM_32MB + 1)
#define IDS_P02_RAM_48MB        (IDS_P02_RAM_40MB + 1)
#define IDS_P02_RAM_UNKNOWN     (IDS_P02_RAM_48MB + 1)

/* ----- Error Message Sting ID, Saratoga ----- */

#define IDS_P02_OVERRANGE       (IDS_P02_RAM_UNKNOWN + 3)

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef FLEXMEM FAR*    LPFLEXMEM;

typedef struct _P02STR {
    WCHAR    aszDscrb[P02_DSCRB_NO][P02_DSCRB_LEN];      /* descripton */
} P02STR, FAR *LPP02STR;

typedef struct _P02BUFF {
    DWORD   dwData;         /* item data */ 
    BYTE    bPtd;           /* PTD file selection */
    DWORD   dwPara;         /* parameter file size */
    DWORD   dwTotal;        /* total file size */
} P02BUFF, FAR *LPP02BUFF;

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
typedef struct _P02BUFF {
    WORD    wData;          /* item data */
    BYTE    bPtd;           /* PTD file selection */
    DWORD   dwPara;         /* parameter file size */
    DWORD   dwTotal;        /* total file size */
} P02BUFF, FAR *LPP02BUFF;
#endif

typedef struct _P02CHECK {
    USHORT  nModFileID;     /* file maintenance flag for file */
    WORD    fwFile;         /* file maintenance flag for transfer */
} P02CHECK, FAR *LPP02CHECK;

typedef struct _P02PARA {
    FLEXMEM aCurt[MAX_FLXMEM_SIZE];     /* current parameter */
    FLEXMEM aPrev[MAX_FLXMEM_SIZE];     /* previous parameter */
} P02PARA, FAR *LPP02PARA;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
VOID    P02CalcSize(LPP02BUFF, WORD wIdx);
BOOL    P02ChkRng(HWND, LPP02BUFF, WORD wIdx);
BOOL	P02ChkAllDataRng(HWND);
BOOL    P02FinDlg(HWND, LPP02PARA, HGLOBAL, WPARAM);
BOOL    P02GetData(HWND, LPP02PARA);
BOOL    P02ChkFileChg(HWND, LPP02PARA, LPP02CHECK);
BOOL    P02ExecErrProc(HWND, LPP02PARA, WORD, LPP02CHECK);
VOID    P02DispCaution(HWND, UINT, SHORT);
VOID    P02DispGCMax( HWND, UCHAR );
static  USHORT  RecreatePluFile(HWND, ULONG);               /* Saratoga */
static  USHORT  RecreatePluIndexFile(HWND, ULONG);          /* Saratoga */

/* ----- ###DEL Saratoga
BOOL    P02ChkFileConfig(HWND, LPWORD);
WORD    P02CalcBlock(HWND, WORD);
BOOL    P02InitDlg(HWND, LPWORD, LPWORD, LPP02PARA, LPHGLOBAL);
WORD    P02DrawMemUse(HWND, WORD, LPWORD);
BOOL    P02ChkMemUse(HWND, WORD, WORD);
VOID    P02DispMemRate(HWND, WORD, LPWORD, WORD);
BOOL    P02SpinProc(HWND, WPARAM, LPARAM, LPWORD);
WORD    P02CalcIndexBlock(DWORD);
WORD    P02CalcSize_Item( LPP02BUFF lpBuff );
WORD    P02CalcSize_Cons( HWND, LPP02BUFF lpBuff );

----- */

DWORD   P02CalcBlock(HWND, WORD);
BOOL    P02InitDlg(HWND, LPDWORD, LPDWORD, LPP02PARA, LPHGLOBAL);
DWORD   P02DrawMemUse(HWND, DWORD, LPDWORD);
BOOL    P02ChkMemUse(HWND, DWORD, DWORD);
VOID    P02DispMemRate(HWND, DWORD, LPDWORD, WORD);
BOOL    P02SpinProc(HWND, WPARAM, LPARAM, LPDWORD);
BOOL    P02ChkFileConfig(HWND, LPDWORD);
DWORD   P02CalcIndexBlock(DWORD);
DWORD   P02CalcSize_Item( LPP02BUFF lpBuff );
DWORD   P02CalcSize_Cons( HWND, LPP02BUFF lpBuff );

VOID	InitDialogStrings(HWND hDlg);
/* ===== End of P002.H ===== */