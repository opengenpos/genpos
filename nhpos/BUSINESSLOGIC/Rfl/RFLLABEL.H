/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170 GP  *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library Headder File
* Category    : Reentrant Functions Library, NCR 2170 Application
* Program Name: RflLabel.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This is a header for LABEL CONVERTION.
*
*           The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version  :   Name     : Description
* Apr-01-93 : 00.00.01 : M.Suzuki   : Initial
* Oct-15-96 : 02.00.00 : Y.Shozy    : R2.0
* Nov-25-96 : 02.00.00 : hrkato     : R2.0
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
*------------------------------------------------------------------------------
*   LA(Label Analysis)  define length(price / quantity / dept)
*------------------------------------------------------------------------------
*/
/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  PLU 1st & 2nd LABEL
*------------------------------------------------------------------------------
*/
#define     PLU02   0x02            /* In-store Label : 02xxxxxxxxxxC  */
#define     PLU20   0x20            /* In-store Label : 20xxxxxxxxxxC  */
#define     PLU21   0x21            /* In-store Label : 21xxxxxxxxxxC  */
#define     PLU22   0x22            /* In-store Label : 22xxxxxxxxxxC  */
#define     PLU23   0x23            /* In-store Label : 23xxxxxxxxxxC  */
#define     PLU24   0x24            /* In-store Label : 24xxxxxxxxxxC  */
#define     PLU25   0x25            /* In-store Label : 25xxxxxxxxxxC  */
#define     PLU26   0x26            /* In-store Label : 26xxxxxxxxxxC  */
#define     PLU27   0x27            /* In-store Label : 27xxxxxxxxxxC  */
#define     PLU28   0x28            /* In-store Label : 28xxxxxxxxxxC  */
#define     PLU29   0x29            /* In-store Label : 29xxxxxxxxxxC  */
#define     PLU04   0x04            /* In-store Label : 04xxxxxxxxxxC  */

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  define MDC
*------------------------------------------------------------------------------
*/
#define     MDC_PLU02       MDC_PLULB02L_ADR
#define     MDC_PLU20       MDC_PLULB20L_ADR
#define     MDC_PLU21       MDC_PLULB21L_ADR
#define     MDC_PLU22       MDC_PLULB22L_ADR
#define     MDC_PLU23       MDC_PLULB23L_ADR
#define     MDC_PLU24       MDC_PLULB24L_ADR
#define     MDC_PLU25       MDC_PLULB25L_ADR
#define     MDC_PLU26       MDC_PLULB26L_ADR
#define     MDC_PLU27       MDC_PLULB27L_ADR
#define     MDC_PLU28       MDC_PLULB28L_ADR
#define     MDC_PLU29       MDC_PLULB29L_ADR
#define     MDC_PLU04       MDC_PLULB04L_ADR

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  MDC Mask Data
*      see function LaInst_CheckType().
*------------------------------------------------------------------------------
*/
#define     MASK_GROUPA     0x03            /* 0x00-0x03 => type 1-5        */
#define     MASK_GROUPB     0xE0            /* 0x20-0xE0 => type 6-12       */
#define     MASK_SELL       0x1C            /* bit2,3,4 = Price,Weight,Qty  */

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  Selling on Price/Weight/Qty Information
*------------------------------------------------------------------------------
*/
#define     SEL_NON         0x00            /* bit2,3,4 = 0 => type 1       */
#define     SEL_PRC         0x04            /* bit2 => selling on Price     */
#define     SEL_WGT         0x08            /* bit3 => selling on Weight    */
#define     SEL_QTY         0x10            /* bit4 => selling on Quantity  */

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  Label Type on MDC
*------------------------------------------------------------------------------
*/
#define     MDC_TYPE_02     0x00
#define     MDC_TYPE_03     0x01
#define     MDC_TYPE_04     0x03
#define     MDC_TYPE_05     0x02
#define     MDC_TYPE_06     0x80
#define     MDC_TYPE_07     0x40
#define     MDC_TYPE_08     0xC0
#define     MDC_TYPE_09     0x20
#define     MDC_TYPE_10     0xA0
#define     MDC_TYPE_11     0x60
#define     MDC_TYPE_12     0xE0

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  Label Type Definition
*   see function LaInst_CheckType().
*------------------------------------------------------------------------------
*/
#define     TYPE_01          1     // Article number
#define     TYPE_02          2     // Article number + Variable Field
#define     TYPE_03          3     // Article number + Variable Field
#define     TYPE_04          4     // Article number + Variable Field
#define     TYPE_05          5      // Article number + Variable Field
#define     TYPE_06          6      // Dept number 4 digit + Price 4 digit, LaInst_CheckLabelB()
#define     TYPE_07          7      // Dept number 4 digit + Price 5 digit, LaInst_CheckLabelB()
#define     TYPE_08          8      // Dept number 5 digit + Price 5 digit. LaInst_CheckLabelB()
#define     TYPE_09          9      // SKU number
#define     TYPE_10         10      // SKU number + Price
#define     TYPE_11         11      // Consumer number
#define     TYPE_12         12      // Account number

#define     TYPE_20         20      /* R2.0, LaInst_CheckLabelC(),  price (xx.xx)  */
#define     TYPE_21         21      /* R2.0, LaInst_CheckLabelC(),  price (xxx.x) */
#define     TYPE_22         22      /* R2.0, LaInst_CheckLabelC(),  price (xxxx) */
#define     TYPE_23         23      /* R2.0, LaInst_CheckLabelC(),  weight (x.xxx) */
#define     TYPE_24         24      /* R2.0, LaInst_CheckLabelC(),  weight (xx.xx) */
#define     TYPE_25         25      /* R2.0, LaInst_CheckLabelC(),  weight (xxx.x) */

#define     TYPE_FREQ       30      /* R2.0, Frequent Shopper number */

/*
*------------------------------------------------------------------------------
*   LA(Label Analysis)  Field Size of Each Data
*------------------------------------------------------------------------------
*/
#define     LEN_UPCA        13
#define     LEN_UPCE         6
#define     LEN_EAN8         8
#define		LEN_RSS14		14


/*
*=============================================================================
*   LA(Label Analysis)  PROTO TYPE DEFINITION
*=============================================================================
*/
/*--- RflLabel.c ---*/
SHORT   LaMain_PreCheck(LABELANALYSIS *pData);
SHORT   LaMain_ChkModify(LABELANALYSIS *pData);
USHORT  LaMain_cvlbl(LABELANALYSIS *pData, TCHAR *aszScanPlu, SHORT sScanLen);
USHORT  LaMain_source(LABELANALYSIS *pData);
USHORT	LaMain_rss14(LABELANALYSIS *pData);
USHORT  LaMain_veloc(LABELANALYSIS *pData, SHORT sScanLen);
VOID    LaMain_UpcE(TCHAR *pluno);
TCHAR   LaMain_plucd(TCHAR *pluno);


/*--- RflRandm.c ---*/
SHORT  _RflInst(LABELANALYSIS *pData);
USHORT  LaInst_GetMDCAdr(TCHAR* aszLookPlu);                        /* R2.0 */
SHORT   LaInst_CheckType(UCHAR uchMDC, UCHAR *uchType);
SHORT   LaInst_CheckLabel(LABELANALYSIS *pData, UCHAR uchType, UCHAR uchMDC);
SHORT   LaInst_CheckLabelA(LABELANALYSIS *pData, UCHAR uchType, UCHAR uchMDC);
SHORT   LaInst_CheckLabelB(LABELANALYSIS *pData, UCHAR uchType);
VOID    LaInst_CheckLabelC(LABELANALYSIS *pData, UCHAR uchType);    /* R2.0 */
VOID    LaInst_CheckEANRandom(USHORT usAddr, UCHAR *uchType);       /* R2.0 */
TCHAR   LaInst_pr4cd(TCHAR *uchPrice);
TCHAR   LaInst_pr5cd(TCHAR *uchPrice);
VOID    LaInst_CheckFreq(TCHAR *puchPLU, UCHAR *uchType);           /* R2.0 */

/*--- RflRssVar.c ---*/
SHORT  _RflRssVar(LABELANALYSIS *pData);

/* --- End of Header --- */
