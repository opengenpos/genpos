/*
* ---------------------------------------------------------------------------
* Title         :   Major Department Mnemonics Header File (Prog. 48)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P048.H
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
#define IDD_P48_SCBAR 24800         /* Scrollbar    Saratoga    */

#define IDD_P48_DESC01  (IDD_P48_SCBAR +  1)
#define IDD_P48_DESC02  (IDD_P48_SCBAR +  2)
#define IDD_P48_DESC03  (IDD_P48_SCBAR +  3)
#define IDD_P48_DESC04  (IDD_P48_SCBAR +  4)
#define IDD_P48_DESC05  (IDD_P48_SCBAR +  5)
#define IDD_P48_DESC06  (IDD_P48_SCBAR +  6)
#define IDD_P48_DESC07  (IDD_P48_SCBAR +  7)
#define IDD_P48_DESC08  (IDD_P48_SCBAR +  8)
#define IDD_P48_DESC09  (IDD_P48_SCBAR +  9)
#define IDD_P48_DESC10  (IDD_P48_SCBAR + 10)

#define IDD_P48_01      (IDD_P48_SCBAR + 11)           /* EditText for Address 1   */
#define IDD_P48_02      (IDD_P48_SCBAR + 12)           /* EditText for Address 2   */
#define IDD_P48_03      (IDD_P48_SCBAR + 13)           /* EditText for Address 3   */
#define IDD_P48_04      (IDD_P48_SCBAR + 14)           /* EditText for Address 4   */
#define IDD_P48_05      (IDD_P48_SCBAR + 15)           /* EditText for Address 5   */
#define IDD_P48_06      (IDD_P48_SCBAR + 16)           /* EditText for Address 6   */
#define IDD_P48_07      (IDD_P48_SCBAR + 17)           /* EditText for Address 7   */
#define IDD_P48_08      (IDD_P48_SCBAR + 18)           /* EditText for Address 8   */
#define IDD_P48_09      (IDD_P48_SCBAR + 19)           /* EditText for Address 9   */
#define IDD_P48_10      (IDD_P48_SCBAR + 20)           /* EditText for Address10   */

#define IDD_P48_TITLE	24830
#define IDD_P48_CAPTION1	24831
#define IDD_P48_CAPTION2	24832

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
/*** Saratoga Start ***/

#define IDS_P48_DESC01  IDS_P48
#define IDS_P48_DESC02  (IDS_P48_DESC01 + 1)
#define IDS_P48_DESC03  (IDS_P48_DESC01 + 2)
#define IDS_P48_DESC04  (IDS_P48_DESC01 + 3)
#define IDS_P48_DESC05  (IDS_P48_DESC01 + 4)
#define IDS_P48_DESC06  (IDS_P48_DESC01 + 5)
#define IDS_P48_DESC07  (IDS_P48_DESC01 + 6)
#define IDS_P48_DESC08  (IDS_P48_DESC01 + 7)
#define IDS_P48_DESC09  (IDS_P48_DESC01 + 8)
#define IDS_P48_DESC10  (IDS_P48_DESC01 + 9)
#define IDS_P48_DESC11  (IDS_P48_DESC01 +10)
#define IDS_P48_DESC12  (IDS_P48_DESC01 +11)
#define IDS_P48_DESC13  (IDS_P48_DESC01 +12)
#define IDS_P48_DESC14  (IDS_P48_DESC01 +13)
#define IDS_P48_DESC15  (IDS_P48_DESC01 +14)
#define IDS_P48_DESC16  (IDS_P48_DESC01 +15)
#define IDS_P48_DESC17  (IDS_P48_DESC01 +16)
#define IDS_P48_DESC18  (IDS_P48_DESC01 +17)
#define IDS_P48_DESC19  (IDS_P48_DESC01 +18)
#define IDS_P48_DESC20  (IDS_P48_DESC01 +19)
#define IDS_P48_DESC21  (IDS_P48_DESC01 +20)
#define IDS_P48_DESC22  (IDS_P48_DESC01 +21)
#define IDS_P48_DESC23  (IDS_P48_DESC01 +22)
#define IDS_P48_DESC24  (IDS_P48_DESC01 +23)
#define IDS_P48_DESC25  (IDS_P48_DESC01 +24)
#define IDS_P48_DESC26  (IDS_P48_DESC01 +25)
#define IDS_P48_DESC27  (IDS_P48_DESC01 +26)
#define IDS_P48_DESC28  (IDS_P48_DESC01 +27)
#define IDS_P48_DESC29  (IDS_P48_DESC01 +28)
#define IDS_P48_DESC30  (IDS_P48_DESC01 +29)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P48_ADDR_MAX    MAX_MDEPT_NO    /* Maximum Number of Address    */
//#define P48_MNE_LEN     8               /* Maximum Length of Mnemonic   *#define P48_MNE_LEN     8               /* Maximum Length of Mnemonic   */
#define P48_MNE_LEN     20               /* Maximum Length of Mnemonic   JHHJ Database Changes 3-29-04*/
#define P48_DESC_LEN    64              /* Max Length of Description    */

/*** Saratoga Start ***/
#define P48_SPIN_STEP   1
#define P48_SPIN_TURBO  0

#define P48_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P48_MAX_PAGE    10                  /* No. of Item in One Page      */
#define P48_ONE_PAGE    (P48_MAX_PAGE - P48_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P48_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#define P48_SCBAR_MAX   (P48_ADDR_MAX - P48_MAX_PAGE)
                                            /* Max. Value of Scroll Range   */

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
/*** Saratoga Start ***/
#define P48GETINDEX(id, top)   (BYTE)(((id) - IDD_P48_01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P48_MNE_LEN];       /* Mnemonic Data */
} P48MNEMO, FAR *LPP48MNEMO;

typedef struct  _Work {
            WCHAR    szWork[P48_MNE_LEN + 1];   /* Mnemonic Work Area */
} P48WORK, FAR *LPP48WORK;

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
VOID    P48InitDlg(HWND);
VOID    P48SaveData(HWND);
VOID    P48DispAllocErr(HWND, HGLOBAL, HGLOBAL);

BOOL    P48ScrlProc(HWND hdlg, WPARAM wParam, LPARAM lParam);
BOOL    P48CalcIndex(WORD wScrlCode);
VOID    P48RedrawText(HWND hDlg);
VOID    P48GetMnemo(HWND hDlg, WORD wItem);

/* ===== End of File (P48.H) ===== */
