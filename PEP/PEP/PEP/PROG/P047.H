/*
* ---------------------------------------------------------------------------
* Title         :   Print Modifier Mnemonics Header File (Prog. 47)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P047.H
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
#define IDD_P47_01  24701           /* EditText for Address 1   */
#define IDD_P47_02  24702           /* EditText for Address 2   */
#define IDD_P47_03  24703           /* EditText for Address 3   */
#define IDD_P47_04  24704           /* EditText for Address 4   */
#define IDD_P47_05  24705           /* EditText for Address 5   */
#define IDD_P47_06  24706           /* EditText for Address 6   */
#define IDD_P47_07  24707           /* EditText for Address 7   */
#define IDD_P47_08  24708           /* EditText for Address 8   */

#define IDD_P47_TITLE	24709
#define IDD_P47_PRINT1	24710
#define IDD_P47_PRINT2	24711
#define IDD_P47_PRINT3	24712
#define IDD_P47_PRINT4	24713
#define IDD_P47_PRINT5	24714
#define IDD_P47_PRINT6	24715
#define IDD_P47_PRINT7	24716
#define IDD_P47_PRINT8	24717
#define IDD_P47_CAPTION1	24718
#define IDD_P47_CAPTION2	24719

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P47_ADDR_MAX    MAX_PRTMODI_NO  /* Maximum Number of Address    */
#define P47_MNE_LEN     10              /* Maximum Length of Mnemonic   */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P47_MNE_LEN];       /* Mnemonic Data */
} P47MNEMO, FAR *LPP47MNEMO;

typedef struct  _Work {
            WCHAR   szWork[P47_MNE_LEN + 1];   /* Mnemonic Work Area */
} P47WORK, FAR *LPP47WORK;

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
VOID    P47InitDlg(HWND);
VOID    P47SaveData(HWND);
VOID    P47DispAllocErr(HWND, HGLOBAL, HGLOBAL);

/* ===== End of File (P47.H) ===== */
