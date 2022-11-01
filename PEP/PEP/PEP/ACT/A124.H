/*
* ---------------------------------------------------------------------------
* Title         :   Header File of TAX Table Maintenance (AC 124,125,126,129)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A124.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
*
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
/* ----- Para Size ----- */
#define IDD_A124_TEST       12400   /* tax table test dialog template ID    */
#define IDD_A124_TAXTYPE    12401   /* tax type (US/Canada)                 */
#define IDD_A124_TAXTYPE1   12402   /* Tax1/GST selection radio-button      */
#define IDD_A124_TAXTYPE2   12403   /* Tax2/PST1 selection radio-button     */
#define IDD_A124_TAXTYPE3   12404   /* Tax3/PST2 selection radio-button     */
#define IDD_A124_TAXTYPE4   12405   /* PST3 selection radio-button          */
#define IDD_A124_QUOTI      12406   /* tax rate - quotient                  */
#define IDD_A124_DENOM      12407   /* tax rate - denominator               */
#define IDD_A124_NUMER      12408   /* tax rate - numerator                 */
#define IDD_A124_CLR        12409   /* all clear button                     */
#define IDD_A124_SAMT1      12410   /* sales amount 1 (static text)         */
#define IDD_A124_SAMT2      12411   /* sales amount 2 (edit control)        */
#define IDD_A124_TAMT       12412   /* tax amount (edit control)            */
#define IDD_A124_TBLLIST    12413   /* tax table listbox                    */
#define IDD_A124_TESTMODE   12414   /* test mode button                     */
#define IDD_A124_CHG        12415   /* tax table data change button         */
#define IDD_A124_DEL        12416   /* tax table data delete button         */
#define IDD_A124_WDC        12417   /* whole doller constant (static text)  */
#define IDD_A124_CONDIT     12418   /* tax table condition(complete or not) */

#define IDD_A124_TAXRATE	12419
#define IDD_A124_CURTAX		12420
#define IDD_A124_SALEAMT	12421
#define IDD_A124_TAXAMT		12422
#define IDD_A124_SALEAMT2	12423
#define IDD_A124_TAXAMT2	12424
#define IDD_A124_DLRCONST	12425
#define IDD_A124_TAXCOND	12426
#define IDD_A124_CAPTION1	12427
#define IDD_A124_CAPTION2	12428
#define IDD_A124_CAPTION3	12429

#define IDD_A124_TEST_TYPE  12430   /* tax type in test mode                */
#define IDD_A124_TEST_RATE  12431   /* tax rate in test mode                */
#define IDD_A124_TEST_SAMT  12432   /* sales amount in test mode            */
#define IDD_A124_TEST_TAMT  12433   /* tax amount in test mode              */

#define IDD_A124_TEST_AMT	12434
#define IDD_A124_TEST_TAX	12435
/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_A124_ERROVER    (IDS_A124)
#define IDS_A124_WARNMSG1   (IDS_A124 +  1)
#define IDS_A124_WARNMSG2   (IDS_A124 +  2)
#define IDS_A124_SEPARAT    (IDS_A124 +  3)
#define IDS_A124_TYPE_US    (IDS_A124 +  4)
#define IDS_A124_TYPE_CN    (IDS_A124 +  5)
#define IDS_A124_TYPE_US1   (IDS_A124 +  6)
#define IDS_A124_TYPE_US2   (IDS_A124 +  7)
#define IDS_A124_TYPE_US3   (IDS_A124 +  8)
#define IDS_A124_TYPE_US4   (IDS_A124 +  9)
#define IDS_A124_TYPE_CN1   (IDS_A124 + 10)
#define IDS_A124_TYPE_CN2   (IDS_A124 + 11)
#define IDS_A124_TYPE_CN3   (IDS_A124 + 12)
#define IDS_A124_TYPE_CN4   (IDS_A124 + 13)
#define IDS_A124_TAXRATE    (IDS_A124 + 14)
#define IDS_A124_CONDIT_1   (IDS_A124 + 15)
#define IDS_A124_CONDIT_2   (IDS_A124 + 16)
#define IDS_A124_CHKCHG_1   (IDS_A124 + 17)
#define IDS_A124_CHKCHG_2   (IDS_A124 + 18)
#define IDS_A124_CHKCHG_3   (IDS_A124 + 19)
#define IDS_A124_CHKCHG_4   (IDS_A124 + 20)
#define IDS_A124_DELETE     (IDS_A124 + 21)
#define IDS_A124_TEST       (IDS_A124 + 22)
#define IDS_A124_0INPUT     (IDS_A124 + 23)
#define IDS_A124_RATEERROR  (IDS_A124 + 24)
#define IDS_A124_NOINPUT    (IDS_A124 + 25)
#define IDS_A124_SMALLER    (IDS_A124 + 26)
#define IDS_A124_INIT_SAMT  (IDS_A124 + 27)
#define IDS_A124_BUFF_SAMT  (IDS_A124 + 28)
#define IDS_A124_RATELIST   (IDS_A124 + 29)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A124_LEN_TEST           5   /* length of test amount data           */
#define A124_LEN_SAMT           5   /* length of sales amount data          */
#define A124_LEN_TAMT           3   /* length of tax amount data            */
#define A124_LEN_BUFF          64   /* length of description buffer         */
#define A124_LEN_RATE           2   /* length of tax rate                   */
#define A124_NUM_TYPE           4   /* number of tax type(124,125,126,129)  */
#define A124_OFS_TAX1           0   /* offset of tax1/GST (AC 124)          */
#define A124_OFS_TAX2           1   /* offset of tax2/PST1 (AC 125)         */
#define A124_OFS_TAX3           2   /* offset of tax3/PST2 (AC 126)         */
#define A124_OFS_TAX4           3   /* offset of tax4/PST3 (AC 129)         */
#define A124_OFS_WDC            0   /* offset of WDC in tax data            */
#define A124_OFS_QUOTI          2   /* offset of Quotient in tax data       */
#define A124_OFS_DENOM          3   /* offset of Denominator in tax data    */
#define A124_OFS_NUMER          4   /* offset of Numerator in tax data      */
#define A124_OFS_SAMT           5   /* offset of sales amount in tax data   */
#define A124_SIZE_INFO          6   /* size of information (wdc, q,d,n,...) */
#define A124_SIZE_HEADER        8   /* size of data header                  */
#define A124_MAX_SAMT         255   /* maximum of salse amount              */
#define A124_WDC_PERCENT      100   /* WDC percent rate translation         */
#define A124_WDC_COMPLETE_CHK   2   /* check WDC value for complete table   */
#define A124_INIT_COUNT         1   /* initial value of WDC counter         */
#define A124_TAX_ADDR          15   /* MDC tax address (US/Canada)          */
#define A124_TAX_CHK         0x01   /* MDC tax check bit                    */

#define A124_ERR_TAMT_UND      -1   /* error code (TAmt < last TAmt)        */
#define A124_ERR_SAMT_UND      -2   /* error code (SAmt < last SAmt)        */
#define A124_ERR_TAMT_OVR      -3   /* error code (TAmt > next TAmt)        */
#define A124_ERR_SAMT_OVR      -4   /* error code (SAmt > next SAmt)        */

#define A124_CHK_TAX1      0x0001   /* check bit for tax1                   */
#define A124_CHK_TAX2      0x0002   /* check bit for tax2                   */
#define A124_CHK_TAX3      0x0004   /* check bit for tax3                   */
#define A124_CHK_TAX4      0x0008   /* check bit for tax4                   */
#define A124_CHK_RATE      0x0010   /* check bit for tax rate entered       */
#define A124_CHK_TEST      0x0020   /* check bit for test mode              */

/*
* ===========================================================================
*       Structure type defintions
* ===========================================================================
*/
typedef struct  _A124TAXDATA {  /* user edit data */
    ULONG   ulSlsAmt;                           /* sales amount */
    ULONG   ulTaxAmt;                           /* tax amount */
} A124TAXDATA, FAR *LPA124TAXDATA;

typedef struct  _A124TAXLEN {  /* tax length buffer */
    USHORT  usTaxLen1;                          /* length of tax1 */
    USHORT  usTaxLen2;                          /* length of tax2 */
    USHORT  usTaxLen3;                          /* length of tax3 */
    USHORT  usTaxLen4;                          /* length of tax4 */
} A124TAXLEN, FAR *LPA124TAXLEN;

typedef struct  _A124FILEDATA { /* file data */
    A124TAXLEN  TaxLen;                         /* tax length buffer */
    UCHAR       auchTaxTbl[MAX_TTBLDATA_SIZE];  /* tax table data buffer */
} A124FILEDATA, FAR *LPA124FILEDATA;

typedef struct  _A124BUFFDATA { /* temporary tax data */
    A124TAXLEN      TaxLen;                     /* tax length buffer */
    LPA124TAXDATA   alpTaxData[A124_NUM_TYPE];    /* tax data buffer */
} A124BUFFDATA, FAR *LPA124BUFFDATA;

typedef struct  _A124TAXRATE {  /* tax rate */
    BOOL    fStatus;                            /* Status */
    USHORT  usWdc;                              /* Whole Doller Constant */
    USHORT  usQuoti;                            /* Quotient */
    USHORT  usDenom;                            /* Denominator */
    USHORT  usNumer;                            /* Numerator */
    USHORT  usWdcCount;                         /* WDC Counter */
} A124TAXRATE, FAR *LPA124TAXRATE;

/*
* ===========================================================================
*       Macro Difinitions
* ===========================================================================
*/
#define A124GETWDC(x)       (USHORT)(((USHORT)(x) & 0x00ff) * A124_WDC_PERCENT)
#define A124GETQUOTI(x)     (USHORT)((USHORT)(x) & 0x00ff)
#define A124GETDENOM(x)     (USHORT)((USHORT)(x) & 0x00ff)
#define A124GETNUMER(x)     (USHORT)((USHORT)(x) & 0x00ff)
#define A124GETCOUNT(x,y)   (USHORT)((x) / (((y) > 0) ? (y) * A124_WDC_PERCENT : A124_WDC_PERCENT))
#define A124GETTAMT(x)      (USHORT)((USHORT)(x) & 0x00ff)

/*
* ===========================================================================
*       External Valiables
* ===========================================================================
*/

/*
* ===========================================================================
*       Function Prototypes
* ===========================================================================
*/
VOID        A124SetInitData(HWND, LPHGLOBAL, LPA124FILEDATA, LPA124BUFFDATA);
VOID        A124SetTaxRate(HWND);
BOOL        A124SetTaxData(HWND, LPA124BUFFDATA);
VOID        A124ChgTblData(HWND, LPA124BUFFDATA);
VOID        A124DelTblData(HWND, LPA124BUFFDATA);
VOID        A124ClrAllData(HWND, LPA124BUFFDATA);
VOID        A124DspTblData(HWND);
VOID        A124ChgEditType(HWND, WPARAM, LPA124BUFFDATA);
VOID        A124ResetListbox(HWND, USHORT, LPA124TAXDATA);
VOID        A124ShowRate(HWND, WPARAM);
VOID        A124SetEntStat(HWND);
VOID        A124SetTestStat(HWND);
VOID        A124ResetTestStat(HWND);
VOID        A124ConvReadData(LPA124FILEDATA, LPA124BUFFDATA);
VOID        A124ConvWriteData(LPA124FILEDATA, LPA124BUFFDATA);
UCHAR       A124QueryTaxWdc(USHORT, LPA124TAXDATA, USHORT);
BOOL        A124QueryMaxData(ULONG, ULONG, LPA124BUFFDATA);
VOID        A124MakeDblTbl(HWND);
VOID        A124ShowWdc(HWND);
BOOL        A124ChkEntData(ULONG, ULONG, LPA124BUFFDATA);
BOOL        A124ChkChgData(HWND, ULONG, ULONG, LPA124BUFFDATA);
BOOL        A124QuerySaveData(HWND, LPA124TAXRATE);
ULONG       A124TestCalcTaxRate(ULONG);
int         A124DspError(HWND, WPARAM, UINT);

/* ===== End of File ===== */
