/*
* ---------------------------------------------------------------------------
* Title         :   Set Tax Rates Header File (AC 127)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A127.H
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
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID 
* ===========================================================================
*/
#define A127_CTRL_OFF   10

#define IDD_A127MODE    12701
#define IDD_A127ADDR1   12702
#define IDD_A127ADDR2   12703
#define IDD_A127ADDR3   12704
#define IDD_A127ADDR4   12705
#define IDD_A127ADDR5   12706

#define IDD_A127_CAPTION1	12707

#define IDD_A127DSC1    (IDD_A127ADDR1 + A127_CTRL_OFF)
#define IDD_A127DSC2    (IDD_A127ADDR2 + A127_CTRL_OFF)
#define IDD_A127DSC3    (IDD_A127ADDR3 + A127_CTRL_OFF)
#define IDD_A127DSC4    (IDD_A127ADDR4 + A127_CTRL_OFF)
#define IDD_A127DSC5    (IDD_A127ADDR5 + A127_CTRL_OFF)
#define IDD_A127EDIT1   (IDD_A127DSC1  + A127_CTRL_OFF)
#define IDD_A127EDIT2   (IDD_A127DSC2  + A127_CTRL_OFF)
#define IDD_A127EDIT3   (IDD_A127DSC3  + A127_CTRL_OFF)
#define IDD_A127EDIT4   (IDD_A127DSC4  + A127_CTRL_OFF)
#define IDD_A127EDIT5   (IDD_A127DSC5  + A127_CTRL_OFF)
#define IDD_A127RNG1    (IDD_A127EDIT1 + A127_CTRL_OFF)
#define IDD_A127RNG2    (IDD_A127EDIT2 + A127_CTRL_OFF)
#define IDD_A127RNG3    (IDD_A127EDIT3 + A127_CTRL_OFF)
#define IDD_A127RNG4    (IDD_A127EDIT4 + A127_CTRL_OFF)
#define IDD_A127RNG5    (IDD_A127EDIT5 + A127_CTRL_OFF)

/*
* ===========================================================================
*       String ID  Definitions
* ===========================================================================
*/
#define IDS_A127_USMODE     IDS_A127
#define IDS_A127_CANMODE    (IDS_A127 + 1)
#define IDS_A127_USADDR1    (IDS_A127 + 2)
#define IDS_A127_USADDR2    (IDS_A127 + 3)
#define IDS_A127_USADDR3    (IDS_A127 + 4)
#define IDS_A127_USADDR4    (IDS_A127 + 5)
#define IDS_A127_USADDR5    (IDS_A127 + 6)
#define IDS_A127_CANADDR1   (IDS_A127 + 7)
#define IDS_A127_CANADDR2   (IDS_A127 + 8)
#define IDS_A127_CANADDR3   (IDS_A127 + 9)
#define IDS_A127_CANADDR4   (IDS_A127 +10)
#define IDS_A127_CANADDR5   (IDS_A127 +11)
#define IDS_A127_RNGOVER    (IDS_A127 +12)
#define IDS_A127_RNGOVER5   (IDS_A127 +13)
#define IDS_A127_CLMOVER    (IDS_A127 +14)
#define IDS_A127_VATADDR1   (IDS_A127 +15)
#define IDS_A127_VATADDR2   (IDS_A127 +16)
#define IDS_A127_VATADDR3   (IDS_A127 +17)
#define IDS_A127_VATADDR4   (IDS_A127 +18)
#define IDS_A127_VATADDR5   (IDS_A127 +19)
#define IDS_A127_VATMODE    (IDS_A127 +20)

/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A127_NOADDRUS       3   /* number of address us mode                */
#define A127_NOADDRUS2      4   /* number of address VAT mode               */
#define A127_NOMODE         2   /* number of mode                           */
#define A127_COLUMN         7   /* data column(including of decimal point)  */
#define A127_COLUMN5        5   /* data column for address 5                */
#define A127_DECCOLUMN      4   /* data column under decimal point          */
#define A127_DECPNT        46   /* ASCII code of decimal point"."           */
#define A127_CNVLONG    10000L  /* for convert to long type                 */
#define A127_CNVDEC        10L  /* for convert by decimal                   */
#define A127_DATAMAX   999999L  /* data maximum by long type                */
#define A127_DATAMAX5   65535L  /* data maximum by long type for address 5  */
#define A127_DATAMIN        0L  /* data minimum by long type                */
#define A127_NO5_ADR        4   /* offset for address 5                     */
#define A127_ERR_LEN      128   /* length of error message buffer           */

#define TAX_USA             0   /* U.S. tax mode                            */
#define TAX_CANADA          1   /* canadian tax mode                        */
#define TAX_VAT             2   /* International VAT mode                   */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _A127DATA {
    DWORD   dwInt;
    DWORD   dwDec;
} A127DATA, FAR *LPA127DATA;

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
VOID    A127InitDlg(HWND, LPBYTE);
VOID    A127SetData(HWND, LPDWORD, BYTE);
VOID    A127ChkRng(HWND, WORD wEditID);
BOOL    A127GetData(HWND, WPARAM, LPA127DATA);
BOOL    A127ConvData(HWND, WPARAM, LPWSTR, LPA127DATA);
VOID    A127FinDlg(HWND, BYTE);

/* ===== End of File (A127.H) ===== */

