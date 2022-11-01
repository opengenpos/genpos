/*
* ---------------------------------------------------------------------------
* Title         :   Special Mnemonics Header File (Prog. 23)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P023.H
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
* Jan-24-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
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
#define IDD_P23_SCBAR   22301

#define IDD_P23_DESC01  (IDD_P23_SCBAR +  1)
#define IDD_P23_DESC02  (IDD_P23_SCBAR +  2)
#define IDD_P23_DESC03  (IDD_P23_SCBAR +  3)
#define IDD_P23_DESC04  (IDD_P23_SCBAR +  4)
#define IDD_P23_DESC05  (IDD_P23_SCBAR +  5)
#define IDD_P23_DESC06  (IDD_P23_SCBAR +  6)
#define IDD_P23_DESC07  (IDD_P23_SCBAR +  7)
#define IDD_P23_DESC08  (IDD_P23_SCBAR +  8)
#define IDD_P23_DESC09  (IDD_P23_SCBAR +  9)
#define IDD_P23_DESC10  (IDD_P23_SCBAR + 10)

#define IDD_P23_EDIT01  (IDD_P23_SCBAR + 11)
#define IDD_P23_EDIT02  (IDD_P23_SCBAR + 12)
#define IDD_P23_EDIT03  (IDD_P23_SCBAR + 13)
#define IDD_P23_EDIT04  (IDD_P23_SCBAR + 14)
#define IDD_P23_EDIT05  (IDD_P23_SCBAR + 15)
#define IDD_P23_EDIT06  (IDD_P23_SCBAR + 16)
#define IDD_P23_EDIT07  (IDD_P23_SCBAR + 17)
#define IDD_P23_EDIT08  (IDD_P23_SCBAR + 18)
#define IDD_P23_EDIT09  (IDD_P23_SCBAR + 19)
#define IDD_P23_EDIT10  (IDD_P23_SCBAR + 20)

#define IDD_P23_ADDR	22330
#define IDD_P23_DESC	22331
#define IDD_P23_MNEM	22332
#define IDD_P23_CAPTION	22333

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P023DESC001 IDS_P23                 /* Description Resouce ID   */
#define IDS_P023DESC002 (IDS_P023DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P023DESC003 (IDS_P023DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P023DESC004 (IDS_P023DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P023DESC005 (IDS_P023DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P023DESC006 (IDS_P023DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P023DESC007 (IDS_P023DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P023DESC008 (IDS_P023DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P023DESC009 (IDS_P023DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P023DESC010 (IDS_P023DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P023DESC011 (IDS_P023DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P023DESC012 (IDS_P023DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P023DESC013 (IDS_P023DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P023DESC014 (IDS_P023DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P023DESC015 (IDS_P023DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P023DESC016 (IDS_P023DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P023DESC017 (IDS_P023DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P023DESC018 (IDS_P023DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P023DESC019 (IDS_P023DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P023DESC020 (IDS_P023DESC001 + 19)  /* Description Resouce ID   */
#define IDS_P023DESC021 (IDS_P023DESC001 + 20)  /* Description Resouce ID   */
#define IDS_P023DESC022 (IDS_P023DESC001 + 21)  /* Description Resouce ID   */
#define IDS_P023DESC023 (IDS_P023DESC001 + 22)  /* Description Resouce ID   */
#define IDS_P023DESC024 (IDS_P023DESC001 + 23)  /* Description Resouce ID   */
#define IDS_P023DESC025 (IDS_P023DESC001 + 24)  /* Description Resouce ID   */
#define IDS_P023DESC026 (IDS_P023DESC001 + 25)  /* Description Resouce ID   */
#define IDS_P023DESC027 (IDS_P023DESC001 + 26)  /* Description Resouce ID   */
#define IDS_P023DESC028 (IDS_P023DESC001 + 27)  /* Description Resouce ID   */
#define IDS_P023DESC029 (IDS_P023DESC001 + 28)  /* Description Resouce ID   */
#define IDS_P023DESC030 (IDS_P023DESC001 + 29)  /* Description Resouce ID   */
#define IDS_P023DESC031 (IDS_P023DESC001 + 30)  /* Description Resouce ID   */
#define IDS_P023DESC032 (IDS_P023DESC001 + 31)  /* Description Resouce ID   */

/*** NCR2172 ***/

#define IDS_P023DESC033 (IDS_P023DESC001 + 32)  /* Description Resouce ID   */
#define IDS_P023DESC034 (IDS_P023DESC001 + 33)  /* Description Resouce ID   */
#define IDS_P023DESC035 (IDS_P023DESC001 + 34)  /* Description Resouce ID   */
#define IDS_P023DESC036 (IDS_P023DESC001 + 35)  /* Description Resouce ID   */
#define IDS_P023DESC037 (IDS_P023DESC001 + 36)  /* Description Resouce ID   */
#define IDS_P023DESC038 (IDS_P023DESC001 + 37)  /* Description Resouce ID   */
#define IDS_P023DESC039 (IDS_P023DESC001 + 38)  /* Description Resouce ID   */
#define IDS_P023DESC040 (IDS_P023DESC001 + 39)  /* Description Resouce ID   */
#define IDS_P023DESC041 (IDS_P023DESC001 + 40)  /* Description Resouce ID   */
#define IDS_P023DESC042 (IDS_P023DESC001 + 41)  /* Description Resouce ID   */
#define IDS_P023DESC043 (IDS_P023DESC001 + 42)  /* Description Resouce ID   */
#define IDS_P023DESC044 (IDS_P023DESC001 + 43)  /* Description Resouce ID   */
#define IDS_P023DESC045 (IDS_P023DESC001 + 44)  /* Description Resouce ID   */
#define IDS_P023DESC046 (IDS_P023DESC001 + 45)  /* Description Resouce ID   */
#define IDS_P023DESC047 (IDS_P023DESC001 + 46)  /* Description Resouce ID   */
#define IDS_P023DESC048 (IDS_P023DESC001 + 47)  /* Description Resouce ID   */
#define IDS_P023DESC049 (IDS_P023DESC001 + 48)  /* Description Resouce ID   */
#define IDS_P023DESC050 (IDS_P023DESC001 + 49)  /* Description Resouce ID   */
#define IDS_P023DESC051 (IDS_P023DESC001 + 50)  /* Description Resouce ID   */
#define IDS_P023DESC052 (IDS_P023DESC001 + 51)  /* Description Resouce ID   */
#define IDS_P023DESC053 (IDS_P023DESC001 + 52)  /* Description Resouce ID   */
#define IDS_P023DESC054 (IDS_P023DESC001 + 53)  /* Description Resouce ID   */
#define IDS_P023DESC055 (IDS_P023DESC001 + 54)  /* Description Resouce ID   */
#define IDS_P023DESC056 (IDS_P023DESC001 + 55)  /* Description Resouce ID   */
#define IDS_P023DESC057 (IDS_P023DESC001 + 56)  /* Description Resouce ID   */
#define IDS_P023DESC058 (IDS_P023DESC001 + 57)  /* Description Resouce ID   */
#define IDS_P023DESC059 (IDS_P023DESC001 + 58)  /* Description Resouce ID   */
#define IDS_P023DESC060 (IDS_P023DESC001 + 59)  /* Description Resouce ID   */
#define IDS_P023DESC061 (IDS_P023DESC001 + 60)  /* Description Resouce ID   */
#define IDS_P023DESC062 (IDS_P023DESC001 + 61)  /* Description Resouce ID   */
#define IDS_P023DESC063 (IDS_P023DESC001 + 62)  /* Description Resouce ID   */
#define IDS_P023DESC064 (IDS_P023DESC001 + 63)  /* Description Resouce ID   */
#define IDS_P023DESC065 (IDS_P023DESC001 + 64)  /* Description Resouce ID   */
#define IDS_P023DESC066 (IDS_P023DESC001 + 65)  /* Description Resouce ID   */
#define IDS_P023DESC067 (IDS_P023DESC001 + 66)  /* Description Resouce ID   */
#define IDS_P023DESC068 (IDS_P023DESC001 + 67)  /* Description Resouce ID   */
#define IDS_P023DESC069 (IDS_P023DESC001 + 68)  /* Description Resouce ID   */
#define IDS_P023DESC070 (IDS_P023DESC001 + 69)  /* Description Resouce ID   */
#define IDS_P023DESC071 (IDS_P023DESC001 + 70)  /* Description Resouce ID   */
#define IDS_P023DESC072 (IDS_P023DESC001 + 71)  /* Description Resouce ID   */
#define IDS_P023DESC073 (IDS_P023DESC001 + 72)  /* Description Resouce ID   */
#define IDS_P023DESC074 (IDS_P023DESC001 + 73)  /* Description Resouce ID   */
#define IDS_P023DESC075 (IDS_P023DESC001 + 74)  /* Description Resouce ID   */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P23_ADDR_MAX    MAX_SPEM_NO         /* Number of Address with       */
                                            /*              Reserved Area   */
#define P23_ADDR_NO     SPC_ADR_MAX         /* Number of Address without    */
                                            /*              Reserved Area   */
/* NCR2172 */
/* #define P23_TOTAL_MAX   31 */                 /* View Number of Address Total V3.3 */
#define P23_TOTAL_MAX   (MAX_SPEM_NO-1)          /* View Number of Address Total NCR2172, table awSelectData does not contain all mnemonic addresses */

#define P23_MNE_LEN     4
#define P23_DESC_LEN    64                  /* Max Length of Description    */

#define P23_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P23_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P23_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P23_ONE_PAGE    9                   /* Scroll to One Page Up/ Down  */
#define P23_PAGE_MAX    10                  /* No. of Items in One Page     */

#define P23_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
/* #define P23_SCBAR_MAX   (P23_ADDR_NO - P23_PAGE_MAX) */
#define P23_SCBAR_MAX   (P23_TOTAL_MAX - P23_PAGE_MAX) /* V3.3 */
/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P23GETINDEX(id, top)   (((id) - IDD_P23_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P23_MNE_LEN];       /* Mnemonic Data */
} P23MNEMO, FAR *LPP23MNEMO;

typedef struct  _Work {
            WCHAR   szWork[P23_MNE_LEN + 1];   /* Mnemonic Work Area */
} P23WORK, FAR *LPP23WORK;

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
VOID    P23InitDlg(HWND);
VOID    P23GetMnemo(HWND, WORD wItem);
VOID    P23DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P23ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P23CalcIndex(WORD wScrlCode);
VOID    P23RedrawText(HWND);

/* ===== End of P023.H ===== */
