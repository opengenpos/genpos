/*
* ---------------------------------------------------------------------------
* Title         :   Set Sales Promotion Message Header File (AC 88)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A088.C
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
#define IDD_A088_01  18801      /* EditText for Addr.1 (Prom. L-1)  */
#define IDD_A088_02  18802      /* EditText for Addr.2 (Prom. L-2)  */
#define IDD_A088_03  18803      /* EditText for Addr.3 (Prom. L-3)  */

#define IDD_A088_LINE1	18804
#define IDD_A088_LINE2	18805
#define IDD_A088_LINE3	18806
#define IDD_A088_CAPTION1	18807
#define IDD_A088_CAPTION2	18808

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A88_ADDR_MAX    MAX_PROMO_NO        /* Max. Number of Address   */
#define A88_MNE_LEN     PARA_PROMOTION_LEN  /* Max. Length of Mnemonic  */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    achMnemo[A88_MNE_LEN];
} A88MNEMO, FAR *LPA88MNEMO;

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
BOOL    A088AllocMem(HWND, LPHGLOBAL);
VOID    A088InitDlg(HWND);
VOID    A088SaveData(HWND);

/* ===== End of File (A088.H) ===== */
