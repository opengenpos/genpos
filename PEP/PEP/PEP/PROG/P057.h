/*
* ---------------------------------------------------------------------------
* Title         :   Lead-Through Messages Header File (Prog. 21)
* Category      :   Setup, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P021.H
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
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
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-28-94 : 00.00.02 : M.Yamamoto : Add Address No. 85
* Apr-17-95 : 03.00.00 : H.Ishida   : Add Address NO. 86-92
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Jan-24-96 : 03.00.05 : M.Suzuki   : R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
* Nov-19-99 :          : K.Yanagida : NCR2172
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
/**
#define IDD_P57_SCBAR   25701
**/
#define IDD_P57_SCBAR   15701

#define IDD_P57_DESC01  (IDD_P57_SCBAR +  1)
#define IDD_P57_DESC02  (IDD_P57_SCBAR +  2)
#define IDD_P57_DESC03  (IDD_P57_SCBAR +  3)
#define IDD_P57_DESC04  (IDD_P57_SCBAR +  4)
#define IDD_P57_DESC05  (IDD_P57_SCBAR +  5)
#define IDD_P57_DESC06  (IDD_P57_SCBAR +  6)
#define IDD_P57_DESC07  (IDD_P57_SCBAR +  7)
#define IDD_P57_DESC08  (IDD_P57_SCBAR +  8)
#define IDD_P57_DESC09  (IDD_P57_SCBAR +  9)
#define IDD_P57_DESC10  (IDD_P57_SCBAR + 10)

#define IDD_P57_EDIT01  (IDD_P57_SCBAR + 11)
#define IDD_P57_EDIT02  (IDD_P57_SCBAR + 12)
#define IDD_P57_EDIT03  (IDD_P57_SCBAR + 13)
#define IDD_P57_EDIT04  (IDD_P57_SCBAR + 14)
#define IDD_P57_EDIT05  (IDD_P57_SCBAR + 15)
#define IDD_P57_EDIT06  (IDD_P57_SCBAR + 16)
#define IDD_P57_EDIT07  (IDD_P57_SCBAR + 17)
#define IDD_P57_EDIT08  (IDD_P57_SCBAR + 18)
#define IDD_P57_EDIT09  (IDD_P57_SCBAR + 19)
#define IDD_P57_EDIT10  (IDD_P57_SCBAR + 20)

#define IDD_P57_CAPTION	15725
#define IDD_P57_ADDR	15726
#define IDD_P57_DESC	15727
#define IDD_P57_MNEM	15728

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P057DESC001 IDS_P57                 /* Description Resouce ID   */
#define IDS_P057DESC002 (IDS_P057DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P057DESC003 (IDS_P057DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P057DESC004 (IDS_P057DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P057DESC005 (IDS_P057DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P057DESC006 (IDS_P057DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P057DESC007 (IDS_P057DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P057DESC008 (IDS_P057DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P057DESC009 (IDS_P057DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P057DESC010 (IDS_P057DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P057DESC011 (IDS_P057DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P057DESC012 (IDS_P057DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P057DESC013 (IDS_P057DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P057DESC014 (IDS_P057DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P057DESC015 (IDS_P057DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P057DESC016 (IDS_P057DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P057DESC017 (IDS_P057DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P057DESC018 (IDS_P057DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P057DESC019 (IDS_P057DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P057DESC020 (IDS_P057DESC001 + 19)  /* Description Resouce ID   */
#define IDS_P057DESC021 (IDS_P057DESC001 + 20)  /* Description Resouce ID   */
#define IDS_P057DESC022 (IDS_P057DESC001 + 21)  /* Description Resouce ID   */
#define IDS_P057DESC023 (IDS_P057DESC001 + 22)  /* Description Resouce ID   */
#define IDS_P057DESC024 (IDS_P057DESC001 + 23)  /* Description Resouce ID   */
#define IDS_P057DESC025 (IDS_P057DESC001 + 24)  /* Description Resouce ID   */
#define IDS_P057DESC026 (IDS_P057DESC001 + 25)  /* Description Resouce ID   */
#define IDS_P057DESC027 (IDS_P057DESC001 + 26)  /* Description Resouce ID   */
#define IDS_P057DESC028 (IDS_P057DESC001 + 27)  /* Description Resouce ID   */
#define IDS_P057DESC029 (IDS_P057DESC001 + 28)  /* Description Resouce ID   */
#define IDS_P057DESC030 (IDS_P057DESC001 + 29)  /* Description Resouce ID   */
#define IDS_P057DESC031 (IDS_P057DESC001 + 30)  /* Description Resouce ID   */
#define IDS_P057DESC032 (IDS_P057DESC001 + 31)  /* Description Resouce ID   */
#define IDS_P057DESC033 (IDS_P057DESC001 + 32)  /* Description Resouce ID   */
#define IDS_P057DESC034 (IDS_P057DESC001 + 33)  /* Description Resouce ID   */
#define IDS_P057DESC035 (IDS_P057DESC001 + 34)  /* Description Resouce ID   */
#define IDS_P057DESC036 (IDS_P057DESC001 + 35)  /* Description Resouce ID   */
#define IDS_P057DESC037 (IDS_P057DESC001 + 36)  /* Description Resouce ID   */
#define IDS_P057DESC038 (IDS_P057DESC001 + 37)  /* Description Resouce ID   */
#define IDS_P057DESC039 (IDS_P057DESC001 + 38)  /* Description Resouce ID   */
#define IDS_P057DESC040 (IDS_P057DESC001 + 39)  /* Description Resouce ID   */
/*** End   NCR2172 ***/

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P57_ADDR_MAX    MAX_LEAD_NO         /* Number of Address with       */
                                            /*              Reserved Area   */
#define P57_ADDR_NO     LDT_ADR_MAX         /* Number of Address without    */

/*** NCR2172***/                            /*              Reserved Area   */
#define P57_TOTAL_MAX   MAX_CH24_NO                  /* View Number of Address Total */

#define P57_MNE_LEN     24                  /* Max Length of Mnemonic       */
#define P57_DESC_LEN    40                  /* Max Length of Description    */
/*#define P57_DESC_LEN    82                   Max Length of Description    */

#define P57_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P57_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P57_PAGE_MAX    10                  /* No. of Items in One Page     */
#define P57_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P57_ONE_PAGE    (P57_PAGE_MAX - P57_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P57_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */

#define P57_SCBAR_MAX   (P57_TOTAL_MAX - P57_PAGE_MAX)

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P57GETINDEX(id, top)   (((id) - IDD_P57_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P57_MNE_LEN];       /* Mnemonic Data */
} P57MNEMO, FAR *LPP57MNEMO;

typedef struct  _Work {
            WCHAR    szWork[P57_MNE_LEN + 1];   /* Mnemonic Work Area */
} P57WORK, FAR *LPP57WORK;

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
VOID    P57InitDlg(HWND);
VOID    P57GetMnemo(HWND, WORD wItem);
VOID    P57DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P57ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P57CalcIndex(WORD wScrlCode);
VOID    P57RedrawText(HWND);

/* ===== End of File (P057.H) ===== */
