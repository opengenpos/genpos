/*
* ---------------------------------------------------------------------------
* Title         :   Report Mnemonics Header File (Prog. 22)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P022.H
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
* Feb-16-95 : 03.00.00 : H.Ishida   : Add Coupon Report
* Jan-24-96 : 03.00.05 : M.Suzuki   : Add Service Time Report R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3 (Remove Server Report)
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
#define IDD_P22_SCBAR   22201

#define IDD_P22_DESC01  (IDD_P22_SCBAR +  1)
#define IDD_P22_DESC02  (IDD_P22_SCBAR +  2)
#define IDD_P22_DESC03  (IDD_P22_SCBAR +  3)
#define IDD_P22_DESC04  (IDD_P22_SCBAR +  4)
#define IDD_P22_DESC05  (IDD_P22_SCBAR +  5)
#define IDD_P22_DESC06  (IDD_P22_SCBAR +  6)
#define IDD_P22_DESC07  (IDD_P22_SCBAR +  7)
#define IDD_P22_DESC08  (IDD_P22_SCBAR +  8)
#define IDD_P22_DESC09  (IDD_P22_SCBAR +  9)
#define IDD_P22_DESC10  (IDD_P22_SCBAR + 10)

#define IDD_P22_EDIT01  (IDD_P22_SCBAR + 11)
#define IDD_P22_EDIT02  (IDD_P22_SCBAR + 12)
#define IDD_P22_EDIT03  (IDD_P22_SCBAR + 13)
#define IDD_P22_EDIT04  (IDD_P22_SCBAR + 14)
#define IDD_P22_EDIT05  (IDD_P22_SCBAR + 15)
#define IDD_P22_EDIT06  (IDD_P22_SCBAR + 16)
#define IDD_P22_EDIT07  (IDD_P22_SCBAR + 17)
#define IDD_P22_EDIT08  (IDD_P22_SCBAR + 18)
#define IDD_P22_EDIT09  (IDD_P22_SCBAR + 19)
#define IDD_P22_EDIT10  (IDD_P22_SCBAR + 20)

#define IDD_P22_ADDR	22230
#define IDD_P22_DESC	22231
#define IDD_P22_MNEM	22232
#define IDD_P22_CAPTION	22233

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P022DESC001 IDS_P22                 /* Description Resouce ID   */
#define IDS_P022DESC002 (IDS_P022DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P022DESC003 (IDS_P022DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P022DESC004 (IDS_P022DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P022DESC005 (IDS_P022DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P022DESC006 (IDS_P022DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P022DESC007 (IDS_P022DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P022DESC008 (IDS_P022DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P022DESC009 (IDS_P022DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P022DESC010 (IDS_P022DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P022DESC011 (IDS_P022DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P022DESC012 (IDS_P022DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P022DESC013 (IDS_P022DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P022DESC014 (IDS_P022DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P022DESC015 (IDS_P022DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P022DESC016 (IDS_P022DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P022DESC017 (IDS_P022DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P022DESC018 (IDS_P022DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P022DESC019 (IDS_P022DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P022DESC020 (IDS_P022DESC001 + 19)  /* Description Resouce ID   */
#define IDS_P022DESC021 (IDS_P022DESC001 + 20)  /* Description Resouce ID   */
#define IDS_P022DESC022 (IDS_P022DESC001 + 21)  /* Description Resouce ID   */
#define IDS_P022DESC023 (IDS_P022DESC001 + 22)  /* Description Resouce ID   */
#define IDS_P022DESC024 (IDS_P022DESC001 + 23)  /* Description Resouce ID   */
#define IDS_P022DESC025 (IDS_P022DESC001 + 24)  /* Description Resouce ID   */
#define IDS_P022DESC026 (IDS_P022DESC001 + 25)  /* Description Resouce ID   */
#define IDS_P022DESC027 (IDS_P022DESC001 + 26)  /* Description Resouce ID   */
#define IDS_P022DESC028 (IDS_P022DESC001 + 27)  /* Description Resouce ID   */
#define IDS_P022DESC029 (IDS_P022DESC001 + 28)  /* Description Resouce ID   */
#define IDS_P022DESC030 (IDS_P022DESC001 + 29)  /* Description Resouce ID   */
#define IDS_P022DESC031 (IDS_P022DESC001 + 30)  /* Description Resouce ID   */
#define IDS_P022DESC032 (IDS_P022DESC001 + 31)  /* Description Resouce ID   */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P22_ADDR_MAX    MAX_REPO_NO         /* Number of Address with       */
                                            /*              Reserved Area   */
#define P22_ADDR_NO     RPT_ADR_MAX         /* Number of Address without    */
                                            /*              Reserved Area   */
#define P22_TOTAL_MAX   (P22_ADDR_MAX-1)    /* View Number of Address Total V3.3, table awSelectData does not contain all mnemonic addresses */

#define P22_MNE_LEN     6                   /* Max Length of Mnemonic       */
#define P22_DESC_LEN    64                  /* Max Length of Description    */

#define P22_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P22_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P22_PAGE_MAX    10                  /* No. of Items in One Page     */
#define P22_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P22_ONE_PAGE    (P22_PAGE_MAX - P22_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P22_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
/* #define P22_SCBAR_MAX   (P22_ADDR_NO - P22_PAGE_MAX) */
#define P22_SCBAR_MAX   (P22_TOTAL_MAX - P22_PAGE_MAX) /* V3.3 */

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P22GETINDEX(id, top)   (((id) - IDD_P22_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P22_MNE_LEN];       /* Mnemonic Data */
} P22MNEMO, FAR *LPP22MNEMO;

typedef struct  _Work {
            WCHAR   szWork[P22_MNE_LEN + 1];   /* Mnemonic Work Area */
} P22WORK, FAR *LPP22WORK;

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
VOID    P22InitDlg(HWND);
VOID    P22GetMnemo(HWND, WORD wItem);
VOID    P22DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P22ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P22CalcIndex(WORD wScrlCode);
VOID    P22RedrawText(HWND);

/* ===== End of File (P022.H) ==== */
