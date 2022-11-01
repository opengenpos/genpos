/*
* ---------------------------------------------------------------------------
* Title         :   Trailer Print for Soft Check Header File (AC 87)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A087.H
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
// WARNING:  The following defines used for labeling the various controls on
//           the A087 Dialog box need to be in consecutive order as the code
//           in the A087 dialog steps through the various controls for some functions.
#define IDD_A087_START   18701
#define IDD_A087_01      IDD_A087_START          /* EditText for Line 1  */
#define IDD_A087_02      (IDD_A087_START+1)      /* EditText for Line 2  */
#define IDD_A087_03      (IDD_A087_START+2)      /* EditText for Line 3  */
#define IDD_A087_04      (IDD_A087_START+3)      /* EditText for Line 4  */
#define IDD_A087_05      (IDD_A087_START+4)      /* EditText for Line 5  */
#define IDD_A087_06      (IDD_A087_START+5)      /* EditText for Line 6  */
#define IDD_A087_07      (IDD_A087_START+6)      /* EditText for Line 7  */
#define IDD_A087_08      (IDD_A087_START+7)      /* EditText for Line 8  */
#define IDD_A087_09      (IDD_A087_START+8)      /* EditText for Line 9  */
#define IDD_A087_10      (IDD_A087_START+9)      /* EditText for Line 10 */
#define IDD_A087_11      (IDD_A087_START+10)      /* EditText for Line 11 */
#define IDD_A087_12      (IDD_A087_START+11)      /* EditText for Line 12 */
#define IDD_A087_13      (IDD_A087_START+12)      /* EditText for Line 13 */
#define IDD_A087_14      (IDD_A087_START+13)      /* EditText for Line 14 */
#define IDD_A087_15      (IDD_A087_START+14)      /* EditText for Line 15 */
#define IDD_A087_16      (IDD_A087_START+15)      /* EditText for Line 16 */
#define IDD_A087_17      (IDD_A087_START+16)      /* EditText for Line 17 */
#define IDD_A087_18      (IDD_A087_START+17)      /* EditText for Line 18 */
#define IDD_A087_19      (IDD_A087_START+18)      /* EditText for Line 19 */
#define IDD_A087_20      (IDD_A087_START+19)      /* EditText for Line 20 */

#define IDD_A087_DESC01     (IDD_A087_20+1)
#define IDD_A087_DESC02     (IDD_A087_DESC01 + 1)
#define IDD_A087_DESC03     (IDD_A087_DESC01 + 2)
#define IDD_A087_DESC04     (IDD_A087_DESC01 + 3)
#define IDD_A087_DESC05     (IDD_A087_DESC01 + 4)
#define IDD_A087_DESC06     (IDD_A087_DESC01 + 5)
#define IDD_A087_DESC07     (IDD_A087_DESC01 + 6)
#define IDD_A087_DESC08     (IDD_A087_DESC01 + 7)
#define IDD_A087_DESC09     (IDD_A087_DESC01 + 8)
#define IDD_A087_DESC10     (IDD_A087_DESC01 + 9)
#define IDD_A087_DESC11     (IDD_A087_DESC01 + 10)
#define IDD_A087_DESC12     (IDD_A087_DESC01 + 11)
#define IDD_A087_DESC13     (IDD_A087_DESC01 + 12)
#define IDD_A087_DESC14     (IDD_A087_DESC01 + 13)
#define IDD_A087_DESC15     (IDD_A087_DESC01 + 14)
#define IDD_A087_DESC16     (IDD_A087_DESC01 + 15)
#define IDD_A087_DESC17     (IDD_A087_DESC01 + 16)
#define IDD_A087_DESC18     (IDD_A087_DESC01 + 17)
#define IDD_A087_DESC19     (IDD_A087_DESC01 + 18)
#define IDD_A087_DESC20     (IDD_A087_DESC01 + 19)
#define IDD_A087_LEN        (IDD_A087_DESC01 + 20)   // string describing length, etc of the text to be entered

#define IDD_A087_EPTLOGO	(IDD_A087_LEN+1)
#define IDD_A087_RTNLOGO	(IDD_A087_LEN+2)
#define IDD_A087_UNULOGO	(IDD_A087_LEN+3)
/*
* ===========================================================================
*       String ID 
* ===========================================================================
*/
#define IDS_A087_CAPTION        (IDS_A87)
#define IDS_A087_DESC01         (IDS_A87 +  1)
#define IDS_A087_DESC02         (IDS_A87 +  2)
#define IDS_A087_DESC03         (IDS_A87 +  3)
#define IDS_A087_DESC04         (IDS_A87 +  4)
#define IDS_A087_DESC05         (IDS_A87 +  5)
#define IDS_A087_DESC06         (IDS_A87 +  6)
#define IDS_A087_DESC07         (IDS_A87 +  7)
#define IDS_A087_DESC08         (IDS_A87 +  8)
#define IDS_A087_DESC09         (IDS_A87 +  9)
#define IDS_A087_DESC10         (IDS_A87 + 10)
#define IDS_A087_DESC11         (IDS_A87 + 11)
#define IDS_A087_DESC12         (IDS_A87 + 12)
#define IDS_A087_DESC13         (IDS_A87 + 13)
#define IDS_A087_DESC14         (IDS_A87 + 14)
#define IDS_A087_DESC15         (IDS_A87 + 15)
#define IDS_A087_DESC16         (IDS_A87 + 16)
#define IDS_A087_DESC17         (IDS_A87 + 17)
#define IDS_A087_DESC18         (IDS_A87 + 18)
#define IDS_A087_DESC19         (IDS_A87 + 19)
#define IDS_A087_DESC20         (IDS_A87 + 20)
#define IDS_A087_LEN            (IDS_A87 + 21)   // string describing length, etc of the text to be entered

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A87_ADDR_MAX    MAX_SOFTCHK_NO      /* Max. Number of Address   */
#define A87_MNE_LEN     PARA_SOFTCHK_LEN    /* Max. Length of Mnemonic  */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    achMnemo[A87_MNE_LEN];
} A87MNEMO, FAR *LPA87MNEMO;

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
BOOL    A087AllocMem(HWND, LPHGLOBAL);
VOID    A087InitDlg(HWND);
VOID    A087SaveData(HWND);

/* ===== End of File (A087.H) ===== */
